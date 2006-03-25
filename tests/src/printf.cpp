/************************************************************************
 *
 * printf.cpp - definitions of the snprintfa family of functions
 *
 * $Id$
 *
 ************************************************************************
 *
 * Copyright 2006 The Apache Software Foundation or its licensors,
 * as applicable.
 *
 * Copyright 2005-2006 Rogue Wave Software.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC
#include <testdefs.h>
#include <rw_printf.h>

#include <ctype.h>    // for isalpha(), ...
#include <errno.h>    // for errno, errno constants
#include <float.h>    // for floating point macros
#include <locale.h>
#include <signal.h>   // for signal constant
#include <stdarg.h>   // for va_list, va_start, ...
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _RWSTD_NO_WCHAR_H
#  include <wchar.h>
#endif   // _RWSTD_NO_WCHAR_H

#ifndef _RWSTD_NO_WCTYPE_H
#  include <wctype.h>   // for iswalpha(), ...
#endif   // _RWSTD_NO_WCTYPE_H

#if defined (_WIN32) || defined (_WIN64)
   // define macros to enable Win98 + WinNT support in <windows.h>
#  define _WIN32_WINNT 0x0410
#  define WINVER       0x400
#  include <windows.h>   // for IsDebuggerPresent()
#else
#  include <dlfcn.h>
#endif   // _WIN{32,64}

#include <ios>
#include <iostream>
#include <locale>
#include <string>


_RWSTD_NAMESPACE (__rw) {

_RWSTD_EXPORT _RWSTD_SSIZE_T
 __rw_memattr (const void*, size_t, int);

}


/********************************************************************/

static const union {
    int           ival;
    unsigned char bytes [sizeof (int)];
} _rw_one = { 1 };

static const int
_rw_big_endian = '\0' == _rw_one.bytes [0];


/********************************************************************/

struct Buffer {
    char   **pbuf;       // pointer to the output buffer
    size_t  *pbufsize;   // pointer to the size of the buffer
    size_t   maxsize;    // maximum not-to-exceed size
    size_t   endoff;     // offset of the last character
};

struct FmtSpec;

_RWSTD_INTERNAL int
_rw_fmtlong (const FmtSpec&, Buffer&, long);

#ifdef _RWSTD_LONG_LONG

_RWSTD_INTERNAL int
_rw_fmtllong (const FmtSpec&, Buffer&, _RWSTD_LONG_LONG);

#endif   // _RWSTD_LONG_LONG

static int
_rw_fmtinteger (FmtSpec*, size_t, Buffer&, va_list*);

static int
_rw_fmtfloating (const FmtSpec&, Buffer&, const void*);

_RWSTD_INTERNAL int
_rw_fmtptr (const FmtSpec&, Buffer&, const void*);

// format an invalid address
_RWSTD_INTERNAL int
_rw_fmtbadaddr (const FmtSpec&, Buffer&, const void*, size_t = 0);

typedef void (*funptr_t)();

static int
_rw_fmtfunptr (const FmtSpec&, Buffer&, funptr_t);

struct DummyStruct;
typedef void (DummyStruct::*memptr_t)() const;

static int
_rw_fmtmemptr (const FmtSpec&, Buffer&, memptr_t);

static int
_rw_fmtstr (const FmtSpec&, Buffer&, const char*, size_t);

static int
_rw_fmtchr (const FmtSpec&, Buffer&, int, int);

static int
_rw_fmtwchr (const FmtSpec&, Buffer&, wint_t, int);

static int
_rw_fmtwstr (const FmtSpec&, Buffer&, const wchar_t*, size_t);

// format errno value/name
static int
_rw_fmterrno (const FmtSpec&, Buffer&, int);

// format a character mask (alpha|alnum|...|xdigit)
static int
_rw_fmtmask (const FmtSpec&, Buffer&, int);

// format struct tm
static int
_rw_fmttm (const FmtSpec&, Buffer&, const tm*);

// format ios_base::openmode
static int
_rw_fmtopenmode (const FmtSpec&, Buffer&, int);

// format ios_base::seekdir
static int
_rw_fmtseekdir (const FmtSpec&, Buffer&, int);

// format ios_base::event
static int
_rw_fmtevent (const FmtSpec&, Buffer&, int);

static int
_rw_fmtmonpat (const FmtSpec&, Buffer&, const char [4]);

// format a signal value/name
static int
_rw_fmtsignal (const FmtSpec&, Buffer&, int);

// format an extension
static int
_rw_vasnprintf_ext (FmtSpec*, size_t, Buffer&, const char*, va_list*);

/********************************************************************/

typedef int _rw_usr_cb_t (char**, size_t*, const char*, ...);

static _rw_usr_cb_t* _rw_usr_fun [32];
static size_t        _rw_usr_inx;

/********************************************************************/

struct FmtSpec
{
    // optional flags
    unsigned fl_minus  : 1;
    unsigned fl_plus   : 1;
    unsigned fl_pound  : 1;
    unsigned fl_space  : 1;
    unsigned fl_zero   : 1;

    // optional length modifier
    enum Modifier {
        mod_none = 0,
        mod_h,       // short modifier
        mod_hh,      // char modifier
        mod_l,       // long modifier
        mod_ll,      // long long modifier
        mod_j,       // intmax_t modifier
        mod_z,       // size_t modifier
        mod_t,       // ptrdiff_t modifier
        mod_L,       // long double modifier
        mod_ext_A,   // extension: arrays
        mod_ext_I    // extension: int as ios::iostate
    };

    Modifier mod : 5;

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

    // optional field width and precision
    int width;
    int prec;

    // extension: string argument
    char *strarg;

    // required conversion specifier
    int cvtspec;

    // extension: fill character
    int fill;

#ifndef _RWSTD_NO_LONG_DOUBLE
    typedef long double ldbl_t;
#else
    typedef double ldbl_t;   // bogus (for convenience)
#endif   // _RWSTD_NO_LONG_DOUBLE

#ifdef _RWSTD_LONG_LONG
    typedef _RWSTD_LONG_LONG llong_t;
#else
    typedef long llong_t;   // bogus (for convenience)
#endif   // _RWSTD_LONG_LONG

#ifdef _RWSTD_INT64_T
    typedef _RWSTD_INT64_T i64_t;
#else
    typedef int            i64_t;   // for convenience
#endif   // _RWSTD_INT64_T

#ifdef _RWSTD_INT32_T
    typedef _RWSTD_INT32_T i32_t;
#else
    typedef int i64_t;
#endif   // _RWSTD_INT64_T

#ifdef _RWSTD_WINT_T
    typedef _RWSTD_WINT_T wint_t;
#else
    typedef int wint_t;
#endif

    typedef ::size_t   size_t;
    typedef ptrdiff_t  diff_t;
    typedef ::funptr_t funptr_t;
    typedef ::memptr_t memptr_t;

    typedef int        int_t;
    typedef long       long_t;
    typedef void*      ptr_t;
    typedef double     dbl_t;

    union {
        ldbl_t    ldbl_;
        llong_t   llong_;
        i64_t     i64_;
        ptr_t     ptr_;
        long_t    long_;
        i32_t     i32_;
        int_t     int_;
        diff_t    diff_;
        size_t    size_;
        wint_t    wint_;

