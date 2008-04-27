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
 
# RW_STDC_STDDEF
# --------------
# Performs all standard C library config checks for <stddef.h> header.
AC_DEFUN([RW_STDC_STDDEF],
[
  AC_CHECK_HEADER([stddef.h],
  [
    AC_PATH_HEADER([_RWSTD_ANSI_C_STDDEF_H], [stddef.h])

    AC_CHECK_TYPE([ptrdiff_t],
                  [AC_CHECK_TYPEOF([ptrdiff_t], [_RWSTD_PTRDIFF_T])
                   AC_TYPE_MIN([ptrdiff_t], [_RWSTD_PTRDIFF_MIN])
                   AC_TYPE_MAX([ptrdiff_t], [_RWSTD_PTRDIFF_MAX])])

    AC_CHECK_TYPE([size_t],
                  [AC_CHECK_TYPEOF([size_t], [_RWSTD_SIZE_T])
                   AC_TYPE_MIN([size_t], [_RWSTD_SIZE_MIN])
                   AC_TYPE_MAX([size_t], [_RWSTD_SIZE_MAX])],
                  [AC_DEFINE([_RWSTD_NO_SIZE_T], [1],
[Define to 1 if standard headers do not define `size_t' type.])])

    AC_CHECK_TYPE([wchar_t],
                  [RW_CHECK_SIZEOF([wchar_t])
                   AC_TYPE_MIN([wchar_t], [_RWSTD_WCHAR_MIN])
                   AC_TYPE_MAX([wchar_t], [_RWSTD_WCHAR_MAX])],
                  [AC_DEFINE([_RWSTD_NO_WCHAR_T], [1],
[Define to 1 if standard headers do not define `wchar_t' type.])])

    AC_C89_OFFSETOF([], [AC_DEFINE([_RWSTD_NO_OFFSETOF], [1],
[Define if 1 if standard headers do not define `offsetof()' macro.])])
  ], [
    AC_DEFINE([_RWSTD_NO_STDDEF_H], [1],
[Define to 1 if you don't have the <stddef.h> header file.])
  ])
])# RW_STDC_STDDEF

