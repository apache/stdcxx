/***************************************************************************
 *
 * _atomic-sync.h - definitions of inline functions for atomic
 *                  operations using __sync_xxx() functions set
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

#if defined (__ia64) && defined (__INTEL_COMPILER)
#  include <ia64intrin.h>
#endif

_RWSTD_NAMESPACE (__rw) {

inline char
__rw_atomic_preincrement (char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (char));
    return __sync_add_and_fetch (&__x, 1);
}


inline signed char
__rw_atomic_preincrement (signed char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (signed char));
    return __sync_add_and_fetch (&__x, 1);
}


inline unsigned char
__rw_atomic_preincrement (unsigned char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (unsigned char));
    return __sync_add_and_fetch (&__x, 1);
}


inline short
__rw_atomic_preincrement (short &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (short));
    return __sync_add_and_fetch (&__x, 1);
}


inline unsigned short
__rw_atomic_preincrement (unsigned short &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (unsigned short));
    return __sync_add_and_fetch (&__x, 1);
}


inline int
__rw_atomic_preincrement (int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (int));
    return __sync_add_and_fetch (&__x, 1);
}


inline unsigned int
__rw_atomic_preincrement (unsigned int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (unsigned int));
    return __sync_add_and_fetch (&__x, 1);
}


inline char
__rw_atomic_predecrement (char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (char));
    return __sync_sub_and_fetch (&__x, 1);
}


inline signed char
__rw_atomic_predecrement (signed char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (signed char));
    return __sync_sub_and_fetch (&__x, 1);
}


inline unsigned char
__rw_atomic_predecrement (unsigned char &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (unsigned char));
    return __sync_sub_and_fetch (&__x, 1);
}


inline short
__rw_atomic_predecrement (short &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (short));
    return __sync_sub_and_fetch (&__x, 1);
}


inline unsigned short
__rw_atomic_predecrement (unsigned short &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (unsigned short));
    return __sync_sub_and_fetch (&__x, 1);
}


inline int
__rw_atomic_predecrement (int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (int));
    return __sync_sub_and_fetch (&__x, 1);
}


inline unsigned int
__rw_atomic_predecrement (unsigned int &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (unsigned int));
    return __sync_sub_and_fetch (&__x, 1);
}


inline char
__rw_atomic_exchange (char &__x, char __y, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (char));
    return __sync_lock_test_and_set (&__x, __y);
}


inline signed char
__rw_atomic_exchange (signed char &__x, signed char __y, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (signed char));
    return __sync_lock_test_and_set (&__x, __y);
}


inline unsigned char
__rw_atomic_exchange (unsigned char &__x, unsigned char __y, bool)
{
    _RWSTD_COMPILE_ASSERT (1 == sizeof (unsigned char));
    return __sync_lock_test_and_set (&__x, __y);
}


inline short
__rw_atomic_exchange (short &__x, short __y, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (short));
    return __sync_lock_test_and_set (&__x, __y);
}


inline unsigned short
__rw_atomic_exchange (unsigned short &__x, unsigned short __y, bool)
{
    _RWSTD_COMPILE_ASSERT (2 == sizeof (unsigned short));
    return __sync_lock_test_and_set (&__x, __y);
}


inline int
__rw_atomic_exchange (int &__x, int __y, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (int));
    return __sync_lock_test_and_set (&__x, __y);
}


inline unsigned int
__rw_atomic_exchange (unsigned int &__x, unsigned int __y, bool)
{
    _RWSTD_COMPILE_ASSERT (4 == sizeof (unsigned int));
    return __sync_lock_test_and_set (&__x, __y);
}


#if defined (__ia64) || defined (__x86_64)
#  if 4 < _RWSTD_LONG_SIZE

inline long
__rw_atomic_preincrement (long &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (long));
    return __sync_add_and_fetch (&__x, 1);
}


inline unsigned long
__rw_atomic_preincrement (unsigned long &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (unsigned long));
    return __sync_add_and_fetch (&__x, 1);
}


inline long
__rw_atomic_predecrement (long &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (long));
    return __sync_sub_and_fetch (&__x, 1);
}


inline unsigned long
__rw_atomic_predecrement (unsigned long &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (unsigned long));
    return __sync_sub_and_fetch (&__x, 1);
}


inline long
__rw_atomic_exchange (long &__x, long __y, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (long));
    return __sync_lock_test_and_set (&__x, __y);
}


inline unsigned long
__rw_atomic_exchange (unsigned long &__x, unsigned long __y, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (unsigned long));
    return __sync_lock_test_and_set (&__x, __y);
}

#  else
#    define _RWSTD_NO_LONG_ATOMIC_OPS
#  endif   // 4 < _RWSTD_LONG_SIZE


#  if defined (_RWSTD_LONG_LONG) && (_RWSTD_LLONG_SIZE > _RWSTD_LONG_SIZE)

inline _RWSTD_LONG_LONG
__rw_atomic_preincrement (_RWSTD_LONG_LONG &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (_RWSTD_LONG_LONG));
    return __sync_add_and_fetch (&__x, 1);
}


inline unsigned _RWSTD_LONG_LONG
__rw_atomic_preincrement (unsigned _RWSTD_LONG_LONG &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (unsigned _RWSTD_LONG_LONG));
    return __sync_add_and_fetch (&__x, 1);
}


inline _RWSTD_LONG_LONG
__rw_atomic_predecrement (_RWSTD_LONG_LONG &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (_RWSTD_LONG_LONG));
    return __sync_sub_and_fetch (&__x, 1);
}


inline unsigned _RWSTD_LONG_LONG
__rw_atomic_predecrement (unsigned _RWSTD_LONG_LONG &__x, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (unsigned _RWSTD_LONG_LONG));
    return __sync_sub_and_fetch (&__x, 1);
}


inline _RWSTD_LONG_LONG
__rw_atomic_exchange (_RWSTD_LONG_LONG &__x, _RWSTD_LONG_LONG __y, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (_RWSTD_LONG_LONG));
    return __sync_lock_test_and_set (&__x, __y);
}


inline unsigned _RWSTD_LONG_LONG
__rw_atomic_exchange (unsigned _RWSTD_LONG_LONG &__x,
                      unsigned _RWSTD_LONG_LONG __y, bool)
{
    _RWSTD_COMPILE_ASSERT (8 == sizeof (unsigned _RWSTD_LONG_LONG));
    return __sync_lock_test_and_set (&__x, __y);
}

#  else
#    define _RWSTD_NO_LLONG_ATOMIC_OPS
#  endif   // _RWSTD_LONG_LONG && _RWSTD_LLONG_SIZE > _RWSTD_LONG_SIZE

#else   // !__ia64 && !__x86_64
#  define _RWSTD_NO_LONG_ATOMIC_OPS
#  define _RWSTD_NO_LLONG_ATOMIC_OPS
#endif   // __ia64 || __x86_64

}   // namespace __rw
