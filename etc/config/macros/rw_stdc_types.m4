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
# Copyright 1999-2008 Rogue Wave Software, Inc.
#

# TODO: Write _RW_TR_CPP_PREPARE version of _AS_TR_CPP_PREPARE macro for
# pattern substitution in shell script and replace _AS_TR_CPP_PREPARE
# and as_tr_cpp in RW_TR_CPP macro below.

# RW_TR_CPP(EXPRESSION)
# ---------------------
# Much like AC_TR_CPP except that certain intrinsic types are converted
# into abbreviated form used in macro defines (e.g. `short' is converted
# to `SHRT', `long double' becomes `LDBL').
# TODO: An m4 guru could probably simplify the pattern substitution.
m4_defun([RW_TR_CPP],
[AS_REQUIRE([_AS_TR_CPP_PREPARE])dnl
AS_LITERAL_IF([$1],
              [m4_bpatsubst(
               m4_bpatsubst(
               m4_bpatsubst(
               m4_bpatsubst(
               m4_bpatsubst(
               m4_bpatsubst(
               m4_bpatsubst(
               m4_bpatsubst(
               m4_bpatsubst(
               m4_bpatsubst(
               m4_bpatsubst(
               m4_bpatsubst(
               m4_bpatsubst(
               m4_bpatsubst(
               m4_bpatsubst(m4_translit([[$1]],
                                        [*abcdefghijklmnopqrstuvwxyz],
                                        [PABCDEFGHIJKLMNOPQRSTUVWXYZ]),
                            [UNSIGNED CHAR], [UCHAR]),
                            [SIGNED CHAR], [SCHAR]),
                            [UNSIGNED SHORT], [USHRT]),
                            [SHORT], [SHRT]),
                            [UNSIGNED INT], [UINT]),
                            [UNSIGNED LONG LONG], [ULLONG]),
                            [UNSIGNED LONG], [ULONG]),
                            [LONG LONG], [LLONG]),
                            [FLOAT], [FLT]),
                            [LONG DOUBLE], [LDBL]),
                            [DOUBLE], [DBL]),
                            [VOID(P)()], [FUNPTR]),
                            [VOIDP], [PTR]),
                            [WCHAR_T], [WCHAR]),
                            [[^A-Z0-9_]], [_])],
              [`echo "$1" | $as_tr_cpp`])])
])# RW_TR_CPP

# RW_CHECK_SIZEOF
# ---------------
# Just like AC_CHECK_SIZEOF except that this macro defines
# _RWSTD_{TYPE}_SIZE instead of SIZEOF_{TYPE}.
AC_DEFUN([RW_CHECK_SIZEOF],
[AS_LITERAL_IF([$1], [],
	       [AC_FATAL([$0: requires literal arguments])])dnl
# The cast to long int works around a bug in the HP C Compiler
# version HP92453-01 B.11.11.23709.GP, which incorrectly rejects
# declarations like `int a3[[(sizeof (unsigned char)) >= 0]];'.
# This bug is HP SR number 8606223364.
_AC_CACHE_CHECK_INT([size of $1], [AS_TR_SH([ac_cv_sizeof_$1])],
  [(long int) (sizeof ($1))],
  [AC_INCLUDES_DEFAULT([$3])],
  [if test "$AS_TR_SH([ac_cv_type_$1])" = yes; then
     AC_MSG_FAILURE([cannot compute sizeof ($1)], 77)
   else
     AS_TR_SH([ac_cv_sizeof_$1])=0
   fi])

AC_DEFINE_UNQUOTED(RW_TR_CPP(_rwstd_$1_size), $AS_TR_SH([ac_cv_sizeof_$1]),
		   [The size of `$1', as computed by sizeof.])
])# RW_CHECK_SIZEOF

# RW_STDC_TYPES
# -------------
# Performs all config checks related to intrinisic types in standard C.
AC_DEFUN([RW_STDC_TYPES],
[
  AC_CHECK_TYPE([long long], [], [AC_DEFINE([_RWSTD_NO_LONG_LONG], [1],
  [Define if compiler does not support native `long long' type.])])
  AC_CHECK_TYPE([long double], [], [AC_DEFINE([_RWSTD_NO_LONG_DOUBLE], [1],
  [Define if compiler does not support native `long double' type.])])

  RW_CHECK_SIZEOF([char])
  RW_CHECK_SIZEOF([short])
  RW_CHECK_SIZEOF([int])
  RW_CHECK_SIZEOF([long])
  if test $ac_cv_type_long_long = yes; then
    RW_CHECK_SIZEOF([long long])
    RW_CHECK_SIZEOF([unsigned long long])
  fi

  RW_CHECK_SIZEOF([float])
  RW_CHECK_SIZEOF([double])
  RW_CHECK_SIZEOF([long double])

  RW_CHECK_SIZEOF([void*])
  RW_CHECK_SIZEOF([void(*)()])
  dnl See C++ Language section for size check of member function type.
])# RW_STDC_TYPES

