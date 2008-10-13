/***************************************************************************
 *
 * random.cpp - random number generator
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
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

#define _RWSTD_LIB_SRC

#include <rw/_defs.h>

#ifdef _RWSTD_REENTRANT
#  include <rw/_mutex.h>
#endif   // _RWSTD_REENTRANT

#include <stddef.h>   // for size_t


_RWSTD_NAMESPACE (__rw) {


#ifndef _RWSTD_NO_EXT_PORTABLE_RANDOM_SEQUENCE

// generator produces the same sequence of random numbers
// irrespective of the architecture and width of integers
typedef _RWSTD_UINT32_T __rw_random_t;

#else   // if defined (_RWSTD_NO_EXT_PORTABLE_RANDOM_SEQUENCE)

// generator may produces a different sequence of random numbers
// on different architectures (depending on the width of size_t)
typedef _RWSTD_SIZE_T __rw_random_t;

#endif   // _RWSTD_NO_EXT_PORTABLE_RANDOM_SEQUENCE


static _RWSTD_THREAD __rw_random_t
__rw_table [55];

static _RWSTD_THREAD __rw_random_t
__rw_inx [2];

static const __rw_random_t
__rw_table_size = __rw_random_t (sizeof __rw_table / sizeof *__rw_table);


static void
__rw_seed ( __rw_random_t seed)
{
    __rw_table [__rw_table_size - 1] = seed;

    _RWSTD_SIZE_T i;

    __rw_random_t k = 1;

    for (i = 0; i != __rw_table_size - 1; ++i) {
        const _RWSTD_SIZE_T ii = 21U * i % __rw_table_size;
        __rw_table [ii] = k;
        k               = seed - k;
        seed            = __rw_table [ii];
    }

    for (i = 0; i != 4U; ++i) {
        for (_RWSTD_SIZE_T ii = 0; ii != __rw_table_size; ++ii)
            __rw_table [i] =
                __rw_table [i] - __rw_table [(i + 31U) % __rw_table_size];
    }

    __rw_inx [0] = 0U;
    __rw_inx [1] = __rw_table_size * 2U / 3U - 1U;
}


struct _RandGuardT;

_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_rand (_RWSTD_SIZE_T limit)
{
    _RWSTD_ASSERT (0 != limit);

#ifdef _RWSTD_NO_TLS

    _RWSTD_MT_STATIC_GUARD (_RandGuardT);

#endif   // _RWSTD_NO_TLS

    if (0 == __rw_inx [0] && 0 == __rw_inx [1])
        __rw_seed (161803398U);

    __rw_table [++__rw_inx [0] %= __rw_table_size] -=
        __rw_table [++__rw_inx [1] %= __rw_table_size];

    return __rw_table [__rw_inx [0]] % limit;

}


}   // namespace __rw
