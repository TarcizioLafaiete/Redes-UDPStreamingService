#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../include/socketUtils.h"
#include "../include/clientCore.h"
#include "../include/terminalPrinter.h"



clientCore initClient(char* argv[]){
    clientCore core;

    core.inet = translateIP(argv[1]);

    create_socket(&core.client_fd,core.inet);
    core.serverSocket = configure_addr(core.inet,atoi(argv[3]));
    
    connect_client(&core);
    printf("connect socket \n");

    return core;
}

int main(int argc, char* argv[]){

    printf("Iniciando esta merda \n");

    clientCore core = initClient(argv);

    datagram msg = {.id = 1, .timeStamp = (int)time(NULL), .buffer = "Hello Server \n"};

    printf("Sending datagram \n");
    sendto(core.client_fd,&msg,500,0,(struct sockaddr*)NULL,sizeof(core.serverSocket.addr));
    printf("Datagrama enviado \n");

    datagram ret;
    recvfrom(core.client_fd,&ret,sizeof(ret),0,(struct sockaddr*)NULL,NULL);

    printf("id: %d timestamp: %d buffer: %s \n",ret.id,ret.timeStamp,ret.buffer);

    return 0;
}

