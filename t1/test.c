//#include "utft.h"
#include "utft.h"
#include <stdio.h>

void dump (void *p, int n);

int main(void){
    FILE * u32_big = fopen("examples/utf32_big.txt", "rb");
    FILE * u32_lit = fopen("examples/utf32_lit.txt", "rb");

    FILE * u8_from_big = fopen("big_to_u8.txt", "wb");
    FILE * u8_from_lit = fopen("lit_to_u8.txt", "wb");
    //char seq[4];

    //char big_32_e  = endian(u32_big);
    //char lit_32_e  = endian(u32_lit);
    // printf("utf32_big end: %c\n", big_32_e < 0 ? 'E' : big_32_e);
    // printf("utf32_lit end: %c\n", lit_32_e < 0 ? 'E' : lit_32_e);


    utf32_8(u32_big, u8_from_big);
    utf32_8(u32_lit, u8_from_lit);

    fclose(u8_from_big); fclose(u8_from_lit);
    fclose(u32_big);     fclose(u32_lit);
    return 0;
}
