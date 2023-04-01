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
  int ds = socket(PF_INET, SOCK_DGRAM, 0);

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

 
  /* Etape 4 : recevoir un message du client (voir sujet pour plus de détails)*/
  char msg[300];
  struct sockaddr_in socket_distante;
  size_t lgD = sizeof(socket_distante);
  int res_recv = recvfrom(ds,msg,sizeof(msg),0,(struct sockaddr *)&socket_distante,&lgD);
  if (res_recv==-1){
   perror("Erreur de recv:");
   close(ds);
   exit(1);
  }
  if(res_recv==0){
   perror("Erreur communication: ");
   close(ds);
   exit(1);
  }
  printf("Client: %s\n",msg);
  printf("Serveur:%d\n",res_recv);
  
  /* Etape 5 : envoyer un message au serveur (voir sujet pour plus de détails)*/
  socklen_t lg = sizeof(socket_distante); 
  // size_t lg = sizeof(socket_distante);
  int res_send = sendto(ds,res_recv,sizeof(int),0,(struct sockaddr *) &socket_distante,lg);
  if(res_send==-1){
   perror("Erreur de send ");
   close(ds);
   exit(1);
  }
  if(res_send==0){
   perror("Erreur communication: ");
   close(ds);
   exit(1);
  }
  
  /* Etape 6 : fermer la socket (lorsqu'elle n'est plus utilisée)*/
  
  close(ds);
  printf("Serveur : je termine\n");
  return 0;
}

