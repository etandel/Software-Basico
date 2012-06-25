#include "compila.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
static int xparams(const char *fname, int params[]){
    char digits[6];
    int nparams = 0, i=0, j=0, k=0;
    int flag = 0;
    while (fname){
        if (fname[i] == '_'){
            for (j=0; fname[i+1] != '\0' || fname[i+1] != '_'; i++, j++)
                digits[j] = fname[i+1];
            digits[j+1] = '\0';
            params[k++] = atoi(digits);
            nparams++;
        }
        i++;
    }
    return nparams;
}
*/

int main(const int argc, const char *argv[]){
    FILE * source = fopen(argv[1], "r");
    funcp compiled;
    int ret;
    /*
    int nparams, i;
    int params[10];

    printf("'%s'\n", argv[1]);
    nparams = xparams(argv[1], params);
    printf("nparams: %d\n", nparams);
    for (i=0; i<nparams; i++)
        printf("param %d = %d\n", i, params[i]);

    */
    compiled = compila(source);
    ret = compiled();
    printf("%d\n", ret);
    free(compiled);

    return 0;
}




