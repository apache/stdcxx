// computing numerical limits

// Working around a Compaq c++ headers problem (PR#27459)
#if defined (__PURE_CNAME)
#  undef __PURE_CNAME
#endif // __PURE_CNAME

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#ifndef _RWSTD_NO_LIMITS_H
#  include <limits.h>
#endif   // _RWSTD_NO_LIMITS_H

#ifndef _RWSTD_NO_FLOAT_H
#  include <float.h>

#  if defined (__EDG__) && defined (__linux__)

     // gcc on Linux #defines these using its own propriterary __extension__
#    undef LDBL_EPSILON
#    undef LDBL_MIN
#    undef LDBL_MAX

     // redefine to prevent compilation errors
#    define LDBL_EPSILON 1.0842021724855044e-19L
#    define LDBL_MIN     3.3621031431120935e-4932L
#    define LDBL_MAX     1.1897314953572317e+4932L
#  endif   // defined (__EDG__) && defined (__linux__)

#endif   // _RWSTD_NO_FLOAT_H


#include <errno.h>    // for ERANGE, errno
#include <stdio.h>    // for printf()
#include <stdlib.h>   // for strtod()


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

namespace std {

void terminate ()
{
    for (; ;) {
        char *p = 0;
        *p = 0;        // force a SIGSEGV
    }
}

}   // std


#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


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

    if (is_max) {
        printf ("#define _RWSTD_%s_MAX %s%s", pfx, pnstr, sfx);
    }
    else {
        if (n < zero) {
            printf ("#define _RWSTD_%s_MIN (-_RWSTD_%s_MAX - 1%s)",
                    pfx, pfx, sfx);
        }
        else {
            printf ("#define _RWSTD_%s_MIN %s%s", pfx, pnstr, sfx);
        }
    }

    printf ("\n");
}


template <class T>
T foo (T one, T min)
{
    // prevents overzealous gcc optimizer from invoking
    // undefined behavior on signed integer over/underflow
    for (T tmp; ; --min) {
        tmp = min - one;
        if (tmp >= min)
            break;
    }

    return min;
}

template <class T>
T greater (T lhs, T rhs)
{
    // prevents overzealous gcc optimizer from invoking
    // undefined behavior on signed integer over/underflow
    return lhs > rhs;
}


template <class T>
const char* type_name (T) { return "unknown type"; }

const char* type_name (short) { return "short"; }
const char* type_name (unsigned short) { return "unsigned short"; }
const char* type_name (int) { return "int"; }
const char* type_name (unsigned int) { return "unsigned int"; }
const char* type_name (long) { return "long"; }
const char* type_name (unsigned long) { return "unsigned long"; }

#ifndef _RWSTD_NO_LONG_LONG
const char* type_name (long long) { return "long long"; }
const char* type_name (unsigned long long) { return "unsigned long long"; }
#else
#  if defined (_MSC_VER)
const char* type_name (__int64) { return "__int64"; }
const char* type_name (unsigned __int64) { return "unsigned __int64"; }
#  endif   // _MSC_VER
#endif

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
T compute_limits (T, const char *pfx, const char *sfx, const char *type)
{
    T zero = T (0);
    T min  = T (-1);
    T max  = T (1);
    T one  = T (1);

    int is_signed = T (min - T (1)) < zero;

    if (!is_signed)
        max = min;

    for (; T (max * 2) > max; max *= 2);
    for (T n = max / 4; n; ) {
        if (T (max + n) < max) {
            if (n > 2)
                n /= 2;
            else if (greater (T (max + one), max))
                n = 1;
            else
                break;
        }
        else
            max += n;
    }

    print_limit (max, pfx, sfx, true, type);

    min = -max;

    // compute a minimum
    for (; T (min * 2) < min; min *= 2);

    // working around a gcc optimizer "feature" (PR #26211)
    // signed integer overflow is undefined behavior
    // for (; T (min - 1) < min; min -= 1);
    min = foo (one, min);

    print_limit<T> (min, pfx, sfx, false, type);

    return max;
}


unsigned compute_char_bits ()
{
    unsigned bits = 0;

    for (unsigned char c = '\01'; c; c <<= 1, ++bits);

    printf ("#define _RWSTD_CHAR_BIT %u\n", bits);

    return bits;
}


