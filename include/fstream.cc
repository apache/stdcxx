// -*- C++ -*-
/***************************************************************************
 *
 * fstream.cc - Definition for the Standard Library file streams
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
 * Copyright 1997-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

_RWSTD_NAMESPACE (std) {


template<class _CharT, class _Traits>
basic_filebuf<_CharT, _Traits>*
basic_filebuf<_CharT, _Traits>::
open (const char *__name, ios_base::openmode __mode, long __prot)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    // fail if `mode' has invalid bits set or if the file is already open
    if ((__mode & ~_RWSTD_IOS_OPENMODE_MASK) || is_open ())
        return 0;

    _C_file = _RW::__rw_fopen (__name, __mode, __prot);

    if (!_C_file)
        return 0;

    pos_type __pos = pos_type ();

    if (__mode & ios_base::ate) {

        // the end of a file is assumed to be in the initial shift state
        // this assumption is safe as long as the file has been properly
        // closed (or unshifted) after the last write operation on it
        __pos = _RW::__rw_fseek (_C_file, 0, 0, ios_base::end);

        if (-1L == __pos) {
            _RW::__rw_fclose (_C_file, 0);
            _C_file = 0;
            return 0;
        }
    }

    _C_beg_pos = _C_cur_pos = __pos;

    this->_C_state &= ~_RWSTD_IOS_OPENMODE_MASK;
    this->_C_state |= __mode;

    return this;
}


template<class _CharT, class _Traits>
basic_filebuf<_CharT, _Traits>*
basic_filebuf<_CharT, _Traits>::
close (bool __close_file /* = true */)
{
    // close_file is false when close() is called from detach()

    _RWSTD_ASSERT (this->_C_is_valid ());

    if (!is_open ())
        return 0;   // failure

    // close() returns this on success, 0 on failure
    basic_filebuf *__retval = this;

    _TRY {
        // avoid expensive call to overflow() unless necessary
        if (this->pptr () != this->pbase () && this->_C_is_eof (overflow ()))
            __retval = 0;   // failure

        // write out any unshift sequence if necessary
        // (applies to multibyte, state dependent encodings only)
        if (__retval && this->_C_out_last () && !_C_unshift ())
            __retval = 0;   // failure
    }
    _CATCH (...) {
        // either overflow() or codecvt::unshift() threw

        if (__close_file) {
            _RW::__rw_fclose (_C_file, this->_C_state);

            // zero out the file pointer except when detaching fd
            _C_file    = 0;
            _C_cur_pos = _C_beg_pos = pos_type (off_type (-1));

            // reset input/output sequences to prevent any
            // subsequent I/O attempts on closed file
            this->setg (0, 0, 0);
            this->setp (0, 0);
        }

        // rethrow the caught exception
        _RETHROW;
    }

    if (__close_file) {
        if (_RW::__rw_fclose (_C_file, this->_C_state))
            __retval = 0;

        // zero out the file pointer except when detaching fd
        _C_file    = 0;
        _C_cur_pos = _C_beg_pos = pos_type (off_type (-1));

        // reset input/output sequences to prevent any
        // subsequent I/O attempts on closed file
        this->setg (0, 0, 0);
        this->setp (0, 0);
    }

    return __retval;
}


template<class _CharT, class _Traits>
streamsize
basic_filebuf<_CharT, _Traits>::
showmanyc ()
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    if (   !this->_C_is_in () || !is_open ()
        || _C_cur_pos == pos_type (off_type (-1)))
        return -1;

    // start with the number of chars in get area
    _RWSTD_STREAMSIZE __retval = this->egptr () - this->gptr ();

    // no prob if this fails for non-seekable devices
    const pos_type __end_pos =
        _RW::__rw_fseek (_C_file, this->_C_state, 0, ios_base::end);

    if (__end_pos != pos_type (off_type (-1))) {

        // restore position within file only if seek succeeded
        _RW::__rw_fseek (_C_file, this->_C_state, _C_cur_pos, ios_base::beg);

        typedef typename traits_type::state_type _StateT;
        typedef codecvt<char_type, char, _StateT> _Codecvt;

        const _Codecvt &__cvt = _USE_FACET (_Codecvt, this->getloc ());

        if (__cvt.always_noconv ())
            __retval += __end_pos - _C_cur_pos;
        else
            // make most pessimistic conversion estimate
            __retval += (__end_pos - _C_cur_pos) / __cvt.max_length ();
    }

    return __retval > 0 ? __retval : 0;
}


