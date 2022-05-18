#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
int main()
{

	FILE *fp;
	int num = 0;
	while(1)
	{

		fp = fopen("test1.txt","r");
		if(fp == NULL)
		{
			printf("num:%d\n",num);
			//perror("open() failed");
			//fprintf(stderr,"fopen():%s\n",strerror(errno));
			exit(1);
		}
		num++;
	}
	//puts("OK!");
	//fclose(fp);
	return 0;
}
