#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "../include/socketUtils.h"
#include "../include/serverCore.h"
#include "../include/vector.h"

typedef struct{
    serverCore core;
    struct sockaddr cliaddr;
    struct sockaddr_in6 cliaddr6;
    int cliaddr_len;
    datagram data;
} serverClojure;

int client_id = 0;
S_cVector* writeBuffer;
S_cVector* readBuffer;
S_cVector* threadBuffer;

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
    int index = *id - 1;
    serverClojure recvClojure;
    serverClojure sendClojure;

    pthread_mutex_lock(&readBufferMutex);
    getElementVector(readBuffer,index,&recvClojure);
    printf("Start Connection: %d \n",recvClojure.data.startConnection);
    pthread_mutex_unlock(&readBufferMutex);

    int movieSelected = recvClojure.data.escolha;
    printf("movie Selected: %d \n",movieSelected);

    datagram confirmConnection = { .startConnection = 0,
                                .id = *id,
                                .escolha = -1,
                                .sequence = 0,
                                .ack = -1,
                                .buffer = "\0"};

    sendClojure = recvClojure;
    while(recvClojure.data.ack != *id){
        sendClojure.data =  confirmConnection;
        printf("send Clojure Data: %d \n",sendClojure.data.id);

        pthread_mutex_lock(&writeBufferMutex);
        push(writeBuffer,&sendClojure);
        printf("Datagram pushed to writeBuffer \n");
        pthread_mutex_unlock(&writeBufferMutex);
    
        sleep(1);
        
        pthread_mutex_lock(&readBufferMutex);
        getElementVector(readBuffer,index,&recvClojure);
        printf("read Buffer ack: %d \n",recvClojure.data.ack);
        pthread_mutex_unlock(&readBufferMutex);
    }

}

void* sendHandle(void* arg){
    
    serverCore* core = (serverCore*)arg;

    while(1){
        serverClojure clojure;
        clojure.core = *core;
        
        pthread_mutex_lock(&writeBufferMutex);
        if(getSize(writeBuffer) > 0){
            printf("Has data - size: %d \n",getSize(writeBuffer));
            pop(writeBuffer,&clojure);
            sendDatagram(clojure.core.server_fd,clojure);
            printf("Datagram Sent \n");
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
        printf("startConnection Requisition: %d \n",clojure.data.startConnection);
        if(clojure.data.startConnection){
            printf("connection starting ... \n");

            pthread_mutex_lock(&readBufferMutex);
            push(readBuffer,&clojure);
            printf("Open space in buffer to this client \n");
            client_id = getSize(readBuffer);
            printf("Generate client id: %d \n",client_id);
            pthread_mutex_unlock(&readBufferMutex);
            
            pthread_t newThread;
            pthread_create(&newThread,NULL,clientHandle,&client_id);
            printf("New thread created \n");
            push(threadBuffer,&newThread);
            printf("Put this thread in threadBuffer \n");
        }
        else{
            setElementVector(readBuffer,clojure.data.id,&clojure);
        }
    }
}

int main(int argc,char* argv[]){

    serverCore core = initServer(argv);
    writeBuffer = create_vector(sizeof(serverClojure));
    readBuffer = create_vector(sizeof(serverClojure));
    threadBuffer = create_vector(sizeof(serverClojure));


    pthread_t recvThread, sendThread;
    pthread_create(&recvThread,NULL,recvHandle,&core);
    pthread_create(&sendThread,NULL,sendHandle,&core);
    pthread_mutex_init(&writeBufferMutex,NULL);
    pthread_mutex_init(&readBufferMutex,NULL);

    pthread_join(recvThread,NULL);
    pthread_join(sendThread,NULL);

    return 0;
}