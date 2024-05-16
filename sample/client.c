#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define PORT 2000

int main(void)
{
    int server_sock;
    struct sockaddr_in server_addr;
    char sbuf[2048], rbuf[2048];

    memset(sbuf, '\0', sizeof(sbuf));
    memset(rbuf, '\0', sizeof(rbuf));

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if(server_sock < 0){
        perror("Unable to create socket\n");
        exit(1);
    }
    
    printf("Socket created successfully\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    if(connect(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("Unable to connect\n");
        exit(1);
    }
    printf("Connected with server successfully\n");

    printf("\nEnter message: ");
    gets(sbuf);

    if(send(server_sock, sbuf, strlen(sbuf), 0) < 0){
        perror("Unable to send message\n");
        exit(1);
    }

    if(recv(server_sock, rbuf, sizeof(rbuf), 0) < 0){
        perror("Error while receiving server's msg\n");
        exit(1);
    }
    
    printf("Server : %s\n",rbuf);

    close(server_sock);
    
    return 0;
}