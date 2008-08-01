/***************************************************************************
 *
 * istream.cc - definitions of basic_istream members
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/


_RWSTD_NAMESPACE (__rw) { 


_EXPORT
template <class _CharT, class _Traits, class _NativeType>
_STD::basic_istream<_CharT, _Traits>&
__rw_extract (_STD::basic_istream<_CharT, _Traits> &__strm,
              _NativeType                          &__val)
{
    _RWSTD_ASSERT (0 != __strm.rdbuf ());

    typedef _STD::istreambuf_iterator<_CharT, _Traits> _Iter;
    typedef _STD::num_get<_CharT, _Iter>               _NumGet;

    _STD::ios_base::iostate __err = _STD::ios_base::goodbit;

    // eat leading whitespace, catching exceptions from sentry
    const typename _STD::basic_istream<_CharT, _Traits>::sentry
        __ipfx (__strm /* , noskipws = false */);

    if (__ipfx) {
        _TRY {
            _STD_USE_FACET (_NumGet, __strm.getloc ())
                .get (_Iter (__strm), _Iter (), __strm, __err, __val);
        }
        _CATCH (...) {
            __strm.setstate (__strm.badbit | _RW::__rw_rethrow);
        }
    }

    if (_STD::ios_base::goodbit != __err)
        __strm.setstate (__err);

    return __strm;
}


}   // namespace __rw


