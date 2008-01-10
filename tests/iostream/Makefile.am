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
# TODO: Fix these two test programs then move to TESTS.
check_PROGRAMS = \
	$(TESTS) \
	27.objects \
	27.ostream

# Test programs to execute for the 'check' target.
TESTS = \
	27.filebuf.codecvt \
	27.forward \
	27.istream.fmat.arith \
	27.istream.manip \
	27.istream.readsome \
	27.istream.sentry \
	27.istream.unformatted.get \
	27.stringbuf.virtuals

27_filebuf_codecvt_SOURCES = \
	27.filebuf.codecvt.cpp
27_forward_SOURCES = \
	27.forward.cpp
27_istream_fmat_arith_SOURCES = \
	27.istream.fmat.arith.cpp
27_istream_manip_SOURCES = \
	27.istream.manip.cpp
27_istream_readsome_SOURCES = \
	27.istream.readsome.cpp
27_istream_sentry_SOURCES = \
	27.istream.sentry.cpp
27_istream_unformatted_get_SOURCES = \
	27.istream.unformatted.get.cpp
27_objects_SOURCES = \
	27.objects.cpp
27_ostream_SOURCES = \
	27.ostream.cpp
27_stringbuf_virtuals_SOURCES = \
	27.stringbuf.virtuals.cpp

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

