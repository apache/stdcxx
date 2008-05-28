/***************************************************************************
 *
 * mman.cpp
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

#define _RWSTD_LIB_SRC
#include <rw/_defs.h>

#ifndef _MSC_VER
   // <unistd.h> is included here because of PR #26255
#  include <unistd.h>
#endif  // _MSC_VER

#include <sys/stat.h>

#ifndef _RWSTD_NO_MMAP
#  include <sys/mman.h>
#elif defined (_WIN32)
#  include <windows.h>
#  include <io.h>
#endif   // _MSC_VER

#include <sys/types.h>
#include <fcntl.h>


_RWSTD_NAMESPACE (__rw) {


// maps a named file into memory as shared, read-only, returns
// the beginning address on success and fills `size' with the
// size of the file; returns 0 on failure
void* __rw_mmap (const char* fname, _RWSTD_SIZE_T *size)
{
    _RWSTD_ASSERT (0 != fname);
    _RWSTD_ASSERT (0 != size);

#if !defined (_MSC_VER)
    struct stat sb;
    if (stat (fname, &sb) == -1)
#else
    struct _stat sb;
    if (_stat (fname, &sb) == -1)
#endif
        return 0;

    *size = sb.st_size;


#ifndef _WIN32
    const int fd = open (fname, O_RDONLY);
   
    if (-1 == fd)
        return 0;

#endif   // _WIN32

#ifndef _RWSTD_NO_MMAP

    // On HPUX systems MAP_SHARED will prevent a second mapping of the same
    // file if the regions are overlapping; one solution is to make the 
    // mapping private.
#if defined(__hpux)
    void *data = mmap (0, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
#else
    void *data = mmap (0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
#endif // defined(__hpux__)

    close (fd);

#ifndef MAP_FAILED
#  define MAP_FAILED (void*)-1
#endif

    if (MAP_FAILED == data)   // failure
        return 0;

#elif defined (_WIN32)

    HANDLE mmf = 
        CreateFile (fname, GENERIC_READ, FILE_SHARE_READ, NULL,
                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (mmf == INVALID_HANDLE_VALUE)
        return 0;
                              
    HANDLE mmfv = 
        CreateFileMapping (mmf, NULL, PAGE_READONLY, 0, 0, NULL);
    if (mmfv == INVALID_HANDLE_VALUE) {
        CloseHandle (mmf);
        return 0;
    }

    void * data = 
        MapViewOfFile (mmfv, FILE_MAP_READ, 0, 0, sb.st_size);

    // The handles can be safely closed
    CloseHandle (mmf);
    CloseHandle (mmfv);

#else   // no mmap() or equivalent

#  ifndef _RWSTD_NO_MUNMAP
#    define _RWSTD_NO_MUNMAP
#  endif   // _RWSTD_NO_MUNMAP

    // read() takes a size_t argument, convert off_t to it
    const size_t mapsize = size_t (sb.st_size);

    void* data = operator new (mapsize);
    const ssize_t nread = read (fd, data, mapsize);

    if (size_t (nread) != mapsize) {
        operator delete (data);
        data = 0;
    }

#endif  // _MSC_VER

    return data;
}


void __rw_munmap (const void* pcv, _RWSTD_SIZE_T size)
{
    _RWSTD_ASSERT (pcv && size);

        // cast first to void*
    void* pv = _RWSTD_CONST_CAST (void*, pcv);

    // POSIX munmap() takes a void*, but not all platforms conform
#ifndef _RWSTD_NO_MUNMAP
    munmap (_RWSTD_STATIC_CAST (_RWSTD_MUNMAP_ARG1_T, pv), size);
#elif defined (_WIN32)
    UnmapViewOfFile (pv);
#else   // no munmap()
    operator delete (pv);
#endif  // _MSC_VER
}


}   // namespace __rw
