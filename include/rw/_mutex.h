/***************************************************************************
 *
 * _mutex.h - definitions of classes and inline functions
 *            for thread safety and atomic operations
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
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


#ifdef _RWSTD_REENTRANT

#ifndef _RWSTD_RW_EXCEPTION_H_INCLUDED
#  include <rw/_exception.h>
#endif   // _RWSTD_RW_EXCEPTION_INCLUDED

#ifndef _RWSTD_RW_ERROR_H_INCLUDED
#  include <rw/_error.h>
#endif   // _RWSTD_RW_ERROR_H_INCLUDED


// work around SunOS 5.{8,9}/SunPro bug (see PR #26255)
#if defined (__SunOS_5_8) || defined (__SunOS_5_9)
#  undef _TIME_T
#endif   // __SunOS_5_{8,9}

#if defined (_RWSTD_SOLARIS_THREADS)  // assuming Solaris 2.1 or greater

// SunOS 5.7 Threads Library:
//   "A statically  allocated  mutex does  not  need to be explicitly
//   initialized; by default, a statically allocated mutex is initialized
//   with  all  zeros and its scope is set to be within the calling
//   process."

#  include <synch.h>
#  include <thread.h>

#  define _RWSTD_MUTEX_INIT(mutex)      mutex_init (&mutex, USYNC_THREAD, 0)
#  define _RWSTD_MUTEX_DESTROY(mutex)   mutex_destroy (&mutex)
#  define _RWSTD_MUTEX_LOCK(mutex)      mutex_lock (&mutex)
#  define _RWSTD_MUTEX_UNLOCK(mutex)    mutex_unlock (&mutex)
#  define _RWSTD_MUTEX_T                mutex_t

#elif defined (_RWSTD_POSIX_THREADS)

// LinuxThreads man page:
//   "Variables of type pthread_mutex_t can also be initialized
//    statically, using the constants  PTHREAD_MUTEX_INITIALIZER
//    (for fast mutexes), PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
//    (for recursive mutexes), and PTHREAD_ERRORCHECK_MUTEX_INI-
//    TIALIZER_NP (for error checking mutexes)."
//    ...
//    "Attempting to initialize an already initialized mutex results
//    in undefined behavior."

#  include <pthread.h>

#  define _RWSTD_MUTEX_INIT(mutex)      pthread_mutex_init (&mutex, 0)
#  define _RWSTD_MUTEX_DESTROY(mutex)   pthread_mutex_destroy (&mutex)
#  define _RWSTD_MUTEX_LOCK(mutex)      pthread_mutex_lock (&mutex)
#  define _RWSTD_MUTEX_UNLOCK(mutex)    pthread_mutex_unlock (&mutex)
#  define _RWSTD_MUTEX_T                pthread_mutex_t

#elif defined (_RWSTD_DCE_THREADS)

#  if defined (_RWSTD_NO_DCE_PTHREAD_H)
#    include <pthread.h>
#  else
#    include <dce/pthread.h>
#  endif

#  define _RWSTD_MUTEX_INIT(mutex) \
          pthread_mutex_init (&mutex, pthread_mutexattr_default)
#  define _RWSTD_MUTEX_DESTROY(mutex)   pthread_mutex_destroy (&mutex)
#  define _RWSTD_MUTEX_LOCK(mutex)      pthread_mutex_lock (&mutex)
#  define _RWSTD_MUTEX_UNLOCK(mutex)    pthread_mutex_unlock (&mutex)
#  define _RWSTD_MUTEX_T                pthread_mutex_t

#elif defined (_WIN32) || defined (_WIN64)

#  ifdef _RWSTD_NO_FWD_DECLARATIONS

#    include <windows.h>
#    define _RWSTD_MUTEX_T _RTL_CRITICAL_SECTION

#  else   // if defined (_RWSTD_NO_FWD_DECLARATIONS)

   // avoid #including this header (MFC doesn't like it)
   // #  include <windows.h>

extern "C" {

// but rather declare these globals here
struct _RTL_CRITICAL_SECTION;

__declspec (dllimport) void __stdcall
InitializeCriticalSection (_RTL_CRITICAL_SECTION*);

__declspec (dllimport) void __stdcall
EnterCriticalSection (_RTL_CRITICAL_SECTION*);

__declspec (dllimport) void __stdcall
LeaveCriticalSection (_RTL_CRITICAL_SECTION*);

__declspec (dllimport) void __stdcall
DeleteCriticalSection (_RTL_CRITICAL_SECTION*);

__declspec (dllimport) long __stdcall
InterlockedIncrement (_RWSTD_INTERLOCKED_T*);

__declspec (dllimport) long __stdcall
InterlockedDecrement (_RWSTD_INTERLOCKED_T*);

__declspec (dllimport) long __stdcall
InterlockedExchange (_RWSTD_INTERLOCKED_T*, long);

}   // extern "C"

_RWSTD_NAMESPACE (__rw) { 

// fake critical section type
union __rw_critical_section {
    long _C_pad;   // force alignment
    char _C_buf [24 /* == sizeof (_RTL_CRITICAL_SECTION) */];
};

#    define _RWSTD_MUTEX_T _RW::__rw_critical_section

}   // namespace __rw


#  endif   // _RWSTD_NO_FWD_DECLARATIONS



