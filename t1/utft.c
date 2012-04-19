/* 
 * TODO: Finish utf8_32
 * TODO: Test   utf8_32
*/

#include "utft.h"
#include <stdio.h>

// macros for errors; since EOF is system dependent,
// so are they (though EOF is usually -1)
#define ERR_READ  EOF-1 // probably -2
#define ERR_WRITE EOF-2 // probably -3
#define ERR_PARSE 0     // error when read character is not valid UTF
#define ERR_END   EOF-3 // error when file's endian is not valid
#define SUCCESS   1     //everything went smooth

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
    //reads 1 array of 4 bytes from f and store it in arr
    return fread(arr, 1, 4, f);
}

static int get_err(FILE *f){
    int err = SUCCESS;
    if (ferror(f)){
        perror("Erro ao ler arquivo:\n");
        err = ERR_READ;
    }
    else if (feof(f))
        err = EOF;
    return err;
}

static char endian(FILE * f){
    char ret;
    char read[4];
    char big[4] = {'\0'  , '\0'  , '\376', '\377'};
    char lit[4] = {'\377', '\376', '\0'  , '\0'};
    
    fread_32(f, read);

    //if everything's ok
    if ((ret = get_err(f)) == SUCCESS){
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

static unsigned int next_char_32(FILE *in_file){
    unsigned int c;
    int err;
    fread_32(in_file, &c);
    if ((err = get_err(in_file)) != SUCCESS)
        return err;
    return c;
}

static int interval_32_lit (unsigned int c){
    int ret;
    if      (c < 0 || c > 0x10ffffu) { //out of bounds
        dump_char(&c, sizeof(c), NULL);
        printf("%x\n", c);
        ret = ERR_PARSE;
    }
    else if (c <= 0x007fu)
        ret = 1;
    else if (c <= 0x07ffu)
        ret = 2;
    else if (c <= 0xffff)
        ret = 3;
    else if (c <= 0x10ffffu)
        ret = 4;
    return ret;
}

static int parse_32_lit (unsigned int c, unsigned char conv[]){
    int inter = interval_32_lit(c);
    unsigned char * ch = (unsigned char*)&c;

    switch(inter) {
        case 1:
            conv[0] = 0 | ch[0]; //begins with 0 and the rest is the utf code
            conv[1] = 0;     //EOS
            break;
        case 2:
            conv[0] = (ch[0] >> 6) | (ch[1] << 2) | 0xc0u;
            conv[1] = (ch[0] & 0x3fu) | 0x80u;
            conv[2] = 0;     //EOS
            break;
        case 3:
            conv[0] = ((ch[1] & 0xf0u) >> 4) | 0xe0u;
            conv[1] = (ch[0] >> 6) | ((ch[1] & 0x0fu) << 2) | 0x80u;
            conv[2] = (ch[0] & 0x3fu) | 0x80u;
            conv[3] = 0;     //EOS
            break;
        case 4:
            conv[0] = ((ch[2] & 0x1cu) >> 2) | 0xf0u;
            conv[1] = ((ch[1] & 0xf0u) >> 4) | (ch[2] & 0x03u) | 0x80u ;
            conv[2] = (ch[0] >> 6) | ((ch[1] & 0x0fu) << 2) | 0x80u;
            conv[3] = (ch[0] & 0x3fu) | 0x80u;
            conv[4] = 0;     //EOS
            break;
        default:
            break;
    }

    return inter;           
}

static int big2lit (unsigned int c){
    unsigned char new[4], *real = (unsigned char*)&c; 
    int i;
    for (i=0; i<4; i++)
        new[3-i] = real[i];
    return *(unsigned int *)new;
}
static int parse_32_big(unsigned int c, unsigned char conv[]){
    return parse_32_lit(big2lit(c), conv);
}

int utf32_8(FILE *in_file, FILE *out_file){
    unsigned int r_char;
    unsigned char conv[5]; // maximum of 4 bytes in a utf-8 + EOS
    int (*parse_32)(unsigned int, unsigned char []);

    // sets callback accordingly while checking for possible errors
    switch(endian(in_file)){
        case 'L':
            parse_32 = parse_32_lit; break;
        case 'B':
            parse_32 = parse_32_big; break;
        default:
            return -1;
    }

    r_char = next_char_32(in_file);
    while (r_char != EOF) {
        if (r_char == ERR_READ) //could not read
            return -1;

        if (!parse_32(r_char, conv)){ // bad character
            fprintf(stderr, "Erro! Caracter UTF-32 invalido na posicao %ld:\n  %.8x", ftell(in_file), r_char);
            return -1;
        }

        fprintf(out_file, "%s", conv);
        if (ferror(out_file)){ //could not write
            perror("Erro ao escrever arquivo:\n");
            return -1;
        }

        r_char = next_char_32(in_file);
    }

    return 0;
}

/****************** Begin utf8_32 *******************/

static int next_char_8(FILE * f, unsigned char c[]){
    //TODO: Needs testing;
    unsigned char r_char, nbytes, mask;
    int err;

    r_char = c[0] = fgetc(f);

    //checks for errors / EOF
    if ((err = get_err(f)) != SUCCESS)
        return err;

    //counts number of bytes on utf8 character
    for (nbytes=0, mask=0x80u; r_char & mask; mask>>=1, nbytes++);
    //invalid char or something went wrong
    if (nbytes > 4){
        fprintf(stderr, "Erro! Caracter UTF-8 invalido na posicao %ld:\n", ftell(f));
        return ERR_PARSE;
    }

    // if only one byte, for won't loop, so nbyte will be 0
    nbytes = nbytes ? nbytes : nbytes+1;
    //reads nbytes-1, because of fgetc on start of this function
    fread(c+1, 1, nbytes-1, f);
    if ((err = get_err(f)) != SUCCESS)
        return err;

    return nbytes;
}

int utf8_32(FILE *in_file, FILE *out_file, int order){
    unsigned char r_char[4];
    int r_nbytes;

    r_nbytes = next_char_8(in_file, r_char);
    while(!(feof(in_file) || r_nbytes >= 0)){
        r_nbytes = next_char_8(in_file, r_char);
    }

    return 0;
}
