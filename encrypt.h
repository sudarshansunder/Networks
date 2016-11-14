void encrypt(char *source ,char *dest, int n)
{
	int i;
	for(i=0;i<n;i++)
	{
		if(i%3 == 0)
		{
		 	dest[i] = (int) source[i] + 4;
		} 
		else if(i%3 == 1)
		{
			dest[i] = (int) source[i] + 2;
		}
		else
		{
			dest[i] = source[i];
		}
	}
	dest[n] = '\0';
	strcat(dest, "BlazeIt");
}
void decrypt(char *source, char *dest, int n)
{
	int i;
	for(i=0;i<n-7;i++)
	{
		if(i%3 == 0)
		{
			dest[i] = source[i] - 4;
		} 
		else if(i%3 == 1)
		{
			dest[i] = source[i] - 2;
		}
		else
		{
			dest[i] = source[i];
		}
	}
	dest[n-7] = '\0';
}