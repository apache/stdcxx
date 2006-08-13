/**************************************************************************
 *
 * tmpbuf.cpp - definition of the __rw_tmpbuf() helper
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
 * Copyright 2005-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#include "podarray.h"        // for __rw_aligned_buffer
#include <rw/_allocator.h>   // for __rw_allocate(), ...
#include <rw/_mutex.h>       // for _RWSTD_THREAD_XXX()
#include <rw/_defs.h>


_RWSTD_NAMESPACE (__rw) {

// make sure buffer size is greater than zero
typedef char _CharBuf [_RWSTD_TMPBUF_SIZE + !_RWSTD_TMPBUF_SIZE];

// use `aligned_buffer' to guarantee proper buffer alignment
static _RWSTD_THREAD __rw_aligned_buffer<_CharBuf>
__rw_buffer;

static _RWSTD_THREAD unsigned long
__rw_buffer_busy /* = 0 */;   // > 0 when buffer in use


// extern "C" to avoid binary incompatibility due to changes
// in function arguments (e.g., their sign)
extern "C" {


// used by std::get_temporary_buffer<>()
_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_tmpbuf (void **pptr, _RWSTD_PTRDIFF_T nelems, _RWSTD_SIZE_T size)
{
    _RWSTD_ASSERT (0 != pptr);
    _RWSTD_ASSERT (0 != size);

    // detect overflow and fail
    _RWSTD_SIZE_T nbytes = nelems * size;

    if (   nelems < 0
        || nbytes / size != _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, nelems)
        || nelems && nbytes / nelems != size) {
        *pptr = 0;
        return 0;
    }

    // implicit initialization used to prevent a g++ 2.95.2 warning on Tru64
    // sorry: semantics of inline function static data are wrong (you'll wind
    // up with multiple copies)

    void* const buffer = __rw_buffer._C_data ();

    if (*pptr == buffer) {

        // static buffer being returned, decrement its usage counter
        _RWSTD_THREAD_PREDECREMENT (__rw_buffer_busy, false);

        return 0;
    }

    if (*pptr) {
        // dynamically allocated buffer being returned
        __rw_deallocate (*pptr, 0);
        return 0;
    }

    // allocating a new buffer

    if (!nbytes) {
        // don't waste memory for 0-size requests
        *pptr = 0;
        return 0;
    }

    if (nbytes <= _RWSTD_TMPBUF_SIZE) {

        if (1 == _RWSTD_THREAD_PREINCREMENT (__rw_buffer_busy, false)) {

            *pptr = buffer;

            // static buffer used, its usage counter stays non-zero
            return _RWSTD_TMPBUF_SIZE / size;
        }

        // static buffer not used, decrement its usage counter
        _RWSTD_THREAD_PREDECREMENT (__rw_buffer_busy, false);
    }

    _TRY {
        *pptr = __rw_allocate (nbytes, 0);
    }
    _CATCH (...) {
        *pptr  = 0;
        nbytes = 0;
    }

    return nbytes / size;
}

}   // extern "C"


}   // namespace __rw
