bin_PROGRAMS = fstest coachtester

fstest_SOURCES =       \
fstest.cc              \
../src/dataset.cc      \
../src/forwardselection.cc \
../src/logmgm.cc

fstest_CPPFLAGS = 
fstest_CXXFLAGS = -std=c++98 -g -Wall #-Weffc++
fstest_LDFLAGS = -g -lgsl -lfileio

if DARWIN
fstest_SOURCES += ../src/gslutils.cc
fstest_CPPFLAGS += -I/sw/include -DOLDGSL
fstest_LDFLAGS += -L/sw/lib
endif

coachtester_SOURCES =   \
coachtester.cc

coachtester_CPPFLAGS = 
coachtester_CXXFLAGS = -std=c++98 -g -Wall #-Weffc++
coachtester_LDFLAGS = -g -lgsl -lfileio
