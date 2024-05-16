#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
#define PORT 8080
#define OBJECT_FILE "object.txt"

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    int client_size = sizeof(client_addr);

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

    // Open the base file (object.txt) and count the number of lines (objects)
    FILE* base_file = fopen("object.txt", "r");
    if (base_file == NULL) {
        perror("Failed to open base file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    int num_objects = 0;
    while (fgets(line, sizeof(line), base_file) != NULL) {
        num_objects++;
    }
    fclose(base_file);

    printf("Number of objects: %d\n", num_objects);

    while (1) {
        // Accept a new connection
        if ((client_sock = accept(server_sock, (struct sockaddr*)&client_addr, (socklen_t*)&client_size)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("\nConnection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Convert the number of objects to a string
        char num_objects_str[64];
        snprintf(num_objects_str, sizeof(num_objects_str), "%d", num_objects);

        // Send the number of objects as a response
        send(client_sock, num_objects_str, strlen(num_objects_str), 0);
        printf("Object Sent\n");

        // Close the connection
        close(client_sock);
        printf("Connection closed !!!\n");
    }

    return 0;
}
