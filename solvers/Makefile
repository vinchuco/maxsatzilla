
SUBDIRS = maxsatz minimaxsat minisat+ msuncore

.PHONY: subdirs $(SUBDIRS) clean

subdirs : $(SUBDIRS)

$(SUBDIRS): 
	$(MAKE) -C $@

msuncore: maxsatz minimaxsat minisat+


