### Makefile --- 

## Author:   jpms
## Keywords: 
## Revision: $Id$.
## Copyright (c) 2007, Joao Marques-Silva

SRCS= $(wildcard *.c)
OBJS= $(addsuffix .o, $(basename $(SRCS)))

CTRGT= maxsatz-evaluation.c
OTRGT= maxsatz-evaluation.o

CFLAGS = 

TRGT= maxsatz

all: CFLAGS := $(CFLAGS) -O3

alld: CFLAGS := $(CFLAGS) -g -ggdb -DDEBUG

all: $(TRGT)

alld: $(TRGT)

$(TRGT): $(OTRGT)
	gcc $(CFLAGS) -o $(TRGT) $(CTRGT)

clean:
	@rm -f *.o a.out $(TRGT)

%.o: %.c
	@echo Compiling: $<
	@$(CC) $(CFLAGS) -c -o $@ $<

### Makefile ends here
