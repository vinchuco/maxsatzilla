### Makefile --- 

## Author:   jpms
## Keywords: 
## Revision: $Id$.
## Copyright (c) 2007, Joao Marques-Silva

DISTDATE= $(shell date +'%Y%m%d')

MSU= msuncore

MSCOREDIR = extsrc/mscore
MAXSATZDIR = extsrc/maxsatz
MINISATPDIR = extsrc/minisat+
MINISATDIR = extsrc/minisat+maxsat/minisat+

DISTLIST:= $(MSU)/src $(MSU)/extsrc $(MSU)/scripts $(MSU)/docs
DISTLIST:= $(DISTLIST) $(MSU)/README $(MSU)/Makefile
DISTLIST:= $(DISTLIST) $(MSU)/Results $(MSU)/test-cores
DISTLIST:= $(DISTLIST) $(MSU)/mscore $(MSU)/maxsatz $(MSU)/minisat+
DISTLIST:= $(DISTLIST) $(MSU)/extsrc/mscore $(MSU)/extsrc/maxsatz
DISTLIST:= $(DISTLIST) $(MSU)/extsrc/minisat+ $(MSU)/extsrc/minisat+maxsat

all: clean
	(cd $(MSCOREDIR); make all)
	(cd $(MAXSATZDIR); make all)
	(cd $(MINISATPDIR); make all)
	(cd $(MINISATDIR); make all)

clean:
	(cd $(MSCOREDIR); make clean)
	(cd $(MAXSATZDIR); make clean)
	(cd $(MINISATPDIR); make clean)
	(cd $(MINISATDIR); make clean)

dist: clean
	(cd ..; \
	tar zcvf msuncore-$(DISTDATE).tgz --exclude RCS $(DISTLIST))

### Makefile ends here
