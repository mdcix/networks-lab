#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char response[1024];
    char buffer[1024];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address) )< 0) {
        perror("Connection failed");
        exit(1);
    }

    recv(client_socket, response, sizeof(response), 0);
    printf("%s", response);

    char username[256];
    char password[256];

    // Get username and password from the user
    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("Enter your password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';

    // Send AUTH command
    char auth_command[512];
    sprintf(auth_command, "AUTH: %s %s\r\n", username, password);
    send(client_socket, auth_command, strlen(auth_command), 0);
    recv(client_socket, response, sizeof(response), 0);
    printf("%s", response);

    if (strstr(response, "235 Authentication successful") == NULL) {
        printf("Authentication failed.\n");
        close(client_socket);
        return 1;
    }

    char from_address[256];
    char to_address[256];
    char email_body[1024];

    // Get FROM, TO, and email body from the user
    printf("Enter FROM address: ");
    fgets(from_address, sizeof(from_address), stdin);
    from_address[strcspn(from_address, "\n")] = '\0';

    printf("Enter TO address: ");
    fgets(to_address, sizeof(to_address), stdin);
    to_address[strcspn(to_address, "\n")] = '\0';

    printf("Enter email body (end with a line containing only a period \".\"): \n");
    fgets(email_body, sizeof(email_body), stdin);
    email_body[strcspn(email_body, "\n")] = '\0';

    // Send FROM, TO, and DATA commands
    char from_command[512];
    sprintf(from_command, "MAIL FROM: %s\r\n", from_address);
    send(client_socket, from_command, strlen(from_command), 0);
    recv(client_socket, response, sizeof(response), 0);
    printf("%s", response);

    char to_command[512];
    sprintf(to_command, "RCPT TO: %s\r\n", to_address);
    send(client_socket, to_command, strlen(to_command), 0);
    recv(client_socket, response, sizeof(response), 0);
    printf("%s", response);

    char data_command[] = "DATA\r\n";
    send(client_socket, data_command, sizeof(data_command), 0);
    recv(client_socket, response, sizeof(response), 0);
    printf("%s", response);

    // Send email body
    send(client_socket, email_body, strlen(email_body), 0);

    // Send a period to end the email body
    char period_command[] = ".\r\n";
    send(client_socket, period_command, sizeof(period_command), 0);
    recv(client_socket, response, sizeof(response), 0);
    printf("%s", response);

    // Send QUIT command to end the session
    char quit_command[] = "QUIT\r\n";
    send(client_socket, quit_command, sizeof(quit_command), 0);
    recv(client_socket, response, sizeof(response), 0);
    printf("%s", response);

    close(client_socket);

    return 0;
}