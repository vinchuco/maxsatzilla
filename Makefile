CXXFLAGS = -Wall -std=c++98 -ggdb

MAXSATZILLA_OBJECT_FILES = main.o reader.cc mszmodelreader.o getfeatures_wrapper.o
FEATURES_OBJECT_FILES = features.o MaxSatInstance.o ./ubcsat/libubcsat.a

ifeq ($(shell uname),Darwin)
	CPPFLAGS += -I/sw/include
	LDFLAGS += -L/sw/lib 
endif

MAXSATZILLA_LDFLAGS = -lgsl -lgslcblas -lcblas -lblas $(LDFLAGS)

.PHONY : all clean run subdirs $(SUBDIRS)

all: getfeatures maxsatzilla.bin mszparse coach

maxsatzilla.bin : $(MAXSATZILLA_OBJECT_FILES)
	g++ ${MAXSATZILLA_LDFLAGS} -o $@ $+ 

subdirs: $(SUBDIRS)

$(SUBDIRS): 
	$(MAKE) -C $@

getfeatures: $(FEATURES_OBJECT_FILES)
	g++ $(CXXFLAGS) $(LDFLAGS) -o $@ $+

./ubcsat/libubcsat.a:
	cd ubcsat && $(MAKE)

clean:
	rm -f *~ *.o getfeatures maxsatzilla.bin ./ubcsat/libubcsat.a