template<class _CharT, class _Traits>
typename basic_filebuf<_CharT, _Traits>::int_type
basic_filebuf<_CharT, _Traits>::
underflow ()
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    this->setp (0, 0);          // invalidate put area

    this->_C_out_last (false);  // needed by close ()

    if (!this->_C_is_in () || !is_open())
        return traits_type::eof ();

    char_type* const __to_end = this->_C_buffer + this->_C_bufsize;

    typedef typename traits_type::int_type _IntType;

    _IntType __ret = _IntType ();

    // fill the buffer if it's empty

    if (this->gptr () == this->egptr()) {  // N.B.: gptr() could be null here

        // determine the maximum possible size of putback area (if any)
        // make sure putback area isn't too big - try to honor
        // _RWSTD_PBACK_SIZE if possible, otherwise shrink

        const _RWSTD_SIZE_T __pbackavail = this->_C_putback_avail ();
        _C_pbacksize = __pbackavail < _RWSTD_PBACK_SIZE ?
            __pbackavail : _RWSTD_PBACK_SIZE;

        _RWSTD_ASSERT (0 != this->_C_bufsize);

        if (_C_pbacksize == this->_C_bufsize)
            _C_pbacksize = this->_C_bufsize - 1;

        traits_type::move (this->eback(), this->gptr () - _C_pbacksize,
                           _C_pbacksize);

        //  fill the get area from the file, performing code conversion if
        //  necessary

        _RWSTD_STREAMSIZE __nread = 0; // number of bytes read from file

        _C_beg_pos = _C_cur_pos;

        typedef typename traits_type::state_type _StateT;
        typedef codecvt<char_type, char, _StateT> _Codecvt;

        const _Codecvt &__cvt = _USE_FACET (_Codecvt, this->getloc ());

        if (__cvt.always_noconv ()) {
            // no conversion required
            __nread = (__to_end - this->_C_buffer) - _C_pbacksize;
            __nread = _RW::__rw_fread (_C_file, this->_C_state,
                                       this->_C_buffer + _C_pbacksize,
                                       sizeof (char_type) * __nread);
            if (__nread < 0)
                return traits_type::eof ();       // error while reading

            this->setg (this->_C_buffer,
                        this->_C_buffer + _C_pbacksize,
                        this->_C_buffer + _C_pbacksize + __nread);

            // adjust the current position in the file,
            // taking into account CR/LF conversion on windows
            __nread += _C_crlf_intern_count (this->gptr (),
                                             this->gptr () + __nread);
        }
        else { // conversion required

            char          __xbuf [_RWSTD_DEFAULT_BUFSIZE];
            char*         __from_base = __xbuf;
            const char*   __from_next = 0;
            char_type*    __to_base   = this->_C_buffer + _C_pbacksize;
            char_type*    __to_next   = __to_base;

            _StateT       __state     = _C_cur_pos.state ();

            codecvt_base::result __res = codecvt_base::ok;

            const ctype<char_type> &__ctp =
                _USE_FACET (ctype<char_type>, this->getloc ());

            while (__to_next != __to_end && codecvt_base::error != __res) {

                // read only as many characters as we have positions left in
                //   internal buffer - guarantees we won't read more characters
                //   than we can put into the internal buffer after conversion
                //   and ending file position isn't in the middle of a shift
                //   sequence
                // N.B.: area between __xbuf and __from_base contains partially
                //   converted sequences left from previous read
                _RWSTD_STREAMSIZE __n = __to_end - __to_next;
                if (_RWSTD_DEFAULT_BUFSIZE - (__from_base - __xbuf) < __n)
                    __n = _RWSTD_DEFAULT_BUFSIZE - (__from_base - __xbuf);

                __n = _RW::__rw_fread (_C_file, this->_C_state,
                                       __from_base, __n);

                if (0 > __n)
                    return traits_type::eof ();   // error while reading

                if (0 == __n)
                    break;   // reached eof

                // take into account CR/LF conversion on Win32
                __nread += __n +
                    _C_crlf_extern_count (__from_base, __from_base + __n);

                // adjust 'n' to hold the number of external chars in buffer
                __n += __from_base - __xbuf;

                // convert any partially converted sequence from the previous
                // iteration (possibly empty) plus what we just read in
                __res = __cvt.in (__state, __xbuf, __xbuf + __n, __from_next,
                                  __to_base, __to_end, __to_next);

                switch (__res) {
                case codecvt_base::ok:
                    // there may be yet unconverted elements at the end
                    // of the source sequence, fall through and treat
                    // as partial (`n' below may be 0)

                case codecvt_base::partial:
                    // compute the length of partially converted sequence
                    __n -= __from_next - __xbuf;

                    typedef char_traits<char> CharTraits;

                    // copy the sequence to beginning of xbuf
                    CharTraits::move (__xbuf, __from_next, __n);

                    // will append external chars to end of the sequence
                    __from_base = __xbuf + __n;
                    break;

                case codecvt_base::noconv:
                    // note that according to lwg issue 19, codecvt<wchar_t,
                    // char>::in() may not return noconv since internT and
                    // externT are not the same type

                    // since codecvt<char, char>::always_noconv() is required
                    // to return true, this branch only executes for a user-
                    // defined codecvt<T, T> facet with internT and externT
                    // being the same type

                    // FIXME: do not widen external buffer just memcpy it
                    //        to internal buffer (externT == internT)
                    __ctp.widen (__xbuf, __xbuf + __n, __to_base);
                    __to_next = __to_base + __n;
                    break;

                case codecvt_base::error:
                    // failed to convert part of the buffer
                    // retain the part that has been successfully
                    // converted, and disregard the rest
                    __ret = traits_type::eof ();
                    break;

                default:
                    // bad return value from codecvt
                    return traits_type::eof ();
                }
                __to_base = __to_next;  // continue at end of converted seq
            }

            _C_cur_pos.state (__state);

            this->setg (this->_C_buffer, this->_C_buffer + _C_pbacksize,
                        __to_next);
        }   // end conversion block

        if (__nread == 0)
            return traits_type::eof ();

        _C_cur_pos += __nread;
    }

    return traits_type::eq_int_type (__ret, _IntType ()) ?
        traits_type::to_int_type (*this->gptr ()) : __ret;
}


