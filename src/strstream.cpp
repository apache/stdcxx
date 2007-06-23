/***************************************************************************
 *
 * strstream.cpp - Source for the Standard Library string stream classes
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#if    3 == __GNUG__ && 3 > __GNUC_MINOR__ \
    || 3 == __GNUC_MINOR__ && 1 >__GNUC_PATCHLEVEL__

  // working around a gcc bug (PR #29570)
#  include <rw/_config.h>
#  ifndef _RWSTD_NO_EXTERN_TEMPLATE
#    define _RWSTD_NO_EXTERN_TEMPLATE
#  endif
#endif   // gcc >= 3.0 && gcc < 3.3.1

#include <rw/_defs.h>

#include <limits.h>

#include <ios>
#include <strstream>


_RWSTD_NAMESPACE (std) {


/* virtual */
strstreambuf::~strstreambuf ()
{
    _RWSTD_ASSERT (_C_is_valid ());

    if (   _C_buffer && _C_state & _C_allocated
        && !(_C_state & _C_frozen)) {
        if (_C_pfree)
            _C_pfree (_C_buffer);
        else
            delete[] _C_buffer;
    }
}


/* virtual */ strstreambuf::int_type
strstreambuf::overflow (int_type c)
{
    _RWSTD_ASSERT (_C_is_valid ());

    if (!_C_is_out () || !(_C_state & _C_dynamic))
        return traits_type::eof ();
    
    if (_C_is_eof (c))
        return traits_type::not_eof (c);
    
    // reallocate space if necessary
    if (!(pptr () < epptr ())) {

        const streamsize new_size =
            _C_bufsize ? _C_bufsize * 2 : _C_alsize ? _C_alsize : 128;

        // take care to avoid calling the overridden setbuf(), if any
        if (!strstreambuf::setbuf (0, new_size))
            return traits_type::eof ();
    }

    return sputc (traits_type::to_char_type (c));
}


/* virtual */ strstreambuf::int_type
strstreambuf::pbackfail (int_type c)
{
    _RWSTD_ASSERT (_C_is_valid ());

    typedef traits_type Traits;

    if (eback () < gptr ()) {

        // putback position is available (see 27.5.1, p3, bullet 3)

        if (Traits::eq_int_type (c, Traits::eof ())) {

            // D.7.1.3, p6, bullet 3

            gbump (-1);
            return Traits::not_eof (c);
        }

        if (Traits::eq_int_type (Traits::to_int_type (*(gptr () - 1)), c)) {

            // D.7.1.3, p6, bullet 1

            gbump (-1);
            return c;
        }

        if (!(_C_state & _C_constant)) {

            // D.7.1.3, p6, bullet 2

            gbump (-1);
            Traits::assign (*gptr (), Traits::to_char_type (c));
            return c;
        }
    }

    // D.7.1.3, p7
    return Traits::eof ();
}


/* virtual */ strstreambuf::int_type
strstreambuf::underflow ()
{
    _RWSTD_ASSERT (_C_is_valid ());

    if (gptr () < egptr ()) {
        // read position is available (see 27.5.1, p3, bullet 4)

        // D.7.1.3, p9, bullet 1
        return traits_type::to_int_type (*gptr ());
    }

    if (pptr () > egptr ()) {

        // D.7.1.3, p9, bullet 2
        if (gptr ())
            setg (eback (), gptr (), pptr ());
        else
            setg (pbase (), pbase (), pptr ());

        return traits_type::to_int_type (*gptr ());
    }

    // D.7.1.3, p10
    return traits_type::eof ();
}


/* virtual */ strstreambuf::pos_type
strstreambuf::seekoff (off_type off, ios::seekdir way, ios::openmode which)
{
    _RWSTD_ASSERT (_C_is_valid ());

    // should implicitly hold as long as ios::seekdir is an enum
    _RWSTD_ASSERT (ios::beg == way || ios::cur == way || ios::end == way);
    _RWSTD_ASSERT (_C_is_valid ());

    // determine seekable area - D.7.1, p4
    char* const seeklo = eback ();
    char* const seekhi = epptr () ? epptr () : egptr ();

    const char* const xnext = which & ios::in ? gptr () : pptr ();
    const char* const xbeg  = which & ios::in ? eback () : pbase ();

    // D.7.1.3, p13
    if (!xnext)
        return pos_type (off_type (-1));

    off_type saved_off = off;

    // compute new offset - D.7.1.3, p13, table 105
    if (ios::cur == way)
        off += off_type (xnext - xbeg);
    else if (ios::end == way)
        off += off_type (seekhi - xbeg);

    // test conditions in D.7.1.3, p13, table 105, row 4
    if (off < seeklo - xbeg || off > seekhi - xbeg)
        off = -1;   // failure
    else if (which & ios::in) {   // position input sequence

        // fail if `way' is `cur', otherwise position output sequence first
        if (   which & ios::out
            && (   way == ios::cur
                || pos_type (-1) == seekoff (saved_off, way, ios::out)))
            return pos_type (off_type (-1));

        // adjust input sequence as necessary to maintain invariant
        if (off <= egptr () - eback ())
            setg (eback (), eback () + off, egptr ());
        else if (off <= pptr () - egptr ())   // advance egptr()
            setg (eback (), eback () + off, pptr ());
        else   // advance egptr() even further
            setg (eback (), eback () + off, epptr ());
    }
    else if (which & ios::out) {   // position output sequence
        if (seeklo + off < pbase ()) {
            // adjust beginning of output sequence, then increment pptr()
            setp (seeklo, epptr ());
            pbump (off);
        }
        else {
            // reset pptr() first, then increment it by offset
            setp (pbase (), epptr ());
            pbump (off - (pbase () - seeklo));
        }
    }
    else
        off = -1;   // failure

    _RWSTD_ASSERT (_C_is_valid ());

    return pos_type (off);
}


