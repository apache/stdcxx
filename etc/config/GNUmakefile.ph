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

include ../makefile.in


########################################################################
#  if PHDIR isn't set, don't try to build anything
########################################################################
ifeq ($(PHDIR),)

all: .DEFAULT

listtarget:
	@echo

.DEFAULT:
	@echo "PHDIR not set; skipping Plum Hall tests"

########################################################################
#  end of makefile if PHDIR isn't set
########################################################################

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

ifneq ($(RPATH),)
  LDFLAGS += $(RPATH)$(LIBDIR)
endif

########################################################################
#  TARGETS  
########################################################################

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


