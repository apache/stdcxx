/***************************************************************************
 *
 * sstream.cc - Declarations for the Standard Library basic strings
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


_RWSTD_NAMESPACE (std) { 


template<class _CharT, class _Traits, class _Allocator>
basic_stringbuf<_CharT, _Traits, _Allocator>::
basic_stringbuf (const _C_string_type& __str, ios_base::openmode __mode)
    : basic_streambuf<_CharT, _Traits>(__mode)
{
    str (__str);
}


// extension
template<class _CharT, class _Traits, class _Allocator>
basic_stringbuf<_CharT, _Traits, _Allocator>::
basic_stringbuf (const char_type *__s, ios_base::openmode __mode)
    : basic_streambuf<_CharT, _Traits>(__mode)
{
    _RWSTD_ASSERT (0 != __s);

    str (__s, traits_type::length (__s));
}


template <class _CharT, class _Traits, class _Allocator>
/* virtual */
basic_stringbuf<_CharT, _Traits, _Allocator>::
~basic_stringbuf ()
{
    typedef _RWSTD_ALLOC_TYPE (allocator_type, char_type) _ValueAlloc;

    if (this->_C_own_buf ())
        _ValueAlloc ().deallocate (this->_C_buffer, this->_C_bufsize);
}


// extension
template<class _CharT, class _Traits, class _Allocator>
void
basic_stringbuf<_CharT, _Traits, _Allocator>::
str (const char_type *__s, _RWSTD_SIZE_T __slen /* = -1 */)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    typedef _RWSTD_ALLOC_TYPE (allocator_type, char_type) _ValueAlloc;

    typedef _RWSTD_STREAMSIZE _Streamsize;

    if (_RWSTD_SIZE_MAX == __slen)
        __slen = traits_type::length (__s);

    if (0 == __slen) {

        if (this->_C_own_buf ())
            _ValueAlloc ().deallocate (this->_C_buffer, this->_C_bufsize);

        this->setg (0, 0, 0);
        this->setp (0, 0);

        this->_C_buffer  = 0;
        this->_C_bufsize = 0;
    }
    else {
        if (this->_C_bufsize < _Streamsize (__slen))  {

            // buffer too small - need to reallocate
            if (this->_C_own_buf ())
                _ValueAlloc ().deallocate (this->_C_buffer, this->_C_bufsize);

            this->_C_bufsize = _C_grow (this->_C_bufsize, __slen);

            this->_C_buffer = _ValueAlloc ().allocate (this->_C_bufsize);
            this->_C_own_buf (true);
        }

        traits_type::copy (this->_C_buffer, __s, __slen);

        char_type* const __bufend = this->_C_buffer + __slen;
        
        if (this->_C_is_in ())
            this->setg (this->_C_buffer, this->_C_buffer, __bufend);
        else {
            // when not in in mode set all get pointers to the same
            // value and use egptr() as the "high mark" (see LWG
            // issue 432)
            this->setg (__bufend, __bufend, __bufend);
        }
        
        if (this->_C_is_out ()) {
            this->setp (this->_C_buffer, this->_C_buffer + this->_C_bufsize);
            
            if (this->_C_state & (ios_base::app | ios_base::ate))
                this->pbump (__slen);   // seek to end 
        } 
    }
}


template <class _CharT, class _Traits, class _Allocator>
/* virtual */ streamsize
basic_stringbuf<_CharT, _Traits, _Allocator>::
showmanyc ()
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    // get egptr() caught up with pptr()
    _C_catchup (this->eback ());

    return streamsize (this->egptr () - this->gptr ());
}


template <class _CharT, class _Traits, class _Allocator>
/* virtual */ _TYPENAME basic_stringbuf<_CharT, _Traits, _Allocator>::int_type
basic_stringbuf<_CharT, _Traits, _Allocator>::
underflow ()
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    if (this->gptr () < this->egptr ()) {

        _RWSTD_ASSERT (0 != this->gptr ());

        return traits_type::to_int_type (*this->gptr ());
    }
    else if (this->gptr () < this->pptr ()) {

        // get egptr() caught up with pptr()
        _C_catchup (this->eback ());

        if (this->gptr () < this->egptr ())
            return traits_type::to_int_type (*this->gptr ());
    }

    return traits_type::eof ();
}


