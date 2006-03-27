/**************************************************************************
 *
 * char.cpp - helpers for user-defined character type and its traits
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright 2006 The Apache Software Foundation or its licensors,
 * as applicable.
 *
 * Copyright 2006 Rogue Wave Software.
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
#include <rw_char.h>

#include <rw_printf.h>   // for rw_snprintfa()

#include <ctype.h>       // for isdigit()
#include <stdarg.h>      // for va_arg(), va_list, ...
#include <stdlib.h>      // for strtol()
#include <string.h>      // for memcpy(), strlen(), ...


size_t
UserTraits<UserChar>::
n_calls_ [UserTraits<UserChar>::MemFun::n_funs];


void
UserTraits<UserChar>::
assign (char_type &c1, const char_type &c2)
{
    ++n_calls_ [MemFun::assign];

    c1.f = c2.f;
    c1.c = c2.c;
}


bool
UserTraits<UserChar>::
eq (const char_type &c1, const char_type &c2)
{
    ++n_calls_ [MemFun::eq];

    return c1.f == c2.f && c1.c == c2.c;
}


bool
UserTraits<UserChar>::
lt (const char_type &c1, const char_type &c2)
{
    ++n_calls_ [MemFun::lt];

    return c1.f < c2.f || c1.f == c2.f && c1.c < c2.c;
}


int UserTraits<UserChar>::
compare (const char_type *s1, const char_type *s2, size_t n)
{
    RW_ASSERT (0 == n || s1 && s2);

    ++n_calls_ [MemFun::compare];

    for (size_t i = 0; i != n; ++i) {
        if (s1 [i].f != s2 [i].f || s1 [i].c != s2 [i].c) {
            if (   s1 [i].f < s2 [i].f
                || s1 [i].f == s2 [i].f && s1 [i].c < s2 [i].c)
                return -1;

            return 1;
        }
    }

    return 0;
}
        

size_t UserTraits<UserChar>::
length (const char_type *s)
{
    RW_ASSERT (0 != s);

    ++n_calls_ [MemFun::length];

    size_t len = 0;

    for (; s [len].f || s [len].c; ++len);

    return len;
}
 

const UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
find (const char_type *s, size_t n, const char_type &c)
{
    RW_ASSERT (0 == n || s);

    ++n_calls_ [MemFun::find];

    for (; n--; ++s) {
        if (s->f == c.f && s->c == c.c)
            return s;
    }

    return 0;
}


UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
copy (char_type *dst, const char_type *src, size_t n)
{
    RW_ASSERT (0 == n || dst && src);

    ++n_calls_ [MemFun::copy];

    for (size_t i = 0; i != n; i++)
        dst [i] = src [i];

    return dst;
}


UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
move (char_type *dst, const char_type *src, size_t n)
{
    RW_ASSERT (0 == n || dst && src);

    ++n_calls_ [MemFun::move];

    if (dst < src) {
        for (size_t i = 0; i != n; ++i)
            dst [i] = src [i];
    }
    else {
        while (n--)
            dst [n] = src [n];
    }

    return dst;
}


UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
assign (char_type *s, size_t n, char_type c)
{
    RW_ASSERT (0 == n || s);

    ++n_calls_ [MemFun::assign2];

    for (; n--; s [n] = c);

    return s;
}


UserTraits<UserChar>::int_type
UserTraits<UserChar>::
not_eof (const int_type &i)
{
    ++n_calls_ [MemFun::not_eof];

    if (i.equal (int_type::eof ())) {
        const char_type c = { 0, 0 };

        return int_type::from_char (c);
    }

    return i;
}


UserTraits<UserChar>::char_type
UserTraits<UserChar>::
to_char_type (const int_type &i)
{
    ++n_calls_ [MemFun::to_char_type];

    return i.to_char ();
}
      

UserTraits<UserChar>::int_type
UserTraits<UserChar>::
to_int_type (const char_type &c)
{
    ++n_calls_ [MemFun::to_int_type];

    return int_type::from_char (c);
}


bool
UserTraits<UserChar>::
eq_int_type (const int_type &i1, const int_type &i2)
{
    ++n_calls_ [MemFun::eq_int_type];

    return i1.equal (i2);
}


UserTraits<UserChar>::int_type
UserTraits<UserChar>::
eof ()
{
    ++n_calls_ [MemFun::eof];

    return int_type::eof ();
}

/**************************************************************************/

