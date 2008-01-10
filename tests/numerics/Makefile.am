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
	26.accumulate \
	26.adjacent.diff \
	26.c.math \
	26.class.gslice \
	26.indirect.array \
	26.inner.product \
	26.mask.array \
	26.partial.sum \
	26.valarray.cassign \
	26.valarray.cons

26_accumulate_SOURCES = \
	26.accumulate.cpp
26_adjacent_diff_SOURCES = \
	26.adjacent.diff.cpp
26_c_math_SOURCES = \
	26.c.math.cpp
26_class_gslice_SOURCES = \
	26.class.gslice.cpp
26_indirect_array_SOURCES = \
	26.indirect.array.cpp
26_inner_product_SOURCES = \
	26.inner.product.cpp
26_mask_array_SOURCES = \
	26.mask.array.cpp
26_partial_sum_SOURCES = \
	26.partial.sum.cpp
26_valarray_cassign_SOURCES = \
	26.valarray.cassign.cpp
26_valarray_cons_SOURCES = \
	26.valarray.cons.cpp

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

