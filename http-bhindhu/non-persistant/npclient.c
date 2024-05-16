#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
#define PORT 8080
#define REQUEST_COUNT 3

void send_request(int server_sock, int request_number) {
    char request[] = "GET / HTTP/1.1\r\nHost: " IP "\r\nConnection: keep-alive\r\n\r\n";
    printf("Sending Request #%d:\n%s\n", request_number, request);
    send(server_sock, request, strlen(request), 0);
}

int main() {
    int server_sock;
    struct sockaddr_in server_addr;
    char response[2048];


    for (int i = 1; i <= REQUEST_COUNT; i++) {
        // Create a socket
        server_sock = socket(AF_INET, SOCK_STREAM, 0);
        if(server_sock<0){
            perror("Socket create error");
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

        send_request(server_sock, i);

        recv(server_sock, response, sizeof(response), 0);
        printf("\nresponse #%d received:\n%s\n", i, response);
        memset(response, '\0', sizeof(response));

        close(server_sock);
    }

    // Close the socket after all requests
    close(server_sock);

    return 0;
}
