#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

#define MAX_JOBS 20
#define MAX_CMDS 20
#define MAX_LINE 1024

typedef struct {
    int id;
    pid_t pids[MAX_CMDS];
    int cantidad;
    int restantes;
    char comando[MAX_LINE];
    int activo;
} Job;

void configurar_sigchld();
void agregar_job(pid_t pids[], int cantidad, char *comando);
void revisar_hijos();

#endif