#include<stdio.h>
#define MOD 1000000007
#define TOTAL_NUMS 1000
void main(){
    long int arr[TOTAL_NUMS];
    arr[0] = 12231;
    int a = 3;
    int b = 5;
    printf("%ld\n",arr[0]);
    int t1,t2;
    for(int i=1; i<TOTAL_NUMS; i++){
        t1 = (i-a)%i;
        t2 = (i-b)%i;
        if(t1 < 0) t1 *= -1;
        if(t2 < 0) t2 *= -1;
        arr[i] = (arr[t1] + arr[t2])%MOD;
        printf("%ld\n",arr[i]);
    }
        
}