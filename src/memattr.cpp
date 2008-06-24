 /***************************************************************************
 *
 * memattr.cpp - source for C++ Standard Library helper functions
 *               to determine the attributes of regions of memory
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
 * Copyright 2005-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#include <errno.h>    // for ENOMEM, errno
#include <string.h>   // for memchr

#ifndef EFAULT
#  define EFAULT  14   // Linux value
#endif   // EFAULT

#ifdef __CYGWIN__
   // use the Windows API on Cygwin
#  define _WIN32
#endif

#ifndef _WIN32
#  ifdef __SUNPRO_CC
     // working around SunOS bug #568
#    include <time.h>
#  endif
#  include <unistd.h>     // for getpagesize(), sysconf()

#  if defined (_RWSTD_OS_LINUX) && !defined (__USE_BSD)
     // needed for caddr_t, madvise, and MADV_WILLNEED
#    define __USE_BSD
#  endif   // _RWSTD_OS_LINUX && !__USE_BSD

#  include <sys/mman.h>   // for mincore()
#  include <sys/types.h>

#  ifndef _SC_PAGE_SIZE
     // fall back on the alternative macro if it exists,
     // or use getpagesize() otherwise
#    ifndef _SC_PAGESIZE
#      define GETPAGESIZE()   getpagesize ()
#    else
#      define GETPAGESIZE()   sysconf (_SC_PAGESIZE)
#    endif
#  else
#      define GETPAGESIZE()   sysconf (_SC_PAGE_SIZE)
#  endif   // _SC_PAGE_SIZE
#else
#  include <windows.h>    // for everything (ugh)
#endif   // _WIN32

#include <rw/_defs.h>

// define ENOMEM if it's not #defined (e.g., when using pure libc headers)
#ifndef ENOMEM
#  define ENOMEM   12   /* HP-UX, Solaris, Tru64, ... */
#endif   // ENOMEM


// FIXME: implement PROT_READ and PROT_WRITE correctly
#ifndef _RWSTD_PROT_READ
#  define _RWSTD_PROT_READ -1
#endif

#ifndef _RWSTD_PROT_WRITE
#  define _RWSTD_PROT_WRITE 0
#endif


#define DIST(addr1, addr2)                         \
  (  _RWSTD_REINTERPRET_CAST (const char*, addr1)  \
   - _RWSTD_REINTERPRET_CAST (const char*, addr2))


