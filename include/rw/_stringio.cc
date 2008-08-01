/***************************************************************************
 *
 * _stringio.cc - definitions of the string extractors
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
 * Copyright 1994-2008 Rogue Wave Software.
 * 
 **************************************************************************/


_RWSTD_NAMESPACE (std) { 


_EXPORT
template<class _CharT, class _Traits, class _Allocator>
basic_istream<_CharT, _Traits>&
operator>> (basic_istream<_CharT, _Traits>&            __is, 
            basic_string<_CharT, _Traits, _Allocator>& __str)
{
    _RWSTD_ASSERT (0 != __is.rdbuf ());

    const typename basic_istream<_CharT, _Traits>::sentry
        __ipfx (__is /* , noskipws = false */);

    ios_base::iostate __err = ios_base::goodbit;

    typedef _RWSTD_SIZE_T _SizeT;

    // count of characters read from stream
    _SizeT __gcount = 0;

    _TRY {

        if (__ipfx) {

            __str.clear ();

            // maximum number of characters we can read
            _RWSTD_SIZE_T __n =
                __is.width () ? __is.width () : __str.max_size ();

            basic_streambuf<_CharT, _Traits>* const __rdbuf = __is.rdbuf ();

            const ctype<_CharT> &__ctp =
                _USE_FACET (ctype<_CharT>, __is.getloc ());

#ifndef _RWSTD_NO_FRIEND_TEMPLATE

            while (__n != 0) {

                const _CharT* const __gptr  = __rdbuf->gptr ();
                const _CharT* const __egptr = __rdbuf->egptr ();

                // maximum number of characters would want to extract
                _SizeT __navail = __egptr - __gptr;
                if (__n < __navail)
                    __navail = __n;

                if (__navail) {

                    // find the delimeter in the squence if it exists, or
                    // get pointer to end of sequence
                    const _CharT* __pdel = __gptr;
                    for (/**/; __pdel != __egptr; ++__pdel) {

                        const typename _Traits::int_type
                            __c = _Traits::to_int_type(*__pdel);

                        if (_Traits::eq_int_type (__c, _Traits::eof ())) {
                            __err = ios_base::eofbit;
                            break;
                        }

                        if (__ctp.is (__ctp.space, *__pdel))
                            break;
                    }

                    // __pdel is either pointing to a delimiter or one past
                    // the end of the input stream get area. if it is past
                    // the end, then set it to null.
                    if (__pdel == __egptr) {
                        __pdel = 0;
                    }

                    if (__pdel) {
                        __navail = __pdel - __gptr + 1;
                        __n     -= __navail - 1;
                    }
                    else if (__n == __navail)
                        __n -= --__navail;
                    else
                        __n -= __navail;

                    // store characters excluding the delimiter
                    __str.append (__gptr, __navail - !!__pdel);

                    __gcount += __navail;

                    // advance gptr() by the number of extracted
                    // characters, including the delimiter
                    __rdbuf->gbump (__navail);

                    // we found a delimiter before the end of the get area,
                    // break out of outer loop
                    if (__pdel) {
                        break;
                    }

                    if (2 > __n && _SizeT (__egptr - __gptr) != __navail) {
                        __err = ios_base::failbit;
                        break;
                    }
                }
                else {

                    // n data in buffer, trigger underflow()
                    // note that streambuf may be unbuffered
                    const typename _Traits::int_type
                        __c = __rdbuf->sgetc ();

                    if (_Traits::eq_int_type (__c, _Traits::eof ())) {
                        __err = ios_base::eofbit;
                        break;
                    }

                    // convert to char_type so that isspace works correctly
                    const typename _Traits::char_type
                        __ch = _Traits::to_char_type (__c);

                    if (__ctp.is (__ctp.space, __ch))
                        break;

                    __str.push_back (__ch);
                    --__n;

                    __rdbuf->sbumpc ();

                    // increment gcount only _after_ sbumpc() but _before_
                    // the subsequent call to sgetc() to correctly reflect
                    // the number of extracted characters in the presence
                    // of exceptions thrown from streambuf virtuals
                    ++__gcount;
                }
            }

#else   // if defined (_RWSTD_NO_FRIEND_TEMPLATE)

            for ( ; __n != 0; ) {

                const typename _Traits::int_type
                    __c (__rdbuf->sgetc ());

                if (_Traits::eq_int_type (__c, _Traits::eof ())) {
                    __err = ios_base::eofbit;
                    break;
                }

                // convert to char_type so that isspace works correctly
                const typename _Traits::char_type
                    __ch = _Traits::to_char_type (__c);

                if (__ctp.is (__ctp.space, __ch))
                    break;

                __str.push_back (__ch);
                --__n;

                __rdbuf->sbumpc ();

                // increment gcount only _after_ sbumpc() but _before_
                // the subsequent call to sgetc() to correctly reflect
                // the number of extracted characters in the presence
                // of exceptions thrown from streambuf virtuals
                ++__gcount;
            }

#endif   // if defined (_RWSTD_NO_FRIEND_TEMPLATE)

            __is.width (0);
        }
    }
    _CATCH (...) {
        __is.setstate (ios_base::badbit | _RW::__rw_rethrow);
    }

    if (!__gcount)
        __err |= ios_base::failbit;

    if (__err)
        __is.setstate (__err);

    return __is;
}


_EXPORT
template<class _CharT, class _Traits, class _Allocator>
basic_istream<_CharT, _Traits>&
getline (basic_istream<_CharT, _Traits>&            __is, 
         basic_string<_CharT, _Traits, _Allocator>& __str, 
         _CharT                                     __delim)
{
    _RWSTD_ASSERT (0 != __is.rdbuf ());

    const typename basic_istream<_CharT, _Traits>::sentry
        __ipfx (__is, true /* noskipws */);

#ifndef _RWSTD_NO_FRIEND_TEMPLATE

    ios_base::iostate __err = ios_base::goodbit;

    typedef _RWSTD_SIZE_T _SizeT;

    _SizeT __gcount = 0;

    if (__ipfx) { 

        __str.clear ();

        // carefuly handle arithmetic overflow
        _SizeT __n = __str.max_size ();
        if (__n + _SizeT (1))
            ++__n;

        basic_streambuf<_CharT, _Traits>* const __rdbuf = __is.rdbuf ();

        _TRY {

            for ( ; ; ) {

                typedef typename _Traits::int_type int_type;

                const _CharT* const __gptr  = __rdbuf->gptr ();
                const _CharT* const __egptr = __rdbuf->egptr ();

                // compute the lesser of the number of characters in the
                // stream buffer and the size of the destination buffer
                _SizeT __navail = __egptr - __gptr;
                if (__n < __navail)
                    __navail = __n;

                if (__navail) {

                    // find the delimiter in the sequence if it exists
                    const _CharT* const __pdel =
                        _Traits::find (__gptr, __navail, __delim);

                    if (__pdel) {
                        __navail = __pdel - __gptr + 1;
                        __n     -= __navail - 1;
                    }
                    else if (__n == __navail)
                        __n -= --__navail;
                    else
                        __n -= __navail;

                    // store characters excluding the delimiter
                    __str.append (__gptr, __navail - !!__pdel);

                    __gcount += __navail;

                    // advance gptr() by the number of extracted
                    // characters, including the delimiter
                    __rdbuf->gbump (__navail);

                    if (__pdel) {
                        break;
                    }

                    if (2 > __n && _SizeT (__egptr - __gptr) != __navail) {
                        __err = ios_base::failbit;
                        break;
                    }
                }
                else {

                    // no data in buffer, trigger underflow()
                    // note that streambuf may be unbuffered
                    const int_type __c (__rdbuf->sgetc ());

                    if (_Traits::eq_int_type (__c, _Traits::eof ())) {
                        __err = ios_base::eofbit;
                        break;
                    }

                    const _CharT __ch = _Traits::to_char_type (__c);
                    if (_Traits::eq (__ch, __delim)) {
                        __rdbuf->sbumpc ();
                        __gcount++;
                        break;
                    }

                    if (2 > __n) {
                        __err = ios_base::failbit;
                        break;
                    }

                    __str += __ch;
                    --__n;

                    __rdbuf->sbumpc ();

                    // increment gcount only _after_ sbumpc() but _before_
                    // the subsequent call to sgetc() to correctly reflect
                    // the number of extracted characters in the presence
                    // of exceptions thrown from streambuf virtuals
                    ++__gcount;
                }
            }
        }
        _CATCH (...) {
            __is.setstate (ios_base::badbit | _RW::__rw_rethrow);
        }
    }

    if (!__gcount)
        __err |= ios_base::failbit;

#else   // if defined (_RWSTD_NO_FRIEND_TEMPLATE)

    ios_base::iostate __err = ios_base::failbit;

    if (__ipfx) { 

        _TRY {

            __str.clear ();

            const _RWSTD_SIZE_T __max_size = __str.max_size ();

            // FIXME: code commented out to work around an HP aCC 3.14.10
            // bug #JAGac86264

            // typedef typename
            //     basic_string<_CharT, _Traits, _Allocator>::size_type

            for ( ; ; ) {

                const typename _Traits::int_type
                    __c (__is.rdbuf ()->sgetc ());

                if (_Traits::eq_int_type (__c, _Traits::eof ())) {
                    // 21.3.7.9, p7
                    if (__str.size ())
                        __err = ios_base::eofbit;
                    else
                        __err = ios_base::eofbit | ios_base::failbit;

                    break;
                }

                if (_Traits::eq (_Traits::to_char_type (__c), __delim)) {
                    __is.rdbuf ()->sbumpc ();
                    __err = ios_base::goodbit;
                    break;
                }

                if (__max_size <= __str.size ())
                    break;

                __str.push_back (_Traits::to_char_type (__c));

                __is.rdbuf ()->sbumpc ();
            }
        }
        _CATCH (...) {
            __is.setstate (ios_base::badbit | _RW::__rw_rethrow);
        }
    }

#endif   // _RWSTD_NO_FRIEND_TEMPLATE

    if (__err)
        __is.setstate (__err);

    return __is;
}  


}   // namespace std
