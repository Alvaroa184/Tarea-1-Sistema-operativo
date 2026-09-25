#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#include "pmon.h"
#include "jobs.h"

//Recuperamos MAX_JOBS//
extern Job jobs[MAX_JOBS];

volatile sig_atomic_t pmon_actualizar = 0;
volatile sig_atomic_t pmon_terminar = 0;

void manejador_alarma_pmon(int sig){
    (void)sig;
    pmon_actualizar = 1;
}

void manejador_sigint_pmon(int sig){
    (void)sig;
    pmon_terminar = 1;
}

void ejecutar_pmon(int segundos){
    //Por defecto si no recibimos segundos//
    if (segundos <= 0){
        segundos = 2;
    }

    struct sigaction sa_alarma, old_alarma;
    sa_alarma.sa_handler = manejador_alarma_pmon;
    sigemptyset(&sa_alarma.sa_mask);
    sa_alarma.sa_flags = 0;
    sigaction(SIGALRM, &sa_alarma, &old_alarma);

    struct sigaction sa_sigint, old_sigint;
    sa_sigint.sa_handler = manejador_sigint_pmon;
    sigemptyset(&sa_sigint.sa_mask);
    sa_sigint.sa_flags = 0;
    sigaction(SIGINT, &sa_sigint, &old_sigint);

    pmon_terminar = 0;
    
    //Obtenemos la velocidad del procesador//
    long hz = sysconf(_SC_CLK_TCK);

    //Un arreglo para guardar el tiempo anterior de cada job//
    long ticks_anteriores[MAX_JOBS] = {0};
    while (pmon_terminar == 0){
    pmon_actualizar = 0;

    revisar_hijos();

    printf("\n\n\n--- ACTUALIZACIÓN DE PROCESOS ---\n");
        printf("PID\t| COMANDO\t| ESTADO\t| %%CPU(aprox)\t| RSS(KB)\n");

        for (int i = 0; i < MAX_JOBS; i++){
            if (jobs[i].activo == 1){
                
                int pid = jobs[i].pids[0];
                if (pid == 0) continue;

                char ruta_estadisticas[100];
                sprintf(ruta_estadisticas, "/proc/%d/stat", pid);
                
                FILE *archivo_estadisticas = fopen(ruta_estadisticas, "r");
                //Si "murio" el proceso continuamos//
                if (archivo_estadisticas == NULL) continue; 

                char estado;
                long utime = 0;
                long stime = 0;
                
                //Las posiciones 14 y 15 son utime y stime//
                //Con el * ignoramos los que no nos interesan pero avanzamos para llegar a los que si//
                fscanf(archivo_estadisticas, "%*d %*s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %ld %ld", 
                    &estado, &utime, &stime);
                fclose(archivo_estadisticas);

                char ruta_status[100];
                sprintf(ruta_status, "/proc/%d/status", pid);
                
                FILE *archivo_status = fopen(ruta_status, "r");
                long rss = 0;
                char linea[256];
                
                if (archivo_status != NULL){
                    //Revisamos cada linea hasta encontrar "VmRSS"//
                    while (fgets(linea, sizeof(linea), archivo_status)){
                        if (strncmp(linea, "VmRSS:", 6) == 0){
                            sscanf(linea, "VmRSS: %ld kB", &rss);
                            break;
                        }
                    }
                    fclose(archivo_status);
                }

                long ticks_totales = utime + stime;
                long diferencia = ticks_totales - ticks_anteriores[i];
                double uso_cpu = 0.0;
                
                if (ticks_anteriores[i] != 0){
                    uso_cpu = ((double)diferencia*100.0/hz)/segundos;
                }
                ticks_anteriores[i] = ticks_totales;

                printf("%d\t| %s\t| %c\t\t| %.1f\t| %ld\n", pid, jobs[i].comando, estado, uso_cpu, rss);
            }
        }
        //Para vaciar el buffer//
        fflush(stdout);

        //Cada cuando refrescamos//
        alarm(segundos);

        while (pmon_actualizar == 0 && pmon_terminar == 0) {
            pause();
        }
    }

    //Eliminamos la alarma//
    alarm(0);
    sigaction(SIGALRM, &old_alarma, NULL);
    sigaction(SIGINT, &old_sigint, NULL); 
}