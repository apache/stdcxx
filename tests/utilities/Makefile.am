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

TESTS = $(check_PROGRAMS)

check_PROGRAMS = \
	20.auto.ptr \
	20.function.objects \
	20.operators \
	20.pairs \
	20.specialized \
	20.temp.buffer \
	20.temp.buffer.mt

20_auto_ptr_SOURCES = \
	20.auto.ptr.cpp
20_function_objects_SOURCES = \
	20.function.objects.cpp
20_operators_SOURCES = \
	20.operators.cpp
20_pairs_SOURCES = \
	20.pairs.cpp
20_specialized_SOURCES = \
	20.specialized.cpp
20_temp_buffer_SOURCES = \
	20.temp.buffer.cpp
20_temp_buffer_mt_SOURCES = \
	20.temp.buffer.mt.cpp

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

