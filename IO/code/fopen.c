#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
int main()
{

	FILE *fp;
	fp = fopen("./stdio/code/test1.txt","r");
	
	if(fp == NULL)
	{
		perror("open() failed");
		//fprintf(stderr,"fopen():%s\n",strerror(errno));
		exit(1);
	}
	puts("OK!");
	fclose(fp);
	return 0;
}
