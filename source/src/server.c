#include <stdlib.h>
#include <stdio.h>

#include "../include/socketUtils.h"
#include "../include/serverCore.h"
#include "../include/terminalPrinter.h"


serverCore initServer(char* argv[]){
    serverCore core;

    core.inet = translateIP(argv[1]);

    create_socket(&core.server_fd,core.inet);
    core.socket = configure_addr(core.inet,atoi(argv[2]));
    configure_options(core.server_fd);
    binding(&core);
    return core;
}

int main(int argc,char* argv[]){

    serverCore core = initServer(argv);

    while(1){
    }

    return 0;
}