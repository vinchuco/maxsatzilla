LIBS=-lgsl

CPPFLAGS=-I/sw/include
LDFLAGS=-L/sw/lib ${LIBS} 

all: getfeatures maxsatzilla 

maxsatzilla : main.o MaxSatInstance.o math/dataset.o math/forwardselection.o
	g++ ${LDFLAGS} -o $@ $+ 

getfeatures: features.o MaxSatInstance.o ./ubcsat/libubcsat.a
	g++ ${LDFLAGS} -o $@ $+

mszparse: mszparse.o mszreader.h
	g++ ${LDFLAGS} -o $@ $+ -lz

run:
	./maxsatzilla
	./math/plot-gen.bash . driver

clean:
	rm -f driver* *~ *.o features maxsatzilla
