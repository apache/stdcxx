/************************************************************************
 *
 * printf.cpp - definitions of the rw_printf family of functions
 *
 * $Id$
 *
 ************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <printf.h>

#include <assert.h>
#include <errno.h>
#include <float.h>
#include <locale.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifndef _WIN32
#  include <dlfcn.h>
#endif

#include <ios>
#include <iostream>
#include <locale>
#include <string>

_RWSTD_NAMESPACE (__rw) {

_RWSTD_EXPORT _RWSTD_SSIZE_T
 __rw_memattr (const void*, size_t, int);

}


/********************************************************************/

struct FmtSpec;

static int
rw_fmtlong (const FmtSpec&, char**, size_t*, long);

#ifdef _RWSTD_LONG_LONG

static int
rw_fmtllong (const FmtSpec&, char**, size_t*, _RWSTD_LONG_LONG);

#endif   // _RWSTD_LONG_LONG

static int
rw_fmtinteger (FmtSpec*, size_t, char**, size_t*, va_list*);

static int
rw_fmtfloating (const FmtSpec&, char**, size_t*, const void*);

_RWSTD_INTERNAL int
rw_fmtptr (const FmtSpec&, char**, size_t*, const void*);

typedef void (*funptr_t)();

static int
rw_fmtfunptr (const FmtSpec&, char**, size_t*, funptr_t);

struct DummyStruct;
typedef void (DummyStruct::*memptr_t)() const;

static int
rw_fmtmemptr (const FmtSpec&, char**, size_t*, memptr_t);

static int
rw_fmtstr (const FmtSpec&, char**, size_t*, const char*, size_t);

static int
rw_fmtchr (const FmtSpec&, char**, size_t*, int);

static int
rw_fmtwchr (const FmtSpec&, char**, size_t*, wint_t);

static int
rw_fmtwstr (const FmtSpec&, char**, size_t*, const wchar_t*, size_t);

static int
rw_fmterrno (const FmtSpec&, char**, size_t*, int);

static int
rw_fmtopenmode (const FmtSpec&, char**, size_t*, int);

static int
rw_fmtevent (const FmtSpec&, char**, size_t*, int);

static int
rw_fmtmonpat (const FmtSpec&, char**, size_t*, const char [4]);

/********************************************************************/

struct FmtSpec
{
    // optional flags
    unsigned fl_minus  : 1;
    unsigned fl_plus   : 1;
    unsigned fl_pound  : 1;
    unsigned fl_space  : 1;
    unsigned fl_zero   : 1;

    // optional modifiers
    unsigned mod_A     : 1;    // extension (Arrays)
    unsigned mod_h     : 1;
    unsigned mod_hh    : 1;
    unsigned mod_l     : 1;
    unsigned mod_ll    : 1;
    unsigned mod_j     : 1;
    unsigned mod_z     : 1;
    unsigned mod_t     : 1;
    unsigned mod_L     : 1;
    unsigned mod_I     : 1;    // extension

    unsigned cond       : 1;   // have an if/else clause
    unsigned cond_true  : 1;   // if/else clause is active (true)
    unsigned cond_begin : 1;   // beginning of an if/else clause
    unsigned cond_end   : 1;   // end of an if/else clause

    // note that the signedness of a bitfield is implementation-defined
    // unless explicitly declared signed or unsigned

    // extension: 8, 16, 32, and 64 bit integer width modifier
    signed int iwidth : 4;

    // extension: optional numerical base 2 - 36
    signed int base   : 7;

    // extension: optional parameter number
    long paramno;

    // optional width and precision
    int width;
    int prec;

    // extension: string argument
    char *strarg;

    // required conversion specifier
    int cvtspec;

    // extension: fill character
    int fill;

    union {

#ifndef _RWSTD_NO_LONG_DOUBLE

        long double      ldbl;

#endif   // _RWSTD_NO_LONG_DOUBLE

#ifdef _RWSTD_LONG_LONG

        _RWSTD_LONG_LONG llong;

#endif   // _RWSTD_LONG_LONG

        void*            ptr;

        int              i;
        long             lng;

        _RWSTD_INT32_T   i32;

#ifdef _RWSTD_INT64_T
        _RWSTD_INT64_T   i64;
#endif   // _RWSTD_INT64_T

        ptrdiff_t        diff;
        size_t           size;
        wint_t           wi;

        double           dbl;
        memptr_t         memptr;
        funptr_t         funptr;
    } param;
};

/********************************************************************/

/*

  C99 format specifier:

  % flags width [ . prec ] mod cvtspec


  GNU glibc format specifier:

  % [ paramno $] flags width [ . prec ] mod cvtspec
  or
  % [ paramno $] flags width . * [ paramno $] mod cvtspec


  Extended format specifier:

  % { [ paramno $] [ flags ] [ width [. prec ]] mod cvtspec }
  or
  % { [ paramno $] [ flags ] [@ base [. width [. prec ]]] mod cvtspec }
  or
  % { $ envvar }


*/

static int
rw_fmtspec (FmtSpec *pspec, bool ext, const char *fmt, va_list *pva)
{
    assert (0 != pspec);
    assert (0 != fmt);
    assert (0 != pva);

    memset (pspec, 0, sizeof *pspec);

    pspec->iwidth  = -1;   // integer width not specified
    pspec->width   = -1;   // width not specified
    pspec->prec    = -1;   // precision not specified
    pspec->base    = -1;   // base not specified
    pspec->paramno = -1;   // paramno not specified

    const char* const fmtbeg = fmt;

    if (ext) {
        if ('$' == *fmt) {

            ++fmt;

            const char *str;
            if ('*' == *fmt) {
                str = va_arg (*pva, char*);
                if (!str)
                    str = "";
                ++fmt;
            }
            else {
                str  = fmt;
                fmt += strlen (fmt);
            }

            char *tmp = (char*)malloc (strlen (str));
            pspec->strarg = strcpy (tmp, str);

            return int (fmt - fmtbeg);
        }
    }

    // extension: extract the parameter number (if followed by '$')
    if ('1' <= *fmt && *fmt <= '9') {
        char *end;
        const long tmp = strtol (fmt, &end, 10);

        fmt = end;

        if ('$' == *fmt) {
            pspec->paramno = tmp;
            ++fmt;
        }
        else {
            // when not followed by '$' interpret the number
            // as the width (i.e., that follows the empty set
            // of flags)
            pspec->width = int (tmp);
        }
    }

    if (-1 == pspec->width) {
        // extract any number of optional flags
        for ( ; ; ++fmt) {
            switch (*fmt) {
            case '-': pspec->fl_minus = true; continue;
            case '+': pspec->fl_plus = true; continue;
            case '#': pspec->fl_pound = true; continue;
            case ' ': pspec->fl_space = true; continue;
            case '0': pspec->fl_zero = true; continue;
            }
            break;
        }
    }

    if (ext && '@' == *fmt) {

        ++fmt;

        // extract the numerical base
        if ('0' <= fmt [1] && fmt [1] <= '9') {
            char *end;
            pspec->base = int (strtol (fmt, &end, 10));
            fmt         = end;
        }
        else if ('*' == *fmt) {
            pspec->base = va_arg (*pva, int);
            ++fmt;
        }

        if ('.' == *fmt)
            ++fmt;
    }

    if (-1 == pspec->width) {
        // extract the optional width
        if ('0' <= *fmt && *fmt <= '9') {
            char *end;
            pspec->width = int (strtol (fmt, &end, 10));
            fmt          = end;
        }
        else if ('*' == *fmt) {
            pspec->width = va_arg (*pva, int);

            if (pspec->width < 0) {
                // 7.19.6.1, p5 of ISO/IEC 9899:1999:
                //   A negative field width argument is taken
                //   as a - flag followed by a positive field width.
                pspec->width    = -pspec->width;
                pspec->fl_minus = true;
            }

            ++fmt;
        }
    }

    // extract the optional precision
    if ('.' == *fmt) {

        ++fmt;

        if ('0' <= *fmt && *fmt <= '9') {
            char *end;
            pspec->prec = int (strtol (fmt, &end, 10));
            fmt         = end;
        }
        else if ('*' == *fmt) {
            pspec->prec = va_arg (*pva, int);
            ++fmt;
        }
    }

    // extract an optional modifier
    switch (*fmt) {
    case 'A':
        if (ext) {
            ++fmt;
            pspec->mod_A = true;
            break;
        }
        // fall thru

    case 'h':
        if ('h' == fmt [1]) {
            ++fmt;
            pspec->mod_hh = true;
        }
        else
            pspec->mod_h = true;
        ++fmt;
        break;

    case 'l':
        if ('l' == fmt [1]) {
            ++fmt;
            pspec->mod_ll = true;
        }
        else
            pspec->mod_l = true;
        ++fmt;
        break;

    case 'j': pspec->mod_j = true; ++fmt; break;
    case 'z': pspec->mod_z = true; ++fmt; break;
    case 't': pspec->mod_t = true; ++fmt; break;
    case 'L': pspec->mod_L = true; ++fmt; break;

    case 'I':
        if (ext) {

            ++fmt;

            if ('8' == *fmt) {
                pspec->iwidth = 1;
                ++fmt;
            }
            else if ('1' == fmt [0] && '6' == fmt [1]) {
                pspec->iwidth = 2;
                fmt += 2;
            }
            else if ('3' == fmt [0] && '2' == fmt [1]) {
                pspec->iwidth = 3;
                fmt += 2;
            }
            else if ('6' == fmt [0] && '4' == fmt [1]) {
                pspec->iwidth = 4;
                fmt += 2;
            }
            else {
                pspec->mod_I = true;
            }
            break;
        }
    }

    pspec->cvtspec = *fmt;

    if (pspec->cvtspec)
        ++fmt;

    return int (fmt - fmtbeg);
}

/********************************************************************/

_RWSTD_INTERNAL char*
rw_bufcat (char **pbuf, size_t *pbufsize, const char *str, size_t len)
{
    assert (0 != pbuf);
    assert (0 != pbufsize);

          size_t buflen = *pbuf ? strlen (*pbuf) : 0;
    const size_t bufree = *pbuf ? *pbufsize - buflen : 0;

    if (bufree <= len || !*pbuf) {

        size_t newbufsize = *pbufsize * 2;

        if (newbufsize <= buflen + len)
            newbufsize = 2 * (buflen + len + 1);

        char* const newbuf = (char*)malloc (newbufsize);

        // return 0 on failure to allocate, let caller deal with it
        if (0 == newbuf)
            return 0;

        memcpy (newbuf, *pbuf, buflen);
        free (*pbuf);

        *pbuf     = newbuf;
        *pbufsize = newbufsize;

        (*pbuf)[buflen] = '\0';
    }

    if (0 != str) {
        memcpy (*pbuf + buflen, str, len);
        buflen += len;
        (*pbuf)[buflen] = '\0';
    }

    return *pbuf + buflen;
}

/********************************************************************/

// rw_asnprintf_cb is called to format a character string according
// to the single format specifier `fmt' to the end of the provided
// buffer `*pbuf'; the function can reallocate the buffer
// returns the number of characters appended to the buffer
extern int
(*rw_vasnprintf_cb)(FmtSpec*, size_t, char**, size_t*, const char*, va_list*);

/********************************************************************/

