/**************************************************************************
 *
 * file.cpp - definition of file I/O helper functions
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
 * Copyright 2002-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

// both macros must be defined before #including <rw/_defs.h>
#ifndef _RWSTD_NO_PURE_C_HEADERS
#  define _RWSTD_NO_PURE_C_HEADERS
#endif   // _RWSTD_NO_PURE_C_HEADERS

#ifndef _RWSTD_NO_DEPRECATED_C_HEADERS
#  define _RWSTD_NO_DEPRECATED_C_HEADERS
#endif   // _RWSTD_NO_DEPRECATED_C_HEADERS

#include <errno.h>    // for ERANGE, errno
#include <stdio.h>    // for P_tmpdir, std{err,in,out}, tmpnam()
#include <stdlib.h>   // for mkstemp(), strtoul()
#include <ctype.h>    // for isalpha(), isspace(), toupper()


#if defined (_WIN32) && !defined (__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#else
#  include <unistd.h>
#  include <fcntl.h>
#endif   // _WIN32

#ifdef _WIN32
#  define _BINARY _O_BINARY
#else
#  define _BINARY 0
#endif


#include <rw/_file.h>
#include <rw/_defs.h>


#ifdef _RWSTD_MSVC
   // shut up the braindead warning C4146: unary minus operator
   // applied to unsigned type, result still unsigned
#  pragma warning (disable: 4146)
#endif   // _RWSTD_MSVC


#if     defined (_RWSTD_NO_MKSTEMP) && !defined (_RWSTD_NO_MKSTEMP_IN_LIBC) \
    || !defined (_RWSTD_NO_PURE_C_HEADERS)

extern "C" {

#undef mkstemp
#define mkstemp _RWSTD_LIBC_SYM (mkstemp)

_RWSTD_DLLIMPORT int mkstemp (char*);

}   // extern "C"

#  undef _RWSTD_NO_MKSTEMP

#endif   // _RWSTD_NO_MKSTEMP[_IN_LIBC] || !_NO_PURE_C_HEADERS


#if     defined (_RWSTD_NO_FILENO) && !defined (_RWSTD_NO_FILENO_IN_LIBC) \
    || !defined (_RWSTD_NO_PURE_C_HEADERS)

// declare fileno in case it's not declared (for strict ANSI conformance)
extern "C" {

_RWSTD_DLLIMPORT int (fileno)(FILE*) _LIBC_THROWS ();

#  undef _RWSTD_NO_FILENO

}   // extern "C"

#endif   // _NO_FILENO && !_NO_FILENO_IN_LIBC || !_NO_PURE_C_HEADERS


_RWSTD_NAMESPACE (__rw) {


#if __SUNPRO_CC == 0x530 && defined (__SunOS_5_6)
   // working around a SunPro 5.3 bug on SunOS 5.6 (PR #26283)
#  define _RWSTD_FILENO(f) (f)->_file
#else
#  define _RWSTD_FILENO(f) fileno (f)
#endif


static const int __rw_io_modes [] = {
    /*  0 */ -1,
    /*  1 */ _RWSTD_O_CREAT | _RWSTD_O_APPEND | _RWSTD_O_WRONLY,
    /*  2 */ -1,
    /*  3 */ _RWSTD_O_CREAT | _RWSTD_O_APPEND | _RWSTD_O_WRONLY | _BINARY,
    /*  4 */ _RWSTD_O_RDONLY,
    /*  5 */ _RWSTD_O_RDONLY | _RWSTD_O_APPEND,
    /*  6 */ _RWSTD_O_RDONLY | _BINARY,
    /*  7 */ _RWSTD_O_CREAT | _RWSTD_O_APPEND | _RWSTD_O_RDONLY | _BINARY,
    /*  8 */ _RWSTD_O_CREAT | _RWSTD_O_TRUNC  | _RWSTD_O_WRONLY,
    /*  9 */ _RWSTD_O_CREAT | _RWSTD_O_APPEND | _RWSTD_O_WRONLY,
    /* 10 */ _RWSTD_O_CREAT | _RWSTD_O_TRUNC | _RWSTD_O_WRONLY | _BINARY,
    /* 11 */ _RWSTD_O_CREAT | _RWSTD_O_APPEND | _RWSTD_O_WRONLY | _BINARY,
    /* 12 */ _RWSTD_O_RDWR,
    /* 13 */ _RWSTD_O_CREAT | _RWSTD_O_APPEND | _RWSTD_O_RDWR,
    /* 14 */ _RWSTD_O_RDWR | _BINARY,
    /* 15 */ _RWSTD_O_CREAT | _RWSTD_O_APPEND | _RWSTD_O_RDWR | _BINARY,
    /* 16 */ _RWSTD_O_TRUNC,
    /* 17 */ _RWSTD_O_TRUNC | _RWSTD_O_APPEND,
    /* 18 */ _RWSTD_O_TRUNC | _BINARY,
    /* 19 */ _RWSTD_O_TRUNC | _RWSTD_O_APPEND | _BINARY,
    /* 20 */ _RWSTD_O_TRUNC | _RWSTD_O_RDONLY,
    /* 21 */ _RWSTD_O_TRUNC | _RWSTD_O_APPEND | _RWSTD_O_RDONLY,
    /* 22 */ _RWSTD_O_TRUNC | _RWSTD_O_RDONLY | _BINARY,
    /* 23 */ _RWSTD_O_TRUNC | _RWSTD_O_APPEND | _RWSTD_O_RDONLY | _BINARY,
    /* 24 */ _RWSTD_O_CREAT | _RWSTD_O_TRUNC | _RWSTD_O_WRONLY,
    /* 25 */ _RWSTD_O_CREAT | _RWSTD_O_TRUNC | _RWSTD_O_APPEND
                            | _RWSTD_O_WRONLY,
    /* 26 */ _RWSTD_O_CREAT | _RWSTD_O_TRUNC | _RWSTD_O_WRONLY | _BINARY,
    /* 27 */ _RWSTD_O_CREAT | _RWSTD_O_TRUNC | _RWSTD_O_APPEND
                            | _RWSTD_O_WRONLY | _BINARY,
    /* 28 */ _RWSTD_O_CREAT | _RWSTD_O_TRUNC | _RWSTD_O_RDWR,
    /* 29 */ _RWSTD_O_CREAT | _RWSTD_O_TRUNC | _RWSTD_O_APPEND | _RWSTD_O_RDWR,
    /* 30 */ _RWSTD_O_CREAT | _RWSTD_O_TRUNC | _RWSTD_O_RDWR | _BINARY,
    /* 31 */ _RWSTD_O_CREAT | _RWSTD_O_TRUNC | _RWSTD_O_APPEND | _RWSTD_O_RDWR
                            | _BINARY
};


