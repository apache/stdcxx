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

# AC_C89_OFFSETOF(ACTION-IF-TRUE, ACTION-IF-FALSE)
# ----------------------------------------------
# Perform ACTION-IF-TRUE if standard headers define offsetof() macro.
# Otherwise, performs ACTION-IF-FALSE.
#
# TODO: Define offsetof() if not found as follows:
# offsetof(T, m) (((const char*)&((T*)0)->m) - ((const char*)(T*)0))
AC_DEFUN([AC_C89_OFFSETOF],
[
  AS_VAR_PUSHDEF([ac_Var], [ac_cv_c_offsetof])
  AC_CACHE_CHECK([for offsetof], [ac_Var],
    [AC_RUN_IFELSE([AC_LANG_PROGRAM([AC_INCLUDES_DEFAULT
struct S { int i[[2]][[2]]; };],
[return !(offsetof (struct S, i [[0]][[1]])
          != offsetof (struct S, i [[1]][[0]]));])],
                   [AS_VAR_SET([ac_Var], [yes])],
                   [AS_VAR_SET([ac_Var], [no])])])
  AS_IF([test AS_VAR_GET([ac_Var]) = yes], [$1], [$2])
  AS_VAR_POPDEF([ac_Var])
])# AC_C89_OFFSETOF

