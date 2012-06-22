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

    // alocate local vars
    code[(*offset)++] = 0x83U;
    code[(*offset)++] = 0xecU; //subtract char const from %esp
    code[(*offset)++] = (char)80; //the const is 20*(sizeof(int)) == 80;

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

static void do_attr(FILE *src, unsigned char *code, int *offset, char type){
    int attval, os = *offset;
    int var_index;
    char bp_offset;

    fscanf(src, "%d = $%d", &var_index, &attval);

    // if v, then offset c [-4,-40]; if p, then offset c [-44,-80]
    bp_offset = (char)(-4*(var_index+1) - (type == 'v' ? 0 : 40));

    // move att'd value to local var
    code[os++] = 0xc7U;
    code[os++] = 0x45U; // move val to %ebp+const
    code[os++] = bp_offset;
    cpy_int(&attval, code, &os);

    *offset = os;
}

static void do_ret(FILE *src, unsigned char *code, int *offset){
    int ret_val;
    int i;
    char c, bp_offset;

    //reads "et x", where x can be $,p,v
    for(i=0; i<4; i++)
        c=fgetc(src);

    //reads either constant or var/param index
    fscanf(src, "%d", &ret_val);
    
    switch (c){
        case '$': //constant
            code[(*offset)++] = 0xb8U;
            cpy_int(&ret_val, code, offset);
            break;

        case 'p': //param
            bp_offset = (char)(-4*(ret_val+1) - 40);
            // move local var to %eax
            code[(*offset)++] = 0x8bU;
            code[(*offset)++] = 0x45U; // move from addres %ebp+const 
            code[(*offset)++] = bp_offset; 
            break;
        case 'v': //var
            bp_offset = (char)(-4*(ret_val+1));
            // move local var to %eax
            code[(*offset)++] = 0x8bU;
            code[(*offset)++] = 0x45U; // move from addres %ebp+const 
            code[(*offset)++] = bp_offset; 
            break;
    }

}

funcp compila(FILE *src){
    int offset=0;
    size_t code_size = 50;
    char c;
    
    unsigned char *code = (unsigned char*)malloc(code_size*sizeof(unsigned char));

    set_head(code, &offset);

    while ((c=fgetc(src)) != EOF) {
        switch (c){
            case 'r':
                do_ret(src, code, &offset);
                break;
            case 'v':
            case 'p':
                do_attr(src, code, &offset, c);
                break;
        }
    }

    set_tail(code, &offset);

    return (funcp)code;
}
