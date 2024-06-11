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

void confirmConnectionRoutine(int id){

    serverClojure recvClojure;
    serverClojure sendClojure;

    pthread_mutex_lock(&readBufferMutex);
    getItem(readBuffer,id,&recvClojure);
    pthread_mutex_unlock(&readBufferMutex);

    printf("Confirm connection routine \n");

    datagram confirmConnection = { .startConnection = 0,
                                .endConnection = 0,
                                .id = id,
                                .escolha = -1,
                                .sequence = 0,
                                .ack = -1,
                                .buffer = "\0"};

    sendClojure = recvClojure;
    printf("id: %d - ack: %d \n",id,recvClojure.data.ack);
    while(recvClojure.data.ack != id){
        sendClojure.data =  confirmConnection;
        printf("ID check : %d \n",sendClojure.data.id);

        sendDatagram(sendClojure.core.server_fd,sendClojure);

        sleep(1);
        
        pthread_mutex_lock(&readBufferMutex);
        getItem(readBuffer,id,&recvClojure);
        pthread_mutex_unlock(&readBufferMutex);

        printf("ack received: %d \n",recvClojure.data.ack);
    }

    printf("confirm requisition: %d \n",recvClojure.data.ack);
}


void sendMovieScriptRoutine(int id,int movieSelected){
    serverClojure recvClojure;
    serverClojure sendClojure;

    pthread_mutex_lock(&readBufferMutex);
    getItem(readBuffer,id,&recvClojure);
    pthread_mutex_unlock(&readBufferMutex);

    sendClojure = recvClojure;
    
    printf("Going to streaming routine \n");
    datagram phraseDatagramCycle = {.startConnection = 0,
                                    .endConnection = 0,
                                    .id = id,
                                    .escolha = -1,
                                    .sequence = 0,
                                    .ack = -1,
                                    .buffer = "\0"};

    int sequence = 1;
    while(sequence <= 5){
        memcpy(phraseDatagramCycle.buffer,movie_script[movieSelected-1][sequence-1],250*sizeof(char));
        phraseDatagramCycle.sequence = sequence;
        sendClojure.data = phraseDatagramCycle;
        printf("Clojure was setted \n");

        while(recvClojure.data.ack != id || recvClojure.data.sequence < sequence){
            sendDatagram(sendClojure.core.server_fd,sendClojure);

            sleep(1);

            pthread_mutex_lock(&readBufferMutex);
            printf("Verifying if datagram was sent \n");
            getItem(readBuffer,id,&recvClojure);
            printf("confirmation sequence: %d e real sequence: %d \n",recvClojure.data.sequence,sequence);
            pthread_mutex_unlock(&readBufferMutex);
        }

        sleep(3);
        sequence++;

    }
}

void* clientHandle(void* arg){

    int id = *((int*)arg);

    confirmConnectionRoutine(id);

    serverClojure request;

    pthread_mutex_lock(&readBufferMutex);
    getItem(readBuffer,id,&request);
    pthread_mutex_unlock(&readBufferMutex);

    printf("AAAAAAAAAAAAAAA \n");

    request.data.startConnection = 0;
    request.data.endConnection = 0;

    for(int i = 0; i < 5; i++){
        memcpy(request.data.buffer,movie_script[request.data.escolha - 1][i],250*sizeof(char)); 
        sendDatagram(request.core.server_fd,request);
        sleep(3);
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
            int client_id = getSize(readBuffer);
            printf("client ID %d \n",client_id);
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
            pthread_join(thread,NULL);

            pthread_mutex_lock(&numClientMutex);
            num_client--;
            pthread_mutex_unlock(&numClientMutex);

        }
        else{
            pthread_mutex_lock(&readBufferMutex);
            setItem(readBuffer,clojure.data.id,&clojure);
            printf("Buffer Setted \n");
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
    pthread_join(sendThread,NULL);
    pthread_join(clientNumberThread,NULL);

    return 0;
}