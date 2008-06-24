// computing infinity and NaN's

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

#include <stdio.h>    // for printf()
#include <string.h>   // for memset()

#include "config.h"
#include "float_defs.h"  // for FLT_MIN, DBL_MIN, LDBL_MIN

/***************************************************************************
 *
 * IEEE 754 specifies the following formats:
 *
 *     K+1 exponent bits: 1 - 2^K < k < 2^K
 *     N significant bits: -2^N < n < 2^N
 *
 * +-----------+-----+-----+-----+
 * |   FORMAT  |BYTES|K + 1|    N|
 * +===========+=====+=====+=====+
 * |      float|    4|    8|   24|
 * +-----------+-----+-----+-----+
 * |     double|    8|   11|   53|
 * +-----------+-----+-----+-----+
 * |long double|>= 10|>= 15|>= 64|
 * +-----------+-----+-----+-----+
 * |       quad|   16|   15|  113|
 * +-----------+-----+-----+-----+
 *
 * +-----------+----+----------------+-------+-------------------+
 * |NUMBER TYPE|SIGN|K+1 bit EXPONENT|Nth BIT|N-1 bit SIGNIFICAND|
 * +===========+====+================+=======+===================+
 * |     NaN(s)|  ? |binary 111...111|   1   | binary 1xxx...xxx |
 * +-----------+----+----------------+-------+-------------------+
 * |    SNaN(s)|  ? |binary 111...111|   1   |nonzero 0xxx...xxx |
 * +-----------+----+----------------+-------+-------------------+
 * |     INF(s)| +/-|binary 111...111|   1   |                 0 |
 * +-----------+----+-------------- --+-------+-------------------+
 * |    Normals| +/-|     k - 1 + 2^K|   1   |n-2^(N-1) < 2^(N-1)|
 * +-----------+----+----------------+-------+-------------------+
 * | Subnormals| +/-|               0|   0   |    0 < n < 2^(N-1)|
 * +-----------+----+----------------+-------+-------------------+
 * |      Zeros| +/-|               0|   0   |                 0 |
 * +-----------+----+----------------+-------+-------------------+
 *
 **************************************************************************/


// determine whether the architecture is little-endian or big-endian
enum endian { big_endian, little_endian };

static const int endian_test = 1;

static const union {
    const int *pi;
    const char *pb;
} u = { &endian_test };

// adding zero below to work around an edg 2.44 bug (the compiler
// can't explicitly convert a bool expression to an enumeration)
static const endian e = endian (0 + !!*u.pb);

/**************************************************************************/

// print out `size' bytes starting at data prefixed by `text'
static void print (const char *text, const void *data, unsigned size)
{
    printf ("%s { ", text);

    const unsigned char *p = (const unsigned char*)data;

    for (unsigned i = 0; i != size; ++i) {
        if (p[i])
            printf ("'\\x%x'", p[i]);
        else
            printf ("%d", p[i]);
        if (i < size - 1)
            printf (", ");
    }

    printf (" }\n");
}

/**************************************************************************/

union flt_bits
{
    char bits [sizeof (float)];
    float val;
    
    flt_bits () { memset (bits, 0, sizeof (bits)); }
};

static float flt_zero ()
{
    static float val = 0.0f;

    return val;
}

static flt_bits flt_infinity ()
{
    flt_bits val;
    
#ifdef _RWSTD_NO_DBL_TRAPS

    val.val = 1.0f / flt_zero ();

    // may need to negate to get a correct result
    // i86 FP expressions appear to return a negative infinity
    if (val.val < flt_zero ())
        val.val = -val.val;

#else   // if !defined (_RWSTD_NO_DBL_TRAPS)

    // floating point arithmetic traps

    // assume IEEE 754 floating point format

    if (e == big_endian) {
        val.bits [0] = '\x7f';
        val.bits [1] = '\x80';
        val.bits [2] = '\0';
        val.bits [3] = '\0';
    }
    else {
        val.bits [0] = '\0';
        val.bits [1] = '\0';
        val.bits [2] = '\x80';
        val.bits [3] = '\x7f';
    }

#endif   // _RWSTD_NO_DBL_TRAPS

    return val;
}

static flt_bits flt_qnan ()
{
    flt_bits val;

#ifdef _RWSTD_NO_DBL_TRAPS

    val.val = flt_zero ();

    val.val /= flt_zero ();

    // val.val can be +qnan or -qnan
    // clear sign bit
    if (e == big_endian) {
        val.bits [0] &= '\x7f';
    }
    else {
        val.bits [sizeof (val.val) - 1] &= '\x7f';
    }

#else   // if !defined (_RWSTD_NO_DBL_TRAPS)

    // floating point arithmetic traps

    // assume IEEE 754 floating point format

    if (e == big_endian) {
        val.bits [0] = '\x7f';
        val.bits [1] = '\xc0';
        val.bits [2] = '\0';
        val.bits [3] = '\0';
    }
    else {
        val.bits [0] = '\0';
        val.bits [1] = '\0';
        val.bits [2] = '\xc0';
        val.bits [3] = '\x7f';
    }

#endif   // _RWSTD_NO_DBL_TRAPS

    return val;
}

static flt_bits flt_snan ()
{
    flt_bits val;

#ifdef _RWSTD_NO_DBL_TRAPS

    val = flt_infinity ();

#  if !defined (__hpux)

    // convert infinity into a signaling NAN (toggle any bit in signifcand)
    if (e == little_endian) {
        if (0.0f != val.val)
            val.bits [0] |= 1;
    }
    else {
        if (0.0f != val.val)
            val.bits [sizeof (val.val) - 1] |= 1;
    }

#  else   // if defined (__hpux)

    // QNAN and SNAN on HP-UX are reversed...

    if (e == little_endian) {
        if (0.0f != val.val)
            val.bits [1] = '\xc0';
    }
    else {
        if (0.0f != val.val)
            val.bits [sizeof val.val - 2] = '\xc0';
    }

#  endif   // __hpux

#else   // if !defined (_RWSTD_NO_DBL_TRAPS)

    // floating point arithmetic traps

    // assume IEEE 754 floating point format

    if (e == big_endian) {
        val.bits [0] = '\x7f';
        val.bits [1] = '\x80';
        val.bits [2] = '\0';
        val.bits [3] = '\x01';
    }
    else {
        val.bits [0] = '\x01';
        val.bits [1] = '\0';
        val.bits [2] = '\x80';
        val.bits [3] = '\x7f';
    }

#endif   // _RWSTD_NO_DBL_TRAPS

    return val;
}

static flt_bits flt_denorm_min ()
{
    flt_bits val;

#ifdef _RWSTD_NO_DBL_TRAPS

    // compute denorm min
    for (volatile float tmp = 1.0f; tmp; tmp /= 2.0f)
        val.val = tmp;

    if (FLT_MIN <= val.val)
        val.val = 0.0f;

#else

    // assume IEEE 754 floating point format

    if (e == big_endian) {
        val.bits [0] = '\0';
        val.bits [1] = '\0';
        val.bits [2] = '\0';
        val.bits [3] = '\x01';
    }
    else {
        val.bits [0] = '\x01';
        val.bits [1] = '\0';
        val.bits [2] = '\0';
        val.bits [3] = '\0';
    }

#endif   // _RWSTD_NO_DBL_TRAPS

    return val;
}

/**************************************************************************/

union dbl_bits
{
    char bits [sizeof (double)];
    double val;
    
    dbl_bits () { memset (bits, 0, sizeof (bits)); }
};

static float dbl_zero ()
{
    static double val = 0.0;

    return val;
}

static dbl_bits dbl_infinity ()
{
    dbl_bits val;
    
#ifdef _RWSTD_NO_DBL_TRAPS

    val.val = 1.0 / dbl_zero ();

    // may need to negate to get a correct result
    // i86 FP expressions appear to return a negative infinity
    if (val.val < dbl_zero ())
        val.val = -val.val;

#else   // if !defined (_RWSTD_NO_DBL_TRAPS)

    // floating point arithmetic traps

    // assume IEEE 754 floating point format

    if (e == big_endian) {
        val.bits [0] = '\x7f';
        val.bits [1] = '\xf0';
        val.bits [2] = '\0';
        val.bits [3] = '\0';
        val.bits [4] = '\0';
        val.bits [5] = '\0';
        val.bits [6] = '\0';
        val.bits [7] = '\0';
    }
    else {
        val.bits [0] = '\0';
        val.bits [1] = '\0';
        val.bits [2] = '\0';
        val.bits [3] = '\0';
        val.bits [4] = '\0';
        val.bits [5] = '\0';
        val.bits [6] = '\xf0';
        val.bits [7] = '\x7f';
    }

#endif   // _RWSTD_NO_DBL_TRAPS

    return val;
}

static dbl_bits dbl_qnan ()
{
    dbl_bits val;

#ifdef _RWSTD_NO_DBL_TRAPS

    val.val = dbl_zero ();

    val.val /= dbl_zero ();

    // val.val can be +qnan or -qnan
    // clear sign bit
    if (e == big_endian) {
        val.bits [0] &= '\x7f';
    }
    else {
        val.bits [sizeof (val.val) - 1] &= '\x7f';
    }

#else   // if !defined (_RWSTD_NO_DBL_TRAPS)

    // floating point arithmetic traps

    // assume IEEE 754 floating point format

    if (e == big_endian) {
        val.bits [0] = '\x7f';
        val.bits [1] = '\xf8';
        val.bits [2] = '\0';
        val.bits [3] = '\0';
        val.bits [4] = '\0';
        val.bits [5] = '\0';
        val.bits [6] = '\0';
        val.bits [7] = '\0';
    }
    else {
        val.bits [0] = '\0';
        val.bits [1] = '\0';
        val.bits [2] = '\0';
        val.bits [3] = '\0';
        val.bits [4] = '\0';
        val.bits [5] = '\0';
        val.bits [6] = '\xf8';
        val.bits [7] = '\x7f';
    }

#endif   // _RWSTD_NO_DBL_TRAPS

    return val;
}

static dbl_bits dbl_snan ()
{
    dbl_bits val;

#ifdef _RWSTD_NO_DBL_TRAPS

    val = dbl_infinity ();

#  if !defined (__hpux)

    // convert infinity into a signaling NAN (toggle any bit in signifcand)
    if (e == little_endian) {
        if (0.0 != val.val)
            val.bits [0] |= 1;
    }
    else {
        if (0.0 != val.val)
            val.bits [sizeof val.val - 1] |= 1;
    }

#  else   // if defined (__hpux)

    // QNAN and SNAN on HP-UX are reversed...

    if (e == little_endian) {
        if (0.0 != val.val)
            val.bits [1] = '\xf8';
    }
    else {
        if (0.0 != val.val)
            val.bits [sizeof val.val - 2] = '\xf8';
    }

#  endif   // __hpux

#else   // if !defined (_RWSTD_NO_DBL_TRAPS)

    // floating point arithmetic traps

    // assume IEEE 754 floating point format

    if (e == big_endian) {
        val.bits [0] = '\x7f';
        val.bits [1] = '\xf0';
        val.bits [2] = '\0';
        val.bits [3] = '\0';
        val.bits [4] = '\0';
        val.bits [5] = '\0';
        val.bits [6] = '\0';
        val.bits [7] = '\x01';
    }
    else {
        val.bits [0] = '\x01';
        val.bits [1] = '\0';
        val.bits [2] = '\0';
        val.bits [3] = '\0';
        val.bits [4] = '\0';
        val.bits [5] = '\0';
        val.bits [6] = '\xf0';
        val.bits [7] = '\x7f';
    }

#endif   // _RWSTD_NO_DBL_TRAPS

    return val;
}

static dbl_bits dbl_denorm_min ()
{
    dbl_bits val;

#ifdef _RWSTD_NO_DBL_TRAPS

    // compute denorm min
    for (volatile double tmp = 1.0; tmp; tmp /= 2.0)
        val.val = tmp;

    if (DBL_MIN <= val.val)
        val.val = 0.0;

#else

    // assume IEEE 754 floating point format

    if (e == big_endian) {
        val.bits [0] = '\0';
        val.bits [1] = '\0';
        val.bits [2] = '\0';
        val.bits [3] = '\0';
        val.bits [4] = '\0';
        val.bits [5] = '\0';
        val.bits [6] = '\0';
        val.bits [7] = '\x01';
    }
    else {
        val.bits [0] = '\x01';
        val.bits [1] = '\0';
        val.bits [2] = '\0';
        val.bits [3] = '\0';
        val.bits [4] = '\0';
        val.bits [5] = '\0';
        val.bits [6] = '\0';
        val.bits [7] = '\0';
    }

#endif   // _RWSTD_NO_DBL_TRAPS

    return val;
}

/**************************************************************************/

#ifndef _RWSTD_NO_LONG_DOUBLE

union ldbl_bits
{
    char bits [sizeof (long double)];
    long double val;
    
    ldbl_bits () { memset (bits, 0, sizeof (bits)); }
};

static long double ldbl_zero ()
{
    static long double val = 0.0L;

    return val;
}

static ldbl_bits ldbl_infinity ()
{
    ldbl_bits val;
    
#ifdef _RWSTD_NO_DBL_TRAPS

    val.val = 1.0L / ldbl_zero ();

    // may need to negate to get a correct result
    // i86 FP expressions appear to return a negative infinity
    if (val.val < ldbl_zero ())
        val.val = -val.val;

#else   // if !defined (_RWSTD_NO_DBL_TRAPS)

    // floating point arithmetic traps

    // assume IEEE 754 floating point format

    if (e == big_endian) {
        val.bits [ 0] = '\x7f';
        val.bits [ 1] = '\xff';
        val.bits [ 2] = '\0';
        val.bits [ 3] = '\0';
        val.bits [ 4] = '\0';
        val.bits [ 5] = '\0';
        val.bits [ 6] = '\0';
        val.bits [ 7] = '\0';
        val.bits [ 8] = '\0';
        val.bits [ 9] = '\0';
        val.bits [10] = '\0';
        val.bits [11] = '\0';
        val.bits [12] = '\0';
        val.bits [13] = '\0';
        val.bits [14] = '\0';
        val.bits [15] = '\0';
    }
    else {
        val.bits [ 0] = '\0';
        val.bits [ 1] = '\0';
        val.bits [ 2] = '\0';
        val.bits [ 3] = '\0';
        val.bits [ 4] = '\0';
        val.bits [ 5] = '\0';
        val.bits [ 6] = '\0';
        val.bits [ 7] = '\0';
        val.bits [ 8] = '\0';
        val.bits [ 9] = '\0';
        val.bits [10] = '\0';
        val.bits [11] = '\0';
        val.bits [12] = '\0';
        val.bits [13] = '\0';
        val.bits [14] = '\xff';
        val.bits [15] = '\x7f';
    }

#endif   // _RWSTD_NO_DBL_TRAPS

    return val;
}

static ldbl_bits ldbl_qnan ()
{
    ldbl_bits val;

#ifdef _RWSTD_NO_DBL_TRAPS

    val.val = ldbl_zero ();

    val.val /= ldbl_zero ();

    // val.val can be +qnan or -qnan
    // clear sign bit
    if (e == big_endian) {
        val.bits [0] &= '\x7f';
    }
    else {
        unsigned inx = sizeof (val.val) - 1;
        // skip the trailing zero's
        while (!val.bits [inx]) --inx;
        val.bits [inx] &= '\x7f';
    }

#else   // if !defined (_RWSTD_NO_DBL_TRAPS)

    // floating point arithmetic traps

    // assume IEEE 754 floating point format

    if (e == big_endian) {
        val.bits [ 0] = '\x7f';
        val.bits [ 1] = '\xff';
        val.bits [ 2] = '\x80';
        val.bits [ 3] = '\0';
        val.bits [ 4] = '\0';
        val.bits [ 5] = '\0';
        val.bits [ 6] = '\0';
        val.bits [ 7] = '\0';
        val.bits [ 8] = '\0';
        val.bits [ 9] = '\0';
        val.bits [10] = '\0';
        val.bits [11] = '\0';
        val.bits [12] = '\0';
        val.bits [13] = '\0';
        val.bits [14] = '\0';
        val.bits [15] = '\0';
    }
    else {
        val.bits [ 0] = '\0';
        val.bits [ 1] = '\0';
        val.bits [ 2] = '\0';
        val.bits [ 3] = '\0';
        val.bits [ 4] = '\0';
        val.bits [ 5] = '\0';
        val.bits [ 6] = '\0';
        val.bits [ 7] = '\0';
        val.bits [ 8] = '\0';
        val.bits [ 9] = '\0';
        val.bits [10] = '\0';
        val.bits [11] = '\0';
        val.bits [12] = '\0';
        val.bits [13] = '\x80';
        val.bits [14] = '\xff';
        val.bits [15] = '\x7f';
    }

#endif   // _RWSTD_NO_DBL_TRAPS

    return val;
}

static ldbl_bits ldbl_snan ()
{
    ldbl_bits val;

#ifdef _RWSTD_NO_DBL_TRAPS

    val = ldbl_infinity ();

#  if !defined (__hpux)

    // convert infinity into a signaling NAN (toggle any bit in signifcand)
    if (e == little_endian) {
        if (0.0L != val.val)
            val.bits [0] |= 1;
    }
    else {
        if (0.0L != val.val)
            val.bits [sizeof val.val - 1] |= 1;
    }

#  else   // if defined (__hpux)

    // QNAN and SNAN on HP-UX are reversed...
    if (e == little_endian) {
        if (0.0L != val.val)
            val.bits [2] = '\x80';
    }
    else {
        if (0.0L != val.val)
            val.bits [sizeof val.val - 3] = '\x80';
    }

#  endif   // __hpux

#else   // if !defined (_RWSTD_NO_DBL_TRAPS)

    // floating point arithmetic traps

    // assume IEEE 754 floating point format

    if (e == big_endian) {
        val.bits [ 0] = '\x7f';
        val.bits [ 1] = '\xff';
        val.bits [ 2] = '\0';
        val.bits [ 3] = '\0';
        val.bits [ 4] = '\0';
        val.bits [ 5] = '\0';
        val.bits [ 6] = '\0';
        val.bits [ 7] = '\0';
        val.bits [ 8] = '\0';
        val.bits [ 9] = '\0';
        val.bits [10] = '\0';
        val.bits [11] = '\0';
        val.bits [12] = '\0';
        val.bits [13] = '\0';
        val.bits [14] = '\0';
        val.bits [15] = '\x01';
    }
    else {
        val.bits [ 0] = '\x01';
        val.bits [ 1] = '\0';
        val.bits [ 2] = '\0';
        val.bits [ 3] = '\0';
        val.bits [ 4] = '\0';
        val.bits [ 5] = '\0';
        val.bits [ 6] = '\0';
        val.bits [ 7] = '\0';
        val.bits [ 8] = '\0';
        val.bits [ 9] = '\0';
        val.bits [10] = '\0';
        val.bits [11] = '\0';
        val.bits [12] = '\0';
        val.bits [13] = '\0';
        val.bits [14] = '\xff';
        val.bits [15] = '\x7f';
    }

#endif   // _RWSTD_NO_DBL_TRAPS

    return val;
}

static ldbl_bits ldbl_denorm_min ()
{
    ldbl_bits val;

#ifdef _RWSTD_NO_DBL_TRAPS

    // compute denorm min
    for (volatile long double tmp = 1.0L; tmp; tmp /= 2.0L)
        val.val = tmp;

    if (DBL_MIN < val.val)
        val.val = DBL_MIN;

#else

    // assume IEEE 754 floating point format

    if (e == big_endian) {
        val.bits [ 0] = '\0';
        val.bits [ 1] = '\0';
        val.bits [ 2] = '\0';
        val.bits [ 3] = '\0';
        val.bits [ 4] = '\0';
        val.bits [ 5] = '\0';
        val.bits [ 6] = '\0';
        val.bits [ 7] = '\0';
        val.bits [ 8] = '\0';
        val.bits [ 9] = '\0';
        val.bits [10] = '\0';
        val.bits [11] = '\0';
        val.bits [12] = '\0';
        val.bits [13] = '\0';
        val.bits [14] = '\0';
        val.bits [15] = '\x01';
    }
    else {
        val.bits [ 0] = '\x01';
        val.bits [ 1] = '\0';
        val.bits [ 2] = '\0';
        val.bits [ 3] = '\0';
        val.bits [ 4] = '\0';
        val.bits [ 5] = '\0';
        val.bits [ 6] = '\0';
        val.bits [ 7] = '\0';
        val.bits [ 8] = '\0';
        val.bits [ 9] = '\0';
        val.bits [10] = '\0';
        val.bits [11] = '\0';
        val.bits [12] = '\0';
        val.bits [13] = '\0';
        val.bits [14] = '\0';
        val.bits [15] = '\0';
    }

#endif   // _RWSTD_NO_DBL_TRAPS

    return val;
}

#endif   // _RWSTD_NO_LONG_DOUBLE

/**************************************************************************/

int main ()
{
    const flt_bits f_inf  = flt_infinity ();
    const flt_bits f_qnan = flt_qnan ();
    const flt_bits f_snan = flt_snan ();
    const flt_bits f_den  = flt_denorm_min ();

    const dbl_bits d_inf  = dbl_infinity ();
    const dbl_bits d_qnan = dbl_qnan ();
    const dbl_bits d_snan = dbl_snan ();
    const dbl_bits d_den  = dbl_denorm_min ();

#ifndef _RWSTD_NO_LONG_DOUBLE

    const ldbl_bits l_inf  = ldbl_infinity ();
    const ldbl_bits l_qnan = ldbl_qnan ();
    const ldbl_bits l_snan = ldbl_snan ();
    const ldbl_bits l_den  = ldbl_denorm_min ();

#endif   // _RWSTD_NO_LONG_DOUBLE


#ifdef _RWSTD_NO_DBL_TRAPS

    printf ("// computed infinities and NANs for a %s endian architecture\n",
            e == big_endian ? "big" : "little");

    const int flt_has_denorm = 0.0f != f_den.val;
    const int dbl_has_denorm = 0.0  != d_den.val;

#  ifndef _RWSTD_NO_LONG_DOUBLE

    const int ldbl_has_denorm = 0.0L != l_den.val;

#  endif   // _RWSTD_NO_LONG_DOUBLE

#else   // if !defined (_RWSTD_NO_DBL_TRAPS)

    printf ("// IEEE 754 infinities and NANs for a %s endian architecture\n"
            "// (values not computed due to floating exception trapping)\n",
            e == big_endian ? "big" : "little");

    const int flt_has_denorm  = 1;
    const int dbl_has_denorm  = 1;
    const int ldbl_has_denorm = 1;

#endif   // _RWSTD_NO_DBL_TRAPS

    print ("#define _RWSTD_FLT_INF_BITS ", &f_inf, sizeof f_inf);
    print ("#define _RWSTD_FLT_QNAN_BITS ", &f_qnan, sizeof f_qnan);
    print ("#define _RWSTD_FLT_SNAN_BITS ", &f_snan, sizeof f_snan);
    print ("#define _RWSTD_FLT_DENORM_MIN_BITS ", &f_den, sizeof f_den);

    printf ("#define _RWSTD_FLT_HAS_DENORM  %d\n", flt_has_denorm);

    print ("#define _RWSTD_DBL_INF_BITS ", &d_inf, sizeof d_inf);
    print ("#define _RWSTD_DBL_QNAN_BITS ", &d_qnan, sizeof d_qnan);
    print ("#define _RWSTD_DBL_SNAN_BITS ", &d_snan, sizeof d_snan);
    print ("#define _RWSTD_DBL_DENORM_MIN_BITS ", &d_den, sizeof d_den);

    printf ("#define _RWSTD_DBL_HAS_DENORM  %d\n", dbl_has_denorm);

#ifndef _RWSTD_NO_LONG_DOUBLE

    print ("#define _RWSTD_LDBL_INF_BITS ", &l_inf, sizeof l_inf);
    print ("#define _RWSTD_LDBL_QNAN_BITS ", &l_qnan, sizeof l_qnan);
    print ("#define _RWSTD_LDBL_SNAN_BITS ", &l_snan, sizeof l_snan);
    print ("#define _RWSTD_LDBL_DENORM_MIN_BITS ", &l_den, sizeof l_den);

    printf ("#define _RWSTD_LDBL_HAS_DENORM  %d\n", ldbl_has_denorm);

#endif   // _RWSTD_NO_LONG_DOUBLE

    return 0;
}
