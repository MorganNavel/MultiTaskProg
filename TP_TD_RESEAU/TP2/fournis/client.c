#include <stdio.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

/* Programme client */
int sendTCP(int sock,void * msg,int size){
   // printf("%s",msg);
   int octet_send = 0;
   while(octet_send<size){
      int res_send = send(sock,msg+octet_send,size-octet_send,0);
      if(res_send <= 0){
         return res_send;
      }
      octet_send+=res_send;
   }
   return 1;
}
int recvTCP(int sock,char * msg,int size){
   int octet_recv = 0;
   while(octet_recv<size){
      int res_recv = recv(sock,msg,size-octet_recv,0);
      if(res_recv == -1){
         return -1;
      }
      if(res_recv == 0){
         return 0;
      }
      octet_recv+=res_recv;
      printf("Nombre d'octet reçu: %d\n",octet_recv);
   }
   return 1;
}

int main(int argc, char *argv[]) {

  /* je passe en paramètre l'adresse de la socket du serveur (IP et
     numéro de port) et un numéro de port à donner à la socket créée plus loin.*/
   
  /* Je teste le passage de parametres. Le nombre et la nature des
     paramètres sont à adapter en fonction des besoins. Sans ces
     paramètres, l'exécution doit être arrétée, autrement, elle
     aboutira à des erreurs.*/
  if (argc != 4){
    printf("utilisation : %s ip_serveur port_serveur nb_boucle\n", argv[0]);
    exit(1);
  }

  /* Etape 1 : créer une socket */   
  int ds = socket(PF_INET, SOCK_STREAM, 0);

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
   char  msg[200];
   struct sockaddr_in addrS;
   addrS.sin_family = AF_INET; 
   addrS.sin_addr.s_addr =  inet_addr(addrServeur);
   addrS.sin_port = htons((short)port);
   // int pid = fork();
  /* Etape 4 : envoyer un message au serveur  (voir sujet pour plus de détails)*/
   printf("\nClient:");
   scanf("%s",msg);
   msg[strlen(msg)]="\0";

   int taille_message = (strlen(msg))*atoi(argv[3]);
   printf("nb d'octet supposement envoyé: %i\n",taille_message);

   int res_connect = connect(ds,&addrS,sizeof(addrS));
   if(res_connect==-1){
      perror("Erreur à la connexion: ");
      exit(1);
   }
   
   int res_send = send(ds,&taille_message,sizeof(taille_message),0);
   if(res_send==-1){
      perror("Error send");
      close(ds);
      exit(1);
   }
   
   int i = 0;
   int nb_octet_send = 0;
   while(i<atoi(argv[3])){      
      // res_send = send(ds,msg,strlen(msg),0);
      // if(res_send==-1){
      //    perror("Error send");
      //    close(ds);
      //    exit(1);
      // }
      // if(res_send==0){
      //    perror("Error send");
      //    close(ds);
      //    exit(1);
      // }
      int res_send1 = sendTCP(ds,msg,strlen(msg));
      if(res_send1==-1){
         perror("Error send");
         close(ds);
         exit(1);
      }
      if(res_send1==0){
         perror("Error send");
         close(ds);
         exit(1);
      }
      if(res_send1==1){
         printf("Envoie réussi\n");
      }
      printf("Appel à send %d\n",i);
      i++;
   }
   // printf("Nombre d'octet effectivement envoyé: %d\n",nb_octet_send);
   // int res_send1 = sendTCP(ds,msg,taille_message);
   // if(res_send1==-1){
   //    perror("Error send");
   //    close(ds);
   //    exit(1);
   // }
   // if(res_send1==0){
   //    perror("Error send");
   //    close(ds);
   //    exit(1);
   // }
   // if(res_send1==1){
   //    printf("Envoie réussi\n");

   // }
  //   162.38.84.63
  /* Etape 5 : recevoir un message du serveur (voir sujet pour plus de détails)*/
  int buffer_recv;
  int res_recv = recv(ds,&buffer_recv,sizeof(buffer_recv),0);

  if(res_recv==-1){
    perror("Erreur de recv");
    close(ds);
    exit(1);
  }
  printf("Serveur: %d\n",buffer_recv);
  
  /* Etape 6 : fermer la socket (lorsqu'elle n'est plus utilisée)*/
  
  close(ds);
  printf("\nClient : je termine\n");
  return 0;
}

