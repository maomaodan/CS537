#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
int main(int argc, char* argv[])
{
FILE *fd=fopen ("a.txt", "a");
fstat(fd);
fclose(fd);
return 0;
}


