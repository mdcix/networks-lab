#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define PORT 3000
#define BUFFER_SIZE 1024

int main(){
    int server_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    char sbuf[BUFFER_SIZE], rbuf[BUFFER_SIZE], a[BUFFER_SIZE];

    server_sock = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP);
    server_addr.sin_port = htons(PORT);

    bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("DNS Server running on port:%d\n", PORT);
    while (1) {
        strcpy(sbuf, "");
        FILE *fp = fopen("dns.txt", "r");

        if (fp == NULL) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }

        client_size = sizeof(client_addr);

        recvfrom(server_sock, rbuf, sizeof(rbuf), 0, (struct sockaddr *)&client_addr, &client_size);
        printf("Received request: %s\n", rbuf);

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
        sendto(server_sock, sbuf, sizeof(sbuf), 0, (struct sockaddr *)&client_addr, client_size);
    }

    close(server_sock);

    return 0;
}