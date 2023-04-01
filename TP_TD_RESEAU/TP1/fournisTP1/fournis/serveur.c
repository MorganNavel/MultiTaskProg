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

  	printf("Serveur : Creation de la socket : ");

  	/* Etape 1 : créer une socket */   
  	int ds = socket(PF_INET, SOCK_DGRAM, 0);

  	if (ds == -1){
		perror("Echec\n");
		exit(1);
  	} else printf("Réussi\n");
  
  	/* Etape 2 : Nommer la socket du seveur */

  	printf("Serveur : Nommage de la socket : ");

	struct sockaddr_in ad;
	memset(&ad, 0, sizeof(ad));
	ad.sin_family = AF_INET;
	ad.sin_addr.s_addr = INADDR_ANY;
	ad.sin_port = htons((short)atoi(argv[1]));
  	int bind_res = bind(ds, (struct sockaddr*)&ad, sizeof(ad));

	socklen_t lgAdr = sizeof(struct sockaddr_in);

	if (bind_res == -1 ) {
		perror("Echec\n");
		exit(1);
	} else printf("Réussi : ip : %d\n", ntohs(ad.sin_port));
 
  	/* Etape 4 : recevoir un message du client (voir sujet pour plus de détails)*/

  	printf("Serveur : Reception d'un message \n");

	struct sockaddr_in sockClient;
	char message[100];
	ssize_t recv_res = recvfrom(ds, message, sizeof(message), 0, (struct sockaddr*)&sockClient, &lgAdr);

	if (recv_res == -1) {
		perror("--> Echec\n");
		exit(1);
	} else printf("--> Réussi : %ld octets recu : %s\n", recv_res, message);

	printf("ip : %s, port : %d\n", inet_ntoa(sockClient.sin_addr), ntohs(sockClient.sin_port));
  
  	/* Etape 5 : envoyer un message au serveur (voir sujet pour plus de détails)*/

  	printf("Serveur : Envoie d'un message : ");

	char msg[100];
	snprintf(msg, sizeof(msg), "J'ai bien recu un message de longueur %ld octets", recv_res);
	ssize_t send_res = sendto(ds, msg, strlen(msg)+1, 0, (struct sockaddr *)&sockClient, lgAdr);

	if (send_res == -1) {
		perror("Echec\n");
		exit(1);
	} else printf("Réussi : %ld octets envoyé\n", send_res);
  
  	/* Etape 6 : fermer la socket (lorsqu'elle n'est plus utilisée)*/
  
  	printf("Serveur : Fermeture de la socket : ");

	int close_res = close(ds);

	if (close_res == -1) {
		perror("Echec\n");
		exit(1);
	} else printf("Réussi\n");

  	printf("Serveur : je termine\n");
  	return 0;
}
