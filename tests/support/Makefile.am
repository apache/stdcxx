#
# Licensed to the Apache Software  Foundation (ASF) under one or more
# contributor  license agreements.  See  the NOTICE  file distributed
# with  this  work  for  additional information  regarding  copyright
# ownership.   The ASF  licenses this  file to  you under  the Apache
# License, Version  2.0 (the  License); you may  not use  this file
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

# Test programs to build for the 'check' target.
# TODO: Fix these two test programs (infinite loops) then move them to
#       TESTS.
check_PROGRAMS = \
	$(TESTS) \
	18.csetjmp \
	18.setjmp

# Test programs to execute for the 'check' target.
TESTS = \
	18.exception \
	18.limits.cvqual \
	18.limits.traps \
	atomic_add \
	atomic_xchg

18_csetjmp_SOURCES = \
	18.csetjmp.cpp
18_exception_SOURCES = \
	18.exception.cpp
18_limits_cvqual_SOURCES = \
	18.limits.cvqual.cpp
18_limits_traps_SOURCES = \
	18.limits.traps.cpp
18_setjmp_SOURCES = \
	18.setjmp.cpp
atomic_add_SOURCES = \
	atomic_add.cpp
atomic_xchg_SOURCES = \
	atomic_xchg.cpp

# Conditional defined by M4 AC_ENABLE_DEBUG macro.
if DEBUG_ENABLED
    DEFS += -D_RWSTDDEBUG
endif

INCLUDES = \
	-I${top_srcdir}/include \
	-I${top_srcdir}/include/ansi \
	-I${top_srcdir}/tests/include

LDADD = \
	${top_builddir}/src/lib@PACKAGE_NAME@.la \
	${top_builddir}/tests/src/librwtest.la

DEPENDENCIES = $(LDADD)

${top_builddir}/src/lib@PACKAGE_NAME@.la:
	cd $(top_builddir)/src && $(MAKE) $(AM_MAKEFLAGS) lib@PACKAGE_NAME@.la

${top_builddir}/tests/src/librwtest.la:
	cd $(top_builddir)/tests/src && $(MAKE) $(AM_MAKEFLAGS) librwtest.la

MAINTAINERCLEANFILES = \
        Makefile.in

