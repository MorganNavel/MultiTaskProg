#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include "calcul.h"
#include <stdio.h>
#include <stdlib.h>

struct Player{
    int id;
    struct Team *team;
};
struct Team{
    int id;
    int idRunner;
    int playerCount;
    int water;
    pthread_mutex_t verrou;
    pthread_cond_t cond;
};

void * race(void * p){
    struct Player * player = (struct Player *) p;
    struct Team *team = player->team;
    printf("Joueur n°%d de l'équipe n°%d\n",player->id,team->id);
    while(team->idRunner!= player->id){
        printf("Joueur n°%d: Mis en attente de son tour\n",player->id);
        if(pthread_cond_wait(&(team->cond),&(team->verrou))!=0){
            perror("Erreur lors de l'attente:");
            exit(1);
        }
    }
    if(team->idRunner==player->id){ //celui qui cours c'est bien le bon joueur
        printf("Joueur n°%d: Début de son tour\n",player->id);
        calcul(1);
        printf("Joueur n°%d: Fin de son tour\n",player->id);
        if((team->idRunner+1)%(team->playerCount)==0 || ((team->idRunner)%(team->playerCount)==0 && team->idRunner!=team->id * team->playerCount)){
            team->idRunner = team->id * team->playerCount;
        }
        else{
            team->idRunner++;
        }
        printf("Joueur n°%d: Donne le relai au Joueur n°%d\n",player->id,team->idRunner);
        if(pthread_cond_broadcast(&(team->cond))!=0){
            perror("Erreur du diffusion d'évènement:");
            exit(1);
        }  
    }
    pthread_exit(NULL);
}

int main(int argc, char * argv[]){

    if (argc!=3) {
        printf(" argument requis\n");
        printf("./prog nb de joueur par equipe & nb équipe\n");
        exit(1);
    }
    srand(atoi(argv[1]));
    int playerCount = atoi(argv[1]);
    int teamCount = atoi(argv[2]);
    printf("Nombre de joueur: %d\nNombre d'équipe: %d\n",playerCount,teamCount);
    pthread_t threads[playerCount*teamCount];
    struct Team teams[teamCount];
    struct Player players[playerCount*teamCount];
    int t = 0;
    for(int p = 0;p<playerCount*teamCount;p++){
        struct Player *player = &(players[p]);
        player->id = p;
        if(p%playerCount==0 && p!=0){
            t++;
        }
        struct Team *team = &(teams[t]);
        player->team = team;
        team->id = t;
        team->idRunner = t*playerCount;
        team->playerCount=playerCount;
        team->water = 0;
        if(pthread_cond_init(&team->cond,NULL)!=0){
                perror("erreur initialisation de la condition:");
                exit(1);
        }
        if(pthread_mutex_init(&team->verrou,NULL)!=0){
            perror("erreur initialisation du verrou:");
            exit(1);
        }
        if (pthread_create(&threads[p], NULL,race,&players[p]) != 0){
            perror("erreur creation thread");
            exit(1);
        }
    }

    // attente de la fin des  threards. Partie obligatoire 
    for (int i = 0; i < playerCount*teamCount; i++){
        if (pthread_join(threads[i],NULL) != 0){
            perror("erreur join");
            exit(1);
        }
    }
  printf("thread principal : fin de tous les threads secondaires\n");
  return 0;
}