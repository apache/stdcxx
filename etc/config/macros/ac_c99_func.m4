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

# AC_C99_FUNC(ACTION-IF-FOUND, ACTION-IF-NOT-FOUND)
# -------------------------------------------------
# Performs ACTION-IF-FOUND if the compiler recognizes the C99
# preprocessor symbol `__func__'.  Otherwise, performs
# ACTION-IF-NOT-FOUND.
AC_DEFUN([AC_C99_FUNC],
[
  AC_PROG_CC_C99
  AS_VAR_PUSHDEF([ac_Var], [ac_cv_c__func__])
  AC_CACHE_CHECK([for __func__], [ac_Var],
    [AC_COMPILE_IFELSE([const char* s = __func__;],
                       [AS_VAR_SET([ac_Var], [yes])],
                       [AS_VAR_SET([ac_Var], [no])])])
  AS_IF([test AS_VAR_GET([ac_Var]) = yes], [$1], [$2])
  AS_VAR_POPDEF([ac_Var])dnl
])# AC_C99_FUNC

