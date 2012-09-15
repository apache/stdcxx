// computing numerical limits

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

#include "config.h"

#include <stdio.h>    // for printf()

#ifndef _RWSTD_NO_LIMITS_H
   // avoid including broken <limits.h> (due to the use of #include_next)
   // with EDG eccp on Linux
#  include <limits.h>   // for MB_LEN_MAX
#endif   // _RWSTD_NO_LIMITS_H


// establish a dependency on the test for long long
// and #define the LONG_LONG macro used in "type.h"
#ifndef _RWSTD_NO_LONG_LONG
#  define LONG_LONG long long
#else   // if defined (_RWSTD_NO_LONG_LONG)
#  if defined (_MSC_VER)
#    define LONG_LONG   __int64
#  endif   // _MSC_VER
#endif   // _RWSTD_NO_LONG_LONG

#include "types.h"   // for type_name()


#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE
#    include "terminate.h"
#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


// set to 1 if this is not a two's complement architecture
int no_twos_complement = 0;

// volatile to foil aggressive optimizers
volatile int zero            = 0;
volatile int one             = zero + 1;
volatile int two             = one + 1;
volatile int zero_complement = ~zero;



template<class T>
void print_limit (T n, const char *pfx, const char *sfx,
                  bool is_max, const char *type)
{
    // prevents meaningless comparison warnings when T is unsigned
    // e.g., for (n < 0)
    /* not const */ T zero = T (0);

    char nstr [64] = { 0 };

    char* pnstr = nstr + sizeof nstr - 2;

    const char digit [] = {
        '0','1','2','3','4','5','6','7','8','9', 'a', 'b', 'c', 'd', 'e', 'f'
    };

    if (is_max || n >= zero) {

        T tnstr = n;

        *--pnstr = digit [tnstr % 16];
        while (tnstr /= 16)
            *--pnstr = digit [tnstr % 16];
    }

    *--pnstr = 'x';

    if (   'c' == type [0]
        && 'h' == type [1]
        && 'a' == type [2]
        && 'r' == type [3]
        && '\0' == type [4]) {

        *--pnstr = '\\';
        *--pnstr = '\'';
        nstr [sizeof nstr - 2] = '\'';
    }
    else {
        *--pnstr = '0';
    }

    char macro_name [64];
    char macro_value [64];

    if (is_max) {
        sprintf (macro_name, "_RWSTD_%s_MAX", pfx);
        sprintf (macro_value, "%s%s", pnstr, sfx);

    }
    else {
        sprintf (macro_name, "_RWSTD_%s_MIN ", pfx);

        if (n < zero) {
            if (no_twos_complement) {
                sprintf (macro_value, "-_RWSTD_%s_MAX", pfx);
            }
            else {
                sprintf (macro_value, "(-_RWSTD_%s_MAX - 1%s)", pfx, sfx);
            }
        }
        else {
            sprintf (macro_value, "%s%s", pnstr, sfx);
        }
    }

    printf ("#define %-18s %s\n", macro_name, macro_value);
}


template <class T>
T compute_min (T min)
{
    // prevents overzealous gcc optimizer from invoking
    // undefined behavior on signed integer over/underflow
    for (T tmp; ; --min) {
        tmp = T (min - one);
        if (tmp >= min)
            break;
    }

    return min;
}


template <class T>
const char* type_suffix (T) { return ""; }

const char* type_suffix (short) { return ""; }
const char* type_suffix (unsigned short) { return "U"; }
const char* type_suffix (int) { return ""; }
const char* type_suffix (unsigned int) { return "U"; }
const char* type_suffix (long) { return "L"; }
const char* type_suffix (unsigned long) { return "UL"; }

#ifndef _RWSTD_NO_LONG_LONG
const char* type_suffix (long long) { return "LL"; }
const char* type_suffix (unsigned long long) { return "ULL"; }
#else
#  if defined (_MSC_VER)
const char* type_suffix (__int64) { return "L"; }
const char* type_suffix (unsigned __int64) { return "UL"; }
#  endif   // _MSC_VER
#endif