_RWSTD_NAMESPACE (__rw) { 

// Win32/64 throws non-C++ exceptions rather than returning error status
// from some system calls like most other operating systems do

inline int __rw_mutex_init (_RTL_CRITICAL_SECTION *__mutex)
{
    __try {
        InitializeCriticalSection (__mutex);
    }
    __except (1) {
        return -1;
    }
    return 0;
}

inline int __rw_mutex_destroy (_RTL_CRITICAL_SECTION *__mutex)
{
    __try {
        DeleteCriticalSection (__mutex);
    }
    __except (1) {
        return -1;
    }
    return 0;
}

inline int __rw_mutex_lock (_RTL_CRITICAL_SECTION *__mutex)
{
    __try {
        EnterCriticalSection (__mutex);
    }
    __except (1) {
        return -1;
    }
    return 0;
}

inline int __rw_mutex_unlock (_RTL_CRITICAL_SECTION *__mutex)
{
    __try {
        LeaveCriticalSection (__mutex);
    }
    __except (1) {
        return -1;
    }
    return 0;
}

#  define _RWSTD_MUTEX_INIT(mutex)      \
   __rw_mutex_init (_RWSTD_REINTERPRET_CAST (_RTL_CRITICAL_SECTION*, &mutex))
#  define _RWSTD_MUTEX_DESTROY(mutex)   \
   __rw_mutex_destroy (_RWSTD_REINTERPRET_CAST (_RTL_CRITICAL_SECTION*, &mutex))
#  define _RWSTD_MUTEX_LOCK(mutex)      \
   __rw_mutex_lock (_RWSTD_REINTERPRET_CAST (_RTL_CRITICAL_SECTION*, &mutex))
#  define _RWSTD_MUTEX_UNLOCK(mutex)    \
   __rw_mutex_unlock (_RWSTD_REINTERPRET_CAST (_RTL_CRITICAL_SECTION*, &mutex))

}   // namespace __rw

#elif defined (__OS2__)

#  define INCL_DOSSEMAPHORES

#  include <os2.h>

#  define _RWSTD_MUTEX_INIT(mutex) \
          DosCreateMutexSem (0, &mutex, DC_SEM_SHARED,FALSE)
#  define _RWSTD_MUTEX_DESTROY(mutex)   DosCloseMutexSem (mutex)
#  define _RWSTD_MUTEX_LOCK(mutex) \
          DosRequestMutexSem (mutex, SEM_INDEFINITE_WAIT)
#  define _RWSTD_MUTEX_UNLOCK(mutex)    DosReleaseMutexSem (mutex)
#  define _RWSTD_MUTEX_T                HMTX

#else
#  error unknown thread environment
#endif

#if defined(_RWSTD_NO_STATIC_MUTEX_INIT)
#  include <new>
#endif  //_RWSTD_NO_STATIC_MUTEX_INIT


#if defined (__GNUG__) && defined (__osf__)
   // prevent g++ warnings about missing initializers
   // see <pthread.h> for explanation of _PTHREAD_NOMETER_STATIC
#  ifndef _PTHREAD_NOMETER_STATIC
#    define _RWSTD_PTHREAD_MUTEX_INITIALIZER \
            { _PTHREAD_MSTATE_SLOW, _PTHREAD_MVALID | _PTHREAD_MVF_STA, \
              0, 0, 0, 0, 0, 0 }
#  else   // if defined (_PTHREAD_NOMETER_STATIC)
#    define _RWSTD_PTHREAD_MUTEX_INITIALIZER
            { 0, _PTHREAD_MVALID | _PTHREAD_MVF_STA, 0, 0, 0, 0, 0, 0 }
#  endif   // _PTHREAD_NOMETER_STATIC
#elif defined (__GNUG__) && defined (__sgi__)
   // prevent g++ warnings about a partly bracketed initializer
#  define _RWSTD_PTHREAD_MUTEX_INITIALIZER { PTHREAD_MUTEX_INITIALIZER }
#else
#  define _RWSTD_PTHREAD_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER
#endif


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
#if !defined (__HP_aCC) || __HP_aCC > 32700
        if (0 != _RWSTD_MUTEX_LOCK (_C_mutex))
            _RW::__rw_throw (_RWSTD_ERROR_RUNTIME_ERROR,
                             "synchronization error");
#else
        // working around an HP aCC 3.27 bug JAGac88738
        _RWSTD_MUTEX_LOCK (_C_mutex);
#endif   // !defined (__HP_aCC) || __HP_aCC > 32700
    }

    void _C_release ();

#ifdef _RWSTD_NO_STATIC_MUTEX_INIT

    // static initialization not an option, define ctor and dtor
    // and make member mutex private

    __rw_mutex_base ();

    ~__rw_mutex_base ();

private:

    // not defined
    __rw_mutex_base (const __rw_mutex_base&);
    __rw_mutex_base& operator= (const __rw_mutex_base&);

#endif   // _RWSTD_NO_STATIC_MUTEX_INIT

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

#ifndef _RWSTD_NO_STATIC_MUTEX_INIT

    __rw_mutex ();

    ~__rw_mutex ();

private:

    // not defined
    __rw_mutex (const __rw_mutex&);
    __rw_mutex& operator= (const __rw_mutex&);

#endif   // _RWSTD_NO_STATIC_MUTEX_INIT

};


#ifndef _RWSTD_NO_STATIC_MUTEX_INIT

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

#ifdef _RWSTD_POSIX_THREADS

    = { _RWSTD_PTHREAD_MUTEX_INITIALIZER }

#endif   // _RWSTD_POSIX_THREADS
        ;

#if _RWSTD_INSTANTIATE (_STATIC_MUTEX, _INT)

// explicitly instantiated to work around a g++ 2.95.2 bug on COFF systems
// (such as IBM AIX or DEC OSF1) where it "forgets" to do so implicitly for
// explicitly initialized static data members
_RWSTD_INSTANTIATE_1 (class _RWSTD_EXPORT __rw_static_mutex<int>);
_RWSTD_INSTANTIATE_1 (class _RWSTD_EXPORT __rw_static_mutex<size_t>);

#endif   // _RWSTD_INSTANTIATE (_STATIC_MUTEX, _INT)


inline __rw_mutex::__rw_mutex ()

