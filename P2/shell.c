//
//  main.c
//  shell
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
    
    printf ("hello\n");
    
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
    
        
    
    
    return 0;
}
