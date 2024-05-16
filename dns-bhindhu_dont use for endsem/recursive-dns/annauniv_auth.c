#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_DOMAINS 100
#define BUFFER_SIZE 256

#define ANNAUNIV "127.0.0.5"

#define WWW "14.139.161.7"
#define CS "14.139.161.14"
#define ACOE "14.139.161.45"
#define CTDT "14.139.161.73"

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char sbuf[BUFFER_SIZE], rbuf[BUFFER_SIZE];
    memset(rbuf, '\0', strlen(rbuf));
    memset(sbuf, '\0', strlen(sbuf));

    server_socket = socket(AF_INET, SOCK_STREAM, 0);        //connection from tld server
    if (server_socket == -1) {
        perror(" Root DNS socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2015);  
    server_addr.sin_addr.s_addr = inet_addr(ANNAUNIV);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(1);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listening failed");
        exit(1);
    }

    printf("Annauniv Authoritative server listening...\n");


    while (1) {
        socklen_t addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);

        recv(client_socket, rbuf, sizeof(rbuf), 0);
        printf("\nReceived Request from TLD: %s\n", rbuf);

        if (strstr(rbuf, "www.annauniv.edu") != NULL || strcmp(rbuf, "annauniv.edu")==0){
            send(client_socket, WWW, strlen(WWW), 0);
            printf("Sent Response to TLD: %s\n", WWW);
        }
        else if (strstr(rbuf, "cs.annauniv.edu") != NULL){
            send(client_socket, CS, strlen(CS), 0);
            printf("Sent Response to TLD: %s\n", CS);
        }
        else if (strstr(rbuf, "acoe.annauniv.edu") != NULL){
            send(client_socket, ACOE, strlen(ACOE), 0);
            printf("Sent Response to TLD: %s\n", ACOE);
        }
        else if (strstr(rbuf, "ctdt.annauniv.edu") != NULL){
            send(client_socket, CTDT, strlen(CTDT), 0);
            printf("Sent Response to TLD: %s\n", CTDT);
        }
        else{
            send(client_socket, "Not Found, No IP entry found !!!", strlen("Not Found, No IP entry found !!!"), 0);
            printf("Sent Response to TLD: No IP Entry Found !!\n");
        }

        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));
    }

    close(server_socket);
    return 0;
}