_RWSTD_NAMESPACE (std) { 


template<class _CharT, class _Traits>
basic_istream<_CharT, _Traits>&
basic_istream<_CharT, _Traits>::
_C_ipfx (bool __noskipws, ios_base::iostate __errbits)
{
    // extension - zero out irrespective of state so that gcount()
    // called after any failed extraction (even formatted) reports 0
    // and always count _all_ extracted characters

    if (ios_base::goodbit != this->rdstate ()) {
        this->setstate (ios_base::failbit);
        return *this;
    }

    ios_base::iostate __state = ios_base::goodbit;

    _TRY {        
        if (this->tie ())
            this->tie ()->flush ();

        if (   ios_base::eofbit == __errbits // passed by std::ws
            || !__noskipws && this->flags () & ios_base::skipws) {

            // skip over leading whitespace

            basic_streambuf<char_type, traits_type>* const __rdbuf =
                this->rdbuf ();

            _RWSTD_ASSERT (0 != __rdbuf);

            const ctype<char_type> &__ctp =
                _USE_FACET (ctype<char_type>, this->getloc ());

            for ( ; ; ) {

                const char_type* const __gptr  = __rdbuf->gptr ();
                const char_type* const __egptr = __rdbuf->egptr ();

                if (__gptr < __egptr) {

                    // scan the sequence for the first non-whitespace char
                    const streamsize __nskip =
                          __ctp.scan_not (__ctp.space, __gptr, __egptr)
                        - __gptr;

                    __rdbuf->gbump (__nskip);

                    if (__nskip < __egptr - __gptr)
                        break;
                }
                else {

                    const int_type __c (__rdbuf->sgetc ());

                    if (traits_type::eq_int_type (__c, traits_type::eof ())) {
                        __state = __errbits;
                        break;
                    }

                    if (!__ctp.is (__ctp.space,
                                   traits_type::to_char_type (__c)))
                        break;

                    __rdbuf->sbumpc ();
                }
            }
        }
    }
    _CATCH (...) {
        this->setstate (ios_base::badbit | _RW::__rw_rethrow);
    }

    if (__state)
        this->setstate (__state);

    return *this;
}


template<class _CharT, class _Traits>
typename basic_istream<_CharT, _Traits>::int_type
basic_istream<_CharT, _Traits>::
_C_unsafe_get (streamsize *__cnt   /* = 0     */,               
               int_type    __delim /* = eof() */,
               int         __flags /* = 0     */)
{
    _RWSTD_ASSERT (0 != this->rdbuf ());

    ios_base::iostate __err = ios_base::goodbit;

    // initialize in case sgetc() below throws
    int_type __c (traits_type::eof ());

    _TRY {

        // get next char without extracting
        __c = this->rdbuf ()->sgetc ();

        // extract delimiter or eos only if asked to
        // extract whitespace only if it doesn't terminate input
        if (traits_type::eq_int_type (__c, traits_type::eof ()))
            __err =   ios_base::eofbit
                    | (__flags & _C_faileof ? this->failbit : this->goodbit);
        else if (   (   _C_eatdelim & __flags
                     || !traits_type::eq_int_type (__c, __delim))
                 && (   _C_eatnull & __flags
                     || !traits_type::eq (traits_type::to_char_type (__c),
                                          char_type ()))
                 && (   !(_C_wsterm & __flags)
                     || !_USE_FACET (ctype<char_type>, this->getloc ())
                            .is (ctype_base::space,
                                 traits_type::to_char_type (__c)))) {
            if (traits_type::eq_int_type (this->rdbuf ()->sbumpc (),
                                          traits_type::eof ()))
                __err = ios_base::failbit;
            else if (__cnt)
                ++*__cnt;   // increment number of extracted chars
        }
    }
    _CATCH (...) {
        this->setstate (ios_base::badbit | _RW::__rw_rethrow);
    }

    if (__err)
        this->setstate (__err);

    return __c;    
}


template<class _CharT, class _Traits>
basic_istream<_CharT, _Traits>&
basic_istream<_CharT, _Traits>::
_C_get (basic_streambuf<char_type, traits_type> &__sb, int_type __delim)
{
    _RWSTD_ASSERT (0 != this->rdbuf ());

    ios_base::iostate __err = ios_base::goodbit;   

    const sentry __ipfx (*this, true /* noskipws */);

    if (__ipfx) {

        _C_gcount = 0;

        _TRY {
            for ( ; ; ) {
                int_type __c (this->rdbuf ()->sgetc ());

                if (traits_type::eq_int_type (__c, traits_type::eof ())) {
                    __err = ios_base::eofbit;
                    break;
                }

                if (traits_type::eq_int_type (__c, __delim))
                    break;

                _TRY {
                    __c = __sb.sputc (traits_type::to_char_type (__c));
                }
                _CATCH (...) {

                    // indicate to the outer catch block the the
                    // exception rethrown from the call to setstate()
                    // below should be propagated without setting
                    // badbit (whose purpose is to indicates the
                    // "badness" of *this, not some other object)
                    __err = ios_base::failbit | _RW::__rw_rethrow;

                    // set failbit and rethrow if failbit is set in
                    // exceptions() despite 27.6.1.3, p12, bullet 4
                    this->setstate (__err);
                    break;
                }

                if (traits_type::eq_int_type (__c, traits_type::eof ())) {
                    __err = ios_base::failbit;  
                    break;
                }

                ++_C_gcount;
                this->rdbuf ()->sbumpc ();  
            }
        }
        _CATCH (...) {
            // will rethrow if `err' is set since the caught exception
            // is the one thrown during output and rethrown by setstate()
            // in the nested catch block above
            if (ios_base::goodbit == __err)
                __err = ios_base::badbit;

            this->setstate (__err | _RW::__rw_rethrow);
        }
    }

    // 27.6.1.3, p13
    if (!gcount ())
        __err |= ios_base::failbit;

    if (__err)
        this->setstate (__err);

    return *this;
}


template<class _CharT, class _Traits>
basic_istream<_CharT, _Traits>&
basic_istream<_CharT, _Traits>::
read (char_type *__s, streamsize __n, int_type __delim, int __flags)
{
    // if __s is 0 function doesn't store characters (used by ignore)

    _RWSTD_ASSERT (0 <= __n);
    _RWSTD_ASSERT (0 != this->rdbuf ());

    const char_type __eos = char_type ();

    const bool __getline =     (_C_failend | _C_nullterm)
                           == ((_C_failend | _C_nullterm) & __flags);

    // "clear" buffer if reading a null-terminated string
    // follows the proposed resolution of lwg issue 243
    if (__getline)
        traits_type::assign (*__s, __eos);

    // read and getline are unformatted (noskipws), others are formatted
    const sentry __ipfx (*this, !(_C_skipws & __flags));

    // 27.6.1.2.1, p1 and 27.6.1.3, p1: proceed iff sentry is okay
    if (__ipfx) {

        _C_gcount = 0;

        // read at most n - 1 characters when null-terminating
        while (__n) {

            // handle 27.6.1.3, p17 and p18
            if (0 == --__n && _C_nullterm & __flags) {
                if (!__getline)
                    break;

                const int_type __c (this->rdbuf ()->sgetc ());
                
                if (   !traits_type::eq_int_type (__c, traits_type::eof ())
                    && !traits_type::eq_int_type (__c, __delim))
                    break;

                __flags &= ~_C_failend;   // prevent failbit from being set

                // will break out in the check below...
            }

            // get (possibly extract) next char
            const int_type __c (_C_unsafe_get (&_C_gcount, __delim, __flags));

            // eof or delimiter may terminate input
            if (   traits_type::eq_int_type (__c, traits_type::eof())
                || traits_type::eq_int_type (__c, __delim))
                break;

            const char_type __ch = traits_type::to_char_type (__c);

            // space or eos char may terminate input
            if (   !(_C_eatnull & __flags) && traits_type::eq (__ch, __eos)
                || _C_wsterm & __flags
                && _USE_FACET (ctype<char_type>, this->getloc ())
                       .is (ctype_base::space, __ch))
                break;

            // ignore input if __s is null
            if (__s)
                traits_type::assign (*__s++, __ch);
        }

        if (_C_nullterm & __flags && __s)
            traits_type::assign (*__s, __eos);
    }

    // fail if no chars extracted or if end of buffer has been reached
    if (   _C_failnoi & __flags && !gcount ()
        || _C_failend & __flags && !__n)
        this->setstate (ios_base::failbit);

    return *this;
}


// 27.6.1.3, p28
template<class _CharT, class _Traits>
basic_istream<_CharT, _Traits>&
basic_istream<_CharT, _Traits>::
read (char_type *__s, streamsize __n)
{
    _RWSTD_ASSERT (0 <= __n);
    _RWSTD_ASSERT (0 != this->rdbuf ());

    // 27.6.1.3, p28: sets ios_base::failbit if !this->good()
    const sentry __ipfx (*this, true /* noskipws */);

    // 27.6.1.2.1, p1 and 27.6.1.3, p1: proceed iff sentry is okay
    if (__ipfx) {

        _C_gcount = 0;

        streamsize __nread = 0;

        _TRY {
            __nread = this->rdbuf ()->sgetn (__s, __n);

            if (__nread >= 0)
                _C_gcount = __nread;
        }
        _CATCH (...) {
            this->setstate (ios_base::badbit | _RW::__rw_rethrow);
        }

        if (__n != __nread)
            this->setstate (ios_base::eofbit | ios_base::failbit);
    }

    return *this;
}


template<class _CharT, class _Traits>
streamsize
basic_istream<_CharT, _Traits>::
readsome (char_type *__s, streamsize __n)
{
    _RWSTD_ASSERT (0 != __s);
    _RWSTD_ASSERT (0 <= __n);
    _RWSTD_ASSERT (0 != this->rdbuf());

    const sentry __ipfx (*this, true /* noskipws */);

    if (__ipfx) {

        _C_gcount = 0;

        streamsize __nread = 0;

        _TRY {

            const streamsize __in_avail = this->rdbuf ()->in_avail ();

            if (!__in_avail)
                return 0;

            if (__n > __in_avail)
                __n = __in_avail;

            // the call to sgetn() below is required to return >= 0
            __nread = __n >= 0 ? this->rdbuf ()->sgetn (__s, __n) : -1;

            if (__nread >= 0)
                _C_gcount = __nread;
        }
        _CATCH (...) {
            this->setstate (ios_base::badbit | _RW::__rw_rethrow);
        }

        if (__nread < 0) {
            this->setstate (ios_base::eofbit);
            __nread = 0;
        }

        return __nread;
    }
    else
        this->setstate (ios_base::failbit);

    return 0;
}


template<class _CharT, class _Traits>
typename basic_istream<_CharT, _Traits>::pos_type
basic_istream<_CharT, _Traits>::
tellg ()
{
    _RWSTD_ASSERT (0 != this->rdbuf ());

    const sentry __ipfx (*this, true /* noskipws */);

    if (__ipfx && !this->fail ()) {

        _TRY {
            return this->rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::in);
        }
        _CATCH (...) {
            this->setstate (ios_base::badbit | _RW::__rw_rethrow);
        }
    }

    return off_type (-1);
}


