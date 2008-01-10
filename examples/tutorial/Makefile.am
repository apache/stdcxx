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

noinst_PROGRAMS = \
	alg1 \
	alg2 \
	alg3 \
	alg4 \
	alg5 \
	alg6 \
	alg7 \
	calc \
	complx \
	concord \
	dynatype \
	graph \
	icecream \
	radix \
	sieve \
	spell \
	stdexcept \
	stocks \
	tele \
	teller \
	widwork

alg1_SOURCES = alg1.cpp
alg2_SOURCES = alg2.cpp
alg3_SOURCES = alg3.cpp
alg4_SOURCES = alg4.cpp
alg5_SOURCES = alg5.cpp
alg6_SOURCES = alg6.cpp
alg7_SOURCES = alg7.cpp
calc_SOURCES = calc.cpp
complx_SOURCES = complx.cpp
concord_SOURCES = concord.cpp
dynatype_SOURCES = dynatype.cpp
graph_SOURCES = graph.cpp
icecream_SOURCES = icecream.cpp
radix_SOURCES = radix.cpp
sieve_SOURCES = sieve.cpp
spell_SOURCES = spell.cpp
stdexcept_SOURCES = stdexcept.cpp
stocks_SOURCES = stocks.cpp
tele_SOURCES = tele.cpp
teller_SOURCES = teller.cpp
widwork_SOURCES = widwork.cpp

EXTRA_DIST = \
	in \
	out

# Conditional defined by M4 AC_ENABLE_DEBUG macro.
if DEBUG_ENABLED
    DEFS += -D_RWSTDDEBUG
endif

## Don't search this directory for header files since some executable
## filenames conflict with standard C++ header filenames.
DEFAULT_INCLUDES = \
	-I${top_srcdir}/include \
	-I${top_builddir}/include
INCLUDES = \
	-I${top_srcdir}/include/ansi \
	-I${top_srcdir}/examples/include

LDADD = \
	${top_builddir}/src/lib@PACKAGE_NAME@.la

${top_builddir}/src/lib@PACKAGE_NAME@.la:
	cd $(top_builddir)/src && $(MAKE) $(AM_MAKEFLAGS) lib@PACKAGE_NAME@.la

MAINTAINERCLEANFILES = \
	Makefile.in

