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
# TODO: Fix this test program then move to TESTS.
check_PROGRAMS = \
	$(TESTS) \
	0.printf

# Test programs to execute for the 'check' target.
TESTS = \
	0.alloc \
	0.char \
	0.cmdopts \
	0.ctype \
	0.inputiter \
	0.new \
	0.outputiter \
	0.process \
	0.strncmp \
	0.valcmp

0_alloc_SOURCES = \
	0.alloc.cpp
0_char_SOURCES = \
	0.char.cpp
0_cmdopts_SOURCES = \
	0.cmdopts.cpp
0_ctype_SOURCES = \
	0.ctype.cpp
0_inputiter_SOURCES = \
	0.inputiter.cpp
0_new_SOURCES = \
	0.new.cpp
0_outputiter_SOURCES = \
	0.outputiter.cpp
0_printf_SOURCES = \
	0.printf.cpp
0_process_SOURCES = \
	0.process.cpp
0_strncmp_SOURCES = \
	0.strncmp.cpp
0_valcmp_SOURCES = \
	0.valcmp.cpp

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

CLEANFILES = \
	0.out

MAINTAINERCLEANFILES = \
        Makefile.in

