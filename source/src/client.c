#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "../include/socketUtils.h"
#include "../include/clientCore.h"
#include "../include/linked_list.h"
#include "../include/terminalPrinter.h"

int last_message = 0;
int ackFlag = 0;
datagram* currentDatagram;
pthread_mutex_t sendMutex;


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


int chooseMovie(){
    int movieSelected;

    printMovieOptions();
    while(1){
        printf("Opção Escolhida: ");
        scanf("%d",&movieSelected);
        if(movieSelected >= 0 && movieSelected <= 3){
            return movieSelected;
        }
        printf("Opcao Invalida! \n");
    }
}

int startConnection(clientCore core,int movieSelected){
    datagram startDatagram = {.startConnection = 1,
                            .endConnection = 0,
                            .id = -1,
                            .escolha = movieSelected,
                            .sequence = -1,
                            .ack = -1,
                            .buffer = "\0"
    };

    sendDatagram(core,startDatagram);

    datagram response = receiveDatagram(core.client_fd);

    response.ack = response.id;
    response.escolha = movieSelected;

    sendDatagram(core,response);

    return response.id;
}

void endConnection(clientCore core){

    datagram endDatagram = {.startConnection = 0,
                            .endConnection = 1,
                            .id = core.client_id,
                            .escolha = 0,
                            .sequence = -1,
                            .ack = -1,
                            .buffer = "\0"};
    sendDatagram(core,endDatagram);
}

void phraseRoutine(clientCore core){
    int last_sequence = 0;
    datagram response;
    printBorder();
    for(int i = 0; i < 5; i++){
        response =  receiveDatagram(core.client_fd);
        printPhrase(response.buffer);

    }
    printBorder();

}

int main(int argc, char* argv[]){

    clientCore core = initClient(argv);

    while(1){
        int movieSelected = chooseMovie();
        core.client_id = startConnection(core,movieSelected);
        printf("client id: %d \n",core.client_id);
        phraseRoutine(core);
        
    }

    return 0;
}

