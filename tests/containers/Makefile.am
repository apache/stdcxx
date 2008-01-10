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
	23.bitset.cons \
	23.bitset \
	23.deque.iterators \
	23.deque.modifiers \
	23.deque.special \
	23.list.assign \
	23.list.capacity \
	23.list.cons \
	23.list.erase \
	23.list.insert \
	23.list.iterators \
	23.list.special \
	23.vector.allocator \
	23.vector.capacity \
	23.vector.cons \
	23.vector.modifiers

23_bitset_cons_SOURCES = \
	23.bitset.cons.cpp
23_bitset_SOURCES = \
	23.bitset.cpp
23_deque_iterators_SOURCES = \
	23.deque.iterators.cpp
23_deque_modifiers_SOURCES = \
	23.deque.modifiers.cpp
23_deque_special_SOURCES = \
	23.deque.special.cpp
23_list_assign_SOURCES = \
	23.list.assign.cpp
23_list_capacity_SOURCES = \
	23.list.capacity.cpp
23_list_cons_SOURCES = \
	23.list.cons.cpp
23_list_erase_SOURCES = \
	23.list.erase.cpp
23_list_insert_SOURCES = \
	23.list.insert.cpp
23_list_iterators_SOURCES = \
	23.list.iterators.cpp
23_list_special_SOURCES = \
	23.list.special.cpp
23_vector_allocator_SOURCES = \
	23.vector.allocator.cpp
23_vector_capacity_SOURCES = \
	23.vector.capacity.cpp
23_vector_cons_SOURCES = \
	23.vector.cons.cpp
23_vector_modifiers_SOURCES = \
	23.vector.modifiers.cpp

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

