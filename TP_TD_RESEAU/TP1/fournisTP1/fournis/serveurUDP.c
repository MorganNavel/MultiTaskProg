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

  /* Je passe en paramètre le numéro de port qui sera donné à la socket créée plus loin.*/
   int port = atoi(argv[0]);
  /* Je teste le passage de parametres. Le nombre et la nature des
     paramètres sont à adapter en fonction des besoins. Sans ces
     paramètres, l'exécution doit être arrétée, autrement, elle
     aboutira à des erreurs.*/
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
     
   //162.38.84.63
   printf("Nommage de la socket serveur\n");
   struct sockaddr_in addrS;
   memset(&addrS, 0, sizeof(addrS));
   addrS.sin_family = AF_INET;
   addrS.sin_addr.s_addr =  INADDR_ANY;
   addrS.sin_port = htons((short)port);
   int resNommage = bind (ds,(struct sockaddr *)&addrS,sizeof(addrS));
   if(resNommage==-1){
      perror("Error bind");
      exit(1);
  }
  printf("Nommage réussi\n");
  printf("Port : %d\n",ntohs(ad.sin_port));
  /* Etape 4 : recevoir un message du client (voir sujet pour plus de détails)*/
   char bufferRecv[100];
   struct sockaddr_in  addrExpInconnu;
   size_t lg = sizeof(struct sockaddr_in);
   printf("Serveur en réception d'un message\n");
   
   int resRecev = recvfrom(ds,bufferRecv,sizeof(bufferRecv),0,(struct sockaddr *)&addrExpInconnu,&lg);
   if(resRecev==-1 || resRecev == 0){
      perror("Error recv");
      close(ds);
      exit(1);
   }
   printf("Reception réussi\n");
   printf("Message recu de %s sur le port %d: %s\n",inet_ntoa(addrExpInconnu.sin_addr),ntohs(addrExpInconnu.sin_port),bufferRecv);
  
  
  /* Etape 5 : envoyer un message au serveur (voir sujet pour plus de détails)*/
  
  /* Etape 6 : fermer la socket (lorsqu'elle n'est plus utilisée)*/
  
  
  printf("Serveur : je termine\n");
  close(ds);
  return 0;
}
/*recevoir un tableau d'entiers*/   
//   int msg[200];
//   int res = recvfrom(ds,msg,sizeof(msg),0,(struct sockaddr *)&addrExpInconnu,&lg);
//   if(res==-1){
//          perror("Error recv");
//          exit(1);
//    }
//    for(int i =0;i<sizeof(msg)/sizeof(int);i++){
//       printf("%i",msg[i]);
//    }
