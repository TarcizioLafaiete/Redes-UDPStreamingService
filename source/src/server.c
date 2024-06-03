#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../include/socketUtils.h"
#include "../include/serverCore.h"
#include "../include/terminalPrinter.h"


serverCore initServer(char* argv[]){
    serverCore core;

    core.inet = translateIP(argv[1]);

    printf("IP Translated \n");
    create_socket(&core.server_fd,core.inet);
    printf("create socket \n");
    core.socket = configure_addr(core.inet,atoi(argv[2]));
    printf("configure addr \n");
    configure_options(core.server_fd);
    printf("configure addr \n");
    binding(&core);
    printf("binding \n");
    return core;
}

int main(int argc,char* argv[]){

    printf("Iniciando essa merda \n");

    serverCore core = initServer(argv);

    printf("Esperando datagrama \n");
    datagram rec;
    struct sockaddr cliaddr;
    int len = sizeof(cliaddr);
    int n = recvfrom(core.server_fd,&rec,sizeof(rec),0,&cliaddr,&len);
    printf("datagrama recebido \n");
    
    datagram sen = {.id = 2, .timeStamp = (int)time(NULL),.buffer = "Hello Client \n"};
    sendto(core.server_fd,&sen,500,0,(struct sockaddr*)&cliaddr,sizeof(cliaddr));

    printf("id: %d timestamp: %d buffer: %s \n",rec.id,rec.timeStamp,rec.buffer);


    return 0;
}