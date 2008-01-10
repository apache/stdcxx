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

# TODO: Build these targets only if the value of the USE_NLS variable is
# yes.

# private header files
noinst_HEADERS = \
        aliases.h \
        charmap.h \
        cmdopt.h \
        def.h \
        diagnostic.h \
        display.h \
        exec.h \
        loc_exception.h \
        localedef.h \
        memchk.h \
        output.h \
        path.h \
        scanner.h \
        target.h \
        util.h

bin_PROGRAMS = \
	locale \
	localedef

check_PROGRAMS = \
	exec \
	gencat

exec_SOURCES = \
	cmdopt.cpp \
	display.cpp \
	exec.cpp \
	output.cpp \
	runall.cpp \
	util.cpp
gencat_SOURCES = \
	gencat.cpp
locale_SOURCES = \
	locale_stub.cpp
localedef_SOURCES = \
	localedef.cpp \
	locale.cpp \
	aliases.cpp \
	charmap.cpp \
	codecvt.cpp \
	collate.cpp \
	ctype.cpp \
	def.cpp \
	diagnostic.cpp \
	memchk.cpp \
	messages.cpp \
	monetary.cpp \
	numeric.cpp \
	path.cpp \
	time.cpp \
	scanner.cpp

EXTRA_DIST = \
	codeset_aliases \
	gen_list \
	iconv.cpp

# Conditional defined by M4 AC_ENABLE_DEBUG macro.
if DEBUG_ENABLED
    DEFS += -D_RWSTDDEBUG
endif

nls_srcdir = ${top_srcdir}/etc/nls
nls_builddir = ${top_srcdir}/etc/nls

# Don't search this directory for header files since some executable
## filenames conflict with standard C++ header filenames.
DEFAULT_INCLUDES = \
	-I${top_srcdir}/include \
	-I${top_builddir}/include
INCLUDES = \
	-I${top_srcdir}/include/ansi \
	-I$(srcdir)

LDADD = \
	${top_builddir}/src/lib@PACKAGE_NAME@.la

DEPENDENCIES = $(LDADD)

${top_builddir}/src/lib@PACKAGE_NAME@.la:
	cd $(top_builddir)/src && $(MAKE) $(AM_MAKEFLAGS) lib@PACKAGE_NAME@.la

MAINTAINERCLEANFILES = \
        Makefile.in

