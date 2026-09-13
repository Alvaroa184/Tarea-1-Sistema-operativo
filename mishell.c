#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

int main() {
    char line[MAX_LINE];
    char *args[MAX_ARGS];
    char cwd[1024];

    while (1) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("miShell:%s$ ", cwd);
        } else {
            perror("Error al obtener directorio");
        }
        fflush(stdout);

        if (fgets(line, MAX_LINE, stdin) == NULL) {
            printf("\n");
            break;
        }

        int i = 0;
        args[i] = strtok(line, " \t\n");
        
        while (args[i] != NULL && i < MAX_ARGS - 1) {
            i++;
            args[i] = strtok(NULL, " \t\n");
        }
        
        if (args[0] == NULL) {
            continue;
        }

        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("Error fatal de fork");
            exit(EXIT_FAILURE);
        } 
        else if (pid == 0) {
            if (execvp(args[0], args) < 0) {
                perror("Comando no encontrado");
                exit(EXIT_FAILURE);
            }
        } 
        else {
            waitpid(pid, NULL, 0);
        }
    }

    return 0;
}
