### Makefile --- 

## Author:   jpms
## Keywords: 
## Revision: $Id$.
## Copyright (c) 2007, Joao Marques-Silva

DISTDATE= $(shell date +'%Y%m%d')

MSU= msuncore

MSCOREDIR = extsrc/mscore
MAXSATZDIR = extsrc/maxsatz-stable
MINISATPDIR = extsrc/minisat+

DISTLIST:= $(MSU)/src $(MSU)/extsrc $(MSU)/scripts $(MSU)/docs
DISTLIST:= $(DISTLIST) $(MSU)/README $(MSU)/Makefile
DISTLIST:= $(DISTLIST) $(MSU)/Results $(MSU)/test-cores
DISTLIST:= $(DISTLIST) $(MSU)/mscore $(MSU)/maxsatz-stable $(MSU)/minisat+
DISTLIST:= $(DISTLIST) $(MSU)/extsrc/mscore $(MSU)/extsrc/maxsatz-stable

all: clean
	(cd $(MSCOREDIR); make all)
	(cd $(MAXSATZDIR); make all)
	(cd $(MINISATPDIR); make all)

clean:
	(cd $(MSCOREDIR); make clean)
	(cd $(MAXSATZDIR); make clean)
	(cd $(MINISATPDIR); make clean)

dist: clean
	(cd ..; \
	tar zcvf msuncore-$(DISTDATE).tgz --exclude=save --exclude=docs --exclude RCS $(DISTLIST))

### Makefile ends here
