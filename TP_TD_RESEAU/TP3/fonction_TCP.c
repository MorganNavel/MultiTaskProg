#include "fonction_TCP.h"


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
      int res_recv = recv(sock,msg,size-octet_recv,0);
      if(res_recv == -1){
         return -1;
      }
      if(res_recv == 0){
         return 0;
      }
      octet_recv+=res_recv;
      // printf("Nombre d'octet reçu: %d\n",octet_recv);
   }
   return 1;
}