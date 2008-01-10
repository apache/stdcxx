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
# Copyright 1999-2007 Rogue Wave Software, Inc.
#

# AC_ENABLE_DEBUG([DEFAULT])
# --------------------------
# Enable debug support with the --enable-debug option, or disable debug
# support with the --disable-debug option.  If debug support is enabled,
# all options related to debugging are included in commands (e.g. -g
# option for most compilers).  If debug is disabled, all such options
# are omitted from commands.
#
# DEFAULT is either `yes' or `no'.  If omitted, it defaults to `yes'.
#
AC_DEFUN([AC_ENABLE_DEBUG], [dnl
  AC_MSG_CHECKING([whether to build debug targets])

  define([AC_ENABLE_DEBUG_DEFAULT], ifelse($1, no, no, yes))dnl
  AC_ARG_ENABLE([debug],
                [AC_HELP_STRING([--enable-debug],
[build debug targets @<:@default=]AC_ENABLE_DEBUG_DEFAULT[@:>@])],
                [case $enableval in
                 yes) enable_debug=yes ;;
                 no) enable_debug=no ;;
                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
                 esac],
                [enable_debug=]AC_ENABLE_DEBUG_DEFAULT)

  # Allow Makefiles access to DEBUG_ENABLED conditional.
  AM_CONDITIONAL([DEBUG_ENABLED], [test "x$enable_debug" = "xyes"])

  case $enable_debug in
  yes)
    # TODO: Add debug compile options if not already present.
    ;;
  no)
    # TODO: Remove debug compile options if present.
    ;;
  *)
    ;;
  esac

  AC_MSG_RESULT([$enable_debug])
])# AC_ENABLE_DEBUG

