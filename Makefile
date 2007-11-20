CPPFLAGS=-I/sw/include
LDFLAGS=-L/sw/lib

all: maxsatzilla features

maxsatzilla : main.o MaxSatInstance.o math/dataset.o math/forwardselection.o
	g++ ${LDFLAGS} -o $@ $+

features: features.o MaxSatInstance.o
	g++ ${LDFLAGS} -o $@ $+

run:
	./maxsatzilla
	./math/plot-gen.bash . driver

clean:
	rm -f driver* *~ *.o features maxsatzilla
