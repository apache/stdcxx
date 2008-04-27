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
 
# RW_STDC_LOCALE
# -------------
# Performs all standard C library config checks for <locale.h> header.
AC_DEFUN([RW_STDC_LOCALE],
[
  AC_CHECK_HEADER([locale.h],
  [
    AC_PATH_HEADER([_RWSTD_ANSI_C_LOCALE_H], [locale.h])

    RW_CHECK_FUNCS([setlocale])

    # TODO: Define _RWSTD_NO_LCONV_INT_FMAT
    # TODO: Define _RWSTD_NO_LCONV

    AC_STRUCT_LCONV_DEF([_RWSTD_LCONV])
  ], [])
])# RW_STDC_LOCALE

