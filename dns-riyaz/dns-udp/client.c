#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>

#define PORT 3000
#define BUFFER_SIZE 1024

int main(){
    int cli_sock;
    struct sockaddr_in serv_addr;
    char sbuf[BUFFER_SIZE],rbuf[BUFFER_SIZE];
    socklen_t addrlen=sizeof(serv_addr);
    
    cli_sock=socket(AF_INET,SOCK_DGRAM,0);
    
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(PORT);
    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    
    
    printf("Enter the message: ");
    scanf("%s",sbuf);
    sendto(cli_sock,sbuf,BUFFER_SIZE,0,(struct sockaddr *)&serv_addr,addrlen);
    recvfrom(cli_sock,rbuf,sizeof(rbuf),0,(struct sockaddr *)&serv_addr,&addrlen);
    printf("Received message: %s\n",rbuf);

    close(cli_sock);
    return 0;
}