template <class T>
T compute_limits (T *pval, const char *pfx, const char *sfx, const char *type)
{
    T min = T (-one);
    T volatile max = T (one);

    int is_signed = T (min - one) < T (zero);

    if (!is_signed)
        max = min;

    // compute the maximum representable value
    for (; T (max * two) > max; max *= two);
    for (*pval = max / (two + two); *pval; ) {
        if (T (max + *pval) < max) {
            if (*pval > T (two))
                *pval /= two;
            else if (max < T (max + one))
                *pval = one;
            else
                break;
        }
        else
            max += *pval;
    }

    // print the maximum representable value
    print_limit (max, pfx, sfx, true, type);

    // compute the minimum representable value
    for (min = -max; T (min * two) < min; min *= two);

    // working around a gcc optimizer "feature" (PR #26211)
    // signed integer overflow is undefined behavior
    // for (; T (min - 1) < min; min -= 1);
    min = compute_min (min);

    print_limit (min, pfx, sfx, false, type);

    return max;
}


unsigned compute_char_bits ()
{
    unsigned bits = 0;

    for (unsigned char c = '\01'; c; c <<= 1, ++bits);

    printf ("#define _RWSTD_CHAR_BIT    %2u\n", bits);

    return bits;
}


// used to compute the size of a pointer to a member function
struct EmptyStruct { };


// to silence printf() format comaptibility warnings
#define SIZEOF(T)   unsigned (sizeof (T))


