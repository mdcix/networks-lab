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
    int server_socket, client_socket, tld_server_socket;
    struct sockaddr_in server_addr, client_addr, tld_server_addr;
    char sbuf[BUFFER_SIZE], rbuf[BUFFER_SIZE];
    char TLD_Address[BUFFER_SIZE], request[BUFFER_SIZE], response[BUFFER_SIZE];
    memset(rbuf, '\0', strlen(rbuf));
    memset(sbuf, '\0', strlen(sbuf));

    server_socket = socket(AF_INET, SOCK_STREAM, 0);    //connection from local dns
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
        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));
        socklen_t addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);

        recv(client_socket, rbuf, sizeof(rbuf), 0);
        printf("\nReceived Request from Local DNS: %s\n", rbuf);
        strcpy(request, rbuf);

        //setting the tld address in TLD_Address variable
        if (strstr(rbuf, ".com") != NULL){      
            strcpy(TLD_Address, COM_TLD);
        }
        else if (strstr(rbuf, ".edu") != NULL){
            strcpy(TLD_Address, EDU_TLD);
        }
        else{
            send(client_socket, "Not Found, No TLD domain Found", strlen("Not Found, No TLD domain Found"), 0);
            printf("Sent Response to Local DNS server: No TLD domain found !!!\n");
            continue;
        }

        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));

        tld_server_socket = socket(AF_INET, SOCK_STREAM, 0);    //contact TLD server
        if (tld_server_socket == -1) {
            perror("TLD server Socket creation failed");
            exit(1);
        }

        tld_server_addr.sin_family = AF_INET;
        tld_server_addr.sin_port = htons(2010);
        tld_server_addr.sin_addr.s_addr = inet_addr(TLD_Address);

        if (connect(tld_server_socket, (struct sockaddr *)&tld_server_addr, sizeof(tld_server_addr)) == -1) {
            perror("Connection to tld server failed");
        }

        strcpy(sbuf, request);

        printf("Sending request to TLD...\n");
        send(tld_server_socket, sbuf, strlen(sbuf), 0);

        recv(tld_server_socket, rbuf, sizeof(rbuf), 0);
        printf("Received from TLD : %s\n", rbuf);

        strcpy(response, rbuf);
        send(client_socket, response, strlen(response), 0);
        printf("Sending response to local DNS...\n");
        close(client_socket);
    }

    close(server_socket);
    return 0;
}