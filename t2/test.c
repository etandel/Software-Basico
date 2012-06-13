#include "compila.h"
#include <stdio.h>
#include <stdlib.h>

int main(const int argc, const char *argv[]){
    FILE * source = fopen(argv[1], "r");
    funcp compiled;
    int ret;

    compiled = compila(source);
    ret = compiled();
    printf("%d\n", ret);
    free(compiled);

    return 0;
}




