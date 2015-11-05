#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#define BUFFSIZE 2048

void Die(char *mess) 
{
    perror(mess); 
    exit(1); 
}
int main(int argc, char *argv[]) 
{
    
    int sock;
    struct sockaddr_in echoserver;
    char buffer[BUFFSIZE];
    unsigned int echolen;
    int received = 0;
    if (argc != 3) 
    {
        fprintf(stderr, "USAGE: TCPecho <server_ip> <word> <port>\n");
        exit(1);
    }
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
    {
        Die("Failed to create socket");
    }
    memset(&echoserver, 0, sizeof(echoserver));
    echoserver.sin_family = AF_INET;
    echoserver.sin_addr.s_addr = inet_addr(argv[1]);
    echoserver.sin_port = htons(atoi(argv[2]));
    if (connect(sock,(struct sockaddr *) &echoserver,sizeof(echoserver)) < 0) 
    {
        Die("Failed to connect with server");
    }
    char s[100];
    fgets(s,100,stdin);
    echolen = strlen(s);

    /*  send() from client; */
    if (send(sock, s, echolen, 0) != echolen) 
    {
        Die("Mismatch in number of sent bytes");
    }
    fprintf(stdout, "Message from server: ");
    while (received < echolen) 
    {
        int bytes = 0;
        /* recv() from server; */
        if ((bytes = recv(sock, buffer, BUFFSIZE-1, 0)) < 1) 
        {
            Die("Failed to receive bytes from server");
        }
        received += bytes;
        buffer[bytes] = '\0';
        /* Assure null terminated string */
        fprintf(stdout, buffer);
    }

    int bytes = 0;
    do {
        buffer[bytes] = '\0';
        printf("%s\n", buffer);
    } while(bytes = recv(sock, buffer, BUFFSIZE-1, 0));
    printf("%s\n", buffer);
    printf("\n");
    //close(sock);
    if(strcmp(s,"exit") == 0)
    exit(0);

}