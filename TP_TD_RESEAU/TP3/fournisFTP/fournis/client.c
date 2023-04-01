#include <stdio.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include<arpa/inet.h>
#include<string.h>
#include <sys/stat.h>

#define MAX_BUFFER_SIZE 146980

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

int main(int argc, char *argv[]) {

  if (argc != 4){
    printf("utilisation : client ip_serveur port_serveur nom_fichier\n Remarque : le parametre nom_fichier correspond au nom d'un fichier existant dans le répertoire emission\n");
    exit(0);
  }

  /* etape 1 : créer une socket */   
  int ds = socket(PF_INET, SOCK_STREAM, 0);
  if (ds == -1){
    perror("Client : pb creation socket :");
    exit(1);
  }
  printf("Client : creation de la socket réussie \n");


  
  /* etape 2 : designer la socket du serveur */
  int port = atoi(argv[2]);
  char * addrServeur = argv[1];
  struct sockaddr_in addrS;
  addrS.sin_family = AF_INET; 
  addrS.sin_addr.s_addr =  inet_addr(addrServeur);
  addrS.sin_port = htons((short)port);
  char* enter;
	fgets(enter, sizeof(enter), stdin);
  /* etape 3 : demander une connexion */
  int res_connect = connect(ds,&addrS,sizeof(addrS));
  if(res_connect==-1){
    perror("Erreur à la connexion: ");
    exit(1);
  }
  printf("Connexion Réussi\n");
 
  /* etape 4 : envoi de fichier : attention la question est générale. Il faut creuser pour définir un protocole d'échange entre le client et le serveur pour envoyer correctement un fichier */

  // int totalSent = 0; // variable pour compter le nombre total d'octet effectivement envoyés au serveur du début à la fin des échanges.


 
  /* le bout de code suivant est une lecture de contenu d'un fichier dont le nom est passé en paramètre.
     - pour lire un fichier, il faut l'ouvrir en mode lecture
     - la lecture se fait par blocs d'octets jusqu'à la fin du fichier.
     - la taille d'un bloc est définie par la constante MAX_BUFFER_SIZE que vous pouvez modifier.

     Le code est à compléter pour mettre en place l'envoi d'un fichier.
  */
  int taille_message = strlen(argv[3]);
  int res_send_taille_fichier = send(ds,&taille_message,sizeof(int),0);
  if(res_send_taille_fichier==-1 || res_send_taille_fichier == 0){
    perror("Erreur send taille du nom de fichier:");
    close(ds);
    exit(1);
  }
  char * nom_fichier = argv[3];
  int res_send_nom_fichier = send(ds,nom_fichier,strlen(nom_fichier),0);
  if(res_send_nom_fichier==-1 || res_send_nom_fichier == 0){
    perror("Erreur send du nom de fichier:");
    close(ds);
    exit(1);
  }
  

  // construction du nom du chemin vers le fichier
  char* filepath = malloc(strlen(argv[3]) + 16); // ./emission/+nom fichier +\0
  filepath[0] = '\0';
  strcat(filepath, "./emission/");
  strcat(filepath, argv[3]);

  printf("Client: je vais envoyer %s\n", filepath);

  // obtenir la taille du fichier
  struct stat attributes;
  if(stat(filepath, &attributes) == -1){
    perror("Client : erreur stat");
    free(filepath);
    exit(1);
  }

  long file_size = attributes.st_size; // cette copie est uniquement informer d'où obtenir la taille du fichier.
  
  printf("Client : taille du fichier : %ld octets\n", file_size);
  
  // lecture du contenu d'un fichier
  FILE* file = fopen(filepath, "rb");
  if(file == NULL){
    perror("Client : erreur ouverture fichier \n");
    free(filepath);
    exit(1);   
  }
  free(filepath);

  long total_lu = 0;
  void * buffer = malloc(MAX_BUFFER_SIZE*sizeof(void));
  int res_send_taille = send(ds,&file_size,sizeof(long),0);
    if(res_send_taille == 0 || res_send_taille == -1){
      perror("Erreur send taille du fichier:");
      close(ds);
      fclose(file);
      exit(1);
  }
  while(total_lu < file_size){
    long read = fread(buffer, sizeof(char), MAX_BUFFER_SIZE, file);
    if(read == 0){
      if(ferror(file) != 0){
        perror("Client : erreur lors de la lecture du fichier \n");
        fclose(file);
        exit(1);
      }else{
        printf("Client : arrivé a la fin du la lecture du fichier\n");// fin du fichier
	      break;
      }
    }
    printf("Client : j'ai lu un bloc du fichier de taille %ld \n",read);
    int res_send_taille_block = send(ds,&read,sizeof(long),0);
    if(res_send_taille_block == 0 || res_send_taille_block == -1){
      perror("Erreur send taille du block:");
      close(ds);
      fclose(file);
      exit(1);
    }
    int res_send = sendTCP(ds,buffer,read);
    if(res_send != 1){
      perror("Erreur send block:");
      close(ds);
      fclose(file);
      exit(1);
    } 
    total_lu += read;
  }

  // fermeture du fichier
  fclose(file); 
   
  printf("Client : j'ai lu au total : %ld octets \n", total_lu);  
 
 
  printf("Client : c'est fini\n");
}
