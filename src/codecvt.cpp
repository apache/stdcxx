/***************************************************************************
 *
 * codecvt.cpp - definition of codecvt<char, char, mbstate_t> members
 *
 * $Id$
 *
 ***************************************************************************
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

#define _RWSTD_LIB_SRC

#include <rw/_defs.h>

#ifndef _RWSTD_NO_V3_LOCALE


// working around a Compaq C++ bug (see PR #26778)
#if __DECCXX_VER >= 60300000 && __DECCXX_VER < 60400000

#  include <stdarg.h>
_USING (std::va_list);

// override autoconfigured macro whose value is incorrect
// if <unistd.h> is #included before <iconv.h>
#  include <unistd.h>
#  define _RWSTD_NO_ICONV_CONST_CHAR

#endif   // Compaq C++ 6.3

#include <limits.h>
#include <locale.h>
#include <string.h>   // for memcmp()
#include <errno.h>
#include <wchar.h>    // for mbsinit()

#if    defined (__SUNPRO_CC) && __SUNPRO_CC <= 0x540 \
    && (defined (__SunOS_5_8) || defined (__SunOS_5_9))
   // working around SunOS/SunPro header dependencies (see PR #26255)
#  undef _TIME_T
#endif   // SunPro <= 5.4 && SunOS 5.{8,9}

#include <loc/_codecvt.h>
#include <loc/_locale.h>
#include <loc/_localedef.h>

#include "setlocale.h"
#include "use_facet.h"


_RWSTD_NAMESPACE (__rw) {

static inline int __rw_mbsinit (const _RWSTD_MBSTATE_T *ps)
{
#ifndef _RWSTD_NO_MBSINIT

    return ::mbsinit (ps);

#else   // if defined (_RWSTD_NO_MBSINIT)

    // commented out to work around an HP aCC 1.21 bug
    /* static */ const _RWSTD_MBSTATE_T state = _RWSTD_MBSTATE_T ();
    return !ps || 0 == memcmp (ps, &state, sizeof state);

#endif   // _RWSTD_NO_MBSINIT

}


}   // namespace __rw


_RWSTD_NAMESPACE (_V3_LOCALE) {


_RW::__rw_facet_id codecvt<char, char, _RWSTD_MBSTATE_T>::id;


/* virtual */ codecvt<char, char, _RWSTD_MBSTATE_T>::~codecvt ()
{
    // no-op
}


/* virtual */ codecvt_base::result
codecvt<char, char, _RWSTD_MBSTATE_T>::
do_out (state_type         &state,
        const intern_type  *from,
        const intern_type  *from_end,
        const intern_type *&from_next,
        extern_type        *to,
        extern_type        *to_end,
        extern_type       *&to_next) const
{
    // verify that both ranges are valid
    _RWSTD_ASSERT (from <= from_end);
    _RWSTD_ASSERT (to <= to_end);
    _RWSTD_ASSERT (from && from_end || !from && !from_end);
    _RWSTD_ASSERT (to && to_end || !to && !to_end);

    // next pointers must always be set before returning, even on error
    from_next = from;
    to_next   = to;

#ifdef _RWSTDDEBUG

    // verify that the conversion state is valid
    const int mbstate_valid = _RW::__rw_mbsinit (&state);

    _RWSTD_ASSERT (0 != mbstate_valid);

#else   // if !defined (_RWSTDDEBUG)

    _RWSTD_UNUSED (state);

#endif   // _RWSTDDEBUG

    // be prepared to handle an overridden do_always_noconv()
    // that returns false (highly unlikely but possible)
    if (always_noconv ())
        return noconv;

    const _RWSTD_SIZE_T nfrom = from_end - from;
    const _RWSTD_SIZE_T nto   = to_end - to;

    const _RWSTD_SIZE_T nconv = nfrom < nto ? nfrom : nto;

    // use memmove() in case ranges overlap
    memmove (to, from, nconv);

    from_next += nconv;
    to_next   += nconv;

    // Table 53, and lwg issue 382: do_out() returns partial
    // if not all source characters could be converted (e.g.,
    // because the destination range is full)
    return nto < nfrom ? partial : ok;
}


/* virtual */ codecvt_base::result
codecvt<char, char, _RWSTD_MBSTATE_T>::
do_in (state_type         &state,
       const extern_type  *from,
       const extern_type  *from_end,
       const extern_type *&from_next,
       intern_type        *to,
       intern_type        *to_end,
       intern_type       *&to_next) const
{
    // verify that both ranges are valid
    _RWSTD_ASSERT (from <= from_end);
    _RWSTD_ASSERT (to   <= to_end);
    _RWSTD_ASSERT (from && from_end || !from && !from_end);
    _RWSTD_ASSERT (to && to_end || !to && !to_end);

    typedef codecvt<char, char, _RWSTD_MBSTATE_T> This;

    // call do_out() above (avoid calling overridden do_out(), if any)
    return This::do_out (state,
                         from, from_end, from_next,
                         to, to_end, to_next);
}


/* virtual */ codecvt_base::result
codecvt<char, char, _RWSTD_MBSTATE_T>::
do_unshift (state_type   &state,
            extern_type  *to,
            extern_type  *to_end,
            extern_type *&to_next) const
{
    // verify that the range is valid
    _RWSTD_ASSERT (to <= to_end);
    _RWSTD_ASSERT (to && to_end || !to && !to_end);

    _RWSTD_UNUSED (to_end);

    // next pointer must always be set before returning, even on error
    to_next = to;

    const int mbstate_valid = _RW::__rw_mbsinit (&state);
    _RWSTD_ASSERT (mbstate_valid);

    return mbstate_valid ? noconv : error;
}


/* virtual */ int 
codecvt<char, char, _RWSTD_MBSTATE_T>::
do_encoding () const _THROWS (())
{
    return 1;   // 1 external char converts to a single internal char
}


/* virtual */ bool
codecvt<char, char, _RWSTD_MBSTATE_T>::
do_always_noconv () const _THROWS (())
{
    return true;   // conversion never necessary
}


// signature follows lwg issue 75
/* virtual */ int
codecvt<char, char, _RWSTD_MBSTATE_T>::
do_length (state_type        &state,
           const extern_type *from,
           const extern_type *from_end,
           _RWSTD_SIZE_T      imax) const
{
    // 22.2.1.5.2, p9 - preconditions
    _RWSTD_ASSERT (from <= from_end);

    // verify that the range is valid
    _RWSTD_ASSERT (from && from_end || !from && !from_end);

    const int mbstate_valid = _RW::__rw_mbsinit (&state);
    _RWSTD_ASSERT (mbstate_valid);

    if (!mbstate_valid)
        return 0;

    // 22.2.1.5.2, p10
    const _RWSTD_SIZE_T len = from_end - from;
    return int (len < imax ? len : imax);
}


/* virtual */ int
codecvt<char, char, _RWSTD_MBSTATE_T>::
do_max_length () const _THROWS (())
{
    return 1;   // 22.2.1.5.2, p11
}


}   // namespace _V3_LOCALE

#define TARGS_C   <char, char, _RWSTD_MBSTATE_T>

_RWSTD_DEFINE_FACET_FACTORY (static, codecvt, TARGS_C, codecvt);
_RWSTD_SPECIALIZE_USE_FACET (codecvt);

#endif   // _RWSTD_NO_V3_LOCALE
