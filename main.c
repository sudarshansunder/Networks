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
#include "hosts.h"
#include "encrypt.h"

/*
Flags for login, file download, list file

	1 - Login
	2 - Download
	3 - List

Compile using  cc -o main main.c -fno-stack-protector

*/

int loginUser(char buffer[], int n, char pwd[])
{
	int i;
	buffer[n] = '\0';
	char pass_enc[20];
	for(i=1;i<n;i++)
		pass_enc[i-1] = buffer[i];
	char pass[20];
	decrypt(pass_enc, pass, n-1);
	printf("\nDecrypted password at server is %s", pass);
	fflush(stdout);
	return !strcmp(pass, pwd);
}

void downloadFile(Host* head, char buffer[], int n, char data[])
{
	int i;
	char fileName[20];
	printf("\nValue of n is %d", n);
	fflush(stdout);
	for(i=1;i<n;i++)
		fileName[i-1] = buffer[i];
	char vals[10][20];
	fileName[n-1] = '\0';
	printf("\nFile name is %s", fileName);
	host_search(head, fileName, vals);
	int k = vals[0][0] - '0';
	data[0] = '\0';
	int count = 0;
	for(i=0;i<=k;i++)
	{
		printf("\nvals[%d] = %s", i, vals[i]);
		strcat(data, vals[i]);
		strcat(data, "#");
		count += strlen(vals[i]) + 1;
	}
	data[count] = '\0';
}

void listFile(Host* head, char buffer[], int n, char clientIp[], char msg[])
{
	int i;
	char fileName[20];
	for(i=1;i<n;i++)
	{
		fileName[i-1] = buffer[i];
	}
	printf("\nFilename is %s", fileName);
	Host* host = get_host_from_ip(head, clientIp);
	if(host)
	{
		strcpy(host->files[host->numFiles], fileName);
		host->numFiles++; 
		strcpy(msg, "File added to host");
	}
	else
	{
		char files[20][20]; //Temporary
		strcpy(files[0], fileName);
		head = host_insert(head, clientIp, "temporary mac", files, 0);
		strcpy(msg, "New host created and file added");
	}
	
}

int main()
{
	Host* head = NULL;
	char files1[10][20] = {"abcf.txt", "def.txt"};
	char files2[10][20] = {"abcd.txt", "defg.txt"};
	head = host_insert(head, "10", "100", files1, 2);
	head = host_insert(head, "20", "200", files2, 2);
	head = host_insert(head, "30", "300", files1, 2);
	int ls, s;
	char buffer[256];
	char *ptr = buffer;
	int len = 0;
	int maxlen = sizeof(buffer);
	int n = 0;
	int waitSize = 16;
	char *pwd = "";
	struct sockaddr_in servAddr, clntAddr;
	int clntAddrLen;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = 6901;
	
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
		buffer[n] = '\0';
		printf("\nReceived from client : %s\n", buffer);	
		printf("\nValue of n1 is %d", n);
		fflush(stdout);	
		char clientAddr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(clntAddr.sin_addr), clientAddr, INET_ADDRSTRLEN);
		printf("\nClient's address is %s", clientAddr);
		int auth;
		char sendData[100], msg[20];
		switch(buffer[0])
		{
			case '1' : auth = loginUser(buffer, n, pwd);
					   char res[10];
					   if(auth)
					   {
					   		strcpy(res, "Success");
					   } 
					   else
					   {
					   		strcpy(res, "Failure");
					   }
					   send(s, res, strlen(res), 0);
					   close(s);
					   break;
			case '2' : downloadFile(head, buffer, n, sendData);
					   printf("\nData to client is %s len = %d\n", sendData, (int) strlen(sendData));
				       fflush(stdout);
					   send(s, sendData, strlen(sendData), 0);
					   close(s);
					   break;
			case '3' : listFile(head, buffer, n, clientAddr, msg);
					   send(s, msg, strlen(msg), 0);
					   close(s);
					   break;
		}
	}
}