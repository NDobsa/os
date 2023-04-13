#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>


void obradi_dogadjaj(int sig);
void obradi_sigterm(int sig);
void obradi_sigint(int sig);

int nije_kraj = 1;
int broj, kvadrat;

int main() {
    
    struct sigaction act;
    /* 1. maskiranje signala SIGUSR1 */
    act.sa_handler = obradi_dogadjaj; /* kojom se funkcijom signal obrađuje */
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGTERM); /* blokirati i SIGTERM za vrijeme obrade */
    act.sa_flags = 0; /* naprednije mogućnosti preskočene */
    sigaction(SIGUSR1, &act, NULL); /* maskiranje signala preko sučelja OS-a */
    
    /* 2. maskiranje signala SIGTERM */
    act.sa_handler = obradi_sigterm;
    sigemptyset(&act.sa_mask);
    sigaction(SIGTERM, &act, NULL);
 
    /* 3. maskiranje signala SIGINT */
    act.sa_handler = obradi_sigint;
    sigaction(SIGINT, &act, NULL);

    /* Glavni dio programa */
        FILE *dat;

        dat = fopen("status.txt", "w+");
        if (!fscanf(dat, "%d", &broj)) {
            printf("Neuspješno pročitana datoteka status.txt\n");
            return 1;
        }
        fprintf(dat,"%d", 0);
        fclose(dat);

        //ako je u status.txt upisana 0, provjerava se zadnji broj u obrada.txt
        if (!broj) {
            dat = fopen("obrada.txt", "r");
            while (fscanf(dat, "%d", &kvadrat) == 1) {}
            broj = sqrt(kvadrat);
            fclose(dat);
        }

        printf("Program s PID=%ld krenuo s radom\n", (long) getpid());
        while (nije_kraj) {
            dat = fopen ("obrada.txt", "a");
            broj++;
            kvadrat = broj * broj;
            fprintf(dat, " %d", kvadrat);
            sleep(5);
            fclose(dat);
        }
        printf("Program s PID=%ld zavrsio s radom\n", (long) getpid());

    return 0;
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