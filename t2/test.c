#include "compila.h"
#include <stdio.h>

int main(const int argc, const char *argv[]){
    FILE * source = fopen(argv[1], "r");
    funcp compiled = compila(source);
    int ret = compiled();
    printf("%s => %d\n", argv[1], ret);
    return 0;
}
