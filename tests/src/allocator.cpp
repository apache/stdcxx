/************************************************************************
 *
 * allocator.cpp - definitions of allocator testsuite helpers
 *
 * $Id$
 *
 ************************************************************************
 *
 * Copyright 2006 The Apache Software Foundation or its licensors,
 * as applicable.
 *
 * Copyright 2006 Rogue Wave Software.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <rw_allocator.h>
#include <new>              // for bad_alloc
#include <stdarg.h>         // for va_arg(), va_list
#include <string.h>         // for memset()

/**************************************************************************/

static const char* const
_rw_funnames[] = {
    "UserAlloc::UserAlloc ()",
    "UserAlloc::UserAlloc (const UserAlloc<U>&)",
    "UserAlloc::UserAlloc (const UserAlloc&)",
    "UserAlloc::~UserAlloc ()",
    "UserAlloc::allocate (size_type, void*)",
    "UserAlloc::deallocate (pointer, size_type)",
    "UserAlloc::construct (pointer, const_reference)",
    "UserAlloc::destroy (pointer)",
    "UserAlloc::address (reference)",
    "UserAlloc::max_size ()"
};

static int _rw_id_gen;   // generates unique ids

/**************************************************************************/

_TEST_EXPORT int
rw_vasnprintf (char**, size_t*, const char*, va_list);


static void
_rw_throw_exception (const char *file, int line, const char *fmt, ...)
{
    struct BadSharedAlloc: _RWSTD_BAD_ALLOC {
        char what_ [4096];

        /* virtual */ const char* what () const _THROWS (()) {
            return what_;
        }
    };

    BadSharedAlloc ex;

    va_list va;
    va_start (va, fmt);

    char *buf = ex.what_;
    size_t bufsize = sizeof ex.what_;
    rw_vasnprintf (&buf, &bufsize, fmt, va);

    va_end (va);

    throw ex;
}

/**************************************************************************/

SharedAlloc::
SharedAlloc (size_t nbytes /* = MAX_SIZE */, size_t nblocks /* = MAX_SIZE */)
    : max_bytes_ (nbytes), max_blocks_ (nblocks),
      n_bytes_ (0), n_blocks_ (0), id_ (0)
{
    memset (n_calls_, 0, sizeof n_calls_);
    memset (throw_at_calls_, -1, sizeof throw_at_calls_);
}


/* virtual */ SharedAlloc::
~SharedAlloc ()
{
    // no-op
}


/* virtual */ void* SharedAlloc::
allocate (size_t nelems, size_t size, const void* /* = 0 */)
{
    funcall (m_allocate);

    const size_t nbytes = nelems * size;

    if (max_blocks_ < n_blocks_ + nelems)
        _rw_throw_exception (__FILE__, __LINE__,
                             "allocate (%zu):  reached block limit of %zu",
                             nbytes, max_blocks_);

    if (max_bytes_ < n_bytes_ + nbytes)
        _rw_throw_exception (__FILE__, __LINE__,
                             "allocate (%zu):  reached size limit of %zu",
                             nbytes, max_bytes_);

    return operator new (nbytes);
}


/* virtual */ void SharedAlloc::
deallocate (void *ptr, size_t nelems, size_t size)
{
    funcall (m_deallocate);

    return operator delete (ptr);
}


/* virtual */ size_t SharedAlloc::
max_size (size_t size)
{
    funcall (m_max_size);

    return max_blocks_ / size;
}


/* virtual */ void SharedAlloc::
funcall (MemFun mf)
{
    if (m_ctor == mf) {
        // ordinary (not a copy or converting) ctor
        if (id_ <= 0) {
            RW_ASSERT (0 == n_refs_);
            id_ = ++_rw_id_gen;
        }

        // increment the number of references to this allocator
        ++n_refs_;
    }
    else if (m_cpy_ctor == mf || m_cvt_ctor == mf) {
        // copy or converting ctor
        RW_ASSERT (0 < id_);
        RW_ASSERT (0 < n_refs_);

        // increment the number of references to this allocator
        ++n_refs_;
    }
    else if (m_dtor == mf) {
        // dtor
        RW_ASSERT (0 <= id_ && id_ <= _rw_id_gen);
        RW_ASSERT (0 < n_refs_);

        // decrement the number of references and invalidate
        // id if no object refers to *this
        if (0 == --n_refs_)
            id_ = -1;
    }

    // check the number of calls and throw an exception
    // if the specified limit has been reached
    if (n_calls_ [mf] == throw_at_calls_ [mf])
        _rw_throw_exception (__FILE__, __LINE__,
                             "%s: reached call limit of %zu",
                             _rw_funnames [mf], throw_at_calls_);

    // increment the number of calls
    ++n_calls_ [mf];
}


/* static */ SharedAlloc* SharedAlloc::
instance (SharedAlloc *pinst /* = 0 */)
{
    // get or a set a pointer to the global allocator object
    static size_t instbuf [sizeof (SharedAlloc) / sizeof (size_t) + 1];
    static SharedAlloc* pglobal = 0;

    if (pinst) {
        // set the global allocator object and return
        // a pointer to the old one (may be 0)
        SharedAlloc* const tmp = pglobal;
        pglobal = pinst;
        pinst = tmp;
    }
    else if (0 == pglobal) {
        // construct the global allocator object
        pglobal = new (instbuf) SharedAlloc ();
        pinst   = pglobal;
    }
    else
        pinst = pglobal;

    RW_ASSERT (0 != pinst);

    return pinst;
}
