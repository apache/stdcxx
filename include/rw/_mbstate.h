/***************************************************************************
 *
 * _mbstate.h - definition of mbstate_t
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
 * Copyright 2005-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifndef _RWSTD_RW_MBSTATE_H_INCLUDED
#define _RWSTD_RW_MBSTATE_H_INCLUDED

#ifndef _RWSTD_RW_DEFS_H_INCLUDED
#  include <rw/_defs.h>
#endif   // _RWSTD_RW_DEFS_H_INCLUDED


#if defined (_RWSTD_OS_AIX)
/*** AIX ******************************************************************/

_RWSTD_NAMESPACE (__rw) {

// introduce typedef to avoid const qualification issues
typedef char* __rw_mbstate_t;

#  define _RWSTD_MBSTATE_T   _RW::__rw_mbstate_t

}   // namespace __rw

#elif defined (_RWSTD_OS_HP_UX)
/*** HP-UX ****************************************************************/

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

#      undef _RWSTD_NO_STD_MBSTATE_T
#      define _RWSTD_MBSTATE_T   _STD::mbstate_t
#    else   // if !(HP aCC -AA)

       // let <cwchar> et al know that mbstate_t is not declared
       // in namespace std (and should be introduced there via
       // a using declaration)
#      define _RWSTD_NO_STD_MBSTATE_T

_RWSTD_NAMESPACE (std) {

_USING (::mbstate_t);

}   // namespace std

#      define _RWSTD_MBSTATE_T   mbstate_t
#    endif   // HP aCC -AA
#  else   // if defined (_MBSTATE_T)
     // /usr/include/sys/_mbstate_t.h #included and mbstate_t defined
#    ifndef _NAMESPACE_STD
       // let <cwchar> et al know that mbstate_t is not declared
       // in namespace std (and should be introduced there via
       // a using declaration)
#      define _RWSTD_NO_STD_MBSTATE_T

_RWSTD_NAMESPACE (std) {

_USING (::mbstate_t);

}   // namespace std

#      define _RWSTD_MBSTATE_T   mbstate_t
#    endif   // _NAMESPACE_STD
#  endif   // _MBSTATE_T

#elif defined (_RWSTD_OS_IRIX64)
/*** IRIX64 ***************************************************************/

#  define _RWSTD_MBSTATE_T char

#elif defined (_MSC_VER)
/*** MSVC 6.0 - 8.0 *******************************************************/

#  define _RWSTD_MBSTATE_T int

#elif !defined (_RWSTD_NO_MBSTATE_T)
/*** not HP-UX that has a mbstate_t ***************************************/

#  if defined (_RWSTD_OS_LINUX)
/*** Linux/glibc **********************************************************/

     // define __mbstate_t at file scope (see /usr/include/wchar.h)
#    ifndef __mbstate_t_defined
#      define __mbstate_t_defined 1

extern "C" {

typedef struct {
    int __count;
    union {
        _RWSTD_WINT_T __wch;
        char          __wchb [4];
    } __value;
} __mbstate_t;

}   // extern "C"

#    endif   // __mbstate_t_defined

#    define _RWSTD_MBSTATE_T __mbstate_t

#  elif defined (_RWSTD_OS_SUNOS)
/*** Solaris 7 and beyond *************************************************/

#    ifndef _MBSTATET_H
#      define _MBSTATET_H

extern "C" {

// avoid namespace pollution on Solaris
typedef struct __mbstate_t {

       // Sun uses _LP64 in their header but the macro may not
       // yetbe #defined at this point, depending on what other
       // headers have been #included
#      if 8 == _RWSTD_LONG_SIZE   // LP64
    long __filler [4];
#      else
    int __filler [6];
#      endif   // 8 == sizeof(long)
} __mbstate_t;

}   // extern "C"

#    endif   // _MBSTATET_H

#    define _RWSTD_MBSTATE_T __mbstate_t
     // let <cwchar> et al know that mbstate_t is not declared
     // in namespace std (and should be introduced there via
     // a using declaration)
#    define _RWSTD_NO_STD_MBSTATE_T

#  elif defined (_RWSTD_OS_DARWIN)
/*** Apple Darwin/OS X ****************************************************/

     // include a system header for __mbstate_t
#    include <machine/_types.h>
#    define _RWSTD_MBSTATE_T   __mbstate_t

#  else   // if !defined (_RWSTD_OS_SUNOS)
/*** generic OS ***********************************************************/
     
#    include _RWSTD_CWCHAR
#    define _RWSTD_MBSTATE_T   _RWSTD_C::mbstate_t

#  endif   // _RWSTD_OS_SUNOS

/*** not HP-UX that does not define mbstate_t *****************************/
#elif !defined (_RWSTD_MBSTATE_T_DEFINED)

#  define _RWSTD_MBSTATE_T_DEFINED

/*** SunOS 5.6 and prior **************************************************/
#  if defined (_RWSTD_OS_SUNOS)

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
#    define _RWSTD_MBSTATE_T __mbstate_t
     // let <cwchar> et al know that mbstate_t is not declared
     // in namespace std (and should be introduced there via
     // a using declaration)
#    define _RWSTD_NO_STD_MBSTATE_T
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

#    define _RWSTD_MBSTATE_T   _STD::mbstate_t

#  endif   // generic OS
#endif   // _RWSTD_NO_MBSTATE_T && !_RWSTD_MBSTATE_T_DEFINED


#endif   // _RWSTD_RW_MBSTATE_H_INCLUDED
