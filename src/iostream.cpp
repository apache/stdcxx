/**************************************************************************
 *
 * iostream.cpp - source for the Standard Library iostream objects
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

#define _RWSTD_LIB_SRC

#if    3 == __GNUG__ && 3 > __GNUC_MINOR__ \
    || 3 == __GNUC_MINOR__ && 1 >__GNUC_PATCHLEVEL__

  // working around a gcc bug (PR #29570)
#  include <rw/_config.h>
#  ifndef _RWSTD_NO_EXTERN_TEMPLATE
#    define _RWSTD_NO_EXTERN_TEMPLATE
#  endif
#endif   // gcc >= 3.0 && gcc < 3.3.1

#include <rw/_defs.h>

#ifdef _WIN32
#  include <io.h>
#  ifndef STDIN_FILENO
#    define STDIN_FILENO  0
#    define STDOUT_FILENO 1
#    define STDERR_FILENO 2
#  endif   // STDIN_FILENO
#else
   // unistd.h must preceed other headers to avoid a namespace issue
   // ("time_t" vrs "std::time_t") on SunPro 5.3/SunOS 5.8
#  include <unistd.h>
#endif 

#include <iosfwd>
#include <istream>
#include <ostream>
#include <fstream>
#include <memory>
#include <new>

#include <rw/_error.h>

#include "iosdata.h"
#include "podarray.h"   // for __rw_aligned_buffer

#include <stdio.h>    // fprintf(), stderr
#include <stdlib.h>   // abort()


#ifdef _MSC_VER
   // force early intitalization of Standard Iostream Objects
   // gives warning C4073: initializers put in library initialization
   // area; disable it
#  ifdef _RWSTD_MSVC
#    pragma warning (disable: 4073)
#  endif
#  pragma init_seg (lib)
#endif   // _MSC_VER


// #defined  below
#undef _INIT

// stream objects are initialized with static basic_filebuf objects
// based on static data buffers to avoid dynamic memory allocation
// at library initialization time

#ifndef _RWSTD_NO_STATIC_IOSTREAM_INIT

// iostream objects and buffers are (re)constructed in ios_base::Init()
// via calls to placement new after being zero-initialized (not constructed)
// at definition time (by the means of the __rw_aligned_buffer trick)

#  ifndef _RWSTD_NO_IOSTREAM_OBJECT_REFS

// cin, cout, and the other Standard iostream objects are references to
// these objects which of caurse violates 27.3 but avoids the potential
// for destruction prior to any other objects with static storage duration
// and is undetectable

#define _RWSTD_DEFINE_STREAM_OBJECT(buftype, stream, name, ign1, ign2)        \
  _RWSTD_NAMESPACE (__rw) {                                                   \
    /* static character buffer to prevent dynamic allocation */               \
    static _STD::stream::char_type __rw_##name##_databuf [512];               \
    /* static indestructible `basic_filebuf<>' object */                      \
    static __rw_aligned_buffer<_STD::buftype> __rw_##name##_buffer;           \
    /* static indestructible `basic_[i|o]stream<>' object */                  \
    static __rw_aligned_buffer<_STD::stream> __rw_indestructible_##name;      \
  }   /* namespace __rw */                                                    \
  _RWSTD_NAMESPACE (std) {                                                    \
    /* reference (not object) to a basic_[i|o]stream<> object */              \
    _RWSTD_EXPORT stream &name                                                \
        = _RWSTD_REINTERPRET_CAST (stream&, _RW::__rw_indestructible_##name); \
  }   /* namespace std */                                                     \
  typedef void __rw_unused_typedef

#define _OBJECT(ignore, name)   name

#  else   // if defined (_RWSTD_NO_IOSTREAM_OBJECT_REFS)

// cin, cout, and the other Standard iostream objects are defined to be
// specializations of the POD type __rw::__rw_aligned_buffer<[i|o]stream>
// but declared in <iostream> to be extern [i|o]stream
// since the type information isn't encoded (mangled) in data names this
// lie is transparent to the compiler/linker as neither ever sees both
// declarations

#define _RWSTD_DEFINE_STREAM_OBJECT(buftype, stream, name, ign1, ign2)      \
  _RWSTD_NAMESPACE (__rw) {                                                 \
    /* static character buffer to prevent dynamic allocation */             \
    static _STD::stream::char_type __rw_##name##_databuf [512];             \
    /* static indestructible `basic_filebuf<>' object */                    \
    static __rw_aligned_buffer<_STD::buftype> __rw_##name##_buffer;         \
  }   /* namespace __rw */                                                  \
  _RWSTD_NAMESPACE (std) {                                                  \
    /* extern  indestructible `basic_[i|o]stream<>' object */               \
    _RWSTD_EXPORT _RW::__rw_aligned_buffer<stream> name;                    \
  }   /* namespace std */                                                   \
  _RWSTD_NAMESPACE (__rw) {                                                 \
    /* reference to the actual object for convenience */                    \
    static __rw_aligned_buffer<_STD::stream>&                               \
        __rw_indestructible_##name = _STD::name;                            \
  }   /* namespace __rw */                                                  \
  typedef void __rw_unused_typedef

#define _OBJECT(type, name)   _RWSTD_REINTERPRET_CAST (type&, name)

#  endif   // _RWSTD_NO_IOSTREAM_OBJECT_REFS

// expression returning the address of the initialized object
#define _INIT(buftype, stream, name, buf, f)                                  \
  /* construct basic_[i|o]stream object backed by a filebuf object */         \
  /* backed by a static characater array                           */         \
  new (&_RW::__rw_indestructible_##name) stream                               \
          (new (&_RW::__rw_##name##_buffer) buftype                           \
              (f, _RW::__rw_##name##_databuf,                                 \
     sizeof _RW::__rw_##name##_databuf / sizeof *_RW::__rw_##name##_databuf)) \


#else   // if defined (_RWSTD_NO_STATIC_IOSTREAM_INIT)

// iostream objects and buffers are constructed at definition time

#define _RWSTD_DEFINE_STREAM_OBJECT(buftype, stream, name, buf, f)           \
  _RWSTD_NAMESPACE (__rw) {                                                  \
    /* static character buffer to prevent dynamic allocation */              \
    static _STD::stream::char_type __rw_##name##_databuf [512];              \
    /* static `basic_filebuf<>' object, destroyed at program termination */  \
    static _STD::buftype __rw_##name##_buffer (f,                            \
        __rw_##name##_databuf,                                               \
        sizeof __rw_##name##_databuf / sizeof *__rw_##name##_databuf);       \
  }   /* namespace __rw */                                                   \
  _RWSTD_NAMESPACE (std) {                                                   \
    /* extern `basic_[i|o]stream<>' object, destroyed at termination */      \
    _RWSTD_EXPORT stream name (&_RW::__rw_##buf##_buffer);                   \
  }   /* namespace std */                                                    \
  typedef void __rw_unused_typedef

#define _INIT(ign1, stream, ign3, ign4, ign5)   ((stream*)0)

#endif   // _RWSTD_NO_STATIC_IOSTREAM_INIT


// define iostream objects and their corresponding "buffers"
_RWSTD_DEFINE_STREAM_OBJECT (filebuf, istream, cin,  cin,  STDIN_FILENO);
_RWSTD_DEFINE_STREAM_OBJECT (filebuf, ostream, cout, cout, STDOUT_FILENO);
_RWSTD_DEFINE_STREAM_OBJECT (filebuf, ostream, cerr, cerr, STDERR_FILENO);
_RWSTD_DEFINE_STREAM_OBJECT (filebuf, ostream, clog, cerr, STDERR_FILENO);

#ifndef _RWSTD_NO_WCHAR_T

_RWSTD_DEFINE_STREAM_OBJECT (wfilebuf, wistream, wcin,  wcin,  STDIN_FILENO);
_RWSTD_DEFINE_STREAM_OBJECT (wfilebuf, wostream, wcout, wcout, STDOUT_FILENO);
_RWSTD_DEFINE_STREAM_OBJECT (wfilebuf, wostream, wcerr, wcerr, STDERR_FILENO);
_RWSTD_DEFINE_STREAM_OBJECT (wfilebuf, wostream, wclog, wcerr, STDERR_FILENO);

#endif   // _RWSTD_NO_WCHAR_T


// allow basic_ostream<> inline member functions to refer to the standard
// iostream objects without making those objects visible to user code (so
// that they can safely be dereferenced only after #including <iostream>
// which is the only header where they are declared and by the means of
// which they are initialized - through the static initialization object)

_RWSTD_NAMESPACE (__rw) { 

#ifdef _RWSTD_NO_STATIC_IOSTREAM_INIT

// a single (library-wide) iostream initializer object - must be defined
// after the definition of the iostream objects (above) so that when
// its dtor manipulates the objects they have not been destroyed
ios_base::Init __rw_stream_initializer;

#endif   // _RWSTD_NO_STATIC_IOSTREAM_INIT


// array of pointers to the 8 standard iostream objects
extern const void* __rw_std_streams [];


// iostream initialization counter
static int __rw_ios_initcnt /* = 0 */;

}   // namespace __rw


_RWSTD_NAMESPACE (std) {

ios_base::Init::Init ()
{
    // only the first thread initializes
    if (1 != _RWSTD_ATOMIC_PREINCREMENT (_RW::__rw_ios_initcnt, false))
        return;

    // since references need not be initialzied statically, cin, et al
    // may not be initialized at this time (i.e., &cin == 0 may hold,
    // and does with MSVC 6.0), obtain and use pointers to the objects
    istream *pcin =  _INIT (filebuf, istream, cin,  cin,  STDIN_FILENO);
    ostream *pcout = _INIT (filebuf, ostream, cout, cout, STDOUT_FILENO);
    ostream *pcerr = _INIT (filebuf, ostream, cerr, cerr, STDERR_FILENO);
    ostream *pclog = _INIT (filebuf, ostream, clog, cerr, STDERR_FILENO);

    // stream objects expected to be arranged in this order
    // must be initialized dynamically (as opposed to statically)
    _RW::__rw_std_streams[0] = _RWSTD_STATIC_CAST (ios_base*, pcin);
    _RW::__rw_std_streams[1] = _RWSTD_STATIC_CAST (ios_base*, pcout);
    _RW::__rw_std_streams[2] = _RWSTD_STATIC_CAST (ios_base*, pcerr);
    _RW::__rw_std_streams[3] = _RWSTD_STATIC_CAST (ios_base*, pclog);

    // 27.3.1, p2
    pcin->_C_usr = &_C_usr_data::_C_std_usr_data [0];
    pcin->_C_usr->_C_tie = pcout;

    // 27.3.1, p5
    pcerr->_C_fmtfl |= _RW::__rw_unitbuf;

    pcin->_C_fmtfl  |= _RW::__rw_sync_stdio;
    pcout->_C_fmtfl |= _RW::__rw_sync_stdio;
    pcerr->_C_fmtfl |= _RW::__rw_sync_stdio;
    pclog->_C_fmtfl |= _RW::__rw_sync_stdio;

#if     defined (_RWSTD_REENTRANT)             \
    && !defined (_RWSTD_NO_EXT_REENTRANT_IO)   \
    && !defined (_RWSTD_NO_REENTRANT_IO_DEFAULT)

    // override MT-safety setting done in basic_ios<>::init()
    pcin->_C_fmtfl  &= ~(nolock | nolockbuf);
    pcout->_C_fmtfl &= ~(nolock | nolockbuf);
    pcerr->_C_fmtfl &= ~(nolock | nolockbuf);
    pclog->_C_fmtfl &= ~(nolock | nolockbuf);

#endif   // _RWSTD_REENTRANT && !_RWSTD_NO_EXT_REENTRANT_IO && ...


#ifndef _RWSTD_NO_WCHAR_T

    wistream *pwcin =  _INIT (wfilebuf, wistream, wcin,  wcin,  STDIN_FILENO);
    wostream *pwcout = _INIT (wfilebuf, wostream, wcout, wcout, STDOUT_FILENO);
    wostream *pwcerr = _INIT (wfilebuf, wostream, wcerr, wcerr, STDERR_FILENO);
    wostream *pwclog = _INIT (wfilebuf, wostream, wclog, wcerr, STDERR_FILENO);

    // stream objects expected to be arranged in this order
    // must be initialized dynamically (as opposed to statically)
    _RW::__rw_std_streams[4] = _RWSTD_STATIC_CAST (ios_base*, pwcin);
    _RW::__rw_std_streams[5] = _RWSTD_STATIC_CAST (ios_base*, pwcout);
    _RW::__rw_std_streams[6] = _RWSTD_STATIC_CAST (ios_base*, pwcerr);
    _RW::__rw_std_streams[7] = _RWSTD_STATIC_CAST (ios_base*, pwclog);

    // 27.3.2, p2
    pwcin->_C_usr = &_C_usr_data::_C_std_usr_data [1];
    pwcin->_C_usr->_C_tie = pwcout;

    // 27.3.2, p5
    pwcerr->_C_fmtfl |= _RW::__rw_unitbuf;

    pwcin->_C_fmtfl  |= _RW::__rw_sync_stdio;
    pwcout->_C_fmtfl |= _RW::__rw_sync_stdio;
    pwcerr->_C_fmtfl |= _RW::__rw_sync_stdio;
    pwclog->_C_fmtfl |= _RW::__rw_sync_stdio;

#  if     defined (_RWSTD_REENTRANT)             \
      && !defined (_RWSTD_NO_EXT_REENTRANT_IO)   \
      && !defined (_RWSTD_NO_REENTRANT_IO_DEFAULT)

    // override MT-safety setting done in basic_ios<>::init()
    pwcin->_C_fmtfl  &= ~(nolock | nolockbuf);
    pwcout->_C_fmtfl &= ~(nolock | nolockbuf);
    pwcerr->_C_fmtfl &= ~(nolock | nolockbuf);
    pwclog->_C_fmtfl &= ~(nolock | nolockbuf);

#  endif   // _RWSTD_REENTRANT && !_RWSTD_NO_EXT_REENTRANT_IO && ...

#endif   // _RWSTD_NO_WCHAR_T
}


ios_base::Init::~Init ()
{
    if (_RWSTD_ATOMIC_PREDECREMENT (_RW::__rw_ios_initcnt, false))
        return;

    // put all standard iostream objects in unbuffered mode so that
    // output made from dtors of user objects with static storage
    // duration destroyed from this point on (i.e., after the destruction
    // of *this, the last initializer object) is not lost

    // references must not be destroyed
    _RWSTD_ASSERT (&cout && &cerr && &clog);

    const char fmt[] = "exception during std::%s cleanup\n";

    int caught = 0;

    _TRY {
        _OBJECT (ostream, cout).rdbuf ()->pubsetbuf (0, 0);
    }
    _CATCH (...) {
        fprintf (stderr, fmt, "cout");
        ++caught;
    }
    _TRY {
        _OBJECT (ostream, cerr).rdbuf ()->pubsetbuf (0, 0);
    }
    _CATCH (...) {
        fprintf (stderr, fmt, "cerr");
        ++caught;
    }
    _TRY {
        _OBJECT (ostream, clog).rdbuf ()->pubsetbuf (0, 0);
    }
    _CATCH (...) {
        fprintf (stderr, fmt, "clog");
        ++caught;
    }

#ifndef _RWSTD_NO_WCHAR_T

    _RWSTD_ASSERT (&wcout && &wcerr && &wclog);

    _TRY {
        _OBJECT (wostream, wcout).rdbuf ()->pubsetbuf (0, 0);
    }
    _CATCH (...) {
        fprintf (stderr, fmt, "wcout");
        ++caught;
    }
    _TRY {
        _OBJECT (wostream, wcerr).rdbuf ()->pubsetbuf (0, 0);
    }
    _CATCH (...) {
        fprintf (stderr, fmt, "wcerr");
        ++caught;
    }
    _TRY {
        _OBJECT (wostream, wclog).rdbuf ()->pubsetbuf (0, 0);
    }
    _CATCH (...) {
        fprintf (stderr, fmt, "wclog");
        ++caught;
    }

#endif   // _RWSTD_NO_WCHAR_T

    if (caught)
        abort ();
}


}   // namespace std
