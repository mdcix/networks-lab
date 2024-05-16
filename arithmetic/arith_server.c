#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define IP "127.0.0.1"
#define PORT 2000

int performOperation(int num1, int num2, char operator) {
    switch (operator) {
        case '+':
            return num1 + num2;
        case '-':
            return num1 - num2;
        case '*':
            return num1 * num2;
        case '/':
            if (num2 != 0) {
                return num1 / num2;
            } else {
                return 0; // Handle division by zero
            }
        default:
            return -1; // Invalid operator
    }
}

int main() {
    int server_sock, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    char sbuf[1024], rbuf[1024];
    int num1, num2, result;
    char operator;

    memset(sbuf, '\0', sizeof(sbuf));
    memset(rbuf, '\0', sizeof(rbuf));

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Bind socket to server_addr and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP);
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Accept incoming connections
    if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, (socklen_t *)&client_size)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Receive data from the client`
        if (recv(client_sock, rbuf, sizeof(rbuf), 0) <= 0) {
            perror("Receive failed");
            exit(1);
        }

        sscanf(rbuf, "%d %c %d", &num1, &operator, &num2);
        result = performOperation(num1, num2, operator);

        // Send the result back to the client
        sprintf(sbuf, "%d\t\n", result);
        send(client_sock, sbuf, strlen(sbuf), 0);
        
        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));
    }

    return 0;
}
