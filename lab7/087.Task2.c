#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include<string.h>
 
typedef struct __myarg_t 
{
    int a;
    char *b;
} myarg_t;

typedef struct __myret_t 
{
    int x;
    char *y;
} myret_t;

typedef struct __myarg_t2 
{
    myret_t* t1;
    myret_t* t2;
} myarg_t2;

void * mythread(void * arg){
    myarg_t * m = (myarg_t * ) arg;
    int print_index=0;
    for(print_index=0; print_index<m->a; print_index++){
        printf("Printing %d th character %c\n", print_index, *(m->b+print_index));
    }
    myret_t * r = malloc(sizeof(myret_t));
    r->x = 1;
    r->y = m->b;
    return (void * ) r;
}

void * mythreadCustom(void * argTem){
    myarg_t2 * arg = (myarg_t2 * ) argTem;
    int flen = strlen(arg->t1->y);
    char *newStr = (char *)malloc(flen+1);
    strcpy(newStr,  arg->t1->y);
    newStr[flen] = ' ';
    strcat(newStr, arg->t2->y);

    myarg_t* m= malloc(sizeof(myarg_t));
    m->a = strlen(newStr);
    m->b = newStr;
    int print_index=0;
    for(print_index=0; print_index<m->a; print_index++){
        printf("Printing %d th character %c\n", print_index, *(m->b+print_index));
    }
    myret_t * r = malloc(sizeof(myret_t));
    r->x = 1;
    r->y = m->b;
    return (void * ) r;
}

int  main(int argc, char * argv[]){
    int NUM_THREADS = 2;
    pthread_t p[NUM_THREADS];
    myret_t * m[NUM_THREADS];
    myarg_t args[NUM_THREADS];
    for(int i=0; i<NUM_THREADS; i++){
        args[i].a= strlen(argv[i+1]);
        args[i].b=(char *)malloc(strlen(argv[i+1]));
        strcpy(args[i].b,  argv[i+1]);
        pthread_create(&p[i], NULL, mythread, &args[i]);
    }
    for(int i=0; i<NUM_THREADS; i++){
        pthread_join(p[i], (void ** ) &m[i]);
        printf("returned %d %s\n", m[i]->x, m[i]->y);
    }
    myarg_t2 newArgs;
    newArgs.t1 = m[0];
    newArgs.t2 = m[1];
    pthread_create(&p[0], NULL, mythreadCustom, &newArgs);
    pthread_join(p[0], (void ** ) &m[0]);
    printf("returned %d %s\n", m[0]->x, m[0]->y);
    return 0;
}