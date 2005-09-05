# -*- Makefile -*-
#
# $Id$
#
##############################################################################
#
# Usage:
#
#   $ make
#
#   To make all tests uder $(TOPDIR)/tests. Will generate a .d file
#   an place in the directory $(DEPENDDIR) for every source file.
#
#   $ make [ tagets ] run | runall | run_all
#
#   To make targets and run them one at a time, displaying progress
#   and test results on stdout. When no targets are specified, will
#   make and run all and create a report file in the parent directory.
#   
#   $ make run | runall | run_all [ RUN=<executables> | ALL ]
#
#   Same as above.
#
#   $ make SRCS="...sources..." [ run | runall | run_all ]
#
#   Same as above, but considerably faster since SRCS doesn't have to be
#   found in $(TOPDIR)/test, and only the necessary .d files are included.
#
##############################################################################

include ../makefile.in

# tests & test library directories
TESTDIR      = $(TOPDIR)/tests
SRCDIRS      = $(TESTDIR)

# get the test suite subdirectories
SUBDIRS      = $(shell cd $(TESTDIR) && echo *)

# do not compile these sources
OMIT_SRCS   += $(shell cd $(TESTDIR)/src && echo *.cpp) 22_locale.cpp

# override setting from makefile.in (tests only)
CATFILE      = rwstdmessages.cat

include ../makefile.common

# test library 
TESTLIBBASE   = rwtest$(BUILDTYPE)
TESTLIBNAME   = lib$(TESTLIBBASE).a

# Add to include dirs and link flags:
INCLUDES    += -I$(TESTDIR)/include
LDFLAGS     := -L$(BUILDDIR)/rwtest -l$(TESTLIBBASE) $(LDFLAGS)

# VPATH to look for sources in (appended dir for test.cpp)
VPATH       += $(TESTDIR)/src

# compile-only tests generated in the cwd
COMPILE_SRCS = $(shell echo *.cpp 2>/dev/null)
COMPILE_OBJS = $(patsubst %.cpp,%.o,$(COMPILE_SRCS))

# Targets to be built
TARGET	     = $(patsubst %.cpp,%,$(SRCS))
TARGET      += $(COMPILE_OBJS)

RUNFLAGS    += -X "-C $(CXX)-$(CCVER)" 

##############################################################################
#  TARGETS
##############################################################################

all: $(TARGET)

$(TARGET): $(LIBDIR)/$(LIBNAME) $(BUILDDIR)/rwtest/$(TESTLIBNAME)

$(LIBDIR)/$(LIBNAME):
	@$(MAKE) -C $(LIBDIR) MAKEOVERRIDES= 

$(BUILDDIR)/rwtest/$(TESTLIBNAME):
	@$(MAKE) -C $(BUILDDIR)/rwtest MAKEOVERRIDES=

# do any directory specific cleanup using the realclean target
realclean: clean dependclean
	rm -f _*.cpp

# build all tests in the given subdirectory (subsection of the standard)
$(SUBDIRS):
	$(MAKE) SRCS="`cd $(TESTDIR)/$@/ && echo *.cpp`"


# generate compile-only tests (.cpp's) from lib headers; every header must
# compile on its own (without prior inclusion of any other header)
gentest:
	@(echo "generating compile-only tests..." ;                     \
          HEADERS=`$(FIND) $(TOPDIR)/include -type f                    \
                        ! -name "*.cc" ! -name "*.c"                    \
                        ! -name "*.~*" ! -name "#*#" ! -name "*_spec.h" \
                        | sed "s:$(TOPDIR)/include/::g"`;               \
          for f in $$HEADERS ; do                                       \
              cppfile="_`basename $${f}`.cpp" ;                         \
              printf "%s " $$cppfile ;                                  \
              echo "#include <$$f>" > $$cppfile ;                       \
          done ;                                                        \
          echo ; )

.PHONY: $(SUBDIRS) rwtest

# Common rules for all Makefile_s
include ../makefile.rules