        dbl_t     dbl_;
        memptr_t  memptr_;
        funptr_t  funptr_;
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
_rw_fmtspec (FmtSpec *pspec, bool ext, const char *fmt, va_list *pva)
{
    RW_ASSERT (0 != pspec);
    RW_ASSERT (0 != fmt);
    RW_ASSERT (0 != pva);

    memset (pspec, 0, sizeof *pspec);

    pspec->iwidth  = -1;   // integer width not specified
    pspec->width   = -1;   // width not specified
    pspec->prec    = -1;   // precision not specified
    pspec->base    = -1;   // base not specified
    pspec->paramno = -1;   // paramno not specified

    const char* const fmtbeg = fmt;

    if (ext) {
        if ('$' == *fmt) {

            // %{$<string>}: introduces the name of an environment
            // variable (or some other string)

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

            char* const tmp = (char*)malloc (strlen (str));
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

    // extract an optional modifier but avoid misinterpreting
    // a formatting specifier as a modifier when extensions
    // are enabled; e.g., treat the 't' in %{t} as a specifier
    // and not a modifier for a non-existent specifier
    const char mod = !ext || '}' != fmt [1] ? *fmt : '\0';

    switch (mod) {
    case 'A':
        if (ext) {
            ++fmt;
            pspec->mod = pspec->mod_ext_A;
            break;
        }
        // fall thru

    case 'h':
        if ('h' == fmt [1] && '}' != fmt [2]) {
            pspec->mod = pspec->mod_hh;
            fmt += 2;
        }
        else if ('}' != fmt [1]) {
            pspec->mod = pspec->mod_h;
            ++fmt;
        }
        break;

    case 'l':
        if ('l' == fmt [1] && '}' != fmt [2]) {
            pspec->mod = pspec->mod_ll;
            fmt += 2;
        }
        else if ('}' != fmt [1]) {
            pspec->mod = pspec->mod_l;
            ++fmt;
        }
        break;

    case 'j':
        pspec->mod = pspec->mod_j;
        ++fmt;
        break;

    case 'z':
        pspec->mod = pspec->mod_z;
        ++fmt;
        break;

    case 't':
        pspec->mod = pspec->mod_t;
        ++fmt;
        break;

    case 'L':
        pspec->mod = pspec->mod_L;
        ++fmt;
        break;

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
                pspec->mod = pspec->mod_ext_I;
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
_rw_bufcat (Buffer &buf, const char *str, size_t len)
{
    RW_ASSERT (0 != buf.pbuf);
    RW_ASSERT (0 != buf.pbufsize);

    size_t       buflen = buf.endoff;
    const size_t bufree = *buf.pbuf ? *buf.pbufsize - buflen : 0;

    if (bufree <= len || !*buf.pbuf) {

        // for guard block
        static const char guard[] = "\xde\xad\xbe\xef";

        size_t guardsize = sizeof guard - 1;

        size_t newbufsize = *buf.pbufsize * 2 + guardsize;

        if (newbufsize <= buflen + len + guardsize)
            newbufsize = 2 * (buflen + len + 1) + guardsize;

        // prevent buffer size from exceeding the maximum
        if (buf.maxsize < newbufsize)
            newbufsize = buf.maxsize;

        if (newbufsize < buflen + len + guardsize)
            guardsize = 0;

        if (newbufsize < buflen + len + guardsize) {
#ifdef ENOMEM
            errno = ENOMEM;
#endif   // ENOMEM
            return 0;
        }

        char* const newbuf = (char*)malloc (newbufsize);

        // return 0 on failure to allocate, let caller deal with it
        if (0 == newbuf)
            return 0;

        memcpy (newbuf, *buf.pbuf, buflen);

        // append a guard block to the end of the buffer
        memcpy (newbuf + newbufsize - guardsize, guard, guardsize);

        if (*buf.pbuf) {
            // verify that we didn't write past the end of the buffer
            RW_ASSERT (0 == memcmp (*buf.pbuf + *buf.pbufsize,
                                    guard, guardsize));
            free (*buf.pbuf);
        }

        *buf.pbuf     = newbuf;
        *buf.pbufsize = newbufsize - guardsize;

        (*buf.pbuf)[buflen] = '\0';
    }

    if (0 != str) {
        memcpy (*buf.pbuf + buflen, str, len);
        buflen += len;
        (*buf.pbuf)[buflen] = '\0';
    }

    buf.endoff = buflen;

    return *buf.pbuf + buflen;
}

/********************************************************************/

static int
_rw_vasnprintf_c99 (FmtSpec *pspec,      // array of processed parameters
                    size_t   paramno,    // index of current parameter
                    Buffer  &buf,
                    va_list *pva)        // parameter list
{
    _RWSTD_UNUSED (paramno);

    RW_ASSERT (0 != pspec);

    int len = -1;

    FmtSpec &spec = pspec [paramno];

#define PARAM(name)   \
  (0 < spec.paramno ? pspec [spec.paramno - 1].param.name \
                    : va_arg (*pva, FmtSpec::name ## t))

    switch (spec.cvtspec) {

    case 'b':   // extension: bool
    case 'd':
    case 'i':
    case 'o':
    case 'x':
    case 'X':
    case 'u':
        len = _rw_fmtinteger (pspec, paramno, buf, pva);
        break;

    case 'e':
    case 'E':
    case 'f':
    case 'F':
    case 'g':
    case 'G':
        if (spec.mod == spec.mod_L) {
            spec.param.ldbl_ = PARAM (ldbl_);
            len = _rw_fmtfloating (spec, buf, &spec.param.ldbl_);
        }
        else {
            spec.param.dbl_ = PARAM (dbl_);
            len = _rw_fmtfloating (spec, buf, &spec.param.dbl_);
        }
        break;

    case 'a':
        RW_ASSERT (!"%a not implemented");
        break;

    case 'A':
        RW_ASSERT (!"%A not implemented");
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
        if (spec.mod == spec.mod_l) {
            // wint_t argument formatted as wchar_t (no escapes)
            spec.param.wint_ = PARAM (wint_);
            len = _rw_fmtwchr (spec, buf, spec.param.wint_, 1);
        }
        else {
            // int argument formatted as char (no escapes)
            spec.param.int_ = PARAM (int_);
            len = _rw_fmtchr (spec, buf, spec.param.int_, 1);
        }
        break;

    case 's':
        if (spec.mod == spec.mod_l) {
            spec.param.ptr_ = PARAM (ptr_);
            const wchar_t* const str = (wchar_t*)spec.param.ptr_;
            len = _rw_fmtwstr (spec, buf, str, _RWSTD_SIZE_MAX);
        }
        else {
            spec.param.ptr_ = PARAM (ptr_);
            const char* const str = (char*)spec.param.ptr_;
            len = _rw_fmtstr (spec, buf, str, _RWSTD_SIZE_MAX);
        }
        break;

    case 'p': {
        // The argument shall be a pointer to void. The value of the pointer
        // is converted to a sequence of printing characters, in an
        // implementation-defined manner.
        spec.param.ptr_ = PARAM (ptr_);
        len = _rw_fmtptr (spec, buf, spec.param.ptr_);
        break;
    }

    case 'm': {   // %m (popular extension)
        spec.param.int_ = errno;
        len = _rw_fmterrno (spec, buf, spec.param.int_);
        break;
    }

    case 'n': {
        // The argument shall be a pointer to signed integer into which
        // is written the number of characters written to the output
        // stream so far by this call to fprintf. No argument is converted,
        // but one is consumed. If the conversion specification includes
        // any flags, a field width, or a precision, the behavior is
        // undefined.

        RW_ASSERT (0 != buf.pbuf);
        RW_ASSERT (0 != *buf.pbuf);

        // len = int (strlen (*buf.pbuf));
        len = buf.endoff;

        spec.param.ptr_ = PARAM (ptr_);

        if (spec.mod == spec.mod_hh) {
            unsigned char* const ptr = (unsigned char*)spec.param.ptr_;

            RW_ASSERT (0 != ptr);

            *ptr = len;
        }
        else if (spec.mod == spec.mod_h) {
            short* const ptr = (short*)spec.param.ptr_;

            RW_ASSERT (0 != ptr);

            *ptr = len;
        }
        else if (spec.mod == spec.mod_L) {
#ifdef _RWSTD_LONG_LONG
            _RWSTD_LONG_LONG* const ptr = (_RWSTD_LONG_LONG*)spec.param.ptr_;

            RW_ASSERT (0 != ptr);

            *ptr = short (len);
#else   // if !defined (_RWSTD_LONG_LONG)
            RW_ASSERT (!"%Ln not implemented");
#endif   // _RWSTD_LONG_LONG
        }
        else if (spec.mod == spec.mod_l) {
            long* const ptr = (long*)spec.param.ptr_;

            RW_ASSERT (0 != ptr);

            *ptr = long (unsigned (len));
        }
        else if (spec.mod == spec.mod_t) {
            ptrdiff_t* const ptr = (ptrdiff_t*)spec.param.ptr_;

            RW_ASSERT (0 != ptr);

            *ptr = ptrdiff_t (unsigned (len));
        }
        else {
            int* const ptr = (int*)spec.param.ptr_;

            RW_ASSERT (0 != ptr);

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

    va_list vacpy;
    _RWSTD_VA_COPY (vacpy, varg);

    pva = &vacpy;

// do not use varg of vacpy below this point -- use *pva instead
#define varg  DONT_TOUCH_ME
#define vacpy DONT_TOUCH_ME

    size_t default_bufsize = 1024;
    if (0 == pbufsize)
        pbufsize = &default_bufsize;

    Buffer buf = { pbuf, pbufsize, _RWSTD_SIZE_MAX, 0 };

    RW_ASSERT (0 != buf.pbuf);
    RW_ASSERT (0 != buf.pbufsize);

    // save the initial value of `pbuf'
    char* const pbuf_save = *buf.pbuf;

    // when appending to the buffer
    size_t append_offset = 0;

    // local buffer for a small number of conversion specifiers
    // will grow dynamically if their number exceeds its capacity
    FmtSpec specbuf [32];
    FmtSpec *pspec = specbuf;

    // local buffer for backtrack offsets implementing conditionals
    int backtrack [64];
    int nextoff = 0;

    char fmtspec [64];

    char *next = *buf.pbuf;

    size_t spec_bufsize = sizeof specbuf / sizeof *specbuf;
    size_t paramno = 0;

    if ('%' == fmt [0] && '{' == fmt [1] && '+' == fmt [2] && '}' == fmt [3]) {
        // when the format string begins with the special %{+}
        // directive append to the buffer instead of writing
        // over it
        fmt += 4;
        append_offset = *buf.pbuf ? strlen (*buf.pbuf) : 0;
        buf.endoff    = append_offset;
    }
    else if (*buf.pbuf)
        **buf.pbuf = '\0';

    if ('%' == fmt [0] && '{' == fmt [1]) {
        if ('*' == fmt [2] && '}' == fmt [3]) {
            const int n = va_arg (*pva, int);
            if (n < 0) {
#ifdef EINVAL
                errno = EINVAL;
#endif   // EINVAL
                goto fail;
            }

            buf.maxsize = size_t (n);
            fmt += 4;
        }
        else if (isdigit (fmt [2])) {

            char* end = 0;
            const unsigned long n = strtoul (fmt + 2, &end, 0);
            if ('}' == *end) {
                buf.maxsize = n;
                fmt         = end + 1;
            }
        }
    }

    for (const char *fc = fmt; *fc; ) {

        const char* const pcnt = strchr (fc, '%');

        size_t nchars = pcnt ? pcnt - fmt : strlen (fc);

        next = _rw_bufcat (buf, fmt, nchars);
        if (0 == next)
            goto fail;

        RW_ASSERT (0 != *buf.pbuf);
        RW_ASSERT (0 != *buf.pbufsize);

        if (0 == pcnt)
            break;

        fc = pcnt + 1;

        if ('%' == *fc) {
            // handle "%%"
            next = _rw_bufcat (buf, "%", 1);
            if (0 == next)
                goto fail;

            fmt = ++fc;
            continue;
        }

        if (spec_bufsize == paramno) {
            // grow the buffer of conversion specifiers
            FmtSpec* const tmp =
                (FmtSpec*)malloc (spec_bufsize * 2 * sizeof (FmtSpec));
            if (tmp) {
                memcpy (tmp, pspec, spec_bufsize);
                if (pspec != specbuf)
                    free (pspec);
                pspec = tmp;
            }
            else
                goto fail;

            spec_bufsize *= 2;
        }

        if ('{' == *fc) {
            const char* const endbrace = strchr (++fc, '}');

            RW_ASSERT (0 != endbrace);

            const size_t fmtlen = endbrace - fc;

            // FIXME: handle specifier strings that are longer
            RW_ASSERT (fmtlen < sizeof fmtspec);

            memcpy (fmtspec, fc, fmtlen);
            fmtspec [fmtlen] = '\0';

            // compute the length of the buffer so far
            const size_t buflen = next - *buf.pbuf;

            RW_ASSERT (paramno < spec_bufsize);

            // initiaze the current format specification, setting
            // all unused bits to 0
            const int speclen = _rw_fmtspec (pspec + paramno, true, fc, pva);

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
                _rw_vasnprintf_ext (pspec, paramno, buf, fmtspec, pva);

            // the callback returns a negative value on error
            if (len < 0)
                goto fail;

            RW_ASSERT (size_t (len) < *buf.pbufsize);
            RW_ASSERT (buf.endoff < *buf.pbufsize);

            const size_t offinx = size_t (nextoff - 1);

            if (pspec [paramno].cond_end && pspec [paramno].cond_begin) {
                // change from an if to an else clause

                RW_ASSERT (0 < nextoff);
                RW_ASSERT (0 == len);
                RW_ASSERT (offinx < sizeof backtrack / sizeof *backtrack);

                if (pspec [paramno].cond_true) {
                    // change from an inactive if to an active else
                    // (same as the end of an inactive clause)

                    RW_ASSERT (0 <= backtrack [offinx]);

                    // set the length so as to backtrack to the position
                    // saved on the top of the backtrack stack 
                    len = -int (buflen) + backtrack [offinx];
                    buf.endoff += len;

                    // invert the offset to indicate an active clause
                    backtrack [offinx] = ~backtrack [offinx];
                }
                else {
                    // change from an active if to an inactive else
                    RW_ASSERT (backtrack [offinx] < 0);

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

                // store the value of the conditional expression
                // extracted from the argument list
                ++paramno;
            }
            else if (pspec [paramno].cond_end) {
                // the end of an if/else clause

                if (!pspec [paramno].cond_true) {
                    // the end of an inactive clause

                    RW_ASSERT (offinx < sizeof backtrack / sizeof *backtrack);
                    RW_ASSERT (backtrack [offinx] <= int (buflen));

                    // set the length so as to backtrack to the position
                    // saved on the top of the backtrack stack 
                    len = -int (buflen) + backtrack [offinx];
                    buf.endoff += len;
                }

                // pop it off the top of the stack
                --nextoff;
            }
            else {
                // store the value of the argument extracted from the list
                ++paramno;
            }

            RW_ASSERT (len + buflen < *buf.pbufsize);

            // adjust the next pointer to point to the terminating
            // NUL in the (possibly reallocated) buffer
            next = *buf.pbuf + buflen + len;

            RW_ASSERT (next == *buf.pbuf + buf.endoff);

            fc = endbrace + 1;
        }
        else {
            const int speclen =
                _rw_fmtspec (pspec + paramno, false, fc, pva);

            if (speclen) {

                const int len = _rw_vasnprintf_c99 (pspec, paramno, buf, pva);

                if (-1 == len)
                    goto fail;

                // store the argument extracted from the argument list
                // but discard positional specifiers
                if (-1 == pspec [paramno].paramno)
                    ++paramno;

                next += len;
                fc   += speclen;
            }
            else {
                next = _rw_bufcat (buf, "%", 1);
                if (0 == next)
                    goto fail;
            }
        }

        fmt = fc;
    }

    // deallocate if dynamically allocated
    if (pspec != specbuf)
        free (pspec);

    // NUL-terminate
    next = _rw_bufcat (buf, "", 1);
    if (0 == next)
        goto fail;

    // return the number of characters appended to the buffer
    // not including the terminating NUL
    return int ((next - *buf.pbuf) - append_offset - 1);

fail: // function failed

    fprintf (stderr, "%s:%d: rw_vasnprintf(%p, %p, \"%s\", va_list) "
             "error: errno = %d: %s\n",
             __FILE__, __LINE__, (void*)buf.pbuf, (void*)buf.pbufsize, fmt,
             errno, strerror (errno));

    if (pspec != specbuf)
        free (pspec);

    if (*buf.pbuf != pbuf_save) {
        // free any allocated memory
        free (*buf.pbuf);
        *buf.pbuf = 0;
    }

    return -1;

#undef varg
#undef vacpy

}

/********************************************************************/

static const char _rw_digits[] = {
    "0123456789abcdefghijklmnopqrstuvwxyz"
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
};

_RWSTD_INTERNAL int
_rw_fmtlong (const FmtSpec &spec, Buffer &buf, long val)
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
        *end++ = _rw_digits [upoff + uval % base];
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

    // not NUL-terminated
    RW_ASSERT (buffer <= end);
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
    return _rw_fmtstr (newspec, buf, buffer, size_t (size));
}

/********************************************************************/

#ifdef _RWSTD_LONG_LONG

_RWSTD_INTERNAL int
_rw_fmtllong (const FmtSpec &spec, Buffer &buf, _RWSTD_LONG_LONG val)
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
        *end++ = _rw_digits [upoff + uval % base];
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

    RW_ASSERT (buffer < end);
    size_t size = size_t (end - buffer);

    // FIXME: prevent buffer overrun
    if (0 < spec.prec && size < size_t (spec.prec)) {
        for (size_t i = size; i != size_t (spec.prec); ++i)
            *end++ = '0';
    }

    if (sign)
        *end++ = sign;

    // not NUL-terminated
    RW_ASSERT (buffer < end);
    size = size_t (end - buffer);

    for (char *pc = buffer; pc < end; ++pc) {
        const char tmp = *pc;
        *pc = *--end;
        *end = tmp;
    }

    // handle justification by formatting the resulting string
    return _rw_fmtstr (spec, buf, buffer, size);
}

#endif   // _RWSTD_LONG_LONG

/********************************************************************/

template <class IntT>
inline int
rw_fmtinteger (const FmtSpec &spec, Buffer &buf, IntT val)
{
#ifdef _RWSTD_LONG_LONG

    typedef _RWSTD_LONG_LONG LLong;

    const int len = sizeof (val) <= sizeof (long) ?
          ::_rw_fmtlong (spec, buf, long (val))
        : ::_rw_fmtllong (spec, buf, LLong (val));

#else   // if !defined (_RWSTD_LONG_LONG)

    const int len = ::_rw_fmtlong (spec, buf, long (val));

#endif   // _RWSTD_LONG_LONG

    return len;
}

/********************************************************************/

static int
_rw_fmtinteger (FmtSpec *pspec, size_t paramno, Buffer &buf, va_list *pva)
{
    int len = -1;

    FmtSpec &spec = pspec [paramno];

    switch (spec.cvtspec) {
    case 'b': // extension: int argument formatted as bool
        spec.param.int_ = PARAM (int_);
        len = _rw_fmtstr (spec, buf,
                          spec.param.int_ ? "true" : "false",
                          _RWSTD_SIZE_MAX);
        break;
        
    case 'd':
    case 'i':
        if (spec.mod == spec.mod_hh) {
            // promoted signed char argument
            spec.param.int_ = PARAM (int_);
            const signed char val = spec.param.int_;
            len = rw_fmtinteger (spec, buf, val);
        }
        else if (spec.mod == spec.mod_h) {
            // promoted signed short argument
            spec.param.int_ = PARAM (int_);
            const short val = spec.param.int_;
            len = rw_fmtinteger (spec, buf, val);
        }
        else if (spec.mod == spec.mod_l) {   // %li
            spec.param.long_ = PARAM (long_);
            len = rw_fmtinteger (spec, buf, spec.param.long_);
        }
        else if (spec.mod == spec.mod_ll) {   // %lli

#ifdef _RWSTD_LONG_LONG
            spec.param.llong_ = PARAM (llong_);
            len = rw_fmtinteger (spec, buf, spec.param.llong_);
#elif 8 == _RWSTD_LONG_SIZE
            spec.param.long_ = PARAM (long_);
            len = rw_fmtinteger (spec, buf, spec.param.long_);
#else
            RW_ASSERT (!"%lld, %lli: long long not supported");

#endif   // _RWSTD_LONG_LONG
        }
        else if (spec.mod == spec.mod_t) {
            spec.param.diff_ = PARAM (diff_);
            len = rw_fmtinteger (spec, buf, spec.param.diff_);
        }
        else if (1 == spec.iwidth) {
            spec.param.int_ = PARAM (int_);
            const _RWSTD_INT8_T val = spec.param.int_;
            len = rw_fmtinteger (spec, buf, val);
        }
        else if (2 == spec.iwidth) {
            spec.param.int_ = PARAM (int_);
            const _RWSTD_INT16_T val = spec.param.int_;
            len = rw_fmtinteger (spec, buf, val);
        }
        else if (3 == spec.iwidth) {
            spec.param.i32_ = PARAM (i32_);
            const long val = long (spec.param.i32_);
            len = rw_fmtinteger (spec, buf, val);
        }
        else if (4 == spec.iwidth) {

#ifdef _RWSTD_INT64_T
            spec.param.i64_ = PARAM (i64_);
#else   // if !defined (_RWSTD_INT64_T)
            RW_ASSERT (!"%I64d, %I64i: 64-bit types not supported");
#endif   // _RWSTD_INT64_T

#if 8 == _RWSTD_LONG_SIZE
            const long val = spec.param.i64_;
            len = rw_fmtinteger (spec, buf, val);
#elif defined (_RWSTD_LONG_LONG)
            const _RWSTD_LONG_LONG val = spec.param.i64_;
            len = rw_fmtinteger (spec, buf, val);
#else
            RW_ASSERT (!"%I64d, %I64i: 64-bit types not supported");
#endif
        }
        else {   // %i
            spec.param.int_ = PARAM (int_);
            len = rw_fmtinteger (spec, buf, spec.param.int_);
        }
        break;

    case 'o':
        RW_ASSERT (-1 == spec.base);
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
        if (spec.mod == spec.mod_hh) {
            // promoted unsigned char argument
            spec.param.int_ = PARAM (int_);
            const unsigned char val = spec.param.int_;
            len = rw_fmtinteger (spec, buf, val);
        }
        else if (spec.mod == spec.mod_h) {
            // promoted unsigned short argument
            spec.param.int_ = PARAM (int_);
            const unsigned short val = spec.param.int_;
            len = rw_fmtinteger (spec, buf, val);
        }
        else if (spec.mod == spec.mod_ll) {
#ifdef _RWSTD_LONG_LONG
            spec.param.llong_ = PARAM (llong_);
            const unsigned _RWSTD_LONG_LONG val = spec.param.llong_;
            len = rw_fmtinteger (spec, buf, val);
#elif 8 == _RWSTD_LONG_SIZE
            spec.param.long_ = PARAM (long_);
            const unsigned long val = spec.param.long_;
            len = rw_fmtinteger (spec, buf, val);
#else
            RW_ASSERT (!"long long not supported");
#endif   // _RWSTD_LONG_LONG

        }
        else if (spec.mod == spec.mod_l) {
            spec.param.long_ = PARAM (long_);
            const unsigned long val = spec.param.long_;
            len = rw_fmtinteger (spec, buf, val);
        }
        else if (spec.mod == spec.mod_t) {
            spec.param.size_ = PARAM (size_);
            len = rw_fmtinteger (spec, buf, spec.param.size_);
        }
        else if (1 == spec.iwidth) {
            spec.param.int_ = PARAM (int_);
            const _RWSTD_UINT8_T val = spec.param.int_;
            len = rw_fmtinteger (spec, buf, val);
        }
        else if (2 == spec.iwidth) {
            spec.param.int_ = PARAM (int_);
            const long val = (unsigned short)spec.param.int_;
            len = rw_fmtinteger (spec, buf, val);
        }
        else if (3 == spec.iwidth) {
            spec.param.i32_ = PARAM (i32_);
            const long val = long (unsigned (spec.param.int_));
            len = rw_fmtinteger (spec, buf, val);
        }
        else if (4 == spec.iwidth) {
#ifdef _RWSTD_INT64_T
            spec.param.i64_ = PARAM (i64_);
#else   // if defined 9_RWSTD_INT64_T)
            RW_ASSERT (!"%I64o, %I64u, %I64x: 64-bit types not supported");
#endif   // _RWSTD_INT64_T

#if 8 == _RWSTD_LONG_SIZE
            const unsigned long val = spec.param.i64_;
            len = rw_fmtinteger (spec, buf, val);
#elif defined (_RWSTD_LONG_LONG)
            const unsigned _RWSTD_LONG_LONG val = spec.param.i64_;
            len = rw_fmtinteger (spec, buf, val);
#else
            RW_ASSERT (!"%I64o, %I64u, %I64x: 64-bit types not supported");
#endif
        }
        else {
            spec.param.int_ = PARAM (int_);
            const unsigned val = spec.param.int_;
            len = rw_fmtinteger (spec, buf, val);
        }
        
        break;
    }

    return len;
}

/********************************************************************/

static int
_rw_fmtfloating (const FmtSpec &spec, Buffer &buf, const void *pval)
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

    if (spec.mod == spec.mod_h)
        *pf++ = 'h';
    else if (spec.mod == spec.mod_hh) {
        *pf++ = 'h';
        *pf++ = 'h';
    }
    else if (spec.mod == spec.mod_l)
        *pf++ = 'l';
    else if (spec.mod == spec.mod_ll) {
        *pf++ = 'l';
        *pf++ = 'l';
    }
    else if (spec.mod == spec.mod_j)
        *pf++ = 'j';
    else if (spec.mod == spec.mod_z)
        *pf++ = 'z';
    else if (spec.mod == spec.mod_t)
        *pf++ = 't';
    else if (spec.mod == spec.mod_L) {
        strcpy (pf, _RWSTD_LDBL_PRINTF_PREFIX);
        for ( ; *pf; ++pf);
    }
    else if (spec.mod == spec.mod_ext_A && _RWSTD_LDBL_SIZE == spec.width) {
        strcpy (pf, _RWSTD_LDBL_PRINTF_PREFIX);
        for ( ; *pf; ++pf);
    }

    if (spec.mod != spec.mod_ext_A && 0 <= spec.width) {
        pf += sprintf (pf, "%i", spec.width);
    }

    if (0 <= spec.prec)
        pf += sprintf (pf, ".%i", spec.prec);

    *pf++ = char (spec.cvtspec);
    *pf   = '\0';

    // verify that the format buffer hasn't overflowed
    RW_ASSERT (size_t (pf - fmt) + 1 < sizeof fmt);

    // this might make the buffer almost 5KB
    char buffer [_RWSTD_LDBL_MAX_10_EXP + _RWSTD_LDBL_DIG + 3];
    int len = -1;

    if (spec.mod == spec.mod_ext_A) {

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
            RW_ASSERT (!"unknown floating point size");
        }
    }
    else if (spec.mod == spec.mod_L)
        len = sprintf (buffer, fmt, *(const long double*)pval);
    else
        len = sprintf (buffer, fmt, *(const double*)pval);

    RW_ASSERT (size_t (len) < sizeof buffer);

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


    if (-1 < len && 0 == _rw_bufcat (buf, buffer, size_t (len)))
        return -1;

    return len;
}

/********************************************************************/

// formats a data, function, or class member pointer as follows
//     0x<hex-long-0>[:<hex-long-1>[:...[:<hex-long-N>]]]
// where N is the size of the pointer in long ints


static int
_rw_fmtpointer (const FmtSpec &spec, Buffer &buf,
                const void *pptr, size_t nelems)
{
    FmtSpec newspec (spec);

    // always format data pointers in hexadecimal
    newspec.base = 16;

    // set the number of digits
    newspec.prec = _RWSTD_LONG_SIZE * 2;

    const union {
        const void          *ptr;
        const unsigned long *lptr;
    } uptr = { pptr };

    int len = 0;

    if (newspec.fl_pound) {
        // prepend the 0x prefix even to null pointers
        if (0 == _rw_bufcat (buf, "0x", size_t (len = 2))) {
            return -1;
        }

        // reset the pound flag to prevent the integer formatter
        // from inserting it again
        newspec.fl_pound  = 0;
    }

    for (size_t i = 0; i != nelems; ++i) {
        const size_t inx = _rw_big_endian ? nelems - i - 1 : i;

        len += _rw_fmtlong (newspec, buf, uptr.lptr [inx]);

        if (len < 0) {
            break;
        }

        // separate pointer components with colons
        int n = 0;
        if (i + 1 < nelems) {
            if (0 == _rw_bufcat (buf, ":", size_t (n = 1))) {
                len = -1;
                break;
            }
        }

        len += n;
    }

    return len;
}

/********************************************************************/

_RWSTD_INTERNAL int
_rw_fmtptr (const FmtSpec &spec, Buffer &buf, const void *val)
{
    return _rw_fmtpointer (spec, buf, &val,
                           sizeof val / sizeof (long));
}

/********************************************************************/

_RWSTD_INTERNAL int
_rw_fmtbadaddr (const FmtSpec &spec, Buffer &buf,
                const void *addr, size_t objsize /* = 0 */)
{
    if (0 == addr)
        return _rw_bufcat (buf, "(null)", 6) ? 6 : -1;

    const size_t off = buf.endoff;

    const size_t num_addr = (size_t)addr;

    if (objsize && (num_addr & (objsize - 1))) {
        if (0 == _rw_bufcat (buf, "(unaligned address ", 19))
            return -1;
    }
    else if (0 == _rw_bufcat (buf,    "(invalid address ", 17))
        return -1;

    FmtSpec newspec (spec);
    newspec.fl_pound = 1;

    if (-1 == _rw_fmtptr (newspec, buf, addr))
        return -1;

    if (0 == _rw_bufcat (buf, ")", 1))
        return -1;

    return buf.endoff - off;
}

/********************************************************************/

static int
_rw_fmtfunptr (const FmtSpec &spec, Buffer &buf, funptr_t val)
{
    if (spec.mod == spec.mod_l) {

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
            newspec.mod_l = newspec.mod_none;

            return _rw_fmtstr (newspec, buf, buffer, size_t (len));
        }

#endif   // _RWSTD_OS_SUNOS
#endif   // 0/1
    }

    return _rw_fmtpointer (spec, buf, &val,
                           sizeof val / sizeof (long));
}

/********************************************************************/

static int
_rw_fmtmemptr (const FmtSpec &spec, Buffer &buf, memptr_t val)
{
    return _rw_fmtpointer (spec, buf, &val,
                           sizeof val / sizeof (long));
}

/********************************************************************/

static int
_rw_fmterrno (const FmtSpec &spec, Buffer &buf, int val)
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

        if (0 == _rw_bufcat (buf, name, size_t (len)))
            return -1;

        return len;
    }

    const char* const str = strerror (val);
    const size_t len = strlen (str);

    if (0 == _rw_bufcat (buf, str, len))
        return -1;

    return int (len);
}

/********************************************************************/

static int
_rw_fmtmask (const FmtSpec &spec, Buffer &buf, int c)
{
    enum {
        bit_alnum  = 1,
        bit_alpha  = 1 << 1,
        bit_cntrl  = 1 << 2,
        bit_digit  = 1 << 3,
        bit_graph  = 1 << 4,
        bit_lower  = 1 << 5,
        bit_print  = 1 << 6,
        bit_punct  = 1 << 7,
        bit_space  = 1 << 8,
        bit_upper  = 1 << 9,
        bit_xdigit = 1 << 10
    };

    int mask = 0;

    if (spec.mod == spec.mod_l) {

#ifndef _RWSTD_NO_WCHAR_H

        mask |= iswalnum (c) ? bit_alnum : 0;
        mask |= iswalpha (c) ? bit_alpha : 0;
        mask |= iswcntrl (c) ? bit_cntrl : 0;
        mask |= iswdigit (c) ? bit_digit : 0;
        mask |= iswgraph (c) ? bit_graph : 0;
        mask |= iswlower (c) ? bit_lower : 0;
        mask |= iswprint (c) ? bit_print : 0;
        mask |= iswpunct (c) ? bit_punct : 0;
        mask |= iswspace (c) ? bit_space : 0;
        mask |= iswupper (c) ? bit_upper : 0;
        mask |= iswxdigit (c) ? bit_xdigit : 0;

#endif   // _RWSTD_NO_WCHAR_H

    }
    else {
        typedef unsigned char UChar;

        const UChar uc = c;

        mask |= isalnum (uc) ? bit_alnum : 0;
        mask |= isalpha (uc) ? bit_alpha : 0;
        mask |= iscntrl (uc) ? bit_cntrl : 0;
        mask |= isdigit (uc) ? bit_digit : 0;
        mask |= isgraph (uc) ? bit_graph : 0;
        mask |= islower (uc) ? bit_lower : 0;
        mask |= isprint (uc) ? bit_print : 0;
        mask |= ispunct (uc) ? bit_punct : 0;
        mask |= isspace (uc) ? bit_space : 0;
        mask |= isupper (uc) ? bit_upper : 0;
        mask |= isxdigit (uc) ? bit_xdigit : 0;
    }

    char mask_str [80];
    char *str = mask_str;

    str [0] = '\0';

#define APPEND(bit)                             \
    if (mask & bit_ ## bit)                     \
        strcat (strcat (str, #bit), "|");       \
    else (void)0

    APPEND (alnum);
    APPEND (alpha);
    APPEND (cntrl);
    APPEND (digit);
    APPEND (graph);
    APPEND (lower);
    APPEND (print);
    APPEND (punct);
    APPEND (space);
    APPEND (upper);
    APPEND (xdigit);

    if (str == mask_str)
        *str = '\0';
    else
        str [-1] = '\0';

    const size_t len = strlen (str);
    if (0 == _rw_bufcat (buf, str, len))
        return -1;

    return int (len);
}

/********************************************************************/

static int
_rw_fmttm (const FmtSpec &spec, Buffer &buf, const tm *tmb)
{
    if (   0 == tmb || 0 > _RW::__rw_memattr (tmb, sizeof *tmb, 0)
        || (size_t)tmb & (sizeof (int) - 1)) {
        return _rw_fmtbadaddr (spec, buf, tmb, sizeof (int));
    }

    static const char* const months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    static const char* const wdays[] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    char vbuf [1024];   // verbose format

    int verbose = spec.fl_pound;
    int invalid = 0;

    if (   tmb->tm_sec  < 0 ||  60 < tmb->tm_sec
        || tmb->tm_min  < 0 ||  59 < tmb->tm_min
        || tmb->tm_hour < 0 ||  23 < tmb->tm_hour
        || tmb->tm_wday < 0 ||   6 < tmb->tm_wday
        || tmb->tm_mday < 1 ||  31 < tmb->tm_mday
        || _RWSTD_INT_MAX - 1900 < tmb->tm_year
        || tmb->tm_mon  < 0 ||  11 < tmb->tm_mon
        || tmb->tm_yday < 0 || 365 < tmb->tm_yday) {
        invalid = 1;
        verbose = 1;
    }

    if (!invalid) {
        strcpy (vbuf, wdays [tmb->tm_wday]);
        strcat (vbuf, " ");
        strcat (vbuf, months [tmb->tm_mon]);
        strcat (vbuf, " ");

        sprintf (vbuf + strlen (vbuf), "%2d %02d:%02d:%02d ",
                 tmb->tm_mday, tmb->tm_hour, tmb->tm_min, tmb->tm_sec);

        if (tmb->tm_isdst)
            strcat (vbuf, "DST ");

        if (tmb->tm_year < -1899) {
            sprintf (vbuf + strlen (vbuf), "%d BC",
                     -(tmb->tm_year + 1899));
        }
        else {
            sprintf (vbuf + strlen (vbuf), "%d",
                     tmb->tm_year + 1900);
        }
    }

    if (verbose) {
        static const char comma[] = ", ";
        const char *sep = "";

        strcpy (vbuf, "{ ");

        if (tmb->tm_sec) {
            sprintf (vbuf + strlen (vbuf), "%stm_sec=%d", sep, tmb->tm_sec);
            if (tmb->tm_sec < 0 || 60 < tmb->tm_sec)
                strcat (vbuf, " [0,60]");
            sep = comma;
        }
            
        if (tmb->tm_min) {
            sprintf (vbuf + strlen (vbuf), "%stm_min=%d", sep, tmb->tm_min);
            if (tmb->tm_min < 0 || 59 < tmb->tm_min)
                strcat (vbuf, " [0,59]");
            sep = comma;
        }

        if (tmb->tm_hour) {
            sprintf (vbuf + strlen (vbuf), "%stm_hour=%d", sep, tmb->tm_hour);
            if (tmb->tm_hour < 0 || 23 < tmb->tm_hour)
                strcat (vbuf, " [0,23]");
            sep = comma;
        }

        sprintf (vbuf + strlen (vbuf), "%stm_mday=%d", sep, tmb->tm_mday);
        if (tmb->tm_mday < 1 || 31 < tmb->tm_mday)
            strcat (vbuf, " [1,31]");
        sep = comma;

        if (tmb->tm_mon) {
            sprintf (vbuf + strlen (vbuf), "%stm_mon=%d", sep, tmb->tm_mon);
            if (tmb->tm_mon < 0 || 11 < tmb->tm_mon)
                strcat (vbuf, " [0,11]");
            else {
                sprintf (vbuf + strlen (vbuf), " %s",
                         months [unsigned (tmb->tm_mon) % 7]);
            }
        }

        if (tmb->tm_year) {
            sprintf (vbuf + strlen (vbuf), "%stm_year=%d", sep, tmb->tm_year);
            if (_RWSTD_INT_MAX - 1900 < tmb->tm_year)
                sprintf (vbuf + strlen (vbuf), " [%d,%d]",
                         _RWSTD_INT_MIN, _RWSTD_INT_MAX - 1900);
        }

        if (tmb->tm_wday) {
            sprintf (vbuf + strlen (vbuf), "%stm_wday=%d", sep, tmb->tm_wday);
            if (tmb->tm_wday < 0 || 6 < tmb->tm_wday)
                strcat (vbuf, " [0,6]");
            else {
                sprintf (vbuf + strlen (vbuf), " %s",
                         wdays [unsigned (tmb->tm_wday) % 7]);
            }
        }

        if (tmb->tm_yday) {
            sprintf (vbuf + strlen (vbuf), "%stm_yday=%d", sep, tmb->tm_yday);
            if (tmb->tm_yday < 0 || 365 < tmb->tm_yday)
                strcat (vbuf, " [0,365]");
        }

        if (tmb->tm_isdst)
            sprintf (vbuf + strlen (vbuf), "%stm_isdst=%d", sep, tmb->tm_isdst);

        strcat (vbuf, " }");
    }

    const size_t len = strlen (vbuf);

    if (0 == _rw_bufcat (buf, vbuf, len))
        return -1;

    return int (len);
}

/********************************************************************/

static int
_rw_fmtsignal (const FmtSpec &spec, Buffer &buf, int val)
{
    static const struct {
        int         val;
        const char* str;
    } names[] = {

#undef SIGNAL
#define SIGNAL(val)   { val, #val }

#ifdef SIGABRT
        SIGNAL (SIGABRT),
#endif   // SIGABRT
#ifdef SIGALRM
        SIGNAL (SIGALRM),
#endif   // SIGALRM
#ifdef SIGBUS
        SIGNAL (SIGBUS),
#endif   // SIGBUS
#ifdef SIGCANCEL
        SIGNAL (SIGCANCEL),
#endif   // SIGCANCEL
#ifdef SIGCHLD
        SIGNAL (SIGCHLD),
#endif   // SIGCHLD
#ifdef SIGCKPT
        SIGNAL (SIGCKPT),
#endif   // SIGCKPT
#ifdef SIGCLD
        SIGNAL (SIGCLD),
#endif   // SIGCLD
#ifdef SIGCONT
        SIGNAL (SIGCONT),
#endif   // SIGCONT
#ifdef SIGDIL
        SIGNAL (SIGDIL),
#endif   // SIGDIL
#ifdef SIGEMT
        SIGNAL (SIGEMT),
#endif   // SIGEMT
#ifdef SIGFPE
        SIGNAL (SIGFPE),
#endif   // SIGFPE
#ifdef SIGFREEZE
        SIGNAL (SIGFREEZE),
#endif   // SIGFREEZE
#ifdef SIGGFAULT
        SIGNAL (SIGGFAULT),
#endif   // SIGGFAULT
#ifdef SIGHUP
        SIGNAL (SIGHUP),
#endif   // SIGHUP
#ifdef SIGILL
        SIGNAL (SIGILL),
#endif   // SIGILL
#ifdef SIGINFO
        SIGNAL (SIGINFO),
#endif   // SIGINFO
#ifdef SIGINT
        SIGNAL (SIGINT),
#endif   // SIGINT
#ifdef SIGIO
        SIGNAL (SIGIO),
#endif   // SIGIO
#ifdef SIGIOT
        SIGNAL (SIGIOT),
#endif   // SIGIOT
#ifdef SIGK32
        SIGNAL (SIGK32),
#endif   // SIGK32
#ifdef SIGKILL
        SIGNAL (SIGKILL),
#endif   // SIGKILL
#ifdef SIGLOST
        SIGNAL (SIGLOST),
#endif   // SIGLOST
#ifdef SIGLWP
        SIGNAL (SIGLWP),
#endif   // SIGLWP
#ifdef SIGPIPE
        SIGNAL (SIGPIPE),
#endif   // SIGPIPE
#ifdef SIGPOLL
        SIGNAL (SIGPOLL),
#endif   // SIGPOLL
#ifdef SIGPROF
        SIGNAL (SIGPROF),
#endif   // SIGPROF
#ifdef SIGPTINTR
        SIGNAL (SIGPTINTR),
#endif   // SIGPTINTR
#ifdef SIGPTRESCHED
        SIGNAL (SIGPTRESCHED),
#endif   // SIGPTRESCHED
#ifdef SIGPWR
        SIGNAL (SIGPWR),
#endif   // SIGPWR
#ifdef SIGQUIT
        SIGNAL (SIGQUIT),
#endif   // SIGQUIT
#ifdef SIGRESTART
        SIGNAL (SIGRESTART),
#endif   // SIGRESTART
#ifdef SIGRESV
        SIGNAL (SIGRESV),
#endif   // SIGRESV
#ifdef SIGSEGV
        SIGNAL (SIGSEGV),
#endif   // SIGSEGV
#ifdef SIGSTKFLT
        SIGNAL (SIGSTKFLT),
#endif   // SIGSTKFLT
#ifdef SIGSTOP
        SIGNAL (SIGSTOP),
#endif   // SIGSTOP
#ifdef SIGSYS
        SIGNAL (SIGSYS),
#endif   // SIGSYS
#ifdef SIGTERM
        SIGNAL (SIGTERM),
#endif   // SIGTERM
#ifdef SIGTHAW
        SIGNAL (SIGTHAW),
#endif   // SIGTHAW
#ifdef SIGTRAP
        SIGNAL (SIGTRAP),
#endif   // SIGTRAP
#ifdef SIGTSTP
        SIGNAL (SIGTSTP),
#endif   // SIGTSTP
#ifdef SIGTTIN
        SIGNAL (SIGTTIN),
#endif   // SIGTTIN
#ifdef SIGTTOU
        SIGNAL (SIGTTOU),
#endif   // SIGTTOU
#ifdef SIGUNUSED
        SIGNAL (SIGUNUSED),
#endif   // SIGUNUSED
#ifdef SIGURG
        SIGNAL (SIGURG),
#endif   // SIGURG
#ifdef SIGUSR1
        SIGNAL (SIGUSR1),
#endif   // SIGUSR1
#ifdef SIGUSR2
        SIGNAL (SIGUSR2),
#endif   // SIGUSR2
#ifdef SIGVTALRM
        SIGNAL (SIGVTALRM),
#endif   // SIGVTALRM
#ifdef SIGWAITING
        SIGNAL (SIGWAITING),
#endif   // SIGWAITING
#ifdef SIGWINCH
        SIGNAL (SIGWINCH),
#endif   // SIGWINCH
#ifdef SIGWINDOW
        SIGNAL (SIGWINDOW),
#endif   // SIGWINDOW
#ifdef SIGXCPU
        SIGNAL (SIGXCPU),
#endif   // SIGXCPU
#ifdef SIGXFSZ
        SIGNAL (SIGXFSZ),
#endif   // SIGXFSZ
#ifdef SIGXRES
        SIGNAL (SIGXRES),
#endif   // SIGXRES
        { -1, 0 }
    };

    char buffer [64];
    const char *name = 0;

    for (size_t i = 0; i != sizeof names / sizeof *names; ++i) {
        if (names [i].val == val) {
            name = names [i].str;
            break;
        }
    }

    if (0 == name) {
        sprintf (buffer, "SIG#%d", val);
        name = buffer;
    }

    return _rw_fmtstr (spec, buf, name, _RWSTD_SIZE_MAX);
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
            buf [0] = char (wc);
            buf [1] = '\0';
            len = 1;
        }
    }

    return len;
}


enum {
    A_ESC   = 1,        // use escape sequences
    A_CHAR  = 1 << 1,   // format each element as a char
    A_WCHAR = 1 << 2,   // format each element as a wchar_t
    A_ARRAY = 1 << 3    // generic array formatting
};

template <class elemT>
int rw_fmtarray (const FmtSpec &spec,
                 Buffer        &buf,
                 const elemT   *array,   // pointer to first element
                 size_t         nelems,  // number of elements
                 int            flags)
{
    RW_ASSERT (0 != buf.pbuf);

    if (   0 == array || 0 > _RW::__rw_memattr (array, _RWSTD_SIZE_MAX, 0)
        || ((size_t)array & (sizeof *array - 1))) {
        // qualify the name of the static function in order to
        // allow it to be found when referenced from a template
        return ::_rw_fmtbadaddr (spec, buf, array, sizeof *array);
    }

    if (_RWSTD_SIZE_MAX == nelems) {
        // compute the length of the NUL-terminate string
        nelems = 0;
        for (const elemT *pc = array; *pc; ++pc, ++nelems);
    }

    // separator string to insert between numerical elements
    const char* const sep = flags & (A_CHAR | A_WCHAR) ?
        "" : spec.strarg ? spec.strarg : ",";

    // length of the separator string
    const size_t seplen = strlen (sep);

    // compute the size of the buffer to preallocate
    const size_t bufsize = (nelems + 1) * (65 + seplen) + 3;

    // preallocate a buffer large enough for the formatted array
    char *next = _rw_bufcat (buf, 0, bufsize);
    const char* const bufend = next;

    if (0 == next)
        return -1;

    if (0 == nelems) {
        if (!(flags & A_ESC)) {

#if 0   // width handling disabled (width used for array formatting)
            for (int w = 0; w < spec.width; ++w)
                *next++ = ' ';
#endif   // 0/1

        }
        else {
            // append 'L' for a generic array (i.e., one whose type
            // is not known to be wchar_t) that is the same size as
            // wchar_t (bot not for an array of wchar_t's being
            // formatted using the {%ls} directive -- the caller
            // can easily stick the 'L' there themselves)
            if (flags & A_ARRAY && _RWSTD_WCHAR_T_SIZE == sizeof (elemT))
                *next++ = 'L';

            *next++ = '"';
#if 0   // width handling disabled (width used for array formatting)
            for (int w = 0; w < spec.width; ++w)
                 *next++ = ' ';
#endif   // 0/1
            *next++ = '"';
        }
        buf.endoff += size_t (next - bufend);
        return int (next - bufend);
    }

    char *s = next;

    const elemT *last = array;

    bool any_repeats = false;
    long last_repeat = flags & A_ESC ? -1L : 0L;

    // large enough for a 64-bit integer in binary notation
    char elemstr [65];

    const ptrdiff_t N = ptrdiff_t (flags & A_ESC ? 20 : 0);

    for (const elemT *pelem = last + 1; ; ++pelem) {

        if (*pelem == *last && size_t (pelem - array) < nelems) {
            // if the last processed element repeats, continue
            // until a different element is encountered
            continue;
        }

        if (N > 1 && pelem - last > N) {

            // if the last processed element repeats N or more
            // times, format the repeat count instead of all the
            // repeated occurrences of the element to conserve
            // space and make the string more readable

            const long repeat = pelem - last;

            if (flags & (A_CHAR | A_WCHAR)) {
                // format element into elemstr as a character
                rw_quotechar (elemstr, *last, !(flags & A_ESC));

                s += sprintf (s, "%s'%s' <repeats %ld times>",
                                -1 == last_repeat ? ""
                              :  0 == last_repeat ? "\", " : ", ",
                              elemstr, repeat);
            }
            else {
                // format element into elemstr as an integer
                char *localbuf = elemstr;
                size_t localbufsize = sizeof elemstr;

                Buffer bufspec = {
                    &localbuf, &localbufsize, sizeof elemstr, 0
                };

                *localbuf = '\0';
                const int intlen = rw_fmtinteger (spec, bufspec, *last);

                // no error should happen
                RW_ASSERT (0 < intlen);

                localbuf [intlen] = '\0';

                // verify that rw_fmtinteger didn't try to reallocate
                RW_ASSERT (localbuf == elemstr);

                s += sprintf (s, "%s%s <repeats %ld times>%s",
                              last_repeat < 1 ? "" : sep, elemstr, repeat, sep);
            }

            last = pelem;

            any_repeats = true;
            last_repeat = repeat;
        }
        else {
            // otherwise (if the last processed element repeats
            // fewer than N times) format the element that many
            // times

            if (last_repeat < 0) {
                if (flags & (A_CHAR | A_WCHAR)) {
                    if (   (flags & A_ARRAY)
                        && _RWSTD_WCHAR_T_SIZE == sizeof (elemT))
                        *s++ = 'L';

                    // insert an opening quote
                    *s++ = '\"';
                }
            }
            else if (last_repeat > 0) {
                if (flags & (A_CHAR | A_WCHAR)) {
                    // append a comma after a preceding "<repeats N times>"
                    *s++ = ',';
                    *s++ = ' ';
                    // append an opening quote for character arrays
                    *s++ = '\"';
                }
            }

            if (flags & (A_CHAR | A_WCHAR)) {
                // format element into elemstr as a character
                rw_quotechar (elemstr, *last, !(flags & A_ESC));
            }
            else {
                // format element into elemstr as an integer
                char *localbuf = elemstr;
                size_t localbufsize = sizeof elemstr;

                Buffer bufspec = {
                    &localbuf, &localbufsize, sizeof elemstr, 0
                };

                *localbuf = '\0';
                const int intlen = rw_fmtinteger (spec, bufspec, *last);

                // no error should happen
                RW_ASSERT (0 < intlen);

                localbuf [intlen] = '\0';

                // verify that rw_fmtinteger didn't try to reallocate
                RW_ASSERT (localbuf == elemstr);
            }

            const size_t elemlen = strlen (elemstr);

            for ( ; last != pelem; ++last) {
                memcpy (s, elemstr, elemlen);
                s += elemlen;
                memcpy (s, sep, seplen + 1);
                s += seplen;
            }

            last_repeat = 0;

            if (size_t (pelem - array) == nelems) {
                if (flags & (A_CHAR | A_WCHAR) && flags & A_ESC)
                    *s++ = '\"';

                // eliminate the last separator
                s -= seplen;
                *s = '\0';

                break;
            }
        }

        if (size_t (pelem - array) == nelems) {
            // eliminate the last separator
            s -= seplen;
            *s = '\0';

            break;
        }
    }

    if (any_repeats) {
        // enclose the whole thing in curly braces if any repeated elements
        // have been formatted using the "... <repeats N times>" syntax
        const size_t len = strlen (next);
        memmove (next + 2, next, len);
        next [0] = '{';
        next [1] = ' ';
        next [len + 2] = ' ';
        next [len + 3] = '}';
        next [len + 4] = '\0';
        s = next + len + 4;
    }

    buf.endoff += size_t (s - bufend);

    return int (s - bufend);
}

/********************************************************************/

static int
_rw_fmtarray (FmtSpec *pspec, size_t paramno, Buffer &buf, va_list *pva)
{
    RW_ASSERT (0 != pspec);

    FmtSpec &spec = pspec [paramno];

    // save width and set to unspecified (i.e., 1)
    const int width = spec.width;
    spec.width = -1;

    const int flags = A_ARRAY
        | ('c' == spec.cvtspec || 'S' == spec.cvtspec ?
          A_CHAR | A_ESC : spec.fl_pound ? A_ESC : 0);

    // to format an array of integers using the 0 or 0x/0X prefix
    // both the pound and the zero flags must be set; clear the pound
    // flag unless zero is also set
    const unsigned pound = spec.fl_pound;
    if (0 == spec.fl_zero)
        spec.fl_pound = 0;

    const size_t nelems = size_t (spec.prec);
    spec.prec = -1;

    int len = -1;

    // array formatting: width determines the width of each array element,
    // precision the number of elements (when negative the array is taken
    // to extend up to but not including the first 0 element)
    if (-1 == width || 1 == width) {
        spec.param.ptr_ = PARAM (ptr_);
        const _RWSTD_UINT8_T* const array = (_RWSTD_UINT8_T*)spec.param.ptr_;
        // note that when no precision is specified in the format string
        // (e.g., "%{Ac}") its value will be -1 and the function will format
        // all elements up to but excluding the terminating 0
        len = rw_fmtarray (spec, buf, array, nelems, flags);
    }
    else if (2 == width) {
        spec.param.ptr_ = PARAM (ptr_);
        const _RWSTD_UINT16_T* const array = (_RWSTD_UINT16_T*)spec.param.ptr_;
        len = rw_fmtarray (spec, buf, array, nelems, flags);
    }
    else if (4 == width) {
        spec.param.ptr_ = PARAM (ptr_);
        const _RWSTD_UINT32_T* const array = (_RWSTD_UINT32_T*)spec.param.ptr_;
        len = rw_fmtarray (spec, buf, array, nelems, flags);
    }

#ifdef _RWSTD_UINT64_T

    else if (8 == width) {
        spec.param.ptr_ = PARAM (ptr_);
        const _RWSTD_UINT64_T* const array = (_RWSTD_UINT64_T*)spec.param.ptr_;
        len = rw_fmtarray (spec, buf, array, nelems, flags);
    }

#endif   // _RWSTD_UINT64_T

    else {
        RW_ASSERT (!"%{Ac} not implemented for this character size");
    }

    // restore modified members
    spec.fl_pound = pound;
    spec.prec     = int (nelems);
    spec.width    = width;

    return len;
}

/********************************************************************/

// implements %c and %{c} (narrow character formatting, both without
// and with escape sequences, respectively)
static int
_rw_fmtchr (const FmtSpec &spec, Buffer &buf, int val, int noesc)
{
    typedef unsigned char UChar;
    const UChar uc = UChar (val);

    char buffer [8];

    int len = rw_quotechar (buffer + spec.fl_pound, uc, noesc);
    if (spec.fl_pound) {
        buffer [0] = buffer [len + 1] = '\'';
        buffer [len + 2] = '\0';
        len += 2;
    }

    FmtSpec newspec (spec);
    newspec.fl_pound = 0;
    return _rw_fmtstr (newspec, buf, buffer, size_t (len));
}

/********************************************************************/

// implements %lc and %{lc} (wide character formatting, both without
// and with escape sequences, respectively)
static int
_rw_fmtwchr (const FmtSpec &spec, Buffer &buf, wint_t val, int noesc)
{
    char buffer [16];
    int  len;

    if (0 == noesc && -1 == val) {

        // format EOF
        buffer [0] = 'E';
        buffer [1] = 'O';
        buffer [2] = 'F';
        buffer [3] = '\0';
        len = 3;
    }
    else {
        const wchar_t wc = wchar_t (val);

        len = rw_quotechar (buffer + spec.fl_pound, wc, noesc);
        if (spec.fl_pound) {
            buffer [0] = buffer [len + 1] = '\'';
            buffer [len + 2] = '\0';
            len += 2;
        }
    }

    FmtSpec newspec (spec);
    newspec.fl_pound = 0;
    return _rw_fmtstr (newspec, buf, buffer, size_t (len));
}

/********************************************************************/

static int
_rw_fmtstr (const FmtSpec &spec, Buffer &buf, const char *str, size_t len)
{
    if (spec.fl_pound)
        return rw_fmtarray (spec, buf, str, len, A_CHAR | A_ESC);

    if (0 == str || 0 > _RW::__rw_memattr (str, _RWSTD_SIZE_MAX, 0))
        return _rw_fmtbadaddr (spec, buf, str);

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
    if (0 == _rw_bufcat (buf, 0, pad + len))
        return -1;

    RW_ASSERT (0 != *buf.pbuf);
    char *next = *buf.pbuf + buf.endoff;

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

    len += pad;

    buf.endoff += len;

    return int (len);
}

/********************************************************************/

#ifndef _RWSTD_NO_WCHAR_T

static int
_rw_fmtwstr (const FmtSpec &spec, Buffer &buf,
             const wchar_t *wstr, size_t len)
{
    if (spec.fl_pound) {

        // set up flags so that %{#*S} for generic basic_string quoted
        // formatting (i.e., parametrized on the size of the character
        // type) prepends a 'L' to the quoted string when the character
        // type is (likely to be) wchar_t
        const int flags =
              ('S' == spec.cvtspec && -1 != spec.width ? A_ARRAY : 0)
            | A_WCHAR | A_ESC;

        return rw_fmtarray (spec, buf, wstr, len, flags);
    }

    if (   0 == wstr || 0 > _RW::__rw_memattr (wstr, _RWSTD_SIZE_MAX, 0)
        || ((size_t)wstr & (sizeof *wstr - 1)))
        return _rw_fmtbadaddr (spec, buf, wstr, sizeof *wstr);

    if (_RWSTD_SIZE_MAX == len) {

#ifndef _RWSTD_NO_WCSLEN
        len = wcslen (wstr);
#else   // if defined (_RWSTD_NO_WCSLEN)
        len = 0;
        for (const wchar_t *pwc = wstr; *pwc; ++pwc, ++len);
#endif   // _RWSTD_NO_WCSLEN

    }

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
    if (0 == _rw_bufcat (buf, 0, pad + len * 10))
        return -1;

    RW_ASSERT (0 != *buf.pbuf);
    char *next = *buf.pbuf + buf.endoff;

    if (!spec.fl_minus) {
        for (size_t i = 0; i != pad; ++i)
            *next++ = ' ';
    }

    // wmemcpy (next, wstr, len);
    for (const wchar_t *pwc = wstr; pwc != wstr + len; ++pwc) {
        const int n = rw_quotechar (next, *pwc, 0);

        if (n < 0)
            return n;

        next += n;
    }

    if (spec.fl_minus) {
        for (size_t i = 0; i != pad; ++i)
            *next++ = ' ';
    }

    len += pad;

    buf.endoff += len;

    return int (len);
}

#endif   // _RWSTD_NO_WCHAR_T

/********************************************************************/

struct Bitnames
{
    const char *longname;
    const char *name;
    int         bits;
};

#define BITNAME(qual, name)   { #qual "::" #name, #name, qual::name }

static int
rw_bmpfmt (const FmtSpec &spec, Buffer &buf,
           const Bitnames bmap[], size_t size, int bits)
{
    RW_ASSERT (0 != buf.pbuf);

    char buffer [1024];
    *buffer = '\0';

    // string to use when no bits are set
    const char* all_clear = "0";

    for (size_t i = 0; i != size; ++i) {
        if (bmap [i].bits) {
            if ((bits & bmap [i].bits) == bmap [i].bits) {

                const char* const name = spec.fl_pound ?
                    bmap [i].longname : bmap [i].name;

                strcat (*buffer ? strcat (buffer, " | ") : buffer, name);

                bits &= ~bmap [i].bits;
            }
        }
        else {
            // save the name of the constant to use for 0
            all_clear = spec.fl_pound ? bmap [i].longname : bmap [i].name;
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

        buffersize = strlen (buffer);
    }
    else if (bits) {
        buffersize = strlen (buffer);

        // verify that buffer wasn't overflowed
        RW_ASSERT (buffersize <= sizeof buffer);

        char bitstr [32];
        const int n = sprintf (bitstr, "%#x | ", bits);

        RW_ASSERT (0 < n);

        memmove (buffer + n, buffer, buffersize);
        memcpy (buffer, bitstr, size_t (n));

        buffersize += n;
    }
    else {
        buffersize = strlen (buffer);
    }

    // verify that buffer wasn't overflowed
    RW_ASSERT (buffersize <= sizeof buffer);

    if (0 == _rw_bufcat (buf, buffer, buffersize))
        return -1;

    return int (buffersize);
}

/********************************************************************/

static int
_rw_fmtflags (const FmtSpec &spec, Buffer &buf, int bits)
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
        BITNAME (std::ios, nolockbuf),

#endif   // _RWSTD_NO_EXT_REENTRANT_IO

        { "std::ios::iostate(0)", "iostate(0)", std::ios::iostate () }

    };

    static const size_t count = sizeof names / sizeof *names;

    const int base = (bits >> _RWSTD_IOS_BASEOFF) & _RWSTD_IOS_BASEMASK;

    // zero out bits representingthe numeric base
    bits &= ~(_RWSTD_IOS_BASEMASK << _RWSTD_IOS_BASEOFF);

    int len = rw_bmpfmt (spec, buf, names, count, bits);

    if (base && base != 8 && base != 10 && base != 16) {

        // for numeric bases other than those required by the standard,
        // use the text "base (%d)" to show the extended numeric base

#ifndef _RWSTD_NO_EXT_BIN_IO

        if (bits & std::ios::bin)
            return len;

#endif   // _RWSTD_NO_EXT_BIN_IO

        char basestr [64];
        const int n = sprintf (basestr, " | std::ios::base(%d)", base);
        len += n;

        if (0 == _rw_bufcat (buf, basestr, size_t (n)))
            return -1;
    }

    return len;
}

/********************************************************************/

static int
rw_fmtiostate (const FmtSpec &spec, Buffer &buf, int bits)
{
    static const Bitnames names [] = {
        BITNAME (std::ios, goodbit),
        BITNAME (std::ios, badbit),
        BITNAME (std::ios, eofbit),
        BITNAME (std::ios, failbit)
    };

    static const size_t count = sizeof names / sizeof *names;

    return rw_bmpfmt (spec, buf, names, count, bits);
}

/********************************************************************/

static int
_rw_fmtopenmode (const FmtSpec &spec, Buffer &buf, int bits)
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
        BITNAME (std::ios, ate),

        { "std::ios::openmode(0)", "openmode(0)", std::ios::openmode () }
    };

    static const size_t count = sizeof names / sizeof *names;

    return rw_bmpfmt (spec, buf, names, count, bits);
}

/********************************************************************/

static int
_rw_fmtseekdir (const FmtSpec &spec, Buffer &buf, int bits)
{
    static const Bitnames names [] = {

        BITNAME (std::ios, beg),
        BITNAME (std::ios, cur),
        BITNAME (std::ios, end)
    };

    static const size_t count = sizeof names / sizeof *names;

    return rw_bmpfmt (spec, buf, names, count, bits);
}

/********************************************************************/

static int
_rw_fmtevent (const FmtSpec&, Buffer &buf, int event)
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

    if (0 == _rw_bufcat (buf, str, len))
        return -1;

    return int (len);
}

/********************************************************************/

static int
rw_fmtlc (const FmtSpec &spec, Buffer &buf, int val)
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

