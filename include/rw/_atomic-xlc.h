/***************************************************************************
 *
 * _atomic-xlc.h - definitions of inline functions for atomic
 *                 operations on POWERPC platform
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

#include <sys/atomic_op.h>

#define _RWSTD_NO_CHAR_ATOMIC_OPS
#define _RWSTD_NO_SHORT_ATOMIC_OPS
#define _RWSTD_NO_LLONG_ATOMIC_OPS

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

#  else   // _RWSTD_INT_SIZE == _RWSTD_LONG_SIZE
#    define _RWSTD_NO_LONG_ATOMIC_OPS
#  endif   // _RWSTD_INT_SIZE < _RWSTD_LONG_SIZE

}   // namespace __rw
