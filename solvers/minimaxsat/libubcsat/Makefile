.SUFFIXES:	.c
CCC = g++
CCFLAGS = -Wall -O3

MOBJS =  ubcsat.o algorithms.o gsat.o ubcsat-alg.o ubcsat-parms.o novelty.o walksat.o weighted.o reports.o gwsat.o ubcsat-cnf.o ubcsat-reports.o rnovelty.o ubcsat-defaults.o hsat.o ubcsat-help.o ubcsat-time.o saps.o gsat-tabu.o hwsat.o ubcsat-mem.o ubcsat.o walksat-tabu.o ubcsat-io.o mylocal.o mt19937ar.o

HFILES = algorithms.h gsat-tabu.h gsat.h gwsat.h hsat.h hwsat.h mylocal.h novelty.h reports.h rnovelty.h saps.h ubcsat-alg.h ubcsat-cnf.h ubcsat-defaults.h ubcsat-help.h ubcsat-io.h ubcsat-limits.h ubcsat-lit.h ubcsat-mem.h ubcsat-parms.h ubcsat-reports.h ubcsat-time.h ubcsat-types.h ubcsat.h walksat-tabu.h walksat.h weighted.h

.c.o :
	$(CCC) $(CCFLAGS) -c $<

libubcsat.a:	$(MOBJS)
	rm -f libubcsat.a
	ar cr libubcsat.a $(MOBJS)
	ranlib libubcsat.a

ubcsat:		$(MOBJS)
	g++ -o ubcsat $(MOBJS)

$(MOBJS):	$(HFILES)

cleanall:
	rm *.o ubcsat

clean:
	rm *.o 


