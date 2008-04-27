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
 
# RW_STDC_TIME
# -------------
# Performs all standard C library config checks for <time.h> header.
AC_DEFUN([RW_STDC_TIME],
[
  AC_CHECK_HEADER([time.h],
  [
    AC_PATH_HEADER([_RWSTD_ANSI_C_TIME_H], [time.h])

    AC_CHECK_TYPEOF([clock_t], [_RWSTD_CLOCK_T],
                    [@%:@include <time.h>])
    AC_CHECK_TYPEOF([time_t], [_RWSTD_TIME_T],
                    [@%:@include <time.h>])

    _AC_CACHE_CHECK_INT([value of CLOCKS_PER_SEC],
                        [AS_TR_SH([rw_cv_time_clocks_per_sec])],
                        [(long int)(CLOCKS_PER_SEC)],
                        [@%:@include <time.h>])
    if test $AS_TR_SH([rw_cv_time_clocks_per_sec]) -gt 0 ; then
      AC_DEFINE_UNQUOTED([_RWSTD_CLOCKS_PER_SEC],
                         $AS_TR_SH([rw_cv_time_clocks_per_sec]),
[Value of CLOCKS_PER_SEC symbol defined in <time.h> header.])
    fi

    AC_STRUCT_TM_DEF([_RWSTD_STRUCT_TM])
  ], [])
])# RW_STDC_TIME