        if (0 == _rw_bufcat (buf, str, len))
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

    return rw_bmpfmt (spec, buf, names, count, val);
}

/********************************************************************/

static int
_rw_fmtmonpat (const FmtSpec&, Buffer &buf, const char pat [4])
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

    if (0 == _rw_bufcat (buf, buffer, len))
        return -1;

    return int (len);
}

/********************************************************************/

static int
_rw_vasnprintf_ext (FmtSpec    *pspec,
                    size_t      paramno,
                    Buffer     &buf,
                    const char *fmt,
                    va_list    *pva)
{
    RW_ASSERT (0 != pva);
    RW_ASSERT (0 != pspec);

    // for convenience
    FmtSpec &spec = pspec [paramno];

    // the length of the sequence appended to the buffer to return
    // to the caller, or a negative value (such as -1) on error
    int len = -1;

    for (size_t i = _rw_usr_inx; i-- && _rw_usr_fun [i]; ) {

        // process the stack of user-defined formatting functions
        // top to bottom

        if (_rw_usr_fun [i]) {
            // user-defined extension?
            if (0 < spec.paramno) {
                // pass the address of the paramno argument
                len = (_rw_usr_fun [i])(buf.pbuf, buf.pbufsize, fmt,
                                        &pspec [spec.paramno - 1].param);
            }
            else {
                // pass the address of the variable argument list
                // followed by the address of the current parameter
                len = (_rw_usr_fun [i])(buf.pbuf, buf.pbufsize, fmt, pva,
                                        &spec.param);
            }

            // special value indicating that the user-defined function
            // decided not to process the format string and is passing
            // control back to us
            if (_RWSTD_INT_MIN != len) {
                if (-1 != len)
                    buf.endoff += len;
                return len;
            }
        }
    }

    len = -1;

    switch (spec.cvtspec) {

    case '!':   // %{!}, %{+!}, %{-!}
        // set a user-defined formatting function
        if (spec.fl_plus) {
            // push a new user-defined handler on top of the stack
            spec.param.funptr_          = PARAM (funptr_);
            _rw_usr_fun [_rw_usr_inx++] = (_rw_usr_cb_t*)spec.param.funptr_;
        }
        if (spec.fl_minus) {
            // pop the stack of user-defined handlers
            if (_rw_usr_inx)
                --_rw_usr_inx;
        }

        if (!spec.fl_plus && !spec.fl_minus) {
            // replace the top of the stack of user-defined handlers
            if (0 == _rw_usr_inx)
                ++_rw_usr_inx;

            spec.param.funptr_            = PARAM (funptr_);
            _rw_usr_fun [_rw_usr_inx - 1] = (_rw_usr_cb_t*)spec.param.funptr_;
        }

        len = 0;
        break;

    case '?':   // %{?}
        // beginning of an if clause
        spec.param.int_ = PARAM (int_);
        spec.cond       = 1;
        spec.cond_begin = 1;
        spec.cond_true  = 0 != spec.param.int_;
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
            len = _rw_fmtstr (spec, buf, str, _RWSTD_SIZE_MAX);
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
            len = _rw_fmtstr (spec, buf, str, _RWSTD_SIZE_MAX);
        }
        break;

    case 'c':   // %{c}, %{Ac}, %{Lc}, %{lc}
        if (spec.mod == spec.mod_ext_A) {   // array of characters
            len = _rw_fmtarray (pspec, paramno, buf, pva);
        }
        else if (spec.mod == spec.mod_L) {
            // locale category or LC_XXX constant
            spec.param.int_ = PARAM (int_);
            len = rw_fmtlc (spec, buf, spec.param.int_);
        }
        else if (spec.mod == spec.mod_l) {
            // wint_t argument formatted as wchar_t with non-printable
            // characters represented using traditional C escape
            // sequences
            spec.param.wint_ = PARAM (wint_);
            return _rw_fmtwchr (spec, buf, spec.param.wint_, 0);
        }
        else {
            // int argument formatted as char with non-printable
            // characters represented using traditional C escape
            // sequences
            spec.param.int_ = PARAM (int_);
            return _rw_fmtchr (spec, buf, spec.param.int_, 0);
        }
        break;

    case 'e':   // %{e}, %{Ae}
        if (spec.mod == spec.mod_ext_A) {   // array of floating point values
            spec.param.ptr_ = PARAM (ptr_);
            len = _rw_fmtfloating (spec, buf, spec.param.ptr_);
        }
        else if (spec.mod == spec.mod_ext_I) {   // ios::copyfmt_event
            spec.param.int_ = PARAM (int_);
            len = _rw_fmtevent (spec, buf, spec.param.int_);
        }
        break;

    case 'f':   // %{f}, %{Af}, %{If}
        if (spec.mod == spec.mod_ext_A) {   // array of floating point values
            spec.param.ptr_ = PARAM (ptr_);
            len = _rw_fmtfloating (spec, buf, spec.param.ptr_);
        }
        else if (spec.mod == spec.mod_ext_I) {   // ios::fmtflags
            spec.param.int_ = PARAM (int_);
            len = _rw_fmtflags (spec, buf, spec.param.int_);
        }
        else {   // function pointer
            spec.param.funptr_ = PARAM (funptr_);
            len = _rw_fmtfunptr (spec, buf, spec.param.funptr_);
        }
        break;

    case 'g':   // %{g}, %{Ag}
        if (spec.mod == spec.mod_ext_A) {   // array of floating point values
            spec.param.ptr_ = PARAM (ptr_);
            len = _rw_fmtfloating (spec, buf, spec.param.ptr_);
        }
        else {
            RW_ASSERT (!"%{g} not implemented");
        }
        break;

    case 'd':   // %{Id}
    case 'i':   // %{Ii}
        if (-1 == spec.base)
            spec.base = 10;
        // fall through

    case 'o':   // %{Io}
        if (spec.mod == spec.mod_ext_I) {   // ios::openmode
            spec.param.int_ = PARAM (int_);
            len = _rw_fmtopenmode (spec, buf, spec.param.int_);
            break;
        }
        if (-1 == spec.base)
            spec.base = 8;
        // fall through

    case 'x':   // %{x}, %{Ax}
    case 'X':   // %{X}, %{AX}
        if (-1 == spec.base)
            spec.base = 16;
        // fall through

    case 'u':   // %{u}, %{Au}
        if (spec.mod == spec.mod_ext_A) {   // array
            if (-1 == spec.base)
                spec.base = 10;
            len = _rw_fmtarray (pspec, paramno, buf, pva);
        }
        else {
            // reset base set above
            spec.base = -1;
            len = _rw_fmtinteger (pspec, paramno, buf, pva);
        }
        break;

    case 'K':   // %{K} -- signal
        spec.param.int_ = PARAM (int_);
        len = _rw_fmtsignal (spec, buf, spec.param.int_);
        break;

    case 'm':   // %{m} -- errno
        if (-1 == spec.width)
            len = _rw_fmterrno (spec, buf, errno);
        else
            len = _rw_fmterrno (spec, buf, spec.width);
        break;

    case 'M':   // %{M}, %{LM}
        if (spec.mod == spec.mod_L) {   // money_base::pattern
            spec.param.ptr_ = PARAM (ptr_);
            len = _rw_fmtmonpat (spec, buf, (char*)spec.param.ptr_);
        }
        else {   // member pointer
            spec.param.memptr_ = PARAM (memptr_);
            len = _rw_fmtmemptr (spec, buf, spec.param.memptr_);
        }
        break;

    case 'n': {   // %{n}
        // The argument shall be a pointer to signed integer into which
        // is written the size of the buffer allocated by this call to
        // fprintf. No argument is converted, but one is consumed. If
        // the conversion specification includes any flags, a field
        // width, or a precision, the behavior is undefined.

        RW_ASSERT (0 != buf.pbuf);
        RW_ASSERT (0 != *buf.pbuf);
        RW_ASSERT (0 != buf.pbufsize);

        const size_t nbytes = *buf.pbufsize;

        spec.param.ptr_ = PARAM (ptr_);

        if (spec.mod == spec.mod_hh) {
            unsigned char* const ptr = (unsigned char*)spec.param.ptr_;

            RW_ASSERT (0 != ptr);

            *ptr = (unsigned char)nbytes;
        }
        else if (spec.mod == spec.mod_h) {
            short* const ptr = (short*)spec.param.ptr_;

            RW_ASSERT (0 != ptr);

            *ptr = short (nbytes);
        }
        else if (spec.mod == spec.mod_L) {
#ifdef _RWSTD_LONG_LONG
            _RWSTD_LONG_LONG* const ptr = (_RWSTD_LONG_LONG*)spec.param.ptr_;

            RW_ASSERT (0 != ptr);

            *ptr = (_RWSTD_LONG_LONG)(nbytes);
#else   // if !defined (_RWSTD_LONG_LONG)
            RW_ASSERT (!"%{Ln} not implemented");
#endif   // _RWSTD_LONG_LONG
        }
        else if (spec.mod == spec.mod_l) {
            long* const ptr = (long*)spec.param.ptr_;

            RW_ASSERT (0 != ptr);

            *ptr = long (nbytes);
        }
        else if (spec.mod == spec.mod_t) {
            ptrdiff_t* const ptr = (ptrdiff_t*)spec.param.ptr_;

            RW_ASSERT (0 != ptr);

            *ptr = ptrdiff_t (nbytes);
        }
        else {
            int* const ptr = (int*)spec.param.ptr_;

            RW_ASSERT (0 != ptr);

            *ptr = int (nbytes);
        }
        len = 0;
        break;
    }

    case 's':   // %{s}, %{Is}, %{ls}
        if (spec.mod == spec.mod_ext_I) {   // ios::iostate
            spec.param.int_ = PARAM (int_);
            len = rw_fmtiostate (spec, buf, spec.param.int_);
        }
        else if (spec.mod == spec.mod_l) {   // wchar_t*
            spec.param.ptr_ = PARAM (ptr_);
            const wchar_t* const wstr = (wchar_t*)spec.param.ptr_;
            const size_t wstr_len =
                spec.width < 0 ? _RWSTD_SIZE_MAX : size_t (spec.width);
            len = rw_fmtarray (spec, buf, wstr, wstr_len, A_WCHAR | A_ESC);
        }
        else {   // char*
            spec.param.ptr_ = PARAM (ptr_);
            const char* const str = (char*)spec.param.ptr_;
            const size_t str_len =
                spec.width < 0 ? _RWSTD_SIZE_MAX : size_t (spec.width);
            len = rw_fmtarray (spec, buf, str, str_len, A_CHAR | A_ESC);
        }
        break;

    case 'S':   // %{S}, %{lS}, %{#*S}
        if (   spec.mod == spec.mod_l
            || !spec.mod && spec.fl_pound && sizeof (wchar_t) == spec.width) {
            // std::wstring
            spec.param.ptr_ = PARAM (ptr_);

            const std::wstring* const pstr = (std::wstring*)spec.param.ptr_;

            if (   0 == pstr || 0 > _RW::__rw_memattr (pstr, sizeof *pstr, 0)
                || (size_t)pstr & (sizeof (pstr) - 1)) {
                len = _rw_fmtbadaddr (spec, buf, pstr, sizeof pstr);
            }
            else {

                const wchar_t* const wstr = pstr->data ();
                const std::wstring::size_type size = pstr->size ();

                len = _rw_fmtwstr (spec, buf, wstr, size);
            }
        }
        else {   // std::string
            spec.param.ptr_ = PARAM (ptr_);

            const std::string* const pstr = (std::string*)spec.param.ptr_;

            if (   0 == pstr || 0 > _RW::__rw_memattr (pstr, sizeof *pstr, 0)
                || (size_t)pstr & (sizeof (pstr) - 1)) {
                len = _rw_fmtbadaddr (spec, buf, pstr, sizeof pstr);
            }
            else {
                const char* const str = pstr->data ();
                const std::string::size_type size = pstr->size ();

                len = _rw_fmtstr (spec, buf, str, size);
            }
        }
        break;

    case 't': {   // %{t}
        spec.param.ptr_ = PARAM (ptr_);

        const tm* const tmb = (tm*)spec.param.ptr_;

        len = _rw_fmttm (spec, buf, tmb);

        break;
    }

    case 'w': {   // %{Iw}
        if (spec.mod == spec.mod_ext_I) {   // ios::seekdir
            spec.param.int_ = PARAM (int_);
            len = _rw_fmtseekdir (spec, buf, spec.param.int_);
            break;
        }
    }
        

    default:
        if (spec.strarg) {
            // environment variable
            const char* val = getenv (spec.strarg);

            if (!val)
                val = "";

            len = int (strlen (val));

            if (0 == _rw_bufcat (buf, val, size_t (len)))
                return -1;

            free (spec.strarg);
        }
        else {
            char text [80];
            len = sprintf (text, "*** %%{%.*s}: not implemented ***",
                           int (sizeof fmt - 40), fmt);

            if (0 == _rw_bufcat (buf, text, size_t (len)))
                return -1;
        }
    }

    return len;
}