static int
rw_vasnprintf_c99 (FmtSpec *pspec, size_t paramno,
                   char **pbuf, size_t *pbufsize, va_list *pva)
{
    _RWSTD_UNUSED (paramno);

    _RWSTD_ASSERT (0 != pspec);

    int len = -1;

    FmtSpec &spec = pspec [paramno];

#define PARAM(T, name)   \
  (0 < spec.paramno ? pspec [spec.paramno - 1].param.name : va_arg (*pva, T))

    switch (spec.cvtspec) {

    case 'd':
    case 'i':
    case 'o':
    case 'x':
    case 'X':
    case 'u':
        len = rw_fmtinteger (pspec, paramno, pbuf, pbufsize, pva);
        break;

    case 'e':
    case 'E':
    case 'f':
    case 'F':
    case 'g':
    case 'G':
        if (spec.mod_L) {
            spec.param.ldbl = PARAM (long double, ldbl);
            len = rw_fmtfloating (spec, pbuf, pbufsize, &spec.param.ldbl);
        }
        else {
            spec.param.dbl = PARAM (double, dbl);
            len = rw_fmtfloating (spec, pbuf, pbufsize, &spec.param.dbl);
        }
        break;

    case 'a':
        assert (!"%a not implemented");
        break;

    case 'A':
        assert (!"%A not implemented");
        break;

    case 'c':
        // If no l length modifier is present, the int argument is converted
        // to an unsigned char, and the resulting character is written. If
        // an l length modifier is present, the wint_t argument is converted
        // as if by an ls conversion specification with no precision and an
        // argument that points to the initial element of a two-element array
        // of wchar_t, the first element containing the wint_t argument to
        // the lc conversion specification and the second a null wide
        // character.
        if (spec.mod_l) {
            spec.param.wi = PARAM (wint_t, wi);
            len = rw_fmtwchr (spec, pbuf, pbufsize, spec.param.wi);
        }
        else {
            spec.param.i = PARAM (int, i);
            len = rw_fmtchr (spec, pbuf, pbufsize, spec.param.i);
        }
        break;

    case 's':
        if (spec.mod_l) {
            spec.param.ptr = PARAM (wchar_t*, ptr);
            const wchar_t* const str = (wchar_t*)spec.param.ptr;
            len = rw_fmtwstr (spec, pbuf, pbufsize, str, _RWSTD_SIZE_MAX);
        }
        else {
            spec.param.ptr = PARAM (char*, ptr);
            const char* const str = (char*)spec.param.ptr;
            len = rw_fmtstr (spec, pbuf, pbufsize, str, _RWSTD_SIZE_MAX);
        }
        break;

    case 'p': {
        // The argument shall be a pointer to void. The value of the pointer
        // is converted to a sequence of printing characters, in an
        // implementation-defined manner.
        spec.param.ptr = PARAM (char*, ptr);
        len = rw_fmtptr (spec, pbuf, pbufsize, spec.param.ptr);
        break;
    }

    case 'm': {   // extension
        spec.param.i = errno;
        len = rw_fmterrno (spec, pbuf, pbufsize, spec.param.i);
        break;
    }

    case 'n': {
        // The argument shall be a pointer to signed integer into which
        // is written the number of characters written to the output
        // stream so far by this call to fprintf. No argument is converted,
        // but one is consumed. If the conversion specification includes
        // any flags, a field width, or a precision, the behavior is
        // undefined.

        assert (0 != pbuf);
        assert (0 != *pbuf);

        len = int (strlen (*pbuf));

        spec.param.ptr = PARAM (void*, ptr);

        if (spec.mod_hh) {
            unsigned char* const ptr = (unsigned char*)spec.param.ptr;

            assert (0 != ptr);

            *ptr = len;
        }
        else if (spec.mod_h) {
            short* const ptr = (short*)spec.param.ptr;

            assert (0 != ptr);

            *ptr = len;
        }
        else if (spec.mod_L) {
#ifdef _RWSTD_LONG_LONG
            _RWSTD_LONG_LONG* const ptr = (_RWSTD_LONG_LONG*)spec.param.ptr;

            assert (0 != ptr);

            *ptr = len;
#else   // if !defined (_RWSTD_LONG_LONG)
            assert (!"%Ln not implemented");
#endif   // _RWSTD_LONG_LONG
        }
        else if (spec.mod_l) {
            long* const ptr = (long*)spec.param.ptr;

            assert (0 != ptr);

            *ptr = len;
        }
        else if (spec.mod_t) {
            ptrdiff_t* const ptr = (ptrdiff_t*)spec.param.ptr;

            assert (0 != ptr);

            *ptr = ptrdiff_t (unsigned (len));
        }
        else {
            int* const ptr = (int*)spec.param.ptr;

            assert (0 != ptr);

            *ptr = len;
        }
        break;
    }

    case '%':
        break;
    }

    return len;
}

/********************************************************************/

_TEST_EXPORT int
rw_vasnprintf (char **pbuf, size_t *pbufsize, const char *fmt, va_list varg)
{
    va_list *pva;

#ifdef va_copy
    // create a copy of va whose address can be passed to a function
    // taking a va_list* so that it can modify the original; note that
    // passing &va is not portable since when declared as a function
    // argument, va_list that is an array type decays into a pointer
    // and the address of the pointer will not match va_list* (as is
    // the case on EM64T)
    va_list vacpy;
    va_copy (vacpy, varg);
    pva = &vacpy;

#else   // if !defined (va_copy)

#  if defined (__GNUG__)

    // use the gcc builtin when the va_copy macro is not defined
    // (e.g., with gcc -ansi or Intel C++ icc -ansi or -strict_ansi)
    va_list vacpy;
    __builtin_va_copy (vacpy, varg);
    pva = &vacpy;

#  else   // if !defined (__GNUG__)

    // use varg (in)directly and assume it's safe (e.g., HP aCC on PA)
    pva = &varg;

#  endif   // __GNUG__

#endif   // va_copy

// do not use varg of vacpy below this point -- use *pva instead
#define varg  DONT_TOUCH_ME
#define vacpy DONT_TOUCH_ME

    assert (0 != pbuf);

    // save the initial value of `pbuf'
    char* const pbuf_save = *pbuf;

    // local buffer for a small number of conversion specifiers
    // will grow dynamically if their number exceeds its capacity
    FmtSpec specbuf [32];
    FmtSpec *pspec = specbuf;

    // local buffer for backtrack offsets implementing conditionals
    int backtrack [32];
    int nextoff = 0;

    size_t default_bufsize = 1024;

    if (0 == pbufsize)
        pbufsize = &default_bufsize;

    char fmtspec [64];

    char *next = *pbuf;

    size_t spec_bufsize = sizeof specbuf / sizeof *specbuf;
    size_t paramno = 0;

    for (const char *fc = fmt; *fc; ) {

        const char* const pcnt = strchr (fc, '%');

        size_t nchars = pcnt ? pcnt - fmt : strlen (fc);

        next = rw_bufcat (pbuf, pbufsize, fmt, nchars);
        if (0 == next)
            goto fail;

        assert (0 != *pbuf);
        assert (0 != *pbufsize);

        if (0 == pcnt)
            break;

        fc = pcnt + 1;

        if ('%' == *fc) {
            // handle "%%"
            next = rw_bufcat (pbuf, pbufsize, "%", 1);
            if (0 == next)
                goto fail;

            fmt = ++fc;
            continue;
        }

        if (spec_bufsize == paramno) {
            // grow the buffer of conversion specifiers
            FmtSpec *tmp = (FmtSpec*)malloc (spec_bufsize * 2);
            if (tmp) {
                memcpy (tmp, pspec, spec_bufsize);
                if (pspec != specbuf)
                    free (pspec);
                pspec = tmp;
            }
            else
                goto fail;
        }

        if ('{' == *fc) {
            const char* const endbrace = strchr (++fc, '}');

            assert (0 != endbrace);

            const size_t fmtlen = endbrace - fc;

            memcpy (fmtspec, fc, fmtlen);
            fmtspec [fmtlen] = '\0';

            // compute the length of the buffer so far
            const size_t buflen = next - *pbuf;

            assert (0 != rw_vasnprintf_cb);

            // initiaze the current format specification, setting
            // all unused bits to 0
            const int speclen =
                rw_fmtspec (pspec + paramno, true, fc, pva);

            _RWSTD_UNUSED (speclen);

            // copy the current backtrack offset if one exists
            // and set the condition and condition true bits
            if (nextoff) {

                // set the condition valid bit
                pspec [paramno].cond = 1;

                if (backtrack [nextoff - 1] < 0) {
                    // negative offset indicates an active clause
                    pspec [paramno].cond_true = 1;
                }
                else {
                    // non-negative offset indicates an inactive clause
                    // no-op
                }
            }

            // append formatted string to the end of the buffer
            // reallocating it if necessary; callee may change
            // the specification as necessary (e.g., based on
            // the if/else clause)

            int len =
                rw_vasnprintf_cb (pspec, paramno, pbuf, pbufsize,
                                  fmtspec, pva);

            // the callback returns a negative value on error
            if (len < 0)
                goto fail;

            assert (size_t (len) < *pbufsize);
            assert (strlen (*pbuf) < *pbufsize);

            const size_t offinx = nextoff - 1;

            if (pspec [paramno].cond_end && pspec [paramno].cond_begin) {
                // change from an if to an else clause

                assert (0 < nextoff);
                assert (0 == len);

                if (pspec [paramno].cond_true) {
                    // change from an inactive if to an active else
                    // (same as the end of an inactive clause)

                    assert (0 <= backtrack [offinx]);

                    // set the length so as to backtrack to the position
                    // saved on the top of the backtrack stack 
                    len = -int (buflen) + backtrack [offinx];

                    // invert the offset to indicate an active clause
                    backtrack [offinx] = ~backtrack [offinx];
                }
                else {
                    // change from an active if to an inactive else
                    assert (backtrack [offinx] < 0);

                    // save the current length of the buffer
                    // as the new backtrack offset
                    backtrack [offinx] = int (buflen);
                }
            }
            else if (pspec [paramno].cond_begin) {
                // start of a new if clause

                // push it on the stack of backtracking offsets using
                // negative values to indicate active clauses and
                // non-negative values inactive ones
                if (pspec [paramno].cond_true)
                    backtrack [nextoff++] = ~int (buflen);
                else
                    backtrack [nextoff++] = int (buflen);
            }
            else if (pspec [paramno].cond_end) {
                // the end of an if/else clause

                if (!pspec [paramno].cond_true) {
                    // the end of an inactive clause

                    assert (backtrack [offinx] <= int (buflen));

                    // set the length so as to backtrack to the position
                    // saved on the top of the backtrack stack 
                    len = -int (buflen) + backtrack [offinx];
                }

                // pop it off the top of the stack
                --nextoff;
            }

            assert (len + buflen < *pbufsize);

            // adjust the next pointer to point to the terminating
            // NUL in the (possibly reallocated) buffer
            next  = *pbuf + buflen + len;
            *next = '\0';
            fc    = endbrace + 1;
        }
        else {
            const int speclen =
                rw_fmtspec (pspec + paramno, false, fc, pva);

            if (speclen) {

                const int len =
                    rw_vasnprintf_c99 (pspec, paramno, pbuf, pbufsize, pva);

                if (-1 == len)
                    goto fail;

                // discard positional specifiers
                if (-1 == pspec [paramno].paramno)
                    ++paramno;

                next = rw_bufcat (pbuf, pbufsize, 0, size_t (len));
                if (0 == next)
                    goto fail;

                next += len;
                fc   += speclen;
            }
            else {
                next = rw_bufcat (pbuf, pbufsize, "%", 1);
                if (0 == next)
                    goto fail;
            }
        }

        fmt = fc;
    }

    // deallocate if dynamically allocated
    if (pspec != specbuf)
        free (pspec);

    return int (next - *pbuf);

fail: // function failed

    fprintf (stderr, "%s:%d: rw_vasnprintf(%p, %p, \"%s\", va_list) "
             "error: errno = %d: %s\n",
             __FILE__, __LINE__, (void*)pbuf, (void*)pbufsize, fmt,
             errno, strerror (errno));

    if (pspec != specbuf)
        free (pspec);

    if (*pbuf != pbuf_save) {
        // free any allocated memory
        free (*pbuf);
        *pbuf = 0;
    }

    return -1;

#undef varg
#undef vacpy

}

