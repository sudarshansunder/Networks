#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

void* Server(void *params)
{
	int ls, s;
	char buffer[256];
	char *ptr = buffer;
	int len = 0;
	int maxlen = sizeof(buffer);
	int n = 0;
	int waitSize = 16;
	struct sockaddr_in servAddr, clntAddr;
	int clntAddrLen;
	
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = 6969;
	
	if((ls = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Error : Listen socket failed");
		exit(1);
	}
	
	if((bind(ls,(struct sockaddr*) &servAddr, sizeof(servAddr))) < 0)
	{
		perror("Error : Bind failed");
		exit(1);
	}
	
	if(listen(ls, waitSize) < 0)
	{
		perror("Error : Listening failed");
		exit(1);
	}
	
	while(1)
	{
		if((s = accept(ls, (struct sockaddr*)&clntAddr, &clntAddrLen)) < 0)
		{
			perror("Error : Accepting failed");
			exit(1);
		}
		n = recv(s, ptr, maxlen, 0);
		send(s, buffer, n, 0);
		close(s);
	}
}

void* Client(void *params)
{
	int s,n;
	char servName[100];
	int servPort;
	char string[100];
	int len = 0, maxlen = 256;
	char buffer[256+1];
	char* ptr = buffer;
	struct sockaddr_in servAddr;
	strcpy(servName, "127.0.0.1");
	servPort = 6969;
	while(1)
	{
		printf("\nEnter a string (Client) (type exit to quit) : ");
		scanf("%s", string);
		if(strcmp(string, "exit") == 0)
			exit(0);
		memset(&servAddr, 0, sizeof(servAddr));
		servAddr.sin_family = AF_INET;
		inet_pton(AF_INET, servName, &servAddr.sin_addr);
		servAddr.sin_port = servPort;
		
		if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		{
			perror("\nError : Socket creation failed");
			exit(1);
		}
		
		if((connect(s, (struct sockaddr*) &servAddr, sizeof(servAddr))) < 0)
		{
			perror("\nError : Connection failed");
			exit(1);
		}
		
		send(s, string, strlen(string), 0);
		
		n = recv(s, ptr, maxlen, 0);	
		buffer[n] = '\0';
		printf("\n\nThe string from server is %s\n", buffer);
		close(s);
	}
}

int main()
{

	pthread_t client, server;
	int res = pthread_create(&server, NULL, Server, NULL);
	if(res)
	{
		printf("\nProblem in creating server thread!");
		exit(1);
	}
	res = pthread_create(&client, NULL, Client, NULL);
	if(res)
	{
		printf("\nProblem in creating client thread!");
		exit(1);
	}
	pthread_join(client, NULL);
	pthread_join(server, NULL);
	pthread_exit(NULL);	
}		