// definitions of the floating point constants

/***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 * Copyright 1999-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

// this file must be included only after the config.h

#ifndef _RWSTD_NO_FLOAT_H
#  include <float.h>

#  if defined (__EDG__) \
      && !defined (__DECCXX) \
      && !defined (__HP_aCC) \
      && !defined (__INTEL_COMPILER) \
      && !defined (_SGI_COMPILER_VERSION)
     // define gcc 4 intrinsics otherwise not recognized
     // by the front end
#    define __FLT_DIG__              6
#    define __FLT_EPSILON__          1.19209290e-7F
#    define __FLT_MANT_DIG__        24
#    define __FLT_MAX__              3.40282347e+38F
#    define __FLT_MAX_10_EXP__      38
#    define __FLT_MAX_EXP__        128
#    define __FLT_MIN__              1.17549435e-38F
#    define __FLT_MIN_10_EXP__     -37
#    define __FLT_MIN_EXP__       -125
#    define __FLT_RADIX__            2

#    define __DBL_DIG__             15
#    define __DBL_EPSILON__          2.2204460492503131e-16
#    define __DBL_MANT_DIG__        53
#    define __DBL_MAX__              1.7976931348623157e+308
#    define __DBL_MAX_10_EXP__     308
#    define __DBL_MAX_EXP__       1024
#    define __DBL_MIN__              2.2250738585072014e-308
#    define __DBL_MIN_10_EXP__    -307
#    define __DBL_MIN_EXP__      -1021

#    define __LDBL_DIG__            18
#    define __LDBL_EPSILON__         1.08420217248550443401e-19L
#    define __LDBL_MANT_DIG__       64
#    define __LDBL_MAX__             1.18973149535723176502e+4932L
#    define __LDBL_MAX_10_EXP__   4932
#    define __LDBL_MAX_EXP__     16384
#    define __LDBL_MIN__             3.36210314311209350626e-4932L
#    define __LDBL_MIN_10_EXP__  -4931
#    define __LDBL_MIN_EXP__    -16381

     // Linux value
#    define _RWSTD_FLT_ROUNDS     1   /* round to nearest */

#    if defined (__GNUC__) && __GNUC__ < 4

      // prevent the propriterary gcc __extension__ from
      // throwing the vanilla EDG demo for a loop

#      undef LDBL_EPSILON
#      undef LDBL_MIN
#      undef LDBL_MAX

       // redefine to prevent compilation errors
#      define LDBL_EPSILON 1.0842021724855044e-19L
#      define LDBL_MIN     3.3621031431120935e-4932L
#      define LDBL_MAX     1.1897314953572317e+4932L
#    endif   //gcc 3 (as the back end for eccp)
#  endif   //  EDG eccp vanilla front end
#else   // if defined (_RWSTD_NO_FLOAT_H)
   // assume IEEE 754 floating point
   // FIXME: autodetect floating point format
#  define FLT_DIG               6
#  define FLT_EPSILON           1.19209290e-7F
#  define FLT_MANT_DIG         24
#  define FLT_MAX               3.40282347e+38F
#  define FLT_MAX_10_EXP       38
#  define FLT_MAX_EXP         128
#  define FLT_MIN               1.17549435e-38F
#  define FLT_MIN_10_EXP      -37
#  define FLT_MIN_EXP        -125
#  define FLT_RADIX             2

#  define DBL_DIG              15
#  define DBL_EPSILON           2.2204460492503131e-16
#  define DBL_MANT_DIG         53
#  define DBL_MAX               1.7976931348623157e+308
#  define DBL_MAX_10_EXP      308
#  define DBL_MAX_EXP        1024
#  define DBL_MIN               2.2250738585072014e-308
#  define DBL_MIN_10_EXP     -307
#  define DBL_MIN_EXP       -1021

#  define LDBL_DIG             18
#  define LDBL_EPSILON          1.08420217248550443401e-19L
#  define LDBL_MANT_DIG        64
#  define LDBL_MAX              1.18973149535723176502e+4932L
#  define LDBL_MAX_10_EXP    4932
#  define LDBL_MAX_EXP      16384
#  define LDBL_MIN              3.36210314311209350626e-4932L
#  define LDBL_MIN_10_EXP   -4931
#  define LDBL_MIN_EXP     -16381

   // Linux value
#  define _RWSTD_FLT_ROUNDS     1   /* round to nearest */
#endif   // _RWSTD_NO_FLOAT_H
