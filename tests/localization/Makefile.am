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

# TODO: Build/run these targets only if USE_NLS is yes.

# Test programs to build for 'check' target.
# TODO: Fix these test programs and move to TESTS.
check_PROGRAMS = \
	$(TESTS) \
	22.locale.ctype \
	22.locale.ctype.is \
	22.locale.ctype.mt \
	22.locale.ctype.tolower \
	22.locale.globals.mt \
	22.locale.messages \
	22.locale.money.get.mt \
	22.locale.money.put.mt \
	22.locale.moneypunct \
	22.locale.moneypunct.mt \
	22.locale.num.get.mt \
	22.locale.num.put.mt \
	22.locale.numpunct.mt

# Test programs to execute for the 'check' target.
TESTS = \
	22.locale.codecvt.length \
	22.locale.codecvt.mt \
	22.locale.codecvt.out \
	22.locale.cons.mt \
	22.locale.ctype.narrow \
	22.locale.ctype.scan \
	22.locale.ctype.toupper \
	22.locale.messages.mt \
	22.locale.money.get \
	22.locale.money.put \
	22.locale.num.get \
	22.locale.num.put \
	22.locale.statics.mt \
	22.locale.time.get \
	22.locale.time.get.mt \
	22.locale.time.put \
	22.locale.time.put.mt

22_locale_codecvt_length_SOURCES = \
	22.locale.codecvt.length.cpp
22_locale_codecvt_mt_SOURCES = \
	22.locale.codecvt.mt.cpp
22_locale_codecvt_out_SOURCES = \
	22.locale.codecvt.out.cpp
22_locale_cons_mt_SOURCES = \
	22.locale.cons.mt.cpp
22_locale_ctype_SOURCES = \
	22.locale.ctype.cpp
22_locale_ctype_is_SOURCES = \
	22.locale.ctype.is.cpp
22_locale_ctype_mt_SOURCES = \
	22.locale.ctype.mt.cpp
22_locale_ctype_narrow_SOURCES = \
	22.locale.ctype.narrow.cpp
22_locale_ctype_scan_SOURCES = \
	22.locale.ctype.scan.cpp
22_locale_ctype_tolower_SOURCES = \
	22.locale.ctype.tolower.cpp
22_locale_ctype_toupper_SOURCES = \
	22.locale.ctype.toupper.cpp
22_locale_globals_mt_SOURCES = \
	22.locale.globals.mt.cpp
22_locale_messages_SOURCES = \
	22.locale.messages.cpp
22_locale_messages_mt_SOURCES = \
	22.locale.messages.mt.cpp
22_locale_money_get_SOURCES = \
	22.locale.money.get.cpp
22_locale_money_get_mt_SOURCES = \
	22.locale.money.get.mt.cpp
22_locale_money_put_SOURCES = \
	22.locale.money.put.cpp
22_locale_money_put_mt_SOURCES = \
	22.locale.money.put.mt.cpp
22_locale_moneypunct_SOURCES = \
	22.locale.moneypunct.cpp
22_locale_moneypunct_mt_SOURCES = \
	22.locale.moneypunct.mt.cpp
22_locale_num_get_SOURCES = \
	22.locale.num.get.cpp
22_locale_num_get_mt_SOURCES = \
	22.locale.num.get.mt.cpp
22_locale_num_put_SOURCES = \
	22.locale.num.put.cpp
22_locale_num_put_mt_SOURCES = \
	22.locale.num.put.mt.cpp
22_locale_numpunct_mt_SOURCES = \
	22.locale.numpunct.mt.cpp
22_locale_statics_mt_SOURCES = \
	22.locale.statics.mt.cpp
22_locale_time_get_SOURCES = \
	22.locale.time.get.cpp
22_locale_time_get_mt_SOURCES = \
	22.locale.time.get.mt.cpp
22_locale_time_put_SOURCES = \
	22.locale.time.put.cpp
22_locale_time_put_mt_SOURCES = \
	22.locale.time.put.mt.cpp

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

