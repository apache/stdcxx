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

# AC_LANG_FLOAT_SAVE(C)(PROLOGUE, EXPRESSION)
# -------------------------------------------
# Just like AC_LANG_INT_SAVE except the value is formatted as a
# floating-point value.
m4_define([AC_LANG_FLOAT_SAVE(C)],
[AC_LANG_PROGRAM([$1
static long double longval () { return $2; }
@%:@include <stdio.h>
@%:@include <stdlib.h>],
[
  FILE *f = fopen ("conftest.val", "w");
  if (! f)
    return 1;
  long double i = longval ();
  if (i != ($2))
    return 1;
  fprintf (f, "%Le\n", i);
  return ferror (f) || fclose (f) != 0;
])])# AC_LANG_FLOAT_SAVE(C)

# AC_LANG_FLOAT_SAVE(PROLOGUE, EXPRESSION)
# ----------------------------------------
# Produce a program that saves the runtime evaluation of the
# floating-point EXPRESSION into `conftest.val'.
AC_DEFUN([AC_LANG_FLOAT_SAVE],
[_AC_LANG_DISPATCH([$0], _AC_LANG, $@)])

# _AC_COMPUTE_FLOAT_COMPILE(EXPRESSION, VARIABLE, PROLOGUE, [IF-FAILS])
# ---------------------------------------------------------------------
# Store the value of floating-point EXPRESSION in VARIABLE.  This macro
# computes the value of a floating-point expression by repeatedly
# compiling a source file (for cross-compiling builds) with the given
# PROLOGUE and stores the result in VARIABLE.
m4_define([_AC_COMPUTE_FLOAT_COMPILE],
[
dnl TODO: Finish this if needed.  See _AC_COMPUTE_INT_SAVE.
ac_lo=no
case $ac_lo in
?*) $2=$ac_lo;;
'') $4 ;;
esac[]dnl
])# _AC_COMPUTE_FLOAT_COMPILE


# _AC_COMPUTE_FLOAT_RUN(EXPRESSION, VARIABLE, PROLOGUE, [IF-FAILS])
# -----------------------------------------------------------------
# Store the evaluation of the floating-point EXPRESSION in VARIABLE.
m4_define([_AC_COMPUTE_FLOAT_RUN],
[_AC_RUN_IFELSE([AC_LANG_FLOAT_SAVE([$3], [$1])],
                [if (echo "$2" | grep "FLT") >/dev/null 2>&1; then
                   $2="`cat conftest.val`F"
                 elif (echo "$2" | grep "LDBL") >/dev/null 2>&1; then
                   $2="`cat conftest.val`L"
                 else
                   $2=`cat conftest.val`
                 fi], [$4])])

# AC_COMPUTE_FLOAT(VARIABLE, EXPRESSION, PROLOGUE, [IF-FAILS])
# ------------------------------------------------------------
AC_DEFUN([AC_COMPUTE_FLOAT],
[AC_LANG_COMPILER_REQUIRE()dnl
if test "$cross_compiling" = yes; then
  _AC_COMPUTE_FLOAT_COMPILE([$2], [$1], [$3], [$4])
else
  _AC_COMPUTE_FLOAT_RUN([$2], [$1], [$3], [$4])
fi
rm -f conftest.val[]dnl
])# AC_COMPUTE_FLOAT

# _AC_CACHE_CHECK_FLOAT(MESSAGE, CACHE-ID, EXPRESSION,
#                       [PROLOGUE = DEFAULT-INCLUDES], [IF-FAILS])
# ----------------------------------------------------------------
AC_DEFUN([_AC_CACHE_CHECK_FLOAT],
[AC_CACHE_CHECK([$1], [$2],
   [AC_COMPUTE_FLOAT([$2], [$3], [$4], [$5])])
])# _AC_CACHE_CHECK_FLOAT
 
