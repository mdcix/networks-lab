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
    char sbuf[BUFFER_SIZE], rbuf[BUFFER_SIZE]; //send buffer, receive buffer
    char request[BUFFER_SIZE], response[BUFFER_SIZE];
    char TLD_Address[BUFFER_SIZE], AUTH_Address[BUFFER_SIZE]; //To store address of tld server and authoritative servers
    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Initialize server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);  // Port number for server1
    server_addr.sin_addr.s_addr = inet_addr(LOCAL_DNS);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(1);
    }

    // Listen for connections
    if (listen(server_socket, 5) == -1) {
        perror("Listening failed");
        exit(1);
    }

    printf("Local DNS listening...\n");

    while (1) {
        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));
        socklen_t addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);

        // Receive domain request from client
        recv(client_socket, rbuf, BUFFER_SIZE, 0);
        printf("\nReceived request: %s\n", rbuf);
        strcpy(request, rbuf);

        const char *ip = searchCache(rbuf); // search in cache

        if (strcmp(ip, "Not Found") != 0){
        send(client_socket, ip, strlen(ip), 0);     // if available in cache, send to client and back to while loop
        printf("IP found in cache : %s\n", ip);
        printf("Sent Response : %s\n", ip);
        close(client_socket);
        continue;
        }

        printf("Not found in cache...\n");      // if not found in cache, we have to contact root server, then tld, then authoritative
        int root_server_socket, tld_server_socket, auth_server_socket;
        struct sockaddr_in root_server_addr, tld_server_addr, auth_server_addr;

        root_server_socket = socket(AF_INET, SOCK_STREAM, 0);   //CONTACTING ROOT SERVER
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

        strcpy(TLD_Address, rbuf);

        printf("Response From Root Server(TLD Address) :  %s\n", TLD_Address); //root server gives address of tld server
        close(root_server_socket);

        if (strcmp(TLD_Address,"Not Found")==0){
            strcpy(sbuf, "No TLD domain found !!!");
            send(client_socket, sbuf, strlen(sbuf), 0);
            printf("Sent Response : %s\n", sbuf);
            close(client_socket);
            continue;
        }

        tld_server_socket = socket(AF_INET, SOCK_STREAM, 0);    //to contact TLD server
        if (tld_server_socket == -1) {
            perror("TLD server Socket creation failed");
            exit(1);
        }

        tld_server_addr.sin_family = AF_INET;
        tld_server_addr.sin_port = htons(2010);
        tld_server_addr.sin_addr.s_addr = inet_addr(TLD_Address);

        if (connect(tld_server_socket, (struct sockaddr *)&tld_server_addr, sizeof(tld_server_addr)) == -1) {
            perror("Connection to tld server failed");
        }
        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));
        strcpy(sbuf, request);

        send(tld_server_socket, sbuf, strlen(sbuf), 0);
        recv(tld_server_socket, rbuf, sizeof(rbuf), 0);
        strcpy(AUTH_Address, rbuf);
        printf("Response From TLD Server(Authoritative Address) : %s\n", AUTH_Address); //TLD server will return address of authoritative ser
        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));

        if (strcmp(AUTH_Address, "Not Found")==0){
            strcpy(sbuf, "No Authoritative server found !!!");
            send(client_socket, sbuf, strlen(sbuf), 0);
            printf("Sent Response : %s\n", sbuf);
            close(client_socket);
            continue;
        }

        auth_server_socket = socket(AF_INET, SOCK_STREAM, 0);       //contacting authoritative server
        if (auth_server_socket == -1) {
            perror("TLD server Socket creation failed");
            exit(1);
        }

        auth_server_addr.sin_family = AF_INET;
        auth_server_addr.sin_port = htons(2015);
        auth_server_addr.sin_addr.s_addr = inet_addr(AUTH_Address);

        if (connect(auth_server_socket, (struct sockaddr *)&auth_server_addr, sizeof(auth_server_addr)) == -1) {
            perror("Connection to auth server failed");
        }
        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));
        strcpy(sbuf, request);

        send(auth_server_socket, sbuf, strlen(sbuf), 0);
        recv(auth_server_socket, rbuf, sizeof(rbuf), 0);
        strcpy(response, rbuf);
        printf("Response From Authoritative Server(IP Address) : %s\n", response);      //Receive ip address

        if (strcmp(response, "Not Found")==0){
            strcpy(sbuf, "No IP entry Found !!!");
            send(client_socket, sbuf, strlen(sbuf), 0);
            printf("Sent Response : %s\n", sbuf);
            close(client_socket);
            continue;
        }

        memset(sbuf, '\0', sizeof(sbuf));
        strcpy(sbuf,response);
        send(client_socket, sbuf, strlen(sbuf), 0);
        printf("Sent Response : %s\n", sbuf);
        close(client_socket);
        memset(sbuf, '\0', sizeof(sbuf));
        memset(rbuf, '\0', sizeof(rbuf));
        printf("\n%s : %s added to CACHE\n", request, response);
        addToCache(request, response);
    }

    close(server_socket);
    return 0;
}