template<class _CharT, class _Traits, class _Allocator>
_TYPENAME basic_stringbuf<_CharT, _Traits, _Allocator>::int_type
basic_stringbuf<_CharT, _Traits, _Allocator>::
overflow (int_type __c)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    // 27.7.1.3, p5, bullet 2 of C++ '03: when (c == eof)
    // indicate success even when not in out mode
    if (this->_C_is_eof (__c))
        return traits_type::not_eof (__c);
    
    if (!this->_C_is_out ()) 
        return traits_type::eof ();
    
    char_type* const __bufend = this->_C_buffer + this->_C_bufsize;
    
    if (this->epptr () < __bufend) {
        // bump up epptr() keeping pbase() and pptr() unchanged

        const _RWSTD_STREAMSIZE __off = this->pptr () - this->pbase ();
        this->setp (this->pbase (), __bufend);
        this->pbump (__off);
    }
    else if (this->pptr () == this->epptr ()) {
        // allocate new or reallocate existing buffer

        typedef _RWSTD_ALLOC_TYPE (allocator_type, char_type) _ValueAlloc;

        // calculate size of the new buffer to allocate
        const _RWSTD_STREAMSIZE __new_size =
            _C_grow (this->_C_bufsize + 1, this->_C_bufsize);
        
        char_type* const __new_buf = _ValueAlloc ().allocate (__new_size);
           
        // compute the length of the output sequence
        const _RWSTD_STREAMSIZE __slen = this->pptr () - this->pbase ();

        if (this->_C_buffer) {
            // copy the contents of the old buffer to the new one
            traits_type::copy (__new_buf, this->_C_buffer, __slen);

            // deallocate the old buffer if owned
            if (this->_C_own_buf ()) 
                _ValueAlloc ().deallocate (this->_C_buffer, this->_C_bufsize);
        }

        this->_C_own_buf (true);
        this->_C_bufsize = __new_size;
        this->_C_buffer  = __new_buf;

        // set the put area
        this->setp (this->_C_buffer, this->_C_buffer + this->_C_bufsize);
        this->pbump (__slen);
    }

    const int_type __retval = this->sputc (traits_type::to_char_type (__c));

    // get egptr() caught up with the value of pptr() after the call
    _C_catchup (this->_C_buffer);

    return __retval;
}


template<class _CharT, class _Traits, class _Allocator>
/* virtual */ _TYPENAME basic_stringbuf<_CharT, _Traits, _Allocator>::int_type
basic_stringbuf<_CharT, _Traits, _Allocator>::
pbackfail (int_type __c)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    if (!this->_C_putback_avail ())
        return traits_type::eof ();

    int_type __retval;

    const char_type __ch = traits_type::to_char_type (__c);
    
    if (traits_type::eq (__ch, *(this->gptr () - 1)) || this->_C_is_eof (__c)) {
        // "put back" original value
        this->gbump (-1);
        __retval = traits_type::not_eof (__c);
    }
    else if (this->_C_is_out ()) {
        // overwrite existing value with new value
        this->gbump (-1);
        traits_type::assign (*this->gptr (), __ch);
        __retval = __c;
    }
    else
        __retval = traits_type::eof ();
    
    return __retval;
}


