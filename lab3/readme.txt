lab 3
unosom broja u konzolu mijenja se broj u obradi
unosom broja u cijev mijenja se broj aktivnih dretvi

pokretanje programa -> naredba $ make pokreni
brisanje datoteka -> $ make obrisi

prilikom izvršavanja programa:
status.txt -> 0
obrada.txt -> popis kvadrata brojeva koje je do sad program prošao

signal SIGINT
- poziv: $ kill -SIGINT <PID> ili CTRL+C
- ispis u terminalu: "Primio signal SIGINT, prekidam rad"
- radnja: prekida program naredbom exit(0)
- status.txt -> 0
- obrada.txt -> popis kvadrata brojeva koje je do sada program zapisao u datoteku

signal SIGTERM
- poziv: $ kill -SIGTERM <PID> 
- ispis u terminalu: "Primio signal SIGTERM, pišem u datoteku status zadnji broj i prekidam rad"
- radnja: prekida program postavljanjem varijable nije_kraj = 1 i zapisuje u status.txt zadnji broj koji koristi u obradi
- status.txt -> zadnji broj čiji kvadrat je zapisan u obrada.txt
- obrada.txt -> popis kvadrata brojeva koje je do sada program zapisao u datoteku

signal SIGUSR1
- poziv: $ kill -SIGUSR1 <PID> 
- ispis u terminalu: "Primio signal SIGUSR1, vraćam broj koji koristim u obradi\n"
- radnja: vraća broj koji koristi u obradi
- status.txt -> 0
- obrada.txt -> popis kvadrata brojeva koje je do sada program zapisao u datoteku

slanje broja kroz cjevovod
- naredba echo broj > cijev

primjer rada programa;

terminal 1:                           terminal 2:
make pokreni                          
Program s PID=40 krenuo s radom
na redu je broj: 1
na redu je broj: 2
na redu je broj: 3
na redu je broj: 4                    echo 3 > cijev
Dobiven novi broj dretvi: 3
Stvaram dretvu broj:2
Stvaram dretvu broj:3
na redu je broj: 5                    
2 //unos broja 2 kroz konzolu 
na redu je broj: 3
na redu je broj: 4                    
^C // CTRL + C
Primio signal SIGINT, prekidam rad


