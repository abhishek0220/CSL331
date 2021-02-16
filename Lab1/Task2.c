#include<stdio.h>
#include <stdlib.h>
#include<time.h>

int getRand(int lowerbound, int upperbound){
    int somerandomnumber = (rand()%(upperbound - lowerbound + 1)) + lowerbound;
    return somerandomnumber;
}
void main(){
    srand(time(0));
    int N = 10;
    for(int i=0; i<N; i++){
        int len = getRand(3,15);
        char str[len+1];
        for(int j=0; j<len; j++)
            str[j] = getRand(97,122);
        str[len] = '\0';            
        printf("%s\n",str);
    }
}