# OK, Simple makefile to generate static library
# and link it to the driver.

LDFLAGS = -lgsl -lz
CXXFLAGS = -Wall -std=c++98 -ggdb
LIB_FILES = dataset.cc forwardselection.cc ridgeregression.cc
HEADERS = $(LIB_FILES:%.cc=%.hh)

ifeq ($(shell uname),Darwin)
	LDFLAGS += -L/sw/lib
	CXXFLAGS += -I/sw/include -DOLDGSL
	LIB_FILES += gslutils.cc
endif

DRIVER_FILES = driver.cc
FSTEST_FILES = fstest.cc

LIBOFILES = $(LIB_FILES:%.cc=%.o)

all: libmath.a driver fstest

libmath.a: $(LIBOFILES) $(HEADERS)
	rm -f $@
	ar cq $@ $(LIBOFILES)

driver: $(DRIVER_FILES) $(HEADERS) libmath.a 
	g++ $(LDFLAGS) $(CXXFLAGS) $(DRIVER_FILES) ./libmath.a -o $@

fstest: $(FSTEST_FILES) $(HEADERS) libmath.a
	g++ $(LDFLAGS) $(CXXFLAGS) $(FSTEST_FILES) ./libmath.a -o $@

clean: 
	rm -f $(LIBOFILES) libmath.a driver fstest
