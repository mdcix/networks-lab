#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void simulateSLAAC(char* macAddress, char* ipv6Address) {
    // Simulate SLAAC (same as in the previous example)
    srand(time(NULL));
    int randomBits = rand() % 65536;  // 16 bits
    sprintf(ipv6Address, "2001:db8::%s:%04x", macAddress, randomBits);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(struct sockaddr_in);

    char macAddress[18];
    char ipv6Address[40];

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("Error listening");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8080...\n");

    // Accept a connection
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize);
    if (clientSocket == -1) {
        perror("Error accepting connection");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Receive MAC address from the client
    printf("Waiting for MAC address from the client...\n");
    recv(clientSocket, macAddress, sizeof(macAddress), 0);
    printf("Received MAC address from the client: %s\n", macAddress);

    // Simulate SLAAC
    simulateSLAAC(macAddress, ipv6Address);

    // Send the generated IPv6 address to the client
    send(clientSocket, ipv6Address, sizeof(ipv6Address), 0);
    printf("Sent IPv6 address to the client: %s\n", ipv6Address);

    // Close sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}