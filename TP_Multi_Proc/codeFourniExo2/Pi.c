#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <stdio.h>//perror
#include <string.h>
#include <unistd.h>
#include <iostream>
using namespace std;

int main(int argc, char * argv[]){
    if (argc!=3) {
        printf("Nbre d'args invalide, utilisation :\n");
        printf("%s fichier-pour-cle-ipc entier-pour-cl�-ipc\n", argv[0]);
        exit(0);
    }
    int clesem = ftok(argv[1], atoi(argv[2]));
	  
    
    if (clesem==-1) {
        perror("Erreur ftok : ");
        exit(2);
    }
    int idSem=semget(clesem, 1, 0666);
    if(idSem==-1){
        perror("error: sémaphore");
        exit(1);
    }
    cout<<"Identifiant Semaphore: "<<idSem<<endl;
    struct sembuf opP;
    opP.sem_num = 0;
    opP.sem_op = -1;
    opP.sem_flg = 0;

    
    struct sembuf opZ;
    opZ.sem_num = 0;
    opZ.sem_op = 0;
    opZ.sem_flg = 0;

    //décrémentation de la valeur du sémaphore
    if(semop(idSem,&opP,1)==-1){
        perror("error: opérations P");
        exit(1);
    }
    cout<<"Opération P effectué"<<endl;
    int val_Sem = semctl(idSem,GETVAL,0);
    if(val_Sem==-1){
        perror("error: semctl");
        exit(1);
    }

    cout<<"Valeur du sémaphore: "<<val_Sem<<endl;
    //attente que la valeur du sémaphore soit nulle
    if(semop(idSem,&opZ,1)==-1){
        perror("error: opérations Z");
        exit(1);
    }
    cout<<"Fin d'attente (Z)"<<endl;
    return 0;
}