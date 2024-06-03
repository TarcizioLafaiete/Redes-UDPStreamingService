#include <stdlib.h>
#include <stdio.h>

#include "../include/socketUtils.h"
#include "../include/clientCore.h"
#include "../include/terminalPrinter.h"



clientCore initClient(char* argv[]){
    clientCore core;

    core.inet = translateIP(argv[1]);

    create_socket(&core.client_fd,core.inet);
    core.serverSocket = configure_addr(core.inet,atoi(argv[3]));
    
    connect_client(&core);

    return core;
}

int main(int argc, char* argv[]){
    clientCore core = initClient(argv);

    return 0;
}

