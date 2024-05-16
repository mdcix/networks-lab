#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_DOMAINS 100
#define BUFFER_SIZE 256
#define LOCAL_DNS "127.0.0.1"
#define ROOT_DNS "127.0.0.2"

struct DomainIPCache {
    char domain[BUFFER_SIZE];
    char ip[BUFFER_SIZE];
};

struct DomainIPCache cache[MAX_DOMAINS];
int numDomains = 0;

void addToCache(const char *domain, const char *ip) {
    if (numDomains < MAX_DOMAINS) {
        strcpy(cache[numDomains].domain, domain);
        strcpy(cache[numDomains].ip, ip);
        numDomains++;
    }
}

const char* searchCache(const char *domain) {
    for (int i = 0; i < numDomains; i++) {
        if (strcmp(domain, cache[i].domain) == 0) {
            return cache[i].ip;
        }
    }
    return "Not Found";
}

int main() {

    addToCache("example.com", "192.168.1.1");

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char sbuf[BUFFER_SIZE], rbuf[BUFFER_SIZE];
    char request[BUFFER_SIZE], response[BUFFER_SIZE];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Initialize server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);  
    server_addr.sin_addr.s_addr = inet_addr(LOCAL_DNS);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(1);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listening failed");
        exit(1);
    }

    printf("Local DNS listening...\n");

    while (1) {
        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));
        memset(request, '\0', strlen(request));
        memset(response, '\0', strlen(response));

        socklen_t addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);

        // Receive domain request from client
        recv(client_socket, rbuf, BUFFER_SIZE, 0);
        printf("\nReceived request from client : %s\n", rbuf);
        strcpy(request, rbuf);
        // Find the IP for the requested domain in cache
        const char *ip = searchCache(rbuf);
        strcpy(sbuf, ip);

        if (strcmp(ip, "Not Found") != 0){          //if found in cache, respond to client, back to while loop
        send(client_socket, sbuf, strlen(sbuf), 0);
        printf("IP found in cache : %s\n", sbuf);
        printf("Sent Response : %s\n", sbuf);
        close(client_socket);
        continue;
        }

        printf("Not found in cache...\n");
        int root_server_socket;
        struct sockaddr_in root_server_addr;

        root_server_socket = socket(AF_INET, SOCK_STREAM, 0);   //contact root server
        if (root_server_socket == -1) {
            perror("Root server Socket creation failed");
            exit(1);
        }

        root_server_addr.sin_family = AF_INET;
        root_server_addr.sin_port = htons(2005);
        root_server_addr.sin_addr.s_addr = inet_addr(ROOT_DNS);

        if (connect(root_server_socket, (struct sockaddr *)&root_server_addr, sizeof(root_server_addr)) == -1) {
            perror("Connection to root server failed");
        }

        memset(rbuf, '\0', sizeof(rbuf));
        memset(sbuf, '\0', sizeof(sbuf));
        strcpy(sbuf, request);

        printf("Sending request to Root server :  %s\n", sbuf);

        send(root_server_socket, sbuf, strlen(sbuf), 0);

        recv(root_server_socket, rbuf, sizeof(rbuf), 0);
        printf("Received response from Root server :  %s\n", rbuf);

        strcpy(response, rbuf);

        close(root_server_socket);

        memset(sbuf, '\0', sizeof(sbuf));
        strcpy(sbuf,response);
        send(client_socket, sbuf, strlen(sbuf), 0);
        printf("Sending response to client : %s\n", sbuf);
        close(client_socket);

        if(strstr(response, "Not Found") == NULL){
            printf("######%s : %s added to CACHE#####\n", request, response);
            addToCache(request, response);
        }
    }

    close(server_socket);
    return 0;
}