_RWSTD_NAMESPACE (__rw) {

_RWSTD_EXPORT _RWSTD_SSIZE_T
__rw_memattr (const void *addr, _RWSTD_SIZE_T nbytes, int attr)
{
    // FIXME: allow attr to be set to the equivalent of PROT_READ,
    // PROT_WRITE, and (perhaps also) PROT_EXEC, or any combination
    // of the three, in addition to 0 (PROT_NONE)
    _RWSTD_UNUSED (attr);

#ifndef _WIN32

    const int errno_save = errno;

    // determine the system page size in bytes
    static const _RWSTD_SIZE_T pgsz = size_t (GETPAGESIZE ());

    // compute the address of the beginning of the page
    // to which the address `addr' belongs
    caddr_t const page =
        _RWSTD_REINTERPRET_CAST (caddr_t,
            _RWSTD_REINTERPRET_CAST (_RWSTD_SIZE_T, addr) & ~(pgsz - 1));

    // compute the maximum number of pages occuppied by the memory
    // region pointed to by `addr' (nbytes may be -1 as a request
    // to compute, in a safe way, the length of the string pointed
    // to by `addr')
    _RWSTD_SIZE_T npages = nbytes ? nbytes / pgsz + 1 : 0;

    for (size_t i = 0; i < npages; ++i) {

        const caddr_t next = _RWSTD_REINTERPRET_CAST (char*, page) + i * pgsz;

#  ifdef _RWSTD_OS_SUNOS

        char dummy = '\0';

        // on Solaris use mincore() instead of madvise() since
        // the latter is unreliable
        if (-1 == mincore (next, 1, &dummy)) {

            const int err = errno;
            errno = errno_save;

            if (ENOMEM == err)
                return next == page ? -1 : DIST (next, addr);
        }

#  elif defined (_RWSTD_OS_OSF1)

        // use Tru64 mvalid()
        if (-1 == mvalid (next, 1, PROT_READ)) {

            const int err = errno;
            errno = errno_save;

            if (err)
                return next == page ? -1 : DIST (next, addr);
        }

#  elif defined (_RWSTD_OS_IRIX64)

        // as of 6.5, IRIX has no mincore() or mvalid() call,
        // or posix_madvise(), and madvise() is unreliable
        // use msync() instead
        if (-1 == msync (next, 1, MS_ASYNC)) {

            const int err = errno;
            errno = errno_save;

            if (err)
                return next == page ? -1 : DIST (next, addr);
        }

#  elif !defined (_RWSTD_NO_MADVISE)

#    ifdef _RWSTD_OS_IRIX64
        // IRIX 6.5 recognizes only MADV_DONTNEED
        const int advice = MADV_DONTNEED;
#    else
        const int advice = MADV_WILLNEED;
#    endif

        // on HP-UX, Linux, use madvise() as opposed to mincore()
        // since the latter fails for address ranges that aren't
        // backed by a file (such as stack variables)
        if (-1 == madvise (next, 1, advice)) {

            const int err = errno;
            errno = errno_save;

            bool bad_address;

#    ifdef _RWSTD_OS_LINUX
            // Linux fails with EBADF when "the map exists,
            // but the area maps something that isn't a file"
            bad_address = EFAULT == err || ENOMEM == err;
#    else   // not Linux
            // EINVAL implies bad (e.g., unimplemented, such as
            // IRIX 6.5) advice, misaligned addr (not on page size
            // boundary), or zero size
            bad_address = !(0 == err || EINVAL == err);
#    endif   // Linux

            if (bad_address)
                return next == page ? -1 : DIST (next, addr);
        }

#  else
        _RWSTD_UNUSED (errno_save);
#  endif

        if (_RWSTD_SIZE_MAX == nbytes) {

            // when the size of the region is unspecified look
            // for the first NUL byte and when found, return
            // the total size of the memory region between
            // `addr' and the NUL byte (i.e., the length of
            // the string pointed to by `addr'); this should
            // be safe since the first byte of the range has
            // been determined to be readable

            const _RWSTD_SIZE_T maxpage =
                next == page ? pgsz - DIST (addr, next) : pgsz;

            const void* const pnul =
                memchr (next == page ? addr : next, '\0', maxpage);

            if (pnul) {
                nbytes = DIST (pnul, addr);
                npages = nbytes / pgsz + 1;
                break;
            }
        }
    }

    return _RWSTD_STATIC_CAST (_RWSTD_SSIZE_T, nbytes);

#else   // ifdef _WIN32

    LPVOID const ptr = _RWSTD_CONST_CAST (LPVOID, addr);

    if (_RWSTD_SIZE_MAX == nbytes) {

        // treat the address as a pointer to a NUL-terminated string
        if (IsBadStringPtr (_RWSTD_STATIC_CAST (LPCSTR, ptr), nbytes))
            return -1;

        // compute the length of the string
        nbytes = strlen (_RWSTD_STATIC_CAST (const char*, addr));

        // disable read checking below (since it was done above)
        attr &= ~_RWSTD_PROT_READ;
    }

    if ((attr & _RWSTD_PROT_READ) && IsBadReadPtr (ptr, nbytes))
        return -1;

    if ((attr & _RWSTD_PROT_WRITE) && IsBadWritePtr (ptr, nbytes))
        return -1;

    return _RWSTD_STATIC_CAST (_RWSTD_SSIZE_T, nbytes);

#endif   // _RWSTD_NO_INCORE

}

}
