### Makefile --- 

## Author:   jpms
## Keywords: 
## Revision: $Id$.
## Copyright (c) 2007, Joao Marques-Silva

SRCS= $(wildcard *.c)
OBJS= $(addsuffix .o, $(basename $(SRCS)))

CTRGT= maxsatz14bis+fl.c
OTRGT= maxsatz14bis+fl.o

CFLAGS = 

TRGT= maxsatz

alld: CFLAGS := $(CFLAGS) -g -ggdb -DDEBUG

all: $(TRGT)

alld: $(TRGT).d

$(TRGT): $(OTRGT)
	gcc -o $(TRGT) $(CTRGT)

$(TRGT).d: $(OTRGT)
	gcc -g -ggdb -o $(TRGT) $(CTRGT)

clean:
	@rm -f *.o a.out $(TRGT)

%.o: %.c
	@echo Compiling: $<
	@$(CC) $(CFLAGS) -c -o $@ $<

### Makefile ends here