#else   // if defined (_RWSTD_NO_STATIC_MUTEX_INIT)

inline __rw_mutex_base::__rw_mutex_base ()

#endif   // _RWSTD_NO_STATIC_MUTEX_INIT
{ 
    if (0 != _RWSTD_MUTEX_INIT (_C_mutex))
        _RW::__rw_throw (_RWSTD_ERROR_RUNTIME_ERROR, "synchronization error");
}

#ifndef _RWSTD_NO_STATIC_MUTEX_INIT

inline __rw_mutex::~__rw_mutex () 

#else   // if defined (_RWSTD_NO_STATIC_MUTEX_INIT)

inline __rw_mutex_base::~__rw_mutex_base () 

#endif   // _RWSTD_NO_STATIC_MUTEX_INIT
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

#ifndef _RWSTD_NO_STATIC_MUTEX_INIT

template <class _TypeT>
inline
__rw_mutex_base& __rw_get_static_mutex (_TypeT*)
{
    // POD mutex already initialized during static initialization
    return __rw_static_mutex<_TypeT>::_C_mutex;
}

#else   // if defined (_RWSTD_NO_STATIC_MUTEX_INIT)

#ifdef _INLINE_WITH_STATICS

template <class _TypeT>
_INLINE_WITH_STATICS
__rw_mutex_base& __rw_get_static_mutex (_TypeT*)

#else    // if !defined (_INLINE_WITH_STATICS)

template <class _TypeT>
__rw_mutex_base& __rw_get_static_mutex (_TypeT*)

#endif   // _INLINE_WITH_STATICS

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
    static volatile long __cntr /* = 0 */;   // initialization counter

#if defined (_WIN32) || defined (_WIN64)
    // MT safe
    if (0 == __cntr && 1 == InterlockedIncrement ((long*)&__cntr))
#else
    // not so safe (volatile should help)
    if (0 == __cntr && 1 == ++__cntr)
#endif   // _WIN32

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

#endif   //_RWSTD_NO_STATIC_MUTEX_INIT


// clean up
#undef _RWSTD_MUTEX_LOCK
#undef _RWSTD_MUTEX_UNLOCK
#undef _RWSTD_MUTEX_T


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


// helper functions for atomic value [in|de]crement and exchange
// the functions are atomic with respect to each other as long as
// they are passed the same mutex by the callers
template <class _TypeT>
inline
_TypeT __rw_atomic_preincrement (_TypeT &__t, __rw_mutex_base &__mutex)
{
    _RWSTD_MT_GUARD (__mutex);

    return ++__t;
}


template <class _TypeT>
inline
_TypeT __rw_atomic_predecrement (_TypeT &__t, __rw_mutex_base &__mutex)
{
    _RWSTD_MT_GUARD (__mutex);

    return --__t;
}


template <class _TypeT, class _TypeU>
inline
_TypeT __rw_atomic_exchange (_TypeT &__t, const _TypeU &__u,
                             __rw_mutex_base &__mutex)
{
    _RWSTD_MT_GUARD (__mutex);

    _TypeT __tmp = __t;

#ifndef _WIN64
    __t = __u;
#else
    // silence MSVC conversion warnings (cast breaks SunPro 5.3 and prior)
    __t = _RWSTD_STATIC_CAST (_TypeT, __u);
#endif

    return __tmp;
}


// for use on class statics or on namespace-scope variables
// the unused argument is only here so that all functions
// can be called from the same set of macros

template <class _TypeT>
inline
_TypeT __rw_atomic_preincrement (_TypeT &__t, bool)
{
    return __rw_atomic_preincrement (__t,
                                     __rw_get_static_mutex ((_TypeT*)0));
}


template <class _TypeT>
inline
_TypeT __rw_atomic_predecrement (_TypeT &__t, bool)
{
    return __rw_atomic_predecrement (__t,
                                     __rw_get_static_mutex ((_TypeT*)0));
}


template <class _TypeT, class _TypeU>
inline
_TypeT __rw_atomic_exchange (_TypeT &__t, const _TypeU &__u, bool)
{
    return __rw_atomic_exchange (__t, __u,
                                 __rw_get_static_mutex ((_TypeT*)0));
}

/********************** no atomic ops ********************************/

#if defined (_RWSTD_NO_ATOMIC_OPS)

// do nothing

/********************** DEC CXX **************************************/

#elif defined (__DECCXX)

}   // namespace __rw

// get declarations of __ATOMIC_XXX intrinsics
#  include <machine/builtins.h>

