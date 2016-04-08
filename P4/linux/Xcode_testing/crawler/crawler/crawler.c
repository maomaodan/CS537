#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <pthread.h>
/*RP 4.6*/
#include "cs537.h"//for maxbuf
const char spc[2] = " ";
/* */

struct entry_s {
    char *key;
    char *value;
    struct entry_s *next;
};

typedef struct entry_s entry_t;

struct hashtable_s {
    int size;
    struct entry_s **table;
};
typedef struct hashtable_s hashtable_t;

struct node{
    char* page;
    struct node *next;
};

hashtable_t *hashtable;

struct node downQueue;
int d_fillptr = 0;
int d_use_ptr = 0;
int d_count = 0;

int q_size;
char** parseQueue;
int p_fillptr = 0;
int p_use_ptr = 0;
int p_count = 0;

pthread_cond_t p_empty, p_fill;
pthread_mutex_t p_lock;

pthread_cond_t d_fill;
pthread_mutex_t d_lock;

void (*edge_fn)(char *from, char *to);
char * (*fetch_fn)(char *url);

void putParse(char* value)
{
    *(parseQueue+p_fillptr) = value;
    p_fillptr = (p_fillptr+2)%q_size;
    p_count ++;
    
}

char* getParse()
{
    char* tmp = *(parseQueue + p_use_ptr);
    p_use_ptr = (p_use_ptr+1)%q_size;
    p_count --;
    return tmp;
}

void putDown(char* value)
{
    
    struct node *new = malloc(sizeof(struct node));
    new->page = value;
    new->next = NULL;
    struct node *curr = &downQueue;
    
    if(curr->page == NULL)
    {
        downQueue.page = value;
        //printf("page value %s\n", downQueue.page);
        downQueue.next = NULL;
    }
    else
    {
        while(curr->next!=NULL)
        {
            curr = curr->next;
        }
        
        curr->next = new;
        
    }
    d_count++;
    //printf("down: in putdown \n");
    

}

char* getDown()
{
    char* content = downQueue.page;
    //printf("down[0] %s\n",downQueue.page);
    
    //struct node *nxt = downQueue.next;
    
    //free(&downQueue);
    if(downQueue.next==NULL)
    {
        downQueue.page = NULL;
        downQueue.next = NULL;
    }
    else{
        downQueue.page = downQueue.next->page;
        downQueue.next = downQueue.next->next;
    }
    
    d_count--;
    return content;
    
}

void *parser() //edge method pointer
{
    //int k = 0;
    //while (d_count > 0)
    //for (k = 0; k<10000;k++)
    while(1)
    {
        //printf("in parser\n");
        
        //read from downloader queue
        pthread_mutex_lock(&d_lock);
        while (d_count==0)
            pthread_cond_wait(&d_fill, &d_lock);
        
        char* buf = getDown();
        
        pthread_mutex_unlock(&d_lock);
        
        /* RP 4.6 */
        //printf("buf: %c\n",buf[0]);
        char * from = malloc(MAXBUF);
        from = strtok(buf, spc);
        /* */
        
        char curr = 0;
        char* returnLink = malloc(sizeof(1024));
        int i,j;
        
        for (i = 0; i< sizeof(buf); i++)
        {
            printf("end parser %lu, %s\n",sizeof(buf),buf);
            curr = buf[i];
            
            if (curr == 'l' && buf[i+1] == 'i' && buf[i+2] == 'n' &&
                buf[i+3] == 'k' && buf[i+4] == ':')
            {
                i = i+5;
                int start = i;
                while (buf[i] != '\n')
                    i ++;
                //now buf[i] is \n
                
                for (j = start;j < i; j++)
                {
                    returnLink[start-j] = buf[j];
                }
                
            }
            
            edge_fn(from, returnLink);// /
            // deal w/ the link here
            //put into parser to downloader queue
            
            pthread_mutex_lock(&p_lock);
            while (p_count==q_size)
                pthread_cond_wait(&p_empty,&p_lock);
            putParse(returnLink);
            pthread_cond_signal(&p_fill);
            pthread_mutex_unlock(&p_lock);
        }
        
        
        
        
    }
    return NULL;
}

