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

# AC_LANG_LIMITS_SAVE(min/max, TYPE, [INCLUDES = DEFAULT-INCLUDES])
# -----------------------------------------------------------------
# Expands to source code for a program that calculates the minimum
# or maximum value, dependending on the first argument, of the
# specified TYPE expression.
m4_define([AC_LANG_LIMITS_SAVE],
[AC_LANG_PROGRAM([$3
// volatile to foil aggressive optimizers
volatile int zero            = 0;
volatile int one             = zero + 1;
volatile int two             = one + 1;
volatile int zero_complement = ~zero;

template <class T>
T compute_max (T *pval)
{
    T min = T (-one);
    T max = T (one);

    int is_signed = T (min - one) < T (zero);
    if (!is_signed)
        max = min;

    // compute the maximum representable value
    for (; T (max * two) > max; max *= two);
    for (*pval = max / (two + two); *pval; ) {
        if (T (max + *pval) < max) {
            if (*pval > T (two))
                *pval /= two;
            else if (max < T (max + one))
                *pval = one;
            else
                break;
        }
        else
            max += *pval;
    }

    return max;
}

template <class T>
T compute_min (T *pval)
{
    T min = T (-one);
    T max = compute_max(pval);

    // compute the minimum representable value
    for (min = -max; T (min * two) < min; min *= two);

    // prevents overzealous gcc optimizer from invoking
    // undefined behavior on signed integer over/underflow
    for (T tmp; ; --min) {
        tmp = T (min - one);
        if (tmp >= min)
            break;
    }

    return min;
}

@%:@include <stdio.h>
@%:@include <stdlib.h>
], [
  FILE *f = fopen ("conftest.val", "w");
  if (! f)
    return 1;
  $2 m;
  m = compute_$1 (&m);
  fprintf (f, "%d\n", m);
  return ferror (f) || fclose (f) != 0;
])])

# AC_TYPE_MIN(TYPE, VARIABLE, [INCLUDES = DEFAULT-INCLUDES])
# -----------------------------------------------------------
# Defines VARIABLE as the minimum value of scalar TYPE expression,
# using INCLUDES if given or standard headers if omitted.
AC_DEFUN([AC_TYPE_MIN],
[
  AS_VAR_PUSHDEF([ac_Var], [ac_cv_limit_$1_min])
  AC_CACHE_CHECK([minimum value of $1], [ac_Var], [
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

    AC_RUN_IFELSE([AC_LANG_LIMITS_SAVE([min], [$1],
                                       [AC_INCLUDES_DEFAULT([$3])])],
                  [AS_VAR_SET([ac_Var], [`cat conftest.val`])],
                  [AC_MSG_FAILURE([cannot determine min. value of ($1), 77])])
    rm -f conftest.val[]dnl

    AC_LANG_RESTORE
  ])dnl

  AC_DEFINE_UNQUOTED(AS_TR_CPP([$2]), AS_VAR_GET([ac_Var]),
                     [Minimum value of type expression `$1'.])

  AS_VAR_POPDEF([ac_Var])dnl
])# AC_TYPE_MIN

# AC_TYPE_MAX(TYPE, VARIABLE, [INCLUDES = DEFAULT-INCLUDES])
# -----------------------------------------------------------
# Defines VARIABLE as the maximum value of scalar TYPE expression,
# using INCLUDES if given or standard headers if omitted.
AC_DEFUN([AC_TYPE_MAX],
[
  AS_VAR_PUSHDEF([ac_Var], [ac_cv_limit_$1_max])
  AC_CACHE_CHECK([maximum value of $1], [ac_Var], [
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

    AC_RUN_IFELSE([AC_LANG_LIMITS_SAVE([max], [$1],
                                       [AC_INCLUDES_DEFAULT([$3])])],
                  [AS_VAR_SET([ac_Var], [`cat conftest.val`])],
                  [AC_MSG_FAILURE([cannot determine max. value of ($1), 77])])
    rm -f conftest.val[]dnl

    AC_LANG_RESTORE
  ])dnl

  AC_DEFINE_UNQUOTED(AS_TR_CPP([$2]), AS_VAR_GET([ac_Var]),
                     [Maximum value of type expression `$1'.])

  AS_VAR_POPDEF([ac_Var])dnl
])# AC_TYPE_MAX