template<class _CharT, class _Traits, class _Allocator>
/* virtual */ basic_streambuf<_CharT, _Traits>*
basic_stringbuf<_CharT, _Traits, _Allocator>::
setbuf (char_type* __buf, _RWSTD_STREAMSIZE __n)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    if (!__buf && !__n)   // 27.7.1.3, p16
        return this;

    if (__n < _C_strlen() || !this->_C_is_out())  
        return 0;   // failure

    bool __own_old_buf = this->_C_own_buf ();

    typedef _RWSTD_STREAMSIZE _Streamsize;

    const _Streamsize __slen = _C_strlen ();
    
    typedef _RWSTD_ALLOC_TYPE (allocator_type, char_type) _ValueAlloc;

    if (0 == __buf) {
        __buf = _ValueAlloc ().allocate (__n);
        this->_C_own_buf (true);
    }
    else
        this->_C_own_buf (false);
        
    traits_type::copy (__buf, this->_C_buffer, __slen);   

    if (__own_old_buf)
        _ValueAlloc ().deallocate (this->_C_buffer, this->_C_bufsize);
    
    this->_C_buffer  = __buf;
    this->_C_bufsize = __n;

    const _Streamsize __pptr_off = _Streamsize(this->pptr () - this->pbase ());
    this->setp (this->_C_buffer, this->_C_buffer + __slen);
    this->pbump (__pptr_off);   // ... and restore it
 
    // get egptr() caught up with pptr()
    _C_catchup (this->_C_buffer);
    
    return this;
}


template<class _CharT, class _Traits, class _Allocator>
/* virtual */ _TYPENAME basic_stringbuf<_CharT, _Traits, _Allocator>::pos_type
basic_stringbuf<_CharT, _Traits, _Allocator>::
seekoff (off_type __off, ios_base::seekdir __way, ios_base::openmode __which)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    // should implicitly hold as long as ios::seekdir is an enum
    _RWSTD_ASSERT (   ios_base::beg == __way
                   || ios_base::cur == __way
                   || ios_base::end == __way);
        
    _RWSTD_STREAMSIZE __newoff = -1;

    // get egptr() caught up with pptr()
    _C_catchup (this->eback ());

    if (__which & ios_base::in) {

        if (!this->_C_is_in () || !this->gptr ())
            return pos_type (off_type (-1));

        // do the checks for in|out mode here
        if (   __which & ios_base::out
            && (ios_base::cur == __way || !this->_C_is_out ()))
            return pos_type (off_type (-1));

        switch (__way) {
        case ios_base::beg: __newoff = 0; break;
        case ios_base::cur: __newoff = this->gptr () - this->eback (); break;
        case ios_base::end: __newoff = this->egptr () - this->eback (); break;
        }

        __newoff += __off;

        if ( __newoff < 0 || (this->egptr () - this->eback ()) < __newoff)
            return pos_type (off_type (-1));

        this->setg (this->eback (), this->eback () + __newoff, this->egptr ());
    }
    
    if (__which & ios_base::out) {

        if (!this->_C_is_out () || !this->pptr ())
            return pos_type (off_type (-1));

        // egptr() is used as the "high mark" even when not in "in" mode
        _RWSTD_ASSERT (0 != this->egptr ());

        // compute the number of initialized characters in the buffer
        // (see LWG issue 432)
        const _RWSTD_STREAMSIZE __high = this->egptr () - this->pbase ();
        const _RWSTD_STREAMSIZE __cur  = this->pptr () - this->pbase ();

        switch (__way) {
        case ios_base::beg: __newoff = 0; break;
        case ios_base::cur: __newoff = __cur; break;
        case ios_base::end: __newoff = __high; break;
        }

        __newoff += __off;

        if (__newoff < 0 || __high < __newoff)
            return pos_type (off_type (-1));

        // bump pptr up (or down) to the new position
        this->pbump (__newoff - __cur);
    }

    _RWSTD_ASSERT (this->_C_is_valid ());

    return __newoff < 0 ? pos_type (off_type (-1)) : pos_type (__newoff);
}


template<class _CharT, class _Traits, class _Allocator>
/* virtual */ _TYPENAME basic_stringbuf<_CharT, _Traits, _Allocator>::pos_type
basic_stringbuf<_CharT, _Traits, _Allocator>::
seekpos (pos_type __pos, ios_base::openmode __which)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    return pos_type (basic_stringbuf::seekoff (__pos, ios_base::beg, __which));
}


}   // namespace std
