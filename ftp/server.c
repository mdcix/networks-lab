#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define SERVER_PORT 2121
#define FILE_PATH "file_server/"  // Directory containing files to share
#define BUFFER_SIZE 1024

void send_file(int client_socket, const char* filename) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s%s", FILE_PATH, filename);
    
    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        perror("File not found");
        return;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    fclose(file);
}

int main() {
    int server_sock, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    char request[BUFFER_SIZE];
    memset(request, '\0', sizeof(request));

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP);
    server_addr.sin_port = htons(SERVER_PORT);

    // Bind the socket
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Socket binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 5) == -1) {
        perror("Socket listening failed");
        exit(EXIT_FAILURE);
    }

    printf("FTP server listening on port %d...\n", SERVER_PORT);

    while (1) {
        // Accept a client connection
        client_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_size);
        if (client_sock < 0) {
            perror("Client connection failed");
            continue;
        }

        // Receive the client's request
        recv(client_sock, request, sizeof(request), 0);
        printf("Received request: %s\n", request);

        char filename[1024];
        sscanf(request, "GET /%s HTTP/1.1", filename);

        if (filename != NULL) {
            send_file(client_sock, filename);
        }

        // Close the client socket
        close(client_sock);
        memset(request, '\0', sizeof(request));
    }

    // Close the server socket (this part of the code is unreachable in this example)
    close(server_sock);

    return 0;
}