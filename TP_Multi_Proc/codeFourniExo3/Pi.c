#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <stdio.h>//perror
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>

int main(int argc,char * argv []){
    if(argc!=5){
        printf("Nbre d'args invalide, utilisation :\n");
        printf("%s fichier-pour-cle-ipc entier-pour-cl�-ipc nbre zone id\n", argv[0]);
        exit(1);
    }
    int id = atoi(argv[4]);
    int cle = ftok(argv[1], atoi(argv[2]));
    if (cle==-1) {
        perror("Erreur ftok : ");
        exit(2);
    }
    printf("Cle: %d\n",cle);
    int nbZone = atoi(argv[3]);
    int idSem=semget(cle, nbZone, IPC_EXCL | 0600);
    if(idSem==-1){
        perror("error: sémaphore");
        exit(1);
    }
    //On a un tableau de sémaphore qui correspond au nombre de traitement (processus)
    printf("Identifiant Semaphore: %d\n",idSem);
    // Définition des opérations au sémaphore
    


    // int Mem=shmget(clesem, nbZone*sizeof(int), IPC_CREAT | IPC_EXCL |0600);
    // if(Mem==-1){
    //     perror("erreur accès segment partagé:");
    //     exit(1);
    // }
    // int * p_att = (int *)shmat(Mem, NULL, 0);
    // if (p_att== (int *)-1){
    //     perror("erreur shmat : ");
    //     exit(-1);
    // }
    //p_att est le pointeur vers la zone partagé à l'extérieur de mon espace de mémoire
    for(int i = 0; i<nbZone;i++){ //nombre de zone c'est le nombre de semaphore

        struct sembuf opP;
        opP.sem_num = i;
        opP.sem_op = id;
        opP.sem_flg = 0;


        struct sembuf opV;
        opP.sem_num = i;
        opP.sem_op = id+1;
        opP.sem_flg = 0;
        if(id!=0){
            if(semop(idSem,&opP,1)==-1){
                perror("error: opérations P");
                exit(1);
            }
        }
        // p_att[i]++;
        if(semop(idSem,&opV,1)==-1){
            perror("error: opérations V");
            exit(1);
        }
    }
    return 0;
}