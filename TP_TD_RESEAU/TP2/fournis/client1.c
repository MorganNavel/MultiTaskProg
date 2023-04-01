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

  if (argc != 3){
       printf("utilisation : %s ip_serveur port_serveur\n", argv[0]);
    exit(1);
  }
  /* Etape 1 : créer une socket */   
  int ds = socket(PF_INET, SOCK_STREAM, 0);

  if (ds == -1){
       perror("Client : pb creation socket :");
    exit(1);
  }

  printf("Client : creation de la socket réussie \n");

  /* Etape 2 : Nommer la socket du client */

  /* Etape 3 : Désigner la socket du serveur */
  printf("adresse: %s\nport: %s\n",argv[2],argv[1]);

   struct sockaddr_in sock_serveur;
   sock_serveur.sin_family=AF_INET;
   sock_serveur.sin_port = htons(atoi(argv[1]));
   // inet_pton(AF_INET,argv[2],&(sock_serveur.sin_addr)); 
   sock_serveur.sin_addr.s_addr = inet_addr(argv[2]);

   socklen_t lg = sizeof(struct sockaddr_in);

   int res_connect = connect(ds,(struct sockaddr *) &sock_serveur,lg);
   if(res_connect==-1){
      perror("Erreur de connect");
      close(ds);
      exit(1);
   }   
  /* Etape 4 : envoyer un message au serveur  (voir sujet pour plus de détails)*/
  char msg[200];
  printf("Client: ");
  scanf("%s",msg);
  strcat(msg,"\0");
  //192.168.1.15

   int res_send1 = send(ds,strlen(msg)+1,sizeof(int),0);
   if(res_send1==-1){
      perror("Erreur de send");
      close(ds);
      exit(1);
   }
   if(res_send1==0){
      perror("Erreur de send la commnication est coupé");
      close(ds);
      exit(1);
   }
  int res_send = send(ds,msg,strlen(msg)+1,0);
  if(res_send==-1){
   perror("Erreur de send");
   close(ds);
   exit(1);
  }
  if(res_send==0){
   perror("Erreur de send la commnication est coupé");
   close(ds);
   exit(1);
  }


  /* Etape 5 : recevoir un message du serveur (voir sujet pour plus de détails)*/
   int buffer_recv;
   int res_recv = recv(ds,buffer_recv,sizeof(buffer_recv),0);
   if (res_recv==-1){
      perror("Erreur de recv");
      close(ds);
      exit(1);
   }   
   if(res_recv==0){
      perror("Erreur communication ");
      close(ds);
      exit(1);
   }
   printf("Client: %d\n",buffer_recv);

  /* Etape 6 : fermer la socket (lorsqu'elle n'est plus utilisée)*/


  printf("Client : je termine\n");
  return 0;
}