int main ()
{
    // determine whether this is a two's complement architecture
    // and set the no_twos_complement global variable to 1 if not
    if (two + zero_complement != one)
        no_twos_complement = 1;

    // compute sizes of fundamental types

#ifndef _RWSTD_NO_BOOL
    printf ("#define _RWSTD_BOOL_SIZE   %2u /* sizeof (bool) */\n",
            SIZEOF (bool));
#endif   // _RWSTD_NO_BOOL

    printf ("#define _RWSTD_CHAR_SIZE   %2u /* sizeof (char) */\n",
            SIZEOF (char));
    printf ("#define _RWSTD_SHRT_SIZE   %2u /* sizeof (short) */\n",
            SIZEOF (short));
    printf ("#define _RWSTD_INT_SIZE    %2u /* sizeof (int) */\n",
            SIZEOF (int));
    printf ("#define _RWSTD_LONG_SIZE   %2u /* sizeof (long) */\n",
            SIZEOF (long));

    printf ("#define _RWSTD_FLT_SIZE    %2u /* sizeof (float) */\n",
            SIZEOF (float));
    printf ("#define _RWSTD_DBL_SIZE    %2u /* sizeof (double) */\n",
            SIZEOF (double));

#ifndef _RWSTD_NO_LONG_DOUBLE
    printf ("#define _RWSTD_LDBL_SIZE   %2u /* sizeof (long double) */\n",
            SIZEOF (long double));
#endif   // _RWSTD_NO_LONG_DOUBLE

    // compute the sizes of data and function pointers
    printf ("#define _RWSTD_PTR_SIZE    %2u /* sizeof (void*) */\n",
            SIZEOF (void*));

    typedef void (*fun_ptr_t)();
    printf ("#define _RWSTD_FUNPTR_SIZE %2u /* sizeof (void(*)()) */\n",
            SIZEOF (fun_ptr_t));

    typedef void (EmptyStruct::*memfun_ptr_t)();
    printf ("#define _RWSTD_MEMPTR_SIZE %2u "
            "/* sizeof (void (struct::*)()) */\n",
            SIZEOF (memfun_ptr_t));

    // compute integral limits

    const unsigned char_bits = compute_char_bits ();

#ifndef _RWSTD_NO_BOOL

    printf ("#define _RWSTD_BOOL_MIN   !!0\n");
    printf ("#define _RWSTD_BOOL_MAX    !0\n");

#endif   // _RWSTD_NO_BOOL

    if (0 == no_twos_complement) {
        // comment out the next #define (this is two's complement
        // architecture)
        printf ("%s", "// ");
    }
    printf ("#define _RWSTD_NO_TWOS_COMPLEMENT\n");

#define MKLIMITS(T, pfx, sfx, type)            \
    do {                                       \
        T buf = 0;                             \
        compute_limits (&buf, pfx, sfx, type); \
    } while (0)

    MKLIMITS (char, "CHAR", "", "char");
    MKLIMITS (signed char, "SCHAR", "", "signed char");
    MKLIMITS (unsigned char, "UCHAR", "U", "unsigned char");

    MKLIMITS (short, "SHRT", "", "short");
    MKLIMITS (unsigned short, "USHRT", "U", "unsigned short");

    MKLIMITS (int, "INT", "", "int");
    MKLIMITS (unsigned int, "UINT", "U", "unsigned int");

    MKLIMITS (long, "LONG", "L", "long");
    MKLIMITS (unsigned long, "ULONG", "UL", "unsigned long");

#ifndef _RWSTD_NO_LONG_LONG

#    define LLong long long

    printf ("#define _RWSTD_LLONG_SIZE  %2u\n", SIZEOF (LLong));

    const char llong_name[] = "long long";

    MKLIMITS (LLong, "LLONG", "LL", "long long");
    MKLIMITS (unsigned LLong, "ULLONG", "ULL", "unsigned long long");

#else   // if defined (_RWSTD_NO_LONG_LONG)

#  if defined (_MSC_VER)

#    define LLong __int64

    printf ("#define _RWSTD_LLONG_SIZE  %2u\n", SIZEOF (LLong));

    const char llong_name[] = "__int64";

    MKLIMITS (LLong, "LLONG", "L", "__int64");
    MKLIMITS (unsigned LLong, "ULLONG", "UL", "unsigned __int64");

#    else

#      define LLong long

    const char llong_name[] = "long";

#    endif   // _MSC_VER

#endif   // _RWSTD_NO_LONG_LONG

#ifndef _RWSTD_NO_WCHAR_T

    printf ("#define _RWSTD_WCHAR_SIZE  %2u /* sizeof (wchar_t) */\n",
            SIZEOF (wchar_t));

    const char *suffix = "U";
    if ((wchar_t)~0 < (wchar_t)0)
        suffix = "";
    
    MKLIMITS (wchar_t, "WCHAR", suffix, "wchar_t");

#endif   // _RWSTD_NO_WCHAR_T

#if defined (MB_LEN_MAX)

    printf ("#define _RWSTD_MB_LEN_MAX    %d   /* libc value */\n",
            MB_LEN_MAX);

#else   // if !defined (MB_LEN_MAX)

#  if defined (_AIX)
    printf ("#define _RWSTD_MB_LEN_MAX %d   /* known AIX libc value */\n", 4);
#  elif defined (__hpux)
    printf ("#define _RWSTD_MB_LEN_MAX %d   /* known HP-UX libc value */\n", 4);
#  elif defined (__sgi) || defined (sgi)
    printf ("#define _RWSTD_MB_LEN_MAX %d   /* known IRIX libc value */\n", 5);
#  elif defined (__GLIBC__) && __GLIBC_MINOR__ <= 1
    printf ("#define _RWSTD_MB_LEN_MAX %d   /* known glibc 2.1 value */\n", 6);
#  elif defined (__GLIBC__) && __GLIBC_MINOR__ >= 2
    printf ("#define _RWSTD_MB_LEN_MAX %d   /* known glibc 2.2 value */\n", 16);
#  elif defined (__sun__) || defined (__sun) || defined (__sun)
    printf ("#define _RWSTD_MB_LEN_MAX %d   /* known SunOS libc value */\n", 5);
#  elif defined (_WIN32)
    printf ("#define _RWSTD_MB_LEN_MAX %d   /* known WIN32 libc value */\n", 5);
#  else
    printf ("#define _RWSTD_MB_LEN_MAX %d   /* guess */\n", 8);
#  endif

#endif   // MB_LEN_MAX

    //////////////////////////////////////////////////////////////////
    // #define macros for exact and lest-width integer types

    // width_bits will have the corresponding bit set for each exact-width
    // type already processed (i.e., bit 0 for an 8-bit integer type, bit
    // 1 for a 16-bit integer, etc)
    int width_bits = 0;

#define PRINT_SPECIFIC(width, least, type)                              \
    do {                                                                \
        /* avoid warnings about expression being constant */            \
        const char* least_str = least;                                  \
        if (least_str && *least_str)                                    \
            printf ("#define _RWSTD_INT_LEAST%d_T  %s  _RWSTD_%s_T\n"   \
                    "#define _RWSTD_UINT_LEAST%d_T %s  _RWSTD_U%s_T\n", \
                    width, width < 10 ? " " : "", type,                 \
                    width, width < 10 ? " " : "", type);                \
        else                                                            \
            printf ("#define _RWSTD_INT%d_T %s          signed %s\n"    \
                    "#define _RWSTD_UINT%d_T %s         unsigned %s\n", \
                    width, width < 10 ? " " : "", type,                 \
                    width, width < 10 ? " " : "", type);                \
    } while (0)

    if (8 == char_bits) {
        width_bits |= 1;
        PRINT_SPECIFIC (8, "", "char");
    }
    else if (16 == char_bits) {
        width_bits |= 2;
        PRINT_SPECIFIC (16, "", "char");
    }
    else if (32 == char_bits) {
        width_bits |= 4;
        PRINT_SPECIFIC (32, "", "char");
    }
    else if (64 == char_bits) {
        width_bits |= 8;
        PRINT_SPECIFIC (64, "", "char");
    }

    if (16 == char_bits * sizeof (short) && !(width_bits & 2)) {
        width_bits |= 2;
        PRINT_SPECIFIC (16, "", "short");
    }
    else if (32 == char_bits * sizeof (short) && !(width_bits & 4)) {
        width_bits |= 4;
        PRINT_SPECIFIC (32, "", "short");
    }
    else if (64 == char_bits * sizeof (short) && !(width_bits & 8)) {
        width_bits |= 8;
        PRINT_SPECIFIC (64, "", "short");
    }
    else if (128 == char_bits * sizeof (short) && !(width_bits & 16)) {
        width_bits |= 16;
        PRINT_SPECIFIC (128, "", "short");
    }

    if (32 == char_bits * sizeof (int) && !(width_bits & 4)) {
        width_bits |= 4;
        PRINT_SPECIFIC (32, "", "int");
    }
    else if (64 == char_bits * sizeof (int) && !(width_bits & 8)) {
        width_bits |= 8;
        PRINT_SPECIFIC (64, "", "int");
    }
    else if (128 == char_bits * sizeof (int) && !(width_bits & 16)) {
        width_bits |= 16;
        PRINT_SPECIFIC (128, "", "int");
    }

    if (32 == char_bits * sizeof (long) && !(width_bits & 4)) {
        width_bits |= 4;
        PRINT_SPECIFIC (32, "", "long");
    }
    else if (64 == char_bits * sizeof (long) && !(width_bits & 8)) {
        width_bits |= 8;
        PRINT_SPECIFIC (64, "", "long");
    }
    else if (128 == char_bits * sizeof (long) && !(width_bits & 16)) {
        width_bits |= 16;
        PRINT_SPECIFIC (128, "", "long");
    }

    if (32 == char_bits * sizeof (LLong) && !(width_bits & 4)) {
        width_bits |= 4;
        PRINT_SPECIFIC (32, "", llong_name);
    }
    else if (64 == char_bits * sizeof (LLong) && !(width_bits & 8)) {
        width_bits |= 8;
        PRINT_SPECIFIC (64, "", llong_name);

    }
    else if (128 == char_bits * sizeof (LLong) && !(width_bits & 16)) {
        width_bits |= 16;
        PRINT_SPECIFIC (128, "", llong_name);
    }

    //////////////////////////////////////////////////////////////////
    // print the names of the width-specific least integer types
    // i.e., INT_LEAST8_T, INT_LEAST16_T, INT_LEAST32_T, ...

    if (width_bits & (1 << 0))
        PRINT_SPECIFIC (8, "_LEAST", "INT8");
    else if (width_bits & (1 << 1))
        PRINT_SPECIFIC (8, "_LEAST", "INT16");
    else if (width_bits & (1 << 2))
        PRINT_SPECIFIC (8, "_LEAST", "INT32");
    else if (width_bits & (1 << 3))
        PRINT_SPECIFIC (8, "_LEAST", "INT64");
    else if (width_bits & (1 << 4))
        PRINT_SPECIFIC (8, "_LEAST", "INT128");

    if (width_bits & (1 << 1))
        PRINT_SPECIFIC (16, "_LEAST", "INT16");
    else if (width_bits & (1 << 2))
        PRINT_SPECIFIC (16, "_LEAST", "INT32");
    else if (width_bits & (1 << 3))
        PRINT_SPECIFIC (16, "_LEAST", "INT64");
    else if (width_bits & (1 << 4))
        PRINT_SPECIFIC (16, "_LEAST", "INT128");

    if (width_bits & (1 << 2))
        PRINT_SPECIFIC (32, "_LEAST", "INT32");
    else if (width_bits & (1 << 3))
        PRINT_SPECIFIC (32, "_LEAST", "INT64");
    else if (width_bits & (1 << 4))
        PRINT_SPECIFIC (32, "_LEAST", "INT128");

    if (width_bits & (1 << 3))
        PRINT_SPECIFIC (64, "_LEAST", "INT64");
    else if (width_bits & (1 << 4))
        PRINT_SPECIFIC (64, "_LEAST", "INT128");

    if (width_bits & (1 << 4))
        PRINT_SPECIFIC (128, "_LEAST", "INT128");

    return 0;
}
