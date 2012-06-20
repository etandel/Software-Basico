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

static void set_head(unsigned char *code, int *offset){
    // inital push and move
    unsigned char begin[3] = {0x55U,0x89U, 0xe5U};
    memcpy(code+*offset, begin, 3);
    *offset += 3;
}

static void set_tail(unsigned char *code, int *offset){
    // final move, push and ret
    unsigned char end[4] = {0x89U, 0xecU, 0x5dU, 0xc3U};
    memcpy(code+*offset, end, 4);
    *offset += 4;
}

static void cpy_int(int *i, unsigned char *code, int *offset){
    memcpy(code+*offset, i, sizeof(int)); //cpy val
    *offset += 4;
}

static void do_attr(FILE *src, unsigned char *code, int *offset){
    int attval, os = *offset;
    fscanf(src, "0 = $%d", &attval);

    // alocate local int
    code[os++] = 0x83U;
    code[os++] = 0xecU; //subtract char const from %esp
    code[os++] = (char)4; //the const is 4 (sizeof(int));

    // move att'd value to local var0
    code[os++] = 0xc7U;
    code[os++] = 0x45U; // move val to %ebp+const
    code[os++] = (char)-4; //const is -4 (var0 is the first int)
    cpy_int(&attval, code, &os);

    // move local var0 to %eax (ret)
    code[os++] = 0x8bU;
    code[os++] = 0x45U; // move from addres %ebp+const 
    code[os++] = (char)-4; //const is -4 (var0 is the first int)

    *offset = os;
}

static void do_ret(FILE *src, unsigned char *code, int *offset){
    int ret_val;
    fscanf(src, "et $%d", &ret_val);
    code[(*offset)++] = 0xb8U;
    cpy_int(&ret_val, code, offset);
}

funcp compila(FILE *src){
    int offset=0, i;
    size_t code_size = 50;
    char c;

    int ret_val;
    
    unsigned char *code = (unsigned char*)malloc(code_size*sizeof(unsigned char));

    set_head(code, &offset);

    if ((c=fgetc(src)) == 'v'){
        do_attr(src, code, &offset);
    }
    else {
        do_ret(src, code, &offset);
    }

    set_tail(code, &offset);

    return (funcp)code;
}
