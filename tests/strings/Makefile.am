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
	21.cctype \
	21.cwchar \
	21.cwctype \
	21.string.access \
	21.string.append \
	21.string.assign \
	21.string.capacity \
	21.string.compare \
	21.string.cons \
	21.string.cons.mt \
	21.string.copy \
	21.string.erase \
	21.string.find \
	21.string.find.first.not.of \
	21.string.find.first.of \
	21.string.find.last.not.of \
	21.string.find.last.of \
	21.string.insert \
	21.string.io \
	21.string.iterators \
	21.string.op.plus \
	21.string.op.plus.equal \
	21.string.operators \
	21.string.push_back.mt \
	21.string.replace \
	21.string.rfind \
	21.string.substr \
	21.string.swap

21_cctype_SOURCES = \
	21.cctype.cpp
21_cwchar_SOURCES = \
	21.cwchar.cpp
21_cwctype_SOURCES = \
	21.cwctype.cpp
21_string_access_SOURCES = \
	21.string.access.cpp
21_string_append_SOURCES = \
	21.string.append.cpp
21_string_assign_SOURCES = \
	21.string.assign.cpp
21_string_capacity_SOURCES = \
	21.string.capacity.cpp
21_string_compare_SOURCES = \
	21.string.compare.cpp
21_string_cons_SOURCES = \
	21.string.cons.cpp
21_string_cons_mt_SOURCES = \
	21.string.cons.mt.cpp
21_string_copy_SOURCES = \
	21.string.copy.cpp
21_string_erase_SOURCES = \
	21.string.erase.cpp
21_string_find_SOURCES = \
	21.string.find.cpp
21_string_find_first_not_of_SOURCES = \
	21.string.find.first.not.of.cpp
21_string_find_first_of_SOURCES = \
	21.string.find.first.of.cpp
21_string_find_last_not_of_SOURCES = \
	21.string.find.last.not.of.cpp
21_string_find_last_of_SOURCES = \
	21.string.find.last.of.cpp
21_string_insert_SOURCES = \
	21.string.insert.cpp
21_string_io_SOURCES = \
	21.string.io.cpp
21_string_iterators_SOURCES = \
	21.string.iterators.cpp
21_string_op_plus_SOURCES = \
	21.string.op.plus.cpp
21_string_op_plus_equal_SOURCES = \
	21.string.op.plus.equal.cpp
21_string_operators_SOURCES = \
	21.string.operators.cpp
21_string_push_back_mt_SOURCES = \
	21.string.push_back.mt.cpp
21_string_replace_SOURCES = \
	21.string.replace.cpp
21_string_rfind_SOURCES = \
	21.string.rfind.cpp
21_string_substr_SOURCES = \
	21.string.substr.cpp
21_string_swap_SOURCES = \
	21.string.swap.cpp

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