_RWSTD_NAMESPACE (__rw) {

// __ATOMIC_[DE|IN]CREMENT_[LONG|QUAD] and __ATOMIC_EXCH_[LONG|QUAD] are
// intrinsic functions declared in <machine/builtins.h> that atomically
// modify their argument and return its original value (__ATOMIC_XXX_LONG
// is misnamed -- it actually operates on an int, not a long)

inline int
__rw_atomic_preincrement (int &__x, bool)
{
    return 1 + __ATOMIC_INCREMENT_LONG (&__x);
}


inline unsigned
__rw_atomic_preincrement (unsigned &__x, bool)
{
    return __rw_atomic_preincrement (_RWSTD_REINTERPRET_CAST (int&, __x),
                                     false);
}


inline long
__rw_atomic_preincrement (long &__x, bool)
{
    return 1 + __ATOMIC_INCREMENT_QUAD (&__x);
}


inline unsigned long
__rw_atomic_preincrement (unsigned long &__x, bool)
{
    return __rw_atomic_preincrement (_RWSTD_REINTERPRET_CAST (long&, __x),
                                     false);
}


inline int
__rw_atomic_predecrement (int &__x, bool)
{
    return __ATOMIC_DECREMENT_LONG (&__x) - 1;
}


inline unsigned
__rw_atomic_predecrement (unsigned &__x, bool)
{
    return __rw_atomic_predecrement (_RWSTD_REINTERPRET_CAST (int&, __x),
                                     false);
}


inline long
__rw_atomic_predecrement (long &__x, bool)
{
    return __ATOMIC_DECREMENT_QUAD (&__x) - 1;
}


inline unsigned long
__rw_atomic_predecrement (unsigned long &__x, bool)
{
    return __rw_atomic_predecrement (_RWSTD_REINTERPRET_CAST (long&, __x),
                                     false);
}


inline int
__rw_atomic_exchange (int &__x, int __y, bool)
{
    return __ATOMIC_EXCH_LONG (&__x, __y);
}


inline unsigned
__rw_atomic_exchange (unsigned &__x, unsigned __y, bool)
{
    return __rw_atomic_exchange (_RWSTD_REINTERPRET_CAST (unsigned&, __x),
                                 _RWSTD_STATIC_CAST (int, __y), false);
}


inline long
__rw_atomic_exchange (long &__x, long __y, bool)
{
    return __ATOMIC_EXCH_QUAD (&__x, __y);
}


inline unsigned long
__rw_atomic_exchange (unsigned long &__x, unsigned long __y, bool)
{
    return __rw_atomic_exchange (_RWSTD_REINTERPRET_CAST (long&, __x),
                                 _RWSTD_STATIC_CAST (long, __y), false);
}

/********************** SPARC **************************************/

#elif defined (__sparc) && (defined (__SUNPRO_CC) || defined (__GNUG__))

extern "C" {

// define in assembler file "atomic-sparc.s" and "atomic-sparc64.s"
// exchange returns the original value, and add returns the incremented
// value

int  __rw_atomic_xchg32 (int*, int);
int  __rw_atomic_add32 (int*, int);

#  if defined (_LP64)

long __rw_atomic_xchg64 (long*, long);
long __rw_atomic_add64 (long*, long);

#  endif   // !_LP64

}   // extern "C"


inline int
__rw_atomic_preincrement (int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (int));
    return __rw_atomic_add32 (&__x, 1);
}


inline unsigned
__rw_atomic_preincrement (unsigned &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (unsigned));
    return __rw_atomic_add32 (_RWSTD_REINTERPRET_CAST (int*, &__x), 1);
}


inline int
__rw_atomic_predecrement (int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (int));
    return __rw_atomic_add32 (&__x, -1);
}


inline unsigned
__rw_atomic_predecrement (unsigned &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (unsigned));
    return __rw_atomic_add32 (_RWSTD_REINTERPRET_CAST (int*, &__x), -1);
}


inline int
__rw_atomic_exchange (int &__x, int __y, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (int));
    return __rw_atomic_xchg32 (&__x, __y);
}


inline unsigned
__rw_atomic_exchange (unsigned &__x, unsigned __y, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (unsigned));
    return __rw_atomic_xchg32 (_RWSTD_REINTERPRET_CAST (int*, &__x),
                               _RWSTD_STATIC_CAST (int, __y));
}

#  if _RWSTD_INT_SIZE < _RWSTD_LONG_SIZE

inline long
__rw_atomic_preincrement (long &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (long));
    return __rw_atomic_add64 (&__x, 1);
}


inline unsigned long
__rw_atomic_preincrement (unsigned long &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (unsigned long));
    return __rw_atomic_add64 (_RWSTD_REINTERPRET_CAST (long*, &__x), 1);
}



inline long
__rw_atomic_predecrement (long &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (long));
    return __rw_atomic_add64 (&__x, -1);
}


inline unsigned long
__rw_atomic_predecrement (unsigned long &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (unsigned long));
    return __rw_atomic_add64 (_RWSTD_REINTERPRET_CAST (long*, &__x), -1);
}


inline long
__rw_atomic_exchange (long &__x, long __y, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (long));
    return __rw_atomic_xchg64 (&__x, __y);
}


inline unsigned long
__rw_atomic_exchange (unsigned long &__x, unsigned long __y, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (unsigned long));
    return __rw_atomic_xchg64 (_RWSTD_REINTERPRET_CAST (long*, &__x),
                               _RWSTD_STATIC_CAST (long, __y));
}

#  endif   // _RWSTD_INT_SIZE < _RWSTD_LONG_SIZE

/********************** AIX **************************************/

#elif defined (_AIX43) && defined (__IBMCPP__)

}   // namespace __rw

#include <sys/atomic_op.h>

