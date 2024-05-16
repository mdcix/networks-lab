#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define PORT 2000

int main() {
    int server_sock;
    struct sockaddr_in server_addr;
    char sbuf[1024], rbuf[1024];
    
    memset(sbuf, '\0', sizeof(sbuf));
    memset(rbuf, '\0', sizeof(rbuf));

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    while (1) {

        printf("Enter arithmetic operation (e.g., 5 + 3) or ( -1 to quit): ");
        gets(sbuf);

        if (strcmp(sbuf, "-1")==0){
            break;
        }

        // Send the operation to the server
        send(server_sock, sbuf, strlen(sbuf), 0);

        // Receive and display the result from the server
        recv(server_sock, rbuf, sizeof(rbuf), 0);
        printf("Server response: %s\n", rbuf);

        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));
    }

    close(server_sock);
    return 0;
}

