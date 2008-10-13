/***************************************************************************
 *
 * _atomic-deccxx.h - definitions of inline functions for atomic
 *                    operations on ALPHA platform
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

// get declarations of __ATOMIC_XXX intrinsics
#  include <machine/builtins.h>

#define _RWSTD_NO_CHAR_ATOMIC_OPS
#define _RWSTD_NO_SHORT_ATOMIC_OPS
#define _RWSTD_NO_LLONG_ATOMIC_OPS

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

}   // namespace __rw
