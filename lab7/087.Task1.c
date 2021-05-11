#include<stdio.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct __myarg_t {
    int a;
    char *b;
} myarg_t;

void * mythread(void * arg) {
    myarg_t * m = (myarg_t * ) arg;
    printf("%s\n", m->b);
    return NULL;
}

int main(int argc, char * argv[]) {
    pthread_t p;
    myarg_t *rvals;
    myarg_t args;
    args.a = 10;
    args.b = "hello world";

    pthread_create(&p, NULL, mythread, &args);
    pthread_join(p, (void**) &rvals);
    //free(rvals);
    return 1;
}