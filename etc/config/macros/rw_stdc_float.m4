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
 
# RW_STDC_FLOAT
# -------------
# Performs all standard C library config checks for <float.h> header.
AC_DEFUN([RW_STDC_FLOAT],
[
  AC_CHECK_HEADER([float.h],
                  [AC_PATH_HEADER([_RWSTD_ANSI_C_FLOAT_H], [float.h])

      AC_FOREACH([rw_Value], [FLT_ROUNDS FLT_RADIX
DBL_DIG DBL_MANT_DIG DBL_MAX_10_EXP DBL_MAX_EXP DBL_MIN_10_EXP DBL_MIN_EXP
FLT_DIG FLT_MANT_DIG FLT_MAX_10_EXP FLT_MAX_EXP FLT_MIN_10_EXP FLT_MIN_EXP
LDBL_DIG LDBL_MANT_DIG LDBL_MAX_10_EXP LDBL_MAX_EXP LDBL_MIN_10_EXP LDBL_MIN_EXP],
        [_AC_CACHE_CHECK_INT([value of rw_Value],
                            [AS_TR_SH([rw_cv_float_]rw_Value)],
                            [(long int)(]rw_Value[)],
                            [@%:@include <float.h>])
         if test $AS_TR_SH([rw_cv_float_]rw_Value) -gt 0 ; then
           AC_DEFINE_UNQUOTED([_RWSTD_]rw_Value,
                              $AS_TR_SH([rw_cv_float_]rw_Value),
[Value of ]rw_Value[ symbol defined in <float.h> header.])
         fi
        ])

      AC_FOREACH([rw_Value], [DBL_MAX FLT_MAX LDBL_MAX DBL_EPSILON
                  DBL_MIN FLT_EPSILON FLT_MIN LDBL_EPSILON LDBL_MIN],
        [_AC_CACHE_CHECK_FLOAT([value of rw_Value],
                               [AS_TR_SH([rw_cv_float_]rw_Value)],
                               [(long double)(]rw_Value[)],
                               [@%:@include <float.h>])
         if test "$AS_TR_SH([rw_cv_float_]rw_Value)" ; then
           AC_DEFINE_UNQUOTED([_RWSTD_]rw_Value,
                              $AS_TR_SH([rw_cv_float_]rw_Value),
[Value of ]rw_Value[ symbol defined in <float.h> header.])
         fi
        ])],
                  [AC_DEFINE([_RWSTD_NO_FLOAT_H], [1],
[Define to 1 if you don't have the <float.h> header file.])])
])# RW_STDC_FLOAT

