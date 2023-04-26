/*
  Programme avion à compléter. Les zones à compléter sont indiquées et il n'est pas nécessaire d'ajouter de nouvelles traces d'exécution.
  
  Rappel pour le dépôt : sur Moodle, donner les instructions pour la création et l'initialisation du tableau de sémaphores nécessaires au lancement de ./bin/semInit (voir le sujet)
./bin/semInit 4 pourCle.txt entier
*/
 
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include "simulation.h"

int main(int argc, char * argv[]){

  initDefault(10); // ne pas modifier ni déplacer ces
  char * couleurTrace = "\x1b[38;5;39m"; // deux lignes.
    
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

  int idSem = semget(cle, 1, 0666);

  // j'utilise semget de sorte à s'assurer que le tableau de sémaphores existe avant de poursuivre l'exécution.
  if (idSem == -1){
    perror("erreur  semget");
    exit(-1);
  }
   
  // ... vous pouvez ajouter du code ici si nécessaire
 
  // dans la suite, les traces d'exécution données sont à garder inchangées.

  
  // vol aller 
  afficher('a', "embarquement immediat !", dest_aller, couleurTrace);
  
  /* ... à compléter pour annoncer le début de l'embarquement aux passagers de l'aller */
  struct sembuf opV;
  opV.sem_num = 0;
  opV.sem_op = nbPassagers_Aller;
  opV.sem_flg = 0;
  afficher('a', "attente fin embarquement", dest_aller, couleurTrace);    

  /* ... à compléter pour attendre que tous les passagers de l'aller aient embarqué.*/
  struct sembuf opZ;
  opZ.sem_num = 0;
  opZ.sem_op = 0;
  opZ.sem_flg = 0;
  if(semop(idSem,&opZ,1)==-1){
    perror("error semop:");
    exit(1);
  }  
  afficher('a',"décolage imminent! Durée du vol estimée à quelques secondes", dest_aller, couleurTrace); 
 
  vol(4); // simulation d'une durée de vol (vous pouvez tester avec d'autres valeurs. Le code rendu doit toutefois être vol(4); 
  
  afficher('a',"arrivé à destination", dest_aller, couleurTrace);


  /* ... à compléter pour annoncer l'arrivée à destination et donc le début du débarquement */
  struct sembuf opV2;
  opV2.sem_num = 1;
  opV2.sem_op = nbPassagers_Aller;
  opV2.sem_flg = 0;
  if(semop(idSem,&opV,1)==-1){
    perror("error semop:");
    exit(1);
  } 
    
  afficher('a', "attente que tout le monde soit descendu", dest_aller, couleurTrace); 
  
  /* ... à compléter pour attendre que tous les passagers soient descendus de l'avion.
  
   */
  struct sembuf opZ2;
  opZ2.sem_num = 1;
  opZ2.sem_op = 0;
  opZ2.sem_flg = 0;
  if(semop(idSem,&opZ2,1)==-1){
    perror("error semop:");
    exit(1);
  } 
 
  // vol retour 
  afficher('a', "embarquement immediat !", dest_retour, couleurTrace);

  /* ... à compléter. 
   
     Remarques :
     - ne faire cette partie qu'une fois l'implémentation du vol aller terminée et testée !
     - les processus passager du vol retour ne doivent pas être réveillés inutilement tout au long de l'étape implémentant l'aller.
   
  */
  // vol retour 
  afficher('a', "embarquement immediat !", dest_aller, couleurTrace);
  
  /* ... à compléter pour annoncer le début de l'embarquement aux passagers de l'aller */
  opV.sem_num = 2;
  opV.sem_op = nbPassagers_Retour;
  opV.sem_flg = 0;
  afficher('a', "attente fin embarquement", dest_aller, couleurTrace);    

  /* ... à compléter pour attendre que tous les passagers de l'aller aient embarqué.*/
  opZ.sem_num = 2;
  opZ.sem_op = 0;
  opZ.sem_flg = 0;
  if(semop(idSem,&opZ,1)==-1){
    perror("error semop:");
    exit(1);
  }  
  afficher('a',"décolage imminent! Durée du vol estimée à quelques secondes", dest_aller, couleurTrace); 
 
  vol(4); // simulation d'une durée de vol (vous pouvez tester avec d'autres valeurs. Le code rendu doit toutefois être vol(4); 
  
  afficher('a',"arrivé à destination", dest_aller, couleurTrace);


  /* ... à compléter pour annoncer l'arrivée à destination et donc le début du débarquement */
  opV2.sem_num = 3;
  opV2.sem_op = nbPassagers_Retour;
  opV2.sem_flg = 0;
  if(semop(idSem,&opV,1)==-1){
    perror("error semop:");
    exit(1);
  } 
    
  afficher('a', "attente que tout le monde soit descendu", dest_aller, couleurTrace); 
  
  /* ... à compléter pour attendre que tous les passagers soient descendus de l'avion.
  
   */
  opZ2.sem_num = 3;
  opZ2.sem_op = 0;
  opZ2.sem_flg = 0;
  if(semop(idSem,&opZ2,1)==-1){
    perror("error semop:");
    exit(1);
  }

  afficher('a', "vol rertour terminé", dest_retour, couleurTrace); 
  return 0;
      
}

