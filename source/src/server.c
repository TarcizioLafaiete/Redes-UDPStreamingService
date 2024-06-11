#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "../include/socketUtils.h"
#include "../include/serverCore.h"
#include "../include/linked_list.h"
#include "../include/movie_script.h"

typedef struct{
    serverCore core;
    struct sockaddr cliaddr;
    struct sockaddr_in6 cliaddr6;
    int cliaddr_len;
    datagram data;
} serverClojure;

int num_client;

S_LinkedList* readBuffer;
S_LinkedList* threadBuffer;

pthread_mutex_t readBufferMutex;
pthread_mutex_t numClientMutex;


serverCore initServer(char* argv[]){
    serverCore core;

    core.inet = translateIP(argv[1]);

    create_socket(&core.server_fd,core.inet);
    core.socket = configure_addr(core.inet,atoi(argv[2]));
    configure_options(core.server_fd);
    binding(&core);
    return core;
}

void receiveDatagram(serverClojure* clojure){

    if(clojure->core.inet.type == IPV4){
        struct sockaddr cliaddr;
        int len = sizeof(cliaddr);
        recvfrom(clojure->core.server_fd,&clojure->data,sizeof(clojure->data),0,&cliaddr,&len);
        clojure->cliaddr = cliaddr;
        clojure->cliaddr_len = len;
    }
    else{
        struct sockaddr_in6 cliaddr;
        int len = sizeof(cliaddr);
        recvfrom(clojure->core.server_fd,&clojure->data,sizeof(clojure->data),0,(struct sockaddr*)&cliaddr,&len);
        clojure->cliaddr6 = cliaddr;
        clojure->cliaddr_len = len;
    }

}

void sendDatagram(serverClojure clojure){

    if(clojure.core.inet.type == IPV4){
        sendto(clojure.core.server_fd,&clojure.data,MAX_MESSAGE_SIZE,0,(struct sockaddr*)&clojure.cliaddr,sizeof(clojure.cliaddr));
    }
    else{
        sendto(clojure.core.server_fd,&clojure.data,MAX_MESSAGE_SIZE,0,(struct sockaddr*)&clojure.cliaddr6,sizeof(clojure.cliaddr6));
    }

}

void confirmConnectionRoutine(int id){

    serverClojure response;

    pthread_mutex_lock(&readBufferMutex);
    getItem(readBuffer,id,&response);
    pthread_mutex_unlock(&readBufferMutex);

    datagram confirmConnection = { .startConnection = 0,
                                .endConnection = 0,
                                .req_sequence = 0,
                                .id = id,
                                .escolha = -1,
                                .buffer = "\0"};

    response.data = confirmConnection;

    sendDatagram(response);

}

void* clientHandle(void* arg){

    int id = *((int*)arg);

    confirmConnectionRoutine(id);
    
    int last_request = 0;

    serverClojure request;

    while(1){
        pthread_mutex_lock(&readBufferMutex);
        getItem(readBuffer,id,&request);
        pthread_mutex_unlock(&readBufferMutex);

        request.data.startConnection = 0;

        if(request.data.req_sequence > last_request){
            for(int i = 0; i < 5; i++){
                memcpy(request.data.buffer,movie_script[request.data.escolha - 1][i],250*sizeof(char)); 
                sendDatagram(request);
                sleep(3);
            }
            last_request = request.data.req_sequence;
        }
    }

}

void* recvHandle(void* arg){
    
    serverCore* core = (serverCore*)arg;

    while(1){
        serverClojure clojure;
        clojure.core = *core;
        receiveDatagram(&clojure);

        if(clojure.data.startConnection){
            pthread_mutex_lock(&readBufferMutex);
            push(readBuffer,&clojure);
            int client_id = getSize(readBuffer);
            pthread_mutex_unlock(&readBufferMutex);
            
            pthread_t newThread;
            pthread_create(&newThread,NULL,clientHandle,&client_id);
            push(threadBuffer,&newThread);

            pthread_mutex_lock(&numClientMutex);
            num_client++;
            pthread_mutex_unlock(&numClientMutex);

        }
        else if(clojure.data.endConnection){
            pthread_t thread;
            getItem(threadBuffer,clojure.data.id,&thread);
            pthread_cancel(thread);

            pthread_mutex_lock(&numClientMutex);
            num_client--;
            pthread_mutex_unlock(&numClientMutex);

        }
        else{
            pthread_mutex_lock(&readBufferMutex);
            setItem(readBuffer,clojure.data.id,&clojure);
            pthread_mutex_unlock(&readBufferMutex);
        }
    }
}


void* clientNumberHandle(void* arg){

    while(1){
        sleep(4);

        pthread_mutex_lock(&numClientMutex);
        printf("Numero de Clientes: %d \n",num_client);
        pthread_mutex_unlock(&numClientMutex);
    }

}

int main(int argc,char* argv[]){num_client++;

    serverCore core = initServer(argv);

    readBuffer = create_list(sizeof(serverClojure));
    threadBuffer = create_list(sizeof(pthread_t));
    num_client = 0;

    pthread_t recvThread, sendThread,clientNumberThread;
    pthread_create(&recvThread,NULL,recvHandle,&core);
    pthread_create(&clientNumberThread,NULL,clientNumberHandle,NULL);
    
    pthread_mutex_init(&readBufferMutex,NULL);
    pthread_mutex_init(&numClientMutex,NULL);

    pthread_join(recvThread,NULL);
    pthread_join(clientNumberThread,NULL);

    return 0;
}