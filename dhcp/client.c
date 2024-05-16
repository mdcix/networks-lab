#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 67
#define CLIENT_PORT 68
#define MAX_BUFFER_SIZE 1024

// DHCP message types
#define DHCP_DISCOVER 1
#define DHCP_OFFER 2
#define DHCP_REQUEST 3
#define DHCP_ACK 5

// DHCP header structure
struct dhcp_header {
    uint8_t op;      // Message type (e.g., DHCP_DISCOVER, DHCP_OFFER)
    uint8_t htype;   // Hardware address type
    uint8_t hlen;    // Hardware address length
    uint8_t hops;    // Hops
    uint32_t xid;    // Transaction ID
    uint16_t secs;   // Seconds elapsed
    uint16_t flags;  // Flags
    uint32_t ciaddr; // Client IP address
    uint32_t yiaddr; // Your (client) IP address
    uint32_t siaddr; // Server IP address
    uint32_t giaddr; // Relay agent IP address
    uint8_t chaddr[16]; // Client hardware address
    uint8_t padding[192]; // Padding to make the structure 300 bytes
    uint32_t magic_cookie; // DHCP magic cookie
    uint8_t options[64];   // DHCP options
};

// Function to send a DHCP message
void send_dhcp_message(int socket_fd, struct sockaddr_in server_addr, struct dhcp_header *dhcp_msg) {
    sendto(socket_fd, dhcp_msg, sizeof(struct dhcp_header), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));
}

int main() {
    // Create a socket
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Set up client address
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(CLIENT_PORT);
    client_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the client address
    if (bind(socket_fd, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1) {
        perror("Socket binding failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    // Build DHCP Discover message
    struct dhcp_header dhcp_msg;
    memset(&dhcp_msg, 0, sizeof(struct dhcp_header));
    dhcp_msg.op = DHCP_DISCOVER;
    dhcp_msg.htype = 1; // Ethernet
    dhcp_msg.hlen = 6;  // Ethernet address length
    dhcp_msg.xid = htonl(12345678); // Transaction ID
    memcpy(dhcp_msg.chaddr, "\x00\x0C\x29\xB7\x66\x28", 6); // Client MAC address
    dhcp_msg.magic_cookie = htonl(0x63825363); // DHCP magic cookie

    // Send DHCP Discover message
    send_dhcp_message(socket_fd, server_addr, &dhcp_msg);
    printf("Sent DHCP Discover to server\n");

    // Receive DHCP Offer or DHCP Ack message
    recvfrom(socket_fd, &dhcp_msg, sizeof(struct dhcp_header), 0, NULL, NULL);

    // Process DHCP Offer or DHCP Ack message
    switch (dhcp_msg.op) {
        case DHCP_OFFER:
            printf("Received DHCP Offer from server\n");
            // Process DHCP Offer, e.g., extract offered IP address
            break;

        case DHCP_ACK:
            printf("Received DHCP Ack from server\n");
            // Process DHCP Ack, e.g., extract assigned IP address
            break;

        default:
            printf("Unexpected DHCP message type\n");
            break;
    }

    // Close the socket
    close(socket_fd);

    return 0;
}