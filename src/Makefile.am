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

# private header files
noinst_HEADERS = \
	access.h \
	catalog.h \
	iosdata.h \
	iso2022.h \
	locale_body.h \
	mman.h \
	once.h \
	podarray.h \
	setlocale.h \
	strtol.h \
	use_facet.h

lib_LTLIBRARIES = libstdcxx.la

libstdcxx_la_SOURCES = \
	assert.cpp \
	atomic.s \
	bitset.cpp \
	catalog.cpp \
	codecvt.cpp \
	collate.cpp \
	ctype.cpp \
	ctype_bits.cpp \
	domain_error.cpp \
	exception.cpp \
	export.cpp \
	facet.cpp \
	file.cpp \
	instance.cpp \
	invalid_argument.cpp \
	ios.cpp \
	ios_bits.cpp \
	iostore.cpp \
	iostream.cpp \
	iso2022.cpp \
	length_error.cpp \
	limits.cpp \
	limits_bits.cpp \
	locale_bits.cpp \
	locale_body.cpp \
	locale_classic.cpp \
	locale_combine.cpp \
	locale_core.cpp \
	locale_eq.cpp \
	locale_global.cpp \
	locale_name.cpp \
	logic_error.cpp \
	memattr.cpp \
	memory.cpp \
	messages.cpp \
	mman.cpp \
	num_get.cpp \
	num_put.cpp \
	once.cpp \
	out_of_range.cpp \
	overflow_error.cpp \
	punct.cpp \
	random.cpp \
	range_error.cpp \
	runtime_error.cpp \
	rwstderr.cpp \
	setlocale.cpp \
	string.cpp \
	strstream.cpp \
	strtol.cpp \
	ti_collate.cpp \
	ti_filebuf.cpp \
	ti_insert_dbl.cpp \
	ti_insert_int.cpp \
	ti_insert_ptr.cpp \
	ti_ios.cpp \
	ti_istream.cpp \
	ti_messages.cpp \
	ti_money_get.cpp \
	ti_money_put.cpp \
	ti_moneypunct.cpp \
	ti_num_get.cpp \
	ti_num_put.cpp \
	ti_numpunct.cpp \
	ti_ostream.cpp \
	ti_podarray.cpp \
	ti_streambuf.cpp \
	ti_string.cpp \
	ti_stringbuf.cpp \
	ti_time_get.cpp \
	ti_time_put.cpp \
	ti_wcollate.cpp \
	ti_wfilebuf.cpp \
	ti_winsert_dbl.cpp \
	ti_winsert_int.cpp \
	ti_winsert_ptr.cpp \
	ti_wios.cpp \
	ti_wistream.cpp \
	ti_wmessages.cpp \
	ti_wmoney_get.cpp \
	ti_wmoney_put.cpp \
	ti_wmoneypunct.cpp \
	ti_wnum_get.cpp \
	ti_wnum_put.cpp \
	ti_wnumpunct.cpp \
	ti_wostream.cpp \
	ti_wstreambuf.cpp \
	ti_wstring.cpp \
	ti_wstringbuf.cpp \
	ti_wtime_get.cpp \
	ti_wtime_put.cpp \
	time_get.cpp \
	time_put.cpp \
	tmpbuf.cpp \
	typeinfo.cpp \
	underflow_error.cpp \
	valarray.cpp \
	vecbool.cpp \
	version.cpp \
	wcodecvt.cpp \
	wctype.cpp

# search paths for header files
INCLUDES = \
        -I$(top_srcdir)/include \
        -I$(top_srcdir)/include/ansi \
        -I$(top_builddir)/include

# Conditional defined by M4 AC_ENABLE_DEBUG macro.
if DEBUG_ENABLED
    DEFS += -D_RWSTDDEBUG
endif

libstdcxx_la_LDFLAGS = -release @PACKAGE_VERSION@
# Read http://www.gnu.org/software/libtool/manual.html#Versioning before
# using this flag.
#libstdcxx_la_LDFLAGS = -version-info 0:0:0

# TODO: Atomic files seem to be conditional sources.  Need to figure out
# how they are intended to be built.
EXTRA_DIST = \
	atomic-cxx.S \
	atomic.asm \
	i86 \
	i86_64 \
	ia64 \
	parisc \
	sparc \
        mapfile.gcc \
        mapfile.gcc-2 \
        podarray.c \
        podarray.cc \
        rwstderr.cpp \
        rwstderr.msg \
        rwstderr.rc

MAINTAINERCLEANFILES = \
        Makefile.in

