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
# TODO: Fix these two test programs then move them to TESTS.
check_PROGRAMS = \
	$(TESTS) \
	20.specialized.stdcxx-390 \
	23_vector_bool_stdcxx_235

# Test programs to execute for the 'check' target.
TESTS = \
	18.limits.stdcxx-436 \
	21.string.replace.stdcxx-175 \
	21.string.stdcxx-162 \
	21.string.stdcxx-466 \
	22.locale.cons.stdcxx-485 \
	22.locale.messages.stdcxx-542 \
	22.locale.stdcxx-554 \
	23.associative.stdcxx-16 \
	23.bitset.cons.stdcxx-297 \
	23.deque.special.stdcxx-127 \
	23.list.cons.stdcxx-268 \
	23.list.insert.stdcxx-331 \
	23.list.special.stdcxx-334 \
	24.operations.stdcxx-234 \
	26.valarray.binary.stdcxx-237 \
	26.valarray.sub.stdcxx-309 \
	26.valarray.sub.stdcxx-448 \
	26.valarray.transcend.stdcxx-315 \
	27.filebuf.virtuals.stdcxx-522 \
	27.stringbuf.members.stdcxx-427 \
	27.stringbuf.str.stdcxx-514 \
	27.stringbuf.xsputn.stdcxx-515

18_limits_stdcxx_436_SOURCES = \
	18.limits.stdcxx-436.cpp
20_specialized_stdcxx_390_SOURCES = \
	20.specialized.stdcxx-390.cpp
21_string_replace_stdcxx_175_SOURCES = \
	21.string.replace.stdcxx-175.cpp
21_string_stdcxx_162_SOURCES = \
	21.string.stdcxx-162.cpp
21_string_stdcxx_466_SOURCES = \
	21.string.stdcxx-466.cpp
22_locale_cons_stdcxx_485_SOURCES = \
	22.locale.cons.stdcxx-485.cpp
22_locale_messages_stdcxx_542_SOURCES = \
	22.locale.messages.stdcxx-542.cpp
22_locale_stdcxx_554_SOURCES = \
	22.locale.stdcxx-554.cpp
23_associative_stdcxx_16_SOURCES = \
	23.associative.stdcxx-16.cpp
23_bitset_cons_stdcxx_297_SOURCES = \
	23.bitset.cons.stdcxx-297.cpp
23_deque_special_stdcxx_127_SOURCES = \
	23.deque.special.stdcxx-127.cpp
23_list_cons_stdcxx_268_SOURCES = \
	23.list.cons.stdcxx-268.cpp
23_list_insert_stdcxx_331_SOURCES = \
	23.list.insert.stdcxx-331.cpp
23_list_special_stdcxx_334_SOURCES = \
	23.list.special.stdcxx-334.cpp
23_vector_bool_stdcxx_235_SOURCES = \
	23.vector.bool.stdcxx-235.cpp
24_operations_stdcxx_234_SOURCES = \
	24.operations.stdcxx-234.cpp
26_valarray_binary_stdcxx_237_SOURCES = \
	26.valarray.binary.stdcxx-237.cpp
26_valarray_sub_stdcxx_309_SOURCES = \
	26.valarray.sub.stdcxx-309.cpp
26_valarray_sub_stdcxx_448_SOURCES = \
	26.valarray.sub.stdcxx-448.cpp
26_valarray_transcend_stdcxx_315_SOURCES = \
	26.valarray.transcend.stdcxx-315.cpp
27_filebuf_virtuals_stdcxx_522_SOURCES = \
	27.filebuf.virtuals.stdcxx-522.cpp
27_stringbuf_members_stdcxx_427_SOURCES = \
	27.stringbuf.members.stdcxx-427.cpp
27_stringbuf_str_stdcxx_514_SOURCES = \
	27.stringbuf.str.stdcxx-514.cpp
27_stringbuf_xsputn_stdcxx_515_SOURCES = \
	27.stringbuf.xsputn.stdcxx-515.cpp

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
	file

MAINTAINERCLEANFILES = \
        Makefile.in

