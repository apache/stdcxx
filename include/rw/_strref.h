/***************************************************************************
 *
 * _strref.h - Declarations for the Standard Library string_ref classes
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
 * Copyright 1994-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifndef _RWSTD_RW_STRREF_H_INCLUDED
#define _RWSTD_RW_STRREF_H_INCLUDED

#ifndef _RWSTD_RW_ALLOCATOR_H_INCLUDED
#  include <rw/_allocator.h>
#endif   // _RWSTD_RW_ALLOCATOR_H_INCLUDED

#ifndef _RWSTD_RW_MUTEX_H_INCLUDED
#  include <rw/_mutex.h>
#endif   // _RWSTD_RW_MUTEX_H_INCLUDED

#ifndef _RWSTD_RW_SPECIALIZED_H_INCLUDED
#  include <rw/_specialized.h>
#endif   // _RWSTD_RW_SPECIALIZED_H_INCLUDED

#ifndef _RWSTD_RW_TRAITS_H_INCLUDED
#  include <rw/_traits.h>
#endif   // _RWSTD_RW_TRAITS_H_INCLUDED


#if !defined(_RWSTD_STRING_REF_OFFSET)
#  define _RWSTD_STRING_REF_OFFSET 1
#endif

#if !defined(_RWSTD_STRING_REF_INT)
#  define _RWSTD_STRING_REF_INT long
#endif

#if !defined (_RWSTD_NO_ATOMIC_OPS) && !defined (_RWSTD_NO_STRING_ATOMIC_OPS)
   // disable string mutex when atomic operations are available
#  ifndef _RWSTD_NO_STRING_MUTEX
#    define _RWSTD_NO_STRING_MUTEX
#  endif   // _RWSTD_NO_STRING_MUTEX
#endif   // _RWSTD_NO_ATOMIC_OPS && !_RWSTD_NO_STRING_ATOMIC_OPS


_RWSTD_NAMESPACE (std) { 


// chooses either a single global mutex or a mutex per string object
// or no mutex at all when atomic test-and-set instruction is available
#if !defined (_RWSTD_REENTRANT) || defined (_RWSTD_NO_STRING_MUTEX) 
#  define _RWSTD_STRING_MUTEX(ignore)   false
#elif defined (_RWSTD_ONE_STRING_MUTEX)
#  define _RWSTD_STRING_MUTEX(ignore)   __rw_string_mutex
#else
#  define _RWSTD_STRING_MUTEX(pstr)     pstr->_C_mutex
#endif


template <class _CharT,
          class _Traits = char_traits<_CharT>, 
          class _Allocator = allocator<_CharT> >
class basic_string;


}   // namespace std


_RWSTD_NAMESPACE (__rw) { 


#if    defined (_RWSTD_REENTRANT)        \
    && defined (_RWSTD_ONE_STRING_MUTEX) \
    && !defined (_RWSTD_NO_STRING_MUTEX)

extern  __rw_mutex _RWSTD_EXPORT __rw_string_mutex;

#endif   // _REENTRANT && _ONE_STRING_MUTEX && !NO_STRING_MUTEX


template <class _CharT, class _Traits , class _Allocator>
struct __string_ref
{
    typedef _CharT                                            char_type;
    typedef _Allocator                                        allocator_type;
    typedef typename allocator_type::size_type               size_type;
    typedef _STD::basic_string<_CharT, _Traits, _Allocator>   string_type;

#if     defined (_RWSTD_REENTRANT)              \
    && !defined (_RWSTD_ONE_STRING_MUTEX)       \
    && !defined (_RWSTD_NO_STRING_MUTEX)        \
    && !defined (_RWSTD_NO_STATIC_MUTEX_INIT)

    void _C_init (size_type __cap, size_type __size) {

        _C_cap          = __cap;
        _C_size._C_size = __size;

#  ifndef _RWSTD_NO_STRING_REF_COUNT

        _C_refs = 1 - _RWSTD_STRING_REF_OFFSET;

        if (0 != _RWSTD_MUTEX_INIT (this->_C_mutex._C_mutex))
            _RW::__rw_throw (_RWSTD_ERROR_RUNTIME_ERROR,
                             "synchronization error");

#  endif   // _RWSTD_NO_STRING_REF_COUNT

    }

    void _C_destroy () {
#  ifndef _RWSTD_NO_STRING_REF_COUNT
        _RWSTD_MUTEX_DESTROY (this->_C_mutex._C_mutex);
#  endif
    }

#else

    void _C_init (size_type __cap, size_type __size) {

#  ifndef _RWSTD_NO_STRING_REF_COUNT
        _C_refs = 1 - _RWSTD_STRING_REF_OFFSET;
#  endif   // _RWSTD_NO_STRING_REF_COUNT

        _C_cap          = __cap;
        _C_size._C_size = __size;
    }

    void _C_destroy () { }

#endif // _RWSTD_REENTRANT && !_RWSTD_ONE_STRING_MUTEX && 
       // !_RWSTD_NO_STRING_MUTEX && !_RWSTD_NO_STATIC_MUTEX_INIT


    int _C_get_ref () const {

#  ifndef _RWSTD_NO_STRING_REF_COUNT

        return _C_refs + _RWSTD_STRING_REF_OFFSET;

#else   // if defined (_RWSTD_NO_STRING_REF_COUNT)

        return 0;

#endif   // _RWSTD_NO_STRING_REF_COUNT

    }

    void _C_unref () {

#ifndef _RWSTD_NO_STRING_REF_COUNT

        // not thread-safe (see caller)
        if (this != string_type::_C_nullref ())
            _C_refs = -_RWSTD_STRING_REF_OFFSET;

#endif   // _RWSTD_NO_STRING_REF_COUNT
    }

    int _C_inc_ref () {

#ifndef _RWSTD_NO_STRING_REF_COUNT

        return this == string_type::_C_nullref () ? 1 :
            _RWSTD_STRING_ATOMIC_PREINCREMENT (_C_refs,
                _RWSTD_STRING_MUTEX (this));
#else   // if defined (_RWSTD_NO_STRING_REF_COUNT)
        return 0 + (this != string_type::_C_nullref ());
#endif   // _RWSTD_NO_STRING_REF_COUNT

    }

    int _C_dec_ref () {

#ifndef _RWSTD_NO_STRING_REF_COUNT

        return this == string_type::_C_nullref () ? 1 :
              _RWSTD_STRING_REF_OFFSET
            + _RWSTD_STRING_ATOMIC_PREDECREMENT (_C_refs,
                  _RWSTD_STRING_MUTEX (this));

#else   // if defined (_RWSTD_NO_STRING_REF_COUNT)
        return 0 + (this == string_type::_C_nullref ());
#endif   // _RWSTD_NO_STRING_REF_COUNT

    }

    size_type size () const {
        return _C_size._C_size;
    }

    size_type capacity () const {
        return _C_cap;
    }

    char_type* data () {
        return _RWSTD_REINTERPRET_CAST (char_type*, this + 1);
    }

    const char_type* data () const {
        return _RWSTD_REINTERPRET_CAST (const char_type*, this + 1);
    }

#ifndef _RWSTD_NO_STRING_REF_COUNT

#  if     defined (_RWSTD_REENTRANT)            \
      && !defined (_RWSTD_ONE_STRING_MUTEX)     \
      && !defined (_RWSTD_NO_STRING_MUTEX)

    __rw_mutex_base _C_mutex;

#  endif   // _REENTRANT && !_ONE_STRING_MUTEX && !_NO_STRING_MUTEX

    // reference count:
    // 1 less than the number of references to the string body
    // _RWSTD_STRING_REF_OFFSET less if _RWSTD_NO_STRING_MUTEX is defined
    // -1 if reference counting is disabled
    int _C_refs;

#endif   // _RWSTD_NO_STRING_REF_COUNT

    size_type _C_cap;       // Size of allocated memory

    union {
        size_type _C_size;    // Number of actual data values stored
        _CharT    _C_dummy;   // force the alignment of the first char
    } _C_size;                // named to work around an HP aCC 3.30 bug
};


// representation of the null string; will be a POD wherever possible
// (will not be POD if the reference contains a mutex with a UD-ctor)
template <class _CharT, class _Traits , class _Allocator>
struct __null_ref
    : __string_ref<_CharT, _Traits , _Allocator>
{
    // string reference immediately followed by a single terminating null
    _CharT _C_eos;
};


#ifdef _RWSTD_NO_COLLAPSE_TEMPLATE_STATICS

extern _RWSTD_EXPORT unsigned long __nullref [];

#endif   // _RWSTD_NO_COLLAPSE_TEMPLATE_STATICS

}   // namespace __rw


#endif   // _RWSTD_RW_STRREF_H_INCLUDED
