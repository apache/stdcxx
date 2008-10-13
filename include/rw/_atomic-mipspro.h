/***************************************************************************
 *
 * _atomic-mipspro.h - definitions of inline functions for atomic
 *                     operations on MIPS platform
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

#  include <mutex.h>

#define _RWSTD_NO_CHAR_ATOMIC_OPS
#define _RWSTD_NO_SHORT_ATOMIC_OPS
#define _RWSTD_NO_LLONG_ATOMIC_OPS

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

#  else   // _RWSTD_INT_SIZE == _RWSTD_LONG_SIZE
#    define _RWSTD_NO_LONG_ATOMIC_OPS
#  endif   // _RWSTD_INT_SIZE < _RWSTD_LONG_SIZE

}   // namespace __rw
