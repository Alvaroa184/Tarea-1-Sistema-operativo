#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "jobs.h"

Job jobs[MAX_JOBS];
int siguiente_job = 1;

volatile sig_atomic_t termino_hijo = 0;

void manejar_sigchld(int sig) {
    (void)sig;
    termino_hijo = 1;
}

void configurar_sigchld() {
    struct sigaction sa;

    sa.sa_handler = manejar_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGCHLD, &sa, NULL);
}

void agregar_job(pid_t pids[], int cantidad, char *comando) {
    for (int i = 0; i < MAX_JOBS; i++) {

        if (!jobs[i].activo) {
            jobs[i].id = siguiente_job++;
            jobs[i].cantidad = cantidad;
            jobs[i].restantes = cantidad;
            jobs[i].activo = 1;

            strcpy(jobs[i].comando, comando);

            for (int j = 0; j < cantidad; j++) {
                jobs[i].pids[j] = pids[j];
            }

            printf("[%d] %d\n", jobs[i].id, pids[0]);
            return;
        }
    }
}

void revisar_hijos() {
    if (!termino_hijo)
        return;

    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {

        for (int i = 0; i < MAX_JOBS; i++) {

            if (!jobs[i].activo)
                continue;

            for (int j = 0; j < jobs[i].cantidad; j++) {

                if (jobs[i].pids[j] == pid) {
                    jobs[i].pids[j] = 0;
                    jobs[i].restantes--;
                    break;
                }
            }
        }
    }

    termino_hijo = 0;

    for (int i = 0; i < MAX_JOBS; i++) {

        if (jobs[i].activo && jobs[i].restantes == 0) {
        printf("\r\n[%d] Done %s\n",
           jobs[i].id,
           jobs[i].comando);

        jobs[i].activo = 0;
}
    }
}

void imprimir_jobs() {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].activo) {
            printf("[%d] Ejecutando\t%s\n", jobs[i].id, jobs[i].comando);
        }
    }
}