_RWSTD_NAMESPACE (__rw) {

inline int
__rw_atomic_preincrement (int &__x, bool)
{
    return fetch_and_add (&__x, 1) + 1;
}


inline unsigned
__rw_atomic_preincrement (unsigned &__x, bool)
{
    return __rw_atomic_preincrement (_RWSTD_REINTERPRET_CAST (int&, __x),
                                     false);
}


inline int
__rw_atomic_predecrement (int &__x, bool)
{
    return fetch_and_add(&__x,-1) - 1;
}


inline unsigned
__rw_atomic_predecrement (unsigned &__x, bool)
{
    return __rw_atomic_predecrement (_RWSTD_REINTERPRET_CAST (int&, __x),
                                     false);
}


inline int
__rw_atomic_exchange (int &__x, int __y, bool)
{
    int __tmp;

    do {
        __tmp = __x;
    } while (!compare_and_swap (&__x, &__tmp, __y));

    return __tmp;
}


inline unsigned
__rw_atomic_exchange (unsigned &__x, unsigned __y, bool)
{
    return __rw_atomic_exchange (_RWSTD_REINTERPRET_CAST (int&, __x),
                                 _RWSTD_STATIC_CAST (int, __y), false);
}


#  if _RWSTD_INT_SIZE < _RWSTD_LONG_SIZE

inline long
__rw_atomic_preincrement (long &__x, bool)
{
    return fetch_and_addlp (&__x, 1) + 1;
}


inline unsigned long
__rw_atomic_preincrement (unsigned long &__x, bool)
{
  return __rw_atomic_preincrement (_RWSTD_REINTERPRET_CAST (long&, __x),
                                   false);
}


inline long
__rw_atomic_predecrement (long &__x, bool)
{
    return fetch_and_addlp (&__x, -1) - 1;
}


inline unsigned long
__rw_atomic_predecrement (unsigned long &__x, bool)
{
    return __rw_atomic_predecrement (_RWSTD_REINTERPRET_CAST (long&, __x),
                                     false);
}


inline long
__rw_atomic_exchange (long &__x, long __y, bool)
{
    long __tmp;

    do {
        __tmp = __x;
    } while (!compare_and_swaplp (&__x, &__tmp, __y));

    return __tmp;
}


inline unsigned long
__rw_atomic_exchange (unsigned long &__x, unsigned long __y, bool)
{
    return __rw_atomic_exchange (_RWSTD_REINTERPRET_CAST (long&, __x),
                                 _RWSTD_STATIC_CAST (long, __y), false);
}

#  endif   // _RWSTD_INT_SIZE < _RWSTD_LONG_SIZE


/********************** SGI **************************************/

#elif defined (__sgi) && defined (__host_mips)

}   // namespace __rw

#  include <mutex.h>

_RWSTD_NAMESPACE (__rw) {


inline unsigned
__rw_atomic_preincrement (unsigned &__x, bool)
{
    return __add_then_test32 (&__x, 1U);
}


inline int
__rw_atomic_preincrement (int &__x, bool)
{
    return __rw_atomic_preincrement (_RWSTD_REINTERPRET_CAST (unsigned&, __x),
                                     false);
}


inline unsigned
__rw_atomic_predecrement (unsigned &__x, bool)
{
    return __add_then_test32 (&__x, unsigned (-1));
}


inline int
__rw_atomic_predecrement (int &__x, bool)
{
    return __rw_atomic_predecrement (_RWSTD_REINTERPRET_CAST (unsigned &, __x),
                                     false);
}


inline unsigned
__rw_atomic_exchange (unsigned &__x, unsigned __y, bool)
{
    return __test_and_set32 (&__x, __y);
}


inline int
__rw_atomic_exchange (int &__x, int __y, bool)
{
    return __rw_atomic_exchange (_RWSTD_REINTERPRET_CAST (unsigned int&, __x),
                                 _RWSTD_STATIC_CAST (unsigned int, __y), false);
}


#  if _RWSTD_INT_SIZE < _RWSTD_LONG_SIZE

inline unsigned long
__rw_atomic_preincrement (unsigned long &__x, bool)
{
    return __add_then_test (&__x, 1);
}


inline long
__rw_atomic_preincrement (long &__x, bool)
{
    return __rw_atomic_preincrement (_RWSTD_REINTERPRET_CAST(unsigned long&,
                                                             __x),
                                     false);
}

inline unsigned long
__rw_atomic_predecrement (unsigned long &__x, bool)
{
    return __add_then_test (&__x, -1);
}


inline long
__rw_atomic_predecrement (long &__x, bool)
{
    return __rw_atomic_predecrement (_RWSTD_REINTERPRET_CAST (unsigned long&,
                                                              __x),
                                     false);
}


inline unsigned long
__rw_atomic_exchange (unsigned long &__x, unsigned long __y, bool)
{
    return __test_and_set (&__x, __y);
}


inline long
__rw_atomic_exchange (long &__x, long __y, bool)
{
    return __rw_atomic_exchange (_RWSTD_REINTERPRET_CAST (unsigned long&, __x),
                                 _RWSTD_STATIC_CAST (unsigned long, __y),
                                 false);
}

#  endif   // _RWSTD_INT_SIZE < _RWSTD_LONG_SIZE

/********************** PA-RISC 2.0 ************************************/

#elif defined (_PA_RISC2_0)

extern "C" {

// special constraint: the value of both the argument and the result
// must not be zero

int __rw_atomic_incr32 (int*);
int __rw_atomic_decr32 (int*);
int __rw_atomic_xchg32 (int*, int);

}   // extern "C"


inline int
__rw_string_atomic_preincrement (int &__x, bool)
{
    return  __rw_atomic_incr32 (&__x);
}


inline unsigned
__rw_string_atomic_preincrement (unsigned &__x, bool)
{
  
    return __rw_atomic_preincrement (_RWSTD_REINTERPRET_CAST(int&, __x),
                                     false);
}


inline int
__rw_string_atomic_predecrement (int &__x, bool)
{
    return  __rw_atomic_decr32 (&__x);
}


inline unsigned
__rw_string_atomic_predecrement (unsigned &__x, bool)
{
  
    return __rw_atomic_predecrement (_RWSTD_REINTERPRET_CAST(int&, __x),
                                     false);
}


inline int
__rw_string_atomic_exchange (int &__x, int __y, bool)
{
    return __rw_atomic_xchg32 (&__x, __y);
}


inline unsigned
__rw_string_atomic_exchange (unsigned &__x, unsigned __y, bool)
{
    return __rw_string_atomic_exchange (_RWSTD_REINTERPRET_CAST(int&, __x),
                                        _RWSTD_STATIC_CAST(int,__y),
                                        false);
} 

/********************** i386/gcc **************************************/

#elif defined (__i386__) && (defined (__GNUG__) || defined (__INTEL_COMPILER))

extern "C" {

char __rw_atomic_add8 (char*, int);
short __rw_atomic_add16 (short*, short);
int __rw_atomic_add32 (int*, int);

char __rw_atomic_xchg8 (char*, char);
short __rw_atomic_xchg16 (short*, short);
int __rw_atomic_xchg32 (int*, int);

}   // extern "C"


inline char
__rw_atomic_preincrement (char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (char));
    return __rw_atomic_add8 (&__x, +1);
}


