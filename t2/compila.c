/*Elias Tandel Barrionovo  1010270 3WA*/
/*João Pedro Fabris Campos 1010273 3WA*/

#include "compila.h"
#include <stdlib.h>
#include <string.h>

#define MOVL_FROM_EAX 0x45U
#define MOVL_FROM_EBP 0x8bU

#define MOVL_TO_EAX 0x45U
#define MOVL_TO_ECX 0x4dU
#define MOVL_TO_EBP 0x89U

#define MOVL_CONST_TO_EAX 0xb8U
#define MOVL_CONST_TO_ECX 0xb9U

struct target {
    unsigned char * address;
    int line;
};
struct target_queue {
    int i;
    struct target q[100];
};
typedef struct target_queue TargetQueue;

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

char calc_bp_offset(char t, int i){
    // if v, then offset c [-4,-40]; if p, then offset c [8,48]
    char os = (char)(4*(i+1));
    if (t == 'v')
        os *= -1;
    else
        os += 4;
    return os;
}

static void do_attr(FILE *src, unsigned char *code, int *offset){
    int os = *offset;
    int var0,var1,var2;
    char op;
    char var0t,var1t,var2t;
    fscanf(src, "%c%d = %c%d %c %c%d", &var0t,&var0, &var1t,&var1,&op,&var2t,&var2);

    // move first att'd value to ecx
    if (var1t == '$') {
        //constant
        code[os++] = MOVL_CONST_TO_EAX;
        cpy_int(&var1, code, &os);   
    }
    else {
        //local var
        code[os++] = MOVL_FROM_EBP;
        code[os++] = MOVL_TO_EAX;
        code[os++] = calc_bp_offset(var1t, var1);
    }

    // move second att'd cal to eax
    if (var2t == '$') {
        //constant
        code[os++] = MOVL_CONST_TO_ECX;
        cpy_int(&var2, code, &os);   
    }
    else {
        //local var
        code[os++] = MOVL_FROM_EBP;
        code[os++] = MOVL_TO_ECX;
        code[os++] = calc_bp_offset(var2t, var2);
    }

    //add var2 to var1
    switch (op){
        case '+':
            // addl %ecx, %eax
            code[os++] = 0x01U;
            code[os++] = 0xc8U;
            break;
        
        case '-':
            //sub var2 to var1
            code[os++] = 0x29U;
            code[os++] = 0xc8U;
            break;
        case '*':
            code[os++] = 0x0fU;
            code[os++] = 0xafU;
            code[os++] = 0xc1U;
            break;
    }

    // move var1 op var2 (in %eax) to %ebp+const
    code[os++] = MOVL_TO_EBP;
    code[os++] = MOVL_FROM_EAX;
    code[os++] = calc_bp_offset(var0t, var0);

    *offset = os;
}

static void do_ret(FILE *src, unsigned char *code, int *offset){
    int ret_val;
    int i;
    char c;

    //reads "et x", where x can be $,p,v
    for(i=0; i<4; i++)
        c=fgetc(src);

    //reads either constant or var/param index
    fscanf(src, "%d", &ret_val);
    
    switch (c){
        case '$': //constant
            code[(*offset)++] = 0xb8U; //move constant to eax
            cpy_int(&ret_val, code, offset); //constant is val on ret_val var
            break;

        case 'p': //param
        case 'v': //var
            // move local var to %eax
            code[(*offset)++] = 0x8bU;
            code[(*offset)++] = 0x45U; // move from address %ebp+const 
            code[(*offset)++] = calc_bp_offset(c, ret_val); 
            break;
    }

}

static void do_if(FILE *src, unsigned char *code, int *offset, TargetQueue *queue){
    char var_t;
    int less, equal, greater, var_i, os=*offset;
    fscanf(src, "f %c%d %d %d %d", &var_t, &var_i, &less, &equal, &greater);
    
    //cmpl  $0, offset(%ebp)
    code[os++] = 0x83U; //cmpl
    code[os++] = 0x7dU; //offset ebp
    code[os++] = calc_bp_offset(var_t, var_i);
    code[os++] = (char)0;

    code[os++] = 0x7cU; //jl
    //code[os] = &code[line_address[less]] - &code[os+1];
    code[os++] = (char)0;
    queue->q[queue->i].line = less;
    queue->q[queue->i].address = (code+os);
    queue->i++;

    code[os++] = 0x74U; //je
    //code[os] = &code[line_address[equal]] - &code[os+1];
    code[os++] = (char)0;
    queue->q[queue->i].line = equal;
    queue->q[queue->i].address = (code+os);
    queue->i++;

    code[os++] = 0x7fU; //jg
    //code[os] = &code[line_address[greater]] - &code[os+1];
    code[os++] = (char)0;
    queue->q[queue->i].line = greater;
    queue->q[queue->i].address = (code+os);
    queue->i++;

    *offset = os;
}

static void set_jmp_addresses(unsigned char *code, TargetQueue *queue, int line_address[]){
    int i;
    for (i=0; i<queue->i; i++){
        unsigned char *add = queue->q[i].address;
        *add = &code[line_address[queue->q[i].line]] - add;
    }
}

funcp compila(FILE *src){
    int offset=0;
    size_t code_size = 1000;
    char c;
    int line_address[100];
    TargetQueue target_queue;
    int line_i=0;
    
    target_queue.i = 0;
    unsigned char *code = (unsigned char*)malloc(code_size*sizeof(unsigned char));

    set_head(code, &offset);

    while ((c=fgetc(src)) != EOF) {
        switch (c){
            case 'r':
                line_address[line_i++] = offset;
                do_ret(src, code, &offset);
                break;
            case 'v':
            case 'p':
                line_address[line_i++] = offset;
                ungetc(c, src);
                do_attr(src, code, &offset);
                break;
            case 'i':
                line_address[line_i++] = offset;
                do_if(src, code, &offset, &target_queue);
                break;
        }
    }

    set_tail(code, &offset);
    set_jmp_addresses(code, &target_queue, line_address);

    return (funcp)code;
}
