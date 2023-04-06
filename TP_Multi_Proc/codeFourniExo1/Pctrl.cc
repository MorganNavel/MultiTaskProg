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
        struct msgBuffer vMsgSend;
        if(msgrcv(f_id, &vMsgRecv,(size_t)sizeof(vMsgRecv.pid), 1, 0) == -1){
            perror("error msgrcv (premier rcv):");
            if (msgctl(f_id, IPC_RMID, NULL) == -1){
                perror("erreur suppression file de message :");
                exit(1);
            }
            cout << "suppression file ok" << endl;
            exit(1);
        }
        cout <<"Message reçu, traitement du processus de pid:"<<vMsgRecv.pid<<endl;
        //Repond "oui" ou "non" à cette demande
        vMsgSend.etiq = vMsgRecv.pid;
        if(msgsnd(f_id,&vMsgSend,(size_t)sizeof(vMsgSend.pid), 0)==-1){
            perror("error envoie de message:");
            if (msgctl(f_id, IPC_RMID, NULL) == -1){
                perror("erreur suppression file de message :");
                exit(1);
            }
            cout << "suppression file ok" << endl;
            exit(1);
        }
        cout<<"Partage de la ressource"<<endl;

        int pid = vMsgRecv.pid;
        //Si "Oui" il attend de recevoir une message de libération de la ressource
        if(msgrcv(f_id, &vMsgRecv,(size_t)sizeof(vMsgRecv.pid), pid, 0) == -1){
            perror("error msgrcv (deuxieme rcv):");
            if (msgctl(f_id, IPC_RMID, NULL) == -1){
                perror("erreur suppression file de message :");
                exit(1);
            }
            cout << "suppression file ok" << endl;
            exit(1);
        }
        cout<<"Fin du partage de la ressource"<<endl;
    }
    if (msgctl(f_id, IPC_RMID, NULL) == -1){
        perror("erreur suppression file de message :");
        exit(1);
    }
    cout << "suppression file ok" << endl;
    return 0;
}