void *downloader()
{
    while(1)
        //int k;
        //for (k = 0; k < 10000; k++)
    {
        //printf("in download\n");
        pthread_mutex_lock(&p_lock);
        
        while (p_count == 0){
            //printf("before wait\n");
            pthread_cond_wait(&p_fill,&p_lock);
            
        }
        char* tmp = getParse();
        pthread_cond_signal(&p_empty);
        pthread_mutex_unlock(&p_lock);
        /*RP 4.6*/
        char page[MAXBUF];
        strcpy(page, tmp);
        strcat(page, " ");
        strcat(page,fetch_fn(tmp));
        
        /* */
        
        //get downloader lock to write
        pthread_mutex_lock(&d_lock);
        putDown(page);
        pthread_cond_signal(&d_fill);
        pthread_mutex_unlock(&d_lock);
        //printf("end download\n");
        
    }
    return NULL;
}
//hash
uint32_t fletcher32( uint16_t const *data, size_t words )
{
    uint32_t sum1 = 0xffff, sum2 = 0xffff;
    size_t tlen;
    
    while (words) {
        tlen = words >= 359 ? 359 : words;
        words -= tlen;
        do {
            sum2 += sum1 += *data++;
        } while (--tlen);
        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }
    /* Second reduction step to reduce sums to 16 bits */
    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    return sum2 << 16 | sum1;
}

int crawl(char *start_url,
          int download_workers,
          int parse_workers,
          int queue_size,
          char * (*_fetch_fn)(char *url),
          void (*_edge_fn)(char *from, char *to)) {
    int i;
    pthread_t parserThreads[parse_workers];
    pthread_t downloadThreads [download_workers];
    
    fetch_fn = _fetch_fn;
    edge_fn = _edge_fn;
    
    /////////////////
    char*page = _fetch_fn(start_url);
    assert(page !=NULL);
    printf("PAGE: %s\n", page);
    q_size = queue_size;
    char* parseQ[q_size];
    parseQueue = parseQ;
    
    //create(hashtable);
    for(i = 0; i<queue_size;i++)
    {
        parseQueue[i] = malloc(sizeof(char*));
    }
    parseQueue[0] = start_url;
    p_count = 1;
    //create threads
    for (i = 0 ; i< parse_workers;i++)
    {
        //parserThreads[i] = malloc(sizeof(pthread_t));
        pthread_create(&parserThreads[i],NULL,&parser,NULL);
        
        // /
    }
    for (i = 0 ; i< download_workers;i++)
    {
        //downloadThreads[i] = malloc(sizeof(pthread_t));
        pthread_create(&downloadThreads[i],NULL,&downloader,NULL);// /
    }
    
    for (i = 0 ; i< parse_workers;i++)
    {
        //parserThreads[i] = malloc(sizeof(pthread_t));
        pthread_join(parserThreads[i],NULL);
        
        // /
    }
    for (i = 0 ; i< download_workers;i++)
    {
        //downloadThreads[i] = malloc(sizeof(pthread_t));
        pthread_join(downloadThreads[i],NULL);// /
    }
    
    
    return 0;
}

/*
 void create(*hashtable_t hashtable){
	// Allocate the table itself.
	if( ( hashtable = malloc( sizeof( hashtable_t ) ) ) == NULL ) {
 return NULL;
	}
 
	// Allocate pointers to the head nodes.
	if( ( hashtable->table = malloc( sizeof( entry_t * ) * size ) ) == NULL ) {
 return NULL;
	}
	for( i = 0; i < size; i++ ) {
 hashtable->table[i] = NULL;
	}
	hashtable->size = 99923; //initial size
 }
 */
