CPPFLAGS = 
CXXFLAGS = -Wall -std=c++98 -ggdb -I/sw/include 
LDFLAGS = -lgsl -lgslcblas -lz -L/sw/lib

COACH_FILES = coach.cc
COACH_HEADERS =
SUBDIRS = ubcsat math

.PHONY : all clean run subdirs $(SUBDIRS)

ifeq ($(shell uname),Darwin)
	CPPFLAGS += -I/sw/include
	LDFLAGS += -L/sw/lib 
endif

all: getfeatures maxsatzilla mszparse coach

maxsatzilla : main.o math/dataset.o math/forwardselection.o ./math/libmath.a
	g++ -lgsl -lgslcblas -lcblas -lblas ${LDFLAGS} -o $@ $+ 

mathlib: 
	cd math && $(MAKE)

coach: $(COACH_FILES) $(COACH_HEADERS) ./math/libmath.a
	g++ $(LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -o $@ $+

subdirs: $(SUBDIRS)

$(SUBDIRS): 
	$(MAKE) -C $@

getfeatures: features.o MaxSatInstance.o ./ubcsat/libubcsat.a
	g++ ${LDFLAGS} -o $@ $+

mszparse: mszparse.o
	g++ ${LDFLAGS} -o $@ $+ -lz

mszparse.o: mszparse.cc mszreader.hh

run:
	./maxsatzilla
	./math/plot-gen.bash . driver

clean:
	rm -f driver* *~ *.o getfeatures maxsatzilla mszparse coach
