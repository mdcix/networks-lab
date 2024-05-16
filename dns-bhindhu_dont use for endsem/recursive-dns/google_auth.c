#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_DOMAINS 100
#define BUFFER_SIZE 256

#define GOOGLE "127.0.0.6"

#define WWW "142.250.183.238"
#define DRIVE "172.217.160.142"
#define MAIL "216.58.196.165"
#define CHROME "142.250.76.174"
#define SUPPORT "142.250.193.110"

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char sbuf[BUFFER_SIZE], rbuf[BUFFER_SIZE];
    memset(rbuf, '\0', strlen(rbuf));
    memset(sbuf, '\0', strlen(sbuf));

    server_socket = socket(AF_INET, SOCK_STREAM, 0);    //connection from tld server
    if (server_socket == -1) {
        perror(" Root DNS socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2015);  
    server_addr.sin_addr.s_addr = inet_addr(GOOGLE);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(1);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listening failed");
        exit(1);
    }

    printf("Google Authoritative server listening...\n");


    while (1) {
        socklen_t addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);

        recv(client_socket, rbuf, sizeof(rbuf), 0);
        printf("\nReceived Request from TLD: %s\n", rbuf);

        if (strstr(rbuf, "www.google.com") != NULL || strcmp(rbuf, "google.com")==0){
            send(client_socket, WWW, strlen(WWW), 0);
            printf("Sent Response to TLD: %s\n", WWW);
        }
        else if (strstr(rbuf, "drive.google.com") != NULL){
            send(client_socket, DRIVE, strlen(DRIVE), 0);
            printf("Sent Response to TLD: %s\n", DRIVE);
        }
        else if (strstr(rbuf, "mail.google.com") != NULL){
            send(client_socket, MAIL, strlen(MAIL), 0);
            printf("Sent Response to TLD: %s\n", MAIL);
        }
        else if (strstr(rbuf, "chrome.google.com") != NULL){
            send(client_socket, CHROME, strlen(CHROME), 0);
            printf("Sent Response to TLD: %s\n", CHROME);
        }
        else if (strstr(rbuf, "support.google.com") != NULL){
            send(client_socket, SUPPORT, strlen(SUPPORT), 0);
            printf("Sent Response to TLD: %s\n", SUPPORT);
        }
        else{
            send(client_socket, "Not Found, No IP entry found !!!", strlen("Not Found, No IP entry found !!!"), 0);
            printf("Sent Response to TLD: No IP entry Found !!\n");
        }

        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));

    }

    close(server_socket);
    return 0;
}