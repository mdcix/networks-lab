#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_DOMAINS 100
#define BUFFER_SIZE 256

#define ROOT_DNS "127.0.0.2"
#define COM_TLD "127.0.0.3"
#define EDU_TLD "127.0.0.4"

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char sbuf[BUFFER_SIZE], rbuf[BUFFER_SIZE];
    memset(rbuf, '\0', strlen(rbuf));
    memset(sbuf, '\0', strlen(sbuf));

    server_socket = socket(AF_INET, SOCK_STREAM, 0); //connect with local dns server
    if (server_socket == -1) {
        perror(" Root DNS socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2005);  
    server_addr.sin_addr.s_addr = inet_addr(ROOT_DNS);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(1);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listening failed");
        exit(1);
    }

    printf("Root DNS listening...\n");

    while (1) {
        socklen_t addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);

        recv(client_socket, rbuf, sizeof(rbuf), 0);     //receive request
        printf("\nReceived Request : %s\n", rbuf);

        if (strstr(rbuf, ".com") != NULL){
            send(client_socket, COM_TLD, strlen(COM_TLD), 0);   //if .com, return address of com server
            printf("Sent Response : %s\n", COM_TLD);
        }
        else if (strstr(rbuf, ".edu") != NULL){
            send(client_socket, EDU_TLD, strlen(EDU_TLD), 0);   //if .edu, return address of edu server
            printf("Sent Response : %s\n", EDU_TLD);
        }
        else{
            send(client_socket, "Not Found", strlen("Not Found"), 0);
            printf("Sent Response : No TLD domain found !!!\n");
        }

        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));

    }

    close(server_socket);
    return 0;
}