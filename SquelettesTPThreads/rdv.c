#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include "calcul.h"
#include <stdio.h>
#include <stdlib.h>
struct predicatRdv{
  // regrouoes les donn�e partag�es entres les threads participants aux RdV :
  int nbThread;
  int cpt;
  pthread_mutex_t verrou;
  pthread_cond_t cond;
};

struct params {
  // structure pour regrouper les param�tres d'un thread.
  int idThread; // un identifiant de thread, de 1 � N (N le nombre
	// total de theads secondaires
  struct predicatRdv * varPartagee;
};

// fonction associ�e a chaque thread participant au RdV.

void * participant (void * p){ 

  struct params * args = (struct params *) p;
  struct predicatRdv * predicat = args -> varPartagee;
  printf("Naissance du Thread n°%d\n",args->idThread);

  // simulation d'un long calcul pour le travail avant RdV
  printf("Thread n°%d début du calcul\n",args->idThread);
  calcul(1); // c'est un exemple.
  printf("Thread n°%d fin du calcul\n",args->idThread);
  if(pthread_mutex_lock(&(predicat->verrou))!=0){
    perror("Erreur du vérrouillage du verrou:");
    exit(1);
  }
  predicat->cpt++;
  printf("Thread n°%d il y a %d qui ont fini leur première tâche\n",args->idThread,predicat->cpt);
  // RdV 
  if(predicat->cpt == predicat->nbThread){
    printf("Thread n°%d réveille tous les threads\n",args->idThread);
    if(pthread_cond_broadcast(&(predicat->cond))!=0){
      perror("Erreur du diffusion d'évènement:");
      exit(1);
    }
  }
  else{      
    while(predicat->cpt<predicat->nbThread){  // attention : le dernier arriv� ne doit pas attendre. Il doit r�veiller tous les autres.
      printf("Thread n°%d est mis en attente\n",args->idThread);
      if(pthread_cond_wait(&(predicat->cond),&(predicat->verrou))!=0){
        perror("Erreur lors de l'attente:");
        exit(1);
      }
      printf("Thread n°%d a été réveillé\n",args->idThread);
    }
  }
  if(pthread_mutex_unlock(&predicat->verrou)!=0){
    perror("Erreur du dérrouillage du verrou:");
    exit(1);
  }
  // calcul (1); // reprise et poursuite de l'execution.
  pthread_exit(NULL); // fortement recommand�.
}

int main(int argc, char * argv[]){
  
  if (argc!=2) {
    printf(" argument requis\n");
    printf("./prog nombre_Threads\n");
    exit(1);
  }

  // initialisations 
  pthread_t threads[atoi(argv[1])];
  struct params tabParams[atoi(argv[1])];
  struct predicatRdv vPartagees;
  vPartagees.nbThread = atoi(argv[1]);
  vPartagees.cpt=0;

  if(pthread_cond_init(&vPartagees.cond,NULL)!=0){
    perror("erreur initialisation de la condition:");
    exit(1);
  }

  if(pthread_mutex_init(&vPartagees.verrou,NULL)!=0){
    perror("erreur initialisation du verrou:");
    exit(1);
  }

  srand(atoi(argv[1]));  // initialisation de rand pour la simulation de longs calculs

  // cr�ation des threards 
  for (int i = 0; i < atoi(argv[1]); i++){

    tabParams[i].idThread = i;
    tabParams[i].varPartagee = &vPartagees;

    if (pthread_create(&threads[i], NULL,participant,&tabParams[i]) != 0){
      perror("erreur creation thread");
      exit(1);
    }
  }
  // attente de la fin des  threards. Partie obligatoire 
  for (int i = 0; i < atoi(argv[1]); i++){
    if (pthread_join(threads[i],NULL) != 0){
      perror("erreur join");
      exit(1);
    }
  }
  printf("thread principal : fin de tous les threads secondaires\n");

  // terminer "proprement". 
}