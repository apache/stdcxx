/***************************************************************************
 *
 * _atomic-parisc.h - definitions of inline functions for atomic
 *                    operations on PA RISC platform
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

#define _RWSTD_NO_CHAR_ATOMIC_OPS
#define _RWSTD_NO_SHORT_ATOMIC_OPS
#define _RWSTD_NO_INT_ATOMIC_OPS
#define _RWSTD_NO_LONG_ATOMIC_OPS
#define _RWSTD_NO_LLONG_ATOMIC_OPS

_RWSTD_NAMESPACE (__rw) {
    
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
  
    return __rw_string_atomic_preincrement (_RWSTD_REINTERPRET_CAST(int&, __x),
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
  
    return __rw_string_atomic_predecrement (_RWSTD_REINTERPRET_CAST(int&, __x),
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

}   // namespace __rw
