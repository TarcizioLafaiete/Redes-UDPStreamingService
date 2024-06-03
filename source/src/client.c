#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../include/socketUtils.h"
#include "../include/clientCore.h"
#include "../include/terminalPrinter.h"

typedef struct{
    socket_address socket;
    datagram data;
} clientClojure;

clientCore initClient(char* argv[]){
    clientCore core;

    core.inet = translateIP(argv[1]);

    create_socket(&core.client_fd,core.inet);
    core.serverSocket = configure_addr(core.inet,atoi(argv[3]));
    
    connect_client(&core);
    printf("connect socket \n");

    return core;
}

void sendDatagram(clientCore core,datagram data){

    if(core.inet.type == IPV4){
        sendto(core.client_fd,&data,MAX_MESSAGE_SIZE,0,(struct sockaddr*)NULL,sizeof(core.serverSocket.addr));
    }
    else if(core.inet.type == IPV6){
        sendto(core.client_fd,&data,MAX_MESSAGE_SIZE,0,(struct sockaddr*)NULL,sizeof(core.serverSocket.addr6));
    }

}

datagram receiveDatagram(int socket){
    
    datagram data;
    recvfrom(socket,&data,sizeof(data),0,(struct sockaddr*)NULL,NULL);
    return data;

}

int main(int argc, char* argv[]){

    printf("Iniciando esta merda \n");

    clientCore core = initClient(argv);

    datagram msg = {.id = 1, .timeStamp = (int)time(NULL), .buffer = "Hello Server \n"};

    printf("Sending datagram \n");
    sendDatagram(core,msg);
    printf("Datagrama enviado \n");

    datagram ret = receiveDatagram(core.client_fd);

    printf("id: %d timestamp: %d buffer: %s \n",ret.id,ret.timeStamp,ret.buffer);

    return 0;
}

