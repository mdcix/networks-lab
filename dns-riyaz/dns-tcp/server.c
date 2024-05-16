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
    char sbuf[2048], rbuf[2048], a[2048];

    memset(sbuf, '\0', sizeof(sbuf));
    memset(rbuf, '\0', sizeof(rbuf));

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if(server_sock < 0){
        perror("Error while creating socket\n");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        perror("Couldn't bind to the port\n");
        exit(1);
    }

    if(listen(server_sock, 1) < 0){
        perror("Error while listening\n");
        exit(1);
    }
    printf("\nDNS Server Running on port:%d\n", PORT);

    
    //printf("Client connected with\nIP: %s and port: %i\n\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    while (1) {
    client_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_size);
    
    if (client_sock < 0){
        perror("Can't accept\n");
        exit(1);
    }
    recv(client_sock, rbuf, sizeof(rbuf), 0);
    printf("Received Request : %s\n", rbuf);

    FILE *fp = fopen("dns.txt", "r");

        if (fp == NULL) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }
    while (fscanf(fp, "%s", a) == 1) {
        if (strcmp(a, rbuf) == 0) {
            fscanf(fp, "%s", sbuf);
            break;
        }
    }

    if (strcmp(sbuf, "") == 0) {
        strcpy(sbuf, "Not found");
    }

    fclose(fp);
    
    send(client_sock, sbuf, strlen(sbuf), 0);

    printf("Sent Response : %s\n", sbuf);
    
    memset(sbuf, '\0', sizeof(sbuf));
    memset(rbuf, '\0', sizeof(rbuf));
    }

    close(client_sock);
    close(server_sock);
    
    return 0;
}
