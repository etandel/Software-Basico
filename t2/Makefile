IDIR =.
CC=gcc
CFLAGS=-I$(IDIR) -Wall -ggdb

ODIR=obj
LDIR=../lib

LIBS=

_DEPS = compila.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = test.o compila.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean config

clean:
	rm -f test *.txt $(ODIR)/*.o *~ core $(INCDIR)/*~

config:
	[ -d obj ] || mkdir obj
