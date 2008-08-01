/***************************************************************************
 *
 * sstream.cc - Declarations for the Standard Library basic strings
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
str (const char_type *__s, _RWSTD_SIZE_T __slen)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    typedef _RWSTD_ALLOC_TYPE (allocator_type, char_type) _ValueAlloc;

    typedef _RWSTD_STREAMSIZE _Streamsize;

    // compute the lenth if not specified
    if (_RWSTD_SIZE_MAX == __slen)
        __slen = traits_type::length (__s);

    _ValueAlloc __alloc;

    // new buffer and size
    char_type        *__buf;
    _RWSTD_SIZE_T     __bufsize = __slen;
    // saved offset of pptr 
    _RWSTD_STREAMSIZE __off = -1;

    if (__s == this->_C_buffer) {
        // special case: str(_C_buffer, _C_bufsize + N) called
        // to increase the capacity of buffer

        _C_catchup (this->eback ());

        // set `slen' to the number of initialized characters
        // in the buffer
        __slen = this->egptr () - this->pbase ();
        // save the offset of pptr
        __off = this->pptr () - this->pbase ();
    }

    if (this->_C_bufsize < __bufsize) {
        // requested capacity is greater than the current capacity
        // allocate a new buffer of sufficient size
        const _RWSTD_STREAMSIZE __ratio =
            _RWSTD_STREAMSIZE (  (_RWSTD_NEW_CAPACITY_RATIO << 10)
                               / _RWSTD_RATIO_DIVIDER);

        const _RWSTD_SIZE_T __cap =
            this->_C_bufsize ?
              (this->_C_bufsize >> 10) * __ratio
            + (((this->_C_bufsize & 0x3ff) * __ratio) >> 10)
            : _RWSTD_MINIMUM_STRINGBUF_CAPACITY;

        __bufsize = __cap < __bufsize ? __bufsize : __cap;

        if (__s != this->_C_buffer && this->_C_own_buf ()) {
            // deallocate the existing buffer here only if the string
            // is not the same as the buffer itself; otherwise, copy
            // it to the newly allocated buffer first and deallocate
            // it later
            __alloc.deallocate (this->_C_buffer, this->_C_bufsize);
            this->_C_buffer = 0;
        }

        __buf = __alloc.allocate (__bufsize);
    }
    else if (0 < __bufsize) {
        // requested capacity is the same or less than the current one
        __buf     = this->_C_buffer;
        __bufsize = this->_C_bufsize;
    }
    else {
        // zero size and capacity, deallocate and reset all pointers
        __buf     = 0;
        __bufsize = 0;

        _RWSTD_ASSERT (0 == __slen);
    }

    // compute the "high mark" (see lwg issue 432)
    char_type* const __egptr = __buf + __slen;

    if (__s != __buf) {
        // copy the provided string to buffer
        traits_type::copy (__buf, __s, __slen);

        if (this->_C_buffer != __buf) {
            if (this->_C_buffer && this->_C_own_buf ())
                __alloc.deallocate (this->_C_buffer, this->_C_bufsize);

            // take ownership of the allocated buffer
            this->_C_own_buf (true);

            this->_C_buffer  = __buf;
            this->_C_bufsize = __bufsize;
        }
    }

    if (this->_C_is_in ())
        this->setg (this->_C_buffer, this->_C_buffer, __egptr);
    else {
        // when not in in mode set all get pointers to the same
        // value and use egptr() as the "high mark" (see lwg
        // issue 432)
        this->setg (__egptr, __egptr, __egptr);
    }

    if (this->_C_is_out ()) {
        this->setp (this->_C_buffer, this->_C_buffer + this->_C_bufsize);

        if (0 <= __off) {
            // restore the pptr
            this->pbump (__off);
        }
        else if (   this->_C_state & ios_base::in
                 || this->_C_state & (ios_base::app | ios_base::ate)) {
            // in input or append/ate modes seek to end
            // (see also lwg issue 562 for clarification)
            this->pbump (__slen);
        }
    }

    _RWSTD_ASSERT (this->_C_is_valid ());
}


template <class _CharT, class _Traits, class _Allocator>
streamsize
basic_stringbuf<_CharT, _Traits, _Allocator>::
xsputn (const char_type* __s, streamsize __n)
{
    _RWSTD_ASSERT (0 != __s || 0 == __n);
    _RWSTD_ASSERT (this->_C_is_valid ());

    if (__n <= 0 || !this->_C_is_out ())
        return 0;

    if (this->epptr () - this->pptr () < __n) {

        // compute the total amount of space necessary
        const _RWSTD_SIZE_T __bufsize =
            __n + (this->pptr () - this->pbase ());

        _RWSTD_PTRDIFF_T __off = -1;

        if (this->pbase () <= __s && this->pptr () > __s) {
            // __s is a part of the buffer
            _RWSTD_ASSERT (this->epptr () >= __s + __n);
            // save the offset from pbase()
            __off = this->pbase () - __s;
        }

        // grow the buffer if necessary to accommodate the whole
        // string plus the contents of the buffer up to pptr()
        str (this->_C_buffer, __bufsize);

        _RWSTD_ASSERT (__n <= this->epptr () - this->pptr ());

        if (0 <= __off) {
            // correct __s after the buffer reallocation
            __s = this->pbase () + __off;
        }
    }

    // copy the whole string
    traits_type::copy (this->pptr (), __s, __n);

    this->pbump (__n);

    _C_catchup (this->eback ());

    return __n;
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
/* virtual */ typename basic_stringbuf<_CharT, _Traits, _Allocator>::int_type
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
typename basic_stringbuf<_CharT, _Traits, _Allocator>::int_type
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

    if (this->pptr () == this->epptr ()) {
        // reallocate buffer
        str (this->_C_buffer, this->_C_bufsize + 1);
    }
    else if (this->epptr () < __bufend) {
        // bump up epptr() keeping pbase() and pptr() unchanged

        const _RWSTD_STREAMSIZE __off = this->pptr () - this->pbase ();
        this->setp (this->pbase (), __bufend);
        this->pbump (__off);
    }

    const int_type __retval = this->sputc (traits_type::to_char_type (__c));

    // get egptr() caught up with the value of pptr() after the call
    _C_catchup (this->_C_buffer);

    return __retval;
}


