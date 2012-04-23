/* 
 * TODO: Test error handling  utf32_8
 * TODO: Test error handling  utf8_32
*/

#include "utft.h"
#include <stdio.h>
#include <string.h>

// macros for errors; since EOF is system dependent,
// so are they (though EOF is usually -1)
#define ERR_READ  EOF-1 // probably -2
#define ERR_WRITE EOF-2 // probably -3
#define ERR_PARSE EOF-3     // error when read character is not valid UTF
#define ERR_END   EOF-4 // error when file's endian is not valid
#define SUCCESS   1     //everything went smooth

void dump_char (void *p, int n, char *sep){
    unsigned char *p1 = p;
    sep = sep ? sep : " ";
    while (n--) {
        printf("%02x%s", *p1, sep);
        p1++;
    }
    puts("");
}

void dump (void *p, int n){
    unsigned char *p1 = p;
    while (n--) {
        printf("%p - %02x\n", p1, *p1);
        p1++;
    }
    puts("");
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
    
    fread(read, 1, 4, f);

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

static int next_char_32(FILE *in_file, unsigned char c[]){
    int err;
    fread(c, 1, 4, in_file);
    if ((err = get_err(in_file)) != SUCCESS)
        return err;
    return SUCCESS;
}

static int interval_32_lit (unsigned char r_char[]){
    unsigned int c = *(unsigned int *)r_char;
    int ret;
    if      (c < 0 || c > 0x10ffffu) { //out of bounds
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

static int parse_32_lit (unsigned char r_char[], unsigned char conv[]){
    int inter = interval_32_lit(r_char);

    switch(inter) {
        case 1:
            conv[0] = 0 | r_char[0]; //begins with 0 and the rest is the utf code
            conv[1] = 0;     //EOS
            break;
        case 2:
            conv[0] = (r_char[0] >> 6) | (r_char[1] << 2) | 0xc0u;
            conv[1] = (r_char[0] & 0x3fu) | 0x80u;
            conv[2] = 0;     //EOS
            break;
        case 3:
            conv[0] = ((r_char[1] & 0xf0u) >> 4) | 0xe0u;
            conv[1] = (r_char[0] >> 6) | ((r_char[1] & 0x0fu) << 2) | 0x80u;
            conv[2] = (r_char[0] & 0x3fu) | 0x80u;
            conv[3] = 0;     //EOS
            break;
        case 4:
            conv[0] = ((r_char[2] & 0x1cu) >> 2) | 0xf0u;
            conv[1] = ((r_char[1] & 0xf0u) >> 4) | (r_char[2] & 0x03u) | 0x80u ;
            conv[2] = (r_char[0] >> 6) | ((r_char[1] & 0x0fu) << 2) | 0x80u;
            conv[3] = (r_char[0] & 0x3fu) | 0x80u;
            conv[4] = 0;     //EOS
            break;
        default:
            break;
    }

    return inter;           
}

static void big2lit (unsigned char big[], unsigned char lit[]){
    int i;
    for (i=0; i<4; i++)
        lit[3-i] = big[i];
}

static int parse_32_big(unsigned char c[], unsigned char conv[]){
    unsigned char lit[4];
    big2lit(c, lit);
    return parse_32_lit(lit, conv);
}

int utf32_8(FILE *in_file, FILE *out_file){
    int err;
    unsigned char r_char[4];
    unsigned char conv[5]; // maximum of 4 bytes in a utf-8 + EOS
    int (*parse)(unsigned char[], unsigned char []);

    // sets callback accordingly while checking for possible errors
    switch(endian(in_file)){
        case 'L':
            parse = parse_32_lit; break;
        case 'B':
            parse = parse_32_big; break;
        default:
            return -1;
    }

    err = next_char_32(in_file, r_char);
    while (err != EOF) {
        if (err == ERR_READ) //could not read
            return -1;

        if (parse(r_char, conv) == ERR_PARSE){ // bad character
            fprintf(stderr, "Erro! Caracter UTF-32 invalido na posicao %ld.\n", ftell(in_file));
            return -1;
        }

        fprintf(out_file, "%s", conv);
        if (ferror(out_file)){ //could not write
            perror("Erro ao escrever arquivo:\n");
            return -1;
        }
        err = next_char_32(in_file, r_char);
    }

    return 0;
}

/****************** Begin utf8_32 *******************/

static int write_endian(FILE *out, int is_big){
    char big[4] = {'\0'  , '\0'  , '\376', '\377'};
    char lit[4] = {'\377', '\376', '\0'  , '\0'};
    char *seq = is_big ? big : lit;
    fwrite(seq, 1, 4, out);
    if (ferror(out)){ //could not write
        perror("Erro ao escrever arquivo.\n");
        return ERR_WRITE;
    }
    return SUCCESS;
}

static int parse_8_2big(unsigned char r_char[], int r_nbytes, unsigned char conv[]){
    int i;

    // if nbytes > 1, checks if every byte is 10xx xxxx
    for (i=1; i<r_nbytes; i++)
        if ((r_char[i] & 0xc0u) != 0x80u)
            return ERR_PARSE;

    switch(r_nbytes){
        case 1:
            memset(conv, 0, 3); //first 3 bytes are 0
            conv[3] = r_char[0];
            break;
        case 2:
            memset(conv, 0, 2); //first two bytes are 0
            conv[2] = (r_char[0] >> 2) & 0x07u;
            conv[3] = ((r_char[0] & 0x03u) << 6) | (r_char[1] & 0x3fu);
            break;
        case 3:
            memset(conv, 0, 2); //first two bytes are 0
            conv[2] = (r_char[0] << 4) | ((r_char[1] >> 2) & 0x0fu);
            conv[3] = (r_char[1] << 6) | (r_char[2] & 0x3fu);
            break;
        case 4:
            //TODO: test
            conv[0] = 0; //first byte is 0
            conv[1] = ((r_char[0] & 0x07u) << 2) | ((r_char[1] & 0x30u) >> 4);
            conv[2] = (r_char[1] << 4) | ((r_char[2] & 0x3cu) >> 2);
            conv[3] = (r_char[2] << 6) | (r_char[3] & 0x3du);

            printf("Got:  "); dump_char(r_char, r_nbytes, NULL);
            printf("Conv: "); dump_char(conv, 4, NULL);
            getchar();

            break;
    }

    return SUCCESS;
}

static int parse_8_2lit(unsigned char r_char[], int r_nbytes, unsigned char conv[]){
    int err;
    unsigned char conv2[4];
    if ((err = parse_8_2big(r_char, r_nbytes, conv2)) != SUCCESS)
        return err;

    big2lit(conv2, conv);
    return SUCCESS;
}

static int next_char_8(FILE * f, unsigned char c[]){
    unsigned char r_char, mask;
    int err, nbytes;

    r_char = c[0] = fgetc(f);

    //checks for errors / EOF
    if ((err = get_err(f)) != SUCCESS)
        return err;

    //counts number of bytes on utf8 character
    for (nbytes=0, mask=0x80u; r_char & mask; mask>>=1, nbytes++);
    //invalid char or something went wrong
    if (nbytes == 1 || nbytes > 4){
        fprintf(stderr, "Erro! Caracter UTF-8 invalido na posicao %ld:\n", ftell(f));
        return ERR_PARSE;
    }

    // if only one byte, for won't loop, so nbytes will be 0
    nbytes = nbytes ? nbytes : nbytes+1;
    //reads nbytes-1, because of fgetc on start of this function
    fread(c+1, 1, nbytes-1, f);
    if ((err = get_err(f)) != SUCCESS)
        return err;

    return nbytes;
}

int utf8_32(FILE *in_file, FILE *out_file, int order){
    unsigned char r_char[4], conv[4];
    int r_nbytes;
    int (*parse)(unsigned char[], int, unsigned char []) = order ? parse_8_2big : parse_8_2lit;

    if (write_endian(out_file, order) != SUCCESS)
        return -1;

    if ((r_nbytes = next_char_8(in_file, r_char)) < 0)
        return -1;

    while(!(feof(in_file))){
        if (parse(r_char, r_nbytes, conv) == ERR_PARSE){
            fprintf(stderr, "Erro! Caracter UTF-8 invalido na posicao %ld:\n", ftell(in_file));
            return -1;
        }

        fwrite(conv, 1, 4, out_file);
        if (ferror(out_file)){ //could not write
            perror("Erro ao escrever arquivo.\n");
            return -1;
        }

        r_nbytes = next_char_8(in_file, r_char);
        if (r_nbytes == ERR_PARSE && r_nbytes == ERR_READ)
            return -1;
    }

    return 0;
}
