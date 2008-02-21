// computing floating point properties

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
 * Copyright 1999-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

// working around a Compaq C++ headers problem (PR #27459)
#if defined (__PURE_CNAME)
#  undef __PURE_CNAME
#endif // __PURE_CNAME

#include "config.h"

enum {
    // the enumerators below are expected to be hidden by macros #defined
    // in the <float.h> header #included below through "float_defs.h";
    // references to macros that are not #defined in the header will
    // silently use these enumerators and prevent compilation errors that
    // would result otherwise
    DBL_DIG         =    15,   // default
    DBL_MANT_DIG    =    53,
    DBL_MAX_10_EXP  =   308,
    DBL_MAX_EXP     =  1024,
    DBL_MIN_10_EXP  =  -307,
    DBL_MIN_EXP     = -1021,

    FLT_DIG         =     6,
    FLT_MANT_DIG    =    24,
    FLT_MAX_10_EXP  =    38,
    FLT_MAX_EXP     =   128,
    FLT_MIN_10_EXP  =   -37,
    FLT_MIN_EXP     =  -125,
    FLT_RADIX       =     2,

    LDBL_DIG        = -1,
    LDBL_MANT_DIG   = -1,
    LDBL_MAX_10_EXP = -1,
    LDBL_MAX_EXP    = -1,
    LDBL_MIN_10_EXP = -1,
    LDBL_MIN_EXP    = -1
};


#include <errno.h>    // for ERANGE, errno
#include <stdio.h>    // for printf()
#include <stdlib.h>   // for strtod()

#include "float_defs.h"   // for FLT_XXX, DBL_XXX, LDBL_XXX constants

#ifndef _RWSTD_NO_LIBC_EXCEPTION_SPEC
#  define LIBC_THROWS()   throw ()
#else
#  define LIBC_THROWS()   /* empty */
#endif   // _RWSTD_NO_LIBC_EXCEPTION_SPEC


extern "C" {

#ifdef _RWSTD_NO_STRTOF
#  ifndef _RWSTD_NO_STRTOF_IN_LIBC

#    undef _RWSTD_NO_STRTOF

float strtof (const char*, char**) LIBC_THROWS ();

#  endif   // _RWSTD_NO_STRTOF_IN_LIBC
#endif   // _RWSTD_NO_STRTOF

#ifdef _RWSTD_NO_STRTOD
#  ifndef _RWSTD_NO_STRTOD_IN_LIBC

#    undef _RWSTD_NO_STRTOD

double strtod (const char*, char**) LIBC_THROWS ();

#  endif   // _RWSTD_NO_STRTOD_IN_LIBC
#endif   // _RWSTD_NO_STRTOD

#ifndef _RWSTD_NO_LONG_DOUBLE
#  ifdef _RWSTD_NO_STRTOLD
#    ifndef _RWSTD_NO_STRTOLD_IN_LIBC

#      undef _RWSTD_NO_STRTOLD

long double strtold (const char*, char**) LIBC_THROWS ();

#    endif   // _RWSTD_NO_STRTOLD_IN_LIBC
#  endif   // _RWSTD_NO_STRTOLD
#endif   // _RWSTD_NO_LONG_DOUBLE

}


#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE
#    include "terminate.h"
#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


// print a floating point number, either as a string (if the stringized
// constant is a valid number, and not some complex expression), or as
// a formatted floating point value
template <class FloatT>
void print_float (FloatT x, const char *xstr,
                  const char *macro_name, const char *fmt, int prec)
{
    if ('-' == *xstr || '0' <= *xstr && '9' >= *xstr) {
        printf ("#define _RWSTD_%-16s %s", macro_name, xstr);
    }
    else {
        printf ("#define _RWSTD_%-16s ", macro_name);
        printf (fmt, prec + 2, x);
    }
    puts ("");
}

#define DO_STRINGIZE(x)  #x
#define STRINGIZE(x)     DO_STRINGIZE (x)

