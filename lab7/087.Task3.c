#include<stdio.h>
#include <pthread.h>
#include <stdlib.h>
#define MAX 100

int arrIndex;
int arr[MAX];
pthread_mutex_t lock;
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
    return NULL;
}

int main(){
    arrIndex = 0;
    pthread_t p[MAX];
    for(int i=0; i<MAX; i++) pthread_create(&p[i], NULL, mythread, NULL);
    for(int i=0; i<MAX; i++) pthread_join(p[i], NULL);
    print(arr,MAX);
    return 0;
}