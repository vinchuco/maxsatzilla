LIBS=-lgsl

CPPFLAGS=-I/sw/include
LDFLAGS=-L/sw/lib ${LIBS} 

all: features maxsatzilla 

maxsatzilla : main.o MaxSatInstance.o math/dataset.o math/forwardselection.o ./ubcsat/libubcsat.a
	g++ ${LDFLAGS} -o $@ $+

features: features.o MaxSatInstance.o
	g++ ${LDFLAGS} -o $@ $+

run:
	./maxsatzilla
	./math/plot-gen.bash . driver

clean:
	rm -f driver* *~ *.o features maxsatzilla
