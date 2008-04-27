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

# RW_STDC_WCHAR
# -------------
# Performs all standard C library config checks for <wchar.h> header.
AC_DEFUN([RW_STDC_WCHAR],
[
  AC_CHECK_HEADER([wchar.h],
  [
    AC_PATH_HEADER([_RWSTD_ANSI_C_WCHAR_H], [wchar.h])

    AC_FOREACH([rw_Value], [WCHAR_MIN WCHAR_MAX WEOF],
    [
      _AC_CACHE_CHECK_INT([value of rw_Value],
                          [AS_TR_SH([rw_cv_wchar_]rw_Value)],
                          [(long int)(]rw_Value[)],
                          [@%:@include <wchar.h>])
      if test $AS_TR_SH([rw_cv_wchar_]rw_Value) -gt 0 ; then
        AC_DEFINE_UNQUOTED([_RWSTD_]rw_Value,
                           $AS_TR_SH([rw_cv_wchar_]rw_Value),
[Value of ]rw_Value[ symbol defined in <wchar.h> header.])
      fi
    ])

    AC_CHECK_TYPE([mbstate_t],
                  [RW_CHECK_SIZEOF([mbstate_t], [],
                                   [@%:@include <wchar.h>])],
                  [AC_DEFINE([_RWSTD_NO_MBSTATE_T], [1],
[Define if <wchar.h> header does not define `mbstate_t' type.])],
                  [@%:@include <wchar.h>])

    AC_CHECK_TYPE([wint_t],
                  [AC_CHECK_TYPEOF([wint_t], [_RWSTD_WCINT_T],
                                   [@%:@include <wchar.h>])
                   AC_TYPE_MIN([wint_t], [_RWSTD_WINT_MIN],
                               [@%:@include <wchar.h>])
                   AC_TYPE_MAX([wint_t], [_RWSTD_WINT_MAX],
                               [@%:@include <wchar.h>])],
                  [AC_DEFINE([_RWSTD_NO_WINT_T], [1],
[Define if 1 if <wchar.h> header does not define `wint_t' type.])],
                  [@%:@include <wchar.h>])

    RW_CHECK_FUNCS([btowc fgetwc fgetws fputwc fputws fwide fwprintf \
fwscanf getwc getwchar mbrlen mbrtowc mbsinit mbsrtowcs putwc putwchar \
swprintf swscanf ungetwc vfwprintf vfwscanf vswprintf vswscanf vwprintf \
vwscanf wcrtomb wcscat wcschr wcscmp wcscoll wcscpy wcscspn wcsftime \
wcslen wcsncat wcsncmp wcsncpy wcspbrk wcsrchr wcsrtombs wcsspn wcsstr \
wcstod wcstof wcstok wcstol wcstold wcstoll wcstoul wcstoull wcsxfrm \
wctob wmemchr wmemcmp wmemcpy wmemmove wmemset wprintf wscanf])
  ], [
    AC_DEFINE([_RWSTD_NO_WCHAR_H], [1],
[Define to 1 if you don't have the <wchar.h> header file.])
  ])
])# RW_STDC_WCHAR