inline signed char
__rw_atomic_preincrement (signed char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (signed char));
    return __rw_atomic_add8 (_RWSTD_REINTERPRET_CAST (char*, &__x), +1);
}


inline unsigned char
__rw_atomic_preincrement (unsigned char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (unsigned char));
    return __rw_atomic_add8 (_RWSTD_REINTERPRET_CAST (char*, &__x), +1);
}


inline short
__rw_atomic_preincrement (short &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (short));
    return __rw_atomic_add16 (&__x, +1);
}


inline unsigned short
__rw_atomic_preincrement (unsigned short &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (unsigned short));
    return __rw_atomic_add16 (_RWSTD_REINTERPRET_CAST (short*, &__x), +1);
}


inline int
__rw_atomic_preincrement (int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (int));
    return __rw_atomic_add32 (&__x, 1);
}


inline unsigned int
__rw_atomic_preincrement (unsigned int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (unsigned int));
    return __rw_atomic_add32 (_RWSTD_REINTERPRET_CAST (int*, &__x), 1);
}


inline char
__rw_atomic_predecrement (char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (char));
    return __rw_atomic_add8 (&__x, -1);
}


inline signed char
__rw_atomic_predecrement (signed char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (signed char));
    return __rw_atomic_add8 (_RWSTD_REINTERPRET_CAST (char*, &__x), -1);
}


inline unsigned char
__rw_atomic_predecrement (unsigned char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (unsigned char));
    return __rw_atomic_add8 (_RWSTD_REINTERPRET_CAST (char*, &__x), -1);
}


inline short
__rw_atomic_predecrement (short &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (short));
    return __rw_atomic_add16 (&__x, -1);
}


inline unsigned short
__rw_atomic_predecrement (unsigned short &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (unsigned short));
    return __rw_atomic_add16 (_RWSTD_REINTERPRET_CAST (short*, &__x), -1);
}


inline int
__rw_atomic_predecrement (int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (int));
    return __rw_atomic_add32 (&__x, -1);
}


inline unsigned int
__rw_atomic_predecrement (unsigned int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (unsigned int));
    return __rw_atomic_add32 (_RWSTD_REINTERPRET_CAST (int*, &__x), -1);
}


inline char
__rw_atomic_exchange (char &__x, char __y, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (char));
    return __rw_atomic_xchg8 (&__x, __y);
}


inline signed char
__rw_atomic_exchange (signed char &__x, signed char __y, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (signed char));
    return __rw_atomic_xchg8 (_RWSTD_REINTERPRET_CAST (char*, &__x),
                              _RWSTD_STATIC_CAST (char, __y));
}


inline unsigned char
__rw_atomic_exchange (unsigned char &__x, unsigned char __y, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (unsigned char));
    return __rw_atomic_xchg8 (_RWSTD_REINTERPRET_CAST (char*, &__x),
                              _RWSTD_STATIC_CAST (char, __y));
}


inline short
__rw_atomic_exchange (short &__x, short __y, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (short));
    return __rw_atomic_xchg16 (&__x, __y);
}


inline unsigned short
__rw_atomic_exchange (unsigned short &__x, unsigned short __y, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (unsigned short));
    return __rw_atomic_xchg16 (_RWSTD_REINTERPRET_CAST (short*, &__x),
                               _RWSTD_STATIC_CAST (short, __y));
}


inline int
__rw_atomic_exchange (int &__x, int __y, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (int));
    return __rw_atomic_xchg32 (&__x, __y);
}


inline unsigned int
__rw_atomic_exchange (unsigned int &__x, unsigned int __y, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (unsigned int));
    return __rw_atomic_xchg32 (_RWSTD_REINTERPRET_CAST (int*, &__x),
                               _RWSTD_STATIC_CAST (int, __y));
}


/********************** WIN 32/64 ************************************/

#elif defined (_WIN32)

// Interlocked[In|De]crement functions atomically modify their argument
// and return the new value

// InterlockedExchange atomically sets the value pointed to by the first
// argument to that of the second argument and returns the original value

inline int
__rw_atomic_preincrement (int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (sizeof __x == sizeof (long));
    return InterlockedIncrement (_RWSTD_REINTERPRET_CAST (long*, &__x));
}


inline unsigned int
__rw_atomic_preincrement (unsigned int &__x, bool)
{
    return __rw_atomic_preincrement (_RWSTD_REINTERPRET_CAST (long&, __x),
                                     false);
}


inline int
__rw_atomic_predecrement (int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (sizeof __x == sizeof (long));
    return InterlockedDecrement (_RWSTD_REINTERPRET_CAST (long*, &__x));
}


inline unsigned int
__rw_atomic_predecrement (unsigned int &__x, bool)
{
    return __rw_atomic_predecrement (_RWSTD_REINTERPRET_CAST (int&, __x),
                                     false);
}


inline int
__rw_atomic_exchange (int &__x, int __y, bool)
{
    _RWSTD_COMPILE_ASSERT (sizeof __x == sizeof (long));
    return InterlockedExchange (_RWSTD_REINTERPRET_CAST (long*, &__x),
                                _RWSTD_STATIC_CAST (long, __y));
}


inline unsigned int
__rw_atomic_exchange (unsigned int &__x, unsigned int __y, bool)
{
    return __rw_atomic_exchange (_RWSTD_REINTERPRET_CAST (int&, __x),
                                 _RWSTD_STATIC_CAST (int, __y), false);
}

/********************** IA64/x86_64 ***********************************/

#elif defined (__ia64) || defined (__x86_64)

