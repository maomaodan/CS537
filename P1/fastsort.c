
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char*argv[])
{
	//args: fastsort file
	char* input_file;
	int col;

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
	
	FILE *fp = fopen (input_file,'a');
	//verify file opened correctly
	if (fp == NULL)
	{
		fprintf(stderr, "Error L Cannot open file %s\n", filename);
	}
	printf("%s\n",stat())
	//read from the file





	//compare
	int my_compare (const void *k1, const void *k2)
	{
		strcmp(*k1,*k2);
	}
	qsort ()
	//void qsort(void *base, size_t nmemb, size_t size,
//                  int(*compar)(const void *, const void *));

	return 0;
}