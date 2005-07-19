/***************************************************************************
 *
 * _bitmask.h - helper definitions for bitmask types
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
 *
 * $Id: //stdlib/dev/include/rw/_bitmask.h#6 $
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

#ifndef _RWSTD_BITMASK_H_INCLUDED
#define _RWSTD_BITMASK_H_INCLUDED


#include <rw/_defs.h>


#ifndef _RWSTD_NO_STATIC_CONST_MEMBER_INIT
#  define _RWSTD_BITMASK_ENUM(Bitmask)  Bitmask

#  define _RWSTD_DEFINE_BITMASK_OPERATORS(Bitmask)                 \
                                                                   \
    inline Bitmask& operator&= (Bitmask &__lhs, Bitmask __rhs) {   \
        return __lhs = Bitmask (long (__lhs) & __rhs);             \
    }                                                              \
                                                                   \
    inline Bitmask& operator|= (Bitmask &__lhs, Bitmask __rhs) {   \
        return __lhs = Bitmask (long (__lhs) | __rhs);             \
    }                                                              \
                                                                   \
    inline Bitmask& operator^= (Bitmask &__lhs, Bitmask __rhs) {   \
        return __lhs = Bitmask (long (__lhs) ^ __rhs);             \
    }                                                              \
                                                                   \
    inline Bitmask operator& (Bitmask __lhs, Bitmask __rhs) {      \
        return __lhs &= __rhs;                                     \
    }                                                              \
                                                                   \
    inline Bitmask operator| (Bitmask __lhs, Bitmask __rhs) {      \
        return __lhs |= __rhs;                                     \
    }                                                              \
                                                                   \
    inline Bitmask operator^ (Bitmask __lhs, Bitmask __rhs) {      \
        return __lhs ^= __rhs;                                     \
    }                                                              \
                                                                   \
    inline Bitmask operator~ (Bitmask __rhs) {                     \
        return Bitmask (~long (__rhs));                            \
    }                                                              \
    typedef void __rw_unused_typedef

#else   // if defined (_RWSTD_NO_STATIC_CONST_MEMBER_INIT)
#  define _RWSTD_BITMASK_ENUM(ignore)   int

#  define _RWSTD_DEFINE_BITMASK_OPERATORS(ignore)   \
   typedef void __rw_unused_typedef

#endif   // _RWSTD_NO_STATIC_CONST_MEMBER_INIT


#endif   // _RWSTD_BITMASK_H_INCLUDED
