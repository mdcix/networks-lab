#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_DOMAINS 100
#define BUFFER_SIZE 256

#define EDU_TLD "127.0.0.4"
#define ANNAUNIV "127.0.0.5"

int main() {
    int server_socket, client_socket, auth_server_socket;
    struct sockaddr_in server_addr, client_addr, auth_server_addr;
    char sbuf[BUFFER_SIZE], rbuf[BUFFER_SIZE];
    char AUTH_Address[BUFFER_SIZE], request[BUFFER_SIZE], response[BUFFER_SIZE];

    memset(rbuf, '\0', strlen(rbuf));
    memset(sbuf, '\0', strlen(sbuf));
    memset(request, '\0', strlen(request));
    memset(response, '\0', strlen(response));

    server_socket = socket(AF_INET, SOCK_STREAM, 0);    //connection with root dns
    if (server_socket == -1) {
        perror(" Root DNS socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2010);  
    server_addr.sin_addr.s_addr = inet_addr(EDU_TLD);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(1);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listening failed");
        exit(1);
    }

    printf("EDU server listening...\n");


    while (1) {
        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));
        socklen_t addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);

        recv(client_socket, rbuf, sizeof(rbuf), 0);     //receive request from root dns
        printf("\nReceived Request from ROOT server : %s\n", rbuf);
        strcpy(request, rbuf);
        
        if (strstr(rbuf, "annauniv") != NULL){          //if annauniv present, set AUTH_Address to annauniv server
            strcpy(AUTH_Address, ANNAUNIV);
        }
        else{
            send(client_socket, "Not Found", strlen("Not Found"), 0);
            printf("Sent Response : No Authoritative server found !!!\n");
        }

        auth_server_socket = socket(AF_INET, SOCK_STREAM, 0);   //contact auth server
        if (auth_server_socket == -1) {
            perror("TLD server Socket creation failed");
            exit(1);
        }

        auth_server_addr.sin_family = AF_INET;
        auth_server_addr.sin_port = htons(2015);
        auth_server_addr.sin_addr.s_addr = inet_addr(AUTH_Address);

        if (connect(auth_server_socket, (struct sockaddr *)&auth_server_addr, sizeof(auth_server_addr)) == -1) {
            perror("Connection to auth server failed");
        }
        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));
        strcpy(sbuf, request);

        printf("Sending request to auth server : %s\n", sbuf);
        send(auth_server_socket, sbuf, strlen(sbuf), 0);
        recv(auth_server_socket, rbuf, sizeof(rbuf), 0);
        printf("Received Response from Auth server : %s\n", rbuf);
        strcpy(response, rbuf);
        printf("Sending response to root server : %s\n", rbuf);
        send(client_socket, response, strlen(response), 0);
        close(client_socket);
    }

    close(server_socket);
    return 0;
}