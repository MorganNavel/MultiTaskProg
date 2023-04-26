/*
  Programme passager à compléter : les zones à compléter sont indiquées et il n'est pas nécessaire d'ajouter de nouvelles traces d'exécution.
   
  Rappel pour le dépôt : sur Moodle, donner les instructions pour la création et l'initialisation du tableau de sémaphores nécessaires au lancement de ./bin/semInit (voir le sujet)   
*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include "simulation.h"

int main(int argc, char * argv[]){

  initDefault(10); // ne pas modifier ni déplacer cette ligne.
  
  if (argc!=5) {
    printf("Nbre d'args invalide, utilisation :\n");
    printf("%s destination aller_ou_retour fichier-pour-cle-ipc entier-pour-cle-ipc\n", argv[0]);
    exit(0);
  }
    
  int cle =ftok(argv[3], atoi(argv[4]));

  int idSem = semget(cle, 1, 0666);

  // j'utilise semget de sorte a s'assurer que le tableau existe.
  if (idSem == -1){
    perror("erreur  semget");
    exit(-1);
  }
  
  char * destination = argv[1];
  int aller_ou_retour = atoi(argv[2]); // 0 si je suis un passager du voyage aller, sinon 1 (pour un vol retour) 
  
  char * couleurTrace = "\x1b[38;5;226m";  // ne pas modifier ni déplacer
  if (aller_ou_retour) couleurTrace="\x1b[38;5;46m";  // ces deux lignes.
  
  // ... vous pouvez ajouter du code ici si nécessaire

  afficher('p', "Yoopi, je vais prendre l'avion !", destination, couleurTrace);
   
  afficher('p', "je patiente en salle d'embarquement", destination, couleurTrace);

  /* ... à compléter pour attendre l'annonce de l'embarquement.  */
  if(!aller_ou_retour){
    struct sembuf opZ;
    opZ.sem_num = 0;
    opZ.sem_op = 0;
    opZ.sem_flg = 0;
    if(semop(idSem,&opZ,1)==-1){
      perror("error semop:");
      exit(1);
    }
  }else{
    struct sembuf opZ;
    opZ.sem_num = 2;
    opZ.sem_op = 0;
    opZ.sem_flg = 0;
    if(semop(idSem,&opZ,1)==-1){
      perror("error semop:");
      exit(1);
    }
  }
   

  afficher('p', "j'embarque ...", destination, couleurTrace);
  
  
  action(); // simulation de l'action embarquer.
 
  // ... à compléter
  if(!aller_ou_retour){
    struct sembuf opP;
    opP.sem_num = 0;
    opP.sem_op = -1;
    opP.sem_flg = 0;
    if(semop(idSem,&opP,1)==-1){
      perror("error semop:");
      exit(1);
    } 
  }else{
    struct sembuf opP;
    opP.sem_num = 2;
    opP.sem_op = -1;
    opP.sem_flg = 0;
    if(semop(idSem,&opP,1)==-1){
      perror("error semop:");
      exit(1);
    } 
  }
  
   
  // ici se produit la simulaion du vol. je n'ai rien à faire
   
  afficher('p', "maintenant, je patiente jusqu'à destination", destination, couleurTrace);
 
  /* ... à compléter pour attendre l'arrivée à destination et donc attendre de pouvoir débarquer.*/
  
  
  afficher('p', "je descends de l'avion", destination, couleurTrace);

  action(); // simulation de l'action débarquer.

  // ... à compléter
  if(!aller_ou_retour){
    struct sembuf opP2;
    opP2.sem_num = 1;
    opP2.sem_op = -1;
    opP2.sem_flg = 0;
    if(semop(idSem,&opP2,1)==-1){ //bloquant si ne peut pas décrémenté
      perror("error semop:");
      exit(1);
    } 
  }
  else{
    struct sembuf opP2;
    opP2.sem_num = 3;
    opP2.sem_op = -1;
    opP2.sem_flg = 0;
    if(semop(idSem,&opP2,1)==-1){ //bloquant si ne peut pas décrémenté
      perror("error semop:");
      exit(1);
    }
  }
  
    
  afficher('p', "je quitte l'aéroport !", destination, couleurTrace); 
 
  return 0;
}


 
