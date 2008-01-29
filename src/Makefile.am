bin_PROGRAMS = maxsatzilla.bin getfeatures

maxsatzilla_bin_SOURCES = \
main.cc                   \
MaxSatInstance.cc         \
argumentsparser.cc 

maxsatzilla_bin_CPPFLAGS = -I$(top_srcdir)/../../zilla-common/trunk/libfileIO/src/
maxsatzilla_bin_CXXFLAGS = -Wall -std=c++98 -ggdb
maxsatzilla_bin_LDFLAGS = $(top_srcdir)/ubcsat/libubcsat.la $(top_srcdir)/../../zilla-common/trunk/libfileIO/src/libfileio.la

getfeatures_SOURCES = \
features.cc           \
MaxSatInstance.cc

getfeatures_CXXFLAGS = -Wall -std=c++98 -ggdb
getfeatures_LDFLAGS = $(top_srcdir)/ubcsat/libubcsat.la

