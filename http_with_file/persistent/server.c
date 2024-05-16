#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
#define PORT 8080
#define BASE_FILE "basefile.txt"
#define OBJECT_FILE "object.txt"

void send_base_file(int sockfd) {
    printf("Sending Base File\n");

    // Open the base file
    FILE* base_file = fopen(BASE_FILE, "r");
    if (base_file == NULL) {
        perror("Failed to open base file");
        exit(1);
    }

    char line[256];
    char response[4096];

    // Read and send each line from the base file
    while (fgets(line, sizeof(line), base_file) != NULL) {
        strcat(response, line);
    }

    // Send the concatenated response containing the base file
    send(sockfd, response, strlen(response), 0);

    // Close the base file
    fclose(base_file);
    memset(line, '\0', sizeof(line));
    memset(response, '\0', sizeof(response));
}

void send_all_objects(int sockfd) {
    printf("Sending All Objects\n");

    // Open the object file
    FILE* object_file = fopen(OBJECT_FILE, "r");
    if (object_file == NULL) {
        perror("Failed to open object file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    char response[4096];

    // Read and send each line from the object file
    while (fgets(line, sizeof(line), object_file) != NULL) {
        strcat(response, line);
    }

    // Send the concatenated response containing all objects
    send(sockfd, response, strlen(response), 0);

    // Close the object file
    fclose(object_file);
    memset(line, '\0', sizeof(line));
    memset(response, '\0', sizeof(response));
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    int client_size = sizeof(client_addr);
    char request[2048];

    // Create a socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0){
        perror("Error while creating socket\n");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Bind the socket
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept a new connection
        if ((client_sock = accept(server_sock, (struct sockaddr*)&client_addr, (socklen_t*)&client_size)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("\nConnection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Receive a request from the client to determine what to send
        recv(client_sock, request, sizeof(request), 0);

        if (strcmp(request, "BASE_FILE") == 0) {
            // Send the base file
            send_base_file(client_sock);
        } else if (strcmp(request, "ALL_OBJECTS") == 0) {
            // Send all objects
            send_all_objects(client_sock);
        }

        // Close the connection
        close(client_sock);
        printf("Connection closed!!!\n");
        memset(request, '\0', sizeof(request));
    }

    return 0;
}

