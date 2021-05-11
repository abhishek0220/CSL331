#include<stdio.h>
#include <pthread.h>
#include <stdlib.h>
#define MAX 100

int arrIndex;
int arr[MAX];
pthread_mutex_t lock =  PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

void print(int arr[], int n){
    printf("Printing the array\n");
    for(int i=0; i<n; i++) printf("%d\t",arr[i]);
    printf("\n");
}
void * mythread(void * arg){
    int ind;
    pthread_mutex_lock(&lock);
    ind = arrIndex;
    arrIndex++;
	pthread_mutex_unlock(&lock);
	arr[ind] = rand();
	if(ind == MAX-1){
		pthread_cond_signal(&cond1);
	}
	else{
		pthread_cond_wait(&cond1, &lock); 
	}
    return NULL;
}

int main(){
    arrIndex = 0;
    pthread_t p[MAX];
    for(int i=0; i<MAX; i++) pthread_create(&p[i], NULL, mythread, NULL);
    pthread_join(p[MAX-1], NULL);
    print(arr,MAX);
    return 0;
}