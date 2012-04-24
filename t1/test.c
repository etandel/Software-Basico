//#include "utft.h"
#include "utft.h"
#include <stdio.h>

#define LIT 0
#define BIG 1

void dump_char (void *p, int n, char *sep);

int main(void){
    /*
    //FILE * u8 = fopen("examples/utf8_err2.txt", "rb");
    FILE * u8 = fopen("examples/utf8_demo.txt", "rb");
    FILE * lit_from_u8 = fopen("u8_to_lit.txt", "wb");
    FILE * big_from_u8 = fopen("u8_to_big.txt", "wb");

    printf("Lit return %d\n\n", utf8_32(u8, lit_from_u8, LIT));
    rewind(u8);
    printf("Big return %d\n",   utf8_32(u8, big_from_u8, BIG));

    fclose(u8);
    fclose(lit_from_u8);
    fclose(big_from_u8);

    // */
    
    //*
    FILE * u32_big = fopen("examples/utf32_err2.txt", "rb");
    FILE * u32_lit = fopen("examples/utf32_lit.txt", "rb");
    FILE * u8_big = fopen("big_to_u8.txt", "wb");
    FILE * u8_lit = fopen("lit_to_u8.txt", "wb");
    
    printf("Lit return %d\n\n", utf32_8(u32_lit, u8_lit));
    printf("Big return %d\n",   utf32_8(u32_big, u8_big));

    fclose(u32_big);
    fclose(u32_lit);
    fclose(u8_big);
    fclose(u8_lit);
    // */

    return 0;
}
