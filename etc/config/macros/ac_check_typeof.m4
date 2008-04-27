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

# AC_LANG_TYPEOF_SAVE(C++)(PROLOGUE, EXPRESSION)
# ----------------------------------------------
# Produce a C++ program that saves the runtime evaluation of the type
# EXPRESSION into `conftest.val'.
#
# TODO: Is there a way to generalize this for pointers, cv-qualifiers,
# and function types?
#
m4_define([AC_LANG_TYPEOF_SAVE(C++)],
[AC_LANG_PROGRAM([$1
@%:@define TYPESTRING(T) static const char* typestring(T) { return #T; }
TYPESTRING(char)
TYPESTRING(unsigned char)
TYPESTRING(short int)
TYPESTRING(unsigned short int)
TYPESTRING(int)
TYPESTRING(unsigned int)
TYPESTRING(long int)
TYPESTRING(unsigned long int)
@%:@if defined HAVE_LONG_LONG_INT
  TYPESTRING(long long int)
@%:@endif
@%:@if defined HAVE_UNSIGNED_LONG_LONG_INT
  TYPESTRING(unsigned long long int)
@%:@endif
TYPESTRING(float)
TYPESTRING(double)
TYPESTRING(long double)
TYPESTRING(void*)
TYPESTRING(int*)
TYPESTRING(const void*)
TYPESTRING(const int*)
TYPESTRING(void (*)())
TYPESTRING(bool)

@%:@include <stdio.h>
@%:@include <stdlib.h>
], [
  FILE* fp = fopen ("conftest.val", "w");
  if (!fp) return 1;
  $2 x; fprintf (fp, "%s\n", typestring(x));
  return ferror (fp) || fclose (fp) != 0;
])])# AC_LANG_TYPEOF_SAVE(C++)

# AC_LANG_TYPEOF_SAVE(PROLOGUE, EXPRESSION)
# -----------------------------------------
# Produce a program that saves the runtime evaluation of the type
# EXPRESSION into `conftest.val'.
AC_DEFUN([AC_LANG_TYPEOF_SAVE],
[_AC_LANG_DISPATCH([$0], _AC_LANG, $@)])

# _AC_CHECK_TYPEOF(VARIABLE, EXPRESSION, PROLOGUE, [IF-FAILS])
# ------------------------------------------------------------
AC_DEFUN([_AC_CHECK_TYPEOF],
[
  AC_LANG_COMPILER_REQUIRE()dnl
  AC_REQUIRE([AC_TYPE_LONG_LONG_INT])
  AC_REQUIRE([AC_TYPE_UNSIGNED_LONG_LONG_INT])
  if test "$cross_compiling" = no; then
    _AC_RUN_IFELSE([AC_LANG_TYPEOF_SAVE([$3], [$2])],
                   [$1=`cat conftest.val`], [$4])
  fi
  rm -f conftest.val[]dnl
])# _AC_CHECK_TYPEOF

# AC_CHECK_TYPEOF(TYPE, VARIABLE, [INCLUDES=DEFAULT-INCLUDES])
# ------------------------------------------------------------
# Define VARIABLE as the intrinsic (i.e. built-in) type which evaluates
# to a specified type EXPRESSION.  If EXPRESSION can not be evaluated,
# VARIABLE is undefined.
AC_DEFUN([AC_CHECK_TYPEOF],
[
  AS_VAR_PUSHDEF([ac_Var], [ac_cv_typeof_$1])
  AC_CACHE_CHECK([type of $1], [ac_Var],
      [# Only know how to do this in C++ currently.
       AC_LANG_SAVE
       AC_LANG_CPLUSPLUS
       _AC_CHECK_TYPEOF([ac_Var], [$1],
           [AC_INCLUDES_DEFAULT([$3])],
           [AC_MSG_FAILURE([cannot determine type of ($1), 77])])
       AC_LANG_RESTORE
      ])dnl

  AC_DEFINE_UNQUOTED(AS_TR_CPP([$2]), AS_VAR_GET([ac_Var]),
                     [Evaluated type for type espression `$1'.])
  AS_VAR_POPDEF([ac_Var])dnl
])# AC_CHECK_TYPEOF

