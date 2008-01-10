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

check_LTLIBRARIES = librwtest.la

librwtest_la_SOURCES = \
	21.strings.cpp \
	23.containers.cpp \
	alarm.cpp \
	alg_test.cpp \
	alloc.cpp \
	allocator.cpp \
	any.cpp \
	char.cpp \
	cmdopt.cpp \
	ctype.cpp \
	driver.cpp \
	environ.cpp \
	exception.cpp \
	file.cpp \
	fmt_bits.cpp \
	locale.cpp \
	new.cpp \
	opt_diags.cpp \
	opt_lines.cpp \
	opt_trace.cpp \
	opt_types.cpp \
	printf.cpp \
	process.cpp \
	rand.cpp \
	thread.cpp \
	valcmp.cpp \
	value.cpp

# search paths for header files
DEFAULT_INCLUDES = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/include/ansi \
	-I$(top_srcdir)/tests/include

# Conditional defined by M4 AC_ENABLE_DEBUG macro.
if DEBUG_ENABLED
    DEFS += -D_RWSTDDEBUG
endif

# private header files
noinst_HEADERS = \
	fmt_defs.h \
	opt_diags.h \
	opt_lines.h \
	opt_trace.h \
	opt_types.h

librwtest_la_LDFLAGS = 
#librwtest_la_LDFLAGS = -version-info 0:0:0
# Read http://www.gnu.org/software/libtool/manual.html#Versioning before
# using this flag.

MAINTAINERCLEANFILES = \
        Makefile.in

