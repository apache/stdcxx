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

# information on the package
AC_PREREQ([2.62]) # multi-line definitions were added in this version
AC_COPYRIGHT([Copyright (c) 2005-2007 Apache Software Foundation.
Copyright (c) 1999-2007 Rogue Wave Software, Inc.])
AC_REVISION([$Id$])

AC_INIT([stdcxx], [4.3.0], [http://incubator.apache.org/stdcxx/bugs.html])

AC_CONFIG_AUX_DIR([etc/config/aux])
AC_SUBST([CONFIG_AUXDIR], [\$\{top_srcdir\}/etc/config/aux])
AC_CONFIG_MACRO_DIR([etc/config/macros])
AC_SUBST([CONFIG_MACRODIR], [\$\{top_srcdir\}/etc/config/macros])

AH_TOP([#include <config.old.h>])
AC_CONFIG_HEADERS([include/config.h])

AM_INIT_AUTOMAKE([dist-bzip2 dist-zip])

# custom configure options
AM_NLS
AC_ENABLE_DEBUG
AC_ENABLE_THREADS

AC_MSG_NOTICE([checks for programs])
AC_PROG_LIBTOOL
AC_PROG_CC_STDC
AC_PROG_CXX
AM_PROG_AS

AC_MSG_NOTICE([checks for system-specific features])
RW_SYS_ATOMIC_OPS

AC_MSG_NOTICE([checks for Standard C Language])
AC_C99_FUNC([], [AC_DEFINE([_RWSTD_NO_FUNC], [1],
[Define if compiler does not support C99 preprocessing symbol `__func__'.])])
RW_STDC_TYPES

AC_MSG_NOTICE([checks for Standard C Library])
AC_PATH_HEADER([_RWSTD_ANSI_C_ASSERT_H], [assert.h])
AC_PATH_HEADER([_RWSTD_ANSI_C_CTYPE_H], [ctype.h])
AC_PATH_HEADER([_RWSTD_ANSI_C_ERRNO_H], [errno.h])
AC_PATH_HEADER([_RWSTD_ANSI_C_ISO646_H], [iso646.h])
AC_PATH_HEADER([_RWSTD_ANSI_C_SETJMP_H], [setjmp.h])

RW_STDC_FLOAT
RW_STDC_LOCALE
RW_STDC_MATH
RW_STDC_SIGNAL
RW_STDC_STDARG
RW_STDC_STDIO
RW_STDC_STDDEF
RW_STDC_STDINT
RW_STDC_STDLIB
RW_STDC_STRING
RW_STDC_TIME
RW_STDC_WCHAR
RW_STDC_WCTYPE

AC_MSG_NOTICE([checks for C++ compiler features])
AC_LANG(C++)
AC_HEADER_NO_STD

# generate output files
AC_CONFIG_FILES([Makefile \
                 include/Makefile \
                 src/Makefile \
                 examples/Makefile \
                 examples/manual/Makefile \
                 examples/tutorial/Makefile \
                 tests/Makefile \
                 tests/algorithms/Makefile \
                 tests/containers/Makefile \
                 tests/diagnostics/Makefile \
                 tests/intro/Makefile \
                 tests/iostream/Makefile \
                 tests/localization/Makefile \
                 tests/numerics/Makefile \
                 tests/regress/Makefile \
                 tests/self/Makefile \
                 tests/src/Makefile \
                 tests/strings/Makefile \
                 tests/support/Makefile \
                 tests/tr1.c99/Makefile \
                 tests/tr1.util/Makefile \
                 tests/utilities/Makefile \
                 util/Makefile])
AC_OUTPUT

