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
    int offset=0, i;
    size_t code_size = 12;

    Int ret_val;
    unsigned char
        begin[3] = {0x55U,0x89U, 0xe5U},
        end[4] = {0x89U, 0xecU, 0x5dU, 0xc3U};

    unsigned char *code = (unsigned char*)malloc(code_size*sizeof(unsigned char));

    // inital push and move
    for (i=0; i<3; i++, offset++)
        code[offset] = begin[i];

    fscanf(src, "ret $%d", &ret_val);
    code[offset++] = 0xb8U;

    for (i=0; i<4; i++, offset++) \
        code[offset] = ret_val.c[i];


    // final move, push and ret
    for (i=0; i<4; i++, offset++)
        code[offset] = end[i];

    return (funcp)code;
}
