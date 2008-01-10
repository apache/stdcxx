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

SUBDIRS = \
	examples \
	include \
	src \
	tests \
	util

# Can't include etc/config/macros here because discheck target will fail.
EXTRA_DIST = \
	GNUmakefile \
	LICENSE.txt \
	NOTICE.txt \
	configure.bat \
	doc \
	etc/config/GNUmakefile.bin \
	etc/config/GNUmakefile.cfg \
	etc/config/GNUmakefile.exm \
	etc/config/GNUmakefile.lib \
	etc/config/GNUmakefile.ph \
	etc/config/GNUmakefile.rwt \
	etc/config/GNUmakefile.tst \
	etc/config/acc.config \
	etc/config/como.config \
	etc/config/configure.sh \
	etc/config/eccp.config \
	etc/config/fixtree.sh \
	etc/config/gcc.config \
	etc/config/icc.config \
	etc/config/makefile.common \
	etc/config/makefile.rules \
	etc/config/mipspro.config \
	etc/config/osf_cxx.config \
	etc/config/reliant_cds.config \
	etc/config/run_locale_utils.sh \
	etc/config/runall.sh \
	etc/config/src \
	etc/config/sunpro.config \
	etc/config/vacpp.config \
	etc/config/windows \
	etc/config/xlC_version.sh \
	etc/nls \
	generate.bat

# So that the Makefiles will properly invoke the aclocal utility when
# updating/regenerating maintenance files.
ACLOCAL_AMFLAGS = -I @CONFIG_MACRODIR@

config_auxdir = @CONFIG_AUXDIR@

MAINTAINERCLEANFILES = \
	COPYING \
	INSTALL \
	Makefile.in \
	aclocal.m4 \
	configure \
	$(config_auxdir)/config.guess \
	$(config_auxdir)/config.sub \
	$(config_auxdir)/depcomp \
	$(config_auxdir)/install-sh \
	$(config_auxdir)/ltmain.sh \
	$(config_auxdir)/missing

