#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>
#include "cs537.h"//for maxbuf

typedef struct hashNode_t {
    int done; //1 means parsed, 0 means not yet
    char *link;
    struct hashNode_t *next;
} hashNode;

//node for linked list of pages
struct node{
    char* page;
    char* thisLink; //link of this page
    struct node *next;
};

hashNode *hashtable;

struct node* downQueue;

int d_fillptr = 0;
int d_use_ptr = 0;
int d_count = 0;

char** parseQueue;
int q_size;
int p_fillptr = 0;
int p_use_ptr = 0;
int p_count = 0;

pthread_cond_t p_empty, p_fill;
pthread_mutex_t p_lock;

pthread_cond_t d_fill;
pthread_mutex_t d_lock;

pthread_cond_t h_check;
pthread_mutex_t h_lock; //lock for hash

void (*edge_fn)(char *from, char *to);
char * (*fetch_fn)(char *url);

void putParse(char* value)
{
    
    strcpy(*(parseQueue+p_fillptr),value);
    //*(parseQueue+p_fillptr) = value;
    
    if (q_size == 1){
		p_fillptr = 0;
	}
	else{
		p_fillptr = (p_fillptr+1)%q_size;
    }    
    p_count ++;
    
}

char* getParse()
{
    char* tmp = strdup(*(parseQueue + p_use_ptr));
    if (q_size == 1){
		p_use_ptr = 0;
	}
	else{
		p_use_ptr = (p_use_ptr+1)%q_size;
    }
    p_count --;
    return tmp;
}

int putHash(char* link) { //put into hash/check : not in hash 0, already in hash 1; 
	hashNode* curr = hashtable;
	while (curr->next != NULL) {
		if (strcmp(curr->link, link) == 0){

			return 1;
		}
		curr = curr->next;
	}
	if (strcmp(curr->link, link) == 0){
		return 1;
	}
	curr->next = malloc(sizeof(hashNode));

	curr->next->link = malloc(sizeof(char)*MAXBUF);
	strcpy(curr->next->link,link);
	curr->next->done = 0;
	curr->next->next = NULL;
	return 0;
}

int markHash(char* link){
	hashNode* curr = hashtable;
	while (curr != NULL) {
		if (strcmp(curr->link, link) == 0){
			curr->done = 1;
			return 0;
		}
		curr = curr->next;
	}
	return 1;
}

void putDown(char* value, char * link)
{
    
    
    struct node *curr = downQueue;
    if(curr->page == NULL)
    {
        downQueue->page = malloc(sizeof(char)*MAXBUF);
        strcpy(downQueue->page,value);
        downQueue->thisLink = malloc(sizeof(char)*MAXBUF);
        strcpy(downQueue->thisLink,link);
        downQueue->next = NULL;
    }
    else
    {
        while(curr->next!=NULL)
        {
            curr = curr->next;
        }
        curr->next = malloc(sizeof(struct node));
		curr->next->page=strdup(value);
		curr->next->thisLink=strdup(link);
		curr->next->next = NULL;
    }
    d_count++;

}

struct node * getDown()
{
    
    struct node * outNode = malloc(sizeof(struct node));
    outNode->page= strdup(downQueue->page);
	outNode->thisLink=strdup(downQueue->thisLink);
	outNode->next = NULL;
	
    if(downQueue->next==NULL)
    {
        downQueue->page = NULL;
        downQueue->thisLink = NULL;
        downQueue->next = NULL;
    }
    else{
        downQueue->page = downQueue->next->page;
        downQueue->thisLink = downQueue->next->thisLink;
        downQueue->next = downQueue->next->next;
    }
    
    d_count--;
    return outNode;
    
}

