#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

// Define a struct to represent an email message
struct EmailMessage {
    char* from;
    char* subject;
    char* body;
};

// Function to simulate the POP3 server
void pop3_server(int client_socket) {
    // Simulate the mailbox (in memory) using the EmailMessage struct
    struct EmailMessage mailbox[] = {
        { "sender1@example.com", "Hello", "This is the first message" },
        { "sender2@example.com", "Greetings", "This is the second message" }
    };

    // State: Authorization
    int authorized = 0;
    char user[64];
    char pass[64];

    // Main POP3 server loop
    while (1) {
        char command[512];
        recv(client_socket, command, sizeof(command), 0);
        if (strstr(command, "USER") != NULL) {
            sscanf(command, "USER %s", user);
            char response[] = "+OK User accepted\r\n";
            send(client_socket, response, strlen(response), 0);
        } else if (strstr(command, "PASS") != NULL) {
            sscanf(command, "PASS %s", pass);
            authorized = 1;
            char response[] = "+OK Authorization successful\r\n";
            send(client_socket, response, strlen(response), 0);
        } else if (strstr(command, "STAT") != NULL) {
            if (!authorized) {
                char response[] = "-ERR Authorization required\r\n";
                send(client_socket, response, strlen(response), 0);
            } else {
                int message_count = sizeof(mailbox) / sizeof(mailbox[0]);
                int total_size = 0;
                for (int i = 0; i < message_count; i++) {
                    total_size += strlen(mailbox[i].body);
                }
                char response[128];
                sprintf(response, "+OK %d %d\r\n", message_count, total_size);
                send(client_socket, response, strlen(response), 0);
            }
        } else if (strstr(command, "RETR") != NULL) {
            if (!authorized) {
                char response[] = "-ERR Authorization required\r\n";
                send(client_socket, response, strlen(response), 0);
            } else {
                int message_number;
                sscanf(command, "RETR %d", &message_number);
                printf("hello all");
                if (message_number > 0 && message_number <= sizeof(mailbox) / sizeof(mailbox[0])) {
                    char response[2048];
                    strcpy(response, "+OK\r\n");
                    strcat(response, "From: ");
                    strcat(response, mailbox[message_number - 1].from);
                    strcat(response, "\r\nSubject: ");
                    strcat(response, mailbox[message_number - 1].subject);
                    strcat(response, "\r\nMessage: ");
                    strcat(response, mailbox[message_number - 1].body);
                    strcat(response, "\r\n.\r\n");
                    send(client_socket, response, strlen(response), 0);
                } else {
                    char response[] = "-ERR Message not found\r\n";
                    printf("%ld",sizeof(mailbox));
                    send(client_socket, response, strlen(response), 0);
                }
            }
        } else if (strstr(command, "QUIT") != NULL) {
            char response[] = "+OK POP3 server signing off\r\n";
            send(client_socket, response, strlen(response), 0);
            break;
        } else {
            char response[] = "-ERR Unknown command\r\n";
            send(client_socket, response, strlen(response), 0);
        }
    }

    // Close the client socket
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    struct sockaddr_in server_address, client_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080); // POP3 standard port
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Socket binding failed");
        exit(1);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Listening failed");
        exit(1);
    }

    while (1) {
        socklen_t client_len = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_len);
        if (client_socket == -1) {
            perror("Accepting failed");
            continue;
        }
pop3_server(client_socket);
        // Handle the client in a new process
        // if (fork() == 0) {
        //     close(server_socket); // Child process doesn't need the listening socket
        //     pop3_server(client_socket);
        //     exit(0);
        // } else {
        //     close(client_socket); // Parent process doesn't need the client socket
        // }
    }

    // Close the server socket
    close(server_socket);

    return 0;
}