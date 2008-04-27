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
 
# RW_TYPE_INT(BITS, MACRO)
# ------------------------
# Just like the internal Autoconf _AC_TYPE_INT macro except that this
# version defines MACRO instead of `int{BITS}_t'.
AC_DEFUN([RW_TYPE_INT],
[
  AC_CACHE_CHECK([for int$1_t], [ac_cv_c_int$1_t],
    [ac_cv_c_int$1_t=no
     for ac_type in 'int$1_t' 'int' 'long int' \
         'long long int' 'short int' 'signed char'; do
       AC_COMPILE_IFELSE(
         [AC_LANG_BOOL_COMPILE_TRY(
            [AC_INCLUDES_DEFAULT],
            [[0 < ($ac_type) (((($ac_type) 1 << ($1 - 2)) - 1) * 2 + 1)]])],
         [AC_COMPILE_IFELSE(
            [AC_LANG_BOOL_COMPILE_TRY(
               [AC_INCLUDES_DEFAULT],
               [[($ac_type) (((($ac_type) 1 << ($1 - 2)) - 1) * 2 + 1)
                 < ($ac_type) (((($ac_type) 1 << ($1 - 2)) - 1) * 2 + 2)]])],
            [],
            [AS_CASE([$ac_type], [int$1_t],
               [ac_cv_c_int$1_t=yes],
               [ac_cv_c_int$1_t=$ac_type])])])
       test "$ac_cv_c_int$1_t" != no && break
     done])
  case $ac_cv_c_int$1_t in #(
  no|yes) ;; #(
  *)
    AC_DEFINE_UNQUOTED([$2], [$ac_cv_c_int$1_t],
      [Define to the type of a signed integer type of width exactly $1 bits
       if such a type exists and the standard includes do not define it.]);;
  esac
])# RW_TYPE_INT

# RW_TYPE_UINT(BITS, MACRO)
# -------------------------
# Just like the internal Autoconf _AC_TYPE_UNSIGNED_INT macro except
# that this version defines MACRO instead of `intBITS_t'.
AC_DEFUN([RW_TYPE_UINT],
[
  AC_CACHE_CHECK([for uint$1_t], [ac_cv_c_uint$1_t],
    [ac_cv_c_uint$1_t=no
     for ac_type in 'uint$1_t' 'unsigned int' 'unsigned long int' \
         'unsigned long long int' 'unsigned short int' 'unsigned char'; do
       AC_COMPILE_IFELSE(
         [AC_LANG_BOOL_COMPILE_TRY(
            [AC_INCLUDES_DEFAULT],
            [[($ac_type) -1 >> ($1 - 1) == 1]])],
         [AS_CASE([$ac_type], [uint$1_t],
            [ac_cv_c_uint$1_t=yes],
            [ac_cv_c_uint$1_t=$ac_type])])
       test "$ac_cv_c_uint$1_t" != no && break
     done])
  case $ac_cv_c_uint$1_t in #(
  no|yes) ;; #(
  *)
    AC_DEFINE_UNQUOTED([$2], [$ac_cv_c_uint$1_t],
      [Define to the type of an unsigned integer type of width exactly $1 bits
       if such a type exists and the standard includes do not define it.]);;
  esac
])# RW_TYPE_UINT

# RW_STDC_STDINT
# --------------
# Performs all standard C library config checks for <stdint.h> header.
AC_DEFUN([RW_STDC_STDINT],
[
  AC_CHECK_HEADER([stdint.h], [
      m4_foreach_w([rw_bits], [8 16 32 64], [
        # These two are not required by the standard so the best fit
        # is actually calculated.
        RW_TYPE_INT(rw_bits, [_RWSTD_INT]rw_bits[_T])
        RW_TYPE_UINT(rw_bits, [_RWSTD_UINT]rw_bits[_T])

        # These two are required by the standard.  The actual types
        # are just deduced from the respective type definition.
        AC_CHECK_TYPEOF([int_least]rw_bits[_t],
                        [_RWSTD_INT_LEAST]rw_bits[_T],
                        [@%:@include <stdint.h>])
        AC_CHECK_TYPEOF([uint_least]rw_bits[_t],
                        [_RWSTD_UINT_LEAST]rw_bits[_T],
                        [@%:@include <stdint.h>])
      ])], [])
])# RW_STDC_STDINT

