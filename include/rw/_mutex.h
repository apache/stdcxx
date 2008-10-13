/***************************************************************************
 *
 * _mutex.h - definitions of classes for thread safety
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifndef _RWSTD_RW_MUTEX_H_INCLUDED
#define _RWSTD_RW_MUTEX_H_INCLUDED


#if __GNUG__ >= 3
#  pragma GCC system_header
#endif   // gcc >= 3


#ifndef _RWSTD_RW_DEFS_H_INCLUDED
#  include <rw/_defs.h>
#endif   // _RWSTD_RW_DEFS_H_INCLUDED


#ifndef _RWSTD_RW_ATOMIC_H_INCLUDED
#  include <rw/_atomic.h>
#endif   // _RWSTD_RW_ATOMIC_H_INCLUDED


#ifdef _RWSTD_REENTRANT

#  ifndef _RWSTD_RW_EXCEPTION_H_INCLUDED
#    include <rw/_exception.h>
#  endif   // _RWSTD_RW_EXCEPTION_INCLUDED

#  ifndef _RWSTD_RW_ERROR_H_INCLUDED
#    include <rw/_error.h>
#  endif   // _RWSTD_RW_ERROR_H_INCLUDED


// work around SunOS 5.{8,9}/SunPro bug (see PR #26255)
#  if defined (__SunOS_5_8) || defined (__SunOS_5_9)
#    undef _TIME_T
#  endif   // __SunOS_5_{8,9}

#  if defined (_RWSTD_SOLARIS_THREADS)  // assuming Solaris 2.1 or greater
#    include <rw/_mutex-solaris.h>
#  elif defined (_RWSTD_POSIX_THREADS)
#    include <rw/_mutex-pthread.h>
#  elif defined (_RWSTD_DCE_THREADS)
#    include <rw/_mutex-dce.h>
#  elif defined (_WIN32)
#    include <rw/_mutex-win32.h>
#  elif defined (__OS2__)
#    include <rw/_mutex-os2.h>
#  else
#    error unknown thread environment
#  endif


#  if defined(_RWSTD_NO_STATIC_MUTEX_INIT)
#    include <new>
#  endif  //_RWSTD_NO_STATIC_MUTEX_INIT


_RWSTD_NAMESPACE (__rw) { 

// a using declaration (rather than a directive)
// used to work around yet another MSVC 6.0 bug
_USING (std::exception);

class __rw_thread_error : public exception
{
public:
    __rw_thread_error () _THROWS (())
    : exception () { }

    virtual const char* what () const _THROWS (()) {
        return "thread synchronization error";
    }
};                            


// POD type with no user-defined ctor or dtor facilitates static
// initialization of mutex objects with static storage duration
// (important during library initialziation time)
class _RWSTD_EXPORT __rw_mutex_base
{
public:

    void _C_acquire () {
#  if !defined (__HP_aCC) || __HP_aCC > 32700
        if (0 != _RWSTD_MUTEX_LOCK (_C_mutex))
            _RW::__rw_throw (_RWSTD_ERROR_RUNTIME_ERROR,
                             "synchronization error");
#  else
        // working around an HP aCC 3.27 bug JAGac88738
        _RWSTD_MUTEX_LOCK (_C_mutex);
#  endif   // !defined (__HP_aCC) || __HP_aCC > 32700
    }

    void _C_release ();

#  ifdef _RWSTD_NO_STATIC_MUTEX_INIT

    // static initialization not an option, define ctor and dtor
    // and make member mutex private

    __rw_mutex_base ();

    ~__rw_mutex_base ();

private:

    // not defined
    __rw_mutex_base (const __rw_mutex_base&);
    __rw_mutex_base& operator= (const __rw_mutex_base&);

#  endif   // _RWSTD_NO_STATIC_MUTEX_INIT

    _RWSTD_MUTEX_T _C_mutex;   // the real thing
};


inline void __rw_mutex_base::_C_release ()
{
    // we should NOT throw from here as _C_release will typically be called
    // during the destruction of local objects such as __rw_guard (perhaps
    // due to another exception)
    _RWSTD_MUTEX_UNLOCK (_C_mutex);
}


// non-POD type, always initializes mutex data member via a function call
class _RWSTD_EXPORT __rw_mutex: public __rw_mutex_base
{
public:

#  ifndef _RWSTD_NO_STATIC_MUTEX_INIT

    __rw_mutex ();

    ~__rw_mutex ();

private:

    // not defined
    __rw_mutex (const __rw_mutex&);
    __rw_mutex& operator= (const __rw_mutex&);

#  endif   // _RWSTD_NO_STATIC_MUTEX_INIT

};


#  ifndef _RWSTD_NO_STATIC_MUTEX_INIT

// helper factory class - static member is guranteed to be constructed
// during static initialization; objects of this POD class are not
// destroyed during program termination (important to allow them
// to be used in dtors of other objects with static storage duration)
template <class _TypeT>
struct __rw_static_mutex
{
    static __rw_mutex_base _C_mutex;
};


template <class _TypeT>
__rw_mutex_base __rw_static_mutex<_TypeT>::_C_mutex

#    ifdef _RWSTD_MUTEX_INITIALIZER

    = { _RWSTD_MUTEX_INITIALIZER }

#      undef _RWSTD_MUTEX_INITIALIZER
#    endif   // _RWSTD_MUTEX_INITIALIZER
        ;

#    if _RWSTD_INSTANTIATE (_STATIC_MUTEX, _INT)

// explicitly instantiated to work around a g++ 2.95.2 bug on COFF systems
// (such as IBM AIX or DEC OSF1) where it "forgets" to do so implicitly for
// explicitly initialized static data members
_RWSTD_INSTANTIATE_1 (class _RWSTD_TI_EXPORT __rw_static_mutex<int>);
_RWSTD_INSTANTIATE_1 (class _RWSTD_TI_EXPORT __rw_static_mutex<size_t>);

#    endif   // _RWSTD_INSTANTIATE (_STATIC_MUTEX, _INT)


inline __rw_mutex::__rw_mutex ()

#  else   // if defined (_RWSTD_NO_STATIC_MUTEX_INIT)

inline __rw_mutex_base::__rw_mutex_base ()

#  endif   // _RWSTD_NO_STATIC_MUTEX_INIT
{ 
    if (0 != _RWSTD_MUTEX_INIT (_C_mutex))
        _RW::__rw_throw (_RWSTD_ERROR_RUNTIME_ERROR, "synchronization error");
}

#  ifndef _RWSTD_NO_STATIC_MUTEX_INIT

inline __rw_mutex::~__rw_mutex () 

#  else   // if defined (_RWSTD_NO_STATIC_MUTEX_INIT)

inline __rw_mutex_base::~__rw_mutex_base () 

#  endif   // _RWSTD_NO_STATIC_MUTEX_INIT
{
    _RWSTD_MUTEX_DESTROY (_C_mutex);
}

// type used in instantiating __rw_get_static_mutex
template<class _TypeT, int _IntI> struct __rw_type { };

// mutex factory function avoids problematic initialization dependencies
// introduced by definitions of mutexes with static storage duration
// instead of defining a static mutex member that may not be initialized
// before first use, a non-local class may either declare a unique type
// (enum or class) or use itself as this type, and call this template
// function with a ptr to that type to obtain an initialized mutex object
// that is the same for each unique type passed to the function

#  ifndef _RWSTD_NO_STATIC_MUTEX_INIT

template <class _TypeT>
inline
__rw_mutex_base& __rw_get_static_mutex (_TypeT*)
{
    // POD mutex already initialized during static initialization
    return __rw_static_mutex<_TypeT>::_C_mutex;
}

#  else   // if defined (_RWSTD_NO_STATIC_MUTEX_INIT)

#    ifdef _INLINE_WITH_STATICS

template <class _TypeT>
_INLINE_WITH_STATICS
__rw_mutex_base& __rw_get_static_mutex (_TypeT*)

#    else    // if !defined (_INLINE_WITH_STATICS)

template <class _TypeT>
__rw_mutex_base& __rw_get_static_mutex (_TypeT*)

#    endif   // _INLINE_WITH_STATICS

{
    // allocate properly aligned memory for static mutex (necessary
    // to prevent static local mutex from being destroyed during
    // program termination)
    union __mutex_buf_t {
        long double __pad;   // force alignment (must be first)
        char        __buf [sizeof (__rw_mutex_base)];
    };

    // using a named union to work around a bug in HP aCC 3.14.10 (JAGad03246)
    static __mutex_buf_t __mutex_buf;

    // initialize mutex reference to refer to the static buffer space
    __rw_mutex_base &__mutex =
        _RWSTD_REINTERPRET_CAST (__rw_mutex_base&, __mutex_buf);

    // keep track of number of mutex initialization attempts
    // although `init' may reach a value greater than 1, `mutex'
    // will (should) never be multiply initialized

    // implicit initialization used to prevent a g++ 2.95.2 warning on Tru64
    // sorry: semantics of inline function static data are wrong (you'll wind
    // up with multiple copies)
    static volatile int __cntr /* = 0 */;   // initialization counter

