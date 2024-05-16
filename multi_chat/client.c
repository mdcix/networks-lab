#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 2000
//10.11.158.24

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024], name[1024], message[2048];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating client socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server.\n");
    printf("Enter your name : ");
    gets(name);

    while (1) {
        printf("Enter a message (or 'quit' to exit): ");
        fgets(buffer, sizeof(buffer), stdin);
        strcat(message, name);
        strcat(message, " says : ");
        strcat(message, buffer);
        send(client_socket, message, strlen(message), 0);

        if (strcmp(buffer, "quit\n") == 0) {
            printf("Client disconnected.\n");
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        memset(message, 0, sizeof(buffer));
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("Server Echo : %s", buffer);
    }

    close(client_socket);

    return 0;
}