#define PRINTINT(macro)                                         \
    if (-1 == (macro))                                          \
        printf ("%s", "//");                                    \
    printf ("#define _RWSTD_%-16s %6d\n", #macro, (macro))

#define PRINTFLT(macro, fmt, prec, suffix) \
    print_float (macro, STRINGIZE (macro), #macro, "%.*" fmt "e" suffix, prec)


int main ()
{
    //////////////////////////////////////////////////////////////////
    // compute floating point limits

#undef LDBL_FMT

#ifdef _RWSTD_LDBL_PRINTF_PREFIX
#  define LDBL_FMT   _RWSTD_LDBL_PRINTF_PREFIX
#else
#  define LDBL_FMT   "L"
#endif

#if defined (FLT_ROUNDS)
    printf ("#define _RWSTD_%-16s %6d   /* %s */\n",
            "FLT_ROUNDS", FLT_ROUNDS,
               0 == FLT_ROUNDS ? "round toward zero"
            :  1 == FLT_ROUNDS ? "round to nearest"
            :  2 == FLT_ROUNDS ? "round toward infinity"
            :  3 == FLT_ROUNDS ? "round toward negative infinity"
            :  "indeterminable");
#endif   // FLT_ROUNDS

    PRINTINT (DBL_DIG);
    PRINTINT (DBL_MANT_DIG);
    PRINTINT (DBL_MAX_10_EXP);
    PRINTINT (DBL_MAX_EXP);
    PRINTINT (DBL_MIN_10_EXP);
    PRINTINT (DBL_MIN_EXP);

    PRINTINT (FLT_DIG);
    PRINTINT (FLT_MANT_DIG);
    PRINTINT (FLT_MAX_10_EXP);
    PRINTINT (FLT_MAX_EXP);
    PRINTINT (FLT_MIN_10_EXP);
    PRINTINT (FLT_MIN_EXP);
    PRINTINT (FLT_RADIX);

#ifndef _RWSTD_NO_LONG_DOUBLE

    PRINTINT (LDBL_DIG);
    PRINTINT (LDBL_MANT_DIG);
    PRINTINT (LDBL_MAX_10_EXP);
    PRINTINT (LDBL_MAX_EXP);
    PRINTINT (LDBL_MIN_10_EXP);
    PRINTINT (LDBL_MIN_EXP);

#endif   //  _RWSTD_NO_LONG_DOUBLE


#if defined (DBL_MAX)
    PRINTFLT (DBL_MAX, "l", DBL_DIG, "");
#endif   // DBL_MAX

#if defined (FLT_MAX)
    PRINTFLT (FLT_MAX, "", FLT_DIG, "F");
#endif   // FLT_MAX

#ifndef _RWSTD_NO_LONG_DOUBLE
#  if defined (LDBL_MAX)
    PRINTFLT (LDBL_MAX, LDBL_FMT, DBL_DIG, "F");
#  endif   // LDBL_MAX
#endif   // _RWSTD_NO_LONG_DOUBLE

#if defined (DBL_EPSILON)
    PRINTFLT (DBL_EPSILON, "l", DBL_DIG, "");
#endif   // DBL_EPSILON

#if defined (DBL_MIN)
    PRINTFLT (DBL_MIN, "l", DBL_DIG, "");
#endif   // DBL_MIN

#if defined (FLT_EPSILON)
    PRINTFLT (FLT_EPSILON, "", FLT_DIG, "F");
#endif   // FLT_EPSILON

#if defined (FLT_MIN)
    PRINTFLT (FLT_MIN, "", FLT_DIG, "F");
#endif   // FLT_MIN

#ifndef _RWSTD_NO_LONG_DOUBLE
#  if defined (LDBL_EPSILON)
    PRINTFLT (LDBL_EPSILON, LDBL_FMT, LDBL_DIG, "L");
#  endif   // LDBL_EPSILON

#  if defined (LDBL_MIN)
    PRINTFLT (LDBL_MIN, LDBL_FMT, LDBL_DIG, "L");
#  endif   // LDBL_MIN
#endif   // _RWSTD_NO_LONG_DOUBLE


#if !defined (ERANGE)
#  define ERANGE -1
#endif   // ERANGE

#ifndef _RWSTD_NO_STRTOF

    errno = 0;

    // determine whether strtof() sets errno on underflow
    const float f = strtof ("1.0e-999", (char**)0);


    if (f < 0.0 || f > 1.0 || !errno)
        printf ("#define _RWSTD_NO_STRTOF_UFLOW\n");
    else
        printf ("// #define _RWSTD_NO_STRTOF_UFLOW    // %d%s\n",
                errno, ERANGE == errno ? " (ERANGE)" : "");

#endif   // _RWSTD_NO_STRTOF

#ifndef _RWSTD_NO_STRTOD

    errno = 0;

    // determine whether strtod() sets errno on underflow
    const double d = strtod ("1.0e-999", (char**)0);


    if (d < 0.0 || d > 1.0 || !errno)
        printf ("#define _RWSTD_NO_STRTOD_UFLOW\n");
    else
        printf ("// #define _RWSTD_NO_STRTOD_UFLOW    // %d%s\n",
                errno, ERANGE == errno ? " (ERANGE)" : "");

#endif   // _RWSTD_NO_STRTOD

#ifndef _RWSTD_NO_LONG_DOUBLE
#  ifndef _RWSTD_NO_STRTOLD

    errno = 0;

#    if !defined (__hpux) || !defined (_LONG_DOUBLE)

    // HP-UX strtold() returns struct long_double
    // the macro _LONG_DOUBLE is #defined when the struct is defined
    // note that gcc's replacement <stdlib.h> may actually define the
    // function with the correct signature

    // determine whether strtold() sets errno on underflow
    const long double ld = strtold ("1.0e-9999", (char**)0);

#    else   // HP-UX with _LONG_DOUBLE #defined

    union {
        long double ld;
        long_double data;
    } ldu;

    ldu.data = strtold ("1.0e-9999", (char**)0);
    const long double ld = ldu.ld;

#    endif   // HP-UX, _LONG_DOUBLE

    if (ld < 0.0 || ld > 1.0 || !errno)
        printf ("#define _RWSTD_NO_STRTOLD_UFLOW\n");
    else
        printf ("// #define _RWSTD_NO_STRTOLD_UFLOW   // %d%s\n",
                errno, ERANGE == errno ? " (ERANGE)" : "");

#  endif   // _RWSTD_NO_STRTOLD
#endif   // _RWSTD_NO_LONG_DOUBLE

    return 0;
}