extern "C" {

_RWSTD_INT8_T  __rw_atomic_xchg8  (_RWSTD_INT8_T*,  _RWSTD_INT8_T);
_RWSTD_INT16_T __rw_atomic_xchg16 (_RWSTD_INT16_T*, _RWSTD_INT16_T);
_RWSTD_INT32_T __rw_atomic_xchg32 (_RWSTD_INT32_T*, _RWSTD_INT32_T);


_RWSTD_INT8_T  __rw_atomic_add8  (_RWSTD_INT8_T*,  _RWSTD_INT8_T);
_RWSTD_INT16_T __rw_atomic_add16 (_RWSTD_INT16_T*, _RWSTD_INT16_T);
_RWSTD_INT32_T __rw_atomic_add32 (_RWSTD_INT32_T*, _RWSTD_INT32_T);

#ifdef _RWSTD_INT64_T

_RWSTD_INT64_T __rw_atomic_xchg64 (_RWSTD_INT64_T*, _RWSTD_INT64_T);
_RWSTD_INT64_T __rw_atomic_add64 (_RWSTD_INT64_T*, _RWSTD_INT64_T);

#endif   // _RWSTD_INT64_T

}   // extern "C"


inline char
__rw_atomic_preincrement (char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (char));

    return __rw_atomic_add8 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT8_T*, &__x),
                             +1);
}


inline signed char
__rw_atomic_preincrement (signed char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (signed char));

    return __rw_atomic_add8 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT8_T*, &__x),
                             +1);
}


inline unsigned char
__rw_atomic_preincrement (unsigned char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (unsigned char));

    return __rw_atomic_add8 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT8_T*, &__x),
                             +1);
}


inline short
__rw_atomic_preincrement (short &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (short));

    return __rw_atomic_add16 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT16_T*, &__x),
                              +1);
}


inline unsigned short
__rw_atomic_preincrement (unsigned short &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (unsigned short));

    return __rw_atomic_add16 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT16_T*, &__x),
                              +1);
}


inline int
__rw_atomic_preincrement (int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (int));

    return __rw_atomic_add32 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT32_T*, &__x),
                              +1);
}


inline unsigned int
__rw_atomic_preincrement (unsigned int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (unsigned int));

    return __rw_atomic_add32 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT32_T*, &__x),
                              +1);
}


#if 4 < _RWSTD_LONG_SIZE

inline long
__rw_atomic_preincrement (long &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (long));

    return __rw_atomic_add64 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT64_T*, &__x),
                              +1);
}


inline unsigned long
__rw_atomic_preincrement (unsigned long &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (unsigned long));

    return __rw_atomic_add64 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT64_T*, &__x),
                              +1);
}

#endif   // _RWSTD_LONG_SIZE


inline char
__rw_atomic_predecrement (char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (char));

    return __rw_atomic_add8 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT8_T*, &__x),
                             -1);
}


inline signed char
__rw_atomic_predecrement (signed char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (signed char));

    return __rw_atomic_add8 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT8_T*, &__x),
                             -1);
}


inline unsigned char
__rw_atomic_predecrement (unsigned char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (unsigned char));

    return __rw_atomic_add8 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT8_T*, &__x),
                             -1);
}


inline short
__rw_atomic_predecrement (short &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (short));

    return __rw_atomic_add16 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT16_T*, &__x),
                              -1);
}


inline unsigned short
__rw_atomic_predecrement (unsigned short &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (unsigned short));

    return __rw_atomic_add16 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT16_T*, &__x),
                              -1);
}


inline int
__rw_atomic_predecrement (int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (int));

    return __rw_atomic_add32 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT32_T*, &__x),
                              -1);
}


inline unsigned int
__rw_atomic_predecrement (unsigned int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (unsigned int));

    return __rw_atomic_add32 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT32_T*, &__x),
                              -1);
}


#if 4 < _RWSTD_LONG_SIZE

inline long
__rw_atomic_predecrement (long &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (long));

    return __rw_atomic_add64 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT64_T*, &__x),
                              -1);
}


inline unsigned long
__rw_atomic_predecrement (unsigned long &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (long));

    return __rw_atomic_add64 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT64_T*, &__x),
                              -1);
}

#endif   // _RWSTD_LONG_SIZE


inline char
__rw_atomic_exchange (char &__x, char __y, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (char));

    return __rw_atomic_xchg8 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT8_T*, &__x),
                              _RWSTD_STATIC_CAST (_RWSTD_INT8_T, __y));
}


inline signed char
__rw_atomic_exchange (signed char &__x, signed char __y, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (signed char));

    return __rw_atomic_xchg8 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT8_T*, &__x),
                              _RWSTD_STATIC_CAST (_RWSTD_INT8_T, __y));
}


inline unsigned char
__rw_atomic_exchange (unsigned char &__x, unsigned char __y, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (unsigned char));

    return __rw_atomic_xchg8 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT8_T*, &__x),
                              _RWSTD_STATIC_CAST (_RWSTD_INT8_T, __y));
}


inline short
__rw_atomic_exchange (short &__x, short __y, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (short));

    return __rw_atomic_xchg16 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT16_T*, &__x),
                               _RWSTD_STATIC_CAST (_RWSTD_INT16_T, __y));
}


inline unsigned short
__rw_atomic_exchange (unsigned short &__x, unsigned short __y, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (unsigned short));

    return __rw_atomic_xchg16 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT16_T*, &__x),
                               _RWSTD_STATIC_CAST (_RWSTD_INT16_T, __y));
}


inline int
__rw_atomic_exchange (int &__x, int __y, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (int));

    return __rw_atomic_xchg32 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT32_T*, &__x),
                               _RWSTD_STATIC_CAST (_RWSTD_INT32_T, __y));
}