_TEST_EXPORT
char*
rw_widen (char *dst, const char *src, size_t len /* = SIZE_MAX */)
{
    // compute the length of src if not specified
    if (_RWSTD_SIZE_MAX == len)
        len = src ? strlen (src) + 1 : 0;

    if (len) {
        RW_ASSERT (0 != dst);

        if (src) {
            // copy src into dst
            memcpy (dst, src, len);
        }
        else {
            // set dst to all NUL
            memset (dst, 0, len);
        }
    }
    else if (dst)
        *dst = '\0';

    return dst;
}


_TEST_EXPORT
char*
rw_narrow (char *dst, const char *src, size_t len /* = SIZE_MAX */)
{
    return rw_widen (dst, src, len);
}


_TEST_EXPORT
size_t
rw_match (const char *s1, const char *s2, size_t len /* = SIZE_MAX */)
{
    if (0 == s1) {
        // return the length of s2 if non-null
        return s2 ? strlen (s2) : 0;
    }

    if (0 == s2)
        return strlen (s1);

    size_t n = 0;

    for ( ; n != len && s1 [n] == s2 [n]; ++n) {
        if (_RWSTD_SIZE_MAX == len && '\0' == s1 [n])
            break;
    }

    return n;
}


#ifndef _RWSTD_WCHAR_T

_TEST_EXPORT
wchar_t*
rw_widen (wchar_t *dst, const char *src, size_t len /* = SIZE_MAX */)
{
    // compute the length of src if not specified
    if (_RWSTD_SIZE_MAX == len)
        len = src ? strlen (src) + 1 : 0;

    // if len is non-zero dst must be non-0 as well
    RW_ASSERT (0 == len || 0 != dst);

    if (dst) {
        if (src) {
            // widen src into dst one element at a time
            for (size_t i = 0; ; ++i) {
                typedef unsigned char UChar;

                if (i == len) {
                    dst [i] = L'\0';
                    break;
                }

                dst [i] = wchar_t (UChar (src [i]));
            }
        }
        else {
            // set dst to all NUL
            memset (dst, 0, len * sizeof *dst);
        }
    }

    return dst;
}


_TEST_EXPORT
char*
rw_narrow (char *dst, const wchar_t *src, size_t len /* = SIZE_MAX */)
{
    // compute the length of src if not specified
    if (_RWSTD_SIZE_MAX == len) {
        if (src) {
            for (len = 0; src [len]; ++len);
        }
        else
            len = 0;
    }

    // if len is non-zero dst must be non-0 as well
    RW_ASSERT (0 == len || 0 != dst);

    if (dst) {
        if (src) {
            // narrow src into dst one element at a time
            for (size_t i = 0; ; ++i) {
                typedef unsigned char UChar;

                if (i == len) {
                    dst [i] = '\0';
                    break;
                }

                dst [i] = char (UChar (src [i]));
            }
        }
        else {
            // set dst to all NUL
            memset (dst, 0, len);
        }
    }

    return dst;
}


_TEST_EXPORT
size_t
rw_match (const char *s1, const wchar_t *s2, size_t len /* = SIZE_MAX */)
{
    if (0 == s1) {
        if (s2) {
            // return the length of s2
            for (len = 0; s2 [len]; ++len);
            return len;
        }

        return 0;
    }

    if (0 == s2)
        return strlen (s1);

    typedef unsigned char UChar;

    size_t n = 0;

    for ( ; n != len && UChar (s1 [n]) == unsigned (s2 [n]); ++n) {
        if (_RWSTD_SIZE_MAX == len && '\0' == s1 [n])
            break;
    }

    return n;
}

#endif   // _RWSTD_WCHAR_T


