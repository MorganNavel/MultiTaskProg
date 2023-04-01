#include <stdio.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include<arpa/inet.h>
#include<string.h>

/* Programme client */

int main(int argc, char *argv[]) {

  /* je passe en paramètre l'adresse de la socket du serveur (IP et
     numéro de port) et un numéro de port à donner à la socket créée plus loin.*/
   
  /* Je teste le passage de parametres. Le nombre et la nature des
     paramètres sont à adapter en fonction des besoins. Sans ces
     paramètres, l'exécution doit être arrétée, autrement, elle
     aboutira à des erreurs.*/
  if (argc != 3){
    printf("utilisation : %s ip_serveur port_serveur\n", argv[0]);
    exit(1);
  }

  /* Etape 1 : créer une socket */   
  int ds = socket(PF_INET, SOCK_DGRAM, 0);

  /* /!\ : Il est indispensable de tester les valeurs de retour de
     toutes les fonctions et agir en fonction des valeurs
     possibles. Voici un exemple */
  if (ds == -1){
    perror("Client : pb creation socket :");
    exit(1); // je choisis ici d'arrêter le programme car le reste
	     // dépendent de la réussite de la création de la socket.
  }
  
  /* J'ajoute des traces pour comprendre l'exécution et savoir
     localiser des éventuelles erreurs */
  printf("Client : creation de la socket réussie \n");
  
  // Je peux tester l'exécution de cette étape avant de passer à la
  // suite. Faire de même pour la suite : n'attendez pas de tout faire
  // avant de tester.
  
  /* Etape 2 : Nommer la socket du client */

  
  /* Etape 3 : Désigner la socket du serveur */
   int port = atoi(argv[2]);
   char * addrServeur = argv[1];
   // char * addrServeur = "127.0.0.1";
   printf("%d\n",port);
   printf("%s\n",addrServeur);
   // printf("%s %i",addrServeur,port);
   printf("Nommage de la socket du serveur\n");
   char  msg[100];
   struct sockaddr_in addrS;
   addrS.sin_family = AF_INET; 
   // addrS.sin_addr.s_addr =  inet_addr(addrServeur);
   inet_pton(AF_INET,argv[1],&(addrS.sin_addr));
   addrS.sin_port = htons((short)port);
   printf("Nommage Réussi\n");
   

  /* Etape 4 : envoyer un message au serveur  (voir sujet pour plus de détails)*/
  printf("Envoie d'un message en cours\n");
  printf("\nClient:");
  scanf("%s",(msg));
  
  int resEnvoie = sendto(ds,msg,strlen(msg)+1,0,&addrS,sizeof(addrS));
  if(resEnvoie==-1){
   perror("Error send");
   close(ds);
   exit(1);
  }
  printf("Envoie réussi il y a %ld octet(s) envoyé\n",resEnvoie);
  
  /* Etape 5 : recevoir un message du serveur (voir sujet pour plus de détails)*/
  printf("Réception en cours\n");
  int bufferRecv;
  //struct sockaddr *adrExp, // pointeur vers l’adresse
  struct sockaddr_in  addrExpInconnu;
  size_t lg = sizeof(struct sockaddr_in);
  int resRecev = recvfrom(ds,&bufferRecv,sizeof(bufferRecv),0,(struct sockaddr *)&addrExpInconnu,&lg);
  if(resRecev==-1){
   perror("Error recv");
   close(ds);
   exit(1);
  }
  printf("Reception Réussi\n");
  printf("Message recu de %s sur le port %d: %d",inet_ntoa(addrExpInconnu.sin_addr),ntohs(addrExpInconnu.sin_port),bufferRecv);
  
  /* Etape 6 : fermer la socket (lorsqu'elle n'est plus utilisée)*/
  
  close(ds);
  printf("\nClient : je termine\n");
  return 0;
}
