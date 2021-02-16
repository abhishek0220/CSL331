#include<stdio.h>
#define POLY_MASK_32 0xB4BCD35C
#define POLY_MASK_31 0x7A5BC2E3

typedef unsigned int uint;
uint lfsr32, lfsr31;
int shift_lfsr(uint *lfsr, uint poly){
    int fb;
    fb = *lfsr & 1;
    *lfsr >>=1;
    if(fb == 1){
        *lfsr ^= poly;
    }
    return *lfsr;
}
void init_lfsr(void){
    lfsr32 = 0xABCDE;
    lfsr31 = 0x23456789;
}
int get_rand(void){
    shift_lfsr(&lfsr32, POLY_MASK_32);
    return (shift_lfsr(&lfsr32, POLY_MASK_32)^shift_lfsr(&lfsr31, POLY_MASK_31)) & 0xffff;
}
# include <stdint.h>
uint16_t start_state = 0xACE1u; 
unsigned lfsr3(void)
{ /* Any nonzero start state will work. */
    uint16_t lfsr = start_state;
    unsigned period = 0;
    printf("%u ---------\n",lfsr);
    do
    {
        lfsr ^= lfsr >> 7;
        lfsr ^= lfsr << 9;
        lfsr ^= lfsr >> 13;
        ++period;
    }
    while (lfsr != start_state);
    start_state = period;
    return period;
}
void main(void){
    int rand;
    init_lfsr();
    printf("%d\n",lfsr3());
    printf("%d\n",lfsr3());
    printf("%d\n",lfsr3());
}