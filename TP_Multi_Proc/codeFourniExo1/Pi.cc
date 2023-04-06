#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>//perror
#include <cstring>
 #include <unistd.h>
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
    cout<<"Demande d'accès d'une ressource du PID: "<<getpid()<<endl;
    if(msgsnd(f_id,&vMsgSend,sizeof(vMsgSend.pid), 0)==-1){
        perror("error envoie de message:");
        exit(1);
    }
    cout<<"Envoie de demande d'accès réussi"<<endl;
    cout<<"Attente d'un message ayant pour étiquette "<<vMsgSend.pid<<endl;
    if(msgrcv(f_id, &vMsgRecv,(size_t)sizeof(vMsgRecv.pid), (int)vMsgSend.pid, 0) == -1){
        perror("error msgrcv:");
        exit(1);
    }
    cout <<"Recepetion d'un message contenant le PID "<<vMsgRecv.etiq<<endl;
    cout <<"Accès Autorisé"<<endl;
    sleep(5);
    vMsgSend.etiq = getpid();
    if(msgsnd(f_id,&vMsgSend,sizeof(vMsgSend.pid), 0)==-1){
        perror("error envoie de message:");
        exit(1);
    }
    cout<<"Libération de la ressource"<<endl;

    return 0;
}