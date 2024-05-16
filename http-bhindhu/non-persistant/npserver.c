#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
#define PORT 8080

void send_response(int server_sock) {
    char response[] = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello, World!\n\n";
    printf("Sending Response \n%s", response);
    send(server_sock, response, strlen(response), 0);
}

int main() {
    int server_sock, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    char request[2048];

    memset(request, '\0', sizeof(request));
    
    // Create a socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock<0){
        perror("Socket create error");
        exit(1);
    }

    server_addr.sin_family = AF_INET;  
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Bind the socket
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_sock, 3) < 0) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        client_size = sizeof(client_addr);
        if ((client_sock = accept(server_sock, (struct sockaddr*)&client_addr, (socklen_t*)&client_size)) < 0) {
            perror("Accept failed");
            exit(1);
        }
        printf("\nConnection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        recv(client_sock, request, sizeof(request), 0);
        printf("\n\nReceived request:\n%s\n", request);
        // Send a HTTP response
        send_response(client_sock);
        memset(request, '\0', sizeof(request)); 
        printf("\nConnection Closed !!!\n-----------------------------------------\n");
        close(client_sock);
    }

    close(server_sock);

    return 0;
}
