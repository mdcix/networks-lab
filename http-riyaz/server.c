#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
#define PORT 8080
#define MAX_BUFFER_SIZE 1024

// Function to send an HTTP response with the given status code and content
void send_response(int client_socket, int status_code, const char* content) {
    char response[MAX_BUFFER_SIZE];
    const char* status_msg = "OK";

    if (status_code == 404) {
        status_msg = "Not Found";
    }

    snprintf(response, sizeof(response),
        "HTTP/1.1 %d %s\r\n"
        "Content-Length: %zu\r\n"
        "Content-Type: text/plain\r\n\r\n%s",
        status_code, status_msg, strlen(content), content);

    send(client_socket, response, strlen(response), 0);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[MAX_BUFFER_SIZE];

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP);
    server_addr.sin_port = htons(PORT);
    

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Socket binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Socket listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept a client connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket == -1) {
            perror("Client connection failed");
            continue;
        }

        // Receive the client's request
        memset(buffer, 0, sizeof(buffer));
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("Received request:\n%s\n", buffer);

        // Extract the requested filename from the HTTP request
        char filename[MAX_BUFFER_SIZE];
        if (sscanf(buffer, "GET /%s HTTP/1.1", filename) == 1) {
            // Open the requested file
            FILE* file = fopen(filename, "r");
            if (file != NULL) {
                // Read the file content
                char file_content[MAX_BUFFER_SIZE];
                size_t file_size = fread(file_content, 1, sizeof(file_content), file);
                fclose(file);

                // Send the file content as the response
                send_response(client_socket, 200, file_content);
            } else {
                send_response(client_socket, 404, "File Not Found");
            }
        } else {
            send_response(client_socket, 400, "Bad Request");
        }

        // Close the client socket
        close(client_socket);
    }

    // Close the server socket (this part of the code is unreachable in this example)
    close(server_socket);

    return 0;
}
