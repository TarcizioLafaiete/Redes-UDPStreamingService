#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../include/socketUtils.h"
#include "../include/serverCore.h"
#include "../include/terminalPrinter.h"

typedef struct{
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

int main(int argc,char* argv[]){

    serverCore core = initServer(argv);

    while(1){
        
    }

    return 0;
}