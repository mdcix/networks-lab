#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define IP "127.0.0.1"
#define PORT 2000

int main(){

    int server_sock, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    char sbuf[2048];

    memset(sbuf, '\0', sizeof(sbuf));

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock<0){
        perror("Socket create error");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        perror("Binding error");
        exit(1);
    }

    if(listen(server_sock, 1)<0){
        perror("Listen error");
        exit(1);
    }
    printf("Time Server listening on port %d\n", PORT);

    while(1){
    client_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_size);

    if(client_sock < 0){
        perror("Error in accepting");
        exit(1);
    }

    printf("\n client connect at \nIP : %s\nPort : %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    //To extract current time in c.. syntax memorise karo
    time_t t;   
    time(&t);
    strcpy(sbuf, ctime(&t));

    if(send(client_sock, sbuf, strlen(sbuf), 0)<0){
        perror("Error while sending");
        exit(1);
    }
    printf("Time sent to client\n");
    close(client_sock);
    }

    close(server_sock);
    

    return 0;
}
