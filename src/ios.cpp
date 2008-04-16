/**************************************************************************
 *
 * ios.cpp - Source for the Standard Library ios classes
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#include <stdlib.h>
#include <string.h>

#include <iosfwd>

#include <rw/_error.h>

#include "iosdata.h"

#include <rw/_defs.h>


_RWSTD_NAMESPACE (__rw) {

// array of pointers to the 8 standard iostream objects
_RWSTD_EXPORT const void* __rw_std_streams [8];

}   // namespace __rw


_RWSTD_NAMESPACE (std) { 


ios_base::ios_base ()
    : _C_usr (0)
{
    // outlined to hide implementation details
    // no-op
}


// std::cin and std::wcin _C_usr_data structures
ios_base::_C_usr_data ios_base::_C_usr_data::_C_std_usr_data [2];


ios_base::~ios_base()
{
    // verify that _C_usr_data is a POD (i.e., has no user-defined ctor)
    // necessary in order for the C_std_usr_data array defined above to
    // not be overwritten during dynamic initialization
    union U {
        ios_base::_C_usr_data dummy;
    };

    // MT guard or reentrancy not necessary in a dtor

    if (_C_usr) {
        // invoke through a member pointer to avoid a link-time dependency
        // of this translation unit on the one that defines the member
        // function ios_base::_C_fire_event()
        if (_C_usr->_C_fire)
            (this->*_C_usr->_C_fire)(erase_event, false /* not reentrant */);

        _C_usr_data::_C_dealloc (_C_usr);
    }
}


streamsize ios_base::precision (streamsize prec)
{
    // outlined to hide implementation details
    return _RWSTD_ATOMIC_IO_SWAP (_C_prec, prec, _C_mutex);
}


ios_base::fmtflags ios_base::flags (fmtflags fl)
{
    const unsigned mask =
        ~(unsigned (_RWSTD_IOS_BASEMASK) << _RWSTD_IOS_BASEOFF);

    unsigned ifl = unsigned (fl);

    switch (fl & basefield) {
        // if basefield is set, clear the base mask and set
        // the numeric base bits according to the basefield
    case oct:
        ifl = ifl &  mask | 8U << _RWSTD_IOS_BASEOFF;
        break;

    case dec:
        ifl = ifl & mask | 10U << _RWSTD_IOS_BASEOFF;
        break;

    case hex:
        ifl = ifl & mask | 16U << _RWSTD_IOS_BASEOFF;
        break;

    case _RWSTD_IOS_BIN:
        ifl = ifl & mask | 2U << _RWSTD_IOS_BASEOFF;
        break;

    case 0:
        // if basefield is clear and the numeric base bits are set,
        // set the basefield accordingly, making sure that when
        // numeric base bits are set to 10, dec is left alone
        // (necessary for autodected parsing to work correctly)
        switch (ifl >> _RWSTD_IOS_BASEOFF) {
        case  0: ifl |= 10U << _RWSTD_IOS_BASEOFF; break;
        case  2: ifl |= _RWSTD_IOS_BIN; break;
        case  8: ifl |= oct; break;
        case 10: /* no-op */ break;
        case 16: ifl |= hex; break;
        }
        break;

    default:
        ifl |= 10U << _RWSTD_IOS_BASEOFF;
    }

    return fmtflags (_RWSTD_ATOMIC_IO_SWAP (_C_fmtfl, ifl, _C_mutex));
}


locale ios_base::imbue (const locale &loc)
{
    // outlined to hide implementation details

    _RWSTD_MT_GUARD (flags () & _RW::__rw_nolock ? 0 : &_C_mutex);

    return _C_unsafe_imbue (loc);
}


/* static */ bool ios_base::_C_sync_with_stdio = true;

/* static */ bool ios_base::sync_with_stdio (bool syncflag /* = true */)
{
    // outlined to hide implementation details

    // always atomic in MT environments since _C_sync is static
    return _RWSTD_ATOMIC_SWAP (_C_sync_with_stdio, syncflag, false);
}


void ios_base::_C_init (void *sb)
{
    // not guarded, assumed to be called only from ctors
    // all members are assumed to be uninitialized

    // NOTE: `sb' may point to a yet uninitialized object
    //       it is unsafe to reference any of its members

    const unsigned fmtfl = (10U << _RWSTD_IOS_BASEOFF) | skipws | dec;

    _C_state  = (_C_rdbuf = sb) ? goodbit : badbit;
    _C_fmtfl  = fmtfl;
    _C_wide   = 0;
    _C_prec   = 6;
    _C_except = 0;

#if     defined (_RWSTD_REENTRANT)             \
    && !defined (_RWSTD_NO_EXT_REENTRANT_IO)   \
    && !defined (_RWSTD_NO_REENTRANT_IO_DEFAULT)

    // disable locking of iostream objects and their associated buffers
    // standard iostream objects will override in ios_base::Init::Init()
    _C_fmtfl  |= _RW::__rw_nolock | _RW::__rw_nolockbuf;

#endif   // _RWSTD_REENTRANT && !_RWSTD_NO_REENTRANT_IO && ...
}


locale ios_base::_C_unsafe_imbue (const locale &loc)
{
    // outlined to hide implementation details

    const locale tmp (_C_loc);

    _C_loc = loc;

    if (_C_usr) {
        // invoke through a member pointer to avoid a link-time dependency
        // of this translation unit on the one that defines the member
        // function ios_base::_C_fire_event()
        if (_C_usr->_C_fire)
            (this->*_C_usr->_C_fire)(imbue_event, true /* reentrant */);
    }

    return tmp;  
}


void* ios_base::
_C_set (unsigned state, unsigned except, void *rdbuf)
{
    _RWSTD_MT_GUARD ((_C_fmtfl | except) & _RW::__rw_nolock ? 0 : &_C_mutex);

    except &= ~_RW::__rw_nolock;

    if (!rdbuf)
        state |= badbit;

    void* const rdbuf_save = _C_rdbuf;

    _C_rdbuf  = rdbuf;
    _C_state  = iostate (state & ~(_RW::__rw_nothrow | _RW::__rw_rethrow));
    _C_except = except;

#ifndef _RWSTD_NO_EXCEPTIONS

    if (!(state & _RW::__rw_nothrow) && (state & except)) {

        if (state & _RW::__rw_rethrow)
            throw;

        const char *bit =
            "std::ios::badbit, std::ios::failbit, std::ios::eofbit";

        static const char streams[][11] = {
            "std::cin", "std::cout", "std::cerr", "std::clog",
            "std::wcin", "std::wcout", "std::wcerr", "std::wclog"
        };

        // is `this' one of the standard iostreams or is it user-defined?
        const char *strm = "user-defined";
        for (const void * const *ps = _RW::__rw_std_streams; *ps; ++ps) {
            if (this == *ps)
                strm = streams [ps - _RW::__rw_std_streams];
        }

        switch (state & except) {

#ifndef _RWSTD_NO_EXT_FAILURE

        // casts below are necessary to preserve the static type
        // of the exception being thrown (upcast by _C_format ())

        case _RWSTD_IOS_BADBIT:
            _RW::__rw_throw (_RWSTD_ERROR_BADBIT_SET, strm);

        case _RWSTD_IOS_FAILBIT:
            _RW::__rw_throw (_RWSTD_ERROR_FAILBIT_SET, strm);
            
        case _RWSTD_IOS_EOFBIT:
            _RW::__rw_throw (_RWSTD_ERROR_EOFBIT_SET, strm);

#else   // if defined (_RWSTD_NO_EXT_FAILURE)

        case _RWSTD_IOS_BADBIT:
            bit = "badbit";
            break;

        case _RWSTD_IOS_FAILBIT:
            bit = "failbit";
            break;

        case _RWSTD_IOS_EOFBIT:
            bit = "eofbit";
            break;

#endif   // _RWSTD_NO_EXT_FAILURE

        case _RWSTD_IOS_BADBIT | _RWSTD_IOS_FAILBIT:
            bit = "badbit, failbit";
            break;

        case _RWSTD_IOS_BADBIT | _RWSTD_IOS_EOFBIT:
            bit = "badbit, eofbit";
            break;

        case _RWSTD_IOS_FAILBIT | _RWSTD_IOS_EOFBIT:
            bit = "failbit, eofbit";
            break;
        }

        _RW::__rw_throw (_RWSTD_ERROR_IOSTATE_BIT_SET, strm, bit);
    }

#endif   // !defined (_RWSTD_NO_EXCEPTIONS)

    return rdbuf_save;
}


void* ios_base::_C_tie () const
{
    // outlined to hide implementation details

    return _C_usr ? _C_usr->_C_tie : 0;
}


void* ios_base::_C_tie (void *pstrm)
{
    _RWSTD_MT_GUARD (flags () & _RW::__rw_nolock ? 0 : &_C_mutex);

    void *save = 0;

    if (_C_usr) {
        save = _C_usr->_C_tie;

        if (   !pstrm
            && !_C_usr->_C_iarray
            && !_C_usr->_C_parray
            && !_C_usr->_C_cbarray) {
            _C_usr_data::_C_dealloc (_C_usr);
            _C_usr = 0;
        }
    }
    else if (pstrm)
        _C_usr = _C_usr_data::_C_alloc (0);

    if (_C_usr)
        _C_usr->_C_tie = pstrm;

    return save;
}


}  // namespace std
