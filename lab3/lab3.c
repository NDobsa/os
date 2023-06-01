/* Glavna datoteka za lab3 s funkcijom main */

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "signali.h"
#include "datoteke.h"


char* dat_status, * dat_obrada, * dat_mreza;
int broj = 0;
int nije_kraj = 1;
int broj_dretvi = 1; // Trenutni broj radnih dretvi
int postavljeni_broj_dretvi = 1; // Željeni broj radnih dretvi
int novi_broj_dretvi = 0;
int treba_stvoriti_dretve = 0;

pthread_mutex_t monitor; // Monitor za sinkronizaciju pristupa zajednièkim podacima

/* funkcije koje rade dretve */
void* obrada(void*);
void* mreza(void*);

int main(int argc, char* argv[])
{
    pthread_mutex_init(&monitor, NULL);

    if (argc < 4) {
        fprintf(stderr, "Koristenje: %s <status-datoteka> "
            "<datoteka-s-podacima> <cjevovod>\n", argv[0]);
        exit(1);
    }

    dat_status = argv[1];
    dat_obrada = argv[2];
    dat_mreza = argv[3];

    postavi_signale();

    // dohvati prvi broj i zapisi 0 u status 
    broj = pronadji_zadnji_broj();
    zapisi_status(0);

    pthread_t mreza_dretva_id, radna_dretva_id;
    int ret;

    // stvaranje mrezne dretve koja izvrsava funkciju mreza
    ret = pthread_create(&mreza_dretva_id, NULL, mreza, NULL);
    if (ret) {
        fprintf(stderr, "Neuspjelo stvaranje mrežne dretve.\n");
        exit(1);
    }

    // stvaranje radne dretve koja izvrsava funkciju obrada
    ret = pthread_create(&radna_dretva_id, NULL, obrada, NULL);
    if (ret) {
        fprintf(stderr, "Neuspjelo stvaranje radne dretve.\n");
        exit(1);
    }

    printf("Program s PID=%ld krenuo s radom\n", (long)getpid());
    printf("\nSlobodno unesi broj: ");
    while (nije_kraj) {
        int uneseni_broj;
        scanf("%d", &uneseni_broj);

        if (uneseni_broj > 0) {
            pthread_mutex_lock(&monitor);
            broj = uneseni_broj;
            pthread_mutex_unlock(&monitor);
        }

        else if (uneseni_broj <= 0){
            printf("Primljen negativan broj. Prekidam unos.\n");
            nije_kraj = 0;
            break;
        }
    }

    pthread_join(radna_dretva_id, NULL);
    pthread_join(mreza_dretva_id, NULL);
    pthread_mutex_destroy(&monitor);
    
    printf("Program je završio s radom.\n");

    return 0;
}

// funkcija obrada koju koristi radna_dretva
void* obrada(void* p) {

    struct sigaction act;
    // 1. maskiranje signala SIGUSR1
    act.sa_handler = obradi_dogadjaj;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGTERM); // blokirati i SIGTERM za vrijeme obrade */
    act.sa_flags = 0;
    sigaction(SIGUSR1, &act, NULL);

    // 2. maskiranje signala SIGTERM 
    act.sa_handler = obradi_sigterm;
    sigemptyset(&act.sa_mask);
    sigaction(SIGTERM, &act, NULL);

    // 3. maskiranje signala SIGINT 
    act.sa_handler = obradi_sigint;
    sigaction(SIGINT, &act, NULL);

    /* Glavni dio programa */

    int kvadrat, moj_broj;

    pthread_mutex_lock(&monitor);

    while (nije_kraj) {
        if (broj_dretvi > postavljeni_broj_dretvi) {
            broj_dretvi--;
            printf("Brisem dretvu.\n");
            pthread_mutex_unlock(&monitor);
            pthread_exit(NULL);
        }

        printf("Radim s brojem:%d\n", broj + 1);
        moj_broj = broj;
        broj++;
        kvadrat = moj_broj * moj_broj;
        dodaj_broj(kvadrat);

        pthread_mutex_unlock(&monitor);
        sleep(5);
        pthread_mutex_lock(&monitor);
    }
    pthread_mutex_unlock(&monitor);

    printf("Program s PID=%ld zavrsio s radom\n", (long)getpid());
    return NULL;
}


void* mreza(void* p)
{
    int cijev;

    while (1) {
        cijev = dohvati_iz_cijevi();

        if (cijev >= 1 && cijev <= 100) {
            printf("Dobiven novi broj dretvi: %d\n", cijev);

            pthread_mutex_lock(&monitor);
            postavljeni_broj_dretvi = cijev;

            while (broj_dretvi < postavljeni_broj_dretvi) {
                pthread_t dretva;
                pthread_create(&dretva, NULL, obrada, NULL);
                broj_dretvi++;
                printf("Stvaram dretvu broj:%d\n", broj_dretvi);
            }

            pthread_mutex_unlock(&monitor);
        }
        if (!nije_kraj)
            break;
    }

    return NULL;
}
