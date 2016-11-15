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
#include <sys/mman.h>
#include "encrypt.h"

typedef struct Host1
{
	char ipAddress[20], macAddress[30];
	char files[20][20];
	int numFiles;
	struct Host1* next;
} Host;


Host *head[20];
int *size;

void host_insert(char ipAddress[], char macAddress[], char files[][20], int n)
{
	Host new;
	strcpy(new.ipAddress, ipAddress);
	strcpy(new.macAddress, macAddress);
	new.numFiles = n;
	int i;
	for(i=0;i<n;i++)
		strcpy(new.files[i], files[0]);
	*head[*size] = new;
	(*size)++;
}

int get_host_from_ip(char ip[])
{
	int i;
	for(i=0;i<*size;i++)
	{
		fflush(stdout);
		if(strcmp((*head[i]).ipAddress, ip) == 0)
		{
			return i;
		}
	}
	return -1;
}

void host_search(char fileName[], char vals[][20])
{
	int mat = 0;
	int i,j = 1;
	int s;
	for(s=0;s<*size;s++)
	{
		for(i=0;i<(*head[s]).numFiles;i++) 
		{
			if(strcmp(fileName, (*head[s]).files[i]) == 0)
			{
				mat++;
				strcpy(vals[j], (*head[s]).ipAddress);
				j++;
			}
		}
	}
	snprintf (vals[0], sizeof(vals[0]), "%d", mat);
}

void printList()
{
	int i;
	for(i=0;i<*size;i++)
	{
		printf("\nHost %d : IP = %s ", i+1, (*head[i]).ipAddress);
		printf("\nFiles : ");
		int j;
		for(j=0;j<(*head[i]).numFiles;j++)
			printf("%s ", (*head[i]).files[j]);
		fflush(stdout);
	}
	fflush(stdout);
}

int loginUser(char buffer[], int n, char pwd[])
{
	int i;
	buffer[n] = '\0';
	char pass_enc[20];
	for(i=1;i<n;i++)
		pass_enc[i-1] = buffer[i];
	char pass[20];
	decrypt(pass_enc, pass, n-1);
	fflush(stdout);
	return !strcmp(pass, pwd);
}

void downloadFile(char buffer[], int n, char data[])
{
	int i;
	char fileName[20];
	for(i=1;i<n;i++)
		fileName[i-1] = buffer[i];
	char vals[10][20];	
	fileName[n-1] = '\0';
	host_search(fileName, vals);
	int k = vals[0][0] - '0';
	data[0] = '\0';
	int count = 0;
	for(i=0;i<=k;i++)
	{
		strcat(data, vals[i]);
		strcat(data, "#");
		count += strlen(vals[i]) + 1;
	}
	data[count] = '\0';
}
void listFile(char buffer[], int n, char clientIp[], char msg[])
{
	int i;
	char fileName[20];
	for(i=1;i<n;i++)
	{
		fileName[i-1] = buffer[i];
	}
	int t = get_host_from_ip(clientIp);
	if(t != -1)
	{
		strcpy((*head[t]).files[(*head[t]).numFiles], fileName);
		(*head[t]).numFiles++; 
		strcpy(msg, "File added to host");
	}
	else
	{
		char files[20][20]; //Temporary
		strcpy(files[0], fileName);
		host_insert(clientIp, "temporary mac", files, 1);
		strcpy(msg, "New host created and file added");
	}
	
}

int main()
{	
	size = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int i;
    for(i=0;i<20;i++)
    	head[i] = mmap(NULL, sizeof(Host), PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	int ls, s;
	char buffer[256];
	char *ptr = buffer;
	int len = 0;
	int maxlen = sizeof(buffer);
	int n = 0;
	int waitSize = 16;
	char *pwd = "jockey";
	struct sockaddr_in servAddr, clntAddr;
	int clntAddrLen;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("192.168.43.47");
	servAddr.sin_port = 8000;
	
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
	
	while(1){
		
		
			if((s = accept(ls, (struct sockaddr*)&clntAddr, &clntAddrLen)) < 0)
			{
				perror("Error : Accepting failed");
				exit(1);
			}
			if(fork() == 0)	
			{
				n =	recv(s, ptr, maxlen, 0);
				buffer[n] = '\0';
				printf("\nReceived from client : %s", buffer);
				fflush(stdout);	
				char clientAddr[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &(clntAddr.sin_addr), clientAddr, INET_ADDRSTRLEN);
				printf("\nClient's address is %s", clientAddr);
				fflush(stdout);
				int auth;
				char sendData[100], msg[20];
				switch(buffer[0])
				{
					case '1' : auth = loginUser(buffer, n, pwd);
							   char res[10];
							   if(auth)
							   {
							   		strcpy(res, "1");
							   } 
							   else
							   {
							   		strcpy(res, "0");
							   }
							   int si = send(s, res, strlen(res), 0);
							   printf("\nData send to %s is %s", clientAddr, res);
							   printf("\nSize of data is %d", si);
							   fflush(stdout);
							   close(s);
							   break;
					case '2' : downloadFile(buffer, n, sendData);
								printf("\nData send to %s is %s", clientAddr, sendData);
								fflush(stdout);
							   send(s, sendData, strlen(sendData), 0);
							   close(s);
							   break;
					case '3' : listFile(buffer, n, clientAddr, msg);
							   printList(head, size);
							   printf("\nData send to %s is %s", clientAddr, res);
							   fflush(stdout);
							   send(s, msg, strlen(msg), 0);
							   close(s);
							   break;

				}
				printf("\n");
				fflush(stdout);
				//exit(0);
			}
		}
		}

