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
#include "encrypt.h"

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
	servAddr.sin_port = 6900;
	
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
	int auth = 1;
	int s,n;
	char servName[100];
	int servPort;
	char string[20];
	int len = 0, maxlen = 256;
	char buffer[256+1];
	char* ptr = buffer;
	struct sockaddr_in servAddr;
	strcpy(servName, "127.0.0.1");
	servPort = 6901;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	inet_pton(AF_INET, servName, &servAddr.sin_addr);
	servAddr.sin_port = servPort;
	
	int ch;
	do
	{

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
		printf("\n1. Login 2. Download a file 3. List a file 4. Exit\n");
		scanf("%d", &ch);
		char sendData[50];
		switch(ch)
		{
			case 1 : printf("\nEnter the password : ");
					 scanf("%s", string);
					 char encString[20];
					 encrypt(string, encString, strlen(string));
					 sendData[0] = '1';
					 sendData[1] = '\0';
					 strcat(sendData, encString);
					 printf("\nData sent to server is %s", sendData);
					 send(s, sendData, strlen(sendData), 0);
					 n = recv(s, buffer, strlen(buffer), 0);
					 buffer[n] = '\0';
					 //printf("\nReceived from server : %s", buffer);
					 if(strcmp(buffer, "Failur") == 0)
					 {
					 	printf("\nInvalid password, please try again!");
					 	auth = 0;
					 }
					 else if(strcmp(buffer, "Succes") == 0)
					 {
					 	printf("\nLogin successful!");
					 	auth = 1;
					 }
					 close(s);
					 break;
			case 2 : if(auth)
					 {
					 	printf("\nEnter file name : ");
					 	scanf("%s", string);
					 	sendData[0] = '2';
					 	sendData[1] = '\0';
					 	strcat(sendData, string);
					 	printf("\nData send to server is %s", sendData);
					 	send(s, sendData, strlen(sendData), 0);
					 	ptr = buffer;
					 	int len = 0;
					 	while((n = recv(s, ptr, strlen(buffer), 0)) > 0)
					 	{		
					 		ptr += n;
					 		len += n;
					 	}
					 	buffer[len] = '\0';
					 	printf("\nData received from server is %s len = %d", buffer, len);
					 }
					 else
					 {
					 	printf("\nYour are not authenticated, please login and try again!");
					 }
					 close(s);
					 break;

		}
	} while(ch != 4);
	close(s);
	exit(1);
}

int main()
{

	pthread_t client, server;
	int res;
	/*res = pthread_create(&server, NULL, Server, NULL);
	if(res)
	{
		printf("\nProblem in creating server thread!");
		exit(1);
	}*/
	res = pthread_create(&client, NULL, Client, NULL);
	if(res)
	{
		printf("\nProblem in creating client thread!");
		exit(1);
	}
	pthread_join(client, NULL);
	//pthread_join(server, NULL);
	pthread_exit(NULL);	
	printf("\n");
}		