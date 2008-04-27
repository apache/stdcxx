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

# AC_ENABLE_THREADS([DEFAULT])
# --------------------------
# Enable thread support with the --enable-threads option, or disable
# thread support with the --disable-threads option.  If thread support
# is enabled, all options related to thread-safety are included in
# commands (e.g. -thread or -pthread in some compilers).  If thread
# support is disabled, all such options are omitted from commands.
#
# Some platforms support more than one multithread package (e.g. Solaris
# supports both Pthreads and Solaris Threads).  DEFAULT is either `yes',
# `no', `native' (same as `yes'), `posix', `solaris', or `windows'.  If
# omitted, it defaults to `native'.
#
AC_DEFUN([AC_ENABLE_THREADS], [dnl
  AC_MSG_CHECKING([whether to build multithread targets])

  define([AC_ENABLE_THREADS_DEFAULT], ifelse($1, no, no, native))dnl
  AC_ARG_ENABLE([THREADS],
                [AC_HELP_STRING([--enable-threads@<:@=API@:>@],
[build thread targets @<:@default=]AC_ENABLE_THREADS_DEFAULT[@:>@])],
[case $enableval in
  yes) enable_threads=native ;;
  no) enable_threads=no ;;
  native) enable_threads=native ;;
  posix) enable_threads=posix ;;
  solaris) enable_threads=solaris ;;
  windows) enable_threads=windows ;;
  carbon) enable_threads=carbon ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-threads]) ;;
esac],
                [enable_threads=]AC_ENABLE_THREADS_DEFAULT)

  # TODO: Adjust compiler flags, link flags, etc. as necessary for the
  # following thread packages.
  #
  case $enable_threads in
    yes|native)
      ;;
    posix)
      ;;
    solaris)
      ;;
    windows)
      ;;
    carbon)
      ;;
    no)
      ;;
    *)
      ;;
  esac

  if test "$enable_threads" = "no" ; then
    AC_MSG_RESULT([no])
  else
    AC_MSG_RESULT([yes ($enable_threads)])
  fi
])# AC_ENABLE_THREADS

