#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include "calcul.h"
#include <stdio.h>
#include <stdlib.h>

// structure qui regroupe les variables partag�es entre les threads.
struct varPartagees {
  int nbZones;
  int * di; // le tableau indiqu� dans l'�nonc�
  pthread_mutex_t verrou;
  pthread_cond_t cond;


};

// structure qui regroupe les param�tres d'un thread
struct params {

  int idThread; // cet entier correspond au num�ro de traitement associ� � un thread
  struct varPartagees * vPartage;


};

// fonction associ�e � chaque thread secondaire � cr�er.

void * traitement (void * p) {

  struct params * args = (struct params *) p;
  struct  varPartagees *  vPartage = args -> vPartage;
  printf("Naissance du Thread n°%d\n",args->idThread);

  for(int i = 1; i <= vPartage -> nbZones; i++){
    if( args -> idThread != 0){ 
      // je ne suis pas le premier
      if(pthread_mutex_lock(&(vPartage->verrou))!=0){
        perror("Erreur du vérrouillage du verrou:");
        exit(1);
      }
      while(vPartage->di[args->idThread-1]<=i){
   	    // attente fin traitement sur la zone i
        printf("Thread n°%d est mis en attente pour aller sur la zone %d\n",args->idThread,i);
        if(pthread_cond_wait(&(vPartage->cond),&(vPartage->verrou))!=0){
          perror("Erreur lors de l'attente:");
          exit(1);
        }
      }
      if(vPartage->di[args->idThread-1]>i){
        // J'ai fini d'attendre je peux donc effectué mon calcul
        vPartage->di[args->idThread]++;
        printf("Thread n°%d réveille tous les threads\n",args->idThread);
        if(pthread_cond_broadcast(&(vPartage->cond))!=0){
          perror("Erreur du diffusion d'évènement:");
          exit(1);
        }
      }
      if(vPartage->di[args->idThread]==vPartage->nbZones){
        vPartage->di[args->idThread]++;
      }
      if(pthread_mutex_unlock(&vPartage->verrou)!=0){
        perror("Erreur du dérrouillage du verrou:");
        exit(1);
      }
      printf("Thread n°%d début du calcul de la zone %d\n",args->idThread,i);
      calcul(1);
      printf("Thread n°%d fin du calcul de la zone %d\n",args->idThread,i);

    }else{
      // je suis le premier thread
      printf("Thread n°%d début du calcul de la zone %d\n",args->idThread,i);
      calcul(1);
      printf("Thread n°%d fin du calcul de la zone %d\n",args->idThread,i);
      if(pthread_mutex_lock(&(vPartage->verrou))!=0){
        perror("Erreur du vérrouillage du verrou:");
        exit(1);
      }
      vPartage->di[args->idThread]++;
      printf("Thread n°%d réveille tous les threads\n",args->idThread);
      if(pthread_cond_broadcast(&(vPartage->cond))!=0){
        perror("Erreur du diffusion d'évènement:");
        exit(1);
      }
      if(vPartage->di[args->idThread]==vPartage->nbZones){
        vPartage->di[args->idThread]++;
      }
      if(pthread_mutex_unlock(&vPartage->verrou)!=0){
        perror("Erreur du dérrouillage du verrou:");
        exit(1);
      }
    }
  }
  printf("Thread n°%d se termine\n",args->idThread);

  pthread_exit(NULL); 
}





int main(int argc, char * argv[]){
  
  if (argc!=3) {
    printf(" argument requis\n");
    printf("./prog nombre_Traitements nombre_Zones\n");
    exit(1);
  }

 
   // initialisations 
  pthread_t threads[atoi(argv[1])];
  struct params tabParams[atoi(argv[1])];
  struct varPartagees vPartage;
  // allocation du tableau
  vPartage.di = malloc(atoi(argv[2])*sizeof(int));
  // initialisation pour évité les problèmes plutard
  for(int i = 0;i<atoi(argv[2]);i++){
    vPartage.di[i]=0;
  }
  if(pthread_cond_init(&vPartage.cond,NULL)!=0){
    perror("erreur initialisation de la condition:");
    exit(1);
  }

  if(pthread_mutex_init(&vPartage.verrou,NULL)!=0){
    perror("erreur initialisation du verrou:");
    exit(1);
  }
  vPartage.nbZones =  atoi(argv[2]);
  
  srand(atoi(argv[1]));  // initialisation de rand pour la simulation de longs calculs
 
  // cr�ation des threards 
  for (int i = 0; i < atoi(argv[1]); i++){
    tabParams[i].idThread = i;
    tabParams[i].vPartage = &vPartage; 
    if (pthread_create(&threads[i], NULL,traitement,&tabParams[i]) != 0){
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

  // lib�rer les ressources avant terminaison 
  return 1;
}
 
