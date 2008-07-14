bin_PROGRAMS = coach furius getfeatures

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
timeoutmgm.cc          \
configreader.hh        \
featurelabel.hh        \
modelreader.hh         \
modelwriter.hh         \
pputils.hh             \
reader.hh              \
model.hh               \
svmmodel.hh            \
rrmodel.hh             \
timeoutmgm.hh          \
triple.hpp             \
../../extlib/libsvm-2.86/svm.cpp

coach_CPPFLAGS = -I../../extlib/libsvm-2.86/
coach_CXXFLAGS = -std=c++98 -g -Wall 
coach_LDFLAGS = -g -lgsl

furius_SOURCES = \
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
triple.hpp                \
../../extlib/libsvm-2.86/svm.cpp

furius_CPPFLAGS = -I../../extlib/libsvm-2.86/
furius_CXXFLAGS = -Wall -std=c++98 -ggdb
furius_LDFLAGS = $(top_srcdir)/ubcsat/libubcsat.la -lgsl

getfeatures_SOURCES = \
features.cc           \
MaxSatInstance.cc

getfeatures_CXXFLAGS = -Wall -std=c++98 -ggdb
getfeatures_LDFLAGS = $(top_srcdir)/ubcsat/libubcsat.la

