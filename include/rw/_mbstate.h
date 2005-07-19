/***************************************************************************
 *
 * _mbstate.h - definition of mbstate_t
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
 *
 * $Id: //stdlib/dev/include/rw/_mbstate.h#11 $
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

#ifndef _RWSTD_MBSTATE_H_INCLUDED
#define _RWSTD_MBSTATE_H_INCLUDED


#include <rw/_defs.h>


#if defined (__hpux)

#  ifndef _MBSTATE_T
#    define _MBSTATE_T

#    if    !defined (__HP_aCC) && !defined (_RWSTD_NO_LIBC_IN_STD) \
        ||  defined (__HP_aCC) && defined (_NAMESPACE_STD)

_RWSTD_NAMESPACE (std) {

#    endif   //  HP aCC -AA

extern "C" {

// HP-UX 10.00 or 11.00 do not define mbstate_t
// HP-UX 11.11 (and higher) definition of mbstate_t
typedef struct {
    unsigned char __parse_size:  3;
    unsigned char __dummy:       4;
    unsigned char __shift_state: 1;
    char          __parse_buf [7];
} mbstate_t;

}   // extern "C"

#    if    !defined (__HP_aCC) && !defined (_RWSTD_NO_LIBC_IN_STD) \
        ||  defined (__HP_aCC) && defined (_NAMESPACE_STD)

}   // namespace std

#    else

#define _RWSTD_NO_MBSTATE_T_IN_STD

_RWSTD_NAMESPACE (std) {

_USING (::mbstate_t);

}   // namespace std

#    endif   // HP aCC -AA
#  else   // if defined (_MBSTATE_T)
     // /usr/include/sys/_mbstate_t.h #included and mbstate_t defined
#    ifndef _NAMESPACE_STD
#      define _RWSTD_NO_MBSTATE_T_IN_STD

_RWSTD_NAMESPACE (std) {

_USING (::mbstate_t);

}   // namespace std

#    endif   // _NAMESPACE_STD
#  endif   // _MBSTATE_T

#elif !defined (_RWSTD_NO_MBSTATE_T)
#  include _RWSTD_CWCHAR

#  ifndef _RWSTD_NO_USING_LIBC_IN_STD

namespace std {

using ::mbstate_t;

}   // namespace std

#  endif   // _RWSTD_NO_USING_LIBC_IN_STD

#elif !defined (_RWSTD_MBSTATE_T_DEFINED)

#  define _RWSTD_MBSTATE_T_DEFINED

#  if defined (_RWSTD_OS_SUNOS)   // SunOS 5.6

// Solaris 6 does not define mbstate_t; the definition of
// the struct below is taken from <wchar_impl.h> on Solaris 7

#    ifndef _WCHAR_IMPL_H
#      define _WCHAR_IMPL_H
#      ifndef _MBSTATET_H
#        define _MBSTATET_H

extern "C" {

struct __mbstate_t
{

#      ifdef _LP64
    long _C_data [4];
#      else   // if !defined (_LP64)
    int _C_data [6];
#      endif   // _LP64

};

}   // extern "C"

#      endif  // _MBSTATET_H
#    endif   // _WCHAR_IMPL_H

#    ifndef _RWSTD_NO_USING_LIBC_IN_STD

_RWSTD_NAMESPACE (std) {

#    endif   // _RWSTD_NO_USING_LIBC_IN_STD

typedef __mbstate_t mbstate_t;

#    ifndef _RWSTD_NO_USING_LIBC_IN_STD

}   // namespace std

#    endif   // _RWSTD_NO_USING_LIBC_IN_STD

#  else   // any other OS

#    ifndef _RWSTD_NO_USING_LIBC_IN_STD

_RWSTD_NAMESPACE (std) {

#    endif   // _RWSTD_NO_USING_LIBC_IN_STD

extern "C" {

// generic definition of mbstate_t
struct mbstate_t
{
#    ifdef _RWSTD_MBSTATE_T_SIZE
    char _C_data [_RWSTD_MBSTATE_T_SIZE];
#    else
    char _C_data [sizeof (long)];
#    endif

};

}   // extern "C"

#    ifndef _RWSTD_NO_USING_LIBC_IN_STD

}   // namespace std

#    endif   // _RWSTD_NO_USING_LIBC_IN_STD
#  endif   // generic OS
#endif   // _RWSTD_NO_MBSTATE_T && !_RWSTD_MBSTATE_T_DEFINED


#endif   // _RWSTD_MBSTATE_H_INCLUDED
