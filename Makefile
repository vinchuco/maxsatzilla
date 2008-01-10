CXXFLAGS = -Wall -std=c++98 -ggdb

COACH_FILES = coach.cc coachconfigreader.cc coachmodelwriter.cc reader.cc modeltesting.cc
COACH_HEADERS = coachconfigreader.hh coachmodelwriter.hh reader.hh modeltesting.hh
SUBDIRS = ubcsat math

MAXSATZILLA_OBJECT_FILES = main.o reader.cc mszmodelreader.o getfeatures_wrapper.o math/dataset.o math/forwardselection.o ./math/libmath.a
FEATURES_OBJECT_FILES = features.o MaxSatInstance.o ./ubcsat/libubcsat.a

ifeq ($(shell uname),Darwin)
	CPPFLAGS += -I/sw/include
	LDFLAGS += -L/sw/lib 
endif

MAXSATZILLA_LDFLAGS = -lgsl -lgslcblas -lcblas -lblas $(LDFLAGS)
COACH_LDFLAGS = -lgsl -lgslcblas -lcblas -lblas $(LDFLAGS)
MSZPARSE_LDFLAGS = $(LDFLAGS)

.PHONY : all clean run subdirs $(SUBDIRS)

all: getfeatures maxsatzilla.bin mszparse coach

maxsatzilla.bin : $(MAXSATZILLA_OBJECT_FILES)
	g++ ${MAXSATZILLA_LDFLAGS} -o $@ $+ 

./math/libmath.a: 
	cd math && $(MAKE)

coach: $(COACH_FILES) $(COACH_HEADERS) ./math/libmath.a
	g++ $(COACH_LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -o $@ $+

subdirs: $(SUBDIRS)

$(SUBDIRS): 
	$(MAKE) -C $@

getfeatures: $(FEATURES_OBJECT_FILES)
	g++ $(CXXFLAGS) $(LDFLAGS) -o $@ $+

./ubcsat/libubcsat.a:
	cd ubcsat && $(MAKE)

mszparse: mszparse.o
	g++ ${MSZPARSE_LDFLAGS} -o $@ $+

mszparse.o: mszparse.cc mszreader.hh

clean:
	rm -f *~ *.o getfeatures maxsatzilla.bin mszparse coach ./ubcsat/libubcsat.a ./math/libmath.a