_TEST_EXPORT
UserChar*
rw_widen (UserChar *dst, const char *src, size_t len /* = SIZE_MAX */)
{
    // compute the length of src if not specified
    if (_RWSTD_SIZE_MAX == len)
        len = src ? strlen (src) + 1 : 0;

    // if len is non-zero dst must be non-0 as well
    RW_ASSERT (0 == len || 0 != dst);

    if (dst) {
        if (src) {
            // widen src into dst one element at a time
            for (size_t i = 0; ; ++i) {
                typedef unsigned char UChar;

                if (i == len) {
                    dst [i] = UserChar::eos ();
                    break;
                }

                dst [i].f = 0;
                dst [i].c = UChar (src [i]);
            }
        }
        else {
            // set dst to all NUL
            memset (dst, 0, len * sizeof *dst);
        }
    }

    return dst;
}


_TEST_EXPORT
char*
rw_narrow (char *dst, const UserChar *src, size_t len /* = SIZE_MAX */)
{
    // compute the length of src if not specified
    if (_RWSTD_SIZE_MAX == len) {
        if (src) {
            for (len = 0; src [len].f || src [len].c; ++len);
        }
        else
            len = 0;
    }

    // if len is non-zero dst must be non-0 as well
    RW_ASSERT (0 == len || 0 != dst);

    if (dst) {
        if (src) {
            // narrow src into dst one element at a time
            for (size_t i = 0; ; ++i) {
                typedef unsigned char UChar;

                if (i == len) {
                    dst [i] = '\0';
                    break;
                }

                dst [i] = char (src [i].c);
            }
        }
        else {
            // set dst to all NUL
            memset (dst, 0, len);
        }
    }

    return dst;
}


_TEST_EXPORT
size_t
rw_match (const char *s1, const UserChar *s2, size_t len /* = SIZE_MAX */)
{
    if (0 == s1) {
        if (s2) {
            // return the length of s2
            for (len = 0; s2 [len].f || s2 [len].c; ++len);
            return len;
        }

        return 0;
    }

    if (0 == s2)
        return strlen (s1);

    typedef unsigned char UChar;

    size_t n = 0;

    for ( ; n != len && UChar (s1 [n]) == s2 [n].c; ++n) {
        if (_RWSTD_SIZE_MAX == len && '\0' == s1 [n])
            break;
    }

    return n;
}

/**************************************************************************/