static const char __rw_stdio_modes [][4] = {
    /*  0 */ "",      // error
    /*  1 */ "a",     // app
    /*  2 */ "",      // binary
    /*  3 */ "ab",    // binary|app
    /*  4 */ "r",     // in
    /*  5 */ "a",     // in|app
    /*  6 */ "rb",    // in|binary
    /*  7 */ "ab",    // in|binary|app
    /*  8 */ "w",     // out
    /*  9 */ "a",     // out|app
    /* 10 */ "wb",    // out|binary
    /* 11 */ "ab",    // out|binary|app
    /* 12 */ "r+",    // out|in
    /* 13 */ "a+",    // out|in|app
    /* 14 */ "r+b",   // out|in|binary
    /* 15 */ "a+b",   // out|in|binary|app
    /* 16 */ "w",     // trunc
    /* 17 */ "w",     // trunc|app
    /* 18 */ "wb",    // trunc|binary
    /* 19 */ "wb",    // trunc|binary|app
    /* 20 */ "",      // trunc|in
    /* 21 */ "",      // trunc|in|app
    /* 22 */ "",      // trunc|in|binary
    /* 23 */ "",      // trunc|in|binary|app
    /* 24 */ "w",     // trunc|out
    /* 25 */ "w",     // trunc|out|app
    /* 26 */ "wb",    // trunc|out|binary
    /* 27 */ "wb",    // trunc|out|binary|app
    /* 28 */ "w+",    // trunc|out|in
    /* 29 */ "w+",    // trunc|out|in|app
    /* 30 */ "w+b",   // trunc|out|in|binary
    /* 31 */ "w+b"    // trunc|out|in|binary|app
};


static const int
__rw_openmode_mask =
    ~(  _RWSTD_IOS_ATE | _RWSTD_IOS_STDIO
      | _RWSTD_IOS_NOCREATE | _RWSTD_IOS_NOREPLACE);