template<class _CharT, class _Traits>
basic_istream<_CharT, _Traits>&
basic_istream<_CharT, _Traits>::
putback (char_type __c)
{
    const sentry __ipfx (*this, true /* noskipws */);

    if (__ipfx) {

        if (this->rdbuf ()) {
            _TRY {
                if (!traits_type::eq_int_type (this->rdbuf ()->sputbackc (__c),
                                               traits_type::eof ()))
                    return *this;
            }
            _CATCH (...) {
                this->setstate (ios_base::badbit | _RW::__rw_rethrow);
            }
        }

        this->setstate (ios_base::badbit);
    }

    return *this;
}


template<class _CharT, class _Traits>
basic_istream<_CharT, _Traits>&
basic_istream<_CharT, _Traits>::
unget ()
{
    const sentry __ipfx (*this, true /* noskipws */);

    if (__ipfx) {

        if (this->rdbuf ()) {

            _TRY {

                if (!traits_type::eq_int_type (this->rdbuf ()->sungetc (), 
                                               traits_type::eof ()))
                    return *this;
            }
            _CATCH (...) {
                this->setstate (ios_base::badbit | _RW::__rw_rethrow);
            }
        }

        this->setstate (ios_base::badbit);
    }
    
    return *this;
}


template<class _CharT, class _Traits>
int
basic_istream<_CharT, _Traits>::
sync ()
{
    const sentry __ipfx (*this, true /* noskipws */);

    if (__ipfx && this->rdbuf ()) {

        _TRY {
            if (-1 != this->rdbuf ()->pubsync ())
                return 0;
        }
        _CATCH (...) {
            this->setstate (ios_base::badbit | _RW::__rw_rethrow);
        }

        this->setstate (ios_base::badbit);
    }

    return -1;
}


