#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

int main() {
    int client_socket;

    // Create a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Define the server address (replace with your server IP address)
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080); // POP3 standard port
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with your server's IP address

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Connection failed");
        exit(1);
    }
    while(1){
    char imap_command[512];
    printf("Enter an IMAP command: ");
    scanf("%s", imap_command);

    // Send the IMAP command to the server
    send(client_socket, imap_command, strlen(imap_command), 0);

    // Receive and print the server's response
    char response[1024];
    int bytes_received = recv(client_socket, response, sizeof(response), 0);
    if (bytes_received > 0) {
        response[bytes_received] = '\0'; // Null-terminate the response
        printf("Server Response: %s", response);
    } else if (bytes_received == 0) {
        printf("Connection closed by the server.\n");
    } else {
        perror("Receive failed");
    }
    }

    // Close the client socket
    close(client_socket);

    return 0;
}