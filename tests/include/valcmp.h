/************************************************************************
 *
 * valcmp.h - declarations of the rw_valcmp() family of helper functions
 *
 * $Id$
 *
 ************************************************************************
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

#ifndef RW_VALCMP_H_INCLUDED
#define RW_VALCMP_H_INCLUDED

#include <testdefs.h>


#define CMP_NULTERM   1   /* the first 0 terminates processing */
#define CMP_RETOFF    2   /* return offset of the first mismatch */
#define CMP_NOCASE    4   /* case-insensitive character comparison */


_TEST_EXPORT int
rw_valcmp (const void*, const void*,
           _RWSTD_SIZE_T, _RWSTD_SIZE_T, _RWSTD_SIZE_T, int);

/**
 * Compares the contents of two arrays of objects of integral types,
 * possibly of different sizes, for equality, in a strncmp/memcmp
 * way.
 *
 * @param buf1  Pointer to an array of 0 or more objects of integral type.
 * @param buf2  Pointer to an array of 0 or more objects of integral type.
 * @param nelems  The maximum number of elements to compare.
 * @param flags  Bitmap of flags that determine how the objects are
 *               compared.
 * @return  Returns -1, 0, or +1, depending on whether the first array
 *          is less than, equal to, or greater than the second array.
 */

template <class T, class U>
inline int
rw_valcmp (const T*      buf1,
           const U*      buf2,
           _RWSTD_SIZE_T nelems,
           int           flags = 0)
{
    return rw_valcmp (buf1, buf2, nelems, sizeof (T), sizeof (U), flags);
}


_TEST_EXPORT int
rw_strncmp (const char*, const char*,
            _RWSTD_SIZE_T = _RWSTD_SIZE_MAX, int = CMP_NULTERM);

#ifndef _RWSTD_NO_WCHAR_T

_TEST_EXPORT int
rw_strncmp (const char*, const wchar_t*,
            _RWSTD_SIZE_T = _RWSTD_SIZE_MAX, int = CMP_NULTERM);

_TEST_EXPORT int
rw_strncmp (const wchar_t*, const char*,
            _RWSTD_SIZE_T = _RWSTD_SIZE_MAX, int = CMP_NULTERM);

_TEST_EXPORT int
rw_strncmp (const wchar_t*, const wchar_t*,
            _RWSTD_SIZE_T = _RWSTD_SIZE_MAX, int = CMP_NULTERM);

#endif   // _RWSTD_NO_WCHAR_T

#endif   // RW_VALCMP_H_INCLUDED
