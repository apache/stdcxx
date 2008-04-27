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
 
# RW_STDC_SIGNAL
# --------------
# Performs all standard C library config checks for <signal.h> header.
AC_DEFUN([RW_STDC_SIGNAL],
[
  AC_CHECK_HEADER([signal.h],
  [
    AC_PATH_HEADER([_RWSTD_ANSI_C_SIGNAL_H], [signal.h])

    AC_FOREACH([rw_Value],
        [SIG_DFL SIG_ERR SIG_IGN SIGABRT SIGFPE SIGILL SIGINT \
         SIGSEGV SIGTERM],
        [_AC_CACHE_CHECK_INT([value of rw_Value],
                             [AS_TR_SH([rw_cv_signal_]rw_Value)],
                             [(long int)(]rw_Value[)],
                             [@%:@include <signal.h>])
         if test $AS_TR_SH([rw_cv_signal_]rw_Value) -gt 0 ; then
           AC_DEFINE_UNQUOTED([_RWSTD_]rw_Value,
                              $AS_TR_SH([rw_cv_signal_]rw_Value),
[Value of ]rw_Value[ symbol defined in <signal.h> header.])
         fi
        ])

    AC_CHECK_TYPE([sig_atomic_t],
                  [AC_TYPE_MIN([sig_atomic_t],
                               [_RWSTD_SIG_ATOMIC_MIN],
                               [@%:@include <signal.h>])
                   AC_TYPE_MAX([sig_atomic_t],
                               [_RWSTD_SIG_ATOMIC_MAX],
                               [@%:@include <signal.h>])],
                  [AC_DEFINE([_RWSTD_NO_SIG_ATOMIC_T], [1],
[Define if <signal.h> header does not define `sig_atomic_t' type.])],
                  [@%:@include <signal.h>])
  ], [])
])# RW_STDC_SIGNAL

