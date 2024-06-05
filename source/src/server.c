#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "../include/socketUtils.h"
#include "../include/serverCore.h"
#include "../include/linked_list.h"

typedef struct{
    serverCore core;
    struct sockaddr cliaddr;
    struct sockaddr_in6 cliaddr6;
    int cliaddr_len;
    datagram data;
} serverClojure;

int client_id = 0;
S_LinkedList* writeBuffer;
S_LinkedList* readBuffer;
S_LinkedList* threadBuffer;

pthread_mutex_t writeBufferMutex;
pthread_mutex_t readBufferMutex;


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

    if(clojure->core.inet.type == IPV4){
        struct sockaddr cliaddr;
        int len = sizeof(cliaddr);
        recvfrom(socket,&clojure->data,sizeof(clojure->data),0,&cliaddr,&len);
        clojure->cliaddr = cliaddr;
        clojure->cliaddr_len = len;
    }
    else{
        struct sockaddr_in6 cliaddr;
        int len = sizeof(cliaddr);
        recvfrom(socket,&clojure->data,sizeof(clojure->data),0,(struct sockaddr*)&cliaddr,&len);
        clojure->cliaddr6 = cliaddr;
        clojure->cliaddr_len = len;
    }

}

void sendDatagram(int socket, serverClojure clojure){

    if(clojure.core.inet.type == IPV4){
        sendto(socket,&clojure.data,MAX_MESSAGE_SIZE,0,(struct sockaddr*)&clojure.cliaddr,sizeof(clojure.cliaddr));
    }
    else{
        sendto(socket,&clojure.data,MAX_MESSAGE_SIZE,0,(struct sockaddr*)&clojure.cliaddr6,sizeof(clojure.cliaddr6));
    }

}

void* clientHandle(void* arg){

    int* id = (int*)arg;
    serverClojure recvClojure;
    serverClojure sendClojure;

    pthread_mutex_lock(&readBufferMutex);
    getItem(readBuffer,*id,&recvClojure);
    pthread_mutex_unlock(&readBufferMutex);

    int movieSelected = recvClojure.data.escolha;

    datagram confirmConnection = { .startConnection = 0,
                                .id = *id,
                                .escolha = -1,
                                .sequence = 0,
                                .ack = -1,
                                .buffer = "\0"};

    sendClojure = recvClojure;
    while(recvClojure.data.ack != *id){
        sendClojure.data =  confirmConnection;

        pthread_mutex_lock(&writeBufferMutex);
        push(writeBuffer,&sendClojure);
        pthread_mutex_unlock(&writeBufferMutex);
    
        sleep(1);
        
        pthread_mutex_lock(&readBufferMutex);
        getItem(readBuffer,*id,&recvClojure);
        pthread_mutex_unlock(&readBufferMutex);
    }

    printf("Going to streaming routine \n");
    datagram phraseDatagramCycle = {.startConnection = 0,
                                    .id = *id,
                                    .escolha = -1,
                                    .sequence = 0,
                                    .ack = -1,
                                    .buffer = "\0"};

    int sequence = 1;
    char phrase[250] = "Phrase";
    while(sequence <= 5){
        memcpy(phraseDatagramCycle.buffer,phrase,250*sizeof(char));
        phraseDatagramCycle.sequence = sequence;
        sendClojure.data = phraseDatagramCycle;
        printf("Clojure was setted \n");

        while(recvClojure.data.ack != *id || recvClojure.data.sequence != sequence){
            printf("Datagram process \n");
            pthread_mutex_lock(&writeBufferMutex);
            printf("Writing clojure \n");
            push(writeBuffer,&sendClojure);
            pthread_mutex_unlock(&writeBufferMutex);

            sleep(1);

            pthread_mutex_lock(&readBufferMutex);
            printf("Verifying if datagram was sent \n");
            getItem(readBuffer,*id,&recvClojure);
            printf("confirmation sequence: %d \n",recvClojure.data.sequence);
            pthread_mutex_unlock(&readBufferMutex);
        }

        sleep(3);
        sequence++;

    }

}

void* sendHandle(void* arg){
    
    serverCore* core = (serverCore*)arg;

    while(1){
        serverClojure clojure;
        clojure.core = *core;
        
        pthread_mutex_lock(&writeBufferMutex);
        if(getSize(writeBuffer) > 0){
            pop(writeBuffer,&clojure);
            sendDatagram(clojure.core.server_fd,clojure);
        }
        pthread_mutex_unlock(&writeBufferMutex);
    }

}

void* recvHandle(void* arg){
    
    serverCore* core = (serverCore*)arg;

    while(1){
        serverClojure clojure;
        clojure.core = *core;
        receiveDatagram(clojure.core.server_fd,&clojure);

        if(clojure.data.startConnection){

            pthread_mutex_lock(&readBufferMutex);
            push(readBuffer,&clojure);
            client_id = getSize(readBuffer);
            pthread_mutex_unlock(&readBufferMutex);
            
            pthread_t newThread;
            pthread_create(&newThread,NULL,clientHandle,&client_id);
            push(threadBuffer,&newThread);
        }
        else{
            pthread_mutex_lock(&readBufferMutex);
            printf("sequence data: %d  - clojure data: %d \n",clojure.data.sequence,clojure.data.id);
            setItem(readBuffer,clojure.data.id,&clojure);
            pthread_mutex_unlock(&readBufferMutex);
        }
    }
}

int main(int argc,char* argv[]){

    serverCore core = initServer(argv);
    writeBuffer = create_list(sizeof(serverClojure));
    readBuffer = create_list(sizeof(serverClojure));
    threadBuffer = create_list(sizeof(pthread_t));


    pthread_t recvThread, sendThread;
    pthread_create(&recvThread,NULL,recvHandle,&core);
    pthread_create(&sendThread,NULL,sendHandle,&core);
    pthread_mutex_init(&writeBufferMutex,NULL);
    pthread_mutex_init(&readBufferMutex,NULL);

    pthread_join(recvThread,NULL);
    pthread_join(sendThread,NULL);

    return 0;
}