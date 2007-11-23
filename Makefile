LDFLAGS=-lgsl -lcblas -lblas

ifeq ($(shell uname),Darwin)
	CPPFLAGS = -I/sw/include
	LDFLAGS += -L/sw/lib 
endif

all: getfeatures maxsatzilla 

maxsatzilla : main.o math/dataset.o math/forwardselection.o math/libmath.a
	g++ ${LDFLAGS} -o $@ $+ 

getfeatures: features.o MaxSatInstance.o ubcsat/libubcsat.a
	g++ ${LDFLAGS} -o $@ $+

mszparse: mszparse.o mszreader.h
	g++ ${LDFLAGS} -o $@ $+ -lz

run:
	./maxsatzilla
	./math/plot-gen.bash . driver

clean:
	rm -f driver* *~ *.o getfeatures maxsatzilla mszparse
