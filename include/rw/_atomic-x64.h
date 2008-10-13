/***************************************************************************
 *
 * _atomic-x64.h - definitions of inline functions for atomic
 *                 operations on Intel IA64 and X64 platforms
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

#ifdef _MSC_VER
extern "C" long __cdecl _InterlockedIncrement (volatile long*);
extern "C" long __cdecl _InterlockedDecrement (volatile long*);
extern "C" long __cdecl _InterlockedExchange (volatile long*, long);
#  ifdef _RWSTD_MSVC
#    pragma intrinsic (_InterlockedIncrement)
#    pragma intrinsic (_InterlockedDecrement)
#    pragma intrinsic (_InterlockedExchange)
#  endif   // _RWSTD_MSVC

#  if defined (_RWSTD_MSVC) && _RWSTD_MSVC >= 1400
extern "C" short __cdecl _InterlockedIncrement16 (volatile short*);
extern "C" short __cdecl _InterlockedDecrement16 (volatile short*);
#    pragma intrinsic (_InterlockedIncrement16)
#    pragma intrinsic (_InterlockedDecrement16)
#  endif   // _RWSTD_MSVC >= 1400

#  ifdef _M_X64
extern "C" long long __cdecl _InterlockedIncrement64 (volatile long long*);
extern "C" long long __cdecl _InterlockedDecrement64 (volatile long long*);
extern "C" long long __cdecl _InterlockedExchange64 (volatile long long*,
                                                     long long);
#    ifdef _RWSTD_MSVC
#      pragma intrinsic (_InterlockedIncrement64)
#      pragma intrinsic (_InterlockedDecrement64)
#      pragma intrinsic (_InterlockedExchange64)
#    endif   // _RWSTD_MSVC
#  endif   // _M_X64
#endif   // _MSC_VER


_RWSTD_NAMESPACE (__rw) {

extern "C" {

_RWSTD_EXPORT _RWSTD_INT8_T
__rw_atomic_xchg8  (_RWSTD_INT8_T*,  _RWSTD_INT8_T);

_RWSTD_EXPORT _RWSTD_INT16_T
__rw_atomic_xchg16 (_RWSTD_INT16_T*, _RWSTD_INT16_T);

_RWSTD_EXPORT _RWSTD_INT32_T
__rw_atomic_xchg32 (_RWSTD_INT32_T*, _RWSTD_INT32_T);


_RWSTD_EXPORT _RWSTD_INT8_T
__rw_atomic_add8  (_RWSTD_INT8_T*,  _RWSTD_INT8_T);

_RWSTD_EXPORT _RWSTD_INT16_T
__rw_atomic_add16 (_RWSTD_INT16_T*, _RWSTD_INT16_T);

_RWSTD_EXPORT _RWSTD_INT32_T
__rw_atomic_add32 (_RWSTD_INT32_T*, _RWSTD_INT32_T);

#ifdef _RWSTD_INT64_T

_RWSTD_EXPORT _RWSTD_INT64_T
__rw_atomic_xchg64 (_RWSTD_INT64_T*, _RWSTD_INT64_T);

_RWSTD_EXPORT _RWSTD_INT64_T
__rw_atomic_add64 (_RWSTD_INT64_T*, _RWSTD_INT64_T);

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

#if defined (_RWSTD_MSVC) && _RWSTD_MSVC >= 1400
    return _InterlockedIncrement16 (&__x);
#else
    return __rw_atomic_add16 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT16_T*, &__x),
                              +1);
#endif
}


inline unsigned short
__rw_atomic_preincrement (unsigned short &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (unsigned short));

#if defined (_RWSTD_MSVC) && _RWSTD_MSVC >= 1400
    return _InterlockedIncrement16 (_RWSTD_REINTERPRET_CAST (short*, &__x));
#else
    return __rw_atomic_add16 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT16_T*, &__x),
                              +1);
#endif
}


inline int
__rw_atomic_preincrement (int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (int));

#ifdef _MSC_VER
    return _InterlockedIncrement (_RWSTD_REINTERPRET_CAST (long*, &__x));
#else
    return __rw_atomic_add32 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT32_T*, &__x),
                              +1);
#endif
}


inline unsigned int
__rw_atomic_preincrement (unsigned int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (unsigned int));

#ifdef _MSC_VER
    return _InterlockedIncrement (_RWSTD_REINTERPRET_CAST (long*, &__x));
#else
    return __rw_atomic_add32 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT32_T*, &__x),
                              +1);
#endif
}


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

#if defined (_RWSTD_MSVC) && _RWSTD_MSVC >= 1400
    return _InterlockedDecrement16 (&__x);
#else
    return __rw_atomic_add16 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT16_T*, &__x),
                              -1);
#endif
}


inline unsigned short
__rw_atomic_predecrement (unsigned short &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (unsigned short));

#if defined (_RWSTD_MSVC) && _RWSTD_MSVC >= 1400
    return _InterlockedDecrement16 (_RWSTD_REINTERPRET_CAST (short*, &__x));
#else
    return __rw_atomic_add16 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT16_T*, &__x),
                              -1);
#endif
}


inline int
__rw_atomic_predecrement (int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (int));

#ifdef _MSC_VER
    return _InterlockedDecrement (_RWSTD_REINTERPRET_CAST (long*, &__x));
#else
    return __rw_atomic_add32 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT32_T*, &__x),
                              -1);
#endif
}


inline unsigned int
__rw_atomic_predecrement (unsigned int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (unsigned int));

#ifdef _MSC_VER
    return _InterlockedDecrement (_RWSTD_REINTERPRET_CAST (long*, &__x));
#else
    return __rw_atomic_add32 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT32_T*, &__x),
                              -1);
#endif
}


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

#ifdef _MSC_VER
    return _InterlockedExchange (_RWSTD_REINTERPRET_CAST (long*, &__x),
                                 _RWSTD_STATIC_CAST (long, __y));
#else
    return __rw_atomic_xchg32 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT32_T*, &__x),
                               _RWSTD_STATIC_CAST (_RWSTD_INT32_T, __y));
#endif
}


inline unsigned int
__rw_atomic_exchange (unsigned int &__x, unsigned int __y, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (unsigned int));

#ifdef _MSC_VER
    return _InterlockedExchange (_RWSTD_REINTERPRET_CAST (long*, &__x),
                                 _RWSTD_STATIC_CAST (long, __y));
#else
    return __rw_atomic_xchg32 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT32_T*, &__x),
                               _RWSTD_STATIC_CAST (_RWSTD_INT32_T, __y));
#endif
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
    _RWSTD_COMPILE_ASSERT (8 == sizeof (unsigned long));

    return __rw_atomic_add64 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT64_T*, &__x),
                              -1);
}


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

#else
#  define _RWSTD_NO_LONG_ATOMIC_OPS
#endif   // 4 < _RWSTD_LONG_SIZE


#if defined (_RWSTD_LONG_LONG) && (_RWSTD_LLONG_SIZE > _RWSTD_LONG_SIZE)

inline _RWSTD_LONG_LONG
__rw_atomic_preincrement (_RWSTD_LONG_LONG &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (_RWSTD_LONG_LONG));

#ifdef _MSC_VER
    return _InterlockedIncrement64 (_RWSTD_REINTERPRET_CAST (__int64*, &__x));
#else
    return __rw_atomic_add64 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT64_T*, &__x),
                              +1);
#endif
}


inline unsigned _RWSTD_LONG_LONG
__rw_atomic_preincrement (unsigned _RWSTD_LONG_LONG &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (unsigned _RWSTD_LONG_LONG));

#ifdef _MSC_VER
    return _InterlockedIncrement64 (_RWSTD_REINTERPRET_CAST (__int64*, &__x));
#else
    return __rw_atomic_add64 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT64_T*, &__x),
                              +1);
#endif
}


inline _RWSTD_LONG_LONG
__rw_atomic_predecrement (_RWSTD_LONG_LONG &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (_RWSTD_LONG_LONG));

#ifdef _MSC_VER
    return _InterlockedDecrement64 (_RWSTD_REINTERPRET_CAST (__int64*, &__x));
#else
    return __rw_atomic_add64 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT64_T*, &__x),
                              -1);
#endif
}


inline unsigned _RWSTD_LONG_LONG
__rw_atomic_predecrement (unsigned _RWSTD_LONG_LONG &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (unsigned _RWSTD_LONG_LONG));

#ifdef _MSC_VER
    return _InterlockedDecrement64 (_RWSTD_REINTERPRET_CAST (__int64*, &__x));
#else
    return __rw_atomic_add64 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT64_T*, &__x),
                              -1);
#endif
}


inline _RWSTD_LONG_LONG
__rw_atomic_exchange (_RWSTD_LONG_LONG &__x, _RWSTD_LONG_LONG __y, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (_RWSTD_LONG_LONG));

#ifdef _MSC_VER
    return _InterlockedExchange64 (_RWSTD_REINTERPRET_CAST (__int64*, &__x),
                                   _RWSTD_STATIC_CAST (__int64, __y));
#else
    return __rw_atomic_xchg64 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT64_T*, &__x),
                               _RWSTD_STATIC_CAST (_RWSTD_INT64_T, __y));
#endif
}


inline unsigned _RWSTD_LONG_LONG
__rw_atomic_exchange (unsigned _RWSTD_LONG_LONG &__x,
                      unsigned _RWSTD_LONG_LONG __y, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (unsigned _RWSTD_LONG_LONG));

#ifdef _MSC_VER
    return _InterlockedExchange64 (_RWSTD_REINTERPRET_CAST (__int64*, &__x),
                                   _RWSTD_STATIC_CAST (__int64, __y));
#else
    return __rw_atomic_xchg64 (_RWSTD_REINTERPRET_CAST (_RWSTD_INT64_T*, &__x),
                               _RWSTD_STATIC_CAST (_RWSTD_INT64_T, __y));
#endif
}

#else
#  define _RWSTD_NO_LLONG_ATOMIC_OPS
#endif   // _RWSTD_LONG_LONG && _RWSTD_LLONG_SIZE > _RWSTD_LONG_SIZE

}   // namespace __rw
