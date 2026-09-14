#ifndef PIPES_H
#define PIPES_H

#define MAX_CMDS 20
#define MAX_ARGS 64

void ejecutar_pipeline(char *line, int background, char *comando_original);

#endif