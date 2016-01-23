
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>


//compare method
int my_compare (const void *k1, const void *k2)
{
    char * w1 = (char*) k1;
    char * w2 = (char*) k1;
    return strcmp(w1,w2);
}


int main(int argc, char*argv[])
{
    //args: fastsort file
    char* input_file;
    int col;
    int maxLineLength = 128;
    int i;
    
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
    //get the file size to malloc memory
    struct stat status;
    if (stat(input_file,&status)<0)
    {
        exit(1);
    }
    //printf("%lld\n",status.st_size);
    
    //buffer
    char *buffer[1024];
    //= (char*) malloc(1024*128*sizeof(char));
    char *cmp_buffer[1024][128];//= (char**) malloc(status.st_size);

    
    //read from the file
    int lineCount = 0;
    char* currLine = malloc(sizeof(char)*128);
    
    while (fgets(currLine,maxLineLength,fp) != NULL)
    {
        buffer[lineCount] = malloc(sizeof(char)*128);
        strcpy (buffer[lineCount], currLine);
        lineCount++;
    }
    
    //printf("%d\n",lineCount);
    for (i = 0; i < lineCount; i++)
    {
        cmp_buffer[i] = malloc(sizeof(char)*128);
        strcpy(cmp_buffer[i], strtok(buffer[i]," "));

    }
    
    
    
    // qsort ()
     //void qsort(void *base, size_t nmemb, size_t size,
     //                  int(*compar)(const void *, const void *));
    
    return 0;
}