template<class _CharT, class _Traits>
typename basic_filebuf<_CharT, _Traits>::int_type
basic_filebuf<_CharT, _Traits>::
overflow (int_type __c /* = eof () */)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    if (!this->_C_is_out () || !is_open ())
        return traits_type::eof ();

    this->setg (0, 0, 0);            // invalidate the get area

    const bool __unbuf = this->_C_is_unbuffered ();

    const char_type __c_to_char = traits_type::to_char_type (__c);

    if (this->pptr () == 0 && !__unbuf) {
        // put area not valid yet - just need to initialize it
        this->setp (this->_C_buffer, this->_C_buf_end ());
    }
    else if (   this->pptr () == this->epptr ()
             || this->_C_is_eof (__c)
             || __unbuf) {

        const char_type*  __buf;
        _RWSTD_STREAMSIZE __nchars;

        if (__unbuf) {
            if (this->_C_is_eof (__c)){
                __buf    = 0;
                __nchars = 0;
            }
            else {
                __buf    = &__c_to_char;
                __nchars = 1;
            }
        }
        else {
            // call xsputn() with a special value to have it flush
            // the controlled sequence to the file
            __buf    = _RWSTD_REINTERPRET_CAST (char_type*, this);
            __nchars = this->pptr () - this->pbase ();
        }

        // typedef helps HP aCC 3.27
        typedef basic_filebuf _FileBuf;

        if (__nchars != _FileBuf::xsputn (__buf, __nchars))
            return traits_type::eof ();  // error while writing
    }

    // now that there's room in the buffer, call sputc() recursively
    // to actually place the character in the buffer (unless we're
    // in unbuffered mode because we just wrote it out)
    if (!this->_C_is_eof (__c) && !__unbuf)
        this->sputc (__c_to_char);

    this->_C_out_last (true);   // needed by close ()

    return traits_type::not_eof (__c);
}


