/***************************************************************************
 *
 * _defs.h - Common macro definitions to be included by every library file
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

#ifndef _RWSTD_RW_DEFS_H_INCLUDED
#define _RWSTD_RW_DEFS_H_INCLUDED

#ifndef _RWSTD_RW_CONFIG_H_INCLUDED
#  include <rw/_config.h>
#endif   //_RWSTD_RW_CONFIG_H_INCLUDED


// provide definitions for C++ versions of standard C headers
// if a header is not available its name is #defined to the name of
// this file (it's harmless and avoids polluting code with
// tons of ugly platform-specific #ifdefs)
#ifndef _RWSTD_NO_NEW_HEADER

#  define _RWSTD_CASSERT   <cassert>
#  define _RWSTD_CCTYPE    <cctype>
#  define _RWSTD_CERRNO    <cerrno>
#  define _RWSTD_CFLOAT    <cfloat>
#  define _RWSTD_CISO646   <ciso646>
#  define _RWSTD_CLIMITS   <climits>
#  define _RWSTD_CLOCALE   <clocale>
#  define _RWSTD_CMATH     <cmath>
#  define _RWSTD_CSETJMP   <csetjmp>
#  define _RWSTD_CSIGNAL   <csignal>

#  ifdef _RWSTD_EDG_ECCP
      // HACK: fool the vanilla EDG front-end into actually #including
      // the file (it normally doesn't and treats <cstdarg> specially)
#     define _RWSTD_CSTDARG <ansi/cstdarg>
#  else   // if !defined (_RWSTD_EDG_ECCP)
#     define _RWSTD_CSTDARG <cstdarg>
#  endif   // _RWSTD_EDG_ECCP

#  define _RWSTD_CSTDDEF   <cstddef>
#  define _RWSTD_CSTDIO    <cstdio>
#  define _RWSTD_CSTDLIB   <cstdlib>
#  define _RWSTD_CSTRING   <cstring>
#  define _RWSTD_CTIME     <ctime>

#  ifndef _RWSTD_NO_WCHAR_H
#    define _RWSTD_CWCHAR  <cwchar>
#  else
#    define _RWSTD_CWCHAR  <rw/_defs.h>
#  endif   // _RWSTD_NO_WCHAR_H

#  ifndef _RWSTD_NO_WCTYPE_H
#    define _RWSTD_CWCTYPE <cwctype>
#  else
#    define _RWSTD_CWCTYPE <rw/_defs.h>
#  endif   // _RWSTD_NO_WCTYPE_H

#else   // if defined (_RWSTD_NO_NEW_HEADER)

#  define _RWSTD_CASSERT   "assert.h"
#  define _RWSTD_CCTYPE    <ctype.h>
#  define _RWSTD_CERRNO    "errno.h"
#  define _RWSTD_CFLOAT    <float.h>
#  define _RWSTD_CISO646   <iso646.h>
#  define _RWSTD_CLIMITS   <limits.h>
#  define _RWSTD_CLOCALE   <locale.h>
#  define _RWSTD_CMATH     <math.h>
#  define _RWSTD_CSETJMP   "setjmp.h"
#  define _RWSTD_CSIGNAL   <signal.h>
#  define _RWSTD_CSTDARG   <stdarg.h>
#  define _RWSTD_CSTDDEF   <stddef.h>
#  define _RWSTD_CSTDIO    <stdio.h>
#  define _RWSTD_CSTDLIB   <stdlib.h>
#  define _RWSTD_CSTRING   <string.h>
#  define _RWSTD_CTIME     <time.h>

#  ifndef _RWSTD_NO_WCHAR_H
#    define _RWSTD_CWCHAR  <wchar.h>
#  else
#    define _RWSTD_CWCHAR  <rw/_defs.h>
#  endif   // _RWSTD_NO_WCHAR_H

#  ifndef _RWSTD_NO_WCTYPE_H
#    define _RWSTD_CWCTYPE <wctype.h>
#  else
#    define _RWSTD_CWCTYPE <rw/_defs.h>
#  endif   // _RWSTD_NO_WCTYPE_H

#endif   // _RWSTD_NO_NEW_HEADER


#if defined(_WIN64)
#  define _RWSTD_SYS_TYPES_H <basetsd.h>
#elif defined(_WIN32)
#  define _RWSTD_SYS_TYPES_H <rw/_defs.h>
#else
#  define _RWSTD_SYS_TYPES_H <sys/types.h>
#endif   // _WIN32

// provide default #definitions of ANSI C library headers
#if defined (_M_IA64)
   // IA64/Itanium SDK paths
#  if !defined (_RWSTD_NO_ASSERT_H) && !defined (_RWSTD_ANSI_C_ASSERT_H)
     // pathname quoted in order to prevent the assert macro
     // from expanding in the include directive
#    define _RWSTD_ANSI_C_ASSERT_H "../include/crt/assert.h"
#  endif
#  if !defined (_RWSTD_NO_CTYPE_H) && !defined (_RWSTD_ANSI_C_CTYPE_H)
#    define _RWSTD_ANSI_C_CTYPE_H <../include/crt/ctype.h>
#  endif
#  if !defined (_RWSTD_NO_ERRNO_H) && !defined (_RWSTD_ANSI_C_ERRNO_H)
     // pathname quoted in order to prevent the errno macro
     // from expanding in the include directive
#    define _RWSTD_ANSI_C_ERRNO_H "../include/crt/errno.h"
#  endif
#  if !defined (_RWSTD_NO_FLOAT_H) && !defined (_RWSTD_ANSI_C_FLOAT_H)
#    define _RWSTD_ANSI_C_FLOAT_H <../include/crt/float.h>
#  endif
#  if !defined (_RWSTD_NO_ISO646_H) && !defined (_RWSTD_ANSI_C_ISO646_H)
#    define _RWSTD_ANSI_C_ISO646_H <../include/crt/iso646.h>
#  endif  // _RWSTD_NO_ASSERT_H && _RWSTD_ANSI_C_ASSERT_H

#  if !defined (_RWSTD_ANSI_C_LIMITS_H)
#    if !defined (_RWSTD_NO_LIMITS_H)
#      define _RWSTD_ANSI_C_LIMITS_H <../include/crt/limits.h>
#    else
#      define _RWSTD_ANSI_C_LIMITS_H <rw/_defs.h>
#    endif
#  endif  // _RWSTD_ANSI_C_LIMITS_H

#  if !defined (_RWSTD_NO_LOCALE_H) && !defined (_RWSTD_ANSI_C_LOCALE_H)
#    define _RWSTD_ANSI_C_LOCALE_H <../include/crt/locale.h>
#  endif
#  if !defined (_RWSTD_NO_MATH_H) && !defined (_RWSTD_ANSI_C_MATH_H)
#    define _RWSTD_ANSI_C_MATH_H <../include/crt/math.h>
#  endif
#  if !defined (_RWSTD_NO_SETJMP_H) && !defined (_RWSTD_ANSI_C_SETJMP_H)
     // pathname quoted in order to prevent the setjmp macro
     // from expanding in the include directive
#    define _RWSTD_ANSI_C_SETJMP_H "../include/crt/setjmp.h"
#  endif
#  if !defined (_RWSTD_NO_SIGNAL_H) && !defined (_RWSTD_ANSI_C_SIGNAL_H)
#    define _RWSTD_ANSI_C_SIGNAL_H <../include/crt/signal.h>
#  endif
#  if !defined (_RWSTD_NO_STDARG_H) && !defined (_RWSTD_ANSI_C_STDARG_H)
#    define _RWSTD_ANSI_C_STDARG_H <../include/crt/stdarg.h>
#  endif
#  if !defined (_RWSTD_NO_STDDEF_H) && !defined (_RWSTD_ANSI_C_STDDEF_H)
#    define _RWSTD_ANSI_C_STDDEF_H <../include/crt/stddef.h>
#  endif
#  if !defined (_RWSTD_NO_STDIO_H) && !defined (_RWSTD_ANSI_C_STDIO_H)
#    define _RWSTD_ANSI_C_STDIO_H <../include/crt/stdio.h>
#  endif
#  if !defined (_RWSTD_NO_STDLIB_H) && !defined (_RWSTD_ANSI_C_STDLIB_H)
#    define _RWSTD_ANSI_C_STDLIB_H <../include/crt/stdlib.h>
#  endif
#  if !defined (_RWSTD_NO_STRING_H) && !defined (_RWSTD_ANSI_C_STRING_H)
#    define _RWSTD_ANSI_C_STRING_H <../include/crt/string.h>
#  endif
#  if !defined (_RWSTD_NO_TIME_H) && !defined (_RWSTD_ANSI_C_TIME_H)
#    define _RWSTD_ANSI_C_TIME_H <../include/crt/time.h>
#  endif
#  if !defined (_RWSTD_NO_WCHAR_H) && !defined (_RWSTD_ANSI_C_WCHAR_H)
#    define _RWSTD_ANSI_C_WCHAR_H <../include/crt/wchar.h>
#  endif

#  if !defined (_RWSTD_ANSI_C_WCTYPE_H)
#    if !defined (_RWSTD_NO_WCTYPE_H)
#      define _RWSTD_ANSI_C_WCTYPE_H <../include/crt/wctype.h>
#    else
#      define _RWSTD_ANSI_C_WCTYPE_H <rw/_defs.h>
#    endif
#  endif   // _RWSTD_ANSI_C_WCTYPE_H

#elif !defined (_RWSTD_NO_FORCE_RELATIVE_PATHS)

#  undef _RWSTD_ANSI_C_ASSERT_H
   // pathname quoted in order to prevent the assert macro
   // from expanding in the include directive
#  define _RWSTD_ANSI_C_ASSERT_H "../include/assert.h"
#  undef _RWSTD_ANSI_C_CTYPE_H
#  define _RWSTD_ANSI_C_CTYPE_H <../include/ctype.h>
#  undef _RWSTD_ANSI_C_ERRNO_H
   // pathname quoted in order to prevent the errno macro
   // from expanding in the include directive
#  define _RWSTD_ANSI_C_ERRNO_H "../include/errno.h"
#  undef _RWSTD_ANSI_C_FLOAT_H
#  define _RWSTD_ANSI_C_FLOAT_H <../include/float.h>
#  undef _RWSTD_ANSI_C_ISO646_H
#  define _RWSTD_ANSI_C_ISO646_H <../include/iso646.h>

#  undef _RWSTD_ANSI_C_LIMITS_H
#  ifndef _RWSTD_ANSI_C_LIMITS_H
#    define _RWSTD_ANSI_C_LIMITS_H <../include/limits.h>
#  else
#    define _RWSTD_ANSI_C_LIMITS_H <rw/_defs.h>
#  endif

#  undef _RWSTD_ANSI_C_LOCALE_H
#  define _RWSTD_ANSI_C_LOCALE_H <../include/locale.h>
#  undef _RWSTD_ANSI_C_MATH_H
#  define _RWSTD_ANSI_C_MATH_H <../include/math.h>
#  undef _RWSTD_ANSI_C_SETJMP_H
   // pathname quoted in order to prevent the setjmp macro
   // from expanding in the include directive
#  define _RWSTD_ANSI_C_SETJMP_H "../include/setjmp.h"
#  undef _RWSTD_ANSI_C_SIGNAL_H
#  define _RWSTD_ANSI_C_SIGNAL_H <../include/signal.h>
#  undef _RWSTD_ANSI_C_STDARG_H
#  define _RWSTD_ANSI_C_STDARG_H <../include/stdarg.h>
#  undef _RWSTD_ANSI_C_STDDEF_H
#  define _RWSTD_ANSI_C_STDDEF_H <../include/stddef.h>
#  undef _RWSTD_ANSI_C_STDIO_H
#  define _RWSTD_ANSI_C_STDIO_H <../include/stdio.h>
#  undef _RWSTD_ANSI_C_STDLIB_H
#  define _RWSTD_ANSI_C_STDLIB_H <../include/stdlib.h>
#  undef _RWSTD_ANSI_C_STRING_H
#  define _RWSTD_ANSI_C_STRING_H <../include/string.h>
#  undef _RWSTD_ANSI_C_TIME_H
#  define _RWSTD_ANSI_C_TIME_H <../include/time.h>

#  undef _RWSTD_ANSI_C_WCHAR_H
#  ifndef _RWSTD_NO_WCHAR_H
#    define _RWSTD_ANSI_C_WCHAR_H <../include/wchar.h>
#  else
#    define _RWSTD_ANSI_C_WCHAR_H <rw/_defs.h>
#  endif

#  undef _RWSTD_ANSI_C_WCTYPE_H
#  ifndef _RWSTD_NO_WCTYPE_H
#    define _RWSTD_ANSI_C_WCTYPE_H <../include/wctype.h>
#  else
#    define _RWSTD_ANSI_C_WCTYPE_H <rw/_defs.h>
#  endif

#elif defined (_RWSTD_USE_USR_INCLUDE_HEADERS)

#  if !defined (_RWSTD_NO_ASSERT_H) && !defined (_RWSTD_ANSI_C_ASSERT_H)
     // pathname quoted in order to prevent the assert macro
     // from expanding in the include directive
#    define _RWSTD_ANSI_C_ASSERT_H "/usr/include/assert.h"
#  endif
#  if !defined (_RWSTD_NO_CTYPE_H) && !defined (_RWSTD_ANSI_C_CTYPE_H)
#    define _RWSTD_ANSI_C_CTYPE_H </usr/include/ctype.h>
#  endif
#  if !defined (_RWSTD_NO_ERRNO_H) && !defined (_RWSTD_ANSI_C_ERRNO_H)
     // pathname quoted in order to prevent the errno macro
     // from expanding in the include directive
#    define _RWSTD_ANSI_C_ERRNO_H "/usr/include/errno.h"
#  endif
#  if !defined (_RWSTD_NO_FLOAT_H) && !defined (_RWSTD_ANSI_C_FLOAT_H)
#    define _RWSTD_ANSI_C_FLOAT_H </usr/include/float.h>
#  endif
#  if !defined (_RWSTD_NO_ISO646_H) && !defined (_RWSTD_ANSI_C_ISO646_H)
#    define _RWSTD_ANSI_C_ISO646_H </usr/include/iso646.h>
#  endif  // _RWSTD_NO_ASSERT_H && _RWSTD_ANSI_C_ASSERT_H

#  if !defined (_RWSTD_ANSI_C_LIMITS_H)
#    if !defined (_RWSTD_NO_LIMITS_H)
#      define _RWSTD_ANSI_C_LIMITS_H </usr/include/limits.h>
#    else
#      define _RWSTD_ANSI_C_LIMITS_H <rw/_defs.h>
#    endif
#  endif  // _RWSTD_ANSI_C_LIMITS_H

#  if !defined (_RWSTD_NO_LOCALE_H) && !defined (_RWSTD_ANSI_C_LOCALE_H)
#    define _RWSTD_ANSI_C_LOCALE_H </usr/include/locale.h>
#  endif
#  if !defined (_RWSTD_NO_MATH_H) && !defined (_RWSTD_ANSI_C_MATH_H)
#    define _RWSTD_ANSI_C_MATH_H </usr/include/math.h>
#  endif
#  if !defined (_RWSTD_NO_SETJMP_H) && !defined (_RWSTD_ANSI_C_SETJMP_H)
     // pathname quoted in order to prevent the setjmp macro
     // from expanding in the include directive
#    define _RWSTD_ANSI_C_SETJMP_H "/usr/include/setjmp.h"
#  endif
#  if !defined (_RWSTD_NO_SIGNAL_H) && !defined (_RWSTD_ANSI_C_SIGNAL_H)
#    define _RWSTD_ANSI_C_SIGNAL_H </usr/include/signal.h>
#  endif
#  if !defined (_RWSTD_NO_STDARG_H) && !defined (_RWSTD_ANSI_C_STDARG_H)
#    define _RWSTD_ANSI_C_STDARG_H </usr/include/stdarg.h>
#  endif
#  if !defined (_RWSTD_NO_STDDEF_H) && !defined (_RWSTD_ANSI_C_STDDEF_H)
#    define _RWSTD_ANSI_C_STDDEF_H </usr/include/stddef.h>
#  endif
#  if !defined (_RWSTD_NO_STDIO_H) && !defined (_RWSTD_ANSI_C_STDIO_H)
#    define _RWSTD_ANSI_C_STDIO_H </usr/include/stdio.h>
#  endif
#  if !defined (_RWSTD_NO_STDLIB_H) && !defined (_RWSTD_ANSI_C_STDLIB_H)
#    define _RWSTD_ANSI_C_STDLIB_H </usr/include/stdlib.h>
#  endif
#  if !defined (_RWSTD_NO_STRING_H) && !defined (_RWSTD_ANSI_C_STRING_H)
#    define _RWSTD_ANSI_C_STRING_H </usr/include/string.h>
#  endif
#  if !defined (_RWSTD_NO_TIME_H) && !defined (_RWSTD_ANSI_C_TIME_H)
#    define _RWSTD_ANSI_C_TIME_H </usr/include/time.h>
#  endif
#  if !defined (_RWSTD_NO_WCHAR_H) && !defined (_RWSTD_ANSI_C_WCHAR_H)
#    define _RWSTD_ANSI_C_WCHAR_H </usr/include/wchar.h>
#  endif

#  if !defined (_RWSTD_ANSI_C_WCTYPE_H)
#    if !defined (_RWSTD_NO_WCTYPE_H)
#      define _RWSTD_ANSI_C_WCTYPE_H </usr/include/wctype.h>
#    else
#      define _RWSTD_ANSI_C_WCTYPE_H <rw/_defs.h>
#    endif
#  endif   // _RWSTD_ANSI_C_WCTYPE_H

#else   // if !_WIN64 && !_RWSTD_USE_USR_INCLUDE_HEADERS

#  if !defined (_RWSTD_NO_ASSERT_H) && !defined (_RWSTD_ANSI_C_ASSERT_H)
     // pathname quoted in order to prevent the assert macro
     // from expanding in the include directive
#    define _RWSTD_ANSI_C_ASSERT_H "../include/assert.h"
#  endif
#  if !defined (_RWSTD_NO_CTYPE_H) && !defined (_RWSTD_ANSI_C_CTYPE_H)
#    define _RWSTD_ANSI_C_CTYPE_H <../include/ctype.h>
#  endif
#  if !defined (_RWSTD_NO_ERRNO_H) && !defined (_RWSTD_ANSI_C_ERRNO_H)
     // pathname quoted in order to prevent the errno macro
     // from expanding in the include directive
#    define _RWSTD_ANSI_C_ERRNO_H "../include/errno.h"
#  endif
#  if !defined (_RWSTD_NO_FLOAT_H) && !defined (_RWSTD_ANSI_C_FLOAT_H)
#    define _RWSTD_ANSI_C_FLOAT_H <../include/float.h>
#  endif
#  if !defined (_RWSTD_NO_ISO646_H) && !defined (_RWSTD_ANSI_C_ISO646_H)
#    define _RWSTD_ANSI_C_ISO646_H <../include/iso646.h>
#  endif  // _RWSTD_NO_ASSERT_H && _RWSTD_ANSI_C_ASSERT_H

#  if !defined (_RWSTD_ANSI_C_LIMITS_H)
#    if !defined (_RWSTD_NO_LIMITS_H)
#      define _RWSTD_ANSI_C_LIMITS_H <../include/limits.h>
#    else
#      define _RWSTD_ANSI_C_LIMITS_H <rw/_defs.h>
#    endif
#  endif  // _RWSTD_ANSI_C_LIMITS_H

#  if !defined (_RWSTD_NO_LOCALE_H) && !defined (_RWSTD_ANSI_C_LOCALE_H)
#    define _RWSTD_ANSI_C_LOCALE_H <../include/locale.h>
#  endif
#  if !defined (_RWSTD_NO_MATH_H) && !defined (_RWSTD_ANSI_C_MATH_H)
#    define _RWSTD_ANSI_C_MATH_H <../include/math.h>
#  endif
#  if !defined (_RWSTD_NO_SETJMP_H) && !defined (_RWSTD_ANSI_C_SETJMP_H)
     // pathname quoted in order to prevent the setjmp macro
     // from expanding in the include directive
#    define _RWSTD_ANSI_C_SETJMP_H "../include/setjmp.h"
#  endif
#  if !defined (_RWSTD_NO_SIGNAL_H) && !defined (_RWSTD_ANSI_C_SIGNAL_H)
#    define _RWSTD_ANSI_C_SIGNAL_H <../include/signal.h>
#  endif
#  if !defined (_RWSTD_NO_STDARG_H) && !defined (_RWSTD_ANSI_C_STDARG_H)
#    define _RWSTD_ANSI_C_STDARG_H <../include/stdarg.h>
#  endif
#  if !defined (_RWSTD_NO_STDDEF_H) && !defined (_RWSTD_ANSI_C_STDDEF_H)
#    define _RWSTD_ANSI_C_STDDEF_H <../include/stddef.h>
#  endif
#  if !defined (_RWSTD_NO_STDIO_H) && !defined (_RWSTD_ANSI_C_STDIO_H)
#    define _RWSTD_ANSI_C_STDIO_H <../include/stdio.h>
#  endif
#  if !defined (_RWSTD_NO_STDLIB_H) && !defined (_RWSTD_ANSI_C_STDLIB_H)
#    define _RWSTD_ANSI_C_STDLIB_H <../include/stdlib.h>
#  endif
#  if !defined (_RWSTD_NO_STRING_H) && !defined (_RWSTD_ANSI_C_STRING_H)
#    define _RWSTD_ANSI_C_STRING_H <../include/string.h>
#  endif
#  if !defined (_RWSTD_NO_TIME_H) && !defined (_RWSTD_ANSI_C_TIME_H)
#    define _RWSTD_ANSI_C_TIME_H <../include/time.h>
#  endif
#  if !defined (_RWSTD_NO_WCHAR_H) && !defined (_RWSTD_ANSI_C_WCHAR_H)
#    define _RWSTD_ANSI_C_WCHAR_H <../include/wchar.h>
#  endif

#  if !defined (_RWSTD_ANSI_C_WCTYPE_H)
#    if !defined (_RWSTD_NO_WCTYPE_H)
#      define _RWSTD_ANSI_C_WCTYPE_H <../include/wctype.h>
#    else
#      define _RWSTD_ANSI_C_WCTYPE_H <rw/_defs.h>
#    endif
#  endif   // _RWSTD_ANSI_C_WCTYPE_H
#endif  // !_WIN64



// These macros tune the speed vs. space optimization of container
// performance with regard to allocation of memory. These have been tuned
// with speed efficiency as a primary conern. Space efficiency was
// considered, but was secondary. (refer to Onyx QA Entry #4920)
//
// The ratio parameter must be above one for an amortized constant time
// algorithm.  Lowering the ratio will lower rapidity and improve space
// efficiency. This effect will be most noticeable when working with
// containers of few elements (few being less than 32 for the general case)
// If your use case is with containers that typically have many elements
// lowering the ratio will have less effect on rapidity.
//
// The minimum elements parameter refers to minimum amount of additional
// capacity requested (in number of elements) when more memory is needed.
// Rapidity with containers of few elements will be affected most noticeably
// as this amount this is lowered.
//
// For strings, the specializations on char and wchar_t were seen as an
// exploitable common case, and have their own more specialized template
// function overloads in the string header.  These are tweakable here
// with the STRING version of these macros.
//
#if !defined (_RWSTD_RATIO_DIVIDER)
#  define _RWSTD_RATIO_DIVIDER   1000U
#endif

#if !defined(_RWSTD_MINIMUM_NEW_CAPACITY)
#  define _RWSTD_MINIMUM_NEW_CAPACITY   32U
#endif

#if !defined(_RWSTD_NEW_CAPACITY_RATIO)
   // using long doubles to eliminate bogus warnings on g++ 2.95.2/sparc
   // (-W -O2/3 only): warning: overflow on truncation to integer
#  define _RWSTD_NEW_CAPACITY_RATIO 1618U
#endif

#if !defined(_RWSTD_MINIMUM_STRING_CAPACITY)
#  define _RWSTD_MINIMUM_STRING_CAPACITY   128U
#endif

#if !defined(_RWSTD_STRING_CAPACITY_RATIO)
#  define _RWSTD_STRING_CAPACITY_RATIO 1618U
#endif

#if !defined (_RWSTD_MINIMUM_STRINGBUF_CAPACITY)
#  define _RWSTD_MINIMUM_STRINGBUF_CAPACITY   512
#endif   // _RWSTD_MINIMUM_STRINGBUF_CAPACITY

// working around an HP aCC bug (PR #25354 and 22610)
#if !defined (__HP_aCC)
#  define _RWSTD_NEW_CAPACITY(ignore, what, from)   \
      _RW::__rw_new_capacity (from, what)
#else
#  define _RWSTD_NEW_CAPACITY(T, what, from)   \
      _RW::__rw_new_capacity<T>(from, what)
#endif   // HP aCC

// set up MSVC DLL export/import directives
// _DLL - defined by the compiler when either -MD or -MDd is used
// RWDLL - defined for all Rogue Wave(R) products built as shared libs
// _RWSHARED - defined for libstd built/used as a shared lib
#if defined (_MSC_VER) && (defined (RWDLL) || defined (_RWSHARED))

#  ifdef _RWSTD_LIB_SRC
#    define _RWSTD_EXPORT            __declspec (dllexport)
#  else
#    define _RWSTD_EXPORT            __declspec (dllimport)
#  endif   // _RWSTD_LIB_SRC

#    define _RWSTD_CLASS_EXPORT    _RWSTD_EXPORT
#    define _RWSTD_MEMBER_EXPORT   /* empty */
#else
   // disable MSVC hacks