// converts iostream open mode to POSIX file access mode
static inline int
__rw_io_mode (int openmode)
{
    // mask out irrelevant bits
    const _RWSTD_SIZE_T inx = openmode & __rw_openmode_mask;

    if (sizeof __rw_io_modes / sizeof *__rw_io_modes <= inx)
        return -1;

    int fdmode = __rw_io_modes [inx];

    // nocreate: open fails if the file does not exist
    if (openmode & _RWSTD_IOS_NOCREATE)
        fdmode &= ~_RWSTD_O_CREAT;

    // noreplace: open for writing fails if the file exists
    if (   (openmode & (_RWSTD_IOS_OUT | _RWSTD_IOS_NOREPLACE))
        == (_RWSTD_IOS_OUT | _RWSTD_IOS_NOREPLACE))
        fdmode |= _RWSTD_O_EXCL;

    return fdmode;
}


// converts iostream open mode to C stdio file open mode
// or the empty string on error (never returns null)
static inline const char*
__rw_stdio_mode (int openmode)
{
    // mask out irrelevant bits
    const _RWSTD_SIZE_T inx = openmode & __rw_openmode_mask;

    if (sizeof __rw_stdio_modes / sizeof *__rw_stdio_modes <= inx)
        return "";   // error -- bad mode

    if (openmode & (_RWSTD_IOS_NOCREATE | _RWSTD_IOS_NOREPLACE))
        return "";   // error -- not implemented

    const char* const stdio_mode = __rw_stdio_modes [inx];

    // verify postcondition: function never returns null
    _RWSTD_ASSERT (0 != stdio_mode);

    return stdio_mode;
}


static int
__rw_mkstemp (int modebits, long prot)
{
    int fd;

#ifndef _RWSTD_NO_MKSTEMP

    // mkstemp() opens a temporary file for reading and writing
    _RWSTD_UNUSED (modebits);
    _RWSTD_UNUSED (prot);

#  ifndef P_tmpdir   // #defined in <stdio.h> by POSIX
#    define P_tmpdir "/tmp"
#  endif   // P_tmpdir

    char fnamebuf[] = P_tmpdir "/.rwtmpXXXXXX";

    fd = mkstemp (fnamebuf);

    if (fd >= 0)
        unlink (fnamebuf);

#else   // if defined (_RWSTD_NO_MKSTEMP)

    modebits |= _RWSTD_O_EXCL | _RWSTD_O_CREAT;

#ifdef _WIN32

    // tempnam(const char *dir, const char *prefix) will generate
    // a unique file name for a directory chosen by the following rules:
    //
    // *  If the TMP environment variable is defined and set to a valid
    //    directory name, unique file names will be generated for the
    //    directory specified by TMP.
    // *  If the TMP environment variable is not defined or if it is set
    //    to the name of a directory that does not exist, tempnam will
    //    use the dir parameter as the path for which it will generate
    //    unique names.
    // *  If the TMP environment variable is not defined or if it is set
    //    to the name of a directory that does not exist, and if dir is
    //    either NULL or set to the name of a directory that does not
    //    exist, tempnam will use the current working directory to
    //    generate unique names. Currently, if both TMP and dir specify
    //    names of directories that do not exist, the tempnam function
    // call will fail.
    //
    // The name returned by tempnam will be a concatenation of prefix
    // and a sequential number, which will combine to create a unique
    // file name for the specified directory. tempnam generates file
    // names that have no extension. tempnam uses malloc to allocate
    // space for the filename; the program is responsible for freeing
    // this space when it is no longer needed. 
    char* const fname = tempnam (P_tmpdir, ".rwtmp");

    if (!fname)
        return -1;

    // create a temporary file that will be deleted when
    // the last file descriptor that refers to it is closed
    fd = open (fname, modebits | _O_TEMPORARY, prot);

    // deallocate storage allocated by tempnam()
    free (fname);
    
#  else   // ifndef _WIN32

    char tmpbuf [L_tmpnam];

    const char* const fname = tmpnam (tmpbuf);

    if (!fname)
        return -1;

    fd = open (fname, modebits, prot);

    // unlink the file, forcing the OS to delete it when
    // the last file descriptor that refers to it is closed
    if (fd >= 0)
        unlink (fname);

#  endif   // _WIN32
#endif   // _RWSTD_NO_MKSTEMP

    return fd;
}


