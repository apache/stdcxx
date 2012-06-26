/**************************************************************************
 *
 * iostore.cpp - source of the C++ Standard Library 
 *               ios_base storage functions [lib.ios.base.storage]
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

#define _RWSTD_LIB_SRC

#include <string.h>        // memcpy(), memset()
#include <rw/_rawiter.h>   // get_temporary_buffer()

#include "iosdata.h"

#include <rw/_defs.h>


_RWSTD_NAMESPACE (__rw) {


// will grow an array of POD elements, never shrinks
static void*
__rw_realloc (void *array, _RWSTD_SIZE_T size, _RWSTD_SIZE_T &n, int inx)
{
    if (inx < 0)
        return 0;   // bad index

    if (_RWSTD_STATIC_CAST (_RWSTD_SIZE_T, inx) < n)
        return array;   // no need to reallocate

    void *tmp;

    _TRY {
        // make sure index is dereferencable (cast prevents HP aCC warnings)
        tmp = operator new (_RWSTD_STATIC_CAST (_RWSTD_SIZE_T, ++inx) * size);
    }
    _CATCH (...) {
        return 0;
    }

    _RWSTD_ASSERT (0 != tmp);

    // copy elements from array to tmp
    if (array)
        memcpy (tmp, array, n * size);

    // zero out newly allocated storage
    memset (_RWSTD_STATIC_CAST (char*, tmp) + n * size, 0, (inx - n) * size);

    // modify passed-in size
    n = _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, inx);

    operator delete (array);

    return tmp;
}


}   // namespace __rw


_RWSTD_NAMESPACE (std) {


/* static */ int ios_base::xalloc ()
{
    // outlined to hide implementation details

    static int inx /* = 0 */;

    return _RWSTD_ATOMIC_PREINCREMENT (inx, false) - 1;
}


void ios_base::_C_fire_event (event ev, bool reentrant)
{
    if (!_C_usr)
        return;

    _C_usr_data::_C_event_cb *cba    = _C_usr->_C_cbarray;
    _RWSTD_SIZE_T             cbsize = _C_usr->_C_cbsize;

    // verify consistency (either both are 0 or neither is)
    _RWSTD_ASSERT (!!cba == !!cbsize);

    if (!cba)
        return;

    // caller expected to lock if necessary

    _TRY {
        // protect `cba' from an exception (copy only necessary
        // when `reentrant' is non-zero in which case we unlock
        // the mutex and allow registered callbacks to call the
        // stream member functions recursively)
        if (reentrant) {
            // never throws
            cba = _GET_TEMP_BUFFER (_C_usr_data::_C_event_cb, cbsize).first;

            if (!cba) {
                // clear may throw...
                _C_set (_C_state | badbit, _C_except | _RW::__rw_nolock,
                        _C_rdbuf);
                return;
            }

            // verify consistency (neither must be 0 at this point)
            _RWSTD_ASSERT (_C_usr->_C_cbarray && _C_usr->_C_cbsize);

            // copy into temporary buffer
            memcpy (cba, _C_usr->_C_cbarray, cbsize * sizeof *cba);

            // unlock only if stream state allows it
            if (!(flags () & _RW::__rw_nolock))
                _C_unlock ();
        }

        // 27.4.2.6, p1 - call callbacks in opposite order of registration
        for (_RWSTD_SIZE_T i = cbsize; i-- > 0; ) {

            _RWSTD_ASSERT (cba && cba [i]._C_fn);

            // ok if callback calls locking functions, we're unlocked
            // and working with a local copy of callack array
            cba [i]._C_fn (ev, *this, cba [i]._C_index);
        }

        if (reentrant) {
            return_temporary_buffer (cba);

            // lock only if stream state allows it (stream state
            // may have been modified by one of the callbacks)
            if (!(flags () & _RW::__rw_nolock))
                _C_lock ();
        }
    }
    _CATCH (...) {
        if (reentrant) {
            return_temporary_buffer (cba);

            // lock only if `cba' was successfully allocated (or copied)
            // and the stream state allows it (stream state may have been
            // modified by one of the callbacks)
            if (cba && !(flags () & _RW::__rw_nolock))
                _C_lock ();
        }
        _RETHROW;
    }
}


// succeeds irrespective of rdstate()
void ios_base::
_C_copyfmt (const ios_base &rhs,
            void *dst, const void *src, _RWSTD_SIZE_T size)
{
    if (this == &rhs)
        return;

    typedef _C_usr_data::_C_event_cb EventCB;

    // allocate sufficient storage
    long    *ia  = 0;
    void*   *pa  = 0;
    EventCB *cba = 0;

    _RWSTD_SIZE_T a_size [3];    // sizes of arrays above

    unsigned   fmtfl;    // formatting flags
    streamsize prec;     // new precision
    streamsize wide;     // new width
    unsigned   except;   // new exceptions
    void*      ptie;     // tied ostream
    locale     loc;      // new locale

    char srcbuf [16];    // buffer to copy `src' to

    // flags to copy from `rhs'
    const fmtflags flagmask = ~(_RW::__rw_nolock | _RW::__rw_nolockbuf);

    _TRY {
        // lock `rhs', *this not locked yet
        _RWSTD_MT_GUARD (rhs.flags () & _RW::__rw_nolock
                         ? 0 : &_RWSTD_CONST_CAST (ios_base&, rhs)._C_mutex);

        if (rhs._C_usr) {

            // copy pointer to the tied ostream, if any
            ptie = rhs._C_usr->_C_tie;

            // for convenience
            const _C_usr_data* const rusr = rhs._C_usr;

            // verify that either both are 0 or neither is
            _RWSTD_ASSERT (!!rusr->_C_iarray == !!rusr->_C_isize);
            _RWSTD_ASSERT (!!rusr->_C_parray == !!rusr->_C_psize);
            _RWSTD_ASSERT (!!rusr->_C_cbarray == !!rusr->_C_cbsize);

            a_size [0] = rusr->_C_isize;
            a_size [1] = rusr->_C_psize;
            a_size [2] = rusr->_C_cbsize;
                
            ia = _RWSTD_STATIC_CAST (long*,
                     a_size [0] ? operator new (a_size [0] * sizeof *ia) : 0);

            pa = _RWSTD_STATIC_CAST (void**,
                     a_size [1] ? operator new (a_size [1] * sizeof *pa) : 0);

            cba = _RWSTD_STATIC_CAST (EventCB*,
                     a_size [2] ? operator new (a_size [2] * sizeof *cba) : 0);

            // copy rhs into allocated storage and assign (27.4.4.2, p15)
            memcpy (ia,  rusr->_C_iarray,  a_size [0] * sizeof *ia);
            memcpy (pa,  rusr->_C_parray,  a_size [1] * sizeof *pa);
            memcpy (cba, rusr->_C_cbarray, a_size [2] * sizeof *cba);
        }
        else {
            // zero out array to prevent (bogus) gcc warnings
            // about the variable being used uninitialized
            memset (a_size, 0, sizeof a_size);

            ptie = 0;
        }

        // copy the rest of rhs state (save for exceptions)
        // while it's locked to guarantee atomicity
        fmtfl  = rhs._C_fmtfl;
        prec   = rhs._C_prec;
        wide   = rhs._C_wide;
        except = rhs._C_except;
        loc    = rhs._C_loc;

        // copy additional data (rhs's fill char) to the small
        // temporary buffer only if it fits
        if (size <= sizeof srcbuf)
            src = memcpy (srcbuf, src, size);
    }
    _CATCH (...) {
        // *this is unmodified
        operator delete (ia);
        operator delete (pa);
        operator delete (cba);

        _RETHROW;
    }

    // `rhs' unlocked, lock *this
    _RWSTD_MT_GUARD (flags () & _RW::__rw_nolock ? 0 : &_C_mutex);

    _TRY {
        if (_C_usr) {
            // fire erase events (27.4.4.2, p17) - may throw
            if (_C_usr->_C_fire)
                (this->*_C_usr->_C_fire)(erase_event, true /* reentrant */);

            // delete existing arrays, if any; _C_usr will only be deleted
            // if `rhs' contains no user data (see below)
            operator delete (_C_usr->_C_iarray);
            _C_usr->_C_iarray = 0;
            _C_usr->_C_isize = 0;
            operator delete (_C_usr->_C_parray);
            _C_usr->_C_parray = 0;
            _C_usr->_C_psize = 0;
            operator delete (_C_usr->_C_cbarray);
            _C_usr->_C_cbarray = 0;
            _C_usr->_C_cbsize = 0;
        }
        else if (ia || pa || cba || ptie) {
            // allocation may throw
            _C_usr = new _C_usr_data ();

#ifdef _RWSTD_NO_POD_ZERO_INIT

#  ifdef _RWSTD_NO_NEW_THROWS

            if (!_C_usr)
                _RW::__rw_throw (_RWSTD_ERROR_FIRST + 3 /* BAD_ALLOC */);

#  endif   // _RWSTD_NO_NEW_THROWS

            _RWSTD_ASSERT (0 != _C_usr);
            memset (_C_usr, 0, sizeof *_C_usr);

#endif   // _RWSTD_NO_POD_ZERO_INIT

        }
    }
    _CATCH (...) {
        // *this is unmodified
        operator delete (ia);
        operator delete (pa);
        operator delete (cba);

        _RETHROW;
    }

    if (ia || pa || cba || ptie) {

        // assing allocated and copied arrays and their sizes
        _C_usr->_C_iarray  = ia;
        _C_usr->_C_parray  = pa;
        _C_usr->_C_cbarray = cba;

        _C_usr->_C_isize   = a_size [0];
        _C_usr->_C_psize   = a_size [1];
        _C_usr->_C_cbsize  = a_size [2];

        _C_usr->_C_tie     = ptie;

        _C_usr->_C_fire    = &ios_base::_C_fire_event;
    }
    else {
        // `rhs' contains no user data, delete _C_usr
        _C_usr_data::_C_dealloc (_C_usr);
        _C_usr = 0;
    }

    // copy all but masked flags(), leave masked flags alone
    _C_fmtfl = fmtfl & flagmask | _C_fmtfl & ~flagmask;
    _C_prec  = prec;
    _C_wide  = wide;
    _C_loc   = loc;

    // copy additional data (fill character) -- will be atomic
    // only if size is sufficiently small; since this is used
    // for the fill character represented in derived classes
    // as int_type this will be true for all standard iostreams
    memcpy (dst, src, size);

    if (_C_usr && _C_usr->_C_fire) {
        _TRY {
            // fire copy events (27.4.4.2, p17), may temporarily unlock
            // or throw
            (this->*_C_usr->_C_fire)(copyfmt_event, true /* reentrant */);
        }
        _CATCH (...) {
            // must still assign exceptions
            _C_except = except;

            // just rethrow caught exception (don't call clear())
            _RETHROW;
        }
    }

    // assign exceptions last (27.4.4.2, p15, bullet 2)
    _C_except = except;

    // leave state alone but throw an exception if necessary
    _C_set (_C_state, _C_except | _RW::__rw_nolock, _C_rdbuf);
}


long& ios_base::iword (int inx)
{
    _RWSTD_MT_GUARD (flags () & _RW::__rw_nolock ? 0 : &_C_mutex);

    if (!_C_usr)
        _C_usr = _C_usr_data::_C_alloc (&ios_base::_C_fire_event);

    long* const ia = _C_usr
        ? _RWSTD_STATIC_CAST (long*, _RW::__rw_realloc (_C_usr->_C_iarray,
                                                        sizeof (long),
                                                        _C_usr->_C_isize,
                                                        inx))
        : 0;

    if (ia) {
        _RWSTD_ASSERT (  _C_usr->_C_isize
                       > _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, inx));

        return (_C_usr->_C_iarray = ia)[inx];
    }

    _C_set (_C_state | badbit, _C_except | _RW::__rw_nolock, _C_rdbuf);

    // returns a reference to a dummy object on failure (27.4.2.5, p3)
    static long dummy_iword;

    return dummy_iword = 0;
}


void*& ios_base::pword (int inx)
{ 
    _RWSTD_MT_GUARD (flags () & _RW::__rw_nolock ? 0 : &_C_mutex);

    if (!_C_usr)
        _C_usr = _C_usr_data::_C_alloc (&ios_base::_C_fire_event);

    void** const pa = _C_usr
        ? _RWSTD_STATIC_CAST (void**, _RW::__rw_realloc (_C_usr->_C_parray,
                                                         sizeof (void*),
                                                         _C_usr->_C_psize,
                                                         inx))
        : 0;

    if (pa) {
        _RWSTD_ASSERT (  _C_usr->_C_psize
                       > _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, inx));

        return (_C_usr->_C_parray = pa)[inx];
    }

    _C_set (_C_state | badbit, _C_except | _RW::__rw_nolock, _C_rdbuf);

    // returns a reference to a dummy object on failure (27.4.2.5, p3)
    static void *dummy_pword;

    return dummy_pword = 0;
}


void ios_base::
register_callback (event_callback fun, int inx)
{
    _RWSTD_ASSERT (0 != fun);

    _RWSTD_MT_GUARD (flags () & _RW::__rw_nolock ? 0 : &_C_mutex);

    if (!_C_usr)
        _C_usr = _C_usr_data::_C_alloc (&ios_base::_C_fire_event);

    typedef _C_usr_data::_C_event_cb EventCB;

    _RWSTD_SIZE_T  sz  = 0;
    EventCB       *cba = 0;

    if (_C_usr) {
        sz  = _C_usr->_C_cbsize;
        cba = _RWSTD_STATIC_CAST (EventCB*,
                                  _RW::__rw_realloc (_C_usr->_C_cbarray,
                                                     sizeof (EventCB),
                                                     _C_usr->_C_cbsize,
                                                     int (sz)));
    }

    if (cba) {
        // array must have grown by at least one element
        _RWSTD_ASSERT (sz < _C_usr->_C_cbsize);

        _C_usr->_C_cbarray = cba;

        _C_usr->_C_cbarray [sz]._C_index = inx;
        _C_usr->_C_cbarray [sz]._C_fn    = fun;
    }
    else {
        // not required by 27.4.2.6
        _C_set (_C_state | badbit, _C_except | _RW::__rw_nolock, _C_rdbuf);
    }
}


}   // namespace std