template<class _CharT, class _Traits>
basic_istream<_CharT, _Traits>&
basic_istream<_CharT, _Traits>::
get (char_type *__s, streamsize __n, char_type __delim)
{
    _RWSTD_ASSERT (0 != this->rdbuf ());

    if (0 < __n) {
        // lwg issue 243: store the NUL character before
        // constructing the sentry object in case it throws
        traits_type::assign (__s [0], char_type ());
    }

    const sentry __ipfx (*this, true /* noskipws */);

    ios_base::iostate __err = ios_base::goodbit;

    if (__ipfx) {

        _C_gcount = 0;

        _TRY {

            basic_streambuf<char_type, traits_type>* const __rdbuf =
                this->rdbuf ();

            for ( ; __n > 1; ) {

                streamsize __navail =
                    streamsize (__rdbuf->egptr () - __rdbuf->gptr ());

                if (__n - 1 < __navail)
                    __navail = __n - 1;

                if (__navail) {

                    char_type* const __gptr = __rdbuf->gptr ();

                    const char_type* const __pdelim =
                        traits_type::find (__gptr, __navail, __delim);

                    if (__pdelim) {
                        __navail = streamsize (__pdelim - __gptr);

                        if (!__navail)
                            break;
                    }

                    traits_type::copy (__s + _C_gcount, __gptr, __navail);

                    __rdbuf->gbump (__navail);

                    _C_gcount += __navail;
                    __n       -= __navail;
                }
                else {

                    const int_type __c (__rdbuf->sgetc ());

                    if (traits_type::eq_int_type (__c, traits_type::eof ())) {
                        __err = ios_base::eofbit;
                        break;
                    }

                    const char_type __ch = traits_type::to_char_type (__c);

                    if (traits_type::eq (__ch, __delim))
                        break;

                    traits_type::assign (__s [_C_gcount++], __ch);
                    --__n;

                    __rdbuf->sbumpc ();
                }
            }

            traits_type::assign (__s [_C_gcount], char_type ());

            if (!_C_gcount)
                __err |= ios_base::failbit;
        }
        _CATCH (...) {
            this->setstate (ios_base::badbit | _RW::__rw_rethrow);
        }
    }

    if (ios_base::goodbit != __err)
        this->setstate (__err);

    return *this;
}


