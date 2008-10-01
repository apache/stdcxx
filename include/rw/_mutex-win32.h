/***************************************************************************
 *
 * _mutex-win32.h - definitions of classes for thread safety
 *                  using Windows threads
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifdef __MINGW32__
#  define _RWSTD_CRITICAL_SECTION _CRITICAL_SECTION
#else
#  define _RWSTD_CRITICAL_SECTION _RTL_CRITICAL_SECTION
#endif

#ifdef _RWSTD_NO_FWD_DECLARATIONS

   // #including <windows.h> without WIN32_LEAN_AND_MEAN macro defined
   // may cause an errors "WinSock.h has already been included"
   // when <winsock2.h> has been included after this header
#  include <windows.h>
#  define _RWSTD_MUTEX_T _RWSTD_CRITICAL_SECTION

#else   // if !defined (_RWSTD_NO_FWD_DECLARATIONS)

 // avoid #including this header (MFC doesn't like it)
 // #  include <windows.h>

extern "C" {

// but rather declare these globals here
struct _RWSTD_CRITICAL_SECTION;

__declspec (dllimport) void __stdcall
InitializeCriticalSection (_RWSTD_CRITICAL_SECTION*);

__declspec (dllimport) void __stdcall
EnterCriticalSection (_RWSTD_CRITICAL_SECTION*);

__declspec (dllimport) void __stdcall
LeaveCriticalSection (_RWSTD_CRITICAL_SECTION*);

__declspec (dllimport) void __stdcall
DeleteCriticalSection (_RWSTD_CRITICAL_SECTION*);

}   // extern "C"

_RWSTD_NAMESPACE (__rw) { 

// fake critical section type
union __rw_critical_section {
    long _C_pad;   // force alignment

#  ifndef _WIN64
    char _C_buf [24 /* == sizeof (_RTL_CRITICAL_SECTION) */];
#  else    // #ifdef _WIN64
    char _C_buf [40 /* == sizeof (_RTL_CRITICAL_SECTION) */];
#  endif   // _WIN64
};

#  define _RWSTD_MUTEX_T _RW::__rw_critical_section

}   // namespace __rw

#endif   // _RWSTD_NO_FWD_DECLARATIONS


_RWSTD_NAMESPACE (__rw) { 

#ifndef _MSC_VER
#  define __try               if (1)
#  define __except(ignore)    else if (0)
#endif   // _MSC_VER

// Win32/64 throws non-C++ exceptions rather than returning error status
// from some system calls like most other operating systems do

inline int __rw_mutex_init (_RWSTD_CRITICAL_SECTION *__mutex)
{
    __try {
        InitializeCriticalSection (__mutex);
    }
    __except (1) {
        return -1;
    }
    return 0;
}

inline int __rw_mutex_destroy (_RWSTD_CRITICAL_SECTION *__mutex)
{
    __try {
        DeleteCriticalSection (__mutex);
    }
    __except (1) {
        return -1;
    }
    return 0;
}

inline int __rw_mutex_lock (_RWSTD_CRITICAL_SECTION *__mutex)
{
    __try {
        EnterCriticalSection (__mutex);
    }
    __except (1) {
        return -1;
    }
    return 0;
}

inline int __rw_mutex_unlock (_RWSTD_CRITICAL_SECTION *__mutex)
{
    __try {
        LeaveCriticalSection (__mutex);
    }
    __except (1) {
        return -1;
    }
    return 0;
}

#define _RWSTD_MUTEX_INIT(mutex)      \
   __rw_mutex_init (_RWSTD_REINTERPRET_CAST (_RWSTD_CRITICAL_SECTION*, &mutex))
#define _RWSTD_MUTEX_DESTROY(mutex)   \
   __rw_mutex_destroy (_RWSTD_REINTERPRET_CAST (_RWSTD_CRITICAL_SECTION*, &mutex))
#define _RWSTD_MUTEX_LOCK(mutex)      \
   __rw_mutex_lock (_RWSTD_REINTERPRET_CAST (_RWSTD_CRITICAL_SECTION*, &mutex))
#define _RWSTD_MUTEX_UNLOCK(mutex)    \
   __rw_mutex_unlock (_RWSTD_REINTERPRET_CAST (_RWSTD_CRITICAL_SECTION*, &mutex))

#ifndef _MSC_VER
#  undef __try
#  undef __except
#endif   // _MSC_VER

}   // namespace __rw
