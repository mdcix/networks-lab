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
    int server_sock, client_size;   //no client socket in UDP
    struct sockaddr_in server_addr, client_addr;
    char buf[2048];

    memset(buf, '\0', sizeof(buf));

    server_sock = socket(AF_INET, SOCK_DGRAM, 0);   //Change to DGRAM for udp
    
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

    printf("Echo server is listening on port %d...\n\n", PORT);

    client_size = sizeof(client_addr);

    while (1) {

        if(recvfrom(server_sock, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, &client_size)<0){
            perror("recvfrom failed");
            break;
        }
        
        printf("Client: %s\n", buf);
        
        if (sendto(server_sock, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, client_size) < 0) {
            perror("echo failed");
            break;
        }
        printf("Echoed back !!\n");
        
        memset(buf, '\0', sizeof(buf));
        
    }

    close(server_sock);
    
    return 0;
}
