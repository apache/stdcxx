/***************************************************************************
 *
 * strtol.h - declarations of __rw_strtol, __rw_strtoul, and other helpers
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

#include <rw/_defs.h>     


_RWSTD_NAMESPACE (__rw) {


_RWSTD_EXPORT extern const unsigned char
__rw_digit_map[];


// same as strtoul() except that the source sequence must start
// with a sign (either '+' or '-')
unsigned long
__rw_strtoul (const char*, int*, int) _THROWS (());

long
__rw_strtol (const char*, int*, int) _THROWS (());


#ifdef _RWSTD_LONG_LONG

   // using LLONG_SIZE instead of ULLONG_MAX in the preprocessor
   // conditional below to work around a gcc 3.2 bug (PR #28595)
#  if (_RWSTD_LLONG_SIZE <= _RWSTD_LONG_SIZE)

inline unsigned _RWSTD_LONG_LONG
__rw_strtoull (const char *nptr, int *errptr, int base) _THROWS (())
{
    _RWSTD_ASSERT (sizeof (_RWSTD_LONG_LONG) == sizeof (long));

    return __rw_strtoul (nptr, errptr, base);
}


inline _RWSTD_LONG_LONG
__rw_strtoll (const char *nptr, int *errptr, int base) _THROWS (())
{
    _RWSTD_ASSERT (sizeof (_RWSTD_LONG_LONG) == sizeof (long));

    return __rw_strtol (nptr, errptr, base);
}

#  else   // if (_RWSTD_LLONG_SIZE > _RWSTD_LONG_SIZE)

unsigned _RWSTD_LONG_LONG
__rw_strtoull (const char*, int*, int) _THROWS (());

_RWSTD_LONG_LONG
__rw_strtoll (const char*, int*, int) _THROWS (());

#  endif   // _RWSTD_LLONG_SIZE <= _RWSTD_LONG_SIZE
#endif   // _RWSTD_LONG_LONG


#ifdef __SUNPRO_CC

   // tell the Sun C++ optimizer that the functions do not access
   // for reading or writing any part of the program state (either
   // visible at in the caller at the point of the call, or not)
#  pragma no_side_effect (__rw_strtoul)
#  pragma no_side_effect (__rw_strtol)

#  ifdef _RWSTD_LONG_LONG
#    pragma no_side_effect (__rw_strtoull)
#    pragma no_side_effect (__rw_strtoll)
#  endif   // _RWSTD_LONG_LONG

#endif   // Sun C++


}   // namespace __rw