template<class _CharT, class _Traits>
basic_istream<_CharT, _Traits>&
basic_istream<_CharT, _Traits>::
getline (char_type *__line, streamsize __size, char_type __delim)
{
    _RWSTD_ASSERT (__size >= 0);
    _RWSTD_ASSERT (!__size || __line);
    _RWSTD_ASSERT (0 != this->rdbuf ());

    char_type __dummy;

    if (0 < __size) {
        // lwg issue 243: store the NUL character before
        // constructing the sentry object in case it throws
        traits_type::assign (__line [0], char_type ());
    }
    else {
        // use a dummy buffer to avoid having to check size
        // again below, and prevent undefined behavior when
        // size is negative
        __line = &__dummy;
        __size = 0;
    }

    const sentry __ipfx (*this, true /* noskipws */);

    ios_base::iostate __err = ios_base::goodbit;

    if (__ipfx) {

        _C_gcount = 0;

        basic_streambuf<char_type, traits_type>* const __rdbuf =
            this->rdbuf ();

        _TRY {

            for ( ; ; ) {

                const char_type* const __gptr  = __rdbuf->gptr ();
                const char_type* const __egptr = __rdbuf->egptr ();

                // compute the lesser of the number of characters in the
                // stream buffer and the size of the destination buffer
                streamsize __navail = __egptr - __gptr;
                if (__size < __navail)
                    __navail = __size;

                if (__navail) {

                    // find the delimiter in the sequence if it exists
                    const char_type* const __pdel =
                        traits_type::find (__gptr, __navail, __delim);

                    if (__pdel) {
                        __navail = __pdel - __gptr + 1;
                        __size  -= __navail - 1;
                    }
                    else if (__size == __navail)
                        __size -= --__navail;
                    else
                        __size -= __navail;

                    // copy including delimiter, if any
                    // (delimiter will be overwritten below)
                    traits_type::copy (__line + _C_gcount, __gptr, __navail);

                    _C_gcount += __navail;

                    // advance gptr()
                    __rdbuf->gbump (__navail);

                    if (__pdel) {
                        traits_type::assign (__line [_C_gcount - 1],
                                             char_type ());
                        break;
                    }

                    if (2 > __size && __egptr - __gptr != __navail) {
                        traits_type::assign (__line [_C_gcount], char_type ());
                        __err = ios_base::failbit;
                        break;
                    }
                }
                else {

                    // no data in buffer, trigger underflow()
                    // note that streambuf may be unbuffered
                    const int_type __c (__rdbuf->sgetc ());

                    if (traits_type::eq_int_type (__c, traits_type::eof ())) {
                        traits_type::assign (__line [_C_gcount], char_type ());
                        __err = ios_base::eofbit;
                        break;
                    }

                    const char_type __ch = traits_type::to_char_type (__c);
                    if (traits_type::eq (__ch, __delim)) {
                        traits_type::assign (__line [_C_gcount], char_type ());
                        __rdbuf->sbumpc ();
                        _C_gcount++;
                        break;
                    }

                    if (2 > __size) {
                        traits_type::assign (__line [_C_gcount], char_type ());
                        __err = ios_base::failbit;
                        break;
                    }

                    traits_type::assign (__line [_C_gcount], __ch);
                    --__size;

                    __rdbuf->sbumpc ();

                    // increment gcount only _after_ sbumpc() but _before_
                    // the subsequent call to sgetc() to correctly reflect
                    // the number of extracted characters in the presence
                    // of exceptions thrown from streambuf virtuals
                    ++_C_gcount;
                }
            }
        }
        _CATCH (...) {
            this->setstate (ios_base::badbit | _RW::__rw_rethrow);
        }
    }

    if (0 == _C_gcount)
        __err |= ios_base::failbit;

    if (__err)
        this->setstate (__err);

    return *this;
}


#ifdef _RWSTD_NO_UNDEFINED_TEMPLATES

template <class _CharT, class _Traits>
basic_istream<_CharT, _Traits>::sentry::
sentry (const sentry&)
    : _RW::__rw_guard (0)
{
    _RWSTD_ASSERT (!"not callable");
}

template <class _CharT, class _Traits>
void
basic_istream<_CharT, _Traits>::sentry::
operator= (const sentry&)
{
    _RWSTD_ASSERT (!"not callable");
}

#endif   // _RWSTD_NO_UNDEFINED_TEMPLATES


}   // namespace std
