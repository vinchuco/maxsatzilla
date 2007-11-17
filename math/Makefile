# OK, Simple makefile to generate static library
# and link it to the driver.
CXXFLAGS = -Wall -std=c++98 -ggdb
LIB_FILES = dataset.cc
DRIVER_FILES = driver.cc

LIBOFILES = $(LIB_FILES:%.cc=%.o)

all: libmath.a driver

libmath.a: $(LIBOFILES)
	rm -f $@
	ar cq $@ $(LIBOFILES)

driver: $(DRIVER_FILES) libmath.a 
	g++ $(CXXFLAGS) $(DRIVER_FILES) ./libmath.a -o $@

clean: 
	rm -f $(LIBOFILES) libmath.a driver