/********************************************************************/

static const char rw_digits[] = {
    "0123456789abcdefghijklmnopqrstuvwxyz"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
};

static int
rw_fmtlong (const FmtSpec &spec, char **pbuf, size_t *pbufsize, long val)
{
    char buffer [130];   // big enough for a 128-bit long in base 2
    char *end = buffer;

    long upoff = 0;
    const char *pfx = 0;

    if ('X' == spec.cvtspec)
        upoff  = 36;

    if (spec.fl_pound) {
        if (16 == spec.base)
            pfx = upoff ? "0X" : "0x";
        else if (8 == spec.base && val)
            pfx = "0";
    }

    const int base = 1 < spec.base && spec.base < 37 ? spec.base : 10;

    typedef unsigned long ULong;

    ULong uval;

    bool neg;

    if (val < 0) {
        neg  = 'd' == spec.cvtspec || 'i' == spec.cvtspec;
        uval = ULong (neg ? -val : val);
    }
    else {
        neg  = false;
        uval = ULong (val);
    }

    do {
        *end++ = rw_digits [upoff + uval % base];
    } while (uval /= base);

    int size = int (end - buffer);

    // insert as many zeros as specified by precision
    if (-1 < spec.prec && size < spec.prec) {
        // FIXME: prevent buffer overrun
        for (int i = size; i != spec.prec; ++i)
            *end++ = '0';
    }

    // insert octal or hex prefix for non-zero values
    if (pfx && val) {
        if (pfx [1])
            *end++ = pfx [1];
        *end++ = pfx [0];
    }

    if (neg)
        *end++ = '-';
    else if (spec.fl_plus && ('d' == spec.cvtspec || 'i' == spec.cvtspec))
        *end++ = '+';

    if (0 == spec.prec && 0 == val) {
        // 7.19.6.1 of ISO/IEC 9899:1999:
        //   The result of converting a zero value with a precision
        //   of zero is no characters.
        end = buffer;
    }

    *end = '\0';

    size = int (end - buffer);

    for (char *pc = buffer; pc < end; ++pc) {
        const char tmp = *pc;
        *pc = *--end;
        *end = tmp;
    }

    // reset precision to -1 (already handled above)
    FmtSpec newspec (spec);
    newspec.fl_pound = 0;
    newspec.prec = -1;

    // handle justification by formatting the resulting string
    return rw_fmtstr (newspec, pbuf, pbufsize, buffer, size_t (size));
}

/********************************************************************/

#ifdef _RWSTD_LONG_LONG

static int
rw_fmtllong (const FmtSpec &spec,
             char **pbuf, size_t *pbufsize, _RWSTD_LONG_LONG val)
{
    char buffer [130];   // big enough for a 128-bit long long in base 2
    char *end = buffer;

    long upoff = 0;
    const char *pfx = 0;

    if ('X' == spec.cvtspec)
        upoff  = 36;

    if (spec.fl_pound) {
        if (16 == spec.base)
            pfx = upoff ? "0X" : "0x";
        else if (8 == spec.base && val)
            pfx = "0";
    }

    const int base = 1 < spec.base && spec.base < 37 ? spec.base : 10;

    typedef unsigned _RWSTD_LONG_LONG ULLong;

    ULLong uval;

    bool neg;

    if (val < 0) {
        neg  = 'd' == spec.cvtspec || 'i' == spec.cvtspec;
        uval = ULLong (neg ? -val : val);
    }
    else {
        neg  = false;
        uval = ULLong (val);
    }

    do {
        *end++ = rw_digits [upoff + uval % base];
    } while (uval /= base);

    if (pfx) {
        if (pfx [1])
            *end++ = pfx [1];
        *end++ = pfx [0];
    }

    char sign;

    if (neg)
        sign = '-';
    else if (spec.fl_plus && ('d' == spec.cvtspec || 'i' == spec.cvtspec))
        sign= '+';
    else
        sign = '\0';

    assert (buffer < end);
    size_t size = size_t (end - buffer);

    // FIXME: prevent buffer overrun
    if (0 < spec.prec && size < size_t (spec.prec)) {
        for (size_t i = size; i != size_t (spec.prec); ++i)
            *end++ = '0';
    }

    if (sign)
        *end++ = sign;

    *end = '\0';

    assert (buffer < end);
    size = size_t (end - buffer);

    for (char *pc = buffer; pc < end; ++pc) {
        const char tmp = *pc;
        *pc = *--end;
        *end = tmp;
    }

    // handle justification by formatting the resulting string
    return rw_fmtstr (spec, pbuf, pbufsize, buffer, size);
}

#endif   // _RWSTD_LONG_LONG

/********************************************************************/

static int
rw_fmtinteger (FmtSpec *pspec, size_t paramno,
               char **pbuf, size_t *pbufsize, va_list *pva)
{
    int len = -1;

    FmtSpec &spec = pspec [paramno];

    switch (spec.cvtspec) {
    case 'd':
    case 'i':
        if (spec.mod_hh) {
            // promoted signed char argument
            spec.param.i = PARAM (int, i);
            const signed char val = spec.param.i;
            len = rw_fmtlong (spec, pbuf, pbufsize, long (val));
        }
        else if (spec.mod_h) {
            // promoted signed short argument
            spec.param.i = PARAM (int, i);
            const short val = spec.param.i;
            len = rw_fmtlong (spec, pbuf, pbufsize, long (val));
        }
        else if (spec.mod_l) {   // %li
            spec.param.lng = PARAM (long, lng);
            len = rw_fmtlong (spec, pbuf, pbufsize, spec.param.lng);
        }
        else if (spec.mod_ll) {   // %lli

#ifdef _RWSTD_LONG_LONG
            spec.param.llong = PARAM (_RWSTD_LONG_LONG, llong);
            len = rw_fmtllong (spec, pbuf, pbufsize, spec.param.llong);
#elif 8 == _RWSTD_LONG_SIZE
            spec.param.llong = PARAM (long, lnng);
            len = rw_fmtlong (spec, pbuf, pbufsize, spec.param.llong);
#else
            assert (!"%lld, %lli: long long not supported");

#endif   // _RWSTD_LONG_LONG
        }
        else if (spec.mod_t) {
            spec.param.diff = PARAM (ptrdiff_t, diff);
            if (sizeof (ptrdiff_t) == sizeof (long)) {
                len = rw_fmtlong (spec, pbuf, pbufsize, spec.param.diff);
            }
            else {
#ifdef _RWSTD_LONG_LONG
                len = rw_fmtllong (spec, pbuf, pbufsize, spec.param.diff);
#else   // if !defined (_RWSTD_LONG_LONG)
                assert (!"%td, %ti: 64-bit types not supported");
#endif   // _RWSTD_LONG_LONG
            }
        }
        else if (1 == spec.iwidth) {
            spec.param.i = PARAM (int, i);
            const _RWSTD_INT8_T val = spec.param.i;
            len = rw_fmtlong (spec, pbuf, pbufsize, val);
        }
        else if (2 == spec.iwidth) {
            spec.param.i = PARAM (int, i);
            const _RWSTD_INT16_T val = spec.param.i;
            len = rw_fmtlong (spec, pbuf, pbufsize, val);
        }
        else if (3 == spec.iwidth) {
            spec.param.i32 = PARAM (_RWSTD_INT32_T, i32);
            const long val = long (spec.param.i32);
            len = rw_fmtlong (spec, pbuf, pbufsize, val);
        }
        else if (4 == spec.iwidth) {

#ifdef _RWSTD_INT64_T
            spec.param.i64 = PARAM (_RWSTD_INT64_T, i64);
#else   // if !defined (_RWSTD_INT64_T)
            assert (!"%I64d, %I64i: 64-bit types not supported");
#endif   // _RWSTD_INT64_T

#if 8 == _RWSTD_LONG_SIZE
            const long val = spec.param.i64;
            len = rw_fmtlong (spec, pbuf, pbufsize, val);
#elif defined (_RWSTD_LONG_LONG)
            const _RWSTD_LONG_LONG val = spec.param.i64;
            len = rw_fmtllong (spec, pbuf, pbufsize, val);
#else
            assert (!"%I64d, %I64i: 64-bit types not supported");
#endif
        }
        else {   // %i
            spec.param.i = PARAM (int, i);
            len = rw_fmtlong (spec, pbuf, pbufsize, long (spec.param.i));
        }
        break;

    case 'o':
        assert (-1 == spec.base);
        spec.base = 8;
        // fall thru

    case 'x':
        if (-1 == spec.base)
            spec.base = 16;
        // fall thru

    case 'X':
        if (-1 == spec.base)
            spec.base = 16;

    case 'u':
        if (spec.mod_hh) {
            // promoted unsigned char argument
            spec.param.i = PARAM (unsigned, i);
            const unsigned char val = spec.param.i;
            len = rw_fmtlong (spec, pbuf, pbufsize, (unsigned long)val);
        }
        else if (spec.mod_h) {
            // promoted unsigned short argument
            spec.param.i = PARAM (unsigned, i);
            const unsigned short val = spec.param.i;
            len = rw_fmtlong (spec, pbuf, pbufsize, (unsigned long)val);
        }
        else if (spec.mod_ll) {
#ifdef _RWSTD_LONG_LONG
            spec.param.llong = PARAM (unsigned _RWSTD_LONG_LONG, llong);
            const unsigned _RWSTD_LONG_LONG val = spec.param.llong;
            len = rw_fmtllong (spec, pbuf, pbufsize, val);
#elif 8 == _RWSTD_LONG_SIZE
            spec.param.lng = PARAM (unsigned long, lng);
            const unsigned long val = spec.param.lng;
            len = rw_fmtlong (spec, pbuf, pbufsize, val);
#else
            assert (!"long long not supported");
#endif   // _RWSTD_LONG_LONG

        }
        else if (spec.mod_l) {
            spec.param.lng = PARAM (unsigned long, lng);
            const unsigned long val = spec.param.lng;
            len = rw_fmtlong (spec, pbuf, pbufsize, val);
        }
        else if (spec.mod_t) {
            spec.param.lng = PARAM (size_t, size);
            if (sizeof (size_t) == sizeof (unsigned long)) {
                len = rw_fmtlong (spec, pbuf, pbufsize, spec.param.size);
            }
            else {
#ifdef _RWSTD_LONG_LONG
                len = rw_fmtllong (spec, pbuf, pbufsize, spec.param.size);
#else   // if defined (_RWSTD_LONG_LONG)
                assert (!"%to, %tu, %tx: 64-bit types not implemented");
#endif   // _RWSTD_LONG_LONG
            }
        }
        else if (1 == spec.iwidth) {
            spec.param.i = PARAM (int, i);
            const _RWSTD_UINT8_T val = spec.param.i;
            len = rw_fmtlong (spec, pbuf, pbufsize, val);
        }
        else if (2 == spec.iwidth) {
            spec.param.i = PARAM (int, i);
            const long val = (unsigned short)spec.param.i;
            len = rw_fmtlong (spec, pbuf, pbufsize, val);
        }
        else if (3 == spec.iwidth) {
            spec.param.i32 = PARAM (_RWSTD_INT32_T, i32);
            const long val = long (unsigned (spec.param.i));
            len = rw_fmtlong (spec, pbuf, pbufsize, val);
        }
        else if (4 == spec.iwidth) {
#ifdef _RWSTD_INT64_T
            spec.param.i64 = PARAM (_RWSTD_INT64_T, i64);
#else   // if defined 9_RWSTD_INT64_T)
            assert (!"%I64o, %I64u, %I64x: 64-bit types not supported");
#endif   // _RWSTD_INT64_T

#if 8 == _RWSTD_LONG_SIZE
            const unsigned long val = spec.param.i64;
            len = rw_fmtlong (spec, pbuf, pbufsize, val);
#elif defined (_RWSTD_LONG_LONG)
            const unsigned _RWSTD_LONG_LONG val = spec.param.i64;
            len = rw_fmtllong (spec, pbuf, pbufsize, val);
#else
            assert (!"%I64o, %I64u, %I64x: 64-bit types not supported");
#endif
        }
        else {
            spec.param.i = PARAM (unsigned, i);
            const unsigned val = spec.param.i;
            len = rw_fmtlong (spec, pbuf, pbufsize, long (val));
        }
        
        break;
    }

    return len;
}

