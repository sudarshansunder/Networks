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

int main()
{
	int s,n;
	char servName[100];
	int servPort;
	char string[10];
	int len = 0, maxlen = 256;
	char buffer[256+1];
	char* ptr = buffer;
	struct sockaddr_in servAddr;
	printf("\nEnter the server's IP address : ");
	scanf("%s", string);
	strcpy(servName, string);
	servPort = 6901;
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
	while(1)
	{
		printf("\n");
		printf("\nEnter the file name to list (-1 to stop) : ");
		scanf("%s", string);
		if(strcmp(string, "-1") == 0)
			break;
		char sendData[50];
		sendData[0] = '3';
		sendData[1] = '\0';
		strcat(sendData, string);
		send(s, sendData, sizeof(sendData), 0);
		n = recv(s, ptr, maxlen, 0);	
		buffer[n] = '\0';
		printf("\nData from the server is %s", buffer);
		fflush(stdout);
	}
	close(s);
	exit(1);
}