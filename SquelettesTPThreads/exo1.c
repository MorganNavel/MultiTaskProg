#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include "calcul.h"
#include <stdio.h>
#include <stdlib.h>


struct paramsFonctionThread {

  int idThread;


};


void * fonctionThread (void * params){

  struct paramsFonctionThread * args = (struct paramsFonctionThread *) params;
  printf("Naissance du Thread n°%d\n",args->idThread);
  pthread_exit(NULL);
}


int main(int argc, char * argv[]){

  if (argc < 2 ){
    printf("utilisation: %s  nombre_threads  \n", argv[0]);
    return 1;
  }     

  
  pthread_t threads[atoi(argv[1])];
  struct paramsFonctionThread params[atoi(argv[1])];
 
  
  // cr�ation des threards 
  for (int i = 0; i < atoi(argv[1]); i++){

    // Le passage de param�tre est fortement conseill� (�viter les
    // variables globles).
    params[i].idThread=i;
    // compl�ter pour initialiser les param�tres
    if (pthread_create(&threads[i], NULL,fonctionThread, &params[i]) != 0){
      perror("erreur creation thread");
      exit(1);
    }
  }


  for (int i = 0; i < atoi(argv[1]); i++){
    if (pthread_join(threads[i],NULL) != 0){
      perror("erreur join");
      exit(1);
    }
  }

// garder cette saisie et modifier le code en temps venu.
  char c; 
  printf("saisir un caractère \n");
  fgets(&c, 1, stdin);

  printf("thread principal : fin de tous les threads secondaires\n");
  return 0;
}
 