static int
_rw_fmtstringv (char **pbuf, size_t *pbufsize, const char *fmt, va_list va)
{
    RW_ASSERT (0 != pbuf);
    RW_ASSERT (0 != pbufsize);
    RW_ASSERT (0 != fmt);

    // directive syntax:
    // '/' [ '#' ] { '*' | <n> } [ '.' '*' | <n> ] "Gs"

    // NOTE:
    // leading slash (or any other violation of the "standard" directive
    // syntax) prevents the caller from extracting width and precision
    // (etc.) from its variable argument list and allows us to do so

    static int nested_call;

    if (nested_call || '/' != fmt [0])
        return _RWSTD_INT_MIN;

    ++nested_call;
    ++fmt;

    va_list* pva      =  0;
    bool     fl_pound = false;
    int      nelems   = -1;
    int      paramno  = -1;
    int      elemsize = -1;

    union UPtr {
        const char     *pc;
        const wchar_t  *pwc;
        const UserChar *puc;
    };

    if ('#' == *fmt) {
        fl_pound = true;
        ++fmt;
    }

    // saved caller's va_list in case it needs to be restored
    // to its orginal state after extracting argument from it
    va_list va_save;

    if ('*' == *fmt) {
        // process element width (i.e., sizeof(charT))
        pva = va_arg (va, va_list*);

        RW_ASSERT (0 != pva);

        _RWSTD_VA_COPY (va_save, *pva);

        // extract the width from rw_snprintfa's variable argument
        // list pass through to us by the caller
        elemsize = va_arg (*pva, int);
        ++fmt;
    }
    else if (isdigit (*fmt)) {
        // process positional parameter or width
        char* end = 0;
        const int arg = strtol (fmt, &end, 10);
        if ('$' == *end)
            paramno = arg;
        else
            elemsize = arg;

        fmt = end;
    }

    if ('.' == *fmt) {
        // process precision (the length of the array in elements)
        if ('*' == *++fmt) {
            if (0 == pva) {
                pva = va_arg (va, va_list*);

                RW_ASSERT (0 != pva);

                _RWSTD_VA_COPY (va_save, *pva);
            }

            // extract the width from rw_snprintfa's variable argument
            // list passed through to us by the caller
            nelems = va_arg (*pva, int);
            ++fmt;
        }
        else if (isdigit (*fmt)) {
            char* end = 0;
            nelems = int (strtol (fmt, &end, 10));
            fmt    = end;
        }
    }

    // extract the address of the caller's variable argument list
    if (0 == pva) {
        pva = va_arg (va, va_list*);

        RW_ASSERT (0 != pva);

        _RWSTD_VA_COPY (va_save, *pva);
    }

    if ('G' != fmt [0] || 's' != fmt [1] || '\0' != fmt [2]) {

        // restore caller's (potentially modified) va_list
        _RWSTD_VA_COPY (*pva, va_save);

        --nested_call;

        // uknown directive, let caller process
        return _RWSTD_INT_MIN;
    }

    // extract a pointer to the first character from rw_snprintfa's
    // variable argument list pass through to us by the caller 
    const UPtr beg = { va_arg (*pva, char*) };

    {
        // extract the address where to store the extracted argument
        // for use by any subsequent positional paramaters
        const char** const pparam = va_arg (va, const char**);

        RW_ASSERT (0 != pparam);

        // store the extracted argument
        *pparam = beg.pc;
    }

    // compute the length of the buffer formatted so far
    const size_t buflen_0 = *pbuf ? strlen (*pbuf) : 0;

    int nbytes = 0;

    //////////////////////////////////////////////////////////////////
    // invoke rw_asnprintf() recursively to format our arguments
    // and append the result to the end of the buffer; pass the
    // value returned from rw_asnprintf() (i.e., the number of
    // bytes appended) back to the caller

    if (-1 == elemsize || 1 == elemsize) {
        if (nelems < 0)
            nelems = beg.pc ? strlen (beg.pc) : 0;

        nbytes = rw_asnprintf (pbuf, pbufsize, "%{+}%{#*s}", nelems, beg.pc);
    }
    else if (_RWSTD_WCHAR_T_SIZE == elemsize) {
        if (nelems < 0)
            nbytes = rw_asnprintf (pbuf, pbufsize, "%{+}%{#ls}", beg.pwc);
        else
            nbytes = rw_asnprintf (pbuf, pbufsize, "%{+}%{#*ls}",
                                   nelems, beg.pwc);
    }
    else if (sizeof (UserChar) == size_t (elemsize)) {

        // narrow the argument into a local buffer of sufficient size
        // (dynamically allocating memory only when the length of the
        // string exceeds the size of the buffer for efficiency) and formt
        // the resulting narrow string
        char smallbuf [256];
        const size_t len = nelems < 0 ? rw_match (0, beg.puc) : size_t (nelems);
        char* const pb = len < sizeof smallbuf ? smallbuf : new char [len + 1];
        rw_narrow (pb, beg.puc, len);

        if (nelems < 0)
            nelems = int (len);

        nbytes = rw_asnprintf (pbuf, pbufsize, "%{+}%{#*s}",
                               nelems, beg.pc ? pb : beg.pc);

        if (pb != smallbuf)
            delete[] pb;
    }
    else {
        nbytes = rw_asnprintf (pbuf, pbufsize,
                               "*** %%{/Gs}: bad element size: %d ***",
                               elemsize);
    }

    //////////////////////////////////////////////////////////////////

    // compute the new length of the buffer
    const size_t buflen_1 = *pbuf ? strlen (*pbuf) : 0;

    // assert that the function really appended as many characters
    // as it said it did (assumes no NULs embedded in the output)
    // and that it didn't write past the end of the buffer
    RW_ASSERT (buflen_1 == buflen_0 + nbytes);
    RW_ASSERT (buflen_1 < *pbufsize);

    --nested_call;

    return nbytes;
}


static int
_rw_fmtstring (char **pbuf, size_t *pbufsize, const char *fmt, ...)
{
    va_list va;
    va_start (va, fmt);

    const int nbytes = _rw_fmtstringv (pbuf, pbufsize, fmt, va);

    va_end (va);

    return nbytes;
}


UserCharFmatInit::
UserCharFmatInit ()
{
    // install the formatting callback function
    static int format_init = rw_printf ("%{+!}", _rw_fmtstring);

    _RWSTD_UNUSED (format_init);
}
