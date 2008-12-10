// -*- C++ -*-
/***************************************************************************
 *
 * streambuf.cc - definitions of basic_streambuf members
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
 * Copyright 1994-2006 Rogue Wave Software, Inc.
 * 
 **************************************************************************/


_RWSTD_NAMESPACE (std) { 


template <class _CharT, class _Traits>
basic_streambuf<_CharT, _Traits>::
basic_streambuf (ios_base::openmode __mode /* = ios_base::in | ios_base::out */)
    : _C_buffer (0),
    _C_bufsize (0),
    _C_state (__mode),
    _C_eback (0),
    _C_gptr (0),
    _C_egptr (0),
    _C_pbase (0),
    _C_pptr (0),
    _C_epptr (0)
{
    // no op
}


template <class _CharT, class _Traits>
typename basic_streambuf<_CharT, _Traits>::int_type
basic_streambuf<_CharT, _Traits>::
uflow ()
{
    _RWSTD_ASSERT (_C_is_valid ());

    const int_type __c = underflow ();

    if (traits_type::eq_int_type (__c, traits_type::eof ()))
        return traits_type::eof ();

    // handle unbuffered input mode
    if (_C_gptr < _C_egptr)
        return traits_type::to_int_type (*_C_gptr++);

    return __c;
}


template <class _CharT, class _Traits>
streamsize
basic_streambuf<_CharT, _Traits>::
xsgetn (char_type* __buf, streamsize __n)
{
    _RWSTD_ASSERT (_C_is_valid ());

    // number of characters read
    streamsize __nget = 0;

    while (__n) {

        // save the value returned from underflow() since the function
        // can fill the buffer and still fail, say due to a code
        // conversion error

        const int_type __c = gptr () == egptr () ?
            underflow () : traits_type::not_eof (traits_type::eof ());

        // number of characters available in get area
        streamsize __navail = egptr () - gptr ();

        if (0 < __navail) {
            if (__navail > __n)
                __navail = __n;

            // copy contents of get area to the destination buffer
            traits_type::copy (__buf + __nget, gptr (), __navail);

            // increment pointers and counts by the number of characters
            // copied
            gbump (__navail);
            __n    -= __navail;
            __nget += __navail;
        }
        else if (traits_type::eq_int_type (__c, traits_type::eof ())) {
            // break out on underflow() failure (e.g., reaching EOF)
            break;
        }
        else {
            // unbuffered mode: empty pending seuqence but non-EOF
            // overflow() return value

            // append character returned by underflow()
            traits_type::assign (__buf [__nget],
                                 traits_type::to_char_type (__c));

            // avoid incrementing egptr() but adjust counters
            --__n;
            ++__nget;
        }
    }

    return __nget;
}


template<class _CharT, class _Traits>
streamsize
basic_streambuf<_CharT, _Traits>::
xsputn (const char_type* __buf, streamsize __n)
{
    _RWSTD_ASSERT (0 != __buf || 0 == __n);
    _RWSTD_ASSERT (_C_is_valid ());

    if (__n <= 0 || !_C_is_out ())
        return 0;

    streamsize __nput = 0;

    for (; ; ++__buf, --__n, ++__nput) {
        const streamsize __navail = epptr () - pptr ();
        const streamsize __nwrite = __navail < __n ? __navail : __n;

        traits_type::copy (pptr (), __buf, __nwrite);

        pbump (__nwrite);

        __nput += __nwrite;

        if (__nwrite == __n)
            break;

        __buf += __nwrite;
        __n   -= __nwrite;

        const int_type __c = overflow (traits_type::to_int_type (*__buf));

        if (traits_type::eq_int_type (traits_type::eof (), __c))
            break;
    }

    return __nput;
}


}   // namespace std