template <class _CharT, class _Traits>
_RWSTD_STREAMSIZE
basic_filebuf<_CharT, _Traits>::
xsputn (const char_type* __buf, _RWSTD_STREAMSIZE __nchars)
{
    _RWSTD_ASSERT (0 != __buf || 0 == __nchars);
    _RWSTD_ASSERT (this->_C_is_valid ());

    if (0 == __nchars)
        return 0;   // not an error

    if (__nchars < 0 || !this->_C_is_out () || !is_open ())
        return -1;   // error

    if (0 == this->pptr () && !this->_C_is_unbuffered ())
        // put area not valid yet - just need to initialize it
        this->setp (this->_C_buffer, this->_C_buf_end ());

    const _RWSTD_STREAMSIZE __navail = this->epptr () - this->pptr ();

    const char_type* const __special =
        _RWSTD_REINTERPRET_CAST (char_type*, this);

    if (__buf == __special) {
        __buf = this->pbase ();
    }
    else if (__nchars <= __navail) {
        // the amount of available space is big enough

        // append the contents of the buffer to the controlled sequence
        traits_type::copy (this->pptr (), __buf, __nchars);

        this->pbump (__nchars);

        return __nchars;
    }
    else {
        // call self recursively to flush the controlled sequence first
        const _RWSTD_STREAMSIZE __nwrite = this->pptr () - this->pbase ();

        // typedef helps HP aCC 3.27
        typedef basic_filebuf _FileBuf;

        // return -1 on error to flush the controlled sequence
        if (__nwrite != _FileBuf::xsputn (__special, __nwrite))
            return -1;
    }

    //  flush buffer to file, performing code conversion if necessary

    _RWSTD_ASSERT (this->_C_is_valid ());
    _RWSTD_ASSERT (this->_C_is_out ());
    _RWSTD_ASSERT (is_open ());

    const char_type* const __end = __buf + __nchars;

    typedef typename traits_type::state_type _StateT;

    _RWSTD_STREAMSIZE __nwrote = 0;          // num chars to write
    _StateT __state = _C_cur_pos.state ();   // state of stream

    _C_beg_pos = _C_cur_pos;

    typedef codecvt<char_type, char, _StateT> _Codecvt;

    const _Codecvt &__cvt = _USE_FACET (_Codecvt, this->getloc ());

    if (__cvt.always_noconv ()) {

        // no conversion

        __nwrote = __end - __buf;

        const _RWSTD_STREAMSIZE __nbytes = sizeof (char_type) * __nwrote;

        if (__nbytes != _RW::__rw_fwrite (_C_file, this->_C_state,
                                          __buf, __nbytes))
            return -1;  // error while writing
    }
    else {

        // perform codeset conversion in chunks to avoid dynamic
        // memory allocation

        char             __xbuf [_RWSTD_DEFAULT_BUFSIZE];
        char*            __xbuf_end  = __xbuf + sizeof __xbuf;
        char*            __to_next   = 0;
        const char_type* __from_next = 0;

        for (const char_type* __base = __buf; __from_next != __end;
             __base = __from_next) {

            // avoid using const codecvt_base::result here
            // to prevent HP aCC 3.27 errors
            const int __res =
                __cvt.out (__state, __base, __end, __from_next,
                           __xbuf, __xbuf_end, __to_next);

            _RWSTD_STREAMSIZE __nbytes =
                sizeof (char_type) * (__end - __base);

            switch (__res) {
            case codecvt_base::error:
                // write out the sequence successfully converted up
                // to the point of the error in the internal sequence
                // and fail
                _RW::__rw_fwrite (_C_file, this->_C_state, __base, __nbytes);
                    return traits_type::eof ();

            case codecvt_base::noconv:
                // write the entire sequence
                if (__nbytes != _RW::__rw_fwrite (_C_file, this->_C_state,
                                                  __base, __nbytes))
                    return traits_type::eof ();

                __nwrote += __end - __base
                    + _C_crlf_intern_count (__base, __end);

                __from_next = __end; // effectively 'break'
                break;

            default:
                _RWSTD_ASSERT (   codecvt_base::ok == __res
                               || codecvt_base::partial == __res);

                // partial conversion will result if there isn't enough
                // space in the conversion buffer to hold the converted
                // sequence, but we're O.K. since we'll be passing any
                // remaining unconverted characters (starting at
                // __from_next) in the next iteration

                __nbytes = __to_next - __xbuf;

                if (__nbytes != _RW::__rw_fwrite (_C_file, this->_C_state,
                                                  __xbuf, __nbytes))
                    return -1;

                __nwrote += __nbytes
                    + _C_crlf_extern_count (__xbuf, __to_next);
            }
        }
    }

    // adjust the current position in the file
    _C_cur_pos += __nwrote;
    _C_cur_pos.state (__state);

    // reset the put area
    if (!this->_C_is_unbuffered ())
        this->setp (this->_C_buffer, this->_C_buf_end ());

    this->_C_out_last (true);   // needed by close ()

    // return the number of characters (not bytes) in the buffer
    // successfully written to the file
    return __nchars;
}


template<class _CharT, class _Traits>
typename basic_filebuf<_CharT, _Traits>::int_type
basic_filebuf<_CharT, _Traits>::
pbackfail (int_type __c)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    if (!is_open ())
        return traits_type::eof ();

    // we could get here if gptr = eback or if __c != *(gptr-1)
    if (!this->_C_putback_avail ()) {
        // try to make a putback area available

        if (this->seekoff (-1, ios_base::cur) == pos_type (off_type (-1)))
            return traits_type::eof ();

        if (this->_C_is_eof (underflow ()))
            return traits_type::eof ();

        this->gbump (1);
    }

    if (traits_type::eq (traits_type::to_char_type (__c), *(this->gptr () - 1))
        || this->_C_is_eof (__c)) {
        // "put back" original value
        this->gbump (-1);
        return traits_type::not_eof (__c);
    }
    else if (this->_C_is_out ()) {
        // overwrite existing value with new value
        this->gbump (-1);
        *this->gptr () = traits_type::to_char_type (__c);
        return __c;
    }
    else
        return traits_type::eof ();
}


template<class _CharT, class _Traits>
basic_streambuf<_CharT, _Traits>*
basic_filebuf<_CharT, _Traits>::
setbuf (char_type *__buf, streamsize __ssize)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    if (__ssize < 0)
        return 0;

    const _RWSTD_SIZE_T __bufsize =
        _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, __ssize);

    // sync the buffer to the external file so it can be deallocated
    if ((this->gptr () || this->pptr ()) && is_open () && sync () != 0)
        return 0;

    bool __reset = true;

    if (0 < __bufsize) {

        if (!__buf && (this->_C_bufsize < __bufsize || !this->_C_buffer)) {
            // if `buf' is 0 and the requested size is greater than
            // the size of the object's buffer, or of the object's
            // buffer is 0, try to allocate a new buffer of the
            // specified size
            __buf = new char_type [__bufsize];

            // delete old buffer if the object owns it
            if (this->_C_own_buf ())
                delete [] this->_C_buffer;

            // take ownership of the newly allocated buffer
            this->_C_own_buf (true);
        }
        else if (!__buf && __bufsize <= this->_C_bufsize) {
            // if `buf' is 0 and the requested size is less than
            // the size of the object's buffer, simply reuse the
            // object's buffer

            __buf   = this->_C_buffer;
            __reset = false;
        }
        else if (__buf && __buf != this->_C_buffer) {
            // if `buf' is non-0 and different from the existing
            // buffer, use it

            // delete old buffer if the object owns it
            if (this->_C_own_buf ())
                delete [] this->_C_buffer;

            // the object does not own of the new buffer
            this->_C_own_buf (false);
        }

        this->_C_buffer  = __buf;
        this->_C_bufsize = __bufsize;
        this->_C_set_unbuffered (false);
    }
    else {
        // unbuffer this stream object

        // character buffer is preserved (used as get area only),
        // streambuf object unbuffered for writing

        // to put a streambuf object into an unbuffered mode (see 27.8.1.4,
        // p10) and affect the size of the get area, setbuf() should first
        // be called with the desired (non-zero) size and then again with
        // both arguments being 0
        this->_C_set_unbuffered (true);
    }

    if (__reset) {
        this->setg (0, 0, 0);
        this->setp (0, 0);
    }

    // a character buffer of nonzero size must exist even in unbuffered mode
    _RWSTD_ASSERT (0 != this->_C_buffer);
    _RWSTD_ASSERT (0 != this->_C_bufsize);

    return this;
}


// 27.8.1.4, p 11
template<class _CharT, class _Traits>
typename basic_filebuf<_CharT, _Traits>::pos_type
basic_filebuf<_CharT, _Traits>::
seekoff (off_type __off, ios_base::seekdir __way, ios_base::openmode)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    if (!is_open ())
        return pos_type (off_type (-1));

    typedef typename traits_type::state_type _StateT;
    typedef codecvt<char_type, char, _StateT> _Codecvt;

    const int __width = _USE_FACET (_Codecvt, this->getloc ()).encoding ();

    // offset must be zero with state-dependent encoding
    if (0 != __off && __width <= 0)
        return pos_type (off_type (-1));

    // sync the buffer... (this also invalidates the get/put area)
    if (sync () != 0)
        return pos_type (off_type (-1));

    // ...and, if last operation was output, append an unshift sequence
    if (this->_C_out_last ())
        _C_unshift ();

    if (__width > 1)
        __off *= __width;

    // perform the seek and save the result
    pos_type __pos = _RW::__rw_fseek (_C_file, this->_C_state, __off, __way);

    if (__pos != pos_type (off_type (-1))) {

        // preserve the current state if not changing position
        // (only matters for state-dependent encodings for which
        // the offset is required and guaranteed to be 0)
        if (__way == ios_base::cur)
            __pos.state (_C_cur_pos.state ());

        _C_beg_pos = _C_cur_pos = __pos;
    }

    this->_C_out_last (false);  // needed by close()

    return __pos;
}


template<class _CharT, class _Traits>
typename basic_filebuf<_CharT, _Traits>::pos_type
basic_filebuf<_CharT, _Traits>::
seekpos (pos_type __pos, ios_base::openmode)
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    if (!is_open ())
        return pos_type (off_type (-1));

    // flush the output area if it exists
    if (this->pptr () != 0) {
        if (this->_C_is_eof (this->overflow (traits_type::eof ())))
            return pos_type (off_type (-1));
    }

    if (   _RW::__rw_fseek (_C_file, this->_C_state, __pos, ios_base::beg)
        == pos_type (off_type (-1)))
        return pos_type (off_type (-1));

    _C_cur_pos = _C_beg_pos = __pos;

    this->setg (0, 0, 0);
    this->setp (0, 0);

    this->_C_out_last (false);  // needed by close()

    return __pos;
}


template<class _CharT, class _Traits>
int
basic_filebuf<_CharT, _Traits>::
sync ()
{
    _RWSTD_ASSERT (this->_C_is_valid ());

    // put area active
    if (this->pptr () != 0) {
        // flush the buffer to the file
        if (this->_C_is_eof (overflow (traits_type::eof ())))
            return -1;

        if (this->_C_state & _RWSTD_IOS_STDIO)
            _RW::__rw_fflush (_C_file, this->_C_state);
    }

    // get area active
    if (this->gptr () != 0) {

        // pbacksize may need to be adjusted if it's greater than
        // the available putback area (e.g., after calling putback()
        // at the end of the buffer)
        const _RWSTD_SIZE_T __pbackavail = this->_C_putback_avail ();
        if (__pbackavail < _C_pbacksize)
            _C_pbacksize = __pbackavail;

        _RWSTD_ASSERT (0 != this->_C_bufsize);

        if (_C_pbacksize == this->_C_bufsize)
            _C_pbacksize = this->_C_bufsize - 1;

        typedef typename traits_type::state_type _StateT;
        typedef codecvt<char_type, char, _StateT> _Codecvt;

        const _Codecvt &__cvt = _USE_FACET (_Codecvt, this->getloc ());
        int __width = __cvt.encoding ();

        if (__width > 0) {
            // get the number of chars consumed in the buffer
            const off_type __consumed =
                this->gptr () - this->eback () - _C_pbacksize;

            // constant width conversion:
            // adjust the current position/state in the file,
            // taking into account CR/LF conversion on Win32
            _C_cur_pos = _C_beg_pos;
            _C_cur_pos += (__width * __consumed)
                + _C_crlf_intern_count (this->eback () + _C_pbacksize,
                                        this->gptr());
        }
        else {
            // This gets a bit tricky here because we don't know the external
            // file position corresponding to the position in the
            // internal buffer. To figure this out, we'll use the known
            // file position/state corresponding to the start of the buffer
            // (which we have carefully saved in _C_beg_pos) and
            // convert the characters up to the current position in the
            // buffer, counting how many external chars result. We can
            // then use the offset from _C_beg_pos and the state
            // returned from codecvt::out() to construct the current file
            // position

            off_type __ext_chars = 0;   // converted external chars

            char             __xbuf [_RWSTD_DEFAULT_BUFSIZE];
            char*            __xbuf_end  = __xbuf + sizeof __xbuf;
            char*            __to_next   = 0;
            const char_type* __from_next = 0;
            const char_type* __from_end  = this->gptr ();
            _StateT          __state     = _C_beg_pos.state ();
            const char_type* __base      = this->eback () + _C_pbacksize;

            while (__from_next != __from_end) {
                const int __res =
                    __cvt.out (__state, __base, __from_end, __from_next,
                               __xbuf, __xbuf_end, __to_next);

                switch (__res) {
                case codecvt_base::error:
                    return -1;

                case codecvt_base::noconv:
                    __ext_chars += __from_end - __base
                        + _C_crlf_intern_count (__base, __from_end);
                    __from_next = __from_end;   // break out of the loop
                    break;

                default:         // __res = ok or partial
                    // take into account CR/LF conversion on Win32
                    __ext_chars += __to_next - __xbuf
                        + _C_crlf_extern_count (__xbuf, __to_next);
                }
                // repeat with next chunk
                __base = __from_next;
            }

            _C_cur_pos  = _C_beg_pos;
            _C_cur_pos += __ext_chars;
            _C_cur_pos.state (__state);
        }

        // seek within the external file to the position
        // corresponding to the future beginning of the buffer
        const off_type __off =
            _RW::__rw_fseek (_C_file, this->_C_state,
                             _C_cur_pos, ios_base::beg);

        if (-1L == __off)
            return -1;

        traits_type::move (this->eback (), this->gptr () - _C_pbacksize,
                           _C_pbacksize);

        this->setg (this->eback (), this->eback () + _C_pbacksize,
                    this->eback () + _C_pbacksize);
    }

    _C_beg_pos = _C_cur_pos;

    this->setp (0, 0);

    return 0;
}


template<class _CharT, class _Traits>
basic_filebuf<_CharT, _Traits>*
basic_filebuf<_CharT, _Traits>::
_C_open (int __fd, void *__file, char_type *__buf, streamsize __n)
{
    if (is_open () || !__file && -1 == __fd || !setbuf (__buf, __n))
        return 0;

    if (__file) {
        _RWSTD_ASSERT (-1 == __fd);

        _C_file = __file;

        this->_C_state |= this->_C_stdio;
    }
    else {
        _RWSTD_ASSERT (!__file);

        _C_file = _RW::__rw_fdopen (__fd);

        if (!_C_file)
            return 0;

        this->_C_state &= ~this->_C_stdio;
    }

    this->_C_state |= _RW::__rw_fmode (_C_file, this->_C_state);

    this->setg (0, 0, 0);
    this->setp (0, 0);

    _C_cur_pos = _C_beg_pos = pos_type ();

    return this;
}


//  write out an unshift sequence if not in initial shift state
template<class _CharT, class _Traits>
bool
basic_filebuf<_CharT, _Traits>::
_C_unshift ()
{
    typedef typename traits_type::state_type _StateT;
    typedef codecvt<char_type, char, _StateT> _Codecvt;

    const _Codecvt &__cvt = _USE_FACET (_Codecvt, this->getloc ());

    // unshifting isn't necessary if encoding isn't state dependent
    // or if the state is equivalent to initial state (determined
    // by codecvt::unshift())
    if (__cvt.encoding () >= 0)
        return true;

    // buf to hold unshift sequence - assumes that the shift
    // sequence will be less than 64 chars (we can't safely
    // use dynamic allocation because this function could be
    // called as a result of memory allocation exception)

    char  __useq [64];
    char* __useq_end = 0;

    _StateT __state = _C_cur_pos.state ();

    const int __res =
        __cvt.unshift (__state, __useq, __useq + sizeof __useq, __useq_end);

    const _RWSTD_STREAMSIZE __nbytes = __useq_end - __useq;

    // in the unlikely event that the buffer isn't big enough, assert
    _RWSTD_ASSERT (__res != codecvt_base::partial);

    if (__res == codecvt_base::error)
        return false;

    if (__res == codecvt_base::noconv)
        return true;

    const off_type __nwrote =
        _RW::__rw_fwrite (_C_file, this->_C_state, __useq, __nbytes);

    if (__nwrote < 0)
        return false;

    _C_cur_pos += __nwrote;     // CR/LF conversion not an issue here

    return __nwrote == __nbytes;
}


}   // namespace std
