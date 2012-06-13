#ifndef __COMPILA_H
#define __COMPILA_H

#include <stdio.h>

typedef int (*funcp) (void);
funcp compila (FILE *f);

#endif
