#include <stdlib.h>

typedef struct Host1
{
	char ipAddress[20], macAddress[30];
	char files[20][20];
	int numFiles;
	struct Host1* next;
} Host;

void host_insert(Host head[], int size, char ipAddress[], char macAddress[], char files[][20], int n)
{
	Host new;
	strcpy(new.ipAddress, ipAddress);
	strcpy(new.macAddress, macAddress);
	new.numFiles = n;
	int i;
	for(i=0;i<n;i++)
		strcpy(new.files[i], files[0]);
	head[size++] = new;
}

int get_host_from_ip(Host head[], int size, char ip[])
{
	int i;
	for(i=0;i<size;i++)
	{
		if(strcmp(head[i].ipAddress, ip) == 0)
			return i;
	}
	return -1;
}

void host_search(Host head[], int size, char fileName[], char vals[][20])
{
	int mat = 0;
	int i,j = 1;
	int s;
	for(s=0;s<size;s++)
	{
		for(i=0;i<head[s].numFiles;i++) 
		{
			if(strcmp(fileName, head[s].files[i]) == 0)
			{
				mat++;
				strcpy(vals[j], head[s].ipAddress);
				j++;
			}
		}
	}
	snprintf (vals[0], sizeof(vals[0]), "%d", mat);
}
//I can ping alan, but he cant ping me. g fucking g how tf is that 
void printList(Host head[], int size)
{
	printf("\n\nLinked list at main is : \n\n");
	int i;
	for(i=0;i<size;i++)
	{
		printf("\nHost %d : IP = %s ", i+1, head[i].ipAddress);
		printf("\nFiles : ");
		fflush(stdout);
		int j;
		for(j=0;j<head[i].numFiles;j++)
			printf("%s ", head[i].files[j]);
		i++;
	}
	fflush(stdout);
}
