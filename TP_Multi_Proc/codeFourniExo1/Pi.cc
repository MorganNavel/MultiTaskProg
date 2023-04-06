#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>//perror
#include <cstring>
using namespace std;
struct msgBuffer{
    long etiq;
    int pid;
};
int main(int argc, char * argv[]){
    if (argc!=3) {
        cout<<"Nbre d'args invalide, utilisation :"<< endl;
        cout << argv[0] << " fichier-pour-cle-ipc entier-pour-cle-ipc"<< endl;
        exit(0);
    }
	  
    key_t cle=ftok(argv[1], atoi(argv[2]));

    if (cle==-1) {
        perror("Erreur ftok : ");
        exit(2);
    }

    int f_id = msgget(cle,IPC_CREAT|0666); //créer la file si n'existe pas
    if(f_id==-1){
        perror("error msgget");
        exit(1);
    }
    cout<<"Création de la file effectué"<<endl;

    struct msgBuffer vMsgSend;
    struct msgBuffer vMsgRecv;
    vMsgSend.etiq = 1;
    vMsgSend.pid = getpid();
    cout<<"Envoyez un message"<<endl;

    if(msgsnd(f_id,&vMsgSend,sizeof(vMsgSend.pid), 0)==-1){
        perror("error envoie de message:");
        exit(1);
    }
    cout<<"Envoie du message réussi"<<endl;

    if(msgrcv(f_id, &vMsgRecv,(size_t)sizeof(vMsgRecv.texte), 0, 0) == -1){
        perror("error msgrcv:");
        exit(1);
    }
    cout <<"message reçu"<<endl;
    cout<<"Controlleur: "<<vMsgRecv.texte<<endl;

    strcpy(vMsgSend.texte,"libération");
    if(msgsnd(f_id,&vMsgSend,sizeof(vMsgSend.texte), 0)==-1){
        perror("error envoie de message:");
        exit(1);
    }
    return 0;
}