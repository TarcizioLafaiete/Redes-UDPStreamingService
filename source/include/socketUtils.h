#ifndef __SOCKET_UTILS__
#define __SOCKET_UTILS__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define IPV4 0
#define IPV6 1
#define UNK 2

#define MAX_MESSAGE_SIZE 300

typedef struct{
    int type;
} typeIP;

typedef struct{
    struct sockaddr_in addr;
    struct sockaddr_in6 addr6;
    socklen_t len;
} socket_address;

typedef struct{
    int id;
    int sequence;
    int escolha;
    int startConnection;
    char buffer[250];
} datagram;

typeIP translateIP(char* ip);
socket_address configure_addr(typeIP inet, int port);
void create_socket(int* socket_fd,typeIP inet);

#endif