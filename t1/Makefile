IDIR =.
CC=gcc
CFLAGS=-I$(IDIR) -Wall -ggdb

ODIR=obj
LDIR=../lib

LIBS=

_DEPS = utft.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = test.o utft.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f test *.txt $(ODIR)/*.o *~ core $(INCDIR)/*~
