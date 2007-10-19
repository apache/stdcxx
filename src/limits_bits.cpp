/***************************************************************************
 *
 * limits_bits.cpp - source for the C++ Standard Library limits constants
 *
 * $Id$
 *
 ***************************************************************************
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
 * Copyright 1994-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#include <float.h>    // for INFINITY, NAN
#include <stdlib.h>   // for atof()

// must not #include <limits> to avoid colliding with symbols
// declared there with a different type than what they are
// defined with here
// #include <limits>
                       
#include <rw/_defs.h>  
                       

_RWSTD_NAMESPACE (__rw) { 

#ifndef _RWSTD_NO_INFINITY

#  if !defined (_RWSTD_DBL_INF_BITS)
#    error one of _RWSTD_NO_INFINITY and _RWSTD_DBL_INF_BITS must be #defined
#  endif


union _FltBits {
    char  _C_bits [sizeof (float)];
    float _C_val;
};


union _DblBits {
    char   _C_bits [sizeof (double)];
    double _C_val;
};


#  ifndef _RWSTD_NO_LONG_DOUBLE

union _LDblBits {
    char        _C_bits [sizeof (long double)];
    long double _C_val;
};

#  endif   // _RWSTD_NO_LONG_DOUBLE

#  ifndef _RWSTD_NO_OBJECT_MANGLING

extern "C" {

#  else   // if defined (_RWSTD_NO_OBJECT_MANGLING)

extern "C++" {

#  endif   // _RWSTD_NO_OBJECT_MANGLING

// The constants below are declared in <limits> but with a different
// type. C linkage is (conditionally) used to defeat MSVC and other
// "clever" compilers that mangle the type of objects into their names.

// infinity computed at config time
_RWSTD_EXPORT extern const _DblBits
__rw_dbl_infinity = { _RWSTD_DBL_INF_BITS };


_RWSTD_EXPORT extern const _FltBits
__rw_flt_infinity = { _RWSTD_FLT_INF_BITS };


#  ifndef _RWSTD_NO_LONG_DOUBLE

_RWSTD_EXPORT extern const _LDblBits
__rw_ldbl_infinity = { _RWSTD_LDBL_INF_BITS };


#  endif   // _RWSTD_NO_LONG_DOUBLE


// quiet NaN computed at config time
_RWSTD_EXPORT extern const _DblBits
__rw_dbl_qNaN = { _RWSTD_DBL_QNAN_BITS };


_RWSTD_EXPORT extern const _FltBits
__rw_flt_qNaN = { _RWSTD_FLT_QNAN_BITS };


#  ifndef _RWSTD_NO_LONG_DOUBLE

_RWSTD_EXPORT extern const _LDblBits
__rw_ldbl_qNaN = { _RWSTD_LDBL_QNAN_BITS };


#  endif   // _RWSTD_NO_LONG_DOUBLE


// signaling NaN computed at config time
_RWSTD_EXPORT extern const _DblBits
__rw_dbl_sNaN = { _RWSTD_DBL_SNAN_BITS };


_RWSTD_EXPORT extern const _FltBits
__rw_flt_sNaN = { _RWSTD_FLT_SNAN_BITS };


#  ifndef _RWSTD_NO_LONG_DOUBLE

_RWSTD_EXPORT extern const _LDblBits
__rw_ldbl_sNaN = { _RWSTD_LDBL_SNAN_BITS };


#  endif   // _RWSTD_NO_LONG_DOUBLE


// denormalized minima computed at config time
_RWSTD_EXPORT extern const _DblBits
__rw_dbl_denorm_min = { _RWSTD_DBL_DENORM_MIN_BITS };


_RWSTD_EXPORT extern const _FltBits
__rw_flt_denorm_min = { _RWSTD_FLT_DENORM_MIN_BITS };


#  ifndef _RWSTD_NO_LONG_DOUBLE

_RWSTD_EXPORT extern const _LDblBits
__rw_ldbl_denorm_min = { _RWSTD_LDBL_DENORM_MIN_BITS };


#  endif   // _RWSTD_NO_LONG_DOUBLE

}   // extern "C"/"C++"


#  if _RWSTD_VER_MAJOR < 5 && defined (_MSC_VER) && defined (_DLL)

// The constants below are defined with C++ linkage
// to be exported as floating constants with mangled
// names for binary compatibility with stdcxx 4.1.x

extern const _FltBits
__rw_flt_infinity_vc = { _RWSTD_FLT_INF_BITS };

extern const _FltBits
__rw_flt_qNaN_vc = { _RWSTD_FLT_QNAN_BITS };

extern const _FltBits
__rw_flt_sNaN_vc = { _RWSTD_FLT_SNAN_BITS };

extern const _FltBits
__rw_flt_denorm_min_vc = { _RWSTD_FLT_DENORM_MIN_BITS };


extern const _DblBits
__rw_dbl_infinity_vc = { _RWSTD_DBL_INF_BITS };

extern const _DblBits
__rw_dbl_qNaN_vc = { _RWSTD_DBL_QNAN_BITS };

extern const _DblBits
__rw_dbl_sNaN_vc = { _RWSTD_DBL_SNAN_BITS };

extern const _DblBits
__rw_dbl_denorm_min_vc = { _RWSTD_DBL_DENORM_MIN_BITS };


#    ifndef _RWSTD_NO_LONG_DOUBLE

extern const _LDblBits
__rw_ldbl_infinity_vc = { _RWSTD_LDBL_INF_BITS };

extern const _LDblBits
__rw_ldbl_qNaN_vc = { _RWSTD_LDBL_QNAN_BITS };

extern const _LDblBits
__rw_ldbl_sNaN_vc = { _RWSTD_LDBL_SNAN_BITS };

extern const _LDblBits
__rw_ldbl_denorm_min_vc = { _RWSTD_LDBL_DENORM_MIN_BITS };

#    endif   // _RWSTD_NO_LONG_DOUBLE


#    pragma comment (linker, "/EXPORT:?__rw_flt_infinity@__rw@@3MB=?__rw_flt_infinity_vc@__rw@@3T_FltBits@1@B,DATA")
#    pragma comment (linker, "/EXPORT:?__rw_flt_qNaN@__rw@@3MB=?__rw_flt_qNaN_vc@__rw@@3T_FltBits@1@B,DATA")
#    pragma comment (linker, "/EXPORT:?__rw_flt_sNaN@__rw@@3MB=?__rw_flt_sNaN_vc@__rw@@3T_FltBits@1@B,DATA")
#    pragma comment (linker, "/EXPORT:?__rw_flt_denorm_min@__rw@@3MB=?__rw_flt_denorm_min_vc@__rw@@3T_FltBits@1@B,DATA")

#    pragma comment (linker, "/EXPORT:?__rw_dbl_infinity@__rw@@3NB=?__rw_dbl_infinity_vc@__rw@@3T_DblBits@1@B,DATA")
#    pragma comment (linker, "/EXPORT:?__rw_dbl_qNaN@__rw@@3NB=?__rw_dbl_qNaN_vc@__rw@@3T_DblBits@1@B,DATA")
#    pragma comment (linker, "/EXPORT:?__rw_dbl_sNaN@__rw@@3NB=?__rw_dbl_sNaN_vc@__rw@@3T_DblBits@1@B,DATA")
#    pragma comment (linker, "/EXPORT:?__rw_dbl_denorm_min@__rw@@3NB=?__rw_dbl_denorm_min_vc@__rw@@3T_DblBits@1@B,DATA")

#    ifndef _RWSTD_NO_LONG_DOUBLE
#      pragma comment (linker, "/EXPORT:?__rw_ldbl_infinity@__rw@@3OB=?__rw_ldbl_infinity_vc@__rw@@3T_LDblBits@1@B,DATA")
#      pragma comment (linker, "/EXPORT:?__rw_ldbl_qNaN@__rw@@3OB=?__rw_ldbl_qNaN_vc@__rw@@3T_LDblBits@1@B,DATA")
#      pragma comment (linker, "/EXPORT:?__rw_ldbl_sNaN@__rw@@3OB=?__rw_ldbl_sNaN_vc@__rw@@3T_LDblBits@1@B,DATA")
#      pragma comment (linker, "/EXPORT:?__rw_ldbl_denorm_min@__rw@@3OB=?__rw_ldbl_denorm_min_vc@__rw@@3T_LDblBits@1@B,DATA")
#    endif   // _RWSTD_NO_LONG_DOUBLE

#endif   // _RWSTD_VER_MAJOR < 5 && _MSC_VER && _DLL

#else   // if defined (_RWSTD_NO_INFINITY)


#  ifndef _RWSTD_NO_OBJECT_MANGLING

extern "C" {

#  else   // if defined (_RWSTD_NO_OBJECT_MANGLING)

extern "C++" {

#  endif   // _RWSTD_NO_OBJECT_MANGLING


// as the last resort compute values at dynamic initialization time
_RWSTD_EXPORT extern const float __rw_flt_infinity =

#  if defined (INFINITY)

    INFINITY;

#  elif defined (FLT_INFINITY)

    FLT_INFINITY;

#  elif defined (__osf__)

    0.0f;   // prevent SIGFPE at startup (PR #23163)

#  else   // if !defined (__osf__)

_RWSTD_STATIC_CAST (float, atof ("INF"));

#  endif   // __osf__

_RWSTD_EXPORT extern const double __rw_dbl_infinity = __rw_flt_infinity;

#  ifndef _RWSTD_NO_LONG_DOUBLE

_RWSTD_EXPORT extern const long double __rw_ldbl_infinity = __rw_flt_infinity;

#  endif   // _RWSTD_NO_LONG_DOUBLE

_RWSTD_EXPORT extern const float __rw_flt_qNaN =

#  if defined (NAN)

    NAN;

#  elif defined (FLT_QNAN)

    FLT_QNAN;

#  elif defined (__osf__)

    0.0;   // prevent SIGFPE at startup (PR #23163)

#  else   // if !defined (__osf__)

_RWSTD_STATIC_CAST (float, atof ("NAN"));

#  endif   // __osf__

_RWSTD_EXPORT extern const float __rw_flt_sNaN =

#  if defined (FLT_SNAN)
    FLT_SNAN;
#  else   // if !defined (FLT_SNAN)
    __rw_flt_qNaN;
#  endif   // FLT_SNAN

_RWSTD_EXPORT extern const double __rw_dbl_qNaN = __rw_flt_qNaN;
_RWSTD_EXPORT extern const double __rw_dbl_sNaN = __rw_flt_sNaN;

_RWSTD_EXPORT extern const float __rw_flt_denorm_min = 0;
_RWSTD_EXPORT extern const float __rw_dbl_denorm_min = 0;

#  ifndef _RWSTD_NO_LONG_DOUBLE

_RWSTD_EXPORT extern const long double __rw_ldbl_qNaN = __rw_dbl_qNaN;
_RWSTD_EXPORT extern const long double __rw_ldbl_sNaN = __rw_dbl_sNaN;

_RWSTD_EXPORT extern const long double __rw_ldbl_denorm_min = 0;

#  endif   // _RWSTD_NO_LONG_DOUBLE

}   // extern "C"/"C++"

#endif   // _RWSTD_NO_INFINITY


}   // namespace __rw
