#include <signal.h>
#include <stddef.h>

#include "signals.h"

//Preparar para que ignore CTRL+C Y CTRL+\//
void configurar_senales_shell(){
    struct sigaction sa_ign;
    sa_ign.sa_handler = SIG_IGN;
    sigemptyset(&sa_ign.sa_mask);

    sigaction(SIGINT, &sa_ign, NULL);
    sigaction(SIGQUIT, &sa_ign, NULL);
}

//Restaurar las senales por defecto//
void restaurar_senales_foreground(){
    struct sigaction sa_dfl;
    sa_dfl.sa_handler = SIG_DFL;
    sigemptyset(&sa_dfl.sa_mask);
    sa_dfl.sa_flags = 0;

    sigaction(SIGINT, &sa_dfl, NULL);
    sigaction(SIGQUIT, &sa_dfl, NULL);
}