_RWSTD_EXPORT void*
__rw_fopen (const char *fname, int openmode, long prot)
{
    if (openmode & _RWSTD_IOS_STDIO) {

        // convert the iostream open mode to the C stdio file open mode
        const char* const fmode = __rw_stdio_mode (openmode);

        _RWSTD_ASSERT (0 != fmode);

        // fmode is the empty string on failure
        if ('\0' == *fmode)
            return 0;

        FILE *file;

        if (fname) {
            // open the named file using C stdio
            file = fopen (fname, fmode);
        }
        else {
            // FIXME: check openmode to make sure it's valid for "w+"

            // extension: create a temporary file for update ("w+")
            // that will be automatically deleted when all references
            // to the file are closed
            file = tmpfile ();
        }

        if (0 == file)
            return 0;

        return _RWSTD_STATIC_CAST (void*, file);
    }

    // convert the iostream open mode to the POSIX file access mode
    const int fdmode = __rw_io_mode (openmode);

    if (fdmode < 0)
        return 0;

    int fd;

    if (fname) {
        // open the named file
        fd = open (fname, fdmode, prot);
    }
    else {
        // extension: create a temporary file that will be deleted
        // when the last file descriptor that refers to it is closed
        fd = __rw_mkstemp (fdmode, prot);
    }

    if (fd < 0)
        return 0;

    return _RWSTD_REINTERPRET_CAST (void*, fd + 1);
}


_RWSTD_EXPORT void*
__rw_fdopen (int fd)
{
    if (fd < 0)
        return 0;

    return _RWSTD_REINTERPRET_CAST (void*, fd + 1);
}


_RWSTD_EXPORT int
__rw_fclose (void *file, int flags)
{
    if (flags & _RWSTD_IOS_STDIO) {
        FILE* const fp = _RWSTD_STATIC_CAST (FILE*, file);

        return fclose (fp);
    }

    const int fd = int (_RWSTD_STATIC_CAST (char*, file) - 1 - (char*)0);

    return close (fd);
}


_RWSTD_EXPORT int
__rw_fileno (void *file, int flags)
{
    if (flags & _RWSTD_IOS_STDIO)
        return _RWSTD_FILENO (_RWSTD_STATIC_CAST (FILE*, file));

    const int fd = int (_RWSTD_STATIC_CAST (char*, file) - 1 - (char*)0);

    return fd;
}


_RWSTD_EXPORT int
__rw_fdmode (int fd)
{
// FIXME -- need to have equivalent of fcntl() on win32.
#ifdef _WIN32

    return fd == _RWSTD_STDIN_FILENO
               ? _RWSTD_IOS_IN
               : fd == _RWSTD_STDOUT_FILENO || fd == _RWSTD_STDERR_FILENO
                   ? _RWSTD_IOS_OUT : _RWSTD_IOS_OUT | _RWSTD_IOS_IN;

#else   // ifndef _WIN32

   const int m = fcntl (fd, _RWSTD_F_GETFL);

   if (-1 == m)
       return _RWSTD_INVALID_OPENMODE;

   int mode = m & _RWSTD_O_APPEND ? _RWSTD_IOS_APP : 0;

   switch (m & _RWSTD_O_ACCMODE) {
       case _RWSTD_O_RDONLY: mode |= _RWSTD_IOS_IN; break;
       case _RWSTD_O_WRONLY: mode |= _RWSTD_IOS_OUT; break;
       case _RWSTD_O_RDWR:   mode |= _RWSTD_IOS_IN | _RWSTD_IOS_OUT; break;
   }

   return mode;

#endif
}


_RWSTD_EXPORT int
__rw_fmode (void *file, int flags)
{
    if (flags & _RWSTD_IOS_STDIO) {
        FILE* const fp = _RWSTD_STATIC_CAST (FILE*, file);

        return __rw_fdmode (_RWSTD_FILENO (fp));
    }

    const int fd = int (_RWSTD_STATIC_CAST (char*, file) - 1 - (char*)0);

    return __rw_fdmode (fd);
}


#if defined (_RWSTD_MSVC) && defined (_WIN64)
// disable MSVC warning: conversion from '__int64' to 'long', possible loss of data
#pragma warning (disable: 4244)
#endif

