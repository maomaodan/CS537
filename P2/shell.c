//
//  shell.c
//
//  Created by Wenxuan Mao on 16/2/5.
//  All rights reserved.
//
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>

int main(int argc, char * argv[]) {
    //error message
    char error_message[30] = "An error has occurred\n";
    //write(STDERR_FILENO, error_message, strlen(error_message));
    
    
    while (1)
    {
        int i,j;
        
        printf("whoosh> ");
        //my input line arg
        char* input = malloc(sizeof(char)*128);
        fgets(input,128,stdin);
        char* argdup = malloc(sizeof(char)*128);
        strcpy(argdup, input);
        
        if (strlen(input)>=127&&input[127]!='\n')
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        
        
        char* myargv[128];
        
        char* pathvar[128];
        
        char *token;
        
        token = strtok(argdup," ");
        //divide up lines of inputs
        i = 0;
        j = 0;
        while (token !=NULL)
        {
            
            myargv[i] = malloc(sizeof(char)*128);
            strcpy(myargv[i], token);
            token  = strtok(NULL," ");
            i++;
        }
        
        if (strcmp(myargv[0],"exit")==0||strcmp(myargv[0],"exit\n")==0)
        {
            exit(0);
        }
        else if (strcmp(myargv[0],"pwd")==0||strcmp(myargv[0],"pwd\n")==0)
        {
            char* ptr;
            
            ptr = getcwd(NULL,80);
            printf("%s\n",ptr);
            free(ptr);
            continue;
        }
        else if(strcmp(myargv[0],"cd")==0)
        {
            int ret;
            if (myargv[1][strlen(myargv[1])-1]== '\n')
            {
                myargv[1][strlen(myargv[1])-1]=0;
            }
            if (strcmp(myargv[1],"")==0)
            {
                
                ret = chdir(getenv("HOME"));
            }
            else{
                
                
                ret = chdir(myargv[1]);
            }
            if(ret==-1)
            {
                printf("err");
            }
            /*
             char* ptr;
             
             ptr = getcwd(NULL,80);
             printf("%s\n",ptr);
             */
            continue;
        }
        else if (strcmp(myargv[0],"cd\n")==0)
        {
            int ret;
            ret = chdir(getenv("HOME"));
            
            if(ret==-1)
            {
                printf("err");
            }
            continue;
            
        }
        else if (strcmp(myargv[0], "path")==0)
        {
            j = 0;
            for (i = 1;i<128;i++)
            {
                if (myargv[i]==0||myargv[i]==NULL)
                {
                    pathvar[i] = '\0';
                    break;
                }
                else{
                    if (myargv[i][strlen(myargv[i])-1]== '\n')
                    {
                        myargv[i][strlen(myargv[i])-1]=0;
                    }
                    pathvar[j] = malloc (sizeof(char)*128);
                    strcpy(pathvar[j],myargv[i]);
                    //TODO: printf("%s\n",pathvar[j]);
                    j++;
                }
            }
            continue;
        }
        else
        {
            for (i = 0; i < 128; i++)
            {
                if (pathvar[i]!=NULL)
                {
                    char* programArg[128];
                    
                    programArg[0] = malloc(sizeof(char)*(strlen(pathvar[i])+strlen(myargv[0]))+1);
                    
                    strcpy(programArg[0],pathvar[i]);
                    strcat(programArg[0],myargv[0]);
                    int rc = fork();
                    if (rc == 0)
                    {
                        //child
                        
                        
                        i = 1;
                        while (myargv[i]!=NULL)
                        {
                        myargv[i] = strdup(myargv[i]);
                        
                            i++;
                        
                        }
                        programArg[i+1] = NULL;
                        //before exec, change STDOUT
                        close(STDOUT_FILENO);
                        open("/Users/morgan/Desktop/file.stdout", O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR);
                        //open("/tmp/file.stdout",, O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR);
                        
                        //new process
                        
                        
                        execv(myargv[0],myargv);
                        printf("(only runs execv) failed!\n");
                        exit(1);
                        
                    }
                    else if (rc >0)
                    {   //parent
                        int cpid = (int) wait (NULL);
                        printf("parent: child pid was %d\n",cpid);
                    }
                    else{
                        fprintf(stderr, "fail\n");
                    }

                }
            }
            /*
            int rc = fork();
            printf("pid%d\n",rc);
            //new process created, new virtual address space
            //process is a copy of parent, different return code
            if (rc == 0)
            {
                //child
                char *myargv[4];
                myargv[0] = strdup("/bin/ls");
                myargv[1] = strdup("-l");
                
                myargv[2] = strdup("-a");
                myargv[3] = NULL;   //important
                
                
                
                //before exec, change STDOUT
                close(STDOUT_FILENO);
                open("/Users/morgan/Desktop/file.stdout", O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR);
                //open("/tmp/file.stdout",, O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR);
                
                //new process
                
                
                execv(myargv[0],myargv);
                printf("(only runs execv) failed!\n");
                exit(1);
                
            }
            else if (rc >0)
            {   //parent
                int cpid = (int) wait (NULL);
                printf("parent: child pid was %d\n",cpid);
            }
            else{
                fprintf(stderr, "fail\n");
            }
         */
        }
    }
    
    return 0;
}
