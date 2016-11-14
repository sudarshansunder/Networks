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

/*
Flags for login, file download, list file

	1 - Login
	2 - Download
	3 - List

*/

int loginUser(char buffer[], int n, char pwd[])
{
	int i;
	buffer[n] = '\0';
	return !strcmp(buffer, pwd);
}

int main()
{
	int ls, s;
	char buffer[256];
	char *ptr = buffer;
	int len = 0;
	int maxlen = sizeof(buffer);
	int n = 0;
	int waitSize = 16;
	const char *pwd = "1#trumpkilledharambe";
	struct sockaddr_in servAddr, clntAddr;
	int clntAddrLen;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("192.168.43.163");
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

		switch(buffer[0])
		{
			case '1' : int auth = loginUser(buffer, n, pwd);
					   char *res;
					   if(auth)
					   {
					   		res = "Success";
					   } 
					   else
					   {
					   		res = "Failure";
					   }
					   send(s, res, strlen(res), 0);
					   close(s);
					   break;
			case '2' : downloadFile(buffer, n);
					   close(s);
					   break;
			case '3' : listFile(buffer, n);
					   close(s);
					   break;
		}
	}
}