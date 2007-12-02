CFLAGS=-O3

all: maxsatz+fl

maxsatz+fl: maxsatz+fl.c libubcsat/libubcsat.a

libubcsat/libubcsat.a:
	(cd libubcsat; make)

clean:
	rm -f maxsatz+fl
