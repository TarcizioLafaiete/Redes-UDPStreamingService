#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "../include/socketUtils.h"
#include "../include/serverCore.h"
#include "../include/terminalPrinter.h"
#include "../include/queue.h"

typedef struct{
    serverCore core;
    struct sockaddr cliaddr;
    int cliaddr_len;
    datagram data;
} serverClojure;

int client_id = 0;

serverCore initServer(char* argv[]){
    serverCore core;

    core.inet = translateIP(argv[1]);

    create_socket(&core.server_fd,core.inet);
    core.socket = configure_addr(core.inet,atoi(argv[2]));
    configure_options(core.server_fd);
    binding(&core);
    return core;
}

void receiveDatagram(int socket,serverClojure* clojure){
    struct sockaddr cliaddr;
    int len = sizeof(cliaddr);
    recvfrom(socket,&clojure->data,sizeof(clojure->data),0,&clojure->cliaddr,&clojure->cliaddr_len);
}

void sendDatagram(int socket, serverClojure clojure){

    sendto(socket,&clojure.data,MAX_MESSAGE_SIZE,0,(struct sockaddr*)&clojure.cliaddr,sizeof(clojure.cliaddr));

}

void* clientHandler(void *arg){

    serverClojure* clojure = (serverClojure*)arg;

    clojure->data.id = client_id;
    sendDatagram(clojure->core.server_fd,*clojure);

    int sequence = 0;
    while(sequence <= 5){
        sequence++;
        clojure->data.sequence = sequence;
        memcpy(clojure->data.buffer,"Phrase",MAX_MESSAGE_SIZE);
        sendDatagram(clojure->core.server_fd,*clojure);
        sleep(3);
    }

}

int main(int argc,char* argv[]){

    serverClojure clojure;
    clojure.core = initServer(argv);
    S_Queue* threadQueue = create_queue(sizeof(pthread_t));

    while(1){
        receiveDatagram(clojure.core.server_fd,&clojure);
        if(clojure.data.startConnection){
            printf("conexao funcionou \n");
            client_id++;
            pthread_t newConnection;
            serverClojure auxClojure = clojure;
            pthread_create(&newConnection,NULL,clientHandler,&auxClojure);
        }
    }

    return 0;
}