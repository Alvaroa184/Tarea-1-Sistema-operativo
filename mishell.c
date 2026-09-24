#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h> 

#include "pipes.h"
#include "jobs.h"
#include "pmon.h"
#include "signals.h"

#define MAX_LINE 1024
#define MAX_ARGS 64

int main() {
    char line[MAX_LINE];
    char cwd[1024];

    configurar_sigchld();
    configurar_senales_shell();

    while (1) {

        revisar_hijos();

        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("miShell:%s$ ", cwd);
        } else {
            perror("getcwd");
        }

        fflush(stdout);

        if (fgets(line, MAX_LINE, stdin) == NULL) {
            printf("\n");
            break;
        }

        if (strcmp(line, "exit\n") == 0) {
            break;
        }

        int background = 0;
        size_t len = strlen(line);

        while (len > 0 &&
              (line[len - 1] == '\n' ||
               line[len - 1] == ' ' ||
               line[len - 1] == '\t')) {

            line[--len] = '\0';
        }

        if (len == 0) {
            continue;
        }

        if (line[len - 1] == '&') {
            background = 1;
            line[--len] = '\0';

            while (len > 0 &&
                  (line[len - 1] == ' ' ||
                   line[len - 1] == '\t')) {

                line[--len] = '\0';
            }
        }

        char comando_original[MAX_LINE];
        strcpy(comando_original, line);

        int tiene_pipe = strchr(line, '|') != NULL;

        if (tiene_pipe) {
            ejecutar_pipeline(line, background, comando_original);
            continue;
        }

        char *args[MAX_ARGS];
        int i = 0;

        args[i] = strtok(line, " \t\n");

        while (args[i] != NULL && i < MAX_ARGS - 1) {
            i++;
            args[i] = strtok(NULL, " \t\n");
        }

        if (args[0] == NULL) {
            continue;
        }

        // R2
        if (strcmp(args[0], "exit") == 0) {
            int status = 0;
            if (args[1] != NULL) {
                status = atoi(args[1]);
            }
            exit(status);
        }

        if (strcmp(args[0], "cd") == 0) {
            char *dir = args[1];
            if (dir == NULL) {
                dir = getenv("HOME");
            }
            if (chdir(dir) != 0) {
                perror("cd");
            }
            continue; 
        }

        if (strcmp(args[0], "jobs") == 0) {
            imprimir_jobs(); 
            continue;
        }

        if (strcmp(args[0], "pmon") == 0) {
            int segundos = 2;
            if (args[1] != NULL) {
                segundos = atoi(args[1]);
            }
            ejecutar_pmon(segundos); 
            continue;
        }
        // FIN R2

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            if (!background){
                restaurar_senales_foreground();
            }
            // R3
            char *exec_args[MAX_ARGS];
            int k = 0;
            
            for (int j = 0; args[j] != NULL; j++) {
                if (strcmp(args[j], "<") == 0 && args[j+1] != NULL) {
                    int fd = open(args[j+1], O_RDONLY);
                    if (fd < 0) { perror("open <"); exit(EXIT_FAILURE); }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                    j++; 
                } 
                else if (strcmp(args[j], ">") == 0 && args[j+1] != NULL) {
                    int fd = open(args[j+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) { perror("open >"); exit(EXIT_FAILURE); }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    j++; 
                } 
                else if (strcmp(args[j], ">>") == 0 && args[j+1] != NULL) {
                    int fd = open(args[j+1], O_WRONLY | O_CREAT | O_APPEND, 0644);
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

            execvp(exec_args[0], exec_args);
            perror(exec_args[0]);
            exit(EXIT_FAILURE);
            // FIN R3
        }

        if (background) {
            pid_t pids[1];
            pids[0] = pid;

            agregar_job(pids, 1, comando_original);
        } else {
            waitpid(pid, NULL, 0);
        }
    }

    return 0;
}
