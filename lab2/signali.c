/* Operacije nad signalima */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "signali.h"
#include "datoteke.h"

/* implementacija funkcija, vec opisanih u lab1 + postavi_signale() */
void postavi_signale() {
    signal(SIGUSR1, obradi_dogadjaj);
    signal(SIGTERM, obradi_sigterm);
    signal(SIGINT, obradi_sigint);
}

void obradi_dogadjaj(int sig) {
    printf("Primio signal SIGUSR1, vraćam broj koji koristim u obradi\n");
    printf ("%d\n", broj);
    sleep(5);
}

void obradi_sigterm(int sig) {
    printf("Primio signal SIGTERM, pišem u datoteku status zadnji broj i prekidam rad\n");

    FILE *dat;
    
    dat = fopen ("status.txt", "w");
    fprintf(dat, "%d", broj);
    fclose(dat); 
       
    nije_kraj = 0;
}

void obradi_sigint(int sig) {
    printf("Primio signal SIGINT, prekidam rad\n");
    exit(0);
}