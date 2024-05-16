#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;

    char macAddress[18];
    char receivedIPv6Address[40];

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Server's IP address
    serverAddr.sin_port = htons(8080);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to the server");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    // Input MAC address
    printf("Enter MAC address (e.g., 00:11:22:33:44:55): ");
    scanf("%s", macAddress);

    // Send MAC address to the server
    send(clientSocket, macAddress, sizeof(macAddress), 0);
    printf("Sent MAC address to the server: %s\n", macAddress);

    // Receive IPv6 address from the server
    recv(clientSocket, receivedIPv6Address, sizeof(receivedIPv6Address), 0);
    printf("Received IPv6 address from the server: %s\n", receivedIPv6Address);

    // Close the socket
    close(clientSocket);

    return 0;
}