#  define _RWSTD_EXPORT          /* empty */
#  define _RWSTD_CLASS_EXPORT    /* empty */
#  define _RWSTD_MEMBER_EXPORT   /* empty */
#endif // _MSC_VER

#ifdef _RWSTD_NO_BOOL
#  define bool    int
#  define false   0
#  define true    1
#endif // _RWSTD_NO_BOOL


#ifndef _RWSTD_NO_EXPORT
#  define _EXPORT   export
#else
#  define _EXPORT   /* empty */
#endif


#ifndef _RWSTD_NO_MUTABLE
#  define _MUTABLE   mutable
#else
#  define _MUTABLE   /* empty */
#endif


//
// Macro for forming or omitting default template arguments in constructors
//

#ifndef _RWSTD_NO_DEFAULT_TEMPLATE_ARGS
#  define _RWSTD_DEFAULT_ARG(n) = n
#else
#  define _RWSTD_DEFAULT_ARG(n)
#endif



// Work around a bug in MSVC (see PR# 28547); used in basic_string
#if !defined (_MSC_VER) || _MSC_VER < 1300
#  define _RWSTD_REDECLARED_DEFAULT(arg)   = arg
#else   // if !(!defined (_MSC_VER) || (_MSC_VER < 1300))
#  define _RWSTD_REDECLARED_DEFAULT(ignore)
#endif   // !defined (_MSC_VER) || (_MSC_VER < 1300)


