#include<stdio.h>
#include <stdbool.h> 
#include <stdlib.h>
#include<string.h>
#include <sys/time.h>
#define MOD 1000000007
#define VASBits 16 // 64KB (2^6 * 2^10) bits
#define PMemBits 24 // 16MB = 16 * 2^10 * 2^10 = 2^24 bits
#define PGSIZEBits 10 // 1KB = 2*10 bits

long int seed = MOD;
int randA = 7;
int randB = 11;

long int get_rand(){
    seed = (seed * randA + randB)%MOD;
    return seed;
}

struct pageEntry
{
    int page;
    bool Valid;
    bool Present;
    bool Protected[3];
    bool UserBit;
};

int ipow(int base, int exp){
    int result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }
    return result;
}

char *decimal_to_binary(int n, int pl)
{
    int c, d, t;
    char *p;
    t = 0;
    p = (char*)malloc(pl+2);
    if (p == NULL)
        exit(EXIT_FAILURE);
    *p = '0';
    *(p+1) = 'b';
    t = 2;
    for (c = pl-1 ; c >= 0 ; c--){
        d = n >> c;
        if (d & 1)
        *(p+t) = 1 + '0';
        else
        *(p+t) = 0 + '0';
        t++;
    }
    *(p+t) = '\0';
    return  p;
}

void makeEntries(int vpages, int pages, struct pageEntry entries[], bool used[]){
    for(int i=0; i<vpages; i++){
        int valid = get_rand()%2;
        if(valid){
            entries[i].Valid = true;
            for(int j=0; j<3; j++) entries[i].Protected[j] = get_rand()%2;
            if(entries[i].Protected[0] == false){
                int tem_page;
                do{
                    tem_page = get_rand()%pages;
                }while(used[tem_page] == true);
                used[tem_page] = true;
                entries[i].page = tem_page;
            }
        }
        else{
            entries[i].Valid = false;
        }
    }
}

void main(){
    struct timeval start, end;
    int Pmem = ipow(2, PMemBits);
    int VAddrSpace = ipow(2, VASBits);
    int PSize = ipow(2, PGSIZEBits);

    int vpnBits = VASBits - PGSIZEBits;
    int vpages = VAddrSpace/ PSize;
    int pages = Pmem/ PSize;

    bool used[pages];
    for(int i=0; i<pages; i++) used[i] = false;
    struct pageEntry entries[vpages];

    makeEntries(vpages, pages, entries, used);

    int PMASK = 0xFFFF & ~(PSize - 1);
    int flag, paddr;
    long int timeTaken[3] = {0,0,0};
    int occur[3] = {0,0,0};
    long int st, en, dif;
    for(int i=0; i<10; i++){
        gettimeofday(&start, NULL);
        int vadr = get_rand()%VAddrSpace;
        int vpn = (vadr & PMASK) >> PGSIZEBits;
        printf("VA %d: %s (Decimal : %d)\n",i+1,decimal_to_binary(vadr, VASBits), vadr);
        //printf("\tVPN --> %s (Decimal : %d)\n", decimal_to_binary(vpn,vpnBits), vpn );
        if(entries[vpn].Valid == false) flag = 0;
        else if(entries[vpn].Protected[0]) flag = 1; 
        else{
            int offset = vadr & (PSize-1);
            int pageNo = entries[vpn].page;
            paddr = (pageNo << PGSIZEBits) | offset;
            flag = 2;
        }
        gettimeofday(&end, NULL);
        dif = ((end.tv_sec - start.tv_sec) *1000000) + (end.tv_usec - start.tv_usec);
        if(flag == 0) printf("\tSEGMENTATION_FAULT\n");
        else if(flag == 1) printf("\tPROTECTION_FAULT\n");
        else printf("\tPhysical Address --> %s\n", decimal_to_binary(paddr, PMemBits));
        timeTaken[flag] += dif;
        occur[flag]++;
        //printf("\tOFFSET --> %s (Decimal : %d)\n", decimal_to_binary(offset,10), offset );
    }
    printf("\n\n");
    printf("Type of Operation | Total Time Taken(in micro Sec) | No of Ocuurences | Avg Time\n");
    printf("------------------------------------------------------------------\n");
    printf("%-17s | %-30ld | %-16d | %-8.2f\n", "Invalid", timeTaken[0], occur[0], timeTaken[0]/(occur[0]*1.0) );
    printf("%-17s | %-30ld | %-16d | %-8.2f\n", "Protected", timeTaken[1], occur[1], timeTaken[1]/(occur[1]*1.0) );
    printf("%-17s | %-30ld | %-16d | %-8.2f\n", "Valid", timeTaken[2], occur[2], timeTaken[2]/(occur[2]*1.0) );
    printf("\n\n");
    
}