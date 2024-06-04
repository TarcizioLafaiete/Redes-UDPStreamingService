#include "../include/socketUtils.h"

typeIP translateIP(char* typeOfIP){
    typeIP ip;

    if(strcmp(typeOfIP,"ipv4") == 0){
        ip.type = IPV4;
    }
    else if(strcmp(typeOfIP,"ipv6") == 0){
        ip.type = IPV6;
    }
    else{
        ip.type = UNK;
    }
    return ip;
}

socket_address configure_addr(typeIP ip,int port){
    socket_address socket;

    if(ip.type == IPV4){
        bzero(&socket.addr,sizeof(socket.addr));
        socket.addr.sin_family = AF_INET;
        socket.addr.sin_port = htons(port);
        socket.addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else if(ip.type == IPV6){
        memset(&socket.addr6,0,sizeof(socket.addr6));
        socket.addr6.sin6_family = AF_INET6;
        socket.addr6.sin6_port = htons(port);
        socket.addr6.sin6_addr = in6addr_any; 
        
    }
    return socket;
}


void create_socket(int* socket_fd,typeIP ip){
    
    if(ip.type == IPV4){
        *socket_fd = socket(AF_INET,SOCK_DGRAM,0);
    }
    else if(ip.type == IPV6){
        *socket_fd = socket(AF_INET6,SOCK_DGRAM,0);
    }
    else{
        *socket_fd = -1;
    }
    
    if(*socket_fd < 0){
        printf("Nao foi possivel criar o socket \n");
        exit(EXIT_FAILURE);
    }

}