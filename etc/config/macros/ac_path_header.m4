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

# AC_PATH_HEADER(VARIABLE, HEADER-FILE, [INCLUDES],
#                [ACTION-IF-NOT-FOUND])
# -------------------------------------------------
# Define VARIABLE as the absolute pathname of HEADER in double quotes
# (suitable for an #include directive).  If the pathname can not be
# determined but the header is otherwise compilable, VARIABLE is defined
# as HEADER in angle brackets.  Otherwise, VARIABLE is undefined.
AC_DEFUN([AC_PATH_HEADER], [dnl
  AS_VAR_PUSHDEF([ac_Header], [ac_cv_path_header_$2])

  AC_CACHE_CHECK([pathname of $2 header], [ac_Header],
      [AC_COMPILE_IFELSE([AC_LANG_SOURCE([AC_INCLUDES_DEFAULT([$3])
@%:@include <$2>])],
                         [AS_VAR_SET([ac_Header],
[`(eval "$ac_cpp conftest.$ac_ext") 2>&5 \
| sed -n "s|[[^\"]]* \(\".*/$2\"\).*|\1|p" | head -n 1`])],
                         [AS_VAR_SET([ac_Header], [no])])])

  AS_IF([test ! AS_VAR_GET([ac_Header]) = no],
        [AC_DEFINE_UNQUOTED([$1], [AS_VAR_GET([ac_Header])],
[Define as the pathname of the <$2> header file.])])

  AS_VAR_POPDEF([ac_Header])dnl
])# AC_PATH_HEADER

