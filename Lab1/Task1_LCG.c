#include<stdio.h>
#define MOD 1000000007

long int get_rand(int a, int b, long int *seed){
    *seed = (*seed * a + b)%MOD;
    return *seed;
}
void main(){
    long int seed = 912345122;
    int a = 7;
    int b = 11;
    for(int i=0; i<15; i++)
        printf("%ld\n",get_rand(a,b,&seed));
}