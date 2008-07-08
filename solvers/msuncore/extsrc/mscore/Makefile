### Makefile --- 

## Author: jpms
## Version: $Id: Makefile 73 2007-07-26 15:16:48Z jpms $
## Keywords: 
## X-URL: 

#-------------------------------------------------------------------------------
# Basic definition variables (may be extended in each Makefile)
#-------------------------------------------------------------------------------

BUILDDATE  = $(shell date +'%Y-%b-%d %R')
YCSRC      = $(foreach fn,$(basename $(wildcard *.y)), $(fn).tab.c)
LCSRC      = $(foreach fn,$(basename $(wildcard *.l)), lex.$(fn).c)
CXXSRCS    = $(wildcard *.cc)
CXXHDRS    = $(wildcard *.hh)
#CCSRCS     = $(wildcard *.c)
#CCHDRS     = $(wildcard *.h)
SRCS       = $(YCSRC) $(LCSRC) $(CXXSRCS) $(CCSRCS)
OBJS       = $(addsuffix .o, $(basename $(SRCS)))
OBJS	   := $(EXTOBJS) $(OBJS)

YACCSRC    = $(wildcard *.y)
LEXSRC     = $(wildcard *.l)

CPPFLAGS += -Wall -ansi -DBUILDDATE='"$(BUILDDATE)"' -DDISTDATE='"$(DISTDATE)"' -I.
CPPFLAGS   += -O3 -DNDEBUG -DNCHECK

# ___EXT_DEFS___ 
XTRGT=mscore
DISTDATE=2007-09-19
XLNK=-lz

LNKFLAGS += $(XLNK)

#-------------------------------------------------------------------------------
# Make the executable
#-------------------------------------------------------------------------------

all: $(XTRGT)

# link option -static unusable in Mac OS
$(XTRGT): $(SRCS) $(OBJS)
	@$(CXX) $(CPPFLAGS) -o $@ $(OBJS) $(LNKFLAGS)
	strip $(XTRGT)


#-------------------------------------------------------------------------------
# Dependencies
#-------------------------------------------------------------------------------

deps:
	@rm -f Makefile.dependencies
	@touch Makefile.dependencies
	for f in $(SRCS); do \
		$(CXX) $(CPPFLAGS) -MM $$f >> Makefile.dependencies; \
	done


#-------------------------------------------------------------------------------
# Cleanup
#-------------------------------------------------------------------------------

clean:
	@rm -f $(OBJS) $(XTRGT)


#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------

%.o:     %.cc
	@echo Compiling: $<
	@$(CXX) $(CPPFLAGS) -c -o $@ $<

%.o:     %.c
	@echo Compiling: $<
	@$(CC) $(CPPFLAGS) -c -o $@ $<

%.tab.c: %.y
	@echo "Updating yacc..."
	bison -p $(*F) -d -o $(*F).tab.c $<

lex.%.c: %.l
	@echo "Updating lex..."
	flex -P$(*F) -olex.$(*F).c $<


#-------------------------------------------------------------------------------
# Include dependencies
#-------------------------------------------------------------------------------

-include Makefile.dependencies

#-------------------------------------------------------------------------------

### Makefile ends here
