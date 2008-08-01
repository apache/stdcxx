/***************************************************************************
 *
 * ostream.cc - Definitions for the Standard Library ostream classes
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/


_RWSTD_NAMESPACE (std) { 


template<class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>&
basic_ostream<_CharT, _Traits>::flush ()
{
    if (this->rdbuf ()) {

        // see LWG issue 581

        const sentry __opfx (*this);

        if (__opfx) {

            int __res = 0;

            _TRY {
                __res = this->rdbuf ()->pubsync ();
            }
            _CATCH (...) {
                this->setstate (ios_base::badbit | _RW::__rw_rethrow);
            }

            if (-1 == __res)
                this->setstate (ios_base::badbit);
        }
    }

    return *this;
}
  

template<class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>&
basic_ostream<_CharT, _Traits>::seekp (pos_type __pos)
{
    _RWSTD_ASSERT (0 != this->rdbuf ());

    if (!this->fail ()) {
        _RWSTD_MT_GUARD (this->_C_bufmutex ());

        pos_type __res = 0;

        _TRY {
            __res = this->rdbuf ()->pubseekpos (__pos, ios_base::out);
        }
        _CATCH (...) {
            this->setstate (ios_base::badbit | _RW::__rw_rethrow);
        }
        if (-1 == __res)
            this->setstate (ios_base::failbit);   // lwg issue 129
    }

    return *this;
}


template<class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>&
basic_ostream<_CharT, _Traits>::seekp (off_type __off, ios_base::seekdir __dir)
{
    _RWSTD_ASSERT (0 != this->rdbuf ());

    if (!this->fail ()) {
        _RWSTD_MT_GUARD (this->_C_bufmutex ());

        off_type __res = 0;

        _TRY {
            __res = this->rdbuf()->pubseekoff (__off, __dir, ios_base::out);
        }
        _CATCH (...) {
            this->setstate (ios_base::badbit | _RW::__rw_rethrow);
        }

        if (-1 == __res)
            this->setstate (ios_base::failbit);   // lwg issue 129
    }

    return *this;
}


template<class _CharT, class _Traits>
typename basic_ostream<_CharT, _Traits>::pos_type
basic_ostream<_CharT, _Traits>::tellp ()
{
    _RWSTD_ASSERT (0 != this->rdbuf ());

    if (!this->fail ()) {
        _RWSTD_MT_GUARD (this->_C_bufmutex ());

        _TRY {
            return this->rdbuf()->pubseekoff (0, ios_base::cur, ios_base::out);
        }
        _CATCH (...) {
            this->setstate (ios_base::badbit | _RW::__rw_rethrow);
        }
    }

    return pos_type (off_type (-1));
}


template<class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>&
basic_ostream<_CharT, _Traits>::
operator<< (basic_streambuf<char_type, traits_type> *__sb)
{ 
    if (!__sb) {
        this->setstate (ios_base::badbit);
        return *this;
    }

    ios_base::iostate __err = ios_base::goodbit;

    streamsize __nget = 0;   // number of extracted characters
    
    if (__sb->_C_mode () & ios_base::in) {

        const sentry __opfx (*this);

        _TRY {
            
            if (__opfx) {

                basic_streambuf<char_type, traits_type>* const __rdbuf =
                    this->rdbuf ();

                for (const int_type __eof = traits_type::eof (); ; ++__nget) {

                    // determine the amount of available space in our buffer
                    // into which it is safe (i.e., w/o calling overridden
                    // virtuals on the buffer object or risking exceptions)
                    // to copy the contents of the source stream buffer
                    const streamsize __navail =
                        streamsize (__rdbuf->epptr () - __rdbuf->pptr ());

                    if (__navail > 1) {
                        // copy 2 or more characters directly into buffer
                        const streamsize __n =
                            __sb->sgetn (__rdbuf->pptr (), __navail);

                        if (__n < 0) {
                            __err = ios_base::failbit;
                            break;
                        }

                        if (!__n)
                            break;

                        __rdbuf->pbump (__n);

                        __nget += __n;
                    }
                    else {
                        // if space for less than 2 characters is available
                        // try to retrieve and insert a single character
                        int_type __c = __sb->sgetc ();

                        // 27.6.2.5.3, p7, bullet 1
                        if (traits_type::eq_int_type (__eof, __c))
                            break;

                        __c = __rdbuf->sputc (traits_type::to_char_type(__c));

                        if (traits_type::eq_int_type (__eof, __c)) {
                            __err = ios_base::failbit;
                            break;
                        }

                        // 27.6.2.5.3, p7, bullet 2: extract character
                        // only if it has been successfully inserted
                        __sb->snextc ();
                    }
                }
            }
        }
        _CATCH (...) {
            this->setstate (ios_base::badbit | _RW::__rw_rethrow);
        }
    }
    // 27.6.2.5.3.8
    if (!__nget) 
        __err |= ios_base::failbit;
    
    if (__err)
        this->setstate (__err);

    return *this;
}


#ifdef _RWSTD_NO_UNDEFINED_TEMPLATES

template <class _CharT, class _Traits>
basic_ostream<_CharT, _Traits>::sentry::
sentry (const sentry &__rhs)
    : _RW::__rw_guard (0),
      _C_strm (__rhs._C_strm)
{
    _RWSTD_ASSERT (!"not callable");
}

template <class _CharT, class _Traits>
void
basic_ostream<_CharT, _Traits>::sentry::
operator= (const sentry&)
{
    _RWSTD_ASSERT (!"not callable");
}

#endif   // _RWSTD_NO_UNDEFINED_TEMPLATES

}   // namespace std
