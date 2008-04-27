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
 
# RW_STDC_MATH
# ------------
# Performs all standard C library config checks for <math.h> header.
AC_DEFUN([RW_STDC_MATH],
[
  AC_CHECK_HEADER([math.h],
  [
    AC_PATH_HEADER([_RWSTD_ANSI_C_MATH_H], [math.h])

    RW_CHECK_FUNCS([acos acosf acosl asin asinf asinl atan atan2 \
atan2f atan2l atanf atanl ceil ceilf ceill cos cosf cosh coshf coshl \
cosl exp expf expl fabs fabsf fabsl floor floorf floorl fmod fmodf \
fmodl frexp frexpf frexpl ldexp ldexpf ldexpl log log10 log10f log10l \
logf logl modf modff modfl pow powf powl sin sinf sinh sinhf sinhl \
sinl sqrt sqrtf sqrtl tan tanf tanh tanhf tanhl tanl])
  ], [
    AC_DEFINE([_RWSTD_NO_MATH_H], [1],
[Define to 1 if the <math.h> header file is not present.])
  ])
])# RW_STDC_MATH

