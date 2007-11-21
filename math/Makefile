# OK, Simple makefile to generate static library
# and link it to the driver.
LDFLAGS = -L/sw/lib -lgsl
ifdef OLDGSL
LIB_FILES = dataset.cc forwardselection.cc gslutils.cc
CXXFLAGS = -Wall -std=c++98 -ggdb -DOLDGSL -I/sw/include
else
LIB_FILES = dataset.cc forwardselection.cc
CXXFLAGS = -Wall -std=c++98 -ggdb -I/sw/include
endif
DRIVER_FILES = driver.cc

LIBOFILES = $(LIB_FILES:%.cc=%.o)

all: libmath.a driver

libmath.a: $(LIBOFILES)
	rm -f $@
	ar cq $@ $(LIBOFILES)

driver: $(DRIVER_FILES) libmath.a 
	g++ $(LDFLAGS) $(CXXFLAGS) $(DRIVER_FILES) ./libmath.a -o $@

clean: 
	rm -f $(LIBOFILES) libmath.a driver
