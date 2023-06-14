#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "defs.h"

void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){
    int portnum, byte_count, sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;

    char buffer[BUFFER_SIZE];
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port", argv[0]);
        exit(1);
    }

    portnum = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        error("error opening socket");
    }

    server = gethostbyname(argv[1]);
    if(server == NULL){
        fprintf(stderr, "no such host");
    }

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(portnum);

    if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        error("Connection failed");
    }

    while (1) {
        bzero(buffer, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);
        byte_count = write(sockfd, buffer, strlen(buffer));
        if(byte_count < 0){
            error("error on writing");
        }
        bzero(buffer, BUFFER_SIZE);
        byte_count = read(sockfd, buffer, BUFFER_SIZE);
        if(byte_count < 0){
            error("error on reading");
        }
        printf("Server: %s", buffer);

        int i = strncmp(EXIT_WORD, buffer, EXIT_SIZE);
        if( i == 0){
            break;
        }
    }

    close(sockfd);
    return 0;
}
