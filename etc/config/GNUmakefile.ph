# -*- Makefile -*-
##############################################################################
#
# $Id: //stdlib/dev/etc/stdlib/config/GNUmakefile.ph#10 $
#
##############################################################################

include ../makefile.in


#########################################################
#  if PHDIR isn't set, don't try to build anything
#########################################################
ifeq ($(PHDIR),)

all: .DEFAULT

listtarget:
	@echo

.DEFAULT:
	@printf "\nPHDIR not set; skipping Plum Hall test suite\n" >> $(LOGFILE)

#########################################################
#  end of makefile if PHDIR isn't set
#########################################################

else  # ifneq ($(PHDIR),)

SRCDIRS      = $(PHDIR)/conform 

# do not compile these sources...
# exclude files beginning with '_' or 0-9
OMIT_SRCS    += $(notdir $(shell $(FIND) $(SRCDIRS) \
                 -name "[_0-9]*.cpp" -print)) linkwith.cpp


include ../makefile.common

INCLUDES     += -I$(TOPDIR)/include/ansi -I$(PHDIR)/conform -I$(PHDIR)/dst.3

WARNFLAGS    += $(PHWARNFLAGS)

# add --ph to RUNFLAGS to indicate that these are plumhall tests
RUNFLAGS     += --ph -X "-C $(CXX)-$(CCVER)" 

# override the runtarget set in makefile.common
RUNTARGET   = $(shell $(MAKE) -s listtarget listsubtests | sed "s/ / .\//g")

##############################################################################
#  TARGETS  
##############################################################################

# first try to build all tests, then build subtests for
# those tests that didn't compile
all: $(LIBDIR)/$(LIBNAME) maintests subtests

$(LIBDIR)/$(LIBNAME):
	@$(MAKE) -C $(LIBDIR)

# TARGET variable contains the names of all the main tests
# (does not contain subtests)
maintests:  $(TARGET)

# call make recursively to get a list of all subtests that should be deleted
realclean: clean dependclean
	rm -f `$(MAKE) -s listsubtests`


# call make recursively to get a list of all subtests that need to be
# built because the main test didn't compile
subtests:
	@$(MAKE) -r -s `$(MAKE) -s listsubtests`                        

# print out the names of all subtests that need to be compiled
# because the corresponding main test didn't compile
listsubtests:
	@(for test in $(TARGET); do                                 \
            if [ ! -x ./$$test ]; then                              \
              $(FIND) $(PHDIR)/conform/$${test}.dir                 \
                -name "[_0-9]*.cpp" -print                          \
              | sed "s:.*/\(.*\)\.cpp:\1:" ;                        \
            fi;                                                     \
          done)

include ../makefile.rules

endif # ifeq ($(PHDIR),) 


