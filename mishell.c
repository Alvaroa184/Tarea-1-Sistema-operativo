#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "pipes.h"
#include "jobs.h"

#define MAX_LINE 1024
#define MAX_ARGS 64

int main() {
    char line[MAX_LINE];
    char cwd[1024];

    configurar_sigchld();

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

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            execvp(args[0], args);
            perror(args[0]);
            exit(EXIT_FAILURE);
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