// Macro for casting, using either the "old" method
// or the new C++ cast system
#ifndef _RWSTD_NO_STATIC_CAST
#  define _RWSTD_STATIC_CAST(T, x)      static_cast< T >(x)
#  define _RWSTD_REINTERPRET_CAST(T, x) reinterpret_cast< T >(x)
#  define _RWSTD_CONST_CAST(T, x)       const_cast< T >(x)
#else   // if defined (_RWSTD_NO_STATIC_CAST)
#  define _RWSTD_STATIC_CAST(T, x)      ((T)x)
#  define _RWSTD_REINTERPRET_CAST(T, x) ((T)x)
#  define _RWSTD_CONST_CAST(T, x)       ((T)x)
#endif   // _RWSTD_NO_STATIC_CAST

#ifndef _RWSTD_NO_DYNAMIC_CAST
#  define _RWSTD_DYNAMIC_CAST(T, x)      dynamic_cast< T >(x)
#else   // if defined (_RWSTD_NO_DYNAMIC_CAST)
#  define _RWSTD_DYNAMIC_CAST(T, x)      ((T)x)
#endif   // _RWSTD_NO_DYNAMIC_CAST

//
// Macros for the new template specialization syntax
//

#ifdef _RWSTD_NO_NEW_FUNC_TEMPLATE_SYNTAX
#  define _RWSTD_SPECIALIZED_FUNCTION
#else
#  define _RWSTD_SPECIALIZED_FUNCTION template<>
#endif

#ifdef _RWSTD_NO_NEW_CLASS_TEMPLATE_SYNTAX
#  define _RWSTD_SPECIALIZED_CLASS
#else
#  define _RWSTD_SPECIALIZED_CLASS template<>
#endif

// for compilers or environments that do not support namespaces
#ifndef _RWSTD_NO_NAMESPACE
#  define _RWSTD_NAMESPACE(name)    namespace name
#  define _USING(name)              using name

#  undef _STD
#  define _STD   std
#  define _RW    __rw

#else   // if defined (_RWSTD_NO_NAMESPACE)
#  define _RWSTD_NAMESPACE(ignore)    extern "C++"
#  define _USING(ignore)              typedef void __rw_unused_type

#  undef _STD
#  define _STD
#  define _RW
#endif   // _RWSTD_NO_NAMESPACE

//
// for compilers that don't like specialized friends
// such as operator== in template classes
//
#ifdef _RWSTD_NO_SPECIALIZED_FRIEND
#  ifdef __TURBOC__
#    define _RWSTD_SPECIALIZED_FRIEND(name)   (_STD::name)
#  else   // if !defined (__TURBOC__)
#    define _RWSTD_SPECIALIZED_FRIEND(name)   name
#  endif   // __TURBOC__
#else   // if defined (_RWSTD_NO_SPECIALIZED_FRIEND)
#  ifdef __TURBOC__
#    define _RWSTD_SPECIALIZED_FRIEND(name)   (_STD::name)
#  else   // if !defined (__TURBOC__)
     // the space between `name' and `<>' is necessary
     // to prevent ..._FRIEND (operator<) from expanding into operator<<>
     // (only the GNU preprocessor inserts a space)
#    define _RWSTD_SPECIALIZED_FRIEND(name)   name <>
#  endif  // __TURBOC__
#endif   // _RWSTD_NO_SPECIALIZED_FRIEND


//
// If compiler supports member and default templates then it supports
// the _RWSTD_ALLOCATOR
//
#ifndef _RWSTD_NO_SIMPLE_DEFAULT_TEMPLATES
#  define _RWSTD_ALLOCATOR
#endif


#define _RWSTD_MB_CUR_MAX 16  // FIXME: must be determined dynamically

// helper macros for token pasting
#define _RWSTD_DO_PASTE(a, b)    a ## b
#define _RWSTD_PASTE(a, b)       _RWSTD_DO_PASTE (a, b)

#ifdef _RWSTD_REENTRANT
// synchronizes access by all objects holding the same mutex
#  define _RWSTD_MT_GUARD(mutex)  \
          _RW::__rw_guard _RWSTD_PASTE (__guard, __LINE__) (mutex)

// synchronizes access by all threads holding the same mutex 
#  define _RWSTD_MT_STATIC_GUARD(type)                               \
          typedef _RW::__rw_type<type,__LINE__> _UniqueType;         \
          _RWSTD_MT_CLASS_GUARD(_UniqueType)

// synchronizes access by all objects of the same type
#  define _RWSTD_MT_CLASS_GUARD(type)                                \
          _RWSTD_MT_GUARD (_RW::__rw_get_static_mutex ((type*)0))

#  define _RWSTD_ATOMIC_PREINCREMENT(x, mutex)                       \
          _RW::__rw_atomic_preincrement (x, mutex)

#  define _RWSTD_ATOMIC_PREDECREMENT(x, mutex)                       \
          _RW::__rw_atomic_predecrement (x, mutex)

#  define _RWSTD_ATOMIC_SWAP(x, y, mutex)                            \
          _RW::__rw_atomic_exchange  (x, y, mutex)

#  ifndef _RWSTD_NO_EXT_REENTRANT_IO
// conditional swap used in iostreams
#    define _RWSTD_ATOMIC_IO_SWAP(x, y, mutex)                       \
            ((this->flags () & _RWSTD_IOS_NOLOCK) ?                  \
                _RW::__rw_ordinary_exchange  (x, y)                  \
             :  _RW::__rw_atomic_exchange  (x, y, mutex))

#  else   // if defined (_RWSTD_NO_EXT_REENTRANT_IO)
#    define _RWSTD_ATOMIC_IO_SWAP(x, y, mutex)                       \
            _RWSTD_ATOMIC_SWAP(x, y, mutex)
#  endif    // _RWSTD_NO_EXT_REENTRANT_IO

#  if defined (_PA_RISC2_0)

#    define _RWSTD_STRING_ATOMIC_PREINCREMENT(x, mutex)              \
            _RW::__rw_string_atomic_preincrement (x, mutex)

#    define _RWSTD_STRING_ATOMIC_PREDECREMENT(x, mutex)              \
            _RW::__rw_string_atomic_predecrement (x, mutex)

#    define _RWSTD_STRING_ATOMIC_SWAP(x, y, mutex)                   \
            _RW::__rw_string_atomic_exchange (x, y, mutex)

#  else   // if !_PA_RISC2_0
#    define _RWSTD_STRING_ATOMIC_PREINCREMENT(x, mutex)              \
            _RWSTD_ATOMIC_PREINCREMENT (x, mutex)
#    define _RWSTD_STRING_ATOMIC_PREDECREMENT(x, mutex)              \
            _RWSTD_ATOMIC_PREDECREMENT (x, mutex)
#    define _RWSTD_STRING_ATOMIC_SWAP(x, y, mutex)                   \
            _RWSTD_ATOMIC_SWAP (x, y, mutex)
#  endif   // !_PA_RISC2_0

   // thread-local storage
#  ifndef _RWSTD_NO_TLS
#    ifndef _RWSTD_THREAD
       // if the macro isn't yet #defined (e.g., in _config.h)
       // use the generally accepted (e.g., HP aCC, gcc, SunPro)
       // __thread extension
#      define _RWSTD_THREAD   __thread
#    endif   // _RWSTD_THREAD

#    define _RWSTD_THREAD_PREINCREMENT(x, ignore)                    \
        (++(x))
#    define _RWSTD_THREAD_PREDECREMENT(x, ignore)                    \
        (--(x))
#    define _RWSTD_THREAD_SWAP(x, y, ignore)                         \
        _RW::__rw_ordinary_exchange  (x, y)

#  endif   // _RWSTD_NO_TLS

#else   // if !defined _RWSTD_REENTRANT

#  define _RWSTD_MT_GUARD(ignore)                   ((void)0)
#  define _RWSTD_MT_STATIC_GUARD(name)              _RWSTD_MT_GUARD (name)
#  define _RWSTD_MT_CLASS_GUARD(type)               _RWSTD_MT_GUARD (type)

#  define _RWSTD_ATOMIC_PREINCREMENT(x, ignore)     ++(x)
#  define _RWSTD_ATOMIC_PREDECREMENT(x, ignore)     --(x)
#  define _RWSTD_ATOMIC_SWAP(x, y, ignore)  \
          _RW::__rw_atomic_exchange ((x), (y), false)

#  define _RWSTD_ATOMIC_IO_SWAP(x, y, ignore)  \
          _RW::__rw_ordinary_exchange ((x), (y))

#  define _RWSTD_STRING_ATOMIC_PREINCREMENT(x, mutex) \
          _RWSTD_ATOMIC_PREINCREMENT(x, mutex)
#  define _RWSTD_STRING_ATOMIC_PREDECREMENT(x, mutex) \
          _RWSTD_ATOMIC_PREDECREMENT(x, mutex)
#  define _RWSTD_STRING_ATOMIC_SWAP(x, y, mutex) \
          _RWSTD_ATOMIC_SWAP(x, y, mutex)

#endif   // _RWSTD_REENTRANT

// thread-local storage declaration
#ifndef _RWSTD_THREAD
#  define _RWSTD_THREAD   /* empty */
#endif   // _RWSTD_THREAD

// thread-local storage manipulation
#ifndef _RWSTD_THREAD_PREINCREMENT
#  define _RWSTD_THREAD_PREINCREMENT(x, mutex)                  \
          _RWSTD_ATOMIC_PREINCREMENT (x, mutex)
#endif   // _RWSTD_THREAD_PREINCREMENT

#ifndef _RWSTD_THREAD_PREDECREMENT
#  define _RWSTD_THREAD_PREDECREMENT(x, mutex)                  \
          _RWSTD_ATOMIC_PREDECREMENT (x, mutex)
#endif   // _RWSTD_THREAD_PREDECREMENT

#ifndef _RWSTD_THREAD_SWAP
#  define _RWSTD_THREAD_SWAP(x, y, ignore)                       \
          _RWSTD_ATOMIC_SWAP (x, y, mutex)
#endif   // _RWSTD_THREAD_SWAP

// macro for qualifying C library names in the std namespace
#ifndef _RWSTD_NO_LIBC_IN_STD
#  ifndef _RWSTD_NO_NAMESPACE
#    define _RWSTD_C std
#  else
#    define _RWSTD_C /* empty */
#  endif   // _RWSTD_NO_LIBC_IN_STD
#else
#  define _RWSTD_C   /* empty */
#endif

#define _RWSTD_INVALID_FILE_DESC -1
#define _RWSTD_INVALID_FILE_PTR   0

#ifndef _RWSTD_NO_NATIVE_IO
# define _RWSTD_INVALID_FILE     -1
# define _RWSTD_INVALID_OPENMODE -1
#else
# define _RWSTD_INVALID_FILE      (FILE*)0
# define _RWSTD_INVALID_OPENMODE  0
#endif // _RWSTD_NO_NATIVE_IO


//  std::streamoff and std::streamsize defined in <ios>
#define _RWSTD_STREAMOFF    _RWSTD_PTRDIFF_T
#define _RWSTD_STREAMSIZE   _RWSTD_PTRDIFF_T


// std::ios_base static const data members' values
#define _RWSTD_IOS_BOOLALPHA    0x00001
#define _RWSTD_IOS_DEC          0x00002
#define _RWSTD_IOS_FIXED        0x00004
#define _RWSTD_IOS_HEX          0x00008
#define _RWSTD_IOS_INTERNAL     0x00010
#define _RWSTD_IOS_LEFT         0x00020
#define _RWSTD_IOS_OCT          0x00040
#define _RWSTD_IOS_RIGHT        0x00080
#define _RWSTD_IOS_SCIENTIFIC   0x00100
#define _RWSTD_IOS_SHOWBASE     0x00200
#define _RWSTD_IOS_SHOWPOINT    0x00400
#define _RWSTD_IOS_SHOWPOS      0x00800
#define _RWSTD_IOS_SKIPWS       0x01000
#define _RWSTD_IOS_UNITBUF      0x02000
#define _RWSTD_IOS_UPPERCASE    0x04000
#define _RWSTD_IOS_BIN          0x08000
// NOLOCK[BUF] used rather than LOCK[BUF] to maintain functional compatibility
// with release 2.2 (which was always safe when these bits were cleared)
#define _RWSTD_IOS_NOLOCK       0x10000
#define _RWSTD_IOS_NOLOCKBUF    0x20000
// SYNC_STDIO is set for stream objects that may need to be synchronized
// with stdio (by default the standard iostream objects such as cout)
#define _RWSTD_IOS_SYNC_STDIO   0x40000

#define _RWSTD_IOS_FLOATFIELD   (_RWSTD_IOS_FIXED | _RWSTD_IOS_SCIENTIFIC)

#define _RWSTD_IOS_ADJUSTFIELD    \
        (_RWSTD_IOS_LEFT | _RWSTD_IOS_RIGHT | _RWSTD_IOS_INTERNAL)

#ifndef _RWSTD_NO_EXT_BIN_IO
#  define _RWSTD_IOS_BASEFIELD    \
          (_RWSTD_IOS_DEC | _RWSTD_IOS_HEX | _RWSTD_IOS_OCT | _RWSTD_IOS_BIN)
#else
#  define _RWSTD_IOS_BASEFIELD    \
          (_RWSTD_IOS_DEC | _RWSTD_IOS_HEX | _RWSTD_IOS_OCT)
#endif   // _RWSTD_NO_EXT_BIN_IO

// offset of most significant bits where numeric base is stored
#define _RWSTD_IOS_BASEMASK      63
#define _RWSTD_IOS_BASEOFF       26

#define _RWSTD_IOS_GOODBIT       0x00
#define _RWSTD_IOS_BADBIT        0x01
#define _RWSTD_IOS_EOFBIT        0x02
#define _RWSTD_IOS_FAILBIT       0x04
#define _RWSTD_IOS_NOTHROW       0x08   // prevents clear() from throwing
#define _RWSTD_IOS_RETHROW       0x10   // clear() rethrows if appropriate

// ios_base::openmode constants
#define _RWSTD_IOS_APP           0x01
#define _RWSTD_IOS_BINARY        0x02
#define _RWSTD_IOS_IN            0x04
#define _RWSTD_IOS_OUT           0x08
#define _RWSTD_IOS_TRUNC         0x10
#define _RWSTD_IOS_ATE           0x20
#define _RWSTD_IOS_NOCREATE      0x40   // filebuf extension
#define _RWSTD_IOS_NOREPLACE     0x80   // filebuf extension
#define _RWSTD_IOS_STDIO         0x100  // filebuf extension
#define _RWSTD_IOS_NATIVE        0x200  // filebuf extension

#define _RWSTD_IOS_OPENMODE_MASK                 \
  (  _RWSTD_IOS_APP      | _RWSTD_IOS_BINARY     \
   | _RWSTD_IOS_IN       | _RWSTD_IOS_OUT        \
   | _RWSTD_IOS_TRUNC    | _RWSTD_IOS_ATE        \
   | _RWSTD_IOS_NOCREATE | _RWSTD_IOS_NOREPLACE  \
   | _RWSTD_IOS_STDIO    | _RWSTD_IOS_NATIVE)

#if    !defined (_RWSTD_NO_STATIC_IOSTREAM_INIT)  \
    && !defined (_RWSTD_NO_IOSTREAM_OBJECT_REFS)
   // standard iostream objects are references
#  define _RWSTD_IOSTREAM_OBJECT(name)   &name
#else
   // standard iostream objects are declared to be real objects
   // they may be implemented as real objects of the same type
   // that get destroyed during program lifetime or as objects
   // of some POD type that do not get destroyed at all
#  define _RWSTD_IOSTREAM_OBJECT(name)   name
#endif   //  _RWSTD_NO_STATIC_IOSTREAM_INIT && !_RWSTD_NO_IOSTREAM_OBJECT_REFS


// exceptions can be completely disabled (bodies of catch blocks are still
// compiled but will be removed by the optimizing stage of the compiler)
// this approach reveals any errors even if _RWSTD_NO_EXCEPTIONS is #defined
#ifndef _RWSTD_NO_EXCEPTIONS
#  define _TRY           try
#  define _CATCH(ex)     catch (ex)
#  define _THROW(ex)     throw ex
#  define _RETHROW       throw
#else   // if defined (_RWSTD_NO_EXCEPTIONS)
#  define _TRY
#  define _CATCH(ignore)   while (0)
#  define _THROW(ignore)   ((void)0)
#  define _RETHROW         ((void)0)
#endif   // _RWSTD_NO_EXCEPTIONS


// for compilers that can't handle standard allocators we provide four
// workarounds: _RWSTD_REBIND, used in place of _Allocator::rebind, allows
// containers to use the same allocator for different data types
// _RWSTD_ALLOC_TYPE is required to "rebind", or rather wrap,  the user
// supplied non-conforming allocator in allocator_interface so that the
// container need not be aware of the allocator's special properties (namely
// the fact that the allocator allocates in terms of bytes rather than
// elemements)
//
// _RWSTD_VALUE_ALLOC creates a temporary allocator_interface wrapper
// from the given parameter (usually *this) when allocating values in
// the sequence containers with non-conforming allocators.
// _RWSTD_VALUE_ALLOC_CAST casts the given container to a reference to
// a value allocator, allowing access to inherited allocator members
// outside of the class.  For non-conforming compilers, this macro is
// equivalent to _RWSTD_VALUE_ALLOC.
//
// NOTE: the spaces around type below are necessary to prevent errors
//       if `to' is a template-id and the preprocessor doesn't put
//       the spaces there automatically (GNU does, Sun does not)
#ifdef _RWSTD_ALLOCATOR
#  ifndef _RWSTD_NO_DEPENDENT_TEMPLATE
#    define _RWSTD_REBIND(from, to) \
            typename from::template rebind < to >::other
#  else
#    define _RWSTD_REBIND(from, to) \
            typename from:: rebind < to >::other
#  endif   // _RWSTD_NO_DEPENDENT_TEMPLATE

#  define _RWSTD_ALLOC_TYPE(Allocator, ignore)        \
          Allocator
#  define _RWSTD_VALUE_ALLOC(ignore, object, call)    \
          (object).allocator_type::call
#  define _RWSTD_VALUE_ALLOC_CAST(rvalue)             \
          _RWSTD_STATIC_CAST (allocator_type&, rvalue)
#else   // if !defined (_RWSTD_ALLOCATOR)
#  define _RWSTD_REBIND(from, to) \
          _STD::allocator_interface < from, to >
#  define _RWSTD_ALLOC_TYPE(Allocator, T)             \
          _RWSTD_REBIND (Allocator, T)
#  define _RWSTD_VALUE_ALLOC(Allocator, object, call) \
           Allocator (object).call
#  define _RWSTD_VALUE_ALLOC_CAST(rvalue)             \
          _C_value_alloc_type (rvalue)
#endif   // _RWSTD_ALLOCATOR


// for compilers that can't deal with template functions parametrized
// on the function's return type (as opposed to function arguments)
//
// NOTE: the spaces around type below are necessary to prevent errors
//       if `type' is a template-id and the preprocessor doesn't put
//       the spaces there automatically (GNU does, Sun does not)
#ifndef _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE

#  define _STD_HAS_FACET(type, arg)    _STD::has_facet < type >(arg)
#  define _HAS_FACET(type, arg)        has_facet < type >(arg)

#  define _STD_USE_FACET(type, arg)    _STD::use_facet < type >(arg)
#  define _USE_FACET(type, arg)        use_facet < type >(arg)

#  define _GET_TEMP_BUFFER(type, size) get_temporary_buffer < type >(size)
#else   // if defined (_RWSTD_NO_TEMPLATE_ON_RETURN_TYPE)

#  define _STD_HAS_FACET(type, arg)    _STD::has_facet (arg, (type*)(0))
#  define _HAS_FACET(type, arg)        has_facet (arg, (type*)(0))

#  define _STD_USE_FACET(type, arg)    _STD::use_facet (arg, (type*)(0))
#  define _USE_FACET(type, arg)        use_facet (arg, (type*)(0))

#  define _GET_TEMP_BUFFER(type, size) get_temporary_buffer (size, (type*)0)
#endif   // _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE

// working around g++ 2.95.2 -fno-honor-std inability to use std::use_facet
#if    defined (__GNUG__) && __GNUC__ <= 2 && __GNUC_MINOR__ < 97 \
    && defined (_RWSTD_NO_HONOR_STD)
   // use_facet is introduced into namespace _RW via a using declaration
#  undef _STD_HAS_FACET
#  define _STD_HAS_FACET(type, arg)    _HAS_FACET (type, arg)

#  undef _STD_USE_FACET
#  define _STD_USE_FACET(type, arg)    _USE_FACET (type, arg)
#endif   // // __GNUG__ >= 2.97 || !_RWSTD_NO_HONOR_STD

// support for building the new locale (v.3)
#ifndef _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE
#  ifndef _RWSTD_NO_HONOR_STD
#    define _RWSTD_USE_FACET(type, arg)   _STD::use_facet< type >(arg)
#  else
#    define _RWSTD_USE_FACET(type, arg)   use_facet< type >(arg)
#  endif
#else   // if defined (_RWSTD_NO_TEMPLATE_ON_RETURN_TYPE)
#  ifndef _RWSTD_NO_HONOR_STD
#    define _RWSTD_USE_FACET(type, arg)   _STD::use_facet(arg, (type*)(0))
#  else
#    define _RWSTD_USE_FACET(type, arg)   use_facet(arg, (type*)(0))
#  endif
#endif // _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE


#ifndef _RWSTD_NO_NONCLASS_ARROW_RETURN
   // expected signature: "_TypeT* operator->()"
   // and/or:             "const _TypeT* operator->() const"
#  define _RWSTD_OPERATOR_ARROW(signature) signature { return &**this; }
#else
#  define _RWSTD_OPERATOR_ARROW(ignore)
#endif   // _RWSTD_NO_NONCLASS_ARROW_RETURN


// to prevent warnings about unused arguments
#define _RWSTD_UNUSED(arg)   ((void)&arg)


// allows all externs to be declared/defined only in headers
// and defined just by first #defining _RWSTD_DEFINE_EXTERNS
// and then #including the header in a .cpp file
#ifndef _RWSTD_DEFINE_EXTERNS
#  define _RWSTD_EXTERN(decl, ignore)   extern decl
#else
#  define _RWSTD_EXTERN(decl, value)    extern decl = value
#endif   // _RWSTD_DEFINE_EXTERNS


// allows static const integral or enum class members to be initialized
// inside the class and defined (w/o being initialized) outside
// of the class (as required by 9.4.2, p4)
#ifndef _RWSTD_NO_STATIC_CONST_MEMBER_INIT
#  define _RWSTD_STATIC_CONST(type, init)    static const type init
#  ifndef _RWSTD_NO_STATIC_CONST_MEMBER_DEFINITION
#    define _RWSTD_DEFINE_STATIC_CONST(decl)   decl
#  else   // if defined (_RWSTD_NO_STATIC_CONST_MEMBER_DEFINITION)
     // disable defintion of static const integral members
     // to work around compiler bugs such as those in MSVC
     // or Intel C++/Windows that cause multiple definition
     // linker errors (see PR #26562 and #30260)
#    define _RWSTD_DEFINE_STATIC_CONST(ignore)   /* empty */
#  endif   // _RWSTD_NO_STATIC_CONST_MEMBER_DEFINITION
#else   // if defined (_RWSTD_NO_STATIC_CONST_MEMBER_INIT)
#  define _RWSTD_STATIC_CONST(ignore, init)  enum { init }
#  define _RWSTD_DEFINE_STATIC_CONST(ignore)   /* empty */
#endif   // _RWSTD_NO_STATIC_CONST_MEMBER_INIT

#ifndef _RWSTD_NO_STATICS_IN_TEMPLATE
#  define _RWSTD_INTERNAL   static
#else   // if defined (_RWSTD_NO_STATICS_IN_TEMPLATE)
  // symbols declared static cannot be referenced from
  // template (e.g., SunPro 5.3 -- see PR #25961)
#  define _RWSTD_INTERNAL   /* static */
#endif   // _RWSTD_NO_STATICS_IN_TEMPLATE


// Selection of vendor dependent "format" string argument to wcsftime()
#ifndef _RWSTD_NO_WCSFTIME
   // 7.24.5.1 of C99 - wcsftime()'s 3rd arg is const wchar* [restrict]
   //                   HP-UX 11 (and perhaps others) still uses const char*
#  ifndef _RWSTD_NO_WCSFTIME_WCHAR_T_FMAT
#     define _RWSTD_FMT_TYPE(arg)  const wchar_t* arg
#     define _RWSTD_FMT_ASSGN(str) L##str
#  else
#     define _RWSTD_FMT_TYPE(arg)  const char* arg
#     define _RWSTD_FMT_ASSGN(str) str
#  endif
#else   // if defined (_RWSTD_NO_WCSFTIME)
#  define _RWSTD_FMT_TYPE(arg)     const char* arg
#  define _RWSTD_FMT_ASSGN(str)    str
#endif // _RWSTD_NO_WCSFTIME


// _Iterator typedefs
// (note that you must use a semi-colon at the end of this macro)
#define _RWSTD_ITERATOR_TYPES(_Iterator)                               \
     typedef typename _Iterator::difference_type difference_type;     \
     typedef typename _Iterator::value_type value_type;               \
     typedef typename _Iterator::pointer pointer;                     \
     typedef typename _Iterator::reference reference;                 \
     typedef typename _Iterator::iterator_category iterator_category

// helpers making working w/o iterator_traits transparent
#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC

#  define _RWSTD_VALUE_TYPE(iterT) \
       (typename _STD::iterator_traits< iterT >::value_type*)0
#  define _RWSTD_DIFFERENCE_TYPE(iterT)   \
       (typename _STD::iterator_traits< iterT >::difference_type*)0

#  if defined (SNI) || defined (__SUNPRO_CC) && __SUNPRO_CC <= 0x530
     // hacking around the inability of Siemens CDS++ and SunPro 5.3
     // to use the ctor syntax for type-dependent nested type names
#    define _RWSTD_ITERATOR_CATEGORY(iterT, ignore) \
         (const typename _STD::iterator_traits< iterT >::iterator_category&) \
         (_STD::forward_iterator_tag ())

#  else
#    define _RWSTD_ITERATOR_CATEGORY(iterT, ignore) \
         typename _STD::iterator_traits< iterT >::iterator_category ()
#  endif

#  define _RWSTD_REVERSE_ITERATOR(iterT, ign1, ign2, ign3) \
       _STD::reverse_iterator<iterT>

#else   // if defined (_RWSTD_NO_CLASS_PARTIAL_SPEC)

#  define _RWSTD_VALUE_TYPE(iterT) __value_type ((iterT*)0)
   // assume default ctor is defined and accessible
#  define _RWSTD_DIFFERENCE_TYPE(iterT)   \
       _STD::__distance_type (iterT ())
   // macro cannot use the default ctor to construct a temporary
   // with the type of the first argument since the default ctor
   // may not exist or be accessible (e.g., istream_iterator<>)
#  define _RWSTD_ITERATOR_CATEGORY(ignore, iter) \
       _STD::__iterator_category (iter)

#  define _RWSTD_REVERSE_ITERATOR(iterT, Ref, Ptr, Tag)         \
       _STD::reverse_iterator<iterT, _STD::Tag, value_type,     \
                              Ref, Ptr, difference_type>

#endif   // _RWSTD_NO_CLASS_PARTIAL_SPEC

// Use -D_RWSTDDEBUG to compile a version of the libraries to debug
// user code. This will perform pre- and post-condition checks but
// will produce larger code and run more slowly. This option affects
// the binary compatibility of generated code.

#if defined (_RWSTDDEBUG)
#  ifndef _RWSTD_NO_PRETTY_FUNCTION
     // gcc, HP aCC, and Intel C++ all support __PRETTY_FUNCTION__
#    define _RWSTD_ASSERT(expr)                                            \
     ((expr) ? (void)0 : _RW::__rw_assert_fail (#expr, __FILE__, __LINE__, \
                                                __PRETTY_FUNCTION__))
#  elif !defined (_RWSTD_NO_FUNC)
     // C99 specifies the __func__ special identifier
#    define _RWSTD_ASSERT(expr)                                            \
     ((expr) ? (void)0 : _RW::__rw_assert_fail (#expr, __FILE__, __LINE__, \
                                                __func__))
#  elif defined (__FUNCSIG__)
     // MSVC macro
#    define _RWSTD_ASSERT(expr)                                            \
     ((expr) ? (void)0 : _RW::__rw_assert_fail (#expr, __FILE__, __LINE__, \
                                                __FUNCSIG__))
