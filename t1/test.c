//#include "utft.h"
#include "utft.h"
#include <stdio.h>

void dump (void *p, int n);

int main(void){
    FILE * u8 = fopen("examples/utf8_demo.txt", "rb");

    //FILE * u32_big = fopen("examples/utf32_err.txt", "rb");
    //FILE * u32_lit = fopen("examples/utf32_lit.txt", "rb");

    //FILE * u8_from_big = fopen("big_to_u8.txt", "wb");
    //FILE * u8_from_lit = fopen("lit_to_u8.txt", "wb");
    //char seq[4];

    utf8_32(u8, stdout, 1);

    //utf32_8(u32_big, u8_from_big);
    //utf32_8(u32_lit, stdout);

    //fclose(u32_big); fclose(u8_from_big);
    //fclose(u8_from_lit); fclose(u32_lit);
    fclose(u8);

    return 0;
}
