/*Elias Tandel Barrionovo  1010270 3WA*/
/*João Pedro Fabris Campos 1010273 3WA*/

#include "compila.h"
#include <stdlib.h>
#include <string.h>

static void dump(unsigned char *c, size_t n){
    unsigned char *p;
    for (p=c; n>0; n--, p++)
        printf("%x ", *p);
    putchar('\n');
    
}

void set_head(unsigned char *code, int *offset){
    // inital push and move
    unsigned char begin[3] = {0x55U,0x89U, 0xe5U};
    memcpy(code+*offset, begin, 3);
    *offset += 3;
}

void set_tail(unsigned char *code, int *offset){
    // final move, push and ret
    unsigned char end[4] = {0x89U, 0xecU, 0x5dU, 0xc3U};
    memcpy(code+*offset, end, 4);
    *offset += 4;
}

funcp compila(FILE *src){
    int offset=0, i;
    size_t code_size = 50;
    char c;

    int ret_val;
    
    unsigned char *code = (unsigned char*)malloc(code_size*sizeof(unsigned char));

    set_head(code, &offset);

    if ((c=fgetc(src)) == 'v'){
        // att
        fscanf(src, "0 = $%d", &ret_val);

        // alocate local int
        code[offset++] = 0x83U;
        code[offset++] = 0xecU; //subtract char const from %esp
        code[offset++] = (char)4; //the const is 4 (sizeof(int));

        // move att'd value to local var0
        code[offset++] = 0xc7U;
        code[offset++] = 0x45U; // move val to %ebp+const
        code[offset++] = (char)-4; //const is -4 (var0 is the first int)
        memcpy(code+offset, &ret_val, sizeof(int)); //cpy val
        offset += 4;

        // move local var0 to %eax (ret)
        code[offset++] = 0x8bU;
        code[offset++] = 0x45U; // move from addres %ebp+const 
        code[offset++] = (char)-4; //const is -4 (var0 is the first int)
        
    }
    else {
        //return constant
        fscanf(src, "et $%d", &ret_val);
        code[offset++] = 0xb8U;
        memcpy(code+offset, &ret_val, sizeof(int));
        offset += 4;
    }

    set_tail(code, &offset);

    return (funcp)code;
}
