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

# AC_HEADER_NO_STD
# ----------------
# Modifies the appropriate preprocessor and/or compiler flags (depending
# on the current language) so that standard header file locations are
# NOT searched by default.  For example, this macro will add the
# -nostdinc option to CPPFLAGS and CFLAGS if current language is C and
# the compiler is the GNU C compiler.
AC_DEFUN([AC_HEADER_NO_STD], [dnl
  AC_REQUIRE_CPP
  _AC_LANG_DISPATCH([$0], _AC_LANG, $@)
])# AC_HEADER_NO_STD

# TODO: AC_HEADER_NO_STD(C)

# AC_HEADER_NO_STD(C++)
# ---------------------
# Modifies CPPFLAGS and CXXFLAGS as described in AC_HEADER_NO_STD below.
m4_define([AC_HEADER_NO_STD(C++)], [dnl
  save_CPPFLAGS="$CPPFLAGS"
  save_CXXFLAGS="$CXXFLAGS"

  case $CXX in
    g++)
      nostdinc_flag="-nostdinc++"
      ;;

    # TODO: Add options for other compilers here.

    *) 
      nostdinc_flag="no"
      ;;
  esac

  if test "$nostdinc_flag" != "no" ; then
    CPPFLAGS="$CPPFLAGS $nostdinc_flag"
    AC_PREPROC_IFELSE([#include <iostream>], [nostdinc_flag=no], [])

    if test "$nostdinc_flag" != "no" ; then
      CXXFLAGS="$CXXFLAGS $nostdinc_flag"
      AC_COMPILE_IFELSE([#include <iostream>], [nostdinc_flag=no], [])
    fi
  fi

  if test "$nostdinc_flag" = "no" ; then
    CPPFLAGS="$save_CPPFLAGS"
    CXXFLAGS="$save_CXXFLAGS"
  fi
])# AC_HEADER_NO_STD(C++)

