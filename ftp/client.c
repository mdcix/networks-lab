#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define PORT 2121
#define BUFFER_SIZE 1024

void receive_file(int client_socket, const char* filename) {

    // Send a request for the file
    char request[BUFFER_SIZE];
    snprintf(request, sizeof(request), "GET /%s HTTP/1.1", filename);
    send(client_socket, request, strlen(request), 0);

    // Receive and save the file content
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "received_%s", filename);
    
    FILE* file = fopen(filepath, "wb");
    if (file == NULL) {
        perror("Failed to create file");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_received;

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    fclose(file);
    close(client_socket);

    printf("File received and saved as %s\n", filepath);
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char filename[BUFFER_SIZE];

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    printf("Enter the filename to download: ");
    scanf("%s", filename);

    receive_file(client_socket, filename);

    return 0;
}