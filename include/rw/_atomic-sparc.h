/***************************************************************************
 *
 * _atomic-sparc.h - definitions of inline functions for atomic
 *                   operations on SPARC platform
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

_RWSTD_NAMESPACE (__rw) {

#define _RWSTD_NO_CHAR_ATOMIC_OPS
#define _RWSTD_NO_SHORT_ATOMIC_OPS
#define _RWSTD_NO_LLONG_ATOMIC_OPS

extern "C" {

// define in assembler file "atomic-sparc.s" and "atomic-sparc64.s"
// exchange returns the original value, and add returns the incremented
// value

int  __rw_atomic_xchg32 (int*, int);
int  __rw_atomic_add32 (int*, int);

#  if defined (__sparcv9) || defined (__sparcv9__)

long __rw_atomic_xchg64 (long*, long);
long __rw_atomic_add64 (long*, long);

#  endif

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

#  else   // _RWSTD_INT_SIZE == _RWSTD_LONG_SIZE
#    define _RWSTD_NO_LONG_ATOMIC_OPS
#  endif   // _RWSTD_INT_SIZE < _RWSTD_LONG_SIZE

}   // namespace __rw
