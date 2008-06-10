/***************************************************************************
 *
 * memchk.cpp - definitions of memory checking helper functions
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
 * Copyright 2001-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifndef _WIN32
#  include <fcntl.h>     // for open()
#  include <unistd.h>    // for getpagesize(), write()
#else   // if Windows
#  include <fcntl.h>     // for POSIX compatibility APIs
#  include <io.h>        // ditto
#  include <windows.h>   // for all of Win32 junk
#endif   // _WIN32

#include <errno.h>       // for errno, EINTR
#include <stddef.h>      // for size_t
#include <stdio.h>       // for L_tmpnam, P_tmpdir, tempnam()
#include <stdlib.h>      // for mkstemp()
#include <string.h>      // for memchr()

#include <rw/_defs.h>


#ifndef EINTR
#  define EINTR   4   /* AIX, HP-UX, IRIX, Linux, SunOS, Tru64 */
#endif   // EINTR


#if defined (_RWSTD_NO_MKSTEMP) || !defined (_RWSTD_NO_PURE_C_HEADERS)
#  ifndef _RWSTD_NO_MKSTEMP_IN_LIBC

extern "C" int mkstemp (char*) _LIBC_THROWS();

#    undef _RWSTD_NO_MKSTEMP
#  endif   // _RWSTD_NO_MKSTEMP_IN_LIBC
#endif   // _RWSTD_NO_MKSTEMP || !_RWSTD_NO_PURE_C_HEADERS


#ifndef P_tmpdir
#  ifndef _P_tmpdir
#    define P_tmpdir   "/tmp/"
#  else
#    define P_tmpdir   _P_tmpdir
#  endif
#endif   // P_tmpdir


#if defined (_RWSTD_EDG_ECCP) && !defined (_WIN32)

extern "C" {

int getpagesize ();

}   // extern "C"

#endif   // vanilla EDG eccp demo on UNIX


static int page_size ()
{
    static int size = 0;

    if (0 == size) {

#ifdef _WIN32

        SYSTEM_INFO info;

        GetSystemInfo (&info);

        size = int (info.dwPageSize);

#else   // any saner OS

        size = getpagesize ();

#endif   // _WIN32

    }

    return size;
}


size_t memchk (const void *addr, size_t nbytes)
{
    static int fd = -1;

    if (-1 == fd) {

        // writing to /dev/null need not reliably detect invalid
        // address ranges if the operating system optimizes the
        // operation away (as SunOS does, for instance)
        // fd = open ("/dev/null", O_WRONLY);

#ifdef _WIN32

        char* const fname = tempnam (P_tmpdir, ".rwmemchk.tmp");

        if (!fname)
            return size_t (-1);

        // create a temporary file and have Win32 delete it when
        // the last file descriptor that refers to it is closed
        fd = open (fname, O_RDWR | O_CREAT | _O_TEMPORARY, 0666);

        // free storage allocated by tempnam()
        free (fname);

        if (fd < 0) {
            // error: unable to check addr
            return size_t (-1);
        }

#else   // !_WIN32

#  define TMP_TEMPLATE P_tmpdir "/rwmemchk-XXXXXX"

        char fname_buf [] = TMP_TEMPLATE;

        fd = mkstemp (fname_buf);

        if (fd < 0) {
            // error: unable to check addr
            return size_t (-1);
        }

        unlink (fname_buf);

#endif   // _WIN32

    }

    lseek (fd, 0, SEEK_SET);

    size_t size = size_t (-1);   // error

    const int errno_save = errno;

    errno = 0;

    if (size_t (-1) == nbytes) {

        // size not specified, check that addr points
        // to a valid NUL-terminated byte string (NTBS)

        // get the virtual page size
        static const size_t pgsz = page_size ();

        // compute the positive offset from the closest page boundary
        const size_t pgoff = size_t (addr) % pgsz;

        // compute the number of bytes from addr to the end of the page
        nbytes = pgsz - pgoff;

        // go through memory pointed to by `addr' one page
        // at a time looking for the terminating NUL
        for (const char *pc = static_cast<const char*>(addr); ; ) {

            // try to write a page of memory (or what's left of it)
            const size_t nwrote = write (fd, pc, nbytes);

            if (nwrote == nbytes) {

                // on success, look for the NUL character
                const void* const pnull = memchr (pc, 0, nbytes);

                if (pnull) {

                    // return the offset of the NUL character from `addr'
                    size =   static_cast<const char*>(pnull)
                           - static_cast<const char*>(addr) + 1;
                    break;
                }

                pc     += nbytes;
                nbytes  = pgsz;
            }
            else if (size_t (-1) == nwrote && EINTR == errno) {
                // retry the interrupted system call
                errno = 0;
            }
            else if (nwrote < nbytes) {
                // retry after a partial write
                errno = 0;
                pc     += nwrote;
                nbytes -= nwrote;
            }
            else {
                // error: `addr' doesn't point to a valid NTBS
                break;
            }
        }
    }
    else {

        const size_t nwrote = write (fd, addr, nbytes);

        if (nwrote == nbytes)
            size = nbytes;
    }

    errno = errno_save;

    return size_t (size);
}


size_t strchk (const char *str)
{
    return memchk (str, size_t (-1));
}