/* virtual */ strstreambuf::pos_type
strstreambuf::seekpos (pos_type sp, ios::openmode which)
{
    _RWSTD_ASSERT (_C_is_valid ());

    // call seekoff() defined by strstreambuf, and
    // not an overridden() virtual if one exists
    return strstreambuf::seekoff (sp - pos_type (), ios::beg, which);
}


/* virtual */ streambuf*
strstreambuf::setbuf (char_type* buf, streamsize bufsize)
{
    _RWSTD_ASSERT (_C_is_valid ());

    if (   !(_C_state & _C_dynamic) || (_C_state & _C_frozen)
        || !buf && !bufsize) {
        // lwg issue 66
        return 0;
    }

    // determine the existing (possibly disjoint) sequences
    // in the (possibly two distinct) buffer(s) and copy
    // them into the new buffer

    // [.....]  [.....]          or [....[.]...]
    // ^     ^  ^     ^             ^          ^
    // |     |  |     |             |          |
    // |     |  |     +- xend       |          +- xend, gap_beg, gap_end
    // |     |  +------- gap_end    +------------ xbeg
    // |     +---------- gap_beg
    // +---------------- xbeg

    const char_type* const xbeg =
        eback () && eback () < pbase () ? eback () : pbase ();

    const char_type* const xend =
        epptr () && epptr () < egptr () ? egptr () : epptr ();

    const char_type *gap_beg = pptr () < eback ()
        ? pptr () : egptr () < pbase () ? egptr () : pbase ();

    const char_type *gap_end = pptr () < eback ()
        ? eback () : egptr () < pbase () ? egptr () : pbase ();

    // if gap_end is before gap_beg, set both so as to make the gap zero size
    if (gap_end <= gap_beg)
        gap_beg = gap_end = xend;

    // compute the cumulative size of both sequences minus the gap
    const streamsize slen = streamsize ((xend - xbeg) - (gap_end - gap_beg));

    if (bufsize < slen || !_C_is_out ())
        return 0;   // failure

    const bool free_old_buf = _C_own_buf ();
    
    if (!buf) {
        _TRY {
            // allocate a new buffer
            buf = _C_palloc ?
                  _RWSTD_STATIC_CAST (char_type*, _C_palloc (bufsize))
                : new char [bufsize];
        }
        _CATCH (...) {
            // catch all exceptions, indicate failure by returning 0
        }

        if (!buf)
            return 0;

        _C_own_buf (true);
    }
    else
        _C_own_buf (false);

    // copy both sequences from the old buffer to the new buffer
    traits_type::copy (buf, xbeg, gap_beg - xbeg);
    traits_type::copy (buf + (gap_beg - xbeg), gap_end, xend - gap_end);

    if (free_old_buf) {
        if (_C_pfree)
            _C_pfree (_C_buffer);
        else
            delete[] _C_buffer;
    }
    
    _C_buffer  = buf;
    _C_bufsize = bufsize;

    const _RWSTD_PTRDIFF_T seq_1_size = gap_beg - xbeg;

    // restore output sequence
    const streamsize pptr_off = streamsize (pptr () - pbase ());

    if (pbase () < gap_beg)
        setp (_C_buffer, _C_buffer + _C_bufsize);
    else
        setp (_C_buffer + seq_1_size, _C_buffer + _C_bufsize);

    pbump (pptr_off);

    // restore input sequence
    char_type* const gbeg = _C_buffer + (eback () < gap_beg ? 0 : seq_1_size);
    setg (gbeg, gbeg + (gptr () - eback ()), gbeg + (egptr () - eback ()));

    return this;
}


void strstreambuf::
_C_init (streamsize  alsize,
         const void *gnext_v,
         streamsize  n,
         const void *pbeg_v,
         void*     (*palloc)(_RWSTD_SIZE_T),
         void      (*pdealloc)(void*),
         int         state)
{
    _C_alsize = alsize;
    _C_palloc = palloc;
    _C_pfree  = pdealloc;

    char* const gnext =
        _RWSTD_STATIC_CAST (char*, _RWSTD_CONST_CAST (void*, gnext_v));

    char* const pbeg =
        _RWSTD_STATIC_CAST (char*, _RWSTD_CONST_CAST (void*, pbeg_v));

    // D.7.1.1, p4
    if (n < 0)
        n = _RWSTD_INT_MAX;
    else if (!n && gnext)
        n = streamsize (traits_type::length (gnext));

    _C_buffer = gnext;
    _C_state  = state | ios_base::in | ios_base::out;

    if (pbeg) {
        // D.7.1.1, p6
        setg (gnext, gnext, pbeg);
        setp (pbeg, pbeg + n);
    }
    else
        // D.7.1.1, p5
        setg (gnext, gnext, gnext + n);

    _RWSTD_ASSERT (_C_is_valid ());
}


}   // namespace std