/********************************************************************/

static int
rw_fmtfloating (const FmtSpec &spec,
                char **pbuf, size_t *pbufsize, const void *pval)
{
    char fmt [128];
    char *pf = fmt;

    *pf++ = '%';

    if (spec.fl_minus)
        *pf++ = '-';

    if (spec.fl_plus)
        *pf++ = '+';
        
    if (spec.fl_pound)
        *pf++ = '#';

    if (spec.fl_space)
        *pf++ = ' ';

    if (spec.fl_zero)
        *pf++ = '0';

    if (spec.mod_h)
        *pf++ = 'h';
    else if (spec.mod_hh) {
        *pf++ = 'h';
        *pf++ = 'h';
    }
    else if (spec.mod_l)
        *pf++ = 'l';
    else if (spec.mod_ll) {
        *pf++ = 'l';
        *pf++ = 'l';
    }
    else if (spec.mod_j)
        *pf++ = 'j';
    else if (spec.mod_z)
        *pf++ = 'z';
    else if (spec.mod_t)
        *pf++ = 't';
    else if (spec.mod_L) {
        strcpy (pf, _RWSTD_LDBL_PRINTF_PREFIX);
        for ( ; *pf; ++pf);
    }
    else if (spec.mod_A && _RWSTD_LDBL_SIZE == spec.width) {
        strcpy (pf, _RWSTD_LDBL_PRINTF_PREFIX);
        for ( ; *pf; ++pf);
    }

    if (!spec.mod_A && 0 <= spec.width) {
        pf += sprintf (pf, "%i", spec.width);
    }

    if (0 <= spec.prec)
        pf += sprintf (pf, ".%i", spec.prec);

    *pf++ = char (spec.cvtspec);
    *pf   = '\0';

    // verify that the format buffer hasn't overflowed
    assert (size_t (pf - fmt) + 1 < sizeof fmt);

    // this might make the buffer almost 5KB
    char buffer [_RWSTD_LDBL_MAX_10_EXP + _RWSTD_LDBL_DIG + 3];
    int len = -1;

    if (spec.mod_A) {

        if (_RWSTD_FLT_SIZE == spec.width) {
            len = sprintf (buffer, fmt, *(const float*)pval);
        }
        else if (_RWSTD_DBL_SIZE == spec.width) {
            len = sprintf (buffer, fmt, *(const double*)pval);
        }
        else if (_RWSTD_LDBL_SIZE == spec.width) {
            len = sprintf (buffer, fmt, *(const long double*)pval);
        }
        else {
            assert (!"unknown floating point size");
        }
    }
    else if (spec.mod_L)
        len = sprintf (buffer, fmt, *(const long double*)pval);
    else
        len = sprintf (buffer, fmt, *(const double*)pval);

    assert (size_t (len) < sizeof buffer);

#ifdef _MSC_VER

    if (5 < len) {
        // remove redundant zeros from the exponent (if present)
        if (   ('e' == buffer [len - 5] || 'E' == buffer [len - 5])
            && '0' == buffer [len - 3]) {
            buffer [len - 3] = buffer [len - 2];
            buffer [len - 2] = buffer [len - 1];
            buffer [len - 1] = buffer [len];
        }
    }

#endif   // _MSC_VER


    if (-1 < len && 0 == rw_bufcat (pbuf, pbufsize, buffer, size_t (len)))
        return -1;

    return len;
}

/********************************************************************/

_RWSTD_INTERNAL int
rw_fmtptr (const FmtSpec &spec, char **pbuf, size_t *pbufsize,
           const void *val)
{
    FmtSpec newspec (spec);

    // format data pointers in hexadecimal
    newspec.base = 16;

    // set the number of digits
    newspec.prec = _RWSTD_PTR_SIZE * 2;

    int len = 0;

    // always use the "0x" prefix for pointers, even when null
    if (spec.fl_pound && 0 == val) {
        if (0 == rw_bufcat (pbuf, pbufsize, "0x", len = 2))
            return -1;
    }

#if _RWSTD_LONG_SIZE <= _RWSTD_PTR_SIZE

    return len + rw_fmtlong (newspec, pbuf, pbufsize, (long)val);

#elif _RWSTD_LLONG_SIZE <= _RWSTD_PTR_SIZE

    return len + rw_fmtlong (newspec, pbuf, pbufsize, (_RWSTD_LONG_LONG)val);

#else

    assert (!"pointer formatting not implemented");
    return -1;

#endif

}

/********************************************************************/

static int
rw_fmtfunptr (const FmtSpec &spec, char **pbuf, size_t *pbufsize,
              funptr_t val)
{
    if (spec.mod_l) {

#if 0   // disabled until this is implemented on other platforms
#ifdef _RWSTD_OS_SUNOS

        char buffer [256];

        Dl_info dli;

        // find the symbol corresponding to the address
        if (dladdr ((void*)val, &dli)) {
            if (!dli.dli_sname)
                dli.dli_fname = "unknown";

            const size_t sym_off = size_t (dli.dli_saddr);

            // compute the offset of the address from the address
            // of the symbol dladdr() found in the address space
            // of the calling process
            const size_t addr_off = size_t (val) < sym_off ?
                sym_off - size_t (val) : size_t (val) - sym_off;

            // format the address folowed by the name of the symbol
            // followed by the offset
            const int len = sprintf (buffer, "%#x=%s%c%lu",
                                     size_t (val), dli.dli_sname,
                                     size_t (val) < sym_off ? '-' : '+',
                                     addr_off);
            
            FmtSpec newspec (spec);
            newspec.mod_l = false;

            return rw_fmtstr (newspec, pbuf, pbufsize, buffer, size_t (len));
        }

#endif   // _RWSTD_OS_SUNOS
#endif   // 0/1
    }

#if _RWSTD_FUNPTR_SIZE == _RWSTD_PTR_SIZE

    const union {
        funptr_t fptr;
        void    *vptr;
    } uval = { val };

    return rw_fmtptr (spec, pbuf, pbufsize, uval.vptr);
    
#else

    assert (!"function pointer formatting not implemented");
    return -1;

#endif

}

/********************************************************************/

static int
rw_fmtmemptr (const FmtSpec &spec,
              char **pbuf, size_t *pbufsize, memptr_t val)
{
    FmtSpec newspec (spec);

    newspec.base = 16;

    const union {
        memptr_t         mpval;
#ifdef _RWSTD_LONG_LONG
        _RWSTD_LONG_LONG llval;
#endif   // _RWSTD_LONG_LONG
        long             lval;
    } uval = { val };
        
#if _RWSTD_LONG_SIZE <= _RWSTD_MEMPTR_SIZE

    return rw_fmtlong (spec, pbuf, pbufsize, uval.lval);

#elif _RWSTD_LLONG_SIZE <= _RWSTD_MEMPTR_SIZE

    return rw_fmtlong (spec, pbuf, pbufsize, uval.llval);

#else

    _RWSTD_UNUSED (newspec);
    assert (!"member pointer formatting not implemented");
    return -1;

#endif

}

/********************************************************************/

