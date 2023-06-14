#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "defs.h"

void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "port not provided\n");
        exit(1);
    }

    int portnum, byte_count, sockfd, newsockfd;
    char buffer[BUFFER_SIZE];

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        error("error opening socket");
    }
    bzero((char *) &server_addr, sizeof(server_addr));

    portnum = atoi(argv[1]);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(portnum);

    if(bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        error("binding failed");
    }

    listen(sockfd, 5);

    newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_len);

    if (newsockfd < 0){
        error("Error on accept");
    }

    while(1){
        bzero(buffer, BUFFER_SIZE);
        byte_count = read(newsockfd, buffer, BUFFER_SIZE);
        if(byte_count < 0){
            error("error on reading");
        }
        printf("Client: %s", buffer);
        bzero(buffer, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);

        byte_count = write(newsockfd, buffer, strlen(buffer));
        if(byte_count < 0){
            error("error on writing");
        }
        int i = strncmp(EXIT_WORD, buffer, EXIT_SIZE);
        if(i == 0){
            break;
        }
    }

    close(newsockfd);
    close(sockfd);

    return 0;
}
