bin_PROGRAMS = coach 
coach_SOURCES =        \
coach.cc               \
dataset.cc             \
forwardselection.cc    \
modeltesting.cc        \
ridgeregression.cc     \
mszreader.hpp          \
logmgm.cc              \
gsladdon.cc

coach_CPPFLAGS = 
coach_CXXFLAGS = -std=c++98 -g -Wall #-Weffc++
coach_LDFLAGS = -g -lgsl -lfileio

if DARWIN
coach_SOURCES += gslutils.cc
coach_CPPFLAGS += -I/sw/include -DOLDGSL
coach_LDFLAGS += -L/sw/lib 
endif


