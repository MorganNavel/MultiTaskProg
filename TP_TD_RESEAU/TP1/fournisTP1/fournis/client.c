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
    	printf("utilisation : %s ip_serveur port_serveur port_client\n", argv[0]);
    	exit(1);
  	}

  	/* Etape 1 : créer une socket */  

  	printf("Serveur : Creation de la socket : ");

  	int ds = socket(PF_INET, SOCK_DGRAM, 0);

  	if (ds == -1){
		perror("Echec\n");
    	exit(1);
  	} else printf("Réussi\n");
  
  	/* Etape 2 : Nommer la socket du client */

  	// printf("Serveur : Nommage de la socket : ");

	// struct sockaddr_in ad;
	// ad.sin_family = AF_INET;
	// ad.sin_addr.s_addr = INADDR_ANY;
	// ad.sin_port = htons((short)argv[3]);
  	// int bind_res = bind(ds, (struct sockaddr*)&ad, sizeof(ad));

	// if (bind_res == -1) {
	// 	perror("Echec\n");
	// 	exit(1);
	// } else printf("Réussi\n");
  
  	/* Etape 3 : Désigner la socket du serveur */

  	printf("Serveur : Creation de la socket serveur : ");

  	int dsDistante = socket(PF_INET, SOCK_DGRAM, 0);

  	if (dsDistante == -1){
		perror("Echec\n");
    	exit(1);
  	} else printf("Réussi\n");

  	printf("Serveur : Nommage de la socket serveur ");

	struct sockaddr_in sockDistante;
	sockDistante.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &(sockDistante.sin_addr));
	sockDistante.sin_port = htons((short)atoi(argv[2]));

	printf("Réussi\n");
  
  	/* Etape 4 : envoyer un message au serveur  (voir sujet pour plus de détails)*/

  	printf("Serveur : Envoie d'un message : ");

	socklen_t lgAdr = sizeof(struct sockaddr_in) ;
	char msg[100];
	fgets(msg, sizeof(msg), stdin);
	ssize_t send_res = sendto(ds, msg, strlen(msg)+1, 0, (struct sockaddr *)&sockDistante, lgAdr);

	if (send_res == -1) {
		perror("Echec\n");
		exit(1);
	} else printf("Réussi : %ld octets envoyé\n", send_res);
  
  	/* Etape 5 : recevoir un message du serveur (voir sujet pour plus de détails)*/

  	printf("Serveur : Reception d'un message \n");

	char message[100];
	ssize_t recv_res = recvfrom(ds, message, sizeof(message), 0, (struct sockaddr *)&sockDistante, &lgAdr);

	if (recv_res == -1) {
		perror("--> Echec\n");
		exit(1);
	} else printf("--> Réussi : %ld octets recu : %s\n", recv_res, message);
  
  	/* Etape 6 : fermer la socket (lorsqu'elle n'est plus utilisée)*/
  
  	printf("Serveur : Fermeture de la socket client : ");

	int close_res_client = close(ds);

	if (close_res_client == -1) {
		perror("Echec\n");
		exit(1);
	} else printf("Réussi\n");
  
  	printf("Serveur : Fermeture de la socket serveur : ");

	int close_res_serveur = close(dsDistante);

	if (close_res_serveur == -1) {
		perror("Echec\n");
		exit(1);
	} else printf("Réussi\n");
  
  	printf("Client : je termine\n");
  	return 0;
}
