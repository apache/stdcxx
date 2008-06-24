/***************************************************************************
 *
 * exception.cpp - Source for the Standard Library exception classes
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

#if    3 == __GNUG__ && 3 > __GNUC_MINOR__ \
    || 3 == __GNUC_MINOR__ && 1 >__GNUC_PATCHLEVEL__

  // working around a gcc bug (PR #29570)
#  include <rw/_config.h>
#  ifndef _RWSTD_NO_EXTERN_TEMPLATE
#    define _RWSTD_NO_EXTERN_TEMPLATE
#  endif
#endif   // gcc >= 3.0 && gcc < 3.3.1

#include <rw/_defs.h>

#ifndef _RWSTD_EDG_ECCP
#  include <stdarg.h>
#else
   // use "special" magic for the EDG eccp demo
#  include <ansi/_cstdarg.h>

_USING (_STD::va_list);

#endif   // EDG eccp demo

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <loc/_locale.h>
#include <loc/_messages.h>

#include <new>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include <rw/_error.h>
#include <rw/_iosfailure.h>
#include <rw/_mutex.h>

#include "podarray.h"   // for __rw_aligned_buffer


#ifdef _MSC_VER
   // MSVC's libc prepends an underscore
#  define vsnprintf   _vsnprintf
#  undef  _RWSTD_NO_VSNPRINTF
#endif

#if defined (_RWSTD_NO_VSNPRINTF) && !defined (_RWSTD_NO_VSNPRINTF_IN_LIBC)

// vsnprintf() not declared, but it is defined in libc
extern "C" _RWSTD_DLLIMPORT int
vsnprintf (char*, _RWSTD_SIZE_T, const char*, va_list) _LIBC_THROWS();

// use vsnprintf()
#  undef _RWSTD_NO_VSNPRINTF
#endif // _NO_VSNPRINTF && !_NO_VSNPRINTF_IN_LIBC || !_RWSTD_NO_PURE_C_HEADERS


#ifndef _RWSTD_NO_VSNPRINTF
#  define _RWSTD_VSNPRINTF(buf, size, fmat, va)  vsnprintf (buf, size, fmat, va)
#else
    // unsafely work around the lack of vsnprintf by using vsprintf()
#  define _RWSTD_VSNPRINTF(buf, ignore, fmat, va)  vsprintf (buf, fmat, va)
#endif   // _RWSTD_NO_VSNPRINTF


// declare global versions of exception handlers; will be
// used only if the functions aren't defined in namespace std
// in the compiler support library

void terminate () _RWSTD_GNUC_ATTRIBUTE ((__noreturn__));

_STD::terminate_handler
set_terminate (_STD::terminate_handler) _THROWS (());

_STD::unexpected_handler
set_unexpected (_STD::unexpected_handler) _THROWS (());

void unexpected ()  _RWSTD_GNUC_ATTRIBUTE ((__noreturn__));

#ifdef _MSC_VER
   // MSVC 7 and 8 reliably define __uncaught_exception
   // uncaught_exception() only seems to be defined by some compiler
   // magic that requires the compiler's native C++ Standard library
   // headers
#  undef _RWSTD_NO_GLOBAL_UNCAUGHT_EXCEPTION
#  define UNCAUGHT_EXCEPTION   __uncaught_exception
#endif   // MSVC

#ifndef UNCAUGHT_EXCEPTION
#  define UNCAUGHT_EXCEPTION   uncaught_exception
#endif   // UNCAUGHT_EXCEPTION

_RWSTD_DLLIMPORT bool UNCAUGHT_EXCEPTION () _THROWS (());


_RWSTD_NAMESPACE (std) { 

// define functions in namespace std to wrap any globals defined
// defined in the compiler support library

// functions are outlined to prevent linker errors in optimized shared builds

// guard definitions when compiling with gcc with -fno-honor-std   
#ifndef _RWSTD_NO_HONOR_STD
    
#  if defined (_RWSTD_NO_STD_TERMINATE)

// 18.6.3.3
_RWSTD_EXPORT void terminate ()
{
#    ifndef _RWSTD_NO_GLOBAL_TERMINATE

    ::terminate ();

#    else   // if defined (_RWSTD_NO_GLOBAL_TERMINATE)

    abort ();

#    endif   // _RWSTD_NO_GLOBAL_TERMINATE

}

#  endif   // _RWSTD_NO_STD_TERMINATE


#  if defined (_RWSTD_NO_STD_SET_TERMINATE)

// 18.6.3.2
_RWSTD_EXPORT terminate_handler
set_terminate (terminate_handler handler) _THROWS (())
{
#    ifndef _RWSTD_NO_GLOBAL_SET_TERMINATE

    return ::set_terminate (handler);

#    else   // if defined (_RWSTD_NO_GLOBAL_SET_TERMINATE)

    static terminate_handler current;

    _RWSTD_MT_STATIC_GUARD (terminate_handler);

    terminate_handler save = current;

    current = handler;

    return save;

#    endif   // _RWSTD_NO_GLOBAL_SET_TERMINATE

}

#  endif   // _RWSTD_NO_STD_SET_TERMINATE


#  if defined (_RWSTD_NO_STD_SET_UNEXPECTED)

// 18.6.2.3
_RWSTD_EXPORT unexpected_handler
set_unexpected (unexpected_handler handler)  _THROWS (())
{
#    ifndef _RWSTD_NO_GLOBAL_SET_UNEXPECTED

    return ::set_unexpected (handler);

#    else   // if defined (_RWSTD_NO_GLOBAL_SET_UNEXPECTED)

    static unexpected_handler current;

    _RWSTD_MT_STATIC_GUARD (unexpected_handler);

    unexpected_handler save = current;

    current = handler;

    return save;

#    endif   // _RWSTD_NO_GLOBAL_SET_UNEXPECTED

}

#  endif   // _RWSTD_NO_STD_SET_UNEXPECTED


#  if defined (_RWSTD_NO_STD_UNEXPECTED)

// 18.6.2.4
_RWSTD_EXPORT void unexpected ()
{
#    ifndef _RWSTD_NO_GLOBAL_UNEXPECTED

    ::unexpected ();

#    else   // if defined (_RWSTD_NO_GLOBAL_UNEXPECTED)

    abort ();

#    endif   // _RWSTD_NO_GLOBAL_UNEXPECTED

}

#  endif   // _RWSTD_NO_STD_UNEXPECTED


#  if defined (_RWSTD_NO_STD_UNCAUGHT_EXCEPTION)

// 18.6.4
_RWSTD_EXPORT bool uncaught_exception () _THROWS (())
{
#    ifndef _RWSTD_NO_GLOBAL_UNCAUGHT_EXCEPTION

    return ::UNCAUGHT_EXCEPTION ();

#    else   // if defined (_RWSTD_NO_GLOBAL_UNCAUGHT_EXCEPTION)

#      ifndef _RWSTD_NO_EXCEPTIONS

    return true;

#      else   // if defined (_RWSTD_NO_EXCEPTIONS)

    return false;

#      endif   // _RWSTD_NO_EXCEPTIONS

#    endif   // _RWSTD_NO_GLOBAL_UNCAUGHT_EXCEPTION
}

#  endif   // _RWSTD_NO_STD_UNCAUGHT_EXCEPTION

#endif   // _RWSTD_NO_HONOR_STD

}   // namespace std


#if defined _RWSTD_EXCEPTION_INCLUDED

// define exception and bad_exception members only if our own
// implementation's <exception> header (as opposed to the compiler's
// native C++ Standard library) has been included; SunPro always
// #includes its own, regardless of compiler options

//**************************************************************************
// class exception members

#  if    !defined (_RWSTD_NO_STD_EXCEPTION)    \
      || !defined (_RWSTD_NO_RUNTIME_IN_STD)   \
      || 2 == __GNUG__ && !defined (_RWSTD_NO_HONOR_STD)

_RWSTD_NAMESPACE (std) { 

#  endif   // !_RWSTD_NO_STD_EXCEPTION || !_RWSTD_NO_RUNTIME_IN_STD ...


#ifdef _RWSTD_NO_EXCEPTION_DEFAULT_CTOR

exception::exception () _THROWS (())
{
#if defined (_MSC_VER)

    _C_name = 0;
    _C_flag = 0;

#elif defined (__INTEL_COMPILER)

    _C_name = "";

#endif   // _MSC_VER
}

#endif   // _RWSTD_NO_EXCEPTION_DEFAULT_CTOR

#ifdef _RWSTD_NO_EXCEPTION_COPY_CTOR

exception::exception (const exception&) _THROWS (())
{
#if defined (_MSC_VER)

    _C_name = 0;
    _C_flag = 0;

#elif defined (__INTEL_COMPILER)

    _C_name = "";

#endif   // _MSC_VER
}

#endif   // _RWSTD_NO_EXCEPTION_COPY_CTOR

#ifdef _RWSTD_NO_EXCEPTION_DTOR

exception::~exception () _THROWS (())
{
    // empty
}

#endif   // _RWSTD_NO_EXCEPTION_DTOR

#ifdef _RWSTD_NO_EXCEPTION_ASSIGNMENT

exception&
exception::operator= (const exception&) _THROWS (())
{
    return *this;
}

#endif   // _RWSTD_NO_EXCEPTION_ASSIGNMENT

#ifdef _RWSTD_NO_EXCEPTION_WHAT

const char* exception::what () const _THROWS (())
{
    return _RWSTD_ERROR_EXCEPTION;
}

#endif   // _RWSTD_NO_EXCEPTION_WHAT


#  if    !defined (_RWSTD_NO_STD_EXCEPTION)    \
      || !defined (_RWSTD_NO_RUNTIME_IN_STD)   \
      || 2 == __GNUG__ && !defined (_RWSTD_NO_HONOR_STD)

}   // namespace std

#  endif   // !_RWSTD_NO_STD_EXCEPTION || !_RWSTD_NO_RUNTIME_IN_STD ...


//**************************************************************************
// class bad_exception members

#  if    !defined (_RWSTD_NO_STD_BAD_EXCEPTION)    \
      || !defined (_RWSTD_NO_RUNTIME_IN_STD)   \
      || 2 == __GNUG__ && !defined (_RWSTD_NO_HONOR_STD)

_RWSTD_NAMESPACE (std) { 

#  endif   // !_RWSTD_NO_STD_BAD_EXCEPTION || !_RWSTD_NO_RUNTIME_IN_STD ...


#ifdef _RWSTD_NO_BAD_EXCEPTION_DEFAULT_CTOR

bad_exception::bad_exception () _THROWS (())
    : exception ()
{
}

#endif   // _RWSTD_NO_BAD_EXCEPTION_DEFAULT_CTOR

#ifdef _RWSTD_NO_BAD_EXCEPTION_COPY_CTOR

bad_exception::bad_exception (const bad_exception &rhs) _THROWS (())
    : exception (rhs)
{
}

#endif   // _RWSTD_NO_BAD_EXCEPTION_COPY_CTOR

#ifdef _RWSTD_NO_BAD_EXCEPTION_DTOR

bad_exception::~bad_exception () _THROWS (())
{
    // empty
}

#endif   // _RWSTD_NO_BAD_EXCEPTION_DTOR

#ifdef _RWSTD_NO_BAD_EXCEPTION_ASSIGNMENT

bad_exception&
bad_exception::operator= (const bad_exception &rhs) _THROWS (())
{
    exception::operator= (rhs);

    return *this;
}

#endif   // _RWSTD_NO_BAD_EXCEPTION_ASSIGNMENT

#ifdef _RWSTD_NO_BAD_EXCEPTION_WHAT

const char* bad_exception::what () const _THROWS (())
{
    return _RWSTD_ERROR_BAD_EXCEPTION;
}

#endif   // _RWSTD_NO_BAD_EXCEPTION_WHAT


#  if    !defined (_RWSTD_NO_STD_BAD_EXCEPTION)    \
      || !defined (_RWSTD_NO_RUNTIME_IN_STD)   \
      || 2 == __GNUG__ && !defined (_RWSTD_NO_HONOR_STD)

}   // namespace std

#  endif   // !_RWSTD_NO_STD_BAD_EXCEPTION || !_RWSTD_NO_RUNTIME_IN_STD ...

#endif   // _RWSTD_EXCEPTION_INCLUDED

/**************************************************************************/

_RWSTD_NAMESPACE (__rw) {


static _RWSTD_THREAD char
__rw_what_buf [256];

static _RWSTD_THREAD int
__rw_what_refcnt;

// free memory buffer allocated in __rw_vfmtwhat()
_RWSTD_EXPORT void __rw_free_what_buf (char* buf)
{
    if (__rw_what_buf == buf) {
        _RWSTD_ASSERT (0 < __rw_what_refcnt);
        _RWSTD_THREAD_PREDECREMENT (__rw_what_refcnt, false);
    }
    else
        delete[] buf;
}

// allocate a char array and format it sprintf-style
// caller responsible for calling delete[] on returned pointer
// unless a non-zero buffer of non-zero size is passed in
static char*
__rw_vfmtwhat (char          *buf,      // (allocate if 0)
               _RWSTD_SIZE_T  bufsize,  // buffer size (0 if none)
               const char    *fmat,     // printf-style fmat string
               va_list        va)       // variable arguments
{
    _RWSTD_ASSERT (0 != fmat);
    _RWSTD_ASSERT (0 == buf || bufsize);

#ifndef _RWSTD_NO_VSNPRINTF
    // initial size of buffer, may be increased as needed
    _RWSTD_SIZE_T size = bufsize ? bufsize : 256;
#else
    // unsafely work around the lack of vsnprintf by using vsprintf()
    _RWSTD_SIZE_T size = bufsize ? bufsize : strlen (fmat);
    size = size < 1024 ? 1024 : size * 2;
#endif   // _RWSTD_NO_VSNPRINTF

    if (!buf) {

        if (1 == _RWSTD_THREAD_PREINCREMENT (__rw_what_refcnt, false)) {
            buf = __rw_what_buf;
        }
        else {
            buf = new char [size];

            _RWSTD_THREAD_PREDECREMENT (__rw_what_refcnt, false);
        }
    }

    for ( ; ; ) {

        // vsnprintf() return value
        //
        // 7.19.6.12, p3 of C99:
        //
        // The vsnprintf function returns the number of characters that
        // would have been written had n been sufficiently large, not
        // counting the terminating null character, or a neg ative value
        // if an encoding error occurred. Thus, the null-terminated output
        // has been completely written if and only if the returned value
        // is nonnegative and less than n.
        //
        // HP-UX and Windows are known to return a negative value for
        // buffer overflow.

        // On IRIX and Tru64 UNIX on overflow the function returns
        // the number of bytes actually written to the buffer which
        // makes it indistinguishable from a successful return value
        // for a completely full buffer.

        // Others (AIX, Linux, and Solaris) follow the C99 requirement.

        int nwrote;

        // copy va to a temporary and use it in each iteration
        // of the loop since it may be modified by the call to
        // vsnprintf() (e.g., on PowerPC/Linux)

        va_list tmp_va;
        _RWSTD_VA_COPY (tmp_va, va);

        nwrote = _RWSTD_VSNPRINTF (buf, size, fmat, tmp_va);

        va_end (tmp_va);

        if (0 > nwrote) {

#ifdef _RWSTD_NO_VSNPRINTF_VFLOW_VALID

            // set the number of bytes written to 0 to indicate
            // that the buffer isn't big enough without actually
            // telling how big it needs to be
            nwrote = 0;

#else   // if !defined (_RWSTD_NO_VSNPRINTF_VFLOW_VALID)

            // conversion error
            *buf = '\0';
            break;

#endif   // _RWSTD_NO_VSNPRINTF_VFLOW_VALID

        }

#ifdef _RWSTD_NO_VSNPRINTF_VFLOW_VALID

        if (nwrote >= int (size) - 1) {
            // indicate insufficient buffer size
            nwrote = 0;
        }

#endif   // _RWSTD_NO_VSNPRINTF_VFLOW_VALID

        if (0 == nwrote)
            size *= 2;
        else if (int (size) <= nwrote)
            size = nwrote + 1U;
        else
            break;

        // fail if a buffer of insufficient size was passed in
        if (bufsize)
            return 0;

        __rw_free_what_buf (buf);
        buf = new char [size];
    }

    return buf;
}


static
char* __rw_vfmtwhat (char          *buf,      // buffer (allocate if 0)
                     _RWSTD_SIZE_T  bufsize,  // size of buffer or 0
                     int            id,       // catalog id of fmt string
                     va_list        va)       // variable arguments
{
    typedef _STD::messages<char> _Msgs;

    static const char     *__fname  =  0;   // catalog file
    static int             __catset =  1;   // catalog set
    static _Msgs::catalog  __cat    = -1;   // catalog id

    // facet must never be destroyed
    static __rw_aligned_buffer<_Msgs> msgs;

    // try to open catalog if first time through only
    if (!__fname) {

        _RWSTD_MT_STATIC_GUARD (void);

        // double-check to prevent a race opening a catalog more than once
        if (!__fname) {

            // get catalog name (absolute or relative) from the environment
            char *__fn = getenv (_RWSTD_ERROR_ENVVAR);

            // use default hardcoded name if variable doesn't exist
            if (!__fn) {
                // copy literal into static buffer (will be modified)
                static char buffer[] = _RWSTD_ERROR_CATALOG;
                __fn = buffer;
            }

            // catalog name may optionally be followed by a `:' and a set number
            char *__set = strrchr (__fn, ':');
            if (__set) {
                *__set++ = '\0';
                if (   1 != sscanf (__set, "%d", &__catset)
                    || __catset <= 0)
                    __catset = 1;
            }

            new (msgs._C_store ()) _Msgs;

            // assign `fname' _after_ opening catalog to prevent a race
            // between calling messages<>::open() and messages<>::get()
            // always use the classic locale, not the current snapshot
            __cat = msgs._C_data ()->open (__fn, _STD::locale::classic ());
            __fname = __fn;
        }
    }

    if (-1 == __cat)
        return 0;   // indicate a falure to open catalog

    _RWSTD_MT_STATIC_GUARD (void);

    // retrieve error message from catalog and use it to format args
    _STD::string fmat = msgs._C_data ()->get (__cat, __catset, id, "");

    if (fmat.empty ())
        return 0;   // failure to retrieve message

    return __rw_vfmtwhat (buf, bufsize, fmat.data (), va);
}

#ifdef _RWSTD_REENTRANT

extern const char* const __rw_mutex_exception =
    "Thread synchronization exception.";

#else   // if !defined (_RWSTD_REENTRANT)

// dummy empty object (not an actual mutex) - allows lib to "look"
// the same in single-threaded mode as in multi-threaded mode
__rw_mutex _RWSTD_EXPORT __rw_synchronized::_C_mutex;

#endif   // _RWSTD_REENTRANT


static void __rw_throw_exception (int id, char *what)
{
#ifndef _RWSTD_NO_EXCEPTIONS

    // exception, bad_exception, bad_cast, and bad_alloc
    // each or may not be defined in namespace std; avoid
    // complicated preprocessor logic by introducing the
    // whole namespace into scope
    _USING (namespace std);

    switch (id) {
    case _RWSTD_ERROR_FIRST + 1:
        throw exception ();

    case _RWSTD_ERROR_FIRST + 2:
        throw bad_exception ();

    case _RWSTD_ERROR_FIRST + 3:

#ifndef _RWSTD_BAD_ALLOC
#  ifndef _RWSTD_NO_RUNTIME_IN_STD
        throw std::bad_alloc ();
#  else
        throw bad_alloc ();
#  endif   // _RWSTD_NO_RUNTIME_IN_STD
#else   // if defined (_RWSTD_BAD_ALLOC)
        throw _RWSTD_BAD_ALLOC ();
#endif   // _RWSTD_BAD_ALLOC

    case _RWSTD_ERROR_BAD_CAST:
        throw bad_cast ();

    case _RWSTD_ERROR_LOGIC_ERROR:
        throw (_STD::logic_error&)_STD::logic_error ()._C_assign (what, 0);

    case _RWSTD_ERROR_DOMAIN_ERROR:
        throw (_STD::domain_error&)_STD::domain_error ()._C_assign (what, 0);

    case _RWSTD_ERROR_INVALID_ARGUMENT:
        throw (_STD::invalid_argument&)
            _STD::invalid_argument ()._C_assign (what, 0);

    case _RWSTD_ERROR_LENGTH_ERROR:
        throw (_STD::length_error&)_STD::length_error ()._C_assign (what, 0);

    case _RWSTD_ERROR_OUT_OF_RANGE:
        throw (_STD::out_of_range&)_STD::out_of_range ()._C_assign (what, 0);

    case _RWSTD_ERROR_RUNTIME_ERROR:
        throw (_STD::runtime_error&)
            _STD::runtime_error ()._C_assign (what, 0);

    case _RWSTD_ERROR_RANGE_ERROR:
        throw (_STD::range_error&)_STD::range_error ()._C_assign (what, 0);

    case _RWSTD_ERROR_OVERFLOW_ERROR:
        throw (_STD::overflow_error&)
            _STD::overflow_error ()._C_assign (what, 0);

    case _RWSTD_ERROR_UNDERFLOW_ERROR:
        throw (_STD::underflow_error&)
            _STD::underflow_error ()._C_assign (what, 0);

#ifndef _RWSTD_NO_EXT_FAILURE

    case _RWSTD_ERROR_FAILBIT_SET:
        throw (__rw_failbit_set&)
            __rw_failbit_set ()._C_assign (what, 0);

    case _RWSTD_ERROR_BADBIT_SET:
        throw (__rw_badbit_set&)
            __rw_badbit_set ()._C_assign (what, 0);

    case _RWSTD_ERROR_EOFBIT_SET:
        throw (__rw_eofbit_set&)
            __rw_eofbit_set ()._C_assign (what, 0);

#else   // if defined (_RWSTD_NO_EXT_FAILURE)

    case _RWSTD_ERROR_FAILBIT_SET:
    case _RWSTD_ERROR_BADBIT_SET:
    case _RWSTD_ERROR_EOFBIT_SET:
        // fall through...

#endif   // _RWSTD_NO_EXT_FAILURE

    case _RWSTD_ERROR_IOSTATE_BIT_SET:
        throw (__rw_failure&)
            __rw_failure ()._C_assign (what, 0);

    case _RWSTD_ERROR_FACET_NOT_FOUND:
    case _RWSTD_ERROR_LOCALE_BAD_CAT:
    case _RWSTD_ERROR_LOCALE_BAD_NAME:
    case _RWSTD_ERROR_LOCALE_ERROR_NAME:
    case _RWSTD_ERROR_CODECVT:
    case _RWSTD_ERROR_BAD_POINTER:
        throw (_STD::runtime_error&)
            _STD::runtime_error ()._C_assign (what, 0);
    }

    __rw_free_what_buf (what);

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (what) {
        fprintf (stderr,"Exception: %s.\n", what);
        __rw_free_what_buf (what);
    }
    else {
        const char *__str;
        switch (id) {
        case _RWSTD_ERROR_FIRST + 0: __str = _RWSTD_ERROR_EXCEPTION; break;
        case _RWSTD_ERROR_FIRST + 1: __str = _RWSTD_ERROR_BAD_EXCEPTION; break;
        case _RWSTD_ERROR_FIRST + 2:  __str = _RWSTD_ERROR_BAD_ALLOC; break;
        default: __str = "unknown exception";
        }

        fprintf (stderr, "Exception: %s.\n", __str);
    }

    abort ();

#endif   // _RWSTD_NO_EXCEPTIONS
}

// may be assigned to a user-defined handler (e.g., to prevent
// the library from throwing exceptions or to implement logging)
_RWSTD_EXPORT void (*__rw_throw_proc)(int, char*) = __rw_throw_exception;


_RWSTD_EXPORT void __rw_throw (int id, ...)
{
    if (!__rw_throw_proc)
        return;

    // do not allocate or format string for exception,
    // bad_alloc, bad_cast, bad_exception, or bad_cast
    if (id <= _RWSTD_ERROR_BAD_CAST) {
        __rw_throw_proc (id, 0);
    }
    else {
        va_list va;
        va_start (va, id);

#ifdef va_copy

        // copy va to a temporary and use it in the first call
        // to __rw_vfmtwhat() since it may be modified by the
        // call and rendered invalid for subsequent use (e.g.,
        // on PowerPC/Linux)

        va_list tmp_va;
        va_copy (tmp_va, va);

        // try to format a string (look up format in a catalog)...
        char *what = _RW::__rw_vfmtwhat (0, 0, id, tmp_va);

        va_end (tmp_va);

#else   // if !defined (va_copy)

        char *what = _RW::__rw_vfmtwhat (0, 0, id, va);

#endif   // va_copy


        if (!what) {
            // obtain an index into table of error strings
            id -= _RWSTD_ERROR_FIRST;

            static const char* const errors[] = { _RWSTD_ERROR_STRINGS };

            // make sure index is valid
            if (id < 0 || id >= int (sizeof errors / sizeof *errors))
                id = 0;

            // use error string to format
            what = _RW::__rw_vfmtwhat (0, 0, errors [id], va);
        }

        // end va list before throwing
        va_end (va);

        // throw_proc takes ownership of allocated string
        __rw_throw_proc (id, what);
    }
}


__rw_exception::__rw_exception () _THROWS (())
    : _STD::exception (), _C_what (0) { }


__rw_exception::__rw_exception (const __rw_exception &rhs)
    : _STD::exception (rhs), _C_what (0)
{
    _C_assign (rhs.what ());
}

  
/* explicit */ __rw_exception::__rw_exception (const char *str)
    : _STD::exception (), _C_what (0)
{
    _C_assign (str);
}


// outlined to avoid circular dependency on <string>
__rw_exception::__rw_exception (const _STD::string &whatstr)
    : _C_what (0)
{
    _C_assign (whatstr.c_str (), whatstr.length ());
}


// outlined to avoid functional compatibility issues
/* virtual */ __rw_exception::~__rw_exception () _THROWS (())
{
    __rw_free_what_buf (_C_what);

#ifdef _C_dummy_what
    // zero out dummy member of the base exception class
    // to prevent the base dtor from trying to do the same
    // (MSVC 6.0 only)
    _C_dummy_what = 0;
#endif   // _C_dummy_what
    
}


// empty exception specification necessary in order to preserve
// the no-exception guarantee provided by std::exception to
// derived classes (e.g., logic_error)
__rw_exception& __rw_exception::
operator= (const __rw_exception &__rhs) _THROWS (())
{
    _C_assign (__rhs.what ());

    return *this;
}


/* virtual */ const char* __rw_exception::
what () const _THROWS (())
{
    // `what' may be 0 indicating the empty string as an optimization
    return _C_what ? _C_what : "";
}


// assigns the argument to our string by allocating sufficient space
// and copying the string; if `what' is null or `what' points to the
// empty string (""), deletes buffer and assigns 0
// if `what' is non-null and non-empty but `len' is 0, simply assigns
// `what' to the internal pointer and takes ownership of it
__rw_exception& __rw_exception::
_C_assign (const char *whatstr, size_t len /* = ~0 */)
{
    if (whatstr != _C_what) {
        char *tmp = 0;

        if (whatstr && *whatstr) {

            if (whatstr == __rw_what_buf) {
                if (len)
                    _RWSTD_THREAD_PREINCREMENT (__rw_what_refcnt, false);
                tmp = __rw_what_buf;
            }
            else {
                if (_RWSTD_SIZE_MAX == len)
                    len = strlen (whatstr);

                if (len) {
                    // allocate own buffer and copy string
                    tmp = new char [len + 1];
                    memcpy (tmp, whatstr, len + 1);
                }
                else {
                    // special case: do not allocate, just use passed in pointer
                    tmp = _RWSTD_CONST_CAST (char*, whatstr);
                }
            }
        }

        __rw_free_what_buf (_C_what);
        _C_what = tmp;
    }
    return *this;
}


// trivial dtors outlined to reduce code bloat caused by compilers that
// emit the vtbl in every translation unit containing the definition of
// the class

// virtual dtor
/* virtual */ __rw_failure::~__rw_failure () _THROWS (())
{
    // no-op
}


/* virtual */ __rw_badbit_set::~__rw_badbit_set () _THROWS (())
{
    // no-op
}


/* virtual */ __rw_eofbit_set::~__rw_eofbit_set () _THROWS (())
{
    // no-op
}


/* virtual */ __rw_failbit_set::~__rw_failbit_set () _THROWS (())
{
    // no-op
}


}   // namespace __rw