void *parser() //edge method pointer
{
    while(1)
    {   
        //read from downloader queue
        pthread_mutex_lock(&d_lock);
        while (d_count==0)
            pthread_cond_wait(&d_fill, &d_lock);
        
        struct node * outNode = getDown();
        
        pthread_mutex_unlock(&d_lock);
        
        char* buf = outNode->page; 
        
        char curr = buf[0];
        char* returnLink = malloc(sizeof(char)*MAXBUF);
        
        int i = 0;
        int j;
        
        int hasNewLinks = 0;
        
        while ( curr != '\0')
        { 
            curr = buf[i];
            if (curr == 'l' && buf[i+1] == 'i' && buf[i+2] == 'n' &&
                buf[i+3] == 'k' && buf[i+4] == ':'&&(i==0 || buf[i-1] == ' ' || buf[i-1] =='\n'))
            {
				i = i+5;
                int start = i;
                while (buf[i] != '\n' && buf[i] != ' ' && buf[i] != '\0') {
				    i ++;
                }
                //now buf[i] is \n
                for (j = start;j < i; j++)
                {
                    returnLink[j-start] = buf[j];
                }
                
                edge_fn(outNode->thisLink, returnLink);
                int present;
                
                //put in hash
                pthread_mutex_lock(&h_lock);
                present = putHash(returnLink);
                pthread_mutex_unlock(&h_lock);
                
                if (present == 0){
					 hasNewLinks = 1;
					
					// deal w/ the link here
					//put into parser to downloader queue
					pthread_mutex_lock(&p_lock);

					while (p_count==q_size){
						pthread_cond_wait(&p_empty,&p_lock);
					}
					putParse(returnLink);
					pthread_cond_signal(&p_fill);
					pthread_mutex_unlock(&p_lock);
				}
            }
        i++;    
        }//end of page
        
        if (hasNewLinks == 0){
			//signal main thread
			pthread_mutex_lock(&h_lock);
			pthread_cond_signal(&h_check);
			pthread_mutex_unlock(&h_lock);
		}
	
    }//infinite loop
    return NULL;
}

void *downloader()
{
    while(1)
    {
        pthread_mutex_lock(&p_lock);
        while (p_count == 0){
            
            pthread_cond_wait(&p_fill,&p_lock);
            
        }
        char* tmp = getParse();
        pthread_cond_signal(&p_empty);
        pthread_mutex_unlock(&p_lock);
        char* page =fetch_fn(tmp);
        
        
        //get downloader lock to write
        pthread_mutex_lock(&d_lock);
        putDown(page,tmp);
        pthread_cond_signal(&d_fill);
        pthread_mutex_unlock(&d_lock);
         
        //change this link as done in hash
        pthread_mutex_lock(&h_lock);
        markHash(tmp);
        pthread_mutex_unlock(&h_lock);
        
    }
    return NULL;
}

int crawl(char *start_url,
          int download_workers,
          int parse_workers,
          int queue_size,
          char * (*_fetch_fn)(char *url),
          void (*_edge_fn)(char *from, char *to)) {
    int i;
    pthread_t parserThreads[parse_workers];
    
    pthread_t downloadThreads[download_workers];
    
    fetch_fn = _fetch_fn;
    edge_fn = _edge_fn;
    
    //initiate parseQueue
    q_size = queue_size;
    parseQueue = malloc(sizeof(char*)*q_size);
    for(i = 0; i<q_size;i++)
    {
        *(parseQueue+i) = malloc(sizeof(char)*MAXBUF);
    }
    
    //malloc down queue?
    downQueue = malloc(sizeof(struct node));
    downQueue->thisLink = NULL;
    downQueue->page = NULL;
    downQueue->next = NULL;
    
    //put in start url
    putParse(start_url);
    
    //malloc hashtable
    hashtable = malloc(sizeof(hashNode));
    hashtable->link = start_url;
    hashtable->done = 0;
    hashtable->next = NULL;
    

    for (i = 0 ; i< download_workers;i++)
    {
        pthread_create(&downloadThreads[i],NULL,&downloader,NULL);
    }
    
    for (i = 0 ; i< parse_workers;i++)
    {
        pthread_create(&parserThreads[i],NULL,&parser,NULL);
    }
    
    while(1){
		//wait for a page that has no links
		pthread_mutex_lock(&h_lock);
		pthread_cond_wait(&h_check,&h_lock);
		
		//check hash
		hashNode* curr = hashtable;
		int allfinished = 1;  //1 means all links are done
		while (curr != NULL){
			if (curr->done == 0){
				allfinished = 0; //not all links a re done
				break;
			}
			curr = curr->next;
		}	
		pthread_mutex_unlock(&h_lock);
		if (allfinished == 1) {
			for (i = 0 ; i< parse_workers;i++)
			{
				pthread_kill(parserThreads[i],0);
			}
			for (i = 0 ; i< download_workers;i++)
			{
				pthread_kill(downloadThreads[i],0);
			}
			return 0;
		}
    }
    
    return 0;
}

