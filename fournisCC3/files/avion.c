/*
  Programme avion à compléter. Les zones à compléter sont indiquées et il n'est pas nécessaire d'ajouter de nouvelles traces d'exécution.
  
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

  initDefault(10); // ne pas modifier ni déplacer 
  char * couleurTrace = "\x1b[38;5;39m"; //ces deux ligne.
     
  if (argc!=7) {
    printf("Nbre d'args invalide, utilisation :\n");
    printf("%s destination_aller nombre_passagers_aller destination_retour nombre_passagers_retour fichier-pour-cle-ipc entier-pour-cle-ipc\n", argv[0]);
    exit(0);
  }
	  
  char * dest_aller = argv[1];
  int nbPassagers_Aller = atoi(argv[2]);
    
  char * dest_retour = argv[3];
  int nbPassagers_Retour = atoi(argv[4]);
  
  int cle=ftok(argv[5], atoi(argv[6]));

  int idFile = msgget(cle, 0666);

  // j'utilise msgget de sorte à s'assurer que la file existe avant de poursuivre l'exécution.
  if (idFile == -1){
    perror("erreur  msgget");
    exit(-1);
  }
  
  // ... vous pouvez ajouter du code ici si nécessaire
  int cpt_emb = 0;
 
  // dans la suite, les traces d'exécution données sont à garder inchangées.
  
  // vol aller 
  afficher('a', "embarquement immediat !", dest_aller, couleurTrace);
   
  /* ... à compléter pour :
    
     1) annoncer le début de l'embarquement aux passagers de l'aller
     2) attendre que tous les passagers de l'aller aient embarqué. Cette partie doit inclure la ligne suivante au bon moment :
    
     afficher('a', "attente fin embarquement", dest_aller, couleurTrace);   
    
  */
  struct msgBuffer msg;
  msg.etiq = 1;
  for(int i =0;i<nbPassagers_Aller;i++){
    if(msgsnd(idFile,&msg,0, 0)==-1){ //envoie de message de début d'embarquement
      perror("error msgsnd:");
      exit(1);
    }
  }

  while(cpt_emb<nbPassagers_Aller){
    struct msgBuffer msg_recv;
    if(msgrcv(idFile, &msg_recv,0, 2, 0) == -1){ //reception d'un passager
      perror("error msgrcv:");
      exit(1);
    }
    cpt_emb++;
  }
      
  afficher('a',"décolage imminent! Durée du vol estimée à quelques secondes", dest_aller, couleurTrace); 
 
  vol(4);  // simulation d'une durée de vol (vous pouvez tester avec d'autres valeurs. Le code rendu doit toutefois être vol(4); 
  
  afficher('a',"arrivé à destination", dest_aller, couleurTrace);

  /* ... à compléter pour :
  
     1) annoncer l'arrivée à destination et donc le début du débarquement
     2) attendre que tous les passagers soient descendus de l'avion. Cette partie doit inclure la ligne suivante au bon moment :
  
     afficher('a', "attente que tout le monde soit descendu", dest_aller, couleurTrace);
  
  */
  struct msgBuffer msg_arrivee;
  msg_arrivee.etiq = 3;
  for(int i =0;i<nbPassagers_Aller;i++){
    if(msgsnd(idFile,&msg_arrivee,0, 0)==-1){ //envoie de message de début d'embarquement
      perror("error msgsnd:");
      exit(1);
    }
  }
  int cpt_deb=0;
  struct msgBuffer msg_recv;
  afficher('a', "attente que tout le monde soit descendu", dest_aller, couleurTrace);
  while(cpt_deb<nbPassagers_Aller){
    if(msgrcv(idFile, &msg_recv,0, 4, 0) == -1){ //reception d'un passager
      perror("error msgrcv:");
      exit(1);
    }
    cpt_deb++;
  }
     
  afficher('a', "vol aller terminé", dest_aller, couleurTrace);
  
  // vol retour 
  afficher('a', "embarquement immediat !", dest_retour, couleurTrace);
  
  /* ... à compléter. 
   
     Remarques :
     - ne faire cette partie qu'une fois l'implémentation du vol aller terminée et testée !
     - les processus passager du vol retour ne doivent pas être réveillés inutilement tout au long de l'étape implémentant l'aller. 
  */
  //DEBUT RETOUR 
  struct msgBuffer msg2;
  msg2.etiq = 5;
  for(int i =0;i<nbPassagers_Aller;i++){
    if(msgsnd(idFile,&msg2,0, 0)==-1){ //envoie de message de début d'embarquement
      perror("error msgsnd:");
      exit(1);
    }
  }

  while(cpt_emb<nbPassagers_Aller){
    struct msgBuffer msg_recv2;
    if(msgrcv(idFile, &msg_recv2,0, 6, 0) == -1){ //reception d'un passager
      perror("error msgrcv:");
      exit(1);
    }
    cpt_emb++;
  }
      
  afficher('a',"décolage imminent! Durée du vol estimée à quelques secondes", dest_retour, couleurTrace); 
 
  vol(4);  // simulation d'une durée de vol (vous pouvez tester avec d'autres valeurs. Le code rendu doit toutefois être vol(4); 
  
  afficher('a',"arrivé à destination", dest_retour, couleurTrace);

  /* ... à compléter pour :
  
     1) annoncer l'arrivée à destination et donc le début du débarquement
     2) attendre que tous les passagers soient descendus de l'avion. Cette partie doit inclure la ligne suivante au bon moment :
  
     afficher('a', "attente que tout le monde soit descendu", dest_retour, couleurTrace);
  
  */
  struct msgBuffer msg_arrivee2;
  msg_arrivee2.etiq = 7;
  for(int i =0;i<nbPassagers_Retour;i++){
    if(msgsnd(idFile,&msg_arrivee2,0, 0)==-1){ //envoie de message de début d'embarquement
      perror("error msgsnd:");
      exit(1);
    }
  }
  int cpt_deb2=0;
  afficher('a', "attente que tout le monde soit descendu", dest_retour, couleurTrace);
  while(cpt_deb2<nbPassagers_Retour){
    struct msgBuffer msg_recv2;
    if(msgrcv(idFile, &msg_recv2,0, 8, 0) == -1){ //reception d'un passager
      perror("error msgrcv:");
      exit(1);
    }
    cpt_deb2++;
  }
      
  
  afficher('a', "vol retour terminé", dest_retour, couleurTrace); 
  return 0;
      
}

