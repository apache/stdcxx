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
# TODO: Fix 25.search then move it from check_PROGRAMS to TESTS.
check_PROGRAMS = \
	$(TESTS) \
	25.search

# Test programs to execute for the 'check' target.
TESTS = \
	25.adjacent.find \
	25.binary.search \
	25.copy \
	25.equal \
	25.equal.range \
	25.fill \
	25.find \
	25.find.end \
	25.find.first \
	25.for.each \
	25.generate \
	25.heap \
	25.includes \
	25.lex.comparison \
	25.libc \
	25.lower.bound \
	25.merge \
	25.min.max \
	25.mismatch \
	25.nth.element \
	25.partial.sort \
	25.partitions \
	25.permutation \
	25.random.shuffle \
	25.remove \
	25.replace \
	25.reverse \
	25.rotate \
	25.set.difference \
	25.set.intersection \
	25.set.sym.difference \
	25.set.union \
	25.sort \
	25.swap \
	25.transform \
	25.unique \
	25.upper.bound

25_adjacent_find_SOURCES = \
	25.adjacent.find.cpp
25_binary_search_SOURCES = \
	25.binary.search.cpp
25_copy_SOURCES = \
	25.copy.cpp
25_equal_SOURCES = \
	25.equal.cpp
25_equal_range_SOURCES = \
	25.equal.range.cpp
25_fill_SOURCES = \
	25.fill.cpp
25_find_SOURCES = \
	25.find.cpp
25_find_end_SOURCES = \
	25.find.end.cpp
25_find_first_SOURCES = \
	25.find.first.cpp
25_for_each_SOURCES = \
	25.for.each.cpp
25_generate_SOURCES = \
	25.generate.cpp
25_heap_SOURCES = \
	25.heap.cpp
25_includes_SOURCES = \
	25.includes.cpp
25_lex_comparison_SOURCES = \
	25.lex.comparison.cpp
25_libc_SOURCES = \
	25.libc.cpp
25_lower_bound_SOURCES = \
	25.lower.bound.cpp
25_merge_SOURCES = \
	25.merge.cpp
25_min_max_SOURCES = \
	25.min.max.cpp
25_mismatch_SOURCES = \
	25.mismatch.cpp
25_nth_element_SOURCES = \
	25.nth.element.cpp
25_partial_sort_SOURCES = \
	25.partial.sort.cpp
25_partitions_SOURCES = \
	25.partitions.cpp
25_permutation_SOURCES = \
	25.permutation.cpp
25_random_shuffle_SOURCES = \
	25.random.shuffle.cpp
25_remove_SOURCES = \
	25.remove.cpp
25_replace_SOURCES = \
	25.replace.cpp
25_reverse_SOURCES = \
	25.reverse.cpp
25_rotate_SOURCES = \
	25.rotate.cpp
25_search_SOURCES = \
	25.search.cpp
25_set_difference_SOURCES = \
	25.set.difference.cpp
25_set_intersection_SOURCES = \
	25.set.intersection.cpp
25_set_sym_difference_SOURCES = \
	25.set.sym.difference.cpp
25_set_union_SOURCES = \
	25.set.union.cpp
25_sort_SOURCES = \
	25.sort.cpp
25_swap_SOURCES = \
	25.swap.cpp
25_transform_SOURCES = \
	25.transform.cpp
25_unique_SOURCES = \
	25.unique.cpp
25_upper_bound_SOURCES = \
	25.upper.bound.cpp

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

