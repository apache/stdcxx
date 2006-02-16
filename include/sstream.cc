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
        if (this->_C_bufsize < __slen)  {

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
        
        if (this->_C_is_out ()) {
            this->setp (this->_C_buffer, __bufend);
            
            if (this->_C_state & (ios_base::app | ios_base::ate))
                this->pbump (__slen);   // seek to end 
        } 
    }
}


template <class _CharT, class _Traits, class _Allocator>
_TYPENAME basic_stringbuf<_CharT, _Traits, _Allocator>::int_type
basic_stringbuf<_CharT, _Traits, _Allocator>::
underflow ()
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    if (this->gptr () < this->egptr ()) {

        _RWSTD_ASSERT (0 != this->gptr ());

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

    if (!this->_C_is_out ()) 
        return traits_type::eof ();
    
    if (this->_C_is_eof (__c))
        return  traits_type::not_eof (__c);
    
    typedef _RWSTD_STREAMSIZE _Streamsize;

    const _Streamsize __slen = _C_strlen ();
    
    // reallocate space if necessary

    if (!(this->epptr () < this->_C_buf_end ())) {

        typedef _RWSTD_ALLOC_TYPE (allocator_type, char_type) _ValueAlloc;

        // calculate size of buffer to allocate
        const _Streamsize __new_size =
            _C_grow (this->_C_bufsize + 1, this->_C_bufsize);
        
        char_type* __new_buf = _ValueAlloc ().allocate (__new_size);
           
        if (this->_C_buffer) {  // need to copy the old buffer to new buffer
            traits_type::copy (__new_buf, this->_C_buffer, __slen);   
            if (this->_C_own_buf ()) 
                _ValueAlloc ().deallocate (this->_C_buffer, this->_C_bufsize);
        }
        this->_C_own_buf (true);
        this->_C_bufsize = __new_size;
        this->_C_buffer  = __new_buf;
    }

    // increment the end put pointer by one position
    this->setp (this->_C_buffer, this->_C_buffer + __slen + 1);
    this->pbump (__slen);

    // set get area if in in|out mode 
    if (this->_C_is_inout ()) {
        // use the relative offset of gptr () from eback() to set the new gptr
        // although they are invalid, the offset is still valid
        char_type* __gptr_new =   this->_C_buffer
                                + (this->gptr () - this->eback ());  
        // N.B. pptr() has already been incremented  
        this->setg (this->_C_buffer, __gptr_new, this->epptr());
    }

    return this->sputc (traits_type::to_char_type (__c));
}


template<class _CharT, class _Traits, class _Allocator>
_TYPENAME basic_stringbuf<_CharT, _Traits, _Allocator>::int_type
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
basic_streambuf<_CharT, _Traits>*
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
 
    // set get area if in in|out mode
    if (this->_C_is_inout()) {
        // use the relative offset of gptr () from eback() to set the new gptr
        // (although the pointers are invalid, the offset is still valid)
        char_type* const __gptr_new =
            this->_C_buffer + (this->gptr () - this->eback ());

        this->setg (this->_C_buffer, __gptr_new, this->epptr());
    }
    
    return this;
}


template<class _CharT, class _Traits, class _Allocator>
_TYPENAME basic_stringbuf<_CharT, _Traits, _Allocator>::pos_type
basic_stringbuf<_CharT, _Traits, _Allocator>::
seekoff (off_type __off, ios_base::seekdir __way, ios_base::openmode __which)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    // should implicitly hold as long as ios::seekdir is an enum
    _RWSTD_ASSERT (   ios_base::beg == __way
                   || ios_base::cur == __way
                   || ios_base::end == __way);
        
    typedef _RWSTD_STREAMSIZE _Streamsize;

    _Streamsize __newoff = 0;

    if (__which & ios_base::in) {
        if (!this->_C_is_in () || !this->gptr ())
            return pos_type (off_type (-1));
        // do the checks for in|out mode here
        if (__which & ios_base::out) {
            if ((__way & ios_base::cur) || !this->_C_is_out ())
                return pos_type (off_type (-1));
        }     
        switch (__way) {

        case ios_base::beg:
            __newoff = 0;
            break;
            
        case ios_base::cur:
            __newoff = _Streamsize (this->gptr () - this->eback ());
            break;
        
        case ios_base::end:
            __newoff = _Streamsize (this->egptr () - this->eback ());
        }

        if (   (__newoff + __off) < 0
            || (this->egptr () - this->eback ()) < (__newoff + __off))
            return pos_type (off_type (-1));

        this->setg (this->eback (),
                    this->eback () + __newoff + __off,
                    this->egptr ());

    }
    
    if (__which & ios_base::out) {
        if (!this->_C_is_out () || !this->pptr ())
            return pos_type (off_type (-1));

        switch (__way) {

        case ios_base::beg:
            __newoff = 0;
            break;
            
        case ios_base::cur:
            __newoff = _Streamsize (this->pptr () - this->pbase ());
            break;
            
        case ios_base::end:
            __newoff = _Streamsize (this->epptr () - this->pbase ());
            break;
        }
        
        if (   (__newoff + __off) < 0
            || (this->epptr () - this->pbase ()) < (__newoff + __off))
            return pos_type (off_type (-1));

        this->setp (this->pbase (), this->epptr ());
        this->pbump (__newoff + __off);
    }

    return pos_type (__newoff + __off);
}


template<class _CharT, class _Traits, class _Allocator>
_TYPENAME basic_stringbuf<_CharT, _Traits, _Allocator>::pos_type
basic_stringbuf<_CharT, _Traits, _Allocator>::
seekpos (pos_type __sp, ios_base::openmode __which)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

#if !defined (__GNUG__) || __GNUG__ != 4 || __GNUC_MINOR__ != 0

    const _RWSTD_STREAMSIZE __newoff = off_type (__sp);

#else   // !gcc 4.0

    // work around gcc 4.0.0 bug #454
    _RWSTD_STREAMSIZE __newoff = off_type (__sp);

#endif   // gcc 4.0
    
    // return invalid pos if no positioning operation succeeds
    pos_type __retval = pos_type (off_type (-1));
     
    if (__newoff < 0)
        return __retval;

    // see 27.7.1.3 p.11 for required conditions 
    if ((__which & ios_base::in) && this->_C_is_in () && this->gptr()) {
        
        if ((this->eback () + __newoff) > this->egptr ())
            return pos_type (off_type (-1));
        
        this->setg (this->eback (),
                    this->eback () + __newoff,
                    this->egptr ());

        __retval = __sp;
    }
        
    if ((__which & ios_base::out) && this->_C_is_out () && this->pptr()) {

        if ((this->pbase () + __newoff) > this->epptr ()) 
            return pos_type (off_type (-1));
        
        this->setp (this->pbase (), this->epptr ());
        this->pbump (__newoff);

        __retval = __sp;
    }
    
    return __retval;
}


}   // namespace std