#    if !defined (_RWSTD_NO_ATOMIC_OPS) && !defined (_RWSTD_NO_INT_ATOMIC_OPS)
    // MT safe
    // cast __cntr to int& (see STDCXX-792)
    // casting should be removed after fixing STDCXX-794
    if (0 == __cntr && 1 == _RWSTD_ATOMIC_PREINCREMENT (
            _RWSTD_CONST_CAST (int&, __cntr), false))
#    else   // _RWSTD_NO_ATOMIC_OPS || _RWSTD_NO_INT_ATOMIC_OPS
    // not so safe (volatile should help)
    if (0 == __cntr && 1 == ++__cntr)
#    endif   // !_RWSTD_NO_ATOMIC_OPS && !_RWSTD_NO_INT_ATOMIC_OPS

    {
        // manually initialize `mutex' via a call to placement new
        new (&__mutex) __rw_mutex_base ();

        // indicate that `mutex' has been fully initialized
        // (unlikely that we'll have more than 1000 threads)
        __cntr += 1000;
    }
    else
        // busywait until `mutex' has been completely initialized
        while (__cntr < 1000);

    return __mutex;
}

#  endif   //_RWSTD_NO_STATIC_MUTEX_INIT


// clean up
#  undef _RWSTD_MUTEX_LOCK
#  undef _RWSTD_MUTEX_UNLOCK
#  undef _RWSTD_MUTEX_T


