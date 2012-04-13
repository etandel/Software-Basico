#ifndef _UTFT
#define _UTFT

#include <stdio.h>

//from utf-8 to utf-32
int utf8_32(FILE *in_file, FILE *out_file, int order);

//from utf-32 to utf-8
int utf32_8(FILE *in_file, FILE *out_file);

#endif