template<class _CharT, class _Traits, class _Allocator>
/* virtual */ typename basic_stringbuf<_CharT, _Traits, _Allocator>::int_type
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

    const _RWSTD_STREAMSIZE __slen = (this->egptr () < this->pptr () ?
        this->pptr () : this->egptr ()) - this->pbase ();

    if (__n < __slen || !this->_C_is_out())
        return 0;   // failure

    // compute the gptr and pptr offsets so the pointers can be restored
    const _RWSTD_STREAMSIZE __goff = this->gptr () - this->eback ();
    const _RWSTD_STREAMSIZE __poff = this->pptr () - this->pbase ();

    const bool __own_old_buf = this->_C_own_buf ();

    typedef _RWSTD_ALLOC_TYPE (allocator_type, char_type) _ValueAlloc;

    if (0 == __buf) {
        // allocate a new buffer of the specified size
        __buf = _ValueAlloc ().allocate (__n);
        this->_C_own_buf (true);
    }
    else
        this->_C_own_buf (false);

    // copy the contents of the existing buffer to the new one
    traits_type::copy (__buf, this->_C_buffer, __slen);

    if (__own_old_buf)
        _ValueAlloc ().deallocate (this->_C_buffer, this->_C_bufsize);

    this->_C_buffer  = __buf;
    this->_C_bufsize = __n;

    // reset the output and input sequences within the new buffer
    this->setp (this->_C_buffer, this->_C_buffer + this->_C_bufsize);
    this->pbump (__poff);   // ... and restore it

    char_type* const __egptr = this->_C_buffer + __slen;

    if (this->_C_is_in ())
        this->setg (this->_C_buffer, this->_C_buffer + __goff, __egptr);
    else {
        // use egptr as the "high mark" (see lwg issue 432)
        this->setg (__egptr, __egptr, __egptr);
    }

    return this;
}


template<class _CharT, class _Traits, class _Allocator>
/* virtual */ typename basic_stringbuf<_CharT, _Traits, _Allocator>::pos_type
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

        // LWG issue 453: the operation fails if either gptr() or pptr()
        // is a null pointer and the new offset newoff is nonzero
        if (!this->_C_is_in () || __off && !this->gptr ())
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

        if (__newoff < 0 || this->egptr () - this->eback () < __newoff)
            return pos_type (off_type (-1));

        this->setg (this->eback (), this->eback () + __newoff, this->egptr ());
    }

    if (__which & ios_base::out) {

        // LWG issue 453: the operation fails if either gptr() or pptr()
        // is a null pointer and the new offset newoff is nonzero
        if (!this->_C_is_out () || __off && !this->pptr ())
            return pos_type (off_type (-1));

        // egptr() is used as the "high mark" even when not in "in" mode
        _RWSTD_ASSERT (0 == this->pbase () || 0 != this->egptr ());

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
/* virtual */ typename basic_stringbuf<_CharT, _Traits, _Allocator>::pos_type
basic_stringbuf<_CharT, _Traits, _Allocator>::
seekpos (pos_type __pos, ios_base::openmode __which)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    // typedef helps HP aCC 3.27
    typedef basic_stringbuf _StringBuf;

    return pos_type (_StringBuf::seekoff (__pos, ios_base::beg, __which));
}


}   // namespace std
