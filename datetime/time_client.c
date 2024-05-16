#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define PORT 2000

int main(){

    int server_sock;
    struct sockaddr_in server_addr;
    char rbuf[2000];

    memset(rbuf, '\0', sizeof(rbuf));

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock<0){
        perror("Socket create error");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    if(connect(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        perror("Error while connecting");
        exit(1);
    }
    printf("Connected with server\n");

    if(recv(server_sock, rbuf, sizeof(rbuf), 0)<0){
        perror("Error while receiving");
        exit(1);
    }
    printf("\nTime from server : %s\n", rbuf);

    close(server_sock);

    return 0;
}