inline unsigned int
__rw_atomic_exchange (unsigned int &__x, unsigned int __y, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (unsigned int));

    return __rw_atomic_xchg32 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT32_T*, &__x),
                               _RWSTD_STATIC_CAST (_RWSTD_INT32_T, __y));
}


#  if 4 < _RWSTD_LONG_SIZE

inline long
__rw_atomic_exchange (long &__x, long __y, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (long));

    return __rw_atomic_xchg64 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT64_T*, &__x),
                               _RWSTD_STATIC_CAST (_RWSTD_INT64_T, __y));
}


inline unsigned long
__rw_atomic_exchange (unsigned long &__x, unsigned long __y, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (unsigned long));

    return __rw_atomic_xchg64 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT64_T*, &__x),
                               _RWSTD_STATIC_CAST (_RWSTD_INT64_T, __y));
}

#  endif   // _RWSTD_LONG_SIZE == _RWSTD_INT_SIZE

#elif !defined (_RWSTD_NO_ATOMIC_OPS)
#  define _RWSTD_NO_ATOMIC_OPS
#endif   // _RWSTD_NO_ATOMIC_OPS

/********************** generic long functions ************************/

#if _RWSTD_LONG_SIZE == _RWSTD_INT_SIZE

inline long
__rw_atomic_preincrement (long &__x, bool)
{
    return __rw_atomic_preincrement (_RWSTD_REINTERPRET_CAST (int&, __x),
                                     false);
}

inline unsigned long
__rw_atomic_preincrement (unsigned long &__x, bool)
{
    return __rw_atomic_preincrement (_RWSTD_REINTERPRET_CAST (int&, __x),
                                     false);
}

inline long
__rw_atomic_predecrement (long &__x, bool)
{
    return __rw_atomic_predecrement (_RWSTD_REINTERPRET_CAST (int&, __x),
                                     false);
}

inline unsigned long
__rw_atomic_predecrement (unsigned long &__x, bool)
{
    return __rw_atomic_predecrement (_RWSTD_REINTERPRET_CAST (int&, __x),
                                     false);
}

inline long
__rw_atomic_exchange (long &__x, long __y, bool)
{
    return __rw_atomic_exchange (_RWSTD_REINTERPRET_CAST (int&, __x),
                                 _RWSTD_STATIC_CAST (int, __y),
                                 false);
}

inline unsigned long
__rw_atomic_exchange (unsigned long &__x,
                      unsigned long __y, bool)
{
    return __rw_atomic_exchange (_RWSTD_REINTERPRET_CAST (int&, __x),
                                 _RWSTD_STATIC_CAST (int, __y),
                                 false);
}

#endif   // _RWSTD_LONG_SIZE == _RWSTD_INT_SIZE

/********************** generic long long functions *******************/

#ifdef _RWSTD_LONG_LONG
#  if _RWSTD_LLONG_SIZE == _RWSTD_LONG_SIZE

inline _RWSTD_LONG_LONG
__rw_atomic_preincrement (_RWSTD_LONG_LONG &__x, bool)
{
    return __rw_atomic_preincrement (_RWSTD_REINTERPRET_CAST (long&, __x),
                                     false);
}

inline unsigned _RWSTD_LONG_LONG
__rw_atomic_preincrement (unsigned _RWSTD_LONG_LONG &__x, bool)
{
    return __rw_atomic_preincrement (_RWSTD_REINTERPRET_CAST (long&, __x),
                                     false);
}

inline _RWSTD_LONG_LONG
__rw_atomic_predecrement (_RWSTD_LONG_LONG &__x, bool)
{
    return __rw_atomic_predecrement (_RWSTD_REINTERPRET_CAST (long&, __x),
                                     false);
}

inline unsigned _RWSTD_LONG_LONG
__rw_atomic_predecrement (unsigned _RWSTD_LONG_LONG &__x, bool)
{
    return __rw_atomic_predecrement (_RWSTD_REINTERPRET_CAST (long&, __x),
                                     false);
}

inline _RWSTD_LONG_LONG
__rw_atomic_exchange (_RWSTD_LONG_LONG &__x, _RWSTD_LONG_LONG __y, bool)
{
    return __rw_atomic_exchange (_RWSTD_REINTERPRET_CAST (long&, __x),
                                 _RWSTD_STATIC_CAST (long, __y),
                                 false);
}

inline unsigned _RWSTD_LONG_LONG
__rw_atomic_exchange (unsigned _RWSTD_LONG_LONG &__x,
                      unsigned _RWSTD_LONG_LONG __y, bool)
{
    return __rw_atomic_exchange (_RWSTD_REINTERPRET_CAST (long&, __x),
                                 _RWSTD_STATIC_CAST (long, __y),
                                 false);
}

#  endif   // _RWSTD_LLONG_SIZE == _RWSTD_LONG_SIZE
#endif   // _RWSTD_LONG_LONG


}   // namespace __rw


#else   // if !defined (_RWSTD_REENTRANT)


_RWSTD_NAMESPACE (__rw) { 

// atomic in a single-threaded environment
template <class _TypeT, class _TypeU>
inline
_TypeT __rw_atomic_exchange (_TypeT &__t, const _TypeU &__u, bool)
{
    _TypeT __tmp = __t;
    __t = _TypeT (__u);
    return __tmp;
}


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


_RWSTD_NAMESPACE (__rw) { 

// available in all environments (ST and MT), used along with
// __rw_atomic_exchange<>() from conditional expressions in iostreams
template <class _TypeT, class _TypeU>
inline
_TypeT __rw_ordinary_exchange (_TypeT &__t, const _TypeU &__u)
{
    _TypeT __tmp = __t;
    __t = __u;
    return __tmp;
}

}   // namespace __rw


#endif   // _RWSTD_RW_MUTEX_H_INCLUDED
