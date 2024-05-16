#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_DOMAINS 100
#define BUFFER_SIZE 256

#define COM_TLD "127.0.0.3"
#define GOOGLE "127.0.0.6"


int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char sbuf[BUFFER_SIZE], rbuf[BUFFER_SIZE];
    memset(rbuf, '\0', strlen(rbuf));
    memset(sbuf, '\0', strlen(sbuf));

    server_socket = socket(AF_INET, SOCK_STREAM, 0);    //connection with local dns
    if (server_socket == -1) {
        perror(" Root DNS socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2010);  
    server_addr.sin_addr.s_addr = inet_addr(COM_TLD);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(1);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listening failed");
        exit(1);
    }

    printf("COM server listening...\n");


    while (1) {
        socklen_t addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);

        recv(client_socket, rbuf, sizeof(rbuf), 0);     //receive request from local dns
        printf("\nReceived Request : %s\n", rbuf);
        if (strstr(rbuf, "google") != NULL){            //if google present, return address of google authoritative server
            send(client_socket, GOOGLE, strlen(GOOGLE), 0);
            printf("Sent Response : %s\n", GOOGLE);
        }
        else{
            send(client_socket, "Not Found", strlen("Not Found"), 0);
            printf("Sent Response : No Authoritative server found !!!\n");
        }

        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));

    }

    close(server_socket);
    return 0;
}