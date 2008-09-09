/************************************************************************
 *
 * 18.c.limits.stdcxx-988.cpp - regression test for STDCXX-988
 *
 * http://issues.apache.org/jira/browse/STDCXX-988
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
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
 **************************************************************************/

#include <cassert>
#include <limits.h>


int main ()
{
#if    (defined __unix__ || defined __unix || defined unix) \
    && !defined _RWSTD_EDG_ECCP

    // According to POSIX (SUSv3), the following macros are required
    // to be defined in <limits.h> (others, such as OPEN_MAX) are
    // optional):
    // http://www.opengroup.org/onlinepubs/009695399/basedefs/limits.h.html

    // Runtime Increasable Values

    // The magnitude limitations in the following list shall be fixed
    // by specific implementations. An application should assume that
    // the value supplied by <limits.h> in a specific implementation
    // is the minimum that pertains whenever the application is run
    // under that implementation. A specific instance of a specific
    // implementation may increase the value relative to that supplied
    // by <limits.h> for that implementation. The actual value supported
    // by a specific instance shall be provided by the sysconf() function.

    assert (BC_BASE_MAX        != 0 /* >= _POSIX2_BC_BASE_MAX */ );
    assert (BC_DIM_MAX         != 0 /* >= _POSIX2_BC_DIM_MAX */ );
    assert (BC_SCALE_MAX       != 0 /* >= _POSIX2_BC_SCALE_MAX */ );
    assert (BC_STRING_MAX      != 0 /* >= _POSIX2_BC_STRING_MAX */ );
    assert (CHARCLASS_NAME_MAX != 0 /* >= _POSIX2_CHARCLASS_NAME_MAX */ );
    assert (COLL_WEIGHTS_MAX   != 0 /* >= _POSIX2_COLL_WEIGHTS_MAX */ );
    assert (EXPR_NEST_MAX      != 0 /* >= _POSIX2_EXPR_NEST_MAX */ );
    assert (LINE_MAX           != 0 /* >= _POSIX2_LINE_MAX */ );
    assert (NGROUPS_MAX        != 0 /* >= _POSIX_NGROUPS_MAX */ );
    assert (RE_DUP_MAX         != 0 /* >= _POSIX2_RE_DUP_MAX */ );

    // Minimum Values

    // The symbolic constants in the following list shall be defined
    // in <limits.h> with the values shown. These are symbolic names
    // for the most restrictive value for certain features on an
    // implementation conforming to this volume of IEEE Std 1003.1-2001.
    // Related symbolic constants are defined elsewhere in this volume
    // of IEEE Std 1003.1-2001 which reflect the actual implementation
    // and which need not be as restrictive. A conforming implementation
    // shall provide values at least this large. A strictly conforming
    // application must not require a larger value for correct operation.

    // avoid testing specific values since they're not important here
    // all we care is that the macros be defined (i.e., that thet are
    // not somehow hidden or #undef'd by our implementation)

    assert (_POSIX_ARG_MAX        != 0 /* ==  4096 */);
    assert (_POSIX_CHILD_MAX      != 0 /* ==    25 */);
    assert (_POSIX_HOST_NAME_MAX  != 0 /* ==   255 */);
    assert (_POSIX_LINK_MAX       != 0 /* ==     8 */);
    assert (_POSIX_LOGIN_NAME_MAX != 0 /* ==     9 */);
    assert (_POSIX_MAX_CANON      != 0 /* ==   255 */);
    assert (_POSIX_MAX_INPUT      != 0 /* ==   255 */);
    assert (_POSIX_NAME_MAX       != 0 /* ==    14 */);
    assert (_POSIX_NGROUPS_MAX    != 0 /* ==     8 */);
    assert (_POSIX_OPEN_MAX       != 0 /* ==    20 */);
    assert (_POSIX_PATH_MAX       != 0 /* ==   256 */);
    assert (_POSIX_PIPE_BUF       != 0 /* ==   512 */);
    assert (_POSIX_RE_DUP_MAX     != 0 /* ==   255 */);
    assert (_POSIX_SSIZE_MAX      != 0 /* == 32767 */);
    assert (_POSIX_STREAM_MAX     != 0 /* ==     8 */);
    assert (_POSIX_SYMLINK_MAX    != 0 /* ==   255 */);
    assert (_POSIX_SYMLOOP_MAX    != 0 /* ==     8 */);
    assert (_POSIX_TTY_NAME_MAX   != 0 /* ==     9 */);
    assert (_POSIX_TZNAME_MAX     != 0 /* ==     6 */);

#  ifdef _POSIX2_VERSION

    assert (_POSIX2_BC_BASE_MAX        != 0 /* ==   99 */);
    assert (_POSIX2_BC_DIM_MAX         != 0 /* == 2048 */);
    assert (_POSIX2_BC_SCALE_MAX       != 0 /* ==   99 */);
    assert (_POSIX2_BC_STRING_MAX      != 0 /* == 1000 */);
    assert (_POSIX2_CHARCLASS_NAME_MAX != 0 /* ==   14 */);
    assert (_POSIX2_COLL_WEIGHTS_MAX   != 0 /* ==    2 */);
    assert (_POSIX2_EXPR_NEST_MAX      != 0 /* ==   32 */);
    assert (_POSIX2_LINE_MAX           != 0 /* == 2048 */);
    assert (_POSIX2_RE_DUP_MAX         != 0 /* ==  255 */);

#  endif   // _POSIX2_VERSION

#endif   // UNIX && !EDG eccp

    // Numerical Limits

    // The values in the following lists shall be defined in <limits.h>
    // and are constant expressions suitable for use in #if preprocessing
    // directives. Moreover, except for {CHAR_BIT}, {DBL_DIG}, {DBL_MAX},
    // {FLT_DIG}, {FLT_MAX}, {LONG_BIT}, {WORD_BIT}, and {MB_LEN_MAX},
    // the symbolic names are defined as expressions of the correct type.

    // If the value of an object of type char is treated as a signed
    // integer when used in an expression, the value of {CHAR_MIN} is
    // the same as that of {SCHAR_MIN} and the value of {CHAR_MAX} is
    // the same as that of {SCHAR_MAX}. Otherwise, the value of
    // {CHAR_MIN} is 0 and the value of {CHAR_MAX} is the same as that
    // of {UCHAR_MAX}.

    assert (CHAR_BIT   == 8);
    assert (CHAR_MAX   == UCHAR_MAX || CHAR_MAX == SCHAR_MAX);
    assert (CHAR_MIN   == SCHAR_MIN || CHAR_MIN == 0);
    assert (INT_MAX    >= 2147483647);
    assert (LONG_MAX   >= +2147483647);
    assert (MB_LEN_MAX >= 1);
    assert (SCHAR_MAX  == +127);
    assert (SHRT_MAX   >= +32767);

#if !defined (_RWSTD_EDG_ECCP) && !defined (_MSC_VER)

    // avoid exercising with the EDG eccp front end (strict mode)
    assert (LONG_BIT   >= 32);
    assert (SSIZE_MAX  >= _POSIX_SSIZE_MAX);
    assert (WORD_BIT   >= 32);

#endif   // EDG eccp || MSVC || ICC/Windows

    assert (UCHAR_MAX  >= 255);
    assert (UINT_MAX   >= 4294967295);
    assert (ULONG_MAX  >= 4294967295);
    assert (USHRT_MAX  >= 65535);
    assert (INT_MIN    <= -2147483647);
    assert (LONG_MIN   != 0);
    assert (SCHAR_MIN  == -128);
    assert (SHRT_MIN   != 0);

#ifndef _RWSTD_NO_LONG_LONG

#  if !defined __HP_aCC || 199901L <= __STDC_VERSION__

    // HP aCC defines LLONG_MAX and LLONG_MIN only in C99 mode
    // i.e., with -AC99

    assert (LLONG_MIN  != 0);
    assert (LLONG_MAX  >= +9223372036854775807LL);
    assert (ULLONG_MAX >= 18446744073709551615ULL);

#  endif

#endif   // _RWSTD_NO_LONG_LONG

    return 0;
}
