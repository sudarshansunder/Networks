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
#include "utils.h"
#include <fcntl.h> 

void downloadFiles(char buffer[], int len, char fileName[])
{
	int s;
	int num = buffer[0] - '0' + 1, i;
	buffer[len-1] = '\0';
	char **ips;
	ips = str_split(buffer, '#');
	char data[256+1];
	int fd = creat(fileName, 0666);
	printf("\nDownload files : ");
	for(i=0;i<num;i++)
		printf("%s ", ips[i]);
	for(i=1;i<num;i++)
	{
		struct sockaddr_in peerAddr;
		memset(&peerAddr, 0, sizeof(peerAddr));
		peerAddr.sin_family = AF_INET;
		inet_pton(AF_INET, ips[i], &peerAddr.sin_addr);
		peerAddr.sin_port = 4014;
		if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		{
			perror("\nError : Socket creation failed");
			exit(1);
		}

		if((connect(s, (struct sockaddr*) &peerAddr, sizeof(peerAddr))) < 0)
		{	
			perror("\nError : Connection failed");
			exit(1);
		}

		char sendData[50];
		sendData[0] = '0' + (num-1);
		sendData[1] = '#';
		sendData[2] = i + '0';
		sendData[3] = '#';
		sendData[4] = '\0';
		strcat(sendData, fileName);
		printf("\nData send to peer is %s", sendData);
		send(s, sendData, strlen(sendData), 0);
		int n = recv(s, data, strlen(data) + 200, 0);
		data[n] = '\0';
		write(fd, data, strlen(data));
		close(s);
	}
}

void Server()
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
	servAddr.sin_addr.s_addr = inet_addr("192.168.43.190");
	servAddr.sin_port = 4014;
	
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
		
		n = recv(s, buffer, strlen(buffer) + 20, 0);
		buffer[n] = '\0';

		char **strs = str_split(buffer, '#');

		int tot = strs[0][0] - '0';
		int part = strs[1][0] - '1'; //Assume single digit numbers
		char fileName[20];
		strcpy(fileName, strs[2]);

		int i;
		for(i=0;i<3;i++)
		{
			printf("%s ", strs[i]);
		}

		fflush(stdout);

		int fd = open(fileName, 2);

		if(fd == -1)
		{
			printf("\nCannot open file at peer");
			exit(0);
		}

		n = read(fd, buffer, 200);
		int bpp = n/tot;		
		int fp = bpp * (part);

		buffer[n] = '\0';

		printf("\nBuffer is %s", buffer);		

		printf("\nn = %d bpp = %d fp = %d tot = %d part = %d", n, bpp, fp, tot, part);
		
		fflush(stdout);

		char str[200];
		int j=0;
		for(i=fp;i<fp+bpp;i++,j++)
			str[j] = buffer[i];

		str[j+1] = '\0';		
		printf("\nValue of buffer %s", str);
		fflush(stdout);			
		send(s, str, j, 0);
		close(s);
	}
}

void Client()	
{
	int auth = 0;
	int s,n;
	char servName[100];
	int servPort;
	char string[20];
	int len = 0, maxlen = 256;
	char buffer[256+1];
	char* ptr = buffer;
	struct sockaddr_in servAddr;
	strcpy(servName, "192.168.43.47");
	servPort = 8000;
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
			case 1 :
					 if(!auth)
					 {
					 	printf("\nEnter the password : ");
						 scanf("%s", string);
						 char encString[20];
						 encrypt(string, encString, strlen(string));
						 sendData[0] = '1';
						 sendData[1] = '\0';
						 strcat(sendData, encString);
						 send(s, sendData, strlen(sendData), 0);
					 	 n = recv(s, ptr, strlen(buffer)+1, 0);
				
					 	 buffer[n] = '\0';
						 printf("\nBuffer from server is %s -> %d", buffer,n);
						 fflush(stdout);
						 if(strcmp(buffer, "0") == 0)
						 {
						 	printf("\nInvalid password, please try again!");
						 	auth = 0;
						 }
						 else if(strcmp(buffer, "1") == 0)
						 {
						 	printf("\nLogin successful!");
						 	auth = 1;
						 }
					 }
					 else
					 {
					 	printf("\nAlready logged in!");
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
					 	send(s, sendData, strlen(sendData), 0);
					 	ptr = buffer;
					 	int len = 0;
					 	n = recv(s, ptr, strlen(buffer)+50, 0);
					 	buffer[n] = '\0';
						printf("Server has sent: %s -> %d",buffer,n);
					 	if(strcmp(buffer, "0#") == 0)
					 	{
					 		printf("\nNo peer has the file you request.");
					 	}
					 	else
					 	{
					 		downloadFiles(buffer, len, string);
					 	}
					 }
					 else
					 {
					 	printf("\nYour are not authenticated, please login and try again!");
					 }
					 close(s);
					 break;
			case 3 : if(auth)
					 {
					 	printf("\nWhat file would you like to list : ");
					 	scanf("%s", string);
					 	int fd = open(string, 2);
					 	if (fd == -1)
					 	{
					 		printf("\nYou don't have the file da bastard");
					 	}
					 	else
					 	{
					 		close(fd);
						 	sendData[0] = '3';
						 	sendData[1] = '\0';
						 	strcat(sendData, string);
						 	printf("\nData sent to server is %s", sendData);
							fflush(stdout);
						 	send(s, sendData, strlen(sendData), 0);
						 	ptr = buffer;
					 		n = recv(s, ptr, strlen(buffer)+1, 0);
					 		buffer[n] = '\0';
							fflush(stdout);
					 	}
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
	int pid = fork();
	if(pid == 0)
	{
		Server();
	}
	else
	{
		Client();
	}
	printf("\n");
}		