static int
rw_fmterrno (const FmtSpec &spec, char **pbuf, size_t *pbufsize, int val)
{
    static const struct {
        int         val;
        const char* str;
    } names[] = {

#undef ERRNO
#define ERRNO(val)   { val, #val }

#ifdef EPERM
        ERRNO (EPERM),
#endif   // EPERM
#ifdef ENOENT
        ERRNO (ENOENT),
#endif   // ENOENT
#ifdef ESRCH
        ERRNO (ESRCH),
#endif   // ESRCH
#ifdef EINTR
        ERRNO (EINTR),
#endif   // EINTR
#ifdef EIO
        ERRNO (EIO),
#endif   // EIO
#ifdef ENXIO
        ERRNO (ENXIO),
#endif   // ENXIO
#ifdef E2BIG
        ERRNO (E2BIG),
#endif   // E2BIG
#ifdef ENOEXEC
        ERRNO (ENOEXEC),
#endif   // ENOEXEC
#ifdef EBADF
        ERRNO (EBADF),
#endif   // EBADF
#ifdef ECHILD
        ERRNO (ECHILD),
#endif   // ECHILD
#ifdef EAGAIN
        ERRNO (EAGAIN),
#endif   // EAGAIN
#ifdef ENOMEM
        ERRNO (ENOMEM),
#endif   // ENOMEM
#ifdef EACCES
        ERRNO (EACCES),
#endif   // EACCES
#ifdef EFAULT
        ERRNO (EFAULT),
#endif   // EFAULT
#ifdef ENOTBLK
        ERRNO (ENOTBLK),
#endif   // ENOTBLK
#ifdef EBUSY
        ERRNO (EBUSY),
#endif   // EBUSY
#ifdef EEXIST
        ERRNO (EEXIST),
#endif   // EEXIST
#ifdef EXDEV
        ERRNO (EXDEV),
#endif   // EXDEV
#ifdef ENODEV
        ERRNO (ENODEV),
#endif   // ENODEV
#ifdef ENOTDIR
        ERRNO (ENOTDIR),
#endif   // ENOTDIR
#ifdef EISDIR
        ERRNO (EISDIR),
#endif   // EISDIR
#ifdef EINVAL
        ERRNO (EINVAL),
#endif   // EINVAL
#ifdef ENFILE
        ERRNO (ENFILE),
#endif   // ENFILE
#ifdef EMFILE
        ERRNO (EMFILE),
#endif   // EMFILE
#ifdef ENOTTY
        ERRNO (ENOTTY),
#endif   // ENOTTY
#ifdef ETXTBSY
        ERRNO (ETXTBSY),
#endif   // ETXTBSY
#ifdef EFBIG
        ERRNO (EFBIG),
#endif   // EFBIG
#ifdef ENOSPC
        ERRNO (ENOSPC),
#endif   // ENOSPC
#ifdef ESPIPE
        ERRNO (ESPIPE),
#endif   // ESPIPE
#ifdef EROFS
        ERRNO (EROFS),
#endif   // EROFS
#ifdef EMLINK
        ERRNO (EMLINK),
#endif   // EMLINK
#ifdef EPIPE
        ERRNO (EPIPE),
#endif   // EPIPE
#ifdef EDOM
        ERRNO (EDOM),
#endif   // EDOM
#ifdef ERANGE
        ERRNO (ERANGE),
#endif   // ERANGE
#ifdef ENOMSG
        ERRNO (ENOMSG),
#endif   // ENOMSG
#ifdef EIDRM
        ERRNO (EIDRM),
#endif   // EIDRM
#ifdef ECHRNG
        ERRNO (ECHRNG),
#endif   // ECHRNG
#ifdef EL2NSYNC
        ERRNO (EL2NSYNC),
#endif   // EL2NSYNC
#ifdef EL3HLT
        ERRNO (EL3HLT),
#endif   // EL3HLT
#ifdef EL3RST
        ERRNO (EL3RST),
#endif   // EL3RST
#ifdef ELNRNG
        ERRNO (ELNRNG),
#endif   // ELNRNG
#ifdef EUNATCH
        ERRNO (EUNATCH),
#endif   // EUNATCH
#ifdef ENOCSI
        ERRNO (ENOCSI),
#endif   // ENOCSI
#ifdef EL2HLT
        ERRNO (EL2HLT),
#endif   // EL2HLT
#ifdef EDEADLK
        ERRNO (EDEADLK),
#endif   // EDEADLK
#ifdef ENOLCK
        ERRNO (ENOLCK),
#endif   // ENOLCK
#ifdef ECANCELED
        ERRNO (ECANCELED),
#endif   // ECANCELED
#ifdef ENOTSUP
        ERRNO (ENOTSUP),
#endif   // ENOTSUP
#ifdef EDQUOT
        ERRNO (EDQUOT),
#endif   // EDQUOT
#ifdef EBADE
        ERRNO (EBADE),
#endif   // EBADE
#ifdef EBADR
        ERRNO (EBADR),
#endif   // EBADR
#ifdef EXFULL
        ERRNO (EXFULL),
#endif   // EXFULL
#ifdef ENOANO
        ERRNO (ENOANO),
#endif   // ENOANO
#ifdef EBADRQC
        ERRNO (EBADRQC),
#endif   // EBADRQC
#ifdef EBADSLT
        ERRNO (EBADSLT),
#endif   // EBADSLT
#ifdef EDEADLOCK
        ERRNO (EDEADLOCK),
#endif   // EDEADLOCK
#ifdef EBFONT
        ERRNO (EBFONT),
#endif   // EBFONT
#ifdef EOWNERDEAD
        ERRNO (EOWNERDEAD),
#endif   // EOWNERDEAD
#ifdef ENOTRECOVERABLE
        ERRNO (ENOTRECOVERABLE),
#endif   // ENOTRECOVERABLE
#ifdef ENOSTR
        ERRNO (ENOSTR),
#endif   // ENOSTR
#ifdef ENODATA
        ERRNO (ENODATA),
#endif   // ENODATA
#ifdef ETIME
        ERRNO (ETIME),
#endif   // ETIME
#ifdef ENOSR
        ERRNO (ENOSR),
#endif   // ENOSR
#ifdef ENONET
        ERRNO (ENONET),
#endif   // ENONET
#ifdef ENOPKG
        ERRNO (ENOPKG),
#endif   // ENOPKG
#ifdef EREMOTE
        ERRNO (EREMOTE),
#endif   // EREMOTE
#ifdef ENOLINK
        ERRNO (ENOLINK),
#endif   // ENOLINK
#ifdef EADV
        ERRNO (EADV),
#endif   // EADV
#ifdef ESRMNT
        ERRNO (ESRMNT),
#endif   // ESRMNT
#ifdef ECOMM
        ERRNO (ECOMM),
#endif   // ECOMM
#ifdef ELOCKUNMAPPED
        ERRNO (ELOCKUNMAPPED),
#endif   // ELOCKUNMAPPED
#ifdef ENOTACTIVE
        ERRNO (ENOTACTIVE),
#endif   // ENOTACTIVE
#ifdef EMULTIHOP
        ERRNO (EMULTIHOP),
#endif   // EMULTIHOP
#ifdef EBADMSG
        ERRNO (EBADMSG),
#endif   // EBADMSG
#ifdef ENAMETOOLONG
        ERRNO (ENAMETOOLONG),
#endif   // ENAMETOOLONG
#ifdef EOVERFLOW
        ERRNO (EOVERFLOW),
#endif   // EOVERFLOW
#ifdef ENOTUNIQ
        ERRNO (ENOTUNIQ),
#endif   // ENOTUNIQ
#ifdef EBADFD
        ERRNO (EBADFD),
#endif   // EBADFD
#ifdef EREMCHG
        ERRNO (EREMCHG),
#endif   // EREMCHG
#ifdef ELIBACC
        ERRNO (ELIBACC),
#endif   // ELIBACC
#ifdef ELIBBAD
        ERRNO (ELIBBAD),
#endif   // ELIBBAD
#ifdef ELIBSCN
        ERRNO (ELIBSCN),
#endif   // ELIBSCN
#ifdef ELIBMAX
        ERRNO (ELIBMAX),
#endif   // ELIBMAX
#ifdef ELIBEXEC
        ERRNO (ELIBEXEC),
#endif   // ELIBEXEC
#ifdef EILSEQ
        ERRNO (EILSEQ),
#endif   // EILSEQ
#ifdef ENOSYS
        ERRNO (ENOSYS),
#endif   // ENOSYS
#ifdef ELOOP
        ERRNO (ELOOP),
#endif   // ELOOP
#ifdef ERESTART
        ERRNO (ERESTART),
#endif   // ERESTART
#ifdef ESTRPIPE
        ERRNO (ESTRPIPE),
#endif   // ESTRPIPE
#ifdef ENOTEMPTY
        ERRNO (ENOTEMPTY),
#endif   // ENOTEMPTY
#ifdef EUSERS
        ERRNO (EUSERS),
#endif   // EUSERS
#ifdef ENOTSOCK
        ERRNO (ENOTSOCK),
#endif   // ENOTSOCK
#ifdef EDESTADDRREQ
        ERRNO (EDESTADDRREQ),
#endif   // EDESTADDRREQ
#ifdef EMSGSIZE
        ERRNO (EMSGSIZE),
#endif   // EMSGSIZE
#ifdef EPROTOTYPE
        ERRNO (EPROTOTYPE),
#endif   // EPROTOTYPE
#ifdef ENOPROTOOPT
        ERRNO (ENOPROTOOPT),
#endif   // ENOPROTOOPT
#ifdef EPROTONOSUPPORT
        ERRNO (EPROTONOSUPPORT),
#endif   // EPROTONOSUPPORT
#ifdef ESOCKTNOSUPPORT
        ERRNO (ESOCKTNOSUPPORT),
#endif   // ESOCKTNOSUPPORT
#ifdef EOPNOTSUPP
        ERRNO (EOPNOTSUPP),
#endif   // EOPNOTSUPP
#ifdef EPFNOSUPPORT
        ERRNO (EPFNOSUPPORT),
#endif   // EPFNOSUPPORT
#ifdef EAFNOSUPPORT
        ERRNO (EAFNOSUPPORT),
#endif   // EAFNOSUPPORT
#ifdef EADDRINUSE
        ERRNO (EADDRINUSE),
#endif   // EADDRINUSE
#ifdef EADDRNOTAVAIL
        ERRNO (EADDRNOTAVAIL),
#endif   // EADDRNOTAVAIL
#ifdef ENETDOWN
        ERRNO (ENETDOWN),
#endif   // ENETDOWN
#ifdef ENETUNREACH
        ERRNO (ENETUNREACH),
#endif   // ENETUNREACH
#ifdef ENETRESET
        ERRNO (ENETRESET),
#endif   // ENETRESET
#ifdef ECONNABORTED
        ERRNO (ECONNABORTED),
#endif   // ECONNABORTED
#ifdef ECONNRESET
        ERRNO (ECONNRESET),
#endif   // ECONNRESET
#ifdef ENOBUFS
        ERRNO (ENOBUFS),
#endif   // ENOBUFS
#ifdef EISCONN
        ERRNO (EISCONN),
#endif   // EISCONN
#ifdef ENOTCONN
        ERRNO (ENOTCONN),
#endif   // ENOTCONN
#ifdef ESHUTDOWN
        ERRNO (ESHUTDOWN),
#endif   // ESHUTDOWN
#ifdef ETOOMANYREFS
        ERRNO (ETOOMANYREFS),
#endif   // ETOOMANYREFS
#ifdef ETIMEDOUT
        ERRNO (ETIMEDOUT),
#endif   // ETIMEDOUT
#ifdef ECONNREFUSED
        ERRNO (ECONNREFUSED),
#endif   // ECONNREFUSED
#ifdef EHOSTDOWN
        ERRNO (EHOSTDOWN),
#endif   // EHOSTDOWN
#ifdef EHOSTUNREACH
        ERRNO (EHOSTUNREACH),
#endif   // EHOSTUNREACH
#ifdef EWOULDBLOCK
        ERRNO (EWOULDBLOCK),
#endif   // EWOULDBLOCK
#ifdef EALREADY
        ERRNO (EALREADY),
#endif   // EALREADY
#ifdef EINPROGRESS
        ERRNO (EINPROGRESS),
#endif   // EINPROGRESS
#ifdef ESTALE
        ERRNO (ESTALE),
#endif   // ESTALE
        { -1, 0 }
    };

    if (spec.fl_pound) {

        char buffer [64];
        const char *name = 0;

        for (size_t i = 0; i != sizeof names / sizeof *names; ++i) {
            if (names [i].val == val) {
                name = names [i].str;
                break;
            }
        }

        int len;

        if (0 == name) {
            len = sprintf (buffer, "E#%d", val);
            name = buffer;
        }
        else
            len = int (strlen (name));

        if (0 == rw_bufcat (pbuf, pbufsize, name, size_t (len)))
            return -1;

        return len;
    }

    const char* const str = strerror (val);
    const size_t len = strlen (str);

    if (0 == rw_bufcat (pbuf, pbufsize, str, len))
        return -1;

    return int (len);
}

/********************************************************************/

