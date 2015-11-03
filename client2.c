#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#define BUFFSIZE 32
/// Client chuyen mot chuoi ki tu, sau khi an enter n gui cho server roi ket thuc
void Die(char *mess) 
{
	perror(mess); 
 	exit(1); 
}
int main(int argc, char *argv[]) 
{
while(1){
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
/* Create the TCP socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
	{
		Die("Failed to create socket");
	}
/* Construct the server sockaddr_in structure */
	memset(&echoserver, 0, sizeof(echoserver));
	echoserver.sin_family = AF_INET;
	echoserver.sin_addr.s_addr = inet_addr(argv[1]);
	echoserver.sin_port = htons(atoi(argv[2]));
/* Establish connection */
	if (connect(sock,(struct sockaddr *) &echoserver,sizeof(echoserver)) < 0) 
	{
		Die("Failed to connect with server");
	}
/* Send the word to the server */
	//tao mot chuoi s, chuoi s nhan ki tu tu ban phim, roi chuyen cho phia server
	char s[100];
	fgets(s,100,stdin);
	echolen = strlen(s);
	if (send(sock, s, echolen, 0) != echolen) 
	{
		Die("Mismatch in number of sent bytes");
	}
/* Receive the word back from the server */
	fprintf(stdout, "Message from server: ");
	while (received < echolen) 
	{
		int bytes = 0;
		if ((bytes = recv(sock, buffer, BUFFSIZE-1, 0)) < 1) 
		{
			Die("Failed to receive bytes from server");
		}
		received += bytes;
		buffer[bytes] = '\0';
		/* Assure null terminated string */
		fprintf(stdout, buffer);
	}
	fprintf(stdout, "\n");
	close(sock);
	if(strcmp(s,"exit") == 0)
	exit(0);
}
}
