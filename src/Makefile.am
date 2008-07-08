bin_PROGRAMS = coach maxsatzilla.bin getfeatures

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
configreader.cc        \
featurelabel.cc        \
modelreader.cc         \
modelwriter.cc         \
reader.cc              \
model.cc               \
svmmodel.cc            \
rrmodel.cc             \
configreader.hh        \
featurelabel.hh        \
modelreader.hh         \
modelwriter.hh         \
pputils.hh             \
reader.hh              \
model.hh               \
svmmodel.hh            \
rrmodel.hh             \
triple.hpp   
../../../extlib/libsvm-2.86/svm.cpp

coach_CPPFLAGS = -I../../../extlib/libsvm-2.86/
coach_CXXFLAGS = -std=c++98 -g -Wall 
coach_LDFLAGS = -g -lgsl -lfileio

maxsatzilla_bin_SOURCES = \
main.cc                   \
MaxSatInstance.cc         \
argumentsparser.cc        \
configreader.cc           \
featurelabel.cc           \
modelreader.cc            \
modelwriter.cc            \
reader.cc                 \
model.cc                  \
svmmodel.cc               \
rrmodel.cc                \
configreader.hh           \
featurelabel.hh           \
modelreader.hh            \
modelwriter.hh            \
pputils.hh                \
reader.hh                 \
model.hh                  \
svmmodel.hh               \
rrmodel.hh                \
triple.hpp   

maxsatzilla_bin_CPPFLAGS =
maxsatzilla_bin_CXXFLAGS = -Wall -std=c++98 -ggdb
maxsatzilla_bin_LDFLAGS = $(top_srcdir)/ubcsat/libubcsat.la -lgsl

getfeatures_SOURCES = \
features.cc           \
MaxSatInstance.cc

getfeatures_CXXFLAGS = -Wall -std=c++98 -ggdb
getfeatures_LDFLAGS = $(top_srcdir)/ubcsat/libubcsat.la

