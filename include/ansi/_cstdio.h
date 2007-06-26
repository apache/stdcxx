// -*- C++ -*-
/***************************************************************************
 *
 * cstdio - C++ Standard library interface to the ANSI C header stdio.h
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

#ifndef _RWSTD_CSTDIO_H_INCLUDED
#define _RWSTD_CSTDIO_H_INCLUDED


#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 

#ifdef __linux__
   // prevent multiple definitions of SEEK_XXX constants
   // when Linux glibc's <unistd.h> is #included
#  define _STDIO_H
#endif   // __linux__


#ifndef _IOFBF
#  define _IOFBF   _RWSTD_IOFBF
#endif

#ifndef _IOLBF
#  define _IOLBF   _RWSTD_IOLBF
#endif

#ifndef _IONBF
#  define _IONBF   _RWSTD_IONBF
#endif

#ifndef BUFSIZ
#  define BUFSIZ   _RWSTD_BUFSIZ
#endif

#ifndef EOF
#  define EOF _RWSTD_EOF
#endif

struct FILE;

#ifndef FILE
#  define FILE FILE
#endif

#ifndef FILENAME_MAX
#  define FILENAME_MAX   _RWSTD_FILENAME_MAX
#endif

#ifndef FOPEN_MAX
#  define FOPEN_MAX   _RWSTD_FOPEN_MAX
#endif

#ifndef L_tmpnam
#  define L_tmpnam   _RWSTD_L_TMPNAM
#endif   // L_tmpnam

#ifndef SEEK_CUR
#  define SEEK_CUR   _RWSTD_SEEK_CUR
#endif

#ifndef SEEK_END
#  define SEEK_END   _RWSTD_SEEK_END
#endif

#ifndef SEEK_SET
#  define SEEK_SET   _RWSTD_SEEK_SET
#endif

#ifndef stderr
#  define stderr   _RW::__rw_stderr
#endif

#ifndef stdin
#  define stdin   _RW::__rw_stdin
#endif

#ifndef stdout
#  define stdout   _RW::__rw_stdout
#endif

#ifndef TMP_MAX
#  define TMP_MAX   _RWSTD_TMP_MAX
#endif

extern "C" {

#ifdef _RWSTD_SIZE_T
typedef _RWSTD_SIZE_T size_t;
#else
typedef unsigned long size_t;
#endif   // _RWSTD_SIZE_T


#ifdef _RWSTD_FPOS_T
typedef _RWSTD_FPOS_T fpos_t;
#elif defined (_RWSTD_FPOS_T_SIZE)

struct fpos_t
{
    char _C_fill [_RWSTD_FPOS_T_SIZE];
};

#else
typedef long fpos_t;
#endif   // _RWSTD_FPOS_T


int remove (const char*);
int rename (const char*, const char*);
FILE* tmpfile (void);
char* tmpnam (char*);
int fclose (FILE*);
int fflush (FILE*);
FILE* fopen (const char*, const char*);
FILE* freopen (const char*, const char*, FILE*);
void setbuf (FILE*, char*);
int setvbuf (FILE*, char*, int, size_t);
int fprintf (FILE*, const char*, ...);
int fscanf (FILE*, const char*, ...);
int printf (const char*, ...);
int scanf (const char*, ...);
int sprintf (char*, const char*, ...);
int sscanf (const char*, const char*, ...);
int vfprintf (FILE*, const char*, _RWSTD_VA_LIST);
int vprintf (const char*, _RWSTD_VA_LIST);
int vsprintf (char*, const char*, _RWSTD_VA_LIST);
int fgetc (FILE*);
char* fgets (char*, int , FILE*);
int fputc (int, FILE*);
int fputs (const char*, FILE*);
int getc (FILE*);
int getchar (void);
char* gets (char*);
int putc (int, FILE*);
int putchar (int);
int puts (const char*);
int ungetc (int c, FILE*);
size_t fread (void*, size_t size, size_t, FILE*);
size_t fwrite (const void*, size_t, size_t, FILE*);
int fgetpos (FILE*, fpos_t*);
int fseek (FILE*, long, int);
int fsetpos (FILE*, const fpos_t*);
long ftell (FILE*);
void rewind (FILE*);
void clearerr (FILE*);
int feof  (FILE*);
int ferror (FILE*);
void perror (const char*);

}   // extern "C"


}   // namespace std


_RWSTD_NAMESPACE (__rw) { 

extern _STD::FILE* __rw_stdin;
extern _STD::FILE* __rw_stdout;
extern _STD::FILE* __rw_stderr;

}   // namespace __rw



#endif   // _RWSTD_CSTDIO_H_INCLUDED