_RWSTD_EXPORT long
__rw_fseek (void *file, int flags, _RWSTD_PTRDIFF_T offset, int origin)
{
    if (flags & _RWSTD_IOS_STDIO) {
        FILE* const fp = _RWSTD_STATIC_CAST (FILE*, file);

        const int pos = fseek (fp, long (offset), origin);
        if (pos < 0)
            return long (pos);

        return ftell (fp);
    }

    const int fd = int (_RWSTD_STATIC_CAST (char*, file) - 1 - (char*)0);

    return lseek (fd, offset, origin);
}


_RWSTD_EXPORT _RWSTD_SIZE_T
__rw_fread (void *file, int flags, void *buf, _RWSTD_SIZE_T size)
{
    if (flags & _RWSTD_IOS_STDIO) {
        FILE* const fp = _RWSTD_STATIC_CAST (FILE*, file);

        return fread (buf, 1, size, fp);
    }

    const int fd = int (_RWSTD_STATIC_CAST (char*, file) - 1 - (char*)0);

    return read (fd, buf, size);
}


_RWSTD_EXPORT _RWSTD_PTRDIFF_T
__rw_fwrite (void *file, int flags, const void *buf, _RWSTD_SIZE_T size)
{
    if (flags & _RWSTD_IOS_STDIO) {
        FILE* const fp = _RWSTD_STATIC_CAST (FILE*, file);

        return _RWSTD_STATIC_CAST (_RWSTD_PTRDIFF_T, fwrite (buf, 1, size, fp));
    }

    const int fd = int (_RWSTD_STATIC_CAST (char*, file) - 1 - (char*)0);

    return write (fd, buf, size);
}

#if defined (_RWSTD_MSVC) && defined (_WIN64)
// restore MSVC warning: conversion from '__int64' to 'long', possible loss of data
#pragma warning (default: 4244)
#endif


_RWSTD_EXPORT extern const void* __rw_std_streams[];


_RWSTD_EXPORT int
__rw_fflush (void *file, int flags)
{
    if (   file == __rw_std_streams [1] /* cout */
        || file == __rw_std_streams [5] /* wcout */)
        return fflush (stdout);

    if (   file == __rw_std_streams [2] /* cerr */
        || file == __rw_std_streams [3] /* clog */
        || file == __rw_std_streams [6] /* wcerr */
        || file == __rw_std_streams [7] /* wclog */)
        return fflush (stderr);

    if (file) {
        if (flags & _RWSTD_IOS_STDIO) {

            // `file' is a C file objec, flush it
            FILE* const fp = _RWSTD_STATIC_CAST (FILE*, file);

            return fflush (fp);
        }

        // `file' is a file descriptor, no need to flush it
    }
    else {

        // convenience call to flush both C files
        return fflush (stdout) + fflush (stderr);
    }

    return 0;
}


#ifdef _RWSTD_EDG_ECCP

   // undefine macros that expand to __rw_stderr et al
   // before initializing the globals to their values
#  undef stderr
#  undef stdin
#  undef stdout

extern "C" {

#  ifdef _RWSTD_OS_LINUX

// Linux glibc defines stdin, stdout, and stderr as global objects
// of type _IO_FILE but we fake the type using FILEs (it doesn't
// matter since the type isn't mangled into object names)
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

#  elif defined (_RWSTD_OS_SUNOS)

// define a type that's as big as SunOS __FILE
typedef struct  _RW_Sun_FILE {

#    if 8 == _RWSTD_LONG_SIZE

    int fill [4];   // 16 bytes

#    else   // if (ILP32)

    long fill [16];   // 128 bytes

#    endif   // LP64/ILP32

} __FILE;


// Solaris file array
extern struct __FILE __iob [FOPEN_MAX];

#    define stderr   (FILE*)(__iob + 0)
#    define stdin    (FILE*)(__iob + 1)
#    define stdout   (FILE*)(__iob + 2)

#  elif defined (_RWSTD_OS_WINDOWS)
#    error "need stderr, stdin, and stdout"
#  else
#    error "need stderr, stdin, and stdout"
#  endif

}   // extern "C"

#endif   // vanilla EDG eccp

FILE* __rw_stderr = stderr;
FILE* __rw_stdin  = stdin;
FILE* __rw_stdout = stdout;

}   // namespace __rw
