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
 
# RW_STDC_STDLIB
# -------------
# Performs all standard C library config checks for <stdlib.h> header.
AC_DEFUN([RW_STDC_STDLIB],
[
  AC_CHECK_HEADER([stdlib.h],
  [
    AC_PATH_HEADER([_RWSTD_ANSI_C_STDLIB_H], [stdlib.h])

    _AC_CACHE_CHECK_INT([value of RAND_MAX],
                        [rw_cv_stdlib_rand_max],
                        [(long int)(RAND_MAX)],
                        [@%:@include <stdlib.h>])

    if test $rw_cv_stdlib_rand_max -gt 0 ; then
      AC_DEFINE_UNQUOTED([_RWSTD_RAND_MAX],
                         $rw_cv_stdlib_rand_max,
[Value of RAND_MAX symbol defined in <stdlib.h> header.])
    fi

    RW_CHECK_FUNCS([abort abs atexit atof atoi atol atoll bsearch \
calloc div exit free getenv labs ldiv llabs lldiv malloc mblen mbstowcs \
mbtowc qsort rand realloc srand strtod strtof strtol strtold strtoll \
strtoul strtoull system wcstombs wctomb])
  ], [
    AC_DEFINE([_RWSTD_NO_STDLIB_H], [1],
[Define to 1 if you don't have the <stdlib.h> header file.])
  ])
])# RW_STDC_STDLIB

