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
    //Processus permettant le contrôle d'accès 
    //à une ressource demandé par Pi
    if (argc!=3) {
        cout<<"Nbre d'args invalide, utilisation :"<< endl;
        cout << argv[0] << " fichier-pour-cle-ipc entier-pour-cle-ipc"<< endl;
        exit(0);
    }
    key_t cle=ftok(argv[1], atoi(argv[2]));
    if(cle==-1){
        perror("error ftok:");
        exit(1);
    }
    cout << "ftok ok :D"<<endl;
    
    int f_id = msgget(cle,IPC_CREAT|0666); //créer la file si n'existe pas
    if(f_id==-1){
        perror("error msgget");
        exit(1);
    }
    cout<<"Création de la file effectué"<<endl;
    //Reception d'une demande de d'accès
    while(1){
        struct msgBuffer vMsgRecv;
        if(msgrcv(f_id, &vMsgRecv,(size_t)sizeof(vMsgRecv.pid), 0, 0) == -1){
            perror("error msgrcv (premier rcv):");
            exit(1);
        }
        cout <<"message reçu"<<endl;
        cout<<"Proccessus:"<<vMsgRecv.texte<<endl;
        //Repond "oui" ou "non" à cette demande
        struct msgBuffer vMsgSend;
        vMsgSend.etiq = 2;
        if(msgsnd(f_id,&vMsgSend,(size_t)sizeof(vMsgSend.texte), vMsgRecv.pid)==-1){
            perror("error envoie de message:");
            exit(1);
        }
        cout<<"Envoie du message réussi"<<endl;

        int pid = vMsgRecv.pid;
        //Si "Oui" il attend de recevoir une message de libération de la ressource
        if(msgrcv(f_id, &vMsgRecv,(size_t)sizeof(vMsgRecv.pid), pid, 0) == -1){
            perror("error msgrcv (deuxieme rcv):");
            exit(1);
        }
        cout <<"message reçu"<<endl;
    }

    if (msgctl(f_id, IPC_RMID, NULL) == -1){
        perror("erreur suppression file de message :");
        exit(1);
    }
    cout << "suppression file ok" << endl;
    return 0;
}