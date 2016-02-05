
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>





struct sentence{
    char* words[64];
    char* to_sort;
};
//compare method
int my_compare (const void *k1, const void *k2)
{
    struct sentence * w1 = *(struct sentence**) k1;
    struct sentence * w2 = *(struct sentence**) k2;
    return strcmp(w1->to_sort,w2->to_sort);
}


int main(int argc, char*argv[])
{
    //args: fastsort file
    char* input_file;
    int col;
    int maxLineLength = 128;
    int i,j;
    
    if (argc == 2)
    {
        input_file = argv[1];
        col = 0;
//        printf("%s\n",input_file);
        
    }
    else if (argc == 3)
    {
        if (argv[1][0] =='-')
        {
            
            if ((col = atoi(argv[1]+1))==0)
            {
                fprintf(stderr, "Error: Bad command line parameters\n");
                exit(1);
            }
            col--;
            
            input_file = argv[2];
        }
        else
        {
            fprintf(stderr, "Error: Bad command line parameters\n");
            exit(1);
        }
        
        
    }
    else
    {
        fprintf(stderr, "Error: Bad command line parameters\n");
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
       //buffer
    char *buffer[1024];
    //= (char*) malloc(1024*128*sizeof(char));
    
    //char *cmp_buffer[1024][64];//= (char**) malloc(status.st_size);
    
    
    //read from the file
    int lineCount = 0;
    char* currLine = malloc(sizeof(char)*128);
    
    while (fgets(currLine,maxLineLength,fp) != NULL)
    {
        //printf("%c\n",*(currLine+126));
        if (!(*(currLine+126)==0||*(currLine+126)=='\n'))
        {
            fprintf(stderr, "Line too long\n");
            exit(1);
        }
        buffer[lineCount] = malloc(sizeof(char)*128);
        strcpy (buffer[lineCount], currLine);
        lineCount++;
        currLine = malloc(sizeof(char)*128);
    }
    
    char *cpy_buffer [1024];
    for (i = 0; i<lineCount;i++)
    {
        cpy_buffer[i]= malloc(sizeof(char)*128);
        strcpy(cpy_buffer[i], buffer[i]);
    }
    
    //printf("%d\n",lineCount);
    struct sentence * sent_buf[1024];
    for (i = 0; i < lineCount; i++)
    {
        sent_buf[i] = malloc(sizeof(struct sentence));
        j = 0;
        
        sent_buf[i]->to_sort = malloc(sizeof(char)*64);

        char *token;
        token = strtok(cpy_buffer[i]," ");
        //divide up lines of inputs
        while (token !=NULL)
        {
            if (j >=128)
            {
                fprintf(stderr, "Line too long\n");
                exit(1);
            }
            sent_buf[i]->words[j] = malloc(sizeof(char)*128);
            strcpy(sent_buf[i]->words[j], token);
            
            //cmp_buffer[i][j] = malloc(sizeof(char)*64);
            //cmp_buffer[i][j] = token;
            token  = strtok(NULL," ");
            j++;
        }
        if (sent_buf[i]->words[col]==NULL)
        {
            j =0;
            while(sent_buf[i]->words[j]!=NULL)
            {
                j++;
            }
            if(j>0)
            {
            sent_buf[i]->to_sort = sent_buf[i]->words[j-1];
            }
            /*else
            {
                sent_buf[i]->to_sort = ;
            }*/
        }
        else
        {
            sent_buf[i]->to_sort = sent_buf[i]->words[col];
        }
        
        
    }
    /*
    char*  cmp[1024];
    for (i = 0; i < lineCount; i++)
    {
        cmp[i] = malloc(sizeof(char)*128);
        strcpy(cmp[i],cmp_buffer[i][col]);
    }
    */
    
    qsort(&sent_buf,lineCount,sizeof(struct sentence*),my_compare);
    // qsort ()
     //void qsort(void *base, size_t nmemb, size_t size,
     //                  int(*compar)(const void *, const void *));
    for (i = 0; i < lineCount;i++)
    {
        j = 0;
        while (sent_buf[i]->words[j] != '\0'||sent_buf[i]->words[j] != NULL)
        {
            if (j==0)
            {
                printf("%s",sent_buf[i]->words[j]);
            }else
            {
                printf(" %s",sent_buf[i]->words[j]);
            }
            j++;
        }
    }
    
    exit(0);
}
