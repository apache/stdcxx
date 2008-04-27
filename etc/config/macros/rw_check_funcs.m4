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

# RW_CHECK_FUNCS(FUNC..., [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND)
# ----------------------------------------------------------------
# Just like AC_CHECK_FUNCS except this macro defines _RWSTD_NO_{FUNC}
# if not found instead of defining HAVE_{FUNC} if found.
AC_DEFUN([RW_CHECK_FUNCS],
[
  m4_foreach_w([AC_Func], [$1],
      [AH_TEMPLATE(AS_TR_CPP([_RWSTD_NO_]m4_defn([AC_Func])),
[Define to 1 if you do not have the `]m4_defn([AC_Func])[' function.])])dnl

  for ac_func in $1
  do
    AC_CHECK_FUNC($ac_func, [$2],
                 [AC_DEFINE_UNQUOTED(AS_TR_CPP([_RWSTD_NO_$ac_func])) $3])dnl
  done
])# RW_CHECK_FUNCS

