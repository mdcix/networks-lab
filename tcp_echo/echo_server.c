#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define PORT 2000

int main(void)
{
    int server_sock, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    char buf[2048]; //single buffer enough for echo

    memset(buf, '\0', sizeof(buf));

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if(server_sock < 0){
        perror("Error while creating socket\n");
        exit(1);
    }
    printf("Socket created successfully\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        perror("Couldn't bind to the port\n");
        exit(1);
    }
    printf("Binding Done\n");

    if(listen(server_sock, 1) < 0){
        perror("Error while listening\n");
        exit(1);
    }
    printf("Server listening on port %d\n", PORT);

    client_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_size);
    
    if (client_sock < 0){
        perror("Can't accept\n");
        exit(1);
    }
    printf("Client connected with\nIP: %s and port: %i\n\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    while (1) {
    
    recv(client_sock, buf, sizeof(buf), 0);
    printf("Message from client: %s\n", buf);

    send(client_sock, buf, strlen(buf), 0); //echo back the same message
    printf("Echoed back\n");
    
    memset(buf, '\0', sizeof(buf));
    }

    close(client_sock);
    close(server_sock);
    
    return 0;
}
