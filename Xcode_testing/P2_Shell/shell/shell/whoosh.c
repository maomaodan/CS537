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
    //initial path variables
    int pathNum = 1;
    char* pathvar[128];
    pathvar[0] = malloc(128);
    
    pathvar[0] = "/bin";
    
    if (argc>1)
    {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
      while (1)
    {
        int redir = 0;
        int i,j;
        int myargc = 0;
        int outfile,errfile;
        char* outfilename = "";
        char* errfilename = "";
        
        int validarg = 1;
        
        //printf("whoosh> ");
        write (STDOUT_FILENO, "whoosh> ", strlen("whoosh> "));
        //my input line arg
        
        char* input = malloc(sizeof(char)*1024);
        fgets(input,1024,stdin);
        char* argdup = malloc(sizeof(char)*1024);
        strcpy(argdup, input);
        
        //printf("%d %c\n",strlen(input),input[127]);
        /*
        if (strlen(input)>=128&&input[127]!='\n')
        {
            //printf("%lu %c\n",strlen(myargv[i]),myargv[i][127]);
            write(STDERR_FILENO, error_message, strlen(error_message));
            validarg = 0;
            break;
        }
        */
        
        char* myargv[128];
        
        
        char *token;
        
        token = strtok(argdup," ");
        //myargc++;
        //divide up lines of inputs
        i = 0;
        j = 0;
        while (token !=NULL)
        {
            
            myargv[i] = malloc(sizeof(char)*1024);
            strcpy(myargv[i], token);
            token  = strtok(NULL," ");
            i++;
            myargc++;
        }
        
        for (i = 0; i < myargc ;i++)
        {
            if (strlen(myargv[i])>=128&&myargv[i][127]!='\n')
            {
                //printf("%lu %c\n",strlen(myargv[i]),myargv[i][127]);
                write(STDERR_FILENO, error_message, strlen(error_message));
                validarg = 0;
                break;
            }
        }
        
        
        if (strcmp(myargv[0],"exit")==0||strcmp(myargv[0],"exit\n")==0)
        {
            exit(0);
        }
        else if (strcmp(myargv[0],"pwd")==0||strcmp(myargv[0],"pwd\n")==0)
        {
            char* ptr;
            
            ptr = getcwd(NULL,80);
            
            write(STDOUT_FILENO,ptr,strlen(ptr));
            write(STDOUT_FILENO,"\n",1);
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
                write(STDERR_FILENO, error_message, strlen(error_message));
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
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
            continue;
            
        }
        else if (strcmp(myargv[0], "path")==0)
        {
            pathNum = 0;
            j = 0;
            i = 1;
            for (i = 1; i <= argc; i++)
            {
                if(myargv[i]!=NULL&&myargv[i]!=0)
                {
                    
                    if (strlen(myargv[i])>0)
                    {
                        if (myargv[i][strlen(myargv[i])-1]== '\n')
                        {
                            myargv[i][strlen(myargv[i])-1]=0;
                        }
                    }
                    pathvar[j] = malloc (sizeof(char)*128);
                    strcpy(pathvar[j],myargv[i]);
                    
                    //printf("%s should not be here\n",pathvar[j]);
                    j++;
                    i++;
                    pathNum++;
                    
                }
                else{
                    break;
                }
            }
            continue;
        }
        else if (strcmp(myargv[0], "path\n")==0)
        {
            pathvar[0] = "";
        }
        else    //other commands
        {
            //if empty line
            if(myargc == 1&&strcmp(myargv[0],"\n")==0)
            {
                continue;
            }
            //get program arguments, detect '>'
            char* programArg[64];
            i = 1;
            for(i = 1; i <myargc; i++)
            {
                if (myargv[i][strlen(myargv[i])-1]=='\n')
                {
                    myargv[i][strlen(myargv[i])-1]=0;
                }
                //redirection
                if (strcmp(myargv[i],">")==0)
                {
                    if (myargc == (i+2))
                    {
                        if (myargv[i+1][strlen(myargv[i+1])-1]=='\n')
                        {
                            myargv[i+1][strlen(myargv[i+1])-1]=0;
                        }
                        outfilename = strdup(myargv[i+1]);
                        errfilename = strdup(myargv[i+1]);
                        programArg[i] = NULL;
                        redir = 1;
                        break;
                    }
                    else{
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        validarg = 0;
                        continue;
                    }
                }
                
                programArg[i] = (char*)malloc(128);
                programArg[i] = strdup(myargv[i]);
                
                
            }
            if (validarg)
            {
                programArg[myargc]= NULL;
                for(i = 0; i < pathNum ; i++)
                {
                    
                    
                    programArg[0] = malloc(sizeof(char)*(strlen(pathvar[i])+strlen(myargv[0]))+1);
                    if (myargv[0][strlen(myargv[0])-1]=='\n')
                    {
                        myargv[0][strlen(myargv[0])-1]=0;
                    }
                    
                    strcpy(programArg[0],pathvar[i]);
                    strcat(programArg[0],"/");
                    strcat(programArg[0],myargv[0]);
                    
                    
                    
                    int rc = fork();
                    if (rc == 0)
                    {
                        //child
                        if (redir)
                        {
                            //before exec, change STDOUT
                            close(STDOUT_FILENO);
                            //open("/Users/morgan/Desktop/file.stdout", O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR);
                            outfile = open(strcat(outfilename,".out"),O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR);
                            if (outfile ==-1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                            }
                            close(STDERR_FILENO);
                            
                            errfile = open(strcat(errfilename,".err"),O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR);
                            if (errfile==-1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                            }
                        }
                        
                        
                        
                        
                        
                        //new process
                        
                        int suc;
                        suc = execv(programArg[0],programArg);
                        if (suc>=0)
                        {
                            break;
                        }
                        //printf("(only runs execv) failed!\n");
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        exit(1);
                        
                    }
                    else if (rc >0)
                    {   //parent
                        int cpid = (int) wait (NULL);
                        //printf("parent: child pid was %d\n",cpid);
                    }
                    else{
                        fprintf(stderr, "fail\n");
                    }
                    
                    
                }
            }
            
        }
        
        
    }
    
    return 0;
}
