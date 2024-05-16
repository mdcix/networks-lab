#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

// Define the maximum number of emails to store
#define MAX_EMAILS 10

typedef struct {
    char from[256];
    char to[256];
    char body[1024];
} Email;

typedef struct {
    char username[256];
    char password[256];
} User;

User users[] = {
    {"user1", "password1"},
    {"user2", "password2"},
    // Add more users here
};

Email emails[MAX_EMAILS];
int emailCount = 0;

int authenticateUser(char* username, char* password) {
    int i;
    for (i = 0; i < sizeof(users) / sizeof(User); i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            return 1; // Authentication successful
        }
    }
    return 0; // Authentication failed
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t addr_size;

    char response[1024] = "220 MySMTPServer\r\n";
    char buffer[1024];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Binding failed");
        exit(1);
    }

    if (listen(server_socket, 10) == 0) {
        printf("Listening...\n");
    } else {
        printf("Listening failed\n");
        exit(1);
    }

    addr_size = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr*)&client_address, &addr_size);
    if (client_socket < 0) {
        perror("Accept failed");
        exit(1);
    }

    send(client_socket, response, sizeof(response), 0);

    int authenticated = 0;
    char username[256];
    char password[256];

    while (!authenticated) {
        memset(buffer, 0, sizeof(buffer));
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("%s", buffer);

        if (strstr(buffer, "HELO") != NULL) {
            char hello_response[] = "250 Hello, please enter your username and password\r\n";
            send(client_socket, hello_response, sizeof(hello_response), 0);
        } else if (strstr(buffer, "QUIT") != NULL) {
            char quit_response[] = "221 Bye!\r\n";
            send(client_socket, quit_response, sizeof(quit_response), 0);
            break;
        } else {
            char* ptr;
            if ((ptr = strstr(buffer, "AUTH:")) != NULL) {
                ptr += strlen("AUTH:");
                sscanf(ptr, "%s %s", username, password);

                if (authenticateUser(username, password)) {
                    authenticated = 1;
                    char auth_response[] = "235 Authentication successful\r\n";
                    send(client_socket, auth_response, sizeof(auth_response), 0);
                } else {
                    char auth_fail_response[] = "535 Authentication failed\r\n";
                    send(client_socket, auth_fail_response, sizeof(auth_fail_response), 0);
                }
            } else {
                char unknown_command[] = "500 Unknown command\r\n";
                send(client_socket, unknown_command, sizeof(unknown_command), 0);
            }
        }
    }

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("%s", buffer);

        if (strstr(buffer, "MAIL FROM:") != NULL) {
            // Extract the "FROM" address
            sscanf(buffer, "MAIL FROM: %s", emails[emailCount].from);
            char ok_response[] = "250 OK\r\n";
            send(client_socket, ok_response, sizeof(ok_response), 0);
        } else if (strstr(buffer, "RCPT TO:") != NULL) {
            // Extract the "TO" address
            sscanf(buffer, "RCPT TO: %s", emails[emailCount].to);
            char ok_response[] = "250 OK\r\n";
            send(client_socket, ok_response, sizeof(ok_response), 0);
        } else if (strstr(buffer, "DATA") != NULL) {
            char data_response[] = "354 Start mail input; end with <CRLF>.<CRLF>\r\n";
            send(client_socket, data_response, sizeof(data_response), 0);

            // Receive email body
            char email_body[1024];
            int email_body_length = 0;

            while (1) {
                memset(buffer, 0, sizeof(buffer));
                recv(client_socket, buffer, sizeof(buffer), 0);

                if (strcmp(buffer, ".\r\n") == 0) {
                    break; // End of email body
                }

                // Append to email body
                strcpy(email_body + email_body_length, buffer);
                email_body_length += strlen(buffer);
            }

            // Copy the email body to the email struct
            strcpy(emails[emailCount].body, email_body);

            char ok_response[] = "250 OK\r\n";
            send(client_socket, ok_response, sizeof(ok_response), 0);

            emailCount++;
        } else if (strstr(buffer, "QUIT") != NULL) {
            char quit_response[] = "221 Bye!\r\n";
            send(client_socket, quit_response, sizeof(quit_response), 0);
            break;
        } else {
            char unknown_command[] = "500 Unknown command\r\n";
            send(client_socket, unknown_command, sizeof(unknown_command), 0);
        }
    }

    close(server_socket);
    return 0;
}