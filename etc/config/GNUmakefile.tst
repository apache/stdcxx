# -*- Makefile -*-
#
# $Id$
#
########################################################################
#
# Licensed to the Apache Software  Foundation (ASF) under one or more
# contributor  license agreements.  See  the NOTICE  file distributed
# with  this  work  for  additional information  regarding  copyright
# ownership.   The ASF  licenses this  file to  you under  the Apache
# License, Version  2.0 (the  "License"); you may  not use  this file
# except in  compliance with the License.   You may obtain  a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the  License is distributed on an  "AS IS" BASIS,
# WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
# implied.   See  the License  for  the  specific language  governing
# permissions and limitations under the License.
#
# Copyright 1999-2007 Rogue Wave Software, Inc.
#
########################################################################
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
#   found in $(TOPDIR)/tests, and only the necessary .d files are included.
#
########################################################################

include ../makefile.in

# tests & rwtest library directories
TESTDIR   = $(TOPDIR)/tests

# get the test suite subdirectories minus those known
# not to contain any source (i.e., .cpp) files
SRCDIRS := $(filter-out $(TESTDIR)/docs    \
                        $(TESTDIR)/etc     \
                        $(TESTDIR)/include \
                        $(TESTDIR)/src     \
                        %.C %.c %.cc %.cpp \
                        %.h %.hpp,         \
                        $(wildcard $(TESTDIR)/*))

SRCDIRNAMES := $(notdir $(SRCDIRS))

# do not compile these sources
OMIT_SRCS += $(notdir $(wildcard $(TESTDIR)/src/*.cpp)) 22_locale.cpp

# override setting from makefile.in (tests only)
CATFILE = rwstdmessages.cat

include ../makefile.common

# RW test library 
RWTLIBBASE   = rwtest$(BUILDTYPE)
RWTLIBNAME   = lib$(RWTLIBBASE).a

# Add to include dirs and link flags:
INCLUDES    += -I$(TESTDIR)/include
LDFLAGS     := -L$(BUILDDIR)/rwtest -l$(RWTLIBBASE) $(LDFLAGS)

# targets to be built: object files for sources in the source directories
TARGET := $(patsubst %.cpp,%,$(SRCS))
# add to targets objects for any sources in the current working directory
TARGET += $(patsubst %.cpp,%.o,$(wildcard *.cpp))

ifneq ($(RPATH),)
  ifneq ($(shell uname),Darwin)
    # this form doesn't work on Darwin
    LDFLAGS += $(RPATH)$(LIBDIR):$(BUILDDIR)/rwtest
  else
    # but this form does
    LDFLAGS += $(RPATH)$(LIBDIR) $(RPATH)$(BUILDDIR)/rwtest
  endif
endif

RUNFLAGS += --compat -x "--compat -O -" --ulimit=as:1073741824
# No test should use more than 1 GB of memory (See STDCXX-440).
# The magic number 1073741824 is 1 GB in bytes.

########################################################################
#  TARGETS
########################################################################

all: $(TARGET)

$(TARGET): $(LIBDIR)/$(LIBNAME) $(BUILDDIR)/rwtest/$(RWTLIBNAME)

$(LIBDIR)/$(LIBNAME):
	@$(MAKE) -C $(LIBDIR) MAKEOVERRIDES= 

$(BUILDDIR)/rwtest/$(RWTLIBNAME):
	@$(MAKE) -C $(BUILDDIR)/rwtest MAKEOVERRIDES=

$(BINDIR)/runutil:
	@$(MAKE) -C $(BINDIR) runutil

# do any directory specific cleanup using the realclean target
realclean: clean dependclean
	rm -f _*.cpp

# build all tests in the given subdirectory (subsection of the standard)
$(notdir $(SRCDIRNAMES)):
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

.PHONY: $(SRCDIRNAMES) rwtest

# Common rules for all Makefile_s
include ../makefile.rules
