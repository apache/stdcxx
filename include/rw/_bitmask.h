/***************************************************************************
 *
 * _bitmask.h - helper definitions for bitmask types
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _RWSTD_RW_BITMASK_H_INCLUDED
#define _RWSTD_RW_BITMASK_H_INCLUDED

#ifndef _RWSTD_RW_DEFS_H_INCLUDED
#  include <rw/_defs.h>
#endif   // _RWSTD_RW_DEFS_H_INCLUDED


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


#endif   // _RWSTD_RW_BITMASK_H_INCLUDED
