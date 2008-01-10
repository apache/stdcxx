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
AC_PREREQ([2.59])
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

AM_NLS
AC_ENABLE_DEBUG
AC_ENABLE_THREADS

# checks for programs
AC_PROG_LIBTOOL
AC_PROG_CXX
AM_PROG_AS

# checks for system-specific features
RW_SYS_ATOMIC_OPS

# checks for native C language data types
AC_CHECK_TYPE([long long], [], [AC_DEFINE([_RWSTD_NO_LONG_LONG], [1],
[Define if compiler does not support native `long long' type.])])
AC_CHECK_TYPE([long double], [], [AC_DEFINE([_RWSTD_NO_LONG_DOUBLE], [1],
[Define if compiler does not support native `long double' type.])])

# checks for common C library type definitions
AC_CHECK_TYPE([size_t], [], [AC_DEFINE([_RWSTD_NO_SIZE_T], [1],
[Define if standard headers do not define `size_t' type.])])

# checks for C++ compiler-specific features
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

