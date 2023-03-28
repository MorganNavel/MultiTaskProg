#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include "calcul.h"
#include <stdio.h>
#include <stdlib.h>
void changeCoureur(int * di,int idTeam,int nbTeam){
    // Alterne les coureurs
    printf("Changement de coureur de l'équipe n°%d",idTeam);
    int size_ = sizeof(di)/sizeof(int);
    int nbJoueur = size_/nbTeam; //nombre de joueur par équipe
    for(int i = idTeam*nbJoueur;i<idTeam*nbJoueur+nbJoueur;i++){
        if(di[i]==0){
            di[i]=1;
        }
        else{
            di[i]=0;
        }
    }
}
struct params{
    int idThread;
    struct varPartagees * vPartage;
    struct varPartageesTeam * vPartageTeam;
};
struct varPartageesTeam{
    int cpt;
    int idTeam;
    pthread_mutex_t verrou;
    pthread_cond_t cond;
};
struct varPartagees{
    int nbTour;
    int nbCoureur;
    int nbTeam;
    int * di;
    pthread_mutex_t verrou;
    pthread_cond_t cond;
};

void * race (void * p) {
    struct params * args = (struct params *) p;
    struct  varPartagees *  vPartage = args -> vPartage;
    struct  varPartageesTeam *  vPartageTeam = args -> vPartageTeam;
    printf("Naissance du Joueur n°%d\n",args->idThread);
    for(int i = 0;i<vPartage->nbTour;i++){
        // Début du nouveau tour
        if(pthread_mutex_lock(&(vPartage->verrou))!=0){
            perror("Erreur du vérrouillage du verrou:");
            exit(1);
        }
        while(vPartage->di[args->idThread]==0){ 
            // Mise en attente ceux qui ne joue pas (ceux qui ont 0 dans di)
            if(pthread_cond_wait(&(vPartageTeam->cond),&(vPartageTeam->verrou))!=0){
                perror("Erreur lors de l'attente:");
                exit(1);
            }
        }

        if(pthread_mutex_unlock(&(vPartage->verrou))!=0){
            perror("Erreur du vérrouillage du verrou:");
            exit(1);
        }

        printf("Joueur n°%d: DEPART !\n",args->idThread);
        calcul(rand()%10);
        printf("Joueur n°%d: ARRIVEE\n",args->idThread);


        if(pthread_mutex_lock(&(vPartageTeam->verrou))!=0){
            perror("Erreur du vérrouillage du verrou:");
            exit(1);
        }

        vPartageTeam->cpt++;
        if(vPartageTeam->cpt==vPartage->nbCoureur){
            printf("Passage de relai\n"); //réveille les deux autres coureurs
            vPartageTeam->cpt=0; //remets à zero la condition d'attente pour le prochain tour
            printf("Joueur n°%d",args->idThread);

            if(pthread_mutex_lock(&(vPartage->verrou))!=0){
                perror("Erreur du vérrouillage du verrou:");
                exit(1);
            }
            printf("Joueur n°%d",args->idThread);
            changeCoureur(vPartage->di,vPartageTeam->idTeam,vPartage->nbTeam);
            
            if(pthread_mutex_unlock(&(vPartage->verrou))!=0){
                perror("Erreur du dévérrouillage du verrou:");
                exit(1);
            }
            if(pthread_cond_broadcast(&(vPartage->cond))!=0){
                perror("Erreur du diffusion d'évènement:");
                exit(1);
            }
            printf("Joueur n°%d reveille ses partenaires\n",args->idThread);
        }else{
            // On attend le coéquipier
            while(vPartageTeam->cpt<vPartage->nbCoureur){
                printf("Joueur n°%d est mis en attente\n",args->idThread);
                if(pthread_cond_wait(&(vPartageTeam->cond),&(vPartageTeam->verrou))!=0){
                    perror("Erreur lors de l'attente:");
                    exit(1);
                }
            }
        }
        if(pthread_mutex_unlock(&vPartageTeam->verrou)!=0){
            perror("Erreur du dérrouillage du verrou:");
            exit(1);
        }
    }
    
    pthread_exit(NULL); 
}

int main(int argc, char * argv[]){

    if (argc!=5) {
        printf(" argument requis\n");
        printf("./prog nombre_Joueur_Par_Equipe nombre_Equipe nombre_Tour nombre_coureur_par_equipe\n");
        exit(1);
    }
    printf("%d , %d , %d ,%d\n",atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
    pthread_t threads[atoi(argv[1])*atoi(argv[2])];
    struct params tabParams[atoi(argv[1])*atoi(argv[2])];
    struct varPartagees vPartage;
    vPartage.nbTour= atoi(argv[3]);
    vPartage.nbCoureur = atoi(argv[4]);
    if(pthread_cond_init(&vPartage.cond,NULL)!=0){
        perror("erreur initialisation de la condition:");
        exit(1);
    }

    if(pthread_mutex_init(&vPartage.verrou,NULL)!=0){
        perror("erreur initialisation du verrou:");
        exit(1);
    }
    vPartage.di = malloc(atoi(argv[2])*atoi(argv[1])*sizeof(int));


    srand(atoi(argv[1]));  // initialisation de rand pour la simulation de longs calculs
    struct varPartageesTeam vPartageTeam[atoi(argv[2])];
    // Initialisation des équipes
    for(int i = 0;i < atoi(argv[2]);i++){
        vPartageTeam[i].cpt = 0;
        vPartageTeam[i].idTeam = i;
        if(pthread_cond_init(&vPartageTeam[i].cond,NULL)!=0){
                perror("erreur initialisation de la condition:");
                exit(1);
            }
        if(pthread_mutex_init(&vPartageTeam[i].verrou,NULL)!=0){
            perror("erreur initialisation du verrou:");
            exit(1);
        }
    }
    // Initialisation du premier tours
    for(int i =0;i<atoi(argv[2])*atoi(argv[1]);i++){
       if(i%4 == 0 | i%4 == 1){
        vPartage.di[i] = 1;
       }
       else{
        vPartage.di[i]=0;
       }
    } 
    for (int i = 0; i < atoi(argv[2])*atoi(argv[1]); i++){
        tabParams[i].idThread = i;
        tabParams[i].vPartage = &vPartage;
        tabParams[i].vPartageTeam = &(vPartageTeam[i/4]);        
        if (pthread_create(&threads[i], NULL,race,&tabParams[i]) != 0){
            perror("erreur creation thread");
            exit(1);
        }
    }
     // attente de la fin des  threards. Partie obligatoire 
    for (int i = 0; i < atoi(argv[1])*atoi(argv[2]); i++){
        if (pthread_join(threads[i],NULL) != 0){
            perror("erreur join");
            exit(1);
        }
    }
  printf("thread principal : fin de tous les threads secondaires\n");
  return 0;
}