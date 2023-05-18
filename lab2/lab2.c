/* Glavna datoteka za lab2 s funkcijom main */

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "signali.h"
#include "datoteke.h"

char *dat_status, *dat_obrada, *dat_mreza;
int broj = 0;
int nije_kraj = 1;

/* funkcije koje rade dretve */
void *obrada(void *);
void *mreza(void *);

int main(int argc, char *argv[])
{
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

	pthread_t radna_dretva_id, mreza_dretva_id;
    int ret;

    // stvaranje radne dretve koja izvrsava funkciju obrada
    ret = pthread_create(&radna_dretva_id, NULL, obrada, NULL);
    if (ret) {
        fprintf(stderr, "Neuspjelo stvaranje radne dretve.\n");
        exit(1);
    }
    
    // stvaranje mrezne dretve koja izvrsava funkciju mreza
    ret = pthread_create(&mreza_dretva_id, NULL, mreza, NULL);
    if (ret) {
        fprintf(stderr, "Neuspjelo stvaranje mrežne dretve.\n");
        exit(1);
    }

    printf("Program s PID=%ld krenuo s radom\n", (long) getpid());
    printf("\nSlobodno unesi broj: ");
    while (nije_kraj) {
        int uneseni_broj;
        scanf("%d", &uneseni_broj);

        if (uneseni_broj > 0) 
           broj = uneseni_broj; // Promjena trenutnog broja
        
        else {
            printf("Primljen negativan broj. Prekidam unos.\n");
            nije_kraj = 0;
            break;
        }
    }

    
    pthread_join(radna_dretva_id, NULL);
    printf("Program je završio s radom.\n");

    return 0;
}

// funkcija obrada koju koristi radna_dretva
void *obrada(void *p) {
    int kvadrat;  
    
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
        int provjera;

        provjera = procitaj_status();
        zapisi_status(0);

        broj = provjera;
        //ako je u status.txt upisana 0, provjerava se zadnji broj u obrada.txt
        if (!provjera) {
            kvadrat = pronadji_zadnji_broj();
            broj = sqrt(kvadrat);
        }

        while (nije_kraj) {
            printf ("na redu je broj: %d\n", broj+1);
            broj++;
            kvadrat = broj * broj;
            dodaj_broj(kvadrat);
            sleep(5);
        }
        
        printf("Program s PID=%ld zavrsio s radom\n", (long) getpid());
    return NULL;
}

// funckija mreza koju koristi mrezna_dretva
void *mreza(void *p) {

    while (1) {
        // dohvacanje broja iz cijevi
        int broj_cijev = dohvati_iz_cijevi();

        if (broj_cijev > 0) {
            printf("Primljen broj preko mreze: %d\n", broj_cijev);
            broj = broj_cijev;
        }

        if (!nije_kraj)
            break;
    }
    
    return NULL;
}
