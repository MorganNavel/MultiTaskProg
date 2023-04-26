/*
  Programme passager à compléter : les zones à compléter sont indiquées et il n'est pas nécessaire d'ajouter de nouvelles traces d'exécution. 
  
  Vous devez expliquer en commentaires : le sens donné au messages echangés et aux étiquettes.
  Etiquette 1: Message d'annonce d'embarquement pour le vol allé
  Etiquette 2: Message Embarquement des passagers allé
  Etiquette 3: Message d'arrivé du vol allé
  Etiquette 4: Message de débarquement des passagers du vol allé
  Etiquette 5: Message d'annonce d'embarquement pour le vol retour
  Etiquette 6: Message embarquement des passagers retour
  Etiquette 7: Message d'arrivé du vol retour
  Etiquette 8: Message de débarquement des passagers du vol retour
    
*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include "simulation.h"
struct msgBuffer{
    long etiq;
};
int main(int argc, char * argv[]){

  initDefault(10); // ne pas modifier ni déplacer cette ligne
 
  if (argc!=5) {
    printf("Nbre d'args invalide, utilisation :\n");
    printf("%s destination aller_ou_retour fichier-pour-cle-ipc entier-pour-cle-ipc\n", argv[0]);
    exit(0);
  }
	   
  int cle =ftok(argv[3], atoi(argv[4]));
  int idFile = msgget(cle, 0666);

  // j'utilise msgget de sorte à s'assurer que la file existe avant de poursuivre
  if (idFile == -1){
    perror("erreur  msgget");
    exit(-1);
  }
  
  char * destination = argv[1];
  int aller_ou_retour = atoi(argv[2]); // 0 si je suis un passager du voyage aller, sinon 1 (pour un vol retour)  
  
  char * couleurTrace = "\x1b[38;5;226m";  // ne pas déplacer ni modifier
  if (aller_ou_retour) couleurTrace="\x1b[38;5;46m"; // ces deux lignes.
    
  // ... vous pouvez ajouter du code ici si nécessaire
  
  afficher('p', "Yoopi, je vais prendre l'avion !", destination, couleurTrace);
  afficher('p', "je patiente en salle d'embarquement", destination, couleurTrace);
  
  /* ... à compléter pour attendre l'annonce de l'embarquement.  */
  if(!aller_ou_retour){
    struct msgBuffer msg_recv;
    if(msgrcv(idFile, &msg_recv,0, 1, 0) == -1){
      perror("error msgrcv:");
      exit(1);
    }
  }
  else{
    struct msgBuffer msg_recv;
    if(msgrcv(idFile, &msg_recv,0, 5, 0) == -1){
      perror("error msgrcv:");
      exit(1);
    }
  }
  
   
  afficher('p', "j'embarque ...", destination, couleurTrace);
  action(); // simulation de l'action embarquer.
  
  // ... à compléter
  struct msgBuffer msg_send;
  if(!aller_ou_retour){
    msg_send.etiq = 2;
    if(msgsnd(idFile,&msg_send,0, 0)==-1){ //envoie de message de début d'embarquement
      perror("error msgsnd:");
      exit(1);
    }
  }
  else{
    msg_send.etiq = 6;
    if(msgsnd(idFile,&msg_send,0, 0)==-1){ //envoie de message de début d'embarquement
      perror("error msgsnd:");
      exit(1);
    }
  }
  
     
  // ici se produit la simulaion du vol. je n'ai rien à faire
   
  afficher('p', "maintenant, je patiente jusqu'à destination", destination, couleurTrace);
  
  /* ... à compléter pour attendre l'arrivée à destination et donc attendre de pouvoir débarquer.*/
  struct msgBuffer msg_arrivee;
  if(!aller_ou_retour){
    if(msgrcv(idFile, &msg_arrivee,0, 3, 0) == -1){
      perror("error msgrcv:");
      exit(1);
    }
  }
  else{
    if(msgrcv(idFile, &msg_arrivee,0, 7, 0) == -1){
      perror("error msgrcv:");
      exit(1);
    }
  }
  
   
  afficher('p', "je descends de l'avion", destination, couleurTrace);
  struct msgBuffer msg_dec_avion;
  if(!aller_ou_retour){
    msg_dec_avion.etiq = 4;
    if(msgsnd(idFile,&msg_dec_avion,0, 0)==-1){ //envoie de message de début d'embarquement
      perror("error msgsnd:");
      exit(1);
    }
  }
  else{
    msg_dec_avion.etiq = 8;
    if(msgsnd(idFile,&msg_dec_avion,0, 0)==-1){ //envoie de message de début d'embarquement
      perror("error msgsnd:");
      exit(1);
    }
  }
  
  action(); // simulation de l'action débarquer.

  // ... à compléter
  
  afficher('p', "je quitte l'aéroport !", destination, couleurTrace); 
 
  return 0;
}