template <class charT>
int rw_quotechar (char *buf, charT wc, int noesc)
{
#if _RWSTD_WCHAR_T_MIN < 0

    // wchar_t is signed, convert its value to unsigned long
    // without widening (i.e., treat it as an unsigned type)

#  if _RWSTD_WCHAR_T_MIN == _RWSTD_SHRT_MIN
    const unsigned long wi = (unsigned short)wc;
#  elif _RWSTD_WCHAR_T_MIN ==_RWSTD_INT_MIN
    const unsigned long wi = (unsigned int)wc;
#  elif _RWSTD_WCHAR_T_MIN == _RWSTD_LONG_MIN
    const unsigned long wi = (unsigned long)wc;
#  endif

#else   // if _RWSTD_WCHAR_T_MIN >= 0

    // wchar_t is unsigned
    const unsigned long wi = (unsigned long)wc;

#endif   // _RWSTD_WCHAR_T_MIN < 0

    if ((1 == sizeof wc || wi < 0x100) && noesc) {
        buf [0] = char (wc);
        buf [1] = '\0';
        return 1;
    }

    int len = 3;

    buf [0] = '\\';
    buf [2] = '\0';

    switch (wc) {
    case '\a': buf [1] = 'a';  buf [len = 2] = '\0'; break;
    case '\b': buf [1] = 'b';  buf [len = 2] = '\0'; break;
    case '\f': buf [1] = 'f';  buf [len = 2] = '\0'; break;
    case '\n': buf [1] = 'n';  buf [len = 2] = '\0'; break;
    case '\r': buf [1] = 'r';  buf [len = 2] = '\0'; break;
    case '\t': buf [1] = 't';  buf [len = 2] = '\0'; break;
    case '\v': buf [1] = 'v';  buf [len = 2] = '\0'; break;
    case '\\': buf [1] = '\\'; buf [len = 2] = '\0'; break;
    case '"' : buf [1] = '"';  buf [len = 2] = '\0'; break;
    default:
        if (wc < ' ' || wc > '~') {

            if (0 == wc) {
                buf [1] = '0';
                buf [2] = '\0';
                len = 2;
            }
            else if (1 == sizeof wc) {
                len = 1 + sprintf (buf + 1, "x%02x", (unsigned char)wc);
            }
            else {

                const int width =
                      wi > 0xfffffffUL ? 8 : wi > 0xffffffUL ? 7
                    : wi > 0xfffffUL   ? 6 : wi > 0xffffUL   ? 5
                    : wi > 0xfffUL     ? 4 : wi > 0xffUL     ? 3
                    : wi > 0xfUL       ? 2 : 2;

                len = 1 + sprintf (buf + 1, "x%0*lx", width, (unsigned long)wi);
            }
        }
        else {
            buf [0] = wc;
            buf [1] = '\0';
            len = 1;
        }
    }

    return len;
}


template <class charT>
int rw_quotestr (const FmtSpec &spec, char **pbuf, size_t *pbufsize,
                 const charT *wstr, size_t nchars, int noesc)
{
    assert (0 != pbuf);

    if (!wstr) {
        static const charT null[] = { '(', 'n', 'u', 'l', 'l', ')', '\0' };
        wstr   = null;
        nchars = sizeof null / sizeof *null - 1;
    }

    if (0 > _RW::__rw_memattr (wstr, _RWSTD_SIZE_MAX, 0)) {

        const size_t buflen = *pbuf ? strlen (*pbuf) : 0;

        if (0 == rw_bufcat (pbuf, pbufsize, "(invalid address ", 18))
            return -1;

        FmtSpec newspec (spec);
        newspec.fl_pound = 1;
        if (-1 == ::rw_fmtptr (newspec, pbuf, pbufsize, wstr))
            return -1;
        if (0 == rw_bufcat (pbuf, pbufsize, ")", 2))
            return -1;

        return int (strlen (*pbuf) - buflen);
    }

    if (_RWSTD_SIZE_MAX == nchars) {
        // compute the length of the NUL-terminate string
        nchars = 0;
        for (const charT *pc = wstr; *pc; ++pc, ++nchars);
    }

    char *next = rw_bufcat (pbuf, pbufsize, 0, (nchars + 1) * 12 + 3);
    const char* const bufend = next;

    if (0 == next)
        return -1;

    if (0 == nchars) {
        if (noesc) {

#if 0   // width handling disabled (width used for array formatting)
            for (int w = 0; w < spec.width; ++w)
                *next++ = ' ';
#endif   // 0/1

        }
        else {
            if (_RWSTD_WCHAR_T_SIZE == sizeof (charT))
                *next++ = 'L';

            *next++ = '"';
#if 0   // width handling disabled (width used for array formatting)
            for (int w = 0; w < spec.width; ++w)
                 *next++ = ' ';
#endif   // 0/1
            *next++ = '"';
        }
        *next++ = '\0';
        return int (next - bufend);
    }

    char *s = next;

    const charT *last = wstr;

    bool any_repeats = false;
    long last_repeat = noesc ? 0L : -1L;

    char chstr [16];

    const ptrdiff_t N = ptrdiff_t (noesc ? 0 : 20);

    for (const charT *pwc = last + 1; ; ++pwc) {

        if (*pwc == *last && size_t (pwc - wstr) < nchars) {
            // if the last processed character repeats, continue
            // until a different character is encountered
            continue;
        }

        if (N > 1 && pwc - last > N) {

            // if the last processed character repeats N or more
            // times, format the repeat count instead of all the
            // repeated occurrences of the character to conserve
            // space and make the string more readable

            const long repeat = pwc - last;

            rw_quotechar (chstr, *last, noesc);

            s += sprintf (s, "%s'%s' <repeats %ld times>",
                            -1 == last_repeat ? ""
                          :  0 == last_repeat ? "\", " : ", ",
                          chstr, repeat);

            last = pwc;

            any_repeats = true;
            last_repeat = repeat;
        }
        else {
            // otherwise (if the last processed character repeats
            // fewer than N times) format the character that many
            // times

            if (last_repeat < 0) {
                // opening quote
                if (_RWSTD_WCHAR_T_SIZE == sizeof (charT)) {
                    *s++ = 'L';
                }
                *s++ = '\"';
            }
            else if (last_repeat > 0) {
                *s++ = ',';
                *s++ = ' ';
                *s++ = '\"';
            }

            rw_quotechar (chstr, *last, noesc);

            while (last != pwc) {
                s += sprintf (s, "%s", chstr);
                ++last;
            }

            last_repeat = 0;

            if (size_t (pwc - wstr) == nchars) {
                if (!noesc)
                    *s++ = '\"';
                *s = '\0';
                break;
            }
        }

        if (size_t (pwc - wstr) == nchars)
            break;
    }

    if (any_repeats) {
        const size_t len = strlen (next);
        memmove (next + 2, next, len);
        next [0] = '{';
        next [1] = ' ';
        next [len + 2] = ' ';
        next [len + 3] = '}';
        next [len + 4] = '\0';
        s = next + len + 4;
    }

    return int (s - bufend);
}

/********************************************************************/

static int
rw_fmtchr (const FmtSpec &spec, char **pbuf, size_t *pbufsize, int val)
{
    typedef unsigned char UChar;
    const UChar uc = UChar (val);

    char buffer [8];
    int len = rw_quotechar (buffer + spec.fl_pound, uc, !spec.fl_pound);
    if (spec.fl_pound) {
        buffer [0] = buffer [len + 1] = '\'';
        buffer [len + 2] = '\0';
        len += 2;
    }

    FmtSpec newspec (spec);
    newspec.fl_pound = 0;
    return rw_fmtstr (newspec, pbuf, pbufsize, buffer, size_t (len));
}

/********************************************************************/

static int
rw_fmtwchr (const FmtSpec &spec, char **pbuf, size_t *pbufsize, wint_t val)
{
    const wchar_t wc = wchar_t (val);

    char buffer [16];
    int len = rw_quotechar (buffer + 2 * spec.fl_pound, wc, !spec.fl_pound);
    if (spec.fl_pound) {
        buffer [0] = 'L';
        buffer [1] = buffer [len + 2] = '\'';
        buffer [len + 3] = '\0';
        len += 3;
    }

    FmtSpec newspec (spec);
    newspec.fl_pound = 0;
    return rw_fmtstr (newspec, pbuf, pbufsize, buffer, size_t (len));
}

/********************************************************************/

static int
rw_fmtstr (const FmtSpec &spec,
           char **pbuf, size_t *pbufsize, const char *str, size_t len)
{
    if (spec.fl_pound)
        return rw_quotestr (spec, pbuf, pbufsize, str, len, 0);

    if (0 == str)
        str = "(null)";

    if (0 > _RW::__rw_memattr (str, _RWSTD_SIZE_MAX, 0)) {

        const size_t buflen = *pbuf ? strlen (*pbuf) : 0;

        if (0 == rw_bufcat (pbuf, pbufsize, "(invalid address ", 18))
            return -1;

        FmtSpec newspec (spec);
        newspec.fl_pound = 1;

        if (-1 == rw_fmtptr (newspec, pbuf, pbufsize, str))
            return -1;

        if (0 == rw_bufcat (pbuf, pbufsize, ")", 2))
            return -1;

        return int (strlen (*pbuf) - buflen);
    }

    if (_RWSTD_SIZE_MAX == len)
        len = strlen (str);

    // compute the minimum number of characters to be generated
    if (-1 < spec.prec && size_t (spec.prec) < len)
        len = size_t (spec.prec);

    // the number of generated characters depends on three variables:
    // --  the optional field width,
    // --  the optional precision, and
    // --  the length of the argument

    // compute the field width
    const size_t width =
        -1 < spec.width && len < size_t (spec.width) ?
        size_t (spec.width) : len;

    // compute the size of padding
    const size_t pad = len < width ? width - len : 0;

    // [re]allocate enough space in the buffer
    if (0 == rw_bufcat (pbuf, pbufsize, 0, pad + len))
        return -1;

    assert (0 != *pbuf);
    char *next = *pbuf + strlen (*pbuf);

    if (!spec.fl_minus) {
        for (size_t i = 0; i != pad; ++i)
            *next++ = ' ';
    }

    memcpy (next, str, len);
    next += len;

    if (spec.fl_minus) {
        for (size_t i = 0; i != pad; ++i)
            *next++ = ' ';
    }

    *next++ = '\0';

    return int (pad + len);
}

/********************************************************************/

static int
rw_fmtwstr (const FmtSpec &spec,
            char **pbuf, size_t *pbufsize, const wchar_t *wstr, size_t len)
{
    return rw_quotestr (spec, pbuf, pbufsize, wstr, len, 1);
}

/********************************************************************/

struct Bitnames
{
    const char *longname;
    const char *name;
    int         bits;
};

#define BITNAME(qual, name)   { #qual "::" #name, #name, qual::name }

