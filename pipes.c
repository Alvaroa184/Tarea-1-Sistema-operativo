#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "pipes.h"
#include "jobs.h"

void ejecutar_pipeline(char *line, int background, char *comando_original) {
    char *comandos[MAX_CMDS];
    int cantidad = 0;

    char *parte = strtok(line, "|");

    while (parte != NULL && cantidad < MAX_CMDS) {
        comandos[cantidad++] = parte;
        parte = strtok(NULL, "|");
    }

    int pipes[MAX_CMDS - 1][2];
    pid_t pids[MAX_CMDS];

    for (int i = 0; i < cantidad - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe");
            return;
        }
    }

    for (int i = 0; i < cantidad; i++) {

        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork");
            return;
        }

        if (pids[i] == 0) {

            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            if (i < cantidad - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < cantidad - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            char *args[MAX_ARGS];
            int j = 0;

            args[j] = strtok(comandos[i], " \t\n");

            while (args[j] != NULL && j < MAX_ARGS - 1) {
                j++;
                args[j] = strtok(NULL, " \t\n");
            }

            if (args[0] != NULL) {
                execvp(args[0], args);
                perror(args[0]);
            }

            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < cantidad - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    if (background) {
        agregar_job(pids, cantidad, comando_original);
    } else {
        for (int i = 0; i < cantidad; i++) {
            waitpid(pids[i], NULL, 0);
        }
    }
}