#include "compila.h"
#include <stdio.h>

int main(const int argc, const char *argv[]){
    FILE * source = fopen(argv[1], "r");
    funcp compiled = compila(source);
    int ret = compiled();
    printf("%d\n", ret);
    return 0;
}




