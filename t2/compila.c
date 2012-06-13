/*Elias Tandel Barrionovo  1010270 3WA*/
/*João Pedro Fabris Campos 1010273 3WA*/

#include "compila.h"
#include <assert.h>
#include <stdlib.h>

static union integer {
    unsigned char c[4];
    int i;
};
typedef union integer Int;

funcp compila(FILE *src){
    int i, ret_val;
    size_t code_size = 12;
    unsigned char gab[12] = {0x55U,0x89U, 0xe5U, 0x89U, 0xecU, 0x5dU, 0xc3U};

    unsigned char *code = (unsigned char*)malloc(code_size*sizeof(unsigned char));
    // inital push and move
    for (i=0; i<7; i++)
        code[i] = gab[i];

/*
    assert(fscanf(src, "ret $%d", &ret_val));
    *(int*)(code+3) = ret_val;
*/

    return (funcp)code;
}
