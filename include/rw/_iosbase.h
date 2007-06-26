/***************************************************************************
 *
 * _iosbase.h - Declarations for the Standard Library basic stream I/O
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
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

#ifndef _RWSTD_RW_IOSBASE_H_INCLUDED
#define _RWSTD_RW_IOSBASE_H_INCLUDED


#include <loc/_locale.h>

#ifndef _RWSTD_RW_IOSFAILURE_H_INCLUDED
#  include <rw/_iosfailure.h>
#endif   // _RWSTD_RW_IOSFAILURE_H_INCLUDED

#ifndef _RWSTD_RW_IOSFWD_H_INCLUDED
#  include <rw/_iosfwd.h>
#endif   // _RWSTD_RW_IOSFWD_H_INCLUDED

#ifndef _RWSTD_RW_TRAITS_H_INCLUDED
#  include <rw/_traits.h>
#endif   // _RWSTD_RW_TRAITS_H_INCLUDED


_RWSTD_NAMESPACE (std) {


struct _RWSTD_EXPORT ios_base: _RW::__rw_synchronized
{
    // 27.4.2.1.1
    typedef _RW::__rw_failure failure;

#ifndef _RWSTD_NO_EXT_FAILURE

    // extensions
    typedef _RW::__rw_badbit_set  badbit_set;
    typedef _RW::__rw_eofbit_set  eofbit_set;
    typedef _RW::__rw_failbit_set failbit_set;

#endif   // _RWSTD_NO_EXT_FAILURE

    // 27.4.2.1.2
    typedef _RWSTD_BITMASK_ENUM (_RW::__rw_fmtflags) fmtflags;

    // insert and extract bool type in alphabetic format 
    _RWSTD_STATIC_CONST (fmtflags, boolalpha = _RW::__rw_boolalpha);

    // converts integer input or generates integer output in decimal base
    _RWSTD_STATIC_CONST (fmtflags, dec = _RW::__rw_dec);

    // generate floating-point output in fixed-point notation
    _RWSTD_STATIC_CONST (fmtflags, fixed = _RW::__rw_fixed);

    // converts integer input or generates integer output in hexadecimal base
    _RWSTD_STATIC_CONST (fmtflags, hex = _RW::__rw_hex);

    // adds fill characters at a designated internal point in certain generated
    // output, or identical to right if no such point is designated
    _RWSTD_STATIC_CONST (fmtflags, internal = _RW::__rw_internal);

    // adds fill characters on the right of certain generated output
    _RWSTD_STATIC_CONST (fmtflags, left = _RW::__rw_left);

    // converts integer input or generates integer output in octal base
    _RWSTD_STATIC_CONST (fmtflags, oct = _RW::__rw_oct);

    // adds fill characters on the left of certain generated output
    _RWSTD_STATIC_CONST (fmtflags, right = _RW::__rw_right);

    // generates floating-point output in scientific notation
    _RWSTD_STATIC_CONST (fmtflags, scientific = _RW::__rw_scientific);

    // generates a prefix indicating the numeric base of generated integer
    // output (bin - none, oct - "0", dec - none, hex - "0x")
    _RWSTD_STATIC_CONST (fmtflags, showbase = _RW::__rw_showbase);

    // generates a decimal-point character unconditionally in generated
    // floating-point output
    _RWSTD_STATIC_CONST (fmtflags, showpoint = _RW::__rw_showpoint);

    // generates a + sign in non-negative generated numeric output
    _RWSTD_STATIC_CONST (fmtflags, showpos = _RW::__rw_showpos);

    // skips leading white space before certain input operations
    _RWSTD_STATIC_CONST (fmtflags, skipws = _RW::__rw_skipws);

    // flushes output after each output operation
    _RWSTD_STATIC_CONST (fmtflags, unitbuf = _RW::__rw_unitbuf);

    // replaces certain lowercase letters with their uppercase equivalents
    // in generated output
    _RWSTD_STATIC_CONST (fmtflags, uppercase = _RW::__rw_uppercase);

#ifndef _RWSTD_NO_EXT_BIN_IO

    // extension - converts integer input or generates integer output
    // in binary base
    _RWSTD_STATIC_CONST (fmtflags, bin = _RW::__rw_bin);

#endif   // _RWSTD_NO_EXT_BIN_IO

    _RWSTD_STATIC_CONST (fmtflags, basefield = _RW::__rw_basefield);
    _RWSTD_STATIC_CONST (fmtflags, adjustfield = _RW::__rw_adjustfield);
    _RWSTD_STATIC_CONST (fmtflags, floatfield = _RW::__rw_floatfield);

#ifndef _RWSTD_NO_EXT_REENTRANT_IO

    // extension: never locks the object in MT environments
    _RWSTD_STATIC_CONST (fmtflags, nolock = _RW::__rw_nolock);

    // extension: never locks stream buffer in MT environments
    _RWSTD_STATIC_CONST (fmtflags, nolockbuf = _RW::__rw_nolockbuf);

#endif   // _RWSTD_NO_EXT_REENTRANT_IO

    // 27.4.2.1.3
    typedef _RWSTD_BITMASK_ENUM (_RW::__rw_iostate) iostate;

    _RWSTD_STATIC_CONST (iostate, goodbit = _RW::__rw_goodbit);

    // indicates a loss of integrity in an input or output sequence
    // (such as an irrecoverable read error from a file); 
    _RWSTD_STATIC_CONST (iostate, badbit = _RW::__rw_badbit);

    // indicates that an input operation reached the end of an input sequence
    _RWSTD_STATIC_CONST (iostate, eofbit = _RW::__rw_eofbit);

    // indicates that an input operation failed to read the expected
    // characters, or that an output operation failed to generate the
    // desired characters
    _RWSTD_STATIC_CONST (iostate, failbit = _RW::__rw_failbit);

    // 27.4.2.1.4
    typedef _RWSTD_BITMASK_ENUM (_RW::__rw_openmode) openmode;

    // seek to end before each write 
    _RWSTD_STATIC_CONST (openmode, app = _RW::__rw_app);

    // perform input and output in binary mode (as opposed to text mode)
    _RWSTD_STATIC_CONST (openmode, binary = _RW::__rw_binary);

    // open for input
    _RWSTD_STATIC_CONST (openmode, in = _RW::__rw_in);

    // open for output
    _RWSTD_STATIC_CONST (openmode, out = _RW::__rw_out);

    // truncate an existing stream when opening
    _RWSTD_STATIC_CONST (openmode, trunc = _RW::__rw_trunc);

    // open and seek to end immediately after opening 
    _RWSTD_STATIC_CONST (openmode, ate = _RW::__rw_ate);

#ifndef _RWSTD_STRICT_ANSI

    // extensions for compatibility with Classic Iostreams

    // do not create a file if it doesn't exist
    _RWSTD_STATIC_CONST (openmode, nocreate = _RW::__rw_nocreate);

    // do not replace an existing file
    _RWSTD_STATIC_CONST (openmode, noreplace = _RW::__rw_noreplace);

#endif   // _RWSTD_STRICT_ANSI

#ifndef _RWSTD_NO_EXT_STDIO

    // use the C stdio library for all file I/O
    _RWSTD_STATIC_CONST (openmode, stdio = _RW::__rw_stdio);

    // use the native OS calls for all file I/O
    _RWSTD_STATIC_CONST (openmode, native = _RW::__rw_native);

#endif   // _RWSTD_NO_EXT_STDIO

    typedef _RWSTD_BITMASK_ENUM (_RW::__rw_seekdir) seekdir;

    // 27.4.2.1.5, p1
    _RWSTD_STATIC_CONST (seekdir, beg = _RW::__rw_beg);
    _RWSTD_STATIC_CONST (seekdir, cur = _RW::__rw_cur);
    _RWSTD_STATIC_CONST (seekdir, end = _RW::__rw_end);

#ifndef _RWSTD_NO_DEPRECATED    

    // D.6, p1 (must be integer types and not enums)
    typedef int              seek_dir;
    typedef int              open_mode;
    typedef int              io_state;
    typedef _RWSTD_STREAMOFF streamoff;
    typedef _RWSTD_STREAMOFF streampos;

#endif // _RWSTD_NO_DEPRECATED    
    
    // 27.4.2.1.6
    struct _RWSTD_EXPORT Init {
        Init ();

        ~Init ();
    };

    // 27.4.2.2, p1
    fmtflags flags () const {
        return fmtflags (_C_fmtfl);
    }

    // 27.4.2.2, p2
    fmtflags flags (fmtflags);

    // 27.4.2.2, p4
    fmtflags setf (fmtflags __f) {
        return flags (flags () | __f);
    }

    // 27.4.2.2, p6
    fmtflags setf (fmtflags __f, fmtflags __mask) {
        return flags (flags () & ~__mask | __f & __mask);
    }

    // 27.4.2.2, p8
    void unsetf (fmtflags __f) {
        flags (flags () & ~__f);
    }

    // 27.4.2.2, p9
    _RWSTD_STREAMSIZE precision () const {
        return _C_prec;
    }

    // 27.4.2.2, p10
    _RWSTD_STREAMSIZE precision (_RWSTD_STREAMSIZE);

    // 27.4.2.2, p12
    _RWSTD_STREAMSIZE width () const {
        return _C_wide;
    }

    // 27.4.2.2, p13
    _RWSTD_STREAMSIZE width (_RWSTD_STREAMSIZE);

    // 27.4.2.3, p1
    locale imbue (const locale&);

    // extension: avoids reference counting in MT builds (may result in
    // a speedup of up to 50%); this is an alternative to caching a reference
    // (pointer) to a facet in each stream and stream buffer object
    locale& getloc () {
        return _C_loc;
    }

    // 27.4.2.3, p4
    locale getloc () const {
        return _C_loc;
    }

    // 27.4.2.5, p1
    static int xalloc ();

    // 27.4.2.5, p2
    long& iword (int);

    // 27.4.2.5, p4
    void*& pword (int);

    enum event { erase_event, imbue_event, copyfmt_event };

    typedef void (*event_callback)(event, ios_base&, int);

    // 27.4.2.6, p1
    void register_callback (event_callback, int);
    
    // 27.4.2.4, p1
    static bool sync_with_stdio (bool = true);

    // 27.4.2.7, p2
    virtual ~ios_base ();    

    // returns a numeric base as per 22.2.2.1.2, p4
    int _C_base () const {
        return _RWSTD_STATIC_CAST (unsigned, flags ()) >> _RWSTD_IOS_BASEOFF;
    }

protected:

    // 27.4.2.7, p1
    ios_base ();

    // implements basic_ios<T>::init()
    void _C_init (void*);

    // will be reentrant only if `reentrant' is true
    void _C_fire_event (event, bool __reentrant);

    // implements basic_ios<T>::copyfmt()
    void _C_copyfmt (const ios_base&, void*, const void*, _RWSTD_SIZE_T);

    // sets state, exceptions, and rdbuf to the function arguments
    // in that order (except state & ~_RWSTD_IOS_NOTHROW)
    // may throw except when _RWSTD_IOS_NOTHROW is set in the first
    // argument; returns the previous value of rdbuf
    void* _C_set (unsigned, unsigned, void*);

    // retrieves and sets the tied stream pointer
    void* _C_tie () const;
    void* _C_tie (void*);

    // called from basic_ios<>::imbue (), not thread safe
    locale _C_unsafe_imbue (const locale&);

    void              *_C_rdbuf;    // pointer to the associated stream buffer
    _RWSTD_STREAMSIZE  _C_prec;     // current precision
    _RWSTD_STREAMSIZE  _C_wide;     // current width
    unsigned           _C_fmtfl;    // formatting flags
    unsigned char      _C_state;    // stream state
    unsigned char      _C_except;   // active exceptions
    
    static bool _C_sync_with_stdio;

private:

    ios_base (const ios_base&);               // not defined
    ios_base& operator= (const ios_base&);    // not defined

#if __EDG_VERSION__ < 245

    // working around an EDG eccp 2.3x bug (also derivatives,
    // such as MIPSpro, see PR #28631)
public:
    struct _C_usr_data;

#else

protected:
    struct _C_usr_data;

#endif   // EDG < 2.45

protected:
    _C_usr_data *_C_usr;      // user data (iword, pword, callbacks)
    locale       _C_loc;      // locale associated with stream

    friend struct _RWSTD_EXPORT Init;

    // working around an EDG eccp 3.2 and prior bug (see PR #29526)
    friend struct _C_usr_data;
};


inline _RWSTD_STREAMSIZE ios_base::
width (_RWSTD_STREAMSIZE __width)
{
    _RWSTD_ASSERT (0 != this);

    const _RWSTD_STREAMSIZE __tmp = _C_wide;

    // not thread-safe for efficiency since width(0) is called
    // (directly or through a facet) by each formatted output
    // function and the return value is not used
    _C_wide = __width;

    return __tmp;
}


}   // namespace std


#endif   // _RWSTD_RW_IOSBASE_H_INCLUDED
