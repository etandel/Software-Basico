/*Elias Tandel Barrionovo  1010270 3WA*/
/*João Pedro Fabris Campos 1010273 3WA*/

#include "compila.h"

static union integer {
    unsigned char c[4];
    int i;
};
typedef union integer Int;

funcp compila(FILE *src){
    int i, ret_val;
    size_t code_size = 12;

    // inital push and move
    unsigned char code[12] = {0x55U,0x89U, 0xe5U};

    assert(fscanf(src, "ret $%d", &ret_val));
    *(int*)(code+3) = ret_val;

    // final move and pop and ret
    code[code_size-3] = 0x89U;
    code[code_size-2] = 0xecU
    code[code_size-1] = 0x5dU
    code[code_size]   = 0xc3U;

    return (funcp)code;
}