// print a floating point number, either as a string (if the stringized
// constant is a valid number, and not some complex expression), or as
// a formatted floating point value
template <class FloatT>
void print_float (FloatT x, const char *xstr,
                  const char *macro_name, const char *fmt, int prec)
{
    if ('-' == *xstr || '0' <= *xstr && '9' >= *xstr) {
        printf ("#define %s %s", macro_name, xstr);
    }
    else {
        printf ("#define %s ", macro_name);
        printf (fmt, prec + 2, x);
    }
    printf ("\n");
}

#define DO_STRINGIZE(x)  #x
#define STRINGIZE(x)     DO_STRINGIZE (x)

#define PRINTFLT(x, fmt, prec, suffix) \
    print_float (x, STRINGIZE (x), "_RWSTD_" #x, "%.*" fmt "e" suffix, prec)


// used to compute the size of a pointer to a member function
struct EmptyStruct { };


// to silence printf() format comaptibility warnings
#define SIZEOF(T)   unsigned (sizeof (T))


int main ()
{
#if !defined (_RWSTD_USE_CONFIG)

    printf ("/**/\n#undef _RWSTD_LIMITS\n");

#endif   // _RWSTD_USE_CONFIG

    // compute sizes of fundamental types

#ifndef _RWSTD_NO_BOOL
    printf ("#define _RWSTD_BOOL_SIZE   %u /* sizeof (bool) */\n",
            SIZEOF (bool));
#endif   // _RWSTD_NO_BOOL

    printf ("#define _RWSTD_CHAR_SIZE   %u /* sizeof (char) */\n",
            SIZEOF (char));
    printf ("#define _RWSTD_SHRT_SIZE   %u /* sizeof (short) */\n",
            SIZEOF (short));
    printf ("#define _RWSTD_INT_SIZE    %u /* sizeof (int) */\n",
            SIZEOF (int));
    printf ("#define _RWSTD_LONG_SIZE   %u /* sizeof (long) */\n",
            SIZEOF (long));

    printf ("#define _RWSTD_FLT_SIZE    %u /* sizeof (float) */\n",
            SIZEOF (float));
    printf ("#define _RWSTD_DBL_SIZE    %u /* sizeof (double) */\n",
            SIZEOF (double));

#ifndef _RWSTD_NO_LONG_DOUBLE
    printf ("#define _RWSTD_LDBL_SIZE   %u /* sizeof (long double) */\n",
            SIZEOF (long double));
#endif   // _RWSTD_NO_LONG_DOUBLE

    // compute the sizes of data and function pointers
    printf ("#define _RWSTD_PTR_SIZE    %u /* sizeof (void*) */\n",
            SIZEOF (void*));

    typedef void (*fun_ptr_t)();
    printf ("#define _RWSTD_FUNPTR_SIZE %u /* sizeof (void(*)()) */\n",
            SIZEOF (fun_ptr_t));

    typedef void (EmptyStruct::*memfun_ptr_t)();
    printf ("#define _RWSTD_MEMPTR_SIZE %u "
            "/* sizeof (void (struct::*)()) */\n",
            SIZEOF (memfun_ptr_t));

    // compute integral limits

    const unsigned char_bits = compute_char_bits ();

#ifndef _RWSTD_NO_BOOL

    printf ("#define _RWSTD_BOOL_MIN !!0\n");
    printf ("#define _RWSTD_BOOL_MAX !0\n");

#endif   // _RWSTD_NO_BOOL

    compute_limits ((char)0, "CHAR", "", "char");
    compute_limits ((signed char)0, "SCHAR", "", "signed char");
    compute_limits ((unsigned char)0, "UCHAR", "U", "unsigned char");

    compute_limits ((short)0, "SHRT", "", "short");
    compute_limits ((unsigned short)0, "USHRT", "U", "unsigned short");

    compute_limits ((int)0, "INT", "", "int");
    compute_limits ((unsigned int)0, "UINT", "U", "unsigned int");

    compute_limits ((long)0, "LONG", "L", "long");
    compute_limits ((unsigned long)0, "ULONG", "UL", "unsigned long");

#ifndef _RWSTD_NO_LONG_LONG

#    define LLong long long

    printf ("#define _RWSTD_LLONG_SIZE  %u\n", SIZEOF (LLong));

    const char llong_name[] = "long long";

    compute_limits ((LLong)0, "LLONG", "LL", "long long");
    compute_limits ((unsigned LLong)0, "ULLONG", "ULL", "unsigned long long");

#else   // if defined (_RWSTD_NO_LONG_LONG)

#  if defined (_MSC_VER)

#    define LLong __int64

    printf ("#define _RWSTD_LLONG_SIZE  %u\n", SIZEOF (LLong));

    const char llong_name[] = "__int64";

    compute_limits ((LLong)0, "LLONG", "L", "__int64");
    compute_limits ((unsigned LLong)0, "ULLONG", "UL",
                    "unsigned __int64");

#    else

#      define LLong long

    const char llong_name[] = "long";

#    endif   // _MSC_VER

#endif   // _RWSTD_NO_LONG_LONG

#ifndef _RWSTD_NO_WCHAR_T

    printf ("#define _RWSTD_WCHAR_T_SIZE  %u /* sizeof (wchar_t) */\n",
            SIZEOF (wchar_t));

    const char *suffix = "U";
    if ((wchar_t)~0 < (wchar_t)0)
        suffix = "";
    
    compute_limits ((wchar_t)0, "WCHAR_T", suffix, "wchar_t");

#endif   // _RWSTD_NO_WCHAR_T

    // compute the maximum and minimum for size_t and ptrdiff_t
    compute_limits (sizeof (int), "SIZE",
                    type_suffix (sizeof (int)),
                    type_name (sizeof (int)));

    compute_limits ((int*)0 - (int*)0, "PTRDIFF",
                    type_suffix ((int*)0 - (int*)0),
                    type_name ((int*)0 - (int*)0));


#ifndef _RWSTD_NO_WINT_T
   // establish a dependency on WINT_T.cpp
#  if defined (_RWSTD_WINT_T)

    // compute the maximum and minimum for wint_t
    compute_limits ((_RWSTD_WINT_T)0, "WINT",
                    type_suffix ((_RWSTD_WINT_T)0),
                    type_name ((_RWSTD_WINT_T)0));

#  endif   // _RWSTD_WINT_T
#endif   // _RWSTD_NO_WINT_T

#if defined (MB_LEN_MAX)

    printf ("#define _RWSTD_MB_LEN_MAX %d   /* libc value */\n", MB_LEN_MAX);

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
#  else
    printf ("#define _RWSTD_MB_LEN_MAX %d   /* guess */\n", 8);
#  endif

#endif   // MB_LEN_MAX


    // #define macros for exact-width integer types
    int bits = 0;

    if (8 == char_bits) {
        bits |= 1;
        printf ("#define _RWSTD_INT8_T   %s\n",
                '\xff' < 0 ? "char" : "signed char");
        printf ("#define _RWSTD_UINT8_T  unsigned char\n");
    }
    else if (16 == char_bits) {
        bits |= 2;
        printf ("#define _RWSTD_INT16_T  %s\n",
                '\xff' < 0 ? "char" : "signed char");
        printf ("#define _RWSTD_UINT16_T unsigned char\n");
    }
    else if (32 == char_bits) {
        bits |= 4;
        printf ("#define _RWSTD_INT32_T  %s\n",
                '\xff' < 0 ? "char" : "signed char");
        printf ("#define _RWSTD_UINT32_T unsigned char\n");
    }
    else if (64 == char_bits) {
        bits |= 8;
        printf ("#define _RWSTD_INT64_T  %s\n",
                '\xff' < 0 ? "char" : "signed char");
        printf ("#define _RWSTD_UINT64_T unsigned char\n");
    }

    if (16 == char_bits * sizeof (short) && !(bits & 2)) {
        bits |= 2;
        printf ("#define _RWSTD_INT16_T  short\n");
        printf ("#define _RWSTD_UINT16_T unsigned short\n");
    }
    else if (32 == char_bits * sizeof (short) && !(bits & 4)) {
        bits |= 4;
        printf ("#define _RWSTD_INT32_T  short\n");
        printf ("#define _RWSTD_UINT32_T unsigned short\n");
    }
    else if (64 == char_bits * sizeof (short) && !(bits & 8)) {
        bits |= 8;
        printf ("#define _RWSTD_INT64_T  short\n");
        printf ("#define _RWSTD_UINT64_T unsigned short\n");
    }
    else if (128 == char_bits * sizeof (short) && !(bits & 16)) {
        bits |= 16;
        printf ("#define _RWSTD_INT128_T  short\n");
        printf ("#define _RWSTD_UINT128_T unsigned short\n");
    }

    if (32 == char_bits * sizeof (int) && !(bits & 4)) {
        bits |= 4;
        printf ("#define _RWSTD_INT32_T  int\n");
        printf ("#define _RWSTD_UINT32_T unsigned int\n");
    }
    else if (64 == char_bits * sizeof (int) && !(bits & 8)) {
        bits |= 8;
        printf ("#define _RWSTD_INT64_T  int\n");
        printf ("#define _RWSTD_UINT64_T unsigned int\n");
    }
    else if (128 == char_bits * sizeof (int) && !(bits & 16)) {
        bits |= 16;
        printf ("#define _RWSTD_INT64_T  int\n");
        printf ("#define _RWSTD_UINT64_T unsigned int\n");
    }

    if (32 == char_bits * sizeof (long) && !(bits & 4)) {
        bits |= 4;
        printf ("#define _RWSTD_INT32_T  long\n");
        printf ("#define _RWSTD_UINT32_T unsigned long\n");
    }
    else if (64 == char_bits * sizeof (long) && !(bits & 8)) {
        bits |= 8;
        printf ("#define _RWSTD_INT64_T  long\n");
        printf ("#define _RWSTD_UINT64_T unsigned long\n");
    }
    else if (128 == char_bits * sizeof (long) && !(bits & 16)) {
        bits |= 16;
        printf ("#define _RWSTD_INT64_T  long\n");
        printf ("#define _RWSTD_UINT64_T unsigned long\n");
    }

    if (32 == char_bits * sizeof (LLong) && !(bits & 4)) {
        bits |= 4;
        printf ("#define _RWSTD_INT32_T  %s\n", llong_name);
        printf ("#define _RWSTD_UINT32_T unsigned %s\n", llong_name);
    }
    else if (64 == char_bits * sizeof (LLong) && !(bits & 8)) {
        bits |= 8;
        printf ("#define _RWSTD_INT64_T  %s\n", llong_name);
        printf ("#define _RWSTD_UINT64_T unsigned %s\n", llong_name);
    }
    else if (128 == char_bits * sizeof (LLong) && !(bits & 16)) {
        bits |= 16;
        printf ("#define _RWSTD_INT64_T  %s\n", llong_name);
        printf ("#define _RWSTD_UINT64_T unsigned %s\n", llong_name);
    }

    //////////////////////////////////////////////////////////////////
    // compute floating point limits

#undef LDBL_FMT

#ifdef _RWSTD_LDBL_PRINTF_PREFIX
#  define LDBL_FMT   _RWSTD_LDBL_PRINTF_PREFIX
#else
#  define LDBL_FMT   "L"
#endif

#if defined (FLT_ROUNDS)
    printf ("#define _RWSTD_FLT_ROUNDS %d   /* %s */\n", FLT_ROUNDS,
               0 == FLT_ROUNDS ? "round toward zero"
            :  1 == FLT_ROUNDS ? "round to nearest"
            :  2 == FLT_ROUNDS ? "round toward infinity"
            :  3 == FLT_ROUNDS ? "round toward negative infinity"
            :  "indeterminable");
#endif   // FLT_ROUNDS

#if !defined (DBL_DIG)
#  define DBL_DIG   15
#endif   // DBL_DIG

    printf ("#define _RWSTD_DBL_DIG %d\n", DBL_DIG);


#if defined (DBL_MANT_DIG)
    printf ("#define _RWSTD_DBL_MANT_DIG %d\n", DBL_MANT_DIG);
#endif   // DBL_MANT_DIG

#if defined (DBL_MAX_10_EXP)
    printf ("#define _RWSTD_DBL_MAX_10_EXP %d\n", DBL_MAX_10_EXP);
#endif   // DBL_MAX_10_EXP

#if defined (DBL_MAX_EXP)
    printf ("#define _RWSTD_DBL_MAX_EXP %d\n", DBL_MAX_EXP);
#endif   // DBL_MAX_EXP

#if defined (DBL_MIN_10_EXP)
    printf ("#define _RWSTD_DBL_MIN_10_EXP %d\n", DBL_MIN_10_EXP);
#endif   // DBL_MIN_10_EXP

#if defined (DBL_MIN_EXP)
    printf ("#define _RWSTD_DBL_MIN_EXP %d\n", DBL_MIN_EXP);
#endif   // DBL_MIN_EXP

#if defined (DECIMAL_DIG)
    printf ("#define _RWSTD_DECIMAL_DIG %d\n", DECIMAL_DIG);
#endif   // DECIMAL_DIG


#if !defined (FLT_DIG)
#  define FLT_DIG   6
#endif   // FLT_DIG

    printf ("#define _RWSTD_FLT_DIG %d\n", FLT_DIG);

#if defined (FLT_MANT_DIG)
    printf ("#define _RWSTD_FLT_MANT_DIG %d\n", FLT_MANT_DIG);
#endif   // FLT_MANT_DIG

#if defined (FLT_MAX_10_EXP)
    printf ("#define _RWSTD_FLT_MAX_10_EXP %d\n", FLT_MAX_10_EXP);
#endif   // FLT_MAX_10_EXP

#if defined (FLT_MAX_EXP)
    printf ("#define _RWSTD_FLT_MAX_EXP %d\n", FLT_MAX_EXP);
#endif   // FLT_MAX_EXP

#if defined (FLT_MIN_10_EXP)
    printf ("#define _RWSTD_FLT_MIN_10_EXP %d\n", FLT_MIN_10_EXP);
#endif   // FLT_MIN_10_EXP

#if defined (FLT_MIN_EXP)
    printf ("#define _RWSTD_FLT_MIN_EXP %d\n", FLT_MIN_EXP);
#endif   // FLT_MIN_EXP

#if defined (FLT_RADIX)
    printf ("#define _RWSTD_FLT_RADIX %d\n", FLT_RADIX);
#endif   // FLT_RADIX


#ifndef _RWSTD_NO_LONG_DOUBLE

#  if !defined (LDBL_DIG)
#    define LDBL_DIG   31
#  endif   // LDBL_DIG

    printf ("#define _RWSTD_LDBL_DIG %d\n", LDBL_DIG);


#  if defined (LDBL_MANT_DIG)
    printf ("#define _RWSTD_LDBL_MANT_DIG %d\n", LDBL_MANT_DIG);
#  endif   // LDBL_MANT_DIG

#  if defined (LDBL_MAX_10_EXP)
    printf ("#define _RWSTD_LDBL_MAX_10_EXP %d\n", LDBL_MAX_10_EXP);
#  endif   // LDBL_MAX_10_EXP

#  if defined (LDBL_MAX_EXP)
    printf ("#define _RWSTD_LDBL_MAX_EXP %d\n", LDBL_MAX_EXP);
#  endif   // LDBL_MAX_EXP

#  if defined (LDBL_MIN_10_EXP)
    printf ("#define _RWSTD_LDBL_MIN_10_EXP %d\n", LDBL_MIN_10_EXP);
#  endif   // LDBL_MIN_10_EXP

#  if defined (LDBL_MIN_EXP)
    printf ("#define _RWSTD_LDBL_MIN_EXP %d\n", LDBL_MIN_EXP);
#  endif   // LDBL_MIN_EXP

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

#    if !defined (__hpux)

    // determine whether strtold() sets errno on underflow
    const long double ld = strtold ("1.0e-9999", (char**)0);

#    else

    union {
        long double ld;
        long_double data;
    } ldu;

    ldu.data = strtold ("1.0e-9999", (char**)0);
    const long double ld = ldu.ld;

#    endif   // __hpux

    if (ld < 0.0 || ld > 1.0 || !errno)
        printf ("#define _RWSTD_NO_STRTOLD_UFLOW\n");
    else
        printf ("// #define _RWSTD_NO_STRTOLD_UFLOW   // %d%s\n",
                errno, ERANGE == errno ? " (ERANGE)" : "");

#  endif   // _RWSTD_NO_STRTOLD
#endif   // _RWSTD_NO_LONG_DOUBLE

    return 0;
}
