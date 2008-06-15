bin_PROGRAMS = coach 
coach_SOURCES =        \
coach.cc               \
dataset.cc             \
forwardselection.cc    \
modeltesting.cc        \
ridgeregression.cc     \
mszreader.hpp          \
logmgm.cc              \
gsladdon.cc            \
svmregression.cc       \
learningalgorithm.cc   \
../../../extlib/libsvm-2.86/svm.cpp

coach_CPPFLAGS = -I../../../extlib/libsvm-2.86/
coach_CXXFLAGS = -std=c++98 -g -Wall #-Weffc++
coach_LDFLAGS = -g -lgsl -lfileio

if DARWIN
coach_SOURCES += gslutils.cc
coach_CPPFLAGS += -I/sw/include -DOLDGSL
coach_LDFLAGS += -L/sw/lib 
endif