/********************************************************************/

_TEST_EXPORT int
rw_asnprintf (char **pbuf, size_t *pbufsize, const char *fmt, ...)
{
    RW_ASSERT (0 == pbuf || 0 == *pbuf || pbufsize);

    va_list va;
    va_start (va, fmt);

    char* bufptr = 0;
    size_t bufsize = 0;

    if (0 == pbuf) {
        // if pbyf is 0 (i.e., this is a request for the size of the
        // buffer necessary to format all the arguments), set pbuf to
        // point to a local buf
        pbuf = &bufptr;
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
    RW_ASSERT (nchars < 0 || strlen (*pbuf) < *pbufsize);

    if (pbuf == &bufptr) {
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
    RW_ASSERT (nchars < 0 || strlen (buf) < bufsize);

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
    RW_ASSERT (nchars < 0 || buf && strlen (buf) < bufsize);

    _RWSTD_UNUSED (nchars);

    return buf;
}


/********************************************************************/

_TEST_EXPORT int
rw_sprintf (char *buf, const char *fmt, ...)
{
    char *tmpbuf = 0;
    size_t tmpsize = 0;

    va_list va;
    va_start (va, fmt);

    // FIXME: format directly into buf to avoid dynamic allocation
    const int nchars = rw_vasnprintf (&tmpbuf, &tmpsize, fmt, va);

    va_end (va);

    if (-1 < nchars)
        memcpy (buf, tmpbuf, size_t (nchars));

    free (tmpbuf);

    return nchars;
}

/********************************************************************/

_TEST_EXPORT int
rw_snprintf (char *buf, size_t bufsize, const char *fmt, ...)
{
    char *tmpbuf = 0;
    size_t tmpsize = 0;

    va_list va;
    va_start (va, fmt);

    // FIXME: format directly into buf to avoid dynamic allocation
    const int nchars = rw_vasnprintf (&tmpbuf, &tmpsize, fmt, va);

    va_end (va);

    if (size_t (nchars) <= bufsize)
        memcpy (buf, tmpbuf, size_t (nchars));

    free (tmpbuf);

    return nchars;
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

/********************************************************************/

static int
_rw_vfprintf (rw_file *file, const char *fmt, va_list va)
{
    char* buf = 0;
    size_t bufsize = 0;

    const int nchars = rw_vasnprintf (&buf, &bufsize, fmt, va);

    int nwrote = nchars;

    if (0 < nchars) {

        // avoid formatting when there's nothing to output

        if (file) {
            // allow null file argument

            // FIXME: implement this in terms of POSIX write()
            //        for async-signal safety
            FILE* const stdio_file = _RWSTD_REINTERPRET_CAST (FILE*, file);

            nwrote = fwrite (buf, 1, size_t (nchars), stdio_file);

            // flush in case stderr isn't line-buffered (e.g., when
            // it's determined not to refer to a terminal device,
            // for example after it has been redirected to a file)
            fflush (stdio_file);
        }

#ifdef _MSC_VER

        // IsDebuggerPresent() depends on the macros _WIN32_WINNT and WINVER
        // being appropriately #defined prior to the #inclusion of <windows.h>
        if (IsDebuggerPresent ()) {

            // write string to the attached debugger (if any)
            OutputDebugString (buf);
        }

#endif   // _MSC_VER

    }

    free (buf);

    return nwrote;
}

/********************************************************************/

_TEST_EXPORT int
rw_fprintf (rw_file *file, const char *fmt, ...)
{
    va_list va;
    va_start (va, fmt);

    const int nchars = _rw_vfprintf (file, fmt, va);

    va_end (va);

    return nchars;
}

/********************************************************************/

_TEST_EXPORT int
rw_printf (const char *fmt, ...)
{
    va_list va;
    va_start (va, fmt);

    const int nchars = _rw_vfprintf (rw_stdout, fmt, va);

    va_end (va);

    return nchars;
}
