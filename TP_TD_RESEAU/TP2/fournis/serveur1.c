#include <stdio.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include<arpa/inet.h>
#include<string.h>

/* Programme serveur */

int main(int argc, char *argv[]) {


  if (argc != 2){
    printf("utilisation : %s port_serveur\n", argv[0]);
    exit(1);
  }

  /* Etape 1 : créer une socket */   
  int ds = socket(PF_INET, SOCK_STREAM, 0);

  /* /!\ : Il est indispensable de tester les valeurs de retour de
     toutes les fonctions et agir en fonction des valeurs
     possibles. Voici un exemple */
  if (ds == -1){
    perror("Serveur : pb creation socket :");
    exit(1); // je choisis ici d'arrêter le programme car le reste
	     // dépendent de la réussite de la création de la socket.
  }
  
  /* J'ajoute des traces pour comprendre l'exécution et savoir
     localiser des éventuelles erreurs */
  printf("Serveur : creation de la socket réussie \n");
  
  // Je peux tester l'exécution de cette étape avant de passer à la
  // suite. Faire de même pour la suite : n'attendez pas de tout faire
  // avant de tester.
  
  /* Etape 2 : Nommer la socket du seveur */
  struct sockaddr_in sock_serveur;
  sock_serveur.sin_port = htons(atoi(argv[1]));
  sock_serveur.sin_addr.s_addr = INADDR_ANY;
  sock_serveur.sin_family = AF_INET;
  int res_bind = bind(ds,(struct sockaddr *)&sock_serveur,sizeof(sock_serveur));
  if(res_bind==-1){
    perror("Erreur de bind:");
    close(ds);
    exit(1);
  }

  int res_listen = listen(ds,7);
  if(res_listen == -1){
    perror("erreur de listen");
    close(ds);
    exit(1);
  }
  struct sockaddr_in sock_client;
  socklen_t lg = sizeof(struct sockaddr_in);

  int dsC = accept(ds,(struct sockaddr *)&sock_client,&lg);
  if(dsC==-1){
    perror("erreur d'accept");
    close(ds);
    exit(1);
  }
  if(dsC==0){
    perror("erreur accept = 0");
    close(ds);
    exit(1);
  }
  printf("Acceptation d'un client\n");


  /* Etape 4 : recevoir un message du client (voir sujet pour plus de détails)*/

  int taille_msg;

  int res_recv1 = recv(dsC,taille_msg,sizeof(taille_msg),0);
  if (res_recv1==-1){
    perror("Erreur de recv");
    close(ds);
    exit(1);
  }
  // printf("res_recv1: %d",res_recv1);
  if(res_recv1==0){
    perror("Erreur communication recv: ");
    close(dsC);
    close(ds);
    exit(1);
  }
  char msg[300];
  int res_recv = recv(dsC,msg,sizeof(char)*taille_msg,0);
  if (res_recv==-1){
    perror("Erreur de recv:");
    close(dsC);
    close(ds);
    exit(1);
  }
  if(res_recv==0){
    perror("Erreur communication: ");
    close(dsC);
    close(ds);
    exit(1);
  }
  printf("Client: %s\0\n",msg);
  
  /* Etape 5 : envoyer un message au serveur (voir sujet pour plus de détails)*/
  printf("Client: %d\0\n",res_recv);
  int res_send = send(dsC,res_recv,sizeof(res_recv),0);
  if(res_send==-1){
    perror("Erreur de send:");
    close(dsC);
    close(ds);

   exit(1);
  }
  if(res_send==0){
    perror("Erreur communication: ");
    close(dsC);
    close(ds);
    exit(1);
  }
  
  /* Etape 6 : fermer la socket (lorsqu'elle n'est plus utilisée)*/
  
  close(ds);
  printf("Serveur : je termine\n");
  return 0;
}
