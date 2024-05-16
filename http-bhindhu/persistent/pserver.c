#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP "127.0.0.1"
#define PORT 8080

void send_response(int server_sock, int i) {
    char response[] = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello, World!\n";
    printf("Sending Response %d\n%s",i, response);
    send(server_sock, response, strlen(response), 0);
}

int main() {
    int server_sock, client_sock, client_size;
    int count; //To keep count of number of requests
    struct sockaddr_in server_addr, client_addr;
    char request[2048], rbuf[1024];

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
        //Receive number of requests
        recv(client_sock, rbuf, sizeof(rbuf), 0);
        sscanf(rbuf, "%d", &count);
    
        printf("\nConnection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        printf("Number Of Requests = %d\n", count);
    
        for (int i = 1; i <= count; i++) {
            recv(client_sock, request, sizeof(request), 0);
            printf("\n\nReceived request %d:\n%s\n",i ,request);
            // Send a HTTP response
            send_response(client_sock, i);
            memset(request, '\0', sizeof(request));
        }
        close(client_sock);
        printf("\nConnection Closed !!!\n-----------------------------------------\n");
    }
    close(server_sock);
    return 0;
}
