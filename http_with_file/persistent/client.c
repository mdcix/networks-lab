#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
#define PORT 8080

void receive_base_file(int server_sock) {
    printf("Receiving Base File...\n");

    char response[4096];
    memset(response, '\0', sizeof(response));
    // Receive and print the base file from the server
    if(recv(server_sock, response, sizeof(response), 0)<0){
        perror("Error while receiving");
        exit(1);
    }
    printf("\nBase file Content : \n%s\n", response);

    printf("Base File Received\n");
}

void receive_all_objects(int server_sock) {
    printf("\nReceiving All Objects\n");

    char response[4096];
    memset(response, '\0', sizeof(response));
    // Receive and print all objects from the server
    if(recv(server_sock, response, sizeof(response), 0)<0){
        perror("Error while receiving");
        exit(1);
    }
    printf("\nReceived Objects : \n%s\n", response);

    printf("All Objects Received\n");
}

int main() {
    int server_sock;
    struct sockaddr_in server_addr;

    // Create a socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0){
        perror("Error while creating socket\n");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Connect to the server
    if (connect(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Request and receive the base file
    send(server_sock, "BASE_FILE", strlen("BASE_FILE"), 0);
    receive_base_file(server_sock);

    // Close the connection
    close(server_sock);

    // Open a new connection

    // Create a new socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Connect to the server again
    if (connect(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Request and receive all objects
    send(server_sock, "ALL_OBJECTS", sizeof("ALL_OBJECTS"), 0);
    receive_all_objects(server_sock);

    // Close the connection
    close(server_sock);
    return 0;
}

