
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char*argv[])
{
	//args: fastsort file
	char* input_file;
	int col;
	int maxLineLength = 128;

	if (argc == 2)
	{
		input_file = argv[1];
		col = 1;
		printf("%s\n",input_file);

	}
	else if (argc == 3)
	{
		if (argv[1][0] !='-')
		col = atoi(argv[1]);
		input_file = argv[2];
		
	}
	else
	{
		fprintf(stderr, "Error: Bad commad line parameters\n");
		exit(1);
	}
	
	FILE *fp = fopen (input_file,"r");
	//verify file opened correctly
	if (fp == NULL)
	{
		fprintf(stderr, "Error: Cannot open file %s\n", input_file);
		exit(1);
	}


	//buffer
	char *buffer = (char*) malloc(sizeof(char)*maxLineLength);
	int lineCount = 1;
	//read from the file
	char *currLine;
	fgets(currLine,maxLineLength,fp);
	while (currLine != NULL)
	{
		buffer[lineCount] = *currLine;
		fgets(currLine,maxLineLength,fp);
		lineCount++;
	}
	printf("%d",lineCount);

/*

	//compare
	int my_compare (const void *k1, const void *k2)
	{
		strcmp(*k1,*k2);
	}
	qsort ()
	//void qsort(void *base, size_t nmemb, size_t size,
//                  int(*compar)(const void *, const void *));
*/
	return 0;
}