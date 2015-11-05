#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

#define MAXPENDING 5
#define BUFFSIZE 32
#define MAX 128
void Die(char *mess) 
{
	perror(mess);
	exit(1);
}
void setup(char inputBuffer[], char *args[], int *background) //Ham xu li (cat) chuoi lenh
{
		const char s[4] = " \t\n";
		char *token;
		token = strtok(inputBuffer, s);
		int i = 0;
		while( token != NULL)
		{
			args[i] = token;
			i++;
			//printf("%s\n", token);
			token = strtok(NULL,s);
		}
		args[i] = NULL;
}
void HandeClient(int sock){

char buffer[BUFFSIZE];
int received = -1;
char data[MAX];

	data[0] = '\0';

	if((received = recv(sock, buffer,BUFFSIZE,0))<0){

		Die("Failed");
	}

	buffer[received] = '\0';
	strcat (data,  buffer);
	while(received>0)
	{
		if(send(sock,buffer, received,0)!= received){
			Die("Failed");
		}
		if((received=recv(sock,buffer,BUFFSIZE,0))<0){
			Die("Failed");
		}

		buffer[received] = '\0';
		strcat (data, buffer);
	}

	puts (data);
	{
		char *args[100];
		setup(data,args,0);
		pid_t pid = fork();
		if(pid>0)
		{
			
	    	while (wait(NULL) != pid);
		}
		else
			if(pid==0)
			{
				//child process
				execvp(args[0],args);
				exit(1);//
			}
			else
			{
				printf("Error !\n");
				exit(0);//
			}
	}
	if (strcmp (data, "exit")==0)
	{
		printf("%s\n","server closed");
		close(sock);
		exit (1);
	}
	close(sock);
}
int main(int argc, char const *argv[])
{
	int serversock,clientsock;
	struct sockaddr_in echoserver, echoclient;
	/*if(argc !=2)
	{
		fprintf(stderr, "USAGE: echoserver <port>\n");
		exit(0);
	}*/
	if((serversock = socket(PF_INET, SOCK_STREAM,IPPROTO_TCP))<0){
		Die("Failed");
	}
	memset(&echoserver,0,sizeof(echoserver));
	echoserver.sin_family = AF_INET;
	echoserver.sin_addr.s_addr= htonl(INADDR_ANY);
	echoserver.sin_port = htons(atoi(argv[1]));

	if(bind(serversock, (struct sockaddr *) & echoserver,sizeof(echoserver))<0){
		Die("Failed");
	}
	if(listen(serversock,MAXPENDING)<0){
		Die("Failed");
	}
	while(1)
	{
		unsigned int clientlen = sizeof(echoclient);
		if((clientsock = 
			accept(serversock,(struct sockaddr *) &echoclient,
								&clientlen))<0){
			Die("Failed");
		}
		fprintf(stdout, "Client connected: %s\n",
						inet_ntoa(echoclient.sin_addr));
		fprintf(stdout,"Message from client:");
		HandeClient(clientsock);		
	}

	return 0;
}
