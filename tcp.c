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
		char res[100];
		n = recv(s, ptr, maxlen, 0);
		int fd = open(ptr, 2);
		if(fd == -1)
		{
			strcpy(res, "File requested not found!");
			send(s, res, strlen(res), 0);
		}
		else
		{
			n = read(fd, res, 500);
			send(s, res, n, 0);
		}
		close(s);
	}
}

void* Client(void *params)
{
	int s,n;
	char servName[100];
	int servPort;
	char string[10];
	int len = 0, maxlen = 256;
	char buffer[256+1];
	char* ptr = buffer;
	struct sockaddr_in servAddr;
	strcpy(servName, "127.0.0.1");
	servPort = 6969;
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

	printf("\nEnter the file name : ");
	scanf("%s", string);
	send(s, string, sizeof(string), 0);
	n = recv(s, ptr, maxlen, 0);	
	ptr[n] = '\0';
	printf("\nReceived from server : %s\n", buffer);
	if(strcmp(buffer, "File requested not found!") == 0)
	{
		printf("\nFile not found at the server!\n");
		exit(1);
	}
	strcat(string, "copy");
	int fd = creat(string, 0666);
	if(fd == -1)
	{
		perror("\nFile cannot be created!");
		exit(1);
	}
	write(fd, buffer, n);
	printf("\nFile has been created!");
	close(s);
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
	printf("\n");
}		