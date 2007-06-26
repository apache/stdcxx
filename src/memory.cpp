 /***************************************************************************
 *
 * memory.cpp - source for the C++ Standard Library
 *              dynamic memory management functions
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#include <rw/_error.h>
#include <rw/_mutex.h>

#include <new>
#include <stdlib.h>

#include <rw/_defs.h>


_RWSTD_NAMESPACE (__rw) {


_RWSTD_EXPORT void*
__rw_allocate (_RWSTD_SIZE_T nbytes, int /* = 0 */)
{
    void *ptr = 0;

    _TRY {

#ifndef _RWSTD_NO_NEW_OFLOW_SAFE

        ptr = ::operator new (nbytes);

#else   // if defined (_RWSTD_NO_NEW_OFLOW_SAFE)

        // operator new() doesn't check for argument overflow,
        // check before calling it and fail if `n' looks too big

        if (nbytes < (_RWSTD_SIZE_T)-1 - 256)
            ptr = ::operator new (nbytes);

#endif   // _RWSTD_NO_NEW_OFLOW_SAFE

    }
    _CATCH (...) {
        // prevent bad_alloc raised by the compiler
    }

    // fix broken ::operator new ()
    if (0 == ptr)
        _RW::__rw_throw (_RWSTD_ERROR_FIRST + 3 /* == bad_alloc */);

    return ptr;
}


_RWSTD_EXPORT void
__rw_deallocate (void *p, _RWSTD_SIZE_T, int /* = 0 */)
{
    ::operator delete (p);
}


}   // namespace __rw


#if defined _RWSTD_NEW_INCLUDED

// define operator new and delete and bad_alloc members only if our own
// implementation's <new> header (as opposed to the compiler's native C++
// Standard library) has been included; SunPro always #includes its own,
// regardless of compiler options


// The following operators are replaceable; provide definitions if they're
// missing in the compiler support library, and _RWSTD_NO_EXT_OPERATOR_NEW
// is not defined (i.e., not in strict ansi mode). This means that they will
// no longer be replaceable, but at least they will be callable without the
// user providing the definitions
//
#ifndef _RWSTD_NO_EXT_OPERATOR_NEW

#  ifdef _RWSTD_NO_OPERATOR_NEW_NOTHROW

_RWSTD_EXPORT void*
operator new (_RWSTD_SIZE_T __size, const _STD::nothrow_t&) _NEW_THROWS (())
{
    return malloc (__size);
}

#  endif   // _RWSTD_NO_OPERATOR_NEW_NOTHROW


#  if    defined (_RWSTD_NO_OPERATOR_DELETE_NOTHROW) \
     && !defined (_RWSTD_NO_PLACEMENT_DELETE)

_RWSTD_EXPORT void
operator delete (void* __ptr, const _STD::nothrow_t&) _NEW_THROWS (())
{
    free (__ptr);
}

#  endif   // _RWSTD_NO_OPERATOR_DELETE_NOTHROW && _RWSTD_NO_PLACEMENT_DELETE

#  ifdef _RWSTD_NO_OPERATOR_NEW_ARRAY

_RWSTD_EXPORT void*
operator new[] (_RWSTD_SIZE_T __size) _NEW_THROWS ((_RWSTD_BAD_ALLOC))
{
    return ::operator new (__size);
}

#  endif   // _RWSTD_NO_OPERATOR_NEW_ARRAY

#  ifdef _RWSTD_NO_OPERATOR_NEW_ARRAY_NOTHROW

_RWSTD_EXPORT void*
operator new[] (_RWSTD_SIZE_T __size, const _STD::nothrow_t&) _NEW_THROWS (())
{
    _TRY {
        return ::operator new (__size);
    }
    _CATCH (...) {
        void *__nil = 0;    // prevent a g++ 2.95.2 warning
        return __nil;       // about operator new returning 0
    }
}

#  endif   // _RWSTD_NO_OPERATOR_NEW_ARRAY_NOTHROW


#  ifdef _RWSTD_NO_OPERATOR_DELETE_ARRAY

_RWSTD_EXPORT void
operator delete[] (void* __ptr) _NEW_THROWS (())
{
    ::operator delete (__ptr);
}

#  endif   // _RWSTD_NO_OPERATOR_DELETE_ARRAY


#  if     defined (_RWSTD_NO_OPERATOR_DELETE_ARRAY_NOTHROW) \
      && !defined (_RWSTD_NO_PLACEMENT_DELETE)

_RWSTD_EXPORT void
operator delete[] (void* __ptr, const _STD::nothrow_t&) _NEW_THROWS (())
{
    ::operator delete[] (__ptr);  // ::operator delete[] doesn't throw
}

#  endif   // _RWSTD_NO_OPERATOR_DELETE_NOTHROW && _RWSTD_NO_PLACEMENT_DELETE

#endif   // _RWSTD_NO_EXT_OPERATOR_NEW

//**************************************************************************

#ifdef _MSC_VER

typedef int (*__rw_new_handler_t)(_RWSTD_SIZE_T);

_RWSTD_DLLIMPORT __rw_new_handler_t _set_new_handler (__rw_new_handler_t);

_RWSTD_NAMESPACE (__rw) {

static _STD::new_handler __rw_new_handler /* = 0 */;

static int __rw_new_handler_imp (_RWSTD_SIZE_T)
{
    _RWSTD_ASSERT (0 != __rw_new_handler);

    __rw_new_handler ();

    return 1;
}

}   // namespace __rw


_RWSTD_NAMESPACE (std) {


// 18.4.2.3 
_RWSTD_EXPORT new_handler
set_new_handler (new_handler handler) _THROWS (())
{
    // MSVC's set_new_handler(h) calls assert(h == 0)
    // use the _set_new_handler() interface instead

    _RWSTD_MT_STATIC_GUARD (new_handler);

    new_handler save = _RW::__rw_new_handler;

    _RW::__rw_new_handler = handler;

    _set_new_handler (_RW::__rw_new_handler_imp);

    return save;
}

}   // namespace std

#endif   // _MSC_VER

//**************************************************************************


_RWSTD_NAMESPACE (std) { 

// define std::nothrow only if the symbol doesn't appear
// in the compiler support library to prevent linker errors
// (with gcc -fno-honor-std, don't define if global version exists)
// +----------+-------------------+--------------------+----------------+
// | define?  | _NO_STD_NOTHROW_T | _NO_GLOBAL_NOTHROW | _NO_HONOR_STD  |
// +----------+-------------------+--------------------+----------------+
// |    yes   |      defined      |        defined     |    defined     |
// +----------+-------------------+--------------------+----------------+
// |    yes   |      defined      |        defined     |   !defined     |
// +----------+-------------------+--------------------+----------------+
// |    no    |      defined      |       !defined     |    defined     |
// +----------+-------------------+--------------------+----------------+
// |    no    |      defined      |       !defined     |   !defined     |
// +----------+-------------------+--------------------+----------------+
// |    no    |     !defined      |        defined     |    defined     |
// +----------+-------------------+--------------------+----------------+
// |    no    |     !defined      |        defined     |   !defined     |
// +----------+-------------------+--------------------+----------------+
// |    no    |     !defined      |       !defined     |    defined     |
// +----------+-------------------+--------------------+----------------+
// |    no    |     !defined      |       !defined     |   !defined     |
// +----------+-------------------+--------------------+----------------+

#if    defined (_RWSTD_NO_STD_NOTHROW)           \
    && (    defined (_RWSTD_NO_GLOBAL_NOTHROW)   \
        || !defined (_RWSTD_NO_HONOR_STD))

#  if __HP_aCC != 33000 && __HP_aCC != 33100

// HP aCC 3.30 fails to emit data symbols for extern consts
_RWSTD_EXPORT extern const nothrow_t nothrow = nothrow_t ();

#  endif   // __HP_aCC

#endif   // _RWSTD_NO_STD_NOTHROW && _RWSTD_NO_GLOBAL_NOTHROW || ...

}   // namespace std

//**************************************************************************

// class bad_alloc members

#  if    !defined (_RWSTD_NO_STD_BAD_ALLOC)    \
      || !defined (_RWSTD_NO_RUNTIME_IN_STD)

_RWSTD_NAMESPACE (std) { 

#  endif   // !_RWSTD_NO_STD_BAD_ALLOC || !_RWSTD_NO_RUNTIME_IN_STD ...


#  ifdef _RWSTD_NO_BAD_ALLOC_DEFAULT_CTOR

bad_alloc::
bad_alloc () _THROWS (())
    : exception ()
{
    // empty
}

#  endif   // _RWSTD_NO_BAD_ALLOC_DEFAULT_CTOR

#  ifdef _RWSTD_NO_BAD_ALLOC_COPY_CTOR

bad_alloc::
bad_alloc (const bad_alloc &rhs) _THROWS (())
    : exception (rhs)
{
    // empty
}

#  endif   // _RWSTD_NO_BAD_ALLOC_COPY_CTOR

#  ifdef _RWSTD_NO_BAD_ALLOC_DTOR
#    if !__INTEL_COMPILER || __INTEL_COMPILER > 800

/* virtual */ bad_alloc::
~bad_alloc () _THROWS (())
{
    // empty
}
#    endif   // Intel C++ > 8.0
#  endif   // _RWSTD_NO_BAD_ALLOC_DTOR

#  ifdef _RWSTD_NO_BAD_ALLOC_ASSIGNMENT

bad_alloc& bad_alloc::
operator= (const bad_alloc &rhs) _THROWS (())
{
    exception::operator= (rhs);

    return *this;
}

#  endif   // _RWSTD_NO_BAD_ALLOC_ASSIGNMENT

#  ifdef _RWSTD_NO_BAD_ALLOC_WHAT

#    if !__INTEL_COMPILER || __INTEL_COMPILER > 800

/* virtual */ const char* bad_alloc::
what () const _THROWS (())
{
    return _RWSTD_ERROR_BAD_ALLOC;
}

#    endif   // Intel C++ > 8.0
#  endif   // _RWSTD_NO_BAD_ALLOC_WHAT

#  if    !defined (_RWSTD_NO_STD_BAD_ALLOC)    \
      || !defined (_RWSTD_NO_RUNTIME_IN_STD)

}   // namespace std

#  endif   // !_RWSTD_NO_STD_BAD_ALLOC || !_RWSTD_NO_RUNTIME_IN_STD ...
#endif   // _RWSTD_NEW_INCLUDED