static int
rw_bmpfmt (const FmtSpec&, char **pbuf, size_t *pbufsize,
           const Bitnames bmap[], size_t size, int bits)
{
    assert (0 != pbuf);

    char buffer [1024];
    *buffer = '\0';

    // string to use when no bits are set
    const char* all_clear = "0";

    for (size_t i = 0; i != size; ++i) {
        if (bmap [i].bits) {
            if ((bits & bmap [i].bits) == bmap [i].bits) {
                strcat (*buffer ? strcat (buffer, " | ") : buffer,
                        bmap [i].name);
                bits &= ~bmap [i].bits;
            }
        }
        else {
            // save the name of the constant to use for 0
            all_clear = bmap [i].name;
        }
    }

    size_t buffersize;

    if ('\0' == *buffer) {
        // no constant matched, format teh value either as a number
        // or, when 0, using the all_clear name (see above)
        if (bits)
            sprintf (buffer, "%#x", bits);
        else
            strcpy (buffer, all_clear);

        buffersize = strlen (buffer) + 1;
    }
    else if (bits) {
        buffersize = strlen (buffer) + 1;

        // verify that buffer wasn't overflowed
        assert (buffersize <= sizeof buffer);

        char bitstr [32];
        const int n = sprintf (bitstr, "%#x | ", bits);

        assert (0 < n);

        memmove (buffer + n, buffer, buffersize);
        memcpy (buffer, bitstr, size_t (n));

        buffersize += n;
    }
    else {
        buffersize = strlen (buffer) + 1;
    }

    // verify that buffer wasn't overflowed
    assert (buffersize <= sizeof buffer);

    if (0 == rw_bufcat (pbuf, pbufsize, buffer, buffersize))
        return -1;

    return int (buffersize);
}

/********************************************************************/

static int
rw_fmtflags (const FmtSpec &spec, char **pbuf, size_t *pbufsize, int bits)
{
    static const Bitnames names [] = {
        BITNAME (std::ios, adjustfield),
        BITNAME (std::ios, basefield),
        BITNAME (std::ios, boolalpha),
        BITNAME (std::ios, dec),
        BITNAME (std::ios, fixed),
        BITNAME (std::ios, hex),
        BITNAME (std::ios, internal),
        BITNAME (std::ios, left),
        BITNAME (std::ios, oct),
        BITNAME (std::ios, right),
        BITNAME (std::ios, scientific),
        BITNAME (std::ios, showbase),
        BITNAME (std::ios, showpoint),
        BITNAME (std::ios, showpos),
        BITNAME (std::ios, skipws),
        BITNAME (std::ios, unitbuf),
        BITNAME (std::ios, uppercase),

#ifndef _RWSTD_NO_EXT_BIN_IO

        // extension: produce binary output (similar to oct, dec, and hex)
        BITNAME (std::ios, bin),

#endif   // _RWSTD_NO_EXT_BIN_IO

#ifndef _RWSTD_NO_EXT_REENTRANT_IO

        // extension: allow unsychronized access to stream and/or its buffer
        BITNAME (std::ios, nolock),
        BITNAME (std::ios, nolockbuf)

#endif   // _RWSTD_NO_EXT_REENTRANT_IO

    };

    static const size_t count = sizeof names / sizeof *names;

    const int base = (bits >> _RWSTD_IOS_BASEOFF) & _RWSTD_IOS_BASEMASK;

    // zero out bits representingthe numeric base
    bits &= ~(_RWSTD_IOS_BASEMASK << _RWSTD_IOS_BASEOFF);

    int len = rw_bmpfmt (spec, pbuf, pbufsize, names, count, bits);

    if (base && base != 8 && base != 10 && base != 16) {

        // for numeric bases other than those required by the standard,
        // use the text "base (%d)" to show the extended numeric base

#ifndef _RWSTD_NO_EXT_BIN_IO

        if (bits & std::ios::bin)
            return len;

#endif   // _RWSTD_NO_EXT_BIN_IO

        char basestr [64];
        sprintf (basestr, " | std::ios::base(%d)", base);

        strcat (*pbuf, basestr);
        
        len = int (strlen (*pbuf));
    }

    return len;
}

/********************************************************************/

static int
rw_fmtiostate (const FmtSpec &spec, char **pbuf, size_t *pbufsize, int bits)
{
    static const Bitnames names [] = {
        BITNAME (std::ios, goodbit),
        BITNAME (std::ios, badbit),
        BITNAME (std::ios, eofbit),
        BITNAME (std::ios, failbit)
    };

    static const size_t count = sizeof names / sizeof *names;

    return rw_bmpfmt (spec, pbuf, pbufsize, names, count, bits);
}

/********************************************************************/

static int
rw_fmtopenmode (const FmtSpec &spec, char **pbuf, size_t *pbufsize, int bits)
{
    static const Bitnames names [] = {

#ifndef _RWSTD_NO_EXTENSIONS

        { "std::ios::nocreate", "nocreate", std::ios::nocreate },
        { "std::ios::noreplace", "noreplace", std::ios::noreplace },

#else   // if defined (_RWSTD_NO_EXTENSIONS)

        { "__rw:::__rw_nocreate", "__rw_nocreate", _RW::__rw_nocreate },
        { "__rw::__rw_noreplace", "__rw_noreplace", _RW::__rw_noreplace },

#endif   // _RWSTD_NO_EXTENSIONS

#ifndef _RWSTD_NO_EXT_STDIO

        { "std::ios::stdio", "stdio", std::ios::stdio },
        { "std::ios::native", "native", std::ios::native },

#else   // if defined (_RWSTD_NO_EXT_STDIO)

        { "__rw::__rw_stdio", "__rw_stdio", _RW::__rw_stdio },
        { "__rw::__rw_native", "__rw_native", _RW::__rw_native },

#endif   // _RWSTD_NO_EXT_STDIO

        BITNAME (std::ios, app),
        BITNAME (std::ios, binary),
        BITNAME (std::ios, in),
        BITNAME (std::ios, out),
        BITNAME (std::ios, trunc),
        BITNAME (std::ios, ate)
    };

    static const size_t count = sizeof names / sizeof *names;

    return rw_bmpfmt (spec, pbuf, pbufsize, names, count, bits);
}

/********************************************************************/

static int
rw_fmtevent (const FmtSpec&, char **pbuf, size_t *pbufsize, int event)
{
    const char* str =
          std::ios::copyfmt_event == event ? "copyfmt_event"
        : std::ios::imbue_event   == event ? "imbue_event"
        : std::ios::erase_event   == event ? "erase_event"
        : 0;

    char buffer [64];

    if (!str) {
        sprintf (buffer, "copyfmt_event(%d)", event);
        str = buffer;
    }

    const size_t len = strlen (str);

    if (0 == rw_bufcat (pbuf, pbufsize, str, len))
        return -1;

    return int (len);
}

/********************************************************************/

static int
rw_fmtlc (const FmtSpec &spec, char **pbuf, size_t *pbufsize, int val)
{
    const char *str = 0;

    switch (val) {
    case LC_ALL:      str = "LC_ALL"; break;
    case LC_COLLATE:  str = "LC_COLLATE"; break;
    case LC_CTYPE:    str = "LC_CTYPE"; break;
    case LC_MONETARY: str = "LC_MONETARY"; break;
    case LC_NUMERIC:  str = "LC_NUMERIC"; break;
    case LC_TIME:     str = "LC_TIME"; break;

#ifdef LC_MESSAGES
    case LC_MESSAGES: str = "LC_MESSAGES"; break;
#endif   // LC_MESSAGES

    }

    if (str) {
        const std::size_t len = strlen (str);

        if (0 == rw_bufcat (pbuf, pbufsize, str, len))
            return -1;

        return int (len);
    }

    static const Bitnames names [] = {
        BITNAME (std::locale, all),
        BITNAME (std::locale, none),
        BITNAME (std::locale, collate),
        BITNAME (std::locale, ctype),
        BITNAME (std::locale, monetary),
        BITNAME (std::locale, numeric),
        BITNAME (std::locale, messages),
        BITNAME (std::locale, time)
    };

    static const size_t count = sizeof names / sizeof *names;

    return rw_bmpfmt (spec, pbuf, pbufsize, names, count, val);
}

/********************************************************************/

static int
rw_fmtmonpat (const FmtSpec&,
              char **pbuf, size_t *pbufsize, const char pat [4])
{
    char buffer [80];

    buffer [0] = '\0';

    for (int i = 0; i != 4; ++i) {
        switch (pat [i]) {
        case std::money_base::symbol:
            strcat (buffer, "symbol ");
            break;

        case std::money_base::sign:
            strcat (buffer, "sign ");
            break;

        case std::money_base::none:
            strcat (buffer, "none ");
            break;

        case std::money_base::value:
            strcat (buffer, "value ");
            break;

        case std::money_base::space:
            strcat (buffer, "space ");
            break;

        default:
            sprintf (buffer + strlen (buffer), "\\%03o", pat [i]);
            break;
        }
    }

    const size_t len = strlen (buffer);

    if (0 == rw_bufcat (pbuf, pbufsize, buffer, len))
        return -1;

    return int (len);
}

/********************************************************************/

