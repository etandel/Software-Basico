/* TODO: Finish parse_32 
 * TODO: Test   parse_32
 * TODO: Finish utf32_8
 * TODO: Test   utf32_8
 * TODO: Finish utf8_32
 * TODO: Test   utf8_32
*/

#include "utft.h"
#include <stdio.h>

// macros for errors; since EOF is system dependent,
// so are they (though EOF is usually -1)
#define ERR_READ  EOF-1 // probably -2
#define ERR_WRITE EOF-2 // probably -3
#define ERR_PARSE 0    // error when read character is not valid UTF
#define ERR_END   EOF-3    // error when file's endian is not valid

//int utf8_32(FILE *in_file, FILE *out_file, int order){
//}

void dump_char (void *p, int n, char *sep) {
  unsigned char *p1 = p;
  sep = sep ? sep : " ";
  while (n--) {
    printf("%02x%s", *p1, sep);
    p1++;
  }
  puts("");
}

void dump (void *p, int n) {
  unsigned char *p1 = p;
  while (n--) {
    printf("%p - %02x\n", p1, *p1);
    p1++;
  }
  puts("");
}

static size_t fread_32(FILE *f, void *arr){
    return fread(arr, 4, 1, f);
}

static char endian(FILE * f){
    char ret;
    char read[4];
    char big[4] = {'\0'  , '\0'  , '\376', '\377'};
    char lit[4] = {'\377', '\376', '\0'  , '\0'};
    
    // read 1 array of 4 bytes from f and store it on read
    fread(read, 4, 1, f); 

    //check for errors / EOF
    if (ferror(f)){
        perror("Erro ao ler arquivo:\n");
        ret = ERR_READ;
    }
    else if (feof(f))
        ret = EOF;
    //if everything's ok
    else{
        int i;
        char * expected;

        // if read begins like big, expected should be big
        if      (read[0] == big[0]){
            expected = big;
            ret = 'B';
        }
        // if read begins like little, expected should be little
        else if (read[0] == lit[0]){
            expected = lit;
            ret = 'L';
        }
        // if read is different, file is bizarre and returns error
        else {
            return ERR_END;
        }
        for (i=1; i<4; i++)
            ret = read[i] == expected[i] ? ret : ERR_END;
    }
    return ret;
}

static int next_char_32(FILE *in_file){
    int c;
    fread_32(in_file, &c);
    if (ferror(in_file)){ //error
        perror("Erro ao ler arquivo:\n");
        c = ERR_READ;
    }
    else if (feof(in_file))  //end of file
        c = EOF;
    return c;
}

static int interval_32 (int c){
    int ret;
    if      (c < 0 || c > 0x10ffff) { //out of bounds
        ret = ERR_PARSE;
    }
    else if (c <= 0x007f)
        ret = 1;
    else if (c <= 0x07ff)
        ret = 2;
    else if (c <= 0xffff)
        ret = 3;
    else if (c <= 0x10ffff)
        ret = 4;
    return ret;
}

static int parse_32 (int c, unsigned char conv[]){
    int inter = interval_32(c);
    switch(inter) {
        case 1:
            conv[0] = 0 | c; //begins with 0 and the rest is the utf code
            conv[1] = 0;     //EOS
            break;
        case 2:
            conv[3] = 0;     //EOS
            break;
        case 3:
            conv[4] = 0;     //EOS
            break;
        case 4:
            conv[5] = 0;     //EOS
            break;
        default:
            break;
    }

    return inter;           
}

int utf32_8(FILE *in_file, FILE *out_file){
    char end;
    int r_char;
    unsigned char conv[5]; // maximum of 4 bytes in a utf-8 + EOS

    //puts("On utf32_8");

    //end = endian(in_file);
    //printf("Endian: %c\n", end);

    r_char = next_char_32(in_file);
    //printf("Next char: "); dump_char(&r_char, sizeof(r_char), NULL);
    while (r_char != EOF) {
        if (r_char == ERR_READ) //could not read
            return -1;

        if (!parse_32(r_char, conv)){ // bad character
            long int pos = ftell(in_file);
            fprintf(stderr, "Erro! Caracter UTF-32 invalido na posicao %ld.\n", pos);
            dump_char(&r_char, sizeof(r_char), NULL);
            return -1;
        }
//
//        //fprintf(out_file, "%s", conv);
//        if (ferror(out_file)){ //could not write
//            perror("Erro ao escrever arquivo:\n");
//            return -1;
//        }
//
        dump_char(&r_char, sizeof(r_char), NULL);
        r_char = next_char_32(in_file);
    }


    return 0;
}
