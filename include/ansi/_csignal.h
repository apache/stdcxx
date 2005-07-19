// -*- C++ -*-
/***************************************************************************
 *
 * csignal - C++ Standard library interface to the ANSI C header signal.h
 *
 * $Id: //stdlib/dev/include/ansi/_csignal.h#7 $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#ifndef _RWSTD_CSIGNAL_H_INCLUDED
#define _RWSTD_CSIGNAL_H_INCLUDED


#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 

#ifndef SIG_DFL
#  define SIG_DFL _RWSTD_SIG_DFL
#endif   // SIG_DFL

#ifndef SIG_ERR
#  define SIG_ERR _RWSTD_SIG_ERR
#endif   // SIG_ERR

#ifndef SIG_IGN
#  define SIG_IGN _RWSTD_SIG_IGN
#endif   // SIG_IGN

#ifndef SIGABRT
#  define SIGABRT _RWSTD_SIGABRT
#endif   // SIGABRT

#ifndef SIGFPE
#  define SIGFPE _RWSTD_SIGFPE
#endif   // SIGFPE

#ifndef SIGILL
#  define SIGILL _RWSTD_SIGILL
#endif   // SIGILL

#ifndef SIGINT
#  define SIGINT _RWSTD_SIGINT
#endif   // SIGINT

#ifndef SIGSEGV
#  define SIGSEGV _RWSTD_SIGSEGV
#endif   // SIGSEGV

#ifndef SIGTERM
#  define SIGTERM _RWSTD_SIGTERM
#endif   // SIGTERM


extern "C" {

#ifdef _RWSTD_SIG_ATOMIC_T
typedef _RWSTD_SIG_ATOMIC_T sig_atomic_t;
#else
typedef long sig_atomic_t;
#endif   // _RWSTD_SIG_ATOMIC_T


void (*signal(int, void (*)(int))) (int);
int raise (int);

}   // extern "C"


}   // namespace std


#endif   // _RWSTD_CSIGNAL_H_INCLUDED
