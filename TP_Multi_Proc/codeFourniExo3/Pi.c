/*
Dans cet exercice le but est d'imposé un ordre dans l'application de traitement
sur des zones d'unes image par exemple.




*/
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <stdio.h>//perror
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#define ZONE_SIZE 1000

int main(int argc,char * argv []){
    if(argc!=5){
        printf("Nbre d'args invalide, utilisation :\n");
        printf("%s <fichier-ipc> <cle-ipc> <zone> <id>\n", argv[0]);
        exit(1);
    }
    int id = atoi(argv[4]);
    int key = ftok(argv[1], atoi(argv[2]));
    if (key==-1) {
        perror("Erreur ftok : ");
        exit(2);
    }
    printf("Cle: %d\n",key);
    int nbZone = atoi(argv[3]);
    int idSem=semget(key, nbZone, IPC_EXCL | 0600);
    if(idSem==-1){
        perror("error: sémaphore");
        exit(1);
    }
    //On a un tableau de sémaphore qui correspond au nombre de traitement (processus)
    printf("Identifiant Semaphore: %d\n",idSem);
    // Définition des opérations au sémaphore
    int image[nbZone*ZONE_SIZE]; //nbZone traitement par pixel de l'image


    //p_att est le pointeur vers la zone partagé à l'extérieur de mon espace de mémoire
    // int memory = shmget(key, sizeof(image), IPC_CREAT | IPC_EXCL | 0600);
    // if (memory == -1) {
    //     perror("(shmget) ");
    //     return EXIT_FAILURE;
    // }
    for(int i = 0; i<nbZone;i++){
        union semun{
            int val;
            struct semid_ds * buf;
            ushort * array;
        } valinit;

        /* test affichage des valeurs des s�maphores du tableau */
        valinit.array = (ushort*)malloc(nbZone * sizeof(ushort));

        if (semctl(idSem, nbZone, GETALL, valinit) == -1){
            perror("erreur initialisation sem : ");
            exit(1);
        }
        printf("Valeurs des sempahores [ "); 
        for(int i=0; i < nbZone-1; i++){
            printf("%d, ", valinit.array[i]);
        }
        printf("%d ] \n", valinit.array[nbZone-1]);

        free(valinit.array);
        struct sembuf opP;
        opP.sem_num = i;
        opP.sem_op = -id-1;
        opP.sem_flg = 0;

        struct sembuf opV;
        opV.sem_num = i;
        opV.sem_op = (id+1)+1;
        opV.sem_flg = 0;
        
        printf("Numéro de sémaphore à changer %d\n",opP.sem_num);
        printf("Opération %d\n",opP.sem_op);
        if(semop(idSem,&opP,1)==-1){
            perror("error: opérations P");
            exit(1);
        }
        //Calcule
        printf("Numéro de sémaphore à changer %d\n",opV.sem_num);
        printf("Opération %d\n",opV.sem_op);    
        if(semop(idSem,&opV,1)==-1){
            perror("error: opérations V");
            exit(1);
        }
    }
    printf("(Main) Fin\n");
    return 0;
}