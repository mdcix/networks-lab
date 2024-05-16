#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
#define PORT 8080

void send_http_request(int server_sock, const struct sockaddr_in* server_addr, const char* object_name) {
    char request[256];
    snprintf(request, sizeof(request), "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", object_name, IP);
    // Connect to the server
    if (connect(server_sock, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("\nTCP Connection for %s is Open\n", object_name);

    // Send the HTTP request
    send(server_sock, request, strlen(request), 0);

    char response[4096];
    ssize_t bytes_received;

    // Receive and print the server's response for each object
    while ((bytes_received = recv(server_sock, response, sizeof(response) - 1, 0)) > 0) {
        response[bytes_received] = '\0';
        printf("Received %s\n", object_name);
    }

    // Close the connection for this object
    close(server_sock);

    printf("TCP Connection cloesd !!!\n\n");
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

    char response[64];
    ssize_t bytes_received;

    // Receive the number of objects from the server
    bytes_received = recv(server_sock, response, sizeof(response) - 1, 0);
    printf("Object : %s",response);
    if (bytes_received > 0) {
        response[bytes_received] = '\0';
        int num_objects = atoi(response);
        printf("\nTCP Connection open");
        printf("\nReceived number of objects: %d\n", num_objects);
        printf("\nBase file received\n");
        printf("\nTCP Connection close");

        // Close the initial connection
        close(server_sock);

        // Loop to establish connections for each object
        for (int i = 1; i <= num_objects; i++) {
            // Create a new socket for each object
            int object_server_sock;
            if ((object_server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                perror("Socket creation failed");
                exit(EXIT_FAILURE);
            }

            char object_name[64];
            snprintf(object_name, sizeof(object_name), "object%d", i);

            send_http_request(object_server_sock, &server_addr, object_name);
        }
    }

    return 0;
}