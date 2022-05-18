#include <stdio.h>
#include <stdlib.h>
#define BUFSIZE 1024
int main(int argc, char *argv[])
{
	if(argc<3)
	{
		fprintf(stderr,"Usage:%s<src_file> <tar_file>\n",argv[0]);
		exit(1);
	}

	char buf[BUFSIZE];
	FILE *fs=NULL;
	FILE *ft=NULL;
	int ch;
	fs=fopen(argv[1],"r");
	if(fs==NULL)
	{
		perror("fs.fopen():");
		exit(1);
	}
	ft=fopen(argv[2],"w");
	if(ft==NULL)
	{
		perror("ft.fopen():");
		fclose(fs);
		exit(1);
	}
	while(fgets(buf,BUFSIZE,fs))
	{
		fputs(buf,ft);
		//printf("pus\n");
	}
	
	fclose(ft);
	fclose(fs);
	return 0;
}










