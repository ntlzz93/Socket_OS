#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>

#define MAXPENDING 5
#define BUFFSIZE 2048
#define MAX 2048
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
/* This while block has been removed. It causes the server to wait for more data from client when all data are already received.
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
*/

    puts (data);
        char *args[100];
        setup(data,args,0);
        int pipefd[2],lenght;

        if(pipe(pipefd))
            Die("Failed to create pipe");

        pid_t pid = fork();
        char path[MAX];

        if(pid==0)
        {
            close(1); // close the original stdout
            dup2(pipefd[1],1); // duplicate pipefd[1] as stdout
            close(pipefd[0]); // close the readonly end of the pipe
            close(pipefd[1]); // close the original writeonly end of the pipe
            execvp(args[0],args); // execute the command. this will not return and will exit at completion unless the executed args[0] doesn't end.
        }
        else
            if(pid>0)
            {
                close(pipefd[1]);
                memset(path,0,MAX);
                while(lenght=read(pipefd[0],path,MAX-1)){
                    printf("%s\n", path);
                    if(send(sock,path,strlen(path),0) != strlen(path) ){
                        Die("Failed");
                    }
                    fflush(NULL);
                    //printf("Data sent so far %s\n", path);
                memset(path,0,MAX);//
                }
                close(pipefd[0]);
                //return;// This causes the process to terminate instead of returning to main(). Use return instead.
            }
            else
            {
                printf("Error !\n");
                exit(0);//
            }
    if (strcmp (data, "exits")==0)
    {
	printf("Closing socket\n");
        exit (1);
    }
    
    close(sock);
}

int main(int argc, char const *argv[])
{

    int serversock,clientsock;
    struct sockaddr_in echoserver, echoclient;
    signal(SIGPIPE, SIG_IGN); //ignoring SIGPIPE to allow the code to handle the failures instead of crashing with SIGPIPE.
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
