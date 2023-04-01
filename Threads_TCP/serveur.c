#include <stdio.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include<arpa/inet.h>
#include<string.h>

/* Programme serveur */

int recvTCP(int sock,void * msg,int size){
   int octet_recv = 0;
   while(octet_recv<size){
      int res_recv = recv(sock,msg+octet_recv,size-octet_recv,0);
      if(res_recv == -1){
         return -1;
      }
      if(res_recv == 0){
         return 0;
      }
      octet_recv+=res_recv;
   }
   return 1;
}

int sendTCP(int sock,void * msg,int size){
   int octet_send = 0;
   while(octet_send<size){
      int res_send = send(sock,msg,size,0);
      if(res_send == -1){
         return -1;
      }
      if(res_send == 0){
         return 0;
      }
      octet_send+=res_send;
   }
   return 1;
}
struct params {
   int id;
   int socket;
};
void * discussion(void * p){
   struct params * params = (struct params *) p;
   printf("Naissance du thread n°%d\n",params->id);
   printf("Socket n°%d: Connexion établit début de la conversation\n",params->socket);
   
   int taille_recv;
   int res_taille_recv = recv(params->socket,&taille_recv,sizeof(taille_recv),0);
   if(res_taille_recv==-1){
      perror("Erreur de recv");
      close(params->socket);
      pthread_exit(NULL); 

   }
   printf("Client: %d\n",taille_recv);

   char * buffer_recv;
   int res_recv1 = recvTCP(params->socket,buffer_recv,taille_recv);
   if(res_recv1==-1){
      perror("Error recv");
      close(params->socket);
      pthread_exit(NULL);
   }
   if(res_recv1==0){
      perror("Error recv");
      close(params->socket);
      pthread_exit(NULL);
   }
   if(res_recv1==1){
      printf("\nReception réussi\n");
      printf("Serveur: %d\n",taille_recv);
   }
   printf("Client: %s",buffer_recv);
   // strcat(buffer_recv, "\0");
   /* Etape 5 : envoyer un message au serveur (voir sujet pour plus de détails)*/
   int buffer_send = taille_recv;
   int res_send = send(dsC,&buffer_send,sizeof(buffer_send),0);
   if (res_send==-1){
      perror("Erreur de send:");
      close(dsC);
      pthread_exit(NULL);
   }

   /* Etape 6 : fermer la socket (lorsqu'elle n'est plus utilisée)*/
   close(dsC);
   pthread_exit(NULL); 
}
int main(int argc, char *argv[]) {

   /* Je passe en paramètre le numéro de port qui sera donné à la socket créée plus loin.*/
   int port = atoi(argv[1]);
   /* Je teste le passage de parametres. Le nombre et la nature des
     paramètres sont à adapter en fonction des besoins. Sans ces
     paramètres, l'exécution doit être arrétée, autrement, elle
     aboutira à des erreurs.*/
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
     
   // 162.38.84.63
   struct sockaddr_in addrS;
   addrS.sin_family = AF_INET;
   addrS.sin_addr.s_addr =  INADDR_ANY;
   addrS.sin_port = htons((short)port);

   int res_bind = bind (ds,(struct sockaddr *)&addrS,sizeof(addrS));
   if(res_bind==-1){
      perror("Error rename");
      close(ds);
      exit(1);
   }
   /* Etape 4 : recevoir un message du client (voir sujet pour plus de détails)*/
   struct sockaddr_in adrClient;
   size_t lg = sizeof(adrClient);
   int res = listen(ds,1);
   if(res==-1){
      perror("Erreur de listen:");
      close(ds);
      exit(1);
   }
   while(1){
      int dsC = accept(ds,(struct sockaddr *)&adrClient,&lg);
      if(dsC==-1){
         perror("Erreur de Accept: ");
         close(ds);
         exit(1);
      }
      // Création du thread
      
   }
   printf("Serveur : je termine\n");
  close(ds);
  return 0;
}



