#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if(argc<3)
	{
		fprintf(stderr,"Usage:%s<src_file> <tar_file>\n",argv[0]);
		exit(1);
	}

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
	while(1)
	{
		ch=fgetc(fs);
		if(ch==EOF)
		{
			break;
		}
		fputc(ch,ft);
		//printf("pus\n");
	}
	
	fclose(ft);
	fclose(fs);
	return 0;
}










