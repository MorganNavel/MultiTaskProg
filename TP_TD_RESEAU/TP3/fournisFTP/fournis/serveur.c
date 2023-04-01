#include <stdio.h>//perror
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>//close
#include <stdlib.h>
#include <string.h>
#include "fonction_TCP.h"

#define MAX_BUFFER_SIZE 146980



int main(int argc, char *argv[])
{
  /* etape 0 : gestion des paramètres si vous souhaitez en passer */
  if (argc != 2){
    printf("utilisation : %s port_serveur\n", argv[0]);
    exit(1);
  }
  /* etape 1 : creer une socket d'écoute des demandes de connexions*/
  int ds = socket(PF_INET, SOCK_STREAM, 0);

  if (ds == -1){
	  perror("Echec\n");
	  exit(1);
  } else printf("Réussi\n");
  /* etape 2 : nommage de la socket */
 	printf("Serveur : Nommage de la socket : ");
	struct sockaddr_in adrClient;
	memset(&adrClient, 0, sizeof(adrClient));
	adrClient.sin_family = AF_INET;
	adrClient.sin_addr.s_addr = INADDR_ANY;
	adrClient.sin_port = htons((short)atoi(argv[1]));
  int bind_res = bind(ds, (struct sockaddr*)&adrClient, sizeof(adrClient));
 	socklen_t lgAdr = sizeof(struct sockaddr_in);

  if (bind_res == -1 ) {
		perror("Echec\n");
		exit(1);
	} else printf("Réussi : ip : %d\n", ntohs(adrClient.sin_port));
  printf("Descripteur du serveur: %d\n",ds);
  /* etape 3 : mise en ecoute des demandes de connexions */
  int res = listen(ds,7);
  if(res==-1){
    perror("Erreur de listen:");
    close(ds);
    exit(1);
  }
  while(1){
    /* etape 4 : plus qu'a attendre la demadne d'un client */
    int dsC = accept(ds,(struct sockaddr *)&adrClient,&lgAdr);
    if(dsC==-1){
        perror("Erreur de Accept: ");
        close(ds);
        exit(1);
    }
    int pid = fork();
    if(pid==0){
      close(ds);
      int totalRecv = 0; // un compteur du nombre total d'octets recus d'un client

      /* le protocol d'echange avec un client pour recevoir un fichier est à définir. Ici seul un exemple de code pour l'écriture dans un fichier est founi*/
      int taille_nom_fichier;
      int res_recv_taille_nom_fichier = recv(dsC,&taille_nom_fichier,sizeof(int),0);
      if(res_recv_taille_nom_fichier == -1 || res_recv_taille_nom_fichier == 0){
        perror("Erreur recv de la taille du nom du fichier:");
        close(dsC);
        close(ds);
        exit(1);
      }
      char * nom_fichier = malloc(sizeof(char)*taille_nom_fichier);
      long octet_recv = 0;
      while(octet_recv<taille_nom_fichier){
        int res_recv = recv(dsC,nom_fichier+octet_recv,taille_nom_fichier-octet_recv,0);
        if(res_recv == -1 || res_recv==0){
          perror("Erreur recv message:");
          close(dsC);
          close(ds);
          exit(1);
        }
        octet_recv+=res_recv;
      }
      printf("nom fichier: %s\n",nom_fichier);
      char* filepath = malloc(strlen("./reception/") + taille_nom_fichier); // ./emission/+nom fichier +\0
      // filepath[0] = '\0';
      strcat(filepath, "./reception/");
      strcat(filepath, nom_fichier);
      printf("filepath: %s\n",filepath);
      // char * filepath = ."/reception/fichier.txt";
      // On ouvre le fichier dans lequel on va écrire
      FILE* file = fopen(filepath, "wb");
      if(file == NULL){
        perror("Serveur : erreur ouverture fichier: \n");
        exit(1);  
      }

      long size_file;
      int res_taille_fichier = recv(dsC,&size_file,sizeof(long),0);
        if(res_taille_fichier == -1 || res_taille_fichier==0){
          perror("Erreur recv contenu du fichier:");
          close(dsC);
          close(ds);
          exit(1);
      }  
      void * buffer = malloc(MAX_BUFFER_SIZE*sizeof(void));
      malloc(MAX_BUFFER_SIZE*sizeof(void));
      octet_recv = 0;
      printf("size_file: %d\n",size_file);

      //RECEPTION DU FICHIER
      while(octet_recv<size_file){
        long block_size;
        //RECEPTION DE LA TAILLE DU BLOCK
        int res_taille_block = recv(dsC,&block_size,sizeof(long),0);
        if(res_taille_block == -1 || res_taille_block==0){
          perror("Erreur recv contenu du fichier:");
          close(dsC);
          close(ds);
          exit(1);
        }
        printf("octer recv: %d\n",octet_recv);
        printf("buffer: %d\nblock_size: %d\n",strlen(buffer),block_size);
        //ECRITURE DANS LE NOUVEAU FICHIER SI BUFFER PLEIN
        if(strlen(buffer)+block_size >= sizeof(buffer) && strlen(buffer)>0){ //si le buffer va être surcharger alors j'écris les caractères afin de libérer le buffer
          printf("Descripteur de fichier: %d\n",file);
          printf("taille du buffer avant écriture: %d\n",strlen(buffer));

          size_t written = fwrite(buffer, sizeof(void), strlen(buffer), file);
          if(written < strlen(buffer)){  // cette ligne est valide uniquement pour ce simple exemple
            perror("Serveur : Erreur a l'ecriture du fichier \n");
            fclose(file); 
          }
          memset(buffer,0,MAX_BUFFER_SIZE*sizeof(void));
          printf("taille du buffer après écriture: %d\n",strlen(buffer));
          printf("Ecriture partiel du fichier réussi\n");
        }
        printf("taille du buffer: %d\nOctet qu'il reste à lire: %d\n",strlen(buffer),size_file-octet_recv);
        long octet_block_lu=0;
        //RECEPTION DES BLOCKS
        while(octet_block_lu<block_size){
          printf("Nombre d'octet du block lu: %ld\n",octet_block_lu);
          int res_recv_fichier = recv(dsC,buffer+strlen(buffer),block_size-octet_block_lu,0);
          printf("Retour de recv: %d\n",res_recv_fichier);
          if(res_recv_fichier == -1 || res_recv_fichier==0){
            perror("Erreur recv contenu du fichier:\n");
            close(dsC);
            close(ds);
            exit(1);
          }
          octet_block_lu+=res_recv_fichier;
        }
        octet_recv+=octet_block_lu;
        printf("J'ai reçu %ld octets du block\n",octet_recv);
      }
      //ECRITURE SI BUFFER EST ENCORE REMPLIE
      // char * buffer ="ceci est un exemple de contenu a ecrire dans un fichier\n";
      if(strlen(buffer)>0){
        size_t written = fwrite(buffer, sizeof(void), strlen(buffer), file);
        if(written < strlen(buffer)){  // cette ligne est valide uniquement pour ce simple exemple
          perror("Serveur : Erreur a l'ecriture du fichier \n");
          fclose(file); 
        }
      }

      printf("Serveur : ecriture dans fichier reussie. Vous pouvez vérifier la création du fichier et son contenu.\n");
      // fermeture du fichier
      fclose(file);
      close(dsC);
      int status;
      wait(&status);
    }
    close(dsC);
  }
  close(ds);
  printf("Serveur : c'est fini\n");
}