// allows safe use of a mutex in the presence of exceptions
class __rw_guard
{
    __rw_mutex_base *_C_mutex;

    // undefined
    __rw_guard& operator= (const __rw_guard&);
    
public:

    __rw_guard (__rw_guard &__rhs)
        : _C_mutex (__rhs._C_mutex) {
        __rhs._C_mutex = 0;
    }

    __rw_guard (__rw_mutex_base &__mutex): _C_mutex (&__mutex) {
        _C_mutex->_C_acquire ();
    }

    __rw_guard (__rw_mutex_base *__mutex): _C_mutex (__mutex) {
        if (_C_mutex)
            _C_mutex->_C_acquire ();
    }

    ~__rw_guard () {
        if (_C_mutex)
            _C_mutex->_C_release ();
    }

    __rw_mutex_base* _C_set (__rw_mutex_base *__mutex) {
        __rw_mutex_base *__tmp = _C_mutex;
        return _C_mutex = __mutex, __tmp;
    }
};


// base class for obects required to guarantee some degree of MT safety
struct _RWSTD_EXPORT __rw_synchronized
{
    __rw_mutex _C_mutex;

    void _C_lock () {
        _C_mutex._C_acquire ();
    }

    void _C_unlock () {
        _C_mutex._C_release ();
    }

    __rw_guard _C_guard () {
        __rw_guard __guard (_C_mutex);
        return __guard;
    }
};

}   // namespace __rw


// include atomic functions using mutex object
#  ifndef _RWSTD_RW_ATOMIC_MUTEX_H_INCLUDED
#    include <rw/_atomic-mutex.h>
#  endif   // _RWSTD_RW_ATOMIC_MUTEX_H_INCLUDED


#else   // if !defined (_RWSTD_REENTRANT)

_RWSTD_NAMESPACE (__rw) { 

// dummy classes used as a base class in single-threaded environments

struct __rw_mutex_base
{
    void _C_acquire () { }

    void _C_release () { }
};


struct _RWSTD_EXPORT __rw_mutex: public __rw_mutex_base
{
};


struct __rw_guard
{
    __rw_guard (__rw_mutex_base&) { }

    __rw_guard (__rw_mutex_base*) { }

    __rw_mutex_base* _C_set (__rw_mutex_base*) {
        return 0;
    }
};


struct __rw_synchronized
{
    // static so that it takes up no space
    static _RWSTD_EXPORT __rw_mutex _C_mutex;

    void _C_lock () { }

    void _C_unlock () { }

    __rw_guard _C_guard () {
        return __rw_guard (_C_mutex);
    }
};


}   // namespace __rw


#endif   // _RWSTD_REENTRANT

// clean up
#ifdef _RWSTD_NO_CHAR_ATOMIC_OPS
#  undef _RWSTD_NO_CHAR_ATOMIC_OPS
#endif

#ifdef _RWSTD_NO_SHORT_ATOMIC_OPS
#  undef _RWSTD_NO_SHORT_ATOMIC_OPS
#endif

#ifdef _RWSTD_NO_INT_ATOMIC_OPS
#  undef _RWSTD_NO_INT_ATOMIC_OPS
#endif

#ifdef _RWSTD_NO_LONG_ATOMIC_OPS
#  undef _RWSTD_NO_LONG_ATOMIC_OPS
#endif

#ifdef _RWSTD_NO_LLONG_ATOMIC_OPS
#  undef _RWSTD_NO_LLONG_ATOMIC_OPS
#endif

#endif   // _RWSTD_RW_MUTEX_H_INCLUDED
