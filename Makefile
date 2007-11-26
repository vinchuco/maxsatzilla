CPPFLAGS = -Wall

ifeq ($(shell uname),Darwin)
	CPPFLAGS += -I/sw/include
	LDFLAGS = -L/sw/lib 
endif

all: getfeatures maxsatzilla mszparse

maxsatzilla : main.o math/dataset.o math/forwardselection.o math/libmath.a
	g++ -lgsl -lgslcblas -lcblas -lblas ${LDFLAGS} -o $@ $+ 

getfeatures: features.o MaxSatInstance.o ubcsat/libubcsat.a
	g++ ${LDFLAGS} -o $@ $+

mszparse: mszparse.o
	g++ ${LDFLAGS} -o $@ $+ -lz

mszparse.o: mszparse.cc mszreader.hh

run:
	./maxsatzilla
	./math/plot-gen.bash . driver

clean:
	rm -f driver* *~ *.o getfeatures maxsatzilla mszparse
