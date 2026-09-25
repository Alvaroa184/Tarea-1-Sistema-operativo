#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "pipes.h"
#include "jobs.h"
#include "signals.h"

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
            if(!background){
                restaurar_senales_foreground();
            }
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

            char *exec_args[MAX_ARGS];
            int k = 0;

            for (int j = 0; args[j] != NULL; j++) {
                if (strcmp(args[j], "<") == 0 && args[j + 1] != NULL) {
                    int fd = open(args[j + 1], O_RDONLY);
                    if (fd < 0) { perror("open <"); exit(EXIT_FAILURE); }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                    j++;
                }
                else if (strcmp(args[j], ">") == 0 && args[j + 1] != NULL) {
                    int fd = open(args[j + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) { perror("open >"); exit(EXIT_FAILURE); }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    j++;
                }
                else if (strcmp(args[j], ">>") == 0 && args[j + 1] != NULL) {
                    int fd = open(args[j + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                    if (fd < 0) { perror("open >>"); exit(EXIT_FAILURE); }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    j++;
                }
                else {
                    exec_args[k++] = args[j];
                }
            }

            exec_args[k] = NULL;

            if (exec_args[0] != NULL) {
                execvp(exec_args[0], exec_args);
                perror(exec_args[0]);
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