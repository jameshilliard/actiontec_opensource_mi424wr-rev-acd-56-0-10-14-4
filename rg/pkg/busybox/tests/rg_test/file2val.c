#include <stdio.h>

#define LINE_LEN 80
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    FILE *fp;
    char msg[7];
    char buf[BUF_SIZE];
    int ret, i, count = 0;
    int lc = 0;

    if (argc!=2)
    {
	fprintf(stderr, "Usage: %s input_file\n", argv[0]);
	exit(1);
    }

    if(!(fp = fopen(argv[1], "r")))
    {
	perror(argv[1]);
	exit(1);
    }
    
    while((ret = fread(buf, 1, BUF_SIZE, fp))>0)
	for(i=0; i<ret; i++)
	{
	    sprintf(msg, "0x%x, ", buf[i]);
	    count += strlen(msg);
	    if (count>LINE_LEN)
	    {
		printf("\n%s", msg);
		count = strlen(msg);
	    }
	    else
		printf("%s", msg);
	}
    printf("\n");
    
    fclose(fp);
}