#  else   // if _RWSTD_NO_PRETTY_FUNCTION && _RWSTD_NO_FUNC
#    define _RWSTD_ASSERT(expr)   \
     ((expr) ? (void)0 : _RW::__rw_assert_fail (#expr, __FILE__, __LINE__, 0))
#  endif
#elif defined (_RWSTD_ASSUME)
#  define _RWSTD_ASSERT(expr)   _RWSTD_ASSUME (expr)
#else   //  if !defined (_RWSTDDEBUG)
#  define _RWSTD_ASSERT(ignore)   ((void)0)
#endif   // _RWSTDDEBUG

#ifndef _RWSTD_ASSUME
   // must be #defined after _RWSTD_ASSERT
#  define _RWSTD_ASSUME(expr)   _RWSTD_ASSERT (expr)
#endif   // _RWSTD_ASSUME

#define _RWSTD_STR(x)       #x
#define _RWSTD_STRSTR(x)    _RWSTD_STR(x)

#ifndef _RWSTD_NO_PRETTY_FUNCTION
#  define _RWSTD_FUNC(ignore)   _RWSTD_FILE_LINE, __PRETTY_FUNCTION__
#elif !defined (_RWSTD_NO_FUNC)
#  define _RWSTD_FUNC(ignore)   _RWSTD_FILE_LINE, __func__
#elif defined (__FUNCSIG__)
#  define _RWSTD_FUNC(ignore)   _RWSTD_FILE_LINE, __FUNCSIG__
#else
#  define _RWSTD_FUNC(name)     _RWSTD_FILE_LINE, name
#endif

#define _RWSTD_FILE_LINE   __FILE__ ":" _RWSTD_STRSTR (__LINE__)

#if !defined (__DECCXX_VER) || __DECCXX_VER > 60290024
#  define _RWSTD_REQUIRES(pred, args)   (pred) ? (void)0 : _RW::__rw_throw args
#else
   // working around a DEC cxx bug
#  define _RWSTD_REQUIRES(pred, args)   if (pred) ; else _RW::__rw_throw args
#endif


// function exception specification
#if    !defined (_RWSTD_NO_EXCEPTIONS) \
    && !defined (_RWSTD_NO_EXCEPTION_SPECIFICATION)
   // type_id_list is a possibly empty parenthesized list
   //of comma-separated type-id's
#  define _THROWS(type_id_list)   throw type_id_list
#else   // if _RWSTD_NO_EXCEPTIONS || _RWSTD_NO_EXCEPTION_SPECIFICATION
#  define _THROWS(ignore)         /* empty */
#endif   // !_RWSTD_NO_EXCEPTIONS && !_RWSTD_NO_EXCEPTION_SPECIFICATION


// function exception specification on extern "C" libc functions
#ifndef _RWSTD_NO_LIBC_EXCEPTION_SPEC
#  define _LIBC_THROWS(/* empty */)   throw ()
#else
#  define _LIBC_THROWS(/* empty */)   /* empty */
#endif   // _RWSTD_NO_LIBC_EXCEPTION_SPEC


// function exception specification on operator new
#ifndef _RWSTD_NO_EXCEPTION_SPECIFICATION_ON_NEW
#  define _NEW_THROWS(args)     _THROWS (args)
#else
#  define _NEW_THROWS(ignore)   /* empty */
#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION_ON_NEW


#ifndef _RWSTD_ATTRIBUTE_NORETURN
   // gcc (and others) __attribute__ ((noreturn))
#  define _RWSTD_ATTRIBUTE_NORETURN   /* empty */
#endif   // _RWSTD_ATTR_NORETURN


#ifdef _RWSTD_ATTRIBUTE_NOTHROW
   // gcc (and others) __attribute__ ((nothrow))
#  define _RWSTD_DECLARE_NOTHROW  _RWSTD_ATTRIBUTE_NOTHROW
   // attributes cannot appear on function definitions
#  define _RWSTD_DEFINE_NOTHROW   /* empty */
#else
   // make the macro usable even when it doesn't do anything
#  define _RWSTD_ATTRIBUTE_NOTHROW /* empty */
   // emulate using empty exception specifications which must
   // be specified for both declarations and definitions
#  define _RWSTD_DECLARE_NOTHROW  _THROWS(())
#  define _RWSTD_DEFINE_NOTHROW   _THROWS(())
#endif   // _RWSTD_ATTR_NORETURN


// compile-time assertion - asserts constant expressions during
// compilation with no runtime overhead; failed assertions are reported
// as compilation errors

_RWSTD_NAMESPACE (__rw) { 

// file may be #included from within an extern "C" block
extern "C++" {

template <bool __b>
struct __rw_compile_assert;

_RWSTD_SPECIALIZED_CLASS
struct __rw_compile_assert<true> { enum { _C_ok }; };

#define _RWSTD_COMPILE_ASSERT(const_expr) \
        ((void)_RW::__rw_compile_assert<(const_expr)>::_C_ok)

// called for failed assertions
void _RWSTD_EXPORT
__rw_assert_fail (const char*, const char*, int, const char*)
    _RWSTD_DECLARE_NOTHROW _RWSTD_ATTRIBUTE_NORETURN;



#ifdef __SUNPRO_CC
   // help Sun C++ optimizer generate better code
#  pragma does_not_return (__rw_assert_fail)
#endif   // __SUNPRO_CC

}   // extern "C++"

}   // namespace __rw


// for convenience
#if    !defined (_RWSTD_NO_CLASS_PARTIAL_SPEC)          \
    && (!defined (__SUNPRO_CC) || __SUNPRO_CC > 0x540)
#  define _DISTANCE(first, last, ignore) _STD::distance (first, last)
#else
// working around a SunPro 5.{3,4} bug (see PR #25958)
#  define _DISTANCE(first, last, type) _RW::__rw_distance (first, last, type())
#endif   // _RWSTD_NO_CLASS_PARTIAL_SPEC


// enable only in debug mode and only id partial class specialization
// is supported; prevent multiple definition of _RWSTD_NO_DEBUG_ITER
#if    defined(_RWSTDDEBUG) && !defined (_RWSTD_NO_CLASS_PARTIAL_SPEC) \
    && !defined (_RWSTD_NO_DEBUG_ITER) && !defined (SNI)
#  define _RWSTD_ASSERT_RANGE(first, last) \
      _RWSTD_ASSERT (_RW::__rw_valid_range (first, last))
#  define _RWSTD_ASSERT_IN_RANGE(it, first, last) \
      _RWSTD_ASSERT (_RW::__rw_in_range (it, first, last))
#  define _RWSTD_ASSERT_DEREF(it) \
      _RWSTD_ASSERT (_RW::__rw_dereferenceable (it))
#else
#  ifndef _RWSTD_NO_DEBUG_ITER
#    define  _RWSTD_NO_DEBUG_ITER
#  endif   // _RWSTD_NO_DEBUG_ITER

#  define _RWSTD_ASSERT_RANGE(ign1, ign2)          ((void)0)
#  define _RWSTD_ASSERT_IN_RANGE(ign1, ign2, ign3) ((void)0)
#  define _RWSTD_ASSERT_DEREF(ignore)              ((void)0)
#endif   // _RWSTDDEBUG && _RWSTD_NO_CLASS_PARTIAL_SPEC


// convenience macro to get the "real" iterator embedded in __rw_debug_iter<>
#ifndef _RWSTD_NO_DEBUG_ITER
#  define _ITER_BASE(it)   ((it).base ())
#else   // if defined (_RWSTD_NO_DEBUG_ITER)
#  define _ITER_BASE(it)   (it)
#endif   // _RWSTD_NO_DEBUG_ITER


// will use the deduced native type rather than the typedef to prevent
// conflicts caused by inconsistent definition of wint_t (gcc 2.95.2/AIX)
#ifndef _RWSTD_WINT_T
   // hardcode if wint_t is not available in <wchar.h>
   // used by std::traits and std::basic_[i|o]stream
#  define _RWSTD_WINT_T int
#endif   // _RWSTD_WINT_T


// used by std::traits and std::basic_[i|o]stream,
// required to be int by 21.1.3.1, p2
#define _RWSTD_INT_T int


// ssize_t is defined by POSIX.1
#ifndef _RWSTD_SSIZE_T
#  ifdef _WIN64
#    define _RWSTD_SSIZE_T SSIZE_T
#  else
#    define _RWSTD_SSIZE_T long
#  endif
#endif   // _RWSTD_SSIZE_T

// _RWSTD_UWCHAR_INT_T is "unsigned wchar_t"
#ifndef _RWSTD_UWCHAR_INT_T
#  if _RWSTD_CHAR_SIZE == _RWSTD_WCHAR_SIZE
#    define _RWSTD_UWCHAR_INT_T unsigned char
#  elif _RWSTD_SHRT_SIZE == _RWSTD_WCHAR_SIZE
#    define _RWSTD_UWCHAR_INT_T unsigned short
#  elif _RWSTD_INT_SIZE == _RWSTD_WCHAR_SIZE
#    define _RWSTD_UWCHAR_INT_T unsigned int
#  else
#    define _RWSTD_UWCHAR_INT_T _RWSTD_SIZE_T
#  endif
#endif   // _RWSTD_UWCHAR_INT_T


#if defined(SNI)
#  define _RWSTD_USING_SNI(name) _USING(__SNI::name)
#  if defined(_RWSTD_STRICT_ANSI)
#    define fileno(p)       (int) ((p)->_file)
#  endif
#else
#  define _RWSTD_USING_SNI(ignore)    typedef void __rw_unused_type
#endif


#ifndef _RWSTD_TMPBUF_SIZE
#  define _RWSTD_TMPBUF_SIZE 4096
#endif

#ifndef _RWSTD_NO_STRTOLL
#  define _STRTOLL           strtoll
#else
   // libc has no strtoll, use strtol instead
#  define _STRTOLL           (_RWSTD_LONG_LONG)strtol
#endif   // _RWSTD_NO_STRTOLL


#ifndef _RWSTD_NO_STRTOULL
#  define _STRTOULL          strtoull
#else
   // libc has no strtoull, use strtoul instead
#  define _STRTOULL          (unsigned _RWSTD_LONG_LONG)strtoul
#endif   // _RWSTD_NO_STRTOULL


#ifdef _RWSTD_LIB_SRC
#  define _RWSTD_TI_EXPORT    /* empty */
#else    // #ifndef _RWSTD_LIB_SRC
#  define _RWSTD_TI_EXPORT    _RWSTD_EXPORT
#endif   // _RWSTD_LIB_SRC


#if     defined (_RWSTD_INSTANTIATE_TEMPLATES)       \
    && !defined (_RWSTD_NO_EXPLICIT_INSTANTIATION)   \
    && !defined (_RWSTD_NO_INSTANTIATE)

#  define _RWSTD_INSTANTIATE(name, type)   \
       (   _RWSTD_INSTANTIATE ## name      \
        && _RWSTD_INSTANTIATE ## type      \
        && !_RWSTD_NO ## type)

#  define _RWSTD_INSTANTIATE_1(arg)          template arg
#  define _RWSTD_INSTANTIATE_2(a1, a2)       template a1, a2
#  define _RWSTD_INSTANTIATE_3(a1, a2, a3)   template a1, a2, a3

#  undef  _RWSTD_TI_EXPORT
#  define _RWSTD_TI_EXPORT    _RWSTD_EXPORT

#  ifdef _RWSTD_NO_FUNCTION_EXPLICIT_INSTANTIATION
     // replace explicit function template instantiations with
     // declarations of explicit function template specializations
#    define _RWSTD_INSTANTIATE_FUN_1(ignore)             \
       typedef void __rw_unused_type
#    define _RWSTD_INSTANTIATE_FUN_2(ign1, ign2)         \
       typedef void __rw_unused_type
#    define _RWSTD_INSTANTIATE_FUN_3(ign1, ign2, ign3)   \
       typedef void __rw_unused_type
#  endif   // _RWSTD_NO_FUNCTION_EXPLICIT_INSTANTIATION
#elif     defined (_MSC_VER)                          \
      && !defined (_RWSTD_NO_EXPLICIT_INSTANTIATION)  \
      && !defined (_RWSTD_NO_INSTANTIATE)

#  define _RWSTD_INSTANTIATE(ign1, type)   (!_RWSTD_NO ## type)
#  define _RWSTD_INSTANTIATE_1(arg)        extern template arg
#  define _RWSTD_INSTANTIATE_2(a1, a2)     extern template a1, a2
#  define _RWSTD_INSTANTIATE_3(a1, a2, a3) extern template a1, a2, a3
#elif !defined (_RWSTD_NO_EXTERN_TEMPLATE)
   // use the extern template extension to declare template
   // specializations that are explicitly instantiated in
   // source (.cpp) files
#  define _RWSTD_INSTANTIATE(ign1, type)   (!_RWSTD_NO ## type)
#  define _RWSTD_INSTANTIATE_1(arg)        extern template arg
#  define _RWSTD_INSTANTIATE_2(a1, a2)     extern template a1, a2
#  define _RWSTD_INSTANTIATE_3(a1, a2, a3) extern template a1, a2, a3

#  ifndef _RWSTD_NO_FUNCTION_EXPLICIT_INSTANTIATION
#    ifdef _RWSTD_NO_EXTERN_FUNCTION_TEMPLATE
       // explicit instantiation of function templates enabled
       // but extern template declarations of function template
       // instantiations are disabled, use declarations of
       // explicit function template specializations instead
#      define _RWSTD_INSTANTIATE_FUN_1(arg)          template<> arg
#      define _RWSTD_INSTANTIATE_FUN_2(a1, a2)       template<> a1, a2
#      define _RWSTD_INSTANTIATE_FUN_3(a1, a2, a3)   template<> a1, a2, a3
#    endif   // _RWSTD_NO_EXTERN_FUNCTION_TEMPLATE
#  else   // if defined (_RWSTD_NO_FUNCTION_EXPLICIT_INSTANTIATION)
     // function templates not explicitly instantiated

     // FIXME: this probably won't work since both class template
     // and (non-member) function templates are defined in the same
     // implementation (.c or .cc) files
#    define _RWSTD_INSTANTIATE_FUN_1(ignore)             \
       typedef void __rw_unused_type
#    define _RWSTD_INSTANTIATE_FUN_2(ign1, ign2)         \
       typedef void __rw_unused_type
#    define _RWSTD_INSTANTIATE_FUN_3(ign1, ign2, ign3)   \
       typedef void __rw_unused_type
#  endif   // _RWSTD_NO_FUNCTION_EXPLICIT_INSTANTIATION
#else
#  define _RWSTD_INSTANTIATE(ign1, ign2)         0
#  define _RWSTD_INSTANTIATE_1(ignore)           typedef void __rw_unused_type
#  define _RWSTD_INSTANTIATE_2(ign1, ign2)       typedef void __rw_unused_type
#  define _RWSTD_INSTANTIATE_3(ign1, ign2, ign3) typedef void __rw_unused_type
#endif   // _RWSTD_INSTANTIATE_TEMPLATES

#ifndef _RWSTD_INSTANTIATE_FUN_1
  // default to the same definition as _RWSTD_INSTANTIATE_{1,2,3}
#  define _RWSTD_INSTANTIATE_FUN_1(arg)        _RWSTD_INSTANTIATE_1 (arg)
#  define _RWSTD_INSTANTIATE_FUN_2(a1, a2)     _RWSTD_INSTANTIATE_2 (a1, a2)
#  define _RWSTD_INSTANTIATE_FUN_3(a1, a2, a3) _RWSTD_INSTANTIATE_3 (a1, a2, a3)
#endif   // _RWSTD_INSTANTIATE_FUN_1


#ifndef _RWSTD_NO_STD_UNCAUGHT_EXCEPTION
   // function defined in namespace std in compiler support library 
#  define _UNCAUGHT_EXCEPTION()   _STD::uncaught_exception()
#elif !defined (_RWSTD_NO_GLOBAL_UNCAUGHT_EXCEPTION)
   // function defined in global namespace in compiler support library 
#  define _UNCAUGHT_EXCEPTION()   uncaught_exception()
#else   // if NO_STD_UNCAUGHT_EXCEPTION && NO_GLOBAL_UNCAUGHT_EXCEPTION
   // function not defined in compiler support library 
#  define _UNCAUGHT_EXCEPTION()   true
#endif   // NO_STD_UNCAUGHT_EXCEPTION && NO_GLOBAL_UNCAUGHT_EXCEPTION


#ifdef _RWSTD_NO_EXTERN_TEMPLATE_BEFORE_DEFINITION
#  ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION_BEFORE_DEFINITION
#    define _RWSTD_NO_EXPLICIT_INSTANTIATION_BEFORE_DEFINITION
#  endif   // _RWSTD_NO_EXPLICIT_INSTANTIATION_BEFORE_DEFINITION
#endif   // _RWSTD_NO_EXTERN_TEMPLATE_BEFORE_DEFINITION


// allows for efficient compilation of the library sources without
// support for implicit inclusion: only specializations explicitly
// instantiated in the library are available
#if     defined _RWSTD_NO_IMPLICIT_INCLUSION             \
    && (   !defined (_RWSTD_NO_TEMPLATE_DEFINITIONS)     \
        ||  defined (_RWSTD_NO_EXPLICIT_INSTANTIATION)   \
        ||  defined (_RWSTD_NO_IMPLICIT_INSTANTIATION)   \
        ||  defined (_RWSTD_NO_INSTANTIATE))

#  ifndef _RWSTD_LIB_SRC
     // this block is active when using (as opposed to compiling)
     // the library headers and sources

#    define _RWSTD_DEFINE_TEMPLATE(name)   !_RWSTD_NO ## name ## _DEFINITION
     // when implicit inclusion is not enabled, determines whether extern
     // template declarations for the corresponding explicit instantiation
     // directives for library templates may appear lexically before their
     // complete definitions (i.e., before the .cc file that corresponds
     // to each header) has been #included
#    ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION_BEFORE_DEFINITION
       // note that the _FIRST() and _LAST() macros cannot be #defined
       // in terms of the generic macro above because the name argument
       // would end up getting expanded too early (i.e., before the
       // token concatenation)
#      define _RWSTD_DEFINE_TEMPLATE_FIRST(ignore) 0
#      define _RWSTD_DEFINE_TEMPLATE_LAST(name)    \
              !_RWSTD_NO ## name ## _DEFINITION
#    else
#      define _RWSTD_DEFINE_TEMPLATE_FIRST(name)   \
              !_RWSTD_NO ## name ## _DEFINITION
#      define _RWSTD_DEFINE_TEMPLATE_LAST(ignore)  0
#    endif
#  elif    !defined (_RWSTD_NO_EXPLICIT_INSTANTIATION) \
        && !defined (_RWSTD_NO_IMPLICIT_INSTANTIATION) \
        && !defined (_RWSTD_NO_EXPLICIT_INSTANTIATION_WITH_IMPLICIT_INCLUSION)
     // this block is active when compiling (as opposed to using)
     // the library headers and sources and implicit inclusion
     // is being emulated

#    define _RWSTD_DEFINE_TEMPLATE(name)   _RWSTD_INSTANTIATE ## name
#    ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION_BEFORE_DEFINITION
#      define _RWSTD_DEFINE_TEMPLATE_FIRST(ignore) 0
#      define _RWSTD_DEFINE_TEMPLATE_LAST(name)    _RWSTD_INSTANTIATE ## name
#    else
#      define _RWSTD_DEFINE_TEMPLATE_FIRST(name)  _RWSTD_INSTANTIATE ## name
#      define _RWSTD_DEFINE_TEMPLATE_LAST(ignore) 0
#    endif   // _RWSTD_NO_EXPLICIT_INSTANTIATION_BEFORE_DEFINITION
#  else   // if no explicit instantiation (or with implicit inclusion)
     // this block is active when compiling (as opposed to using)
     // the library headers and sources and implicit inclusion is
     // not being emulated

#    define _RWSTD_DEFINE_TEMPLATE(ignore)         1
#    ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION_BEFORE_DEFINITION
#      define _RWSTD_DEFINE_TEMPLATE_FIRST(ignore)   0
#      define _RWSTD_DEFINE_TEMPLATE_LAST(ignore)    1
#    else   // if defined (_RWSTD_NO_EXPLICIT_INSTANTIATION_BEFORE_DEFINITION)
#      define _RWSTD_DEFINE_TEMPLATE_FIRST(ignore)   1
#      define _RWSTD_DEFINE_TEMPLATE_LAST(ignore)    0
#    endif   // _RWSTD_NO_EXPLICIT_INSTANTIATION_BEFORE_DEFINITION
#  endif   // explicit instantiation (with implicit inclusion)
#else
#  ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION_WITH_IMPLICIT_INCLUSION
     // when implicit inclusion is enabled or being emulated,
     // prevent out-of line non-member template functions or
     // out-of-line member functions of class templates (i.e.,
     // those defined in .c and .cc files) from being #included
     // in translation units
#    define _RWSTD_DEFINE_TEMPLATE(ignore)         0
#    define _RWSTD_DEFINE_TEMPLATE_FIRST(ignore)   0
#    define _RWSTD_DEFINE_TEMPLATE_LAST(ignore)    0
#  else   // if defined (NO_EXPLICIT_INSTANTIATION_WITH_IMPLICIT_INCLUSION)
#    ifndef _RWSTD_LIB_SRC
#      define _RWSTD_DEFINE_TEMPLATE(ignore)         0
#      define _RWSTD_DEFINE_TEMPLATE_FIRST(ignore)   0
#      define _RWSTD_DEFINE_TEMPLATE_LAST(ignore)    0
#    else   // if defined (_RWSTD_LIB_SRC)
#      define _RWSTD_DEFINE_TEMPLATE(name)   _RWSTD_INSTANTIATE ## name
#      ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION_BEFORE_DEFINITION
#        define _RWSTD_DEFINE_TEMPLATE_FIRST(ignore) 0
#        define _RWSTD_DEFINE_TEMPLATE_LAST(name)    _RWSTD_INSTANTIATE ## name
#      else
#        define _RWSTD_DEFINE_TEMPLATE_FIRST(name)  _RWSTD_INSTANTIATE ## name
#        define _RWSTD_DEFINE_TEMPLATE_LAST(ignore) 0
#      endif   // _RWSTD_NO_EXPLICIT_INSTANTIATION_BEFORE_DEFINITION
#    endif   // _RWSTD_LIB_SRC
#  endif   // _RWSTD_NO_EXPLICIT_INSTANTIATION_WITH_IMPLICIT_INCLUSION
#endif


// define if neither wctrans() function declaration exists
// (temporary until a reliable wctrans_t test is put in place)
#if !defined (_RWSTD_NO_WCTRANS_T)
#  if defined (_RWSTD_NO_WCTRANS) && defined (_RWSTD_NO_TOWCTRANS)
#    define _RWSTD_NO_WCTRANS_T
#  endif
#endif   // _RWSTD_NO_WCTRANS_T


#ifndef _RWSTD_NO_SPECIALIZATION_ON_RETURN_TYPE
#  define _RWSTD_DUMMY_ARG(ignore)
#else
#  ifndef _RWSTD_NO_DUMMY_DEFAULT_ARG
     // use a dummy argument with a default value to simulate
     // the effect of a function template whose template argument
     // only appears in its return type (and not in the function
     // argument list)
#    define _RWSTD_DUMMY_ARG(T)   , T
#  else   // if defined (_RWSTD_NO_DUMMY_DEFAULT_ARG)
#    define _RWSTD_DUMMY_ARG(ignore)
#  endif   // _RWSTD_NO_DUMMY_DEFAULT_ARG
#endif   // _RWSTD_NO_SPECIALIZATION_ON_RETURN_TYPE

// helpers to make referring to the iterators easier
#define _RWSTD_ISTREAMBUF_ITER(T)   \
    _STD::istreambuf_iterator<T, _STD::char_traits<T > >

#define _RWSTD_OSTREAMBUF_ITER(T)   \
    _STD::ostreambuf_iterator<T, _STD::char_traits<T > >

// introduce namespace std so that "using namespace std;" always works
_RWSTD_NAMESPACE (std) { 
}   // namespace std


// each standard facet's static id member is explicitly instantiated
// (when extern template is available) or explicitly specialized
// (when member explicit specialization is implemented) for the char
// and wchar_t specializations to guarantee at most one definition
// of each in a program (in case the compiler doesn't provide such
// a guarantee; e.g., MSVC or VisualAge on AIX don't)
#ifdef _RWSTD_NO_EXTERN_TEMPLATE
#  ifdef _RWSTD_NO_EXPLICIT_MEMBER_SPECIALIZATION
#    ifndef _RWSTD_NO_SPECIALIZED_FACET_ID
#      define _RWSTD_NO_SPECIALIZED_FACET_ID
#    endif   // _RWSTD_NO_SPECIALIZED_FACET_ID
#  endif   // _RWSTD_NO_EXPLICIT_MEMBER_SPECIALIZATION
#else   // if !defined (_RWSTD_NO_EXTERN_TEMPLATE)
   // no need to explicitly specialize the members when extern
   // template is available since they are explicitly instantiated
   // in the library
#  ifndef _RWSTD_NO_SPECIALIZED_FACET_ID
#    define _RWSTD_NO_SPECIALIZED_FACET_ID
#  endif   // _RWSTD_NO_SPECIALIZED_FACET_ID
#endif   // _RWSTD_NO_EXTERN_TEMPLATE


// configuration for container class templates and their member
// function templates
#  define _RWSTD_ASSIGN_RANGE(first, last, tag)     \
          _C_assign_range (first, last, tag)
#  define _RWSTD_INSERT_RANGE(it, first, last, tag) \
          _C_insert_range (it, first, last, tag)


#if 2 < __GNUG__
   // use gcc builtin by default
#  define _RWSTD_VA_COPY(va_dst, va_src) \
          __builtin_va_copy (va_dst, va_src)
#elif defined (va_copy) || !defined _RWSTD_NO_VA_COPY
   // either va_copy() is already #defined (because <stdarg.h>
   // is already #included), or it was detected at configuration
#  define _RWSTD_VA_COPY(va_dst, va_src) \
          va_copy (va_dst, va_src)
#elif defined (_RWSTD_NO_VA_LIST_ARRAY)
   // va_list is not an array, use ordinary assignment to copy
#  define _RWSTD_VA_COPY(va_dst, va_src) \
          va_dst = va_src
#else   // if defined (_RWSTD_NO_VA_LIST_ARRAY)
   // va_list is an array, use memcpy()
#  define _RWSTD_VA_COPY(va_dst, va_src) \
          memcpy (va_dst, va_src, sizeof (va_list))
#endif   // _RWSTD_NO_VA_LIST_ARRAY


// disable aligned type traits if compiler support is not
// configured
#if    !defined (_RWSTD_TT_ALIGN_OF)    \
    || !defined (_RWSTD_TT_ALIGNED_POD) \
    || !defined (_RWSTD_TT_MAX_ALIGNMENT)
#  ifndef _RWSTD_NO_ALIGN_TRAITS
#    define _RWSTD_NO_ALIGN_TRAITS
#  endif
#endif

#endif   // _RWSTD_RW_DEFS_H_INCLUDED
