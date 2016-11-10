#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int main()
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
	servAddr.sin_addr.s_addr = inet_addr("10.87.2.88");
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
		printf("\nString is %s", buffer);
		send(s, buffer, n, 0);
		close(s);
	}
}
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
