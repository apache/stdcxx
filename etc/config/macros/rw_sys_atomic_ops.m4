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

# RW_SYS_ATOMIC_OPS
# -----------------
# Defines _RWSTD_NO_ATOMIC_OPS if the target platform does not support
# atomic operations.  Also defines _RWSTD_INTERLOCKED_T as the type used
# to define the interlocked_t type (only applies to Windows platforms).
# Finally, substitutes ATOMIC_ARCH in Makefiles to assemble the
# appropriate file for the target platform.
#
AC_DEFUN([RW_SYS_ATOMIC_OPS], [dnl
  AC_MSG_CHECKING([for atomic operations])

  dnl All supported platforms have atomic operations and this macro
  dnl is therefore always undefined.
  AH_TEMPLATE([_RWSTD_NO_ATOMIC_OPS],
[Define if target architecture does not have atomic operations.])

  dnl Check interlocked_t type on Windows platforms.
  dnl AC_REQUIRE([AC_TYPE_INTERLOCKED])
  dnl TODO: Implement this macro.

  dnl Assemble apporopriate atomic operations for target platform.
  dnl TODO: Use appropriate 32-bit/64-bit version if needed.
  case $build_cpu in
    ia64) ;;
    sparc) ;;
    x86|i386|i686) ;;
    x86_64) ;;
    *) AC_MSG_ERROR([unsupported build architecture $build_cpu])
       exit ;;
  esac

  AC_MSG_RESULT([yes ($build_cpu)])
])# RW_SYS_ATOMIC_OPS