static int
libstd_vasnprintf (FmtSpec *pspec, size_t paramno,
                   char **pbuf, size_t *pbufsize,
                   const char *fmt, va_list *pva)
{
    assert (0 != pva);
    assert (0 != pspec);

    _RWSTD_UNUSED (fmt);

    FmtSpec &spec = pspec [paramno];

    // the length of the sequence appended to the buffer to return
    // to the caller, or a negative value (such as -1) on error
    int len = -1;

    switch (spec.cvtspec) {

    case '?':   // %{?}
        // beginning of an if clause
        spec.cond       = 1;
        spec.cond_begin = 1;
        spec.cond_true  = 0 != va_arg (*pva, int);
        len             = 0;
        break;

    case ':':   // %{:}
        if (spec.cond) {
            // end of an active if clause and the beginning
            // of an inactive else clause

            spec.cond_begin = 1;   // beginning of an else clause
            spec.cond_end   = 1;   // end of an if clause
            spec.cond_true  = !spec.cond_true;
            len             = 0;
        }
        else {
            // misplaced "%{:}"?
            static const char str[] = "%{:}";
            len = rw_quotestr (spec, pbuf, pbufsize, str, _RWSTD_SIZE_MAX, 0);
        }
        break;

    case ';':   // %{;}
        if (spec.cond) {
            spec.cond_end = 1;   // end of an if or else clause
            len           = 0;
        }
        else {
            // misplaced "%{;}"?
            static const char str[] = "%{;}";
            len = rw_quotestr (spec, pbuf, pbufsize, str, _RWSTD_SIZE_MAX, 0);
        }
        break;

    case 'c':   // %{c}, %{Ac}, %{Lc}, %{lc}
        if (spec.mod_A) {
            if (-1 == spec.width || 1 == spec.width) {
                spec.param.ptr = PARAM (_RWSTD_UINT8_T*, ptr);
                const _RWSTD_UINT8_T* const array =
                    (_RWSTD_UINT8_T*)spec.param.ptr;
                len = rw_quotestr (spec, pbuf, pbufsize, array,
                                   _RWSTD_SIZE_MAX, 0);
            }
            else if (2 == spec.width) {
                spec.param.ptr = PARAM (_RWSTD_UINT16_T*, ptr);
                const _RWSTD_UINT16_T* const array =
                    (_RWSTD_UINT16_T*)spec.param.ptr;
                len = rw_quotestr (spec, pbuf, pbufsize, array,
                                   _RWSTD_SIZE_MAX, 0);
            }
            else if (4 == spec.width) {
                spec.param.ptr = PARAM (_RWSTD_UINT32_T*, ptr);
                const _RWSTD_UINT32_T* const array =
                    (_RWSTD_UINT32_T*)spec.param.ptr;
                len = rw_quotestr (spec, pbuf, pbufsize, array,
                                   _RWSTD_SIZE_MAX, 0);
            }
            else if (8 == spec.width) {
                spec.param.ptr = PARAM (_RWSTD_UINT64_T*, ptr);
                const _RWSTD_UINT64_T* const array =
                    (_RWSTD_UINT64_T*)spec.param.ptr;
                len = rw_quotestr (spec, pbuf, pbufsize, array,
                                   _RWSTD_SIZE_MAX, 0);
            }
            else {
                assert (!"%{Ac} not implemented for this character size");
            }
        }
        else if (spec.mod_L) {   // locale category or LC_XXX constant
            spec.param.i = PARAM (int, i);
            len = rw_fmtlc (spec, pbuf, pbufsize, spec.param.i);
        }
        else if (spec.mod_l) {   // wchar_t
            spec.param.wi = PARAM (wint_t, i);
            return rw_fmtwchr (spec, pbuf, pbufsize, spec.param.wi);
        }
        else {   // char
            spec.param.i = PARAM (int, i);
            return rw_fmtchr (spec, pbuf, pbufsize, spec.param.i);
        }
        break;

    case 'e':   // %{e}, %{Ae}
        if (spec.mod_A) {   // array of floating point values
            spec.param.ptr = PARAM (void*, ptr);
            len = rw_fmtfloating (spec, pbuf, pbufsize, spec.param.ptr);
        }
        else if (spec.mod_I) {   // ios::copyfmt_event
            spec.param.i = PARAM (int, i);
            len = rw_fmtevent (spec, pbuf, pbufsize, spec.param.i);
        }
        break;

    case 'f':   // %{f}, %{Af}, %{If}
        if (spec.mod_A) {   // array of floating point values
            spec.param.ptr = PARAM (void*, ptr);
            len = rw_fmtfloating (spec, pbuf, pbufsize, spec.param.ptr);
        }
        if (spec.mod_I) {   // ios::fmtflags
            spec.param.i = PARAM (int, i);
            len = rw_fmtflags (spec, pbuf, pbufsize, spec.param.i);
        }
        else {   // function pointer
            spec.param.funptr = PARAM (funptr_t, funptr);
            len = rw_fmtfunptr (spec, pbuf, pbufsize, spec.param.funptr);
        }
        break;

    case 'g':   // %{g}, %{Ag}
        if (spec.mod_A) {   // array of floating point values
            spec.param.ptr = PARAM (void*, ptr);
            len = rw_fmtfloating (spec, pbuf, pbufsize, spec.param.ptr);
        }
        else {
            assert (!"%{g} not implemented");
        }

    case 'd':   // %{Id}
    case 'i':   // %{Ii}
    case 'o':   // %{Io}
        if (spec.mod_I) {   // ios::openmode
            spec.param.i = PARAM (int, i);
            len = rw_fmtopenmode (spec, pbuf, pbufsize, spec.param.i);
            break;
        }
    case 'x':   // %{x}
    case 'X':   // %{X}
    case 'u':   // %{u}
        len = rw_fmtinteger (pspec, paramno, pbuf, pbufsize, pva);
        break;


    case 'm':   // %{m} -- errno
        len = rw_fmterrno (spec, pbuf, pbufsize, errno);
        break;

    case 'M':   // %{M}, %{LM}
        if (spec.mod_L) {   // money_base::pattern
            spec.param.ptr = PARAM (char*, ptr);
            len = rw_fmtmonpat (spec, pbuf, pbufsize, (char*)spec.param.ptr);
        }
        else {   // member pointer
            spec.param.memptr = PARAM (memptr_t, memptr);
            len = rw_fmtmemptr (spec, pbuf, pbufsize, spec.param.memptr);
        }
        break;

    case 'n': {   // %{n}
        // The argument shall be a pointer to signed integer into which
        // is written the size of the buffer allocated by this call to
        // fprintf. No argument is converted, but one is consumed. If
        // the conversion specification includes any flags, a field
        // width, or a precision, the behavior is undefined.

        assert (0 != pbuf);
        assert (0 != *pbuf);

        const size_t nbytes = pbufsize ? *pbufsize : 0;

        spec.param.ptr = PARAM (void*, ptr);

        if (spec.mod_hh) {
            unsigned char* const ptr = (unsigned char*)spec.param.ptr;

            assert (0 != ptr);

            *ptr = (unsigned char)nbytes;
        }
        else if (spec.mod_h) {
            short* const ptr = (short*)spec.param.ptr;

            assert (0 != ptr);

            *ptr = short (nbytes);
        }
        else if (spec.mod_L) {
#ifdef _RWSTD_LONG_LONG
            _RWSTD_LONG_LONG* const ptr = (_RWSTD_LONG_LONG*)spec.param.ptr;

            assert (0 != ptr);

            *ptr = (_RWSTD_LONG_LONG)(nbytes);
#else   // if !defined (_RWSTD_LONG_LONG)
            assert (!"%{Ln} not implemented");
#endif   // _RWSTD_LONG_LONG
        }
        else if (spec.mod_l) {
            long* const ptr = (long*)spec.param.ptr;

            assert (0 != ptr);

            *ptr = long (nbytes);
        }
        else if (spec.mod_t) {
            ptrdiff_t* const ptr = (ptrdiff_t*)spec.param.ptr;

            assert (0 != ptr);

            *ptr = ptrdiff_t (nbytes);
        }
        else {
            int* const ptr = (int*)spec.param.ptr;

            assert (0 != ptr);

            *ptr = int (nbytes);
        }
        len = 0;
        break;
    }

    case 's':   // %{s}, %{Is}, %{ls}
        if (spec.mod_I) {   // ios::iostate
            spec.param.i = PARAM (int, i);
            len = rw_fmtiostate (spec, pbuf, pbufsize, spec.param.i);
        }
        else if (spec.mod_l) {   // wchar_t*
            spec.param.ptr = PARAM (wchar_t*, ptr);
            const wchar_t* const wstr = (wchar_t*)spec.param.ptr;
            len = rw_quotestr (spec, pbuf, pbufsize, wstr, _RWSTD_SIZE_MAX, 0);
        }
        else {   // char*
            spec.param.ptr = PARAM (char*, ptr);
            const char* const str = (char*)spec.param.ptr;
            len = rw_quotestr (spec, pbuf, pbufsize, str, _RWSTD_SIZE_MAX, 0);
        }
        break;

    case 'S':   // %{S}, %{lS}
        if (spec.mod_l) {   // std::wstring
            spec.param.ptr = PARAM (std::wstring*, ptr);

            const std::wstring* const pstr = (std::wstring*)spec.param.ptr;
            const wchar_t* const wstr = pstr->data ();
            const std::wstring::size_type size = pstr->size ();

            len = rw_fmtwstr (spec, pbuf, pbufsize, wstr, size);
        }
        else {   // std::string
            spec.param.ptr = PARAM (std::string*, ptr);

            const std::string* const pstr = (std::string*)spec.param.ptr;
            const char* const str = pstr->data ();
            const std::string::size_type size = pstr->size ();

            len = rw_fmtstr (spec, pbuf, pbufsize, str, size);
        }
        break;

    default:
        if (spec.strarg) {
            // environment variable
            const char* val = getenv (spec.strarg);

            if (!val)
                val = "";

            len = int (strlen (val));

            if (0 == rw_bufcat (pbuf, pbufsize, val, size_t (len)))
                return -1;

            free (spec.strarg);
        }
        else {
            assert (!"not implemented");
        }
    }

    return len;
}

/********************************************************************/

/* extern */ int
(*rw_vasnprintf_cb)(FmtSpec*, size_t, char**, size_t*, const char*, va_list*)
    = libstd_vasnprintf;

/********************************************************************/

_TEST_EXPORT int
rw_ansprintf (char **pbuf, size_t *pbufsize, const char *fmt, ...)
{
    assert (0 == pbuf || 0 == *pbuf || pbufsize);

    va_list va;
    va_start (va, fmt);

    char* buf = 0;
    size_t bufsize = 0;

    if (0 == pbuf) {
        // if pbyf is 0 (i.e., this is a request for the size of the
        // buffer necessary to format all the arguments), set pbuf to
        // point to a local buf
        pbuf = &buf;
    }

    if (0 == pbufsize) {
        // pbuf may be 0 regardless of the value of pbuf
        pbufsize = &bufsize;
    }

    const int nchars = rw_vasnprintf (pbuf, pbufsize, fmt, va);

    va_end (va);

    // verify that the length of the fomatted buffer is less than
    // its size (this test is unreliable if there are any embedded
    // NULs in the output)
    assert (nchars < 0 || strlen (*pbuf) < *pbufsize);

    if (pbuf == &buf) {
        // free the character buffer if pbuf was initially 0
        free (pbuf);
    }

    return nchars;
}

/********************************************************************/

_TEST_EXPORT char*
rw_snprintfa (char *buf, size_t bufsize, const char* fmt, ...)
{
    if (buf)
        *buf = '\0';

    va_list va;
    va_start (va, fmt);

    const int nchars = rw_vasnprintf (&buf, &bufsize, fmt, va);

    va_end (va);

    // verify that the length of the fomatted buffer is less than
    // its size (this test is unreliable if there are any embedded
    // NULs in the output)
    assert (nchars < 0 || strlen (buf) < bufsize);

    _RWSTD_UNUSED (nchars);

    return buf;
}

/********************************************************************/

_TEST_EXPORT char*
rw_sprintfa (const char *fmt, ...)
{
    char* buf = 0;
    size_t bufsize = 0;

    va_list va;
    va_start (va, fmt);

    const int nchars = rw_vasnprintf (&buf, &bufsize, fmt, va);

    va_end (va);

    // verify that the length of the fomatted buffer is less than
    // its size (this test is unreliable if there are any embedded
    // NULs in the output)
    assert (nchars < 0 || strlen (buf) < bufsize);

    _RWSTD_UNUSED (nchars);

    return buf;
}


/********************************************************************/

// avoid re-declaring these as exported here and instead rely on the
// declaration in the header #included above in order to prevent the
// bogus MSVC error:
// error C2201: 'rw_stdout' : must have external linkage in order to
// be exported/imported

/* _TEST_EXPORT */ rw_file* const
rw_stdout = _RWSTD_REINTERPRET_CAST (rw_file*, stdout);

/* _TEST_EXPORT */ rw_file* const
rw_stderr = _RWSTD_REINTERPRET_CAST (rw_file*, stderr);


_TEST_EXPORT int
rw_fprintf (rw_file *file, const char *fmt, ...)
{
    assert (0 != file);

    char buffer [256];

    char* buf = buffer;
    size_t bufsize = sizeof buffer;

    va_list va;
    va_start (va, fmt);

    const int nchars = rw_vasnprintf (&buf, &bufsize, fmt, va);

    va_end (va);

    const int nwrote = 0 < nchars ?
          fwrite (buf, 1, nchars, _RWSTD_REINTERPRET_CAST (FILE*, file))
        : nchars;

    if (buf != buffer)
        free (buf);

    return nwrote;
}

/********************************************************************/

_TEST_EXPORT int
rw_printf (const char *fmt, ...)
{
    char buffer [256];

    char* buf = buffer;
    size_t bufsize = sizeof buffer;

    va_list va;
    va_start (va, fmt);

    const int nchars = rw_vasnprintf (&buf, &bufsize, fmt, va);

    va_end (va);

    const int nwrote = 0 < nchars ? fwrite (buf, 1, nchars, stdout) : nchars;

    if (buf != buffer)
        free (buf);

    return nwrote;
}
