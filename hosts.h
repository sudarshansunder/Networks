#include <stdlib.h>

typedef struct Host1
{
	char ipAddress[20], macAddress[30];
	char files[20][20];
	int numFiles;
	struct Host1* next;
} Host;

Host* createHost(char ipAddress[], char macAddress[], char files[][20], int n) 
{
	Host *h = malloc(sizeof(Host));
	h->numFiles = n;
	int i;
	for(i=0;i<n;i++)
		strcpy(h->files[i], files[i]);
	strcpy(h->ipAddress, ipAddress);
	strcpy(h->macAddress, macAddress);
	return h;
}

Host* host_insert(Host* head, char ipAddress[], char macAddress[], char files[][20], int n)
{
	if(!head)
		return createHost(ipAddress, macAddress, files, n);
	Host* temp = head;
	while(temp->next)
		temp = temp->next;
	temp->next = createHost(ipAddress, macAddress, files, n);
	return head;
}

Host* get_host_from_ip(Host *head, char ip[])
{
	while(head)
	{
		if(strcmp(head->ipAddress, ip) == 0)
			return head;
		head = head->next;
	}
	return NULL;
}

void host_search(Host *head, char fileName[], char vals[][20])
{
	int mat = 0;
	int i,j = 1;
	while(head)
	{
		for(i=0;i<head->numFiles;i++) 
		{
			if(strcmp(fileName, head->files[i]) == 0)
			{
				mat++;
				strcpy(vals[j], head->ipAddress);
				j++;
			}
		}
		head = head->next;
	}
	snprintf (vals[0], sizeof(vals[0]), "%d", mat);
}

void printList(Host* head)
{
	printf("\n\nLinked list at main is : \n\n");
	int i = 1;
	while(head)
	{
		printf("\nHost %d : IP = %s ", i, head->ipAddress);
		printf("\nFiles : ");
		fflush(stdout);
		int j;
		for(j=0;j<head->numFiles;j++)
			printf("%s ", head->files[j]);
		head = head->next;
		i++;
	}
	fflush(stdout);
}
