/***************************************************************************
 *
 * memchk.cpp - declarations of memory checking helper functions
 *
 * $Id: //stdlib/dev/source/stdlib/util/memchk.h#2 $
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

#ifndef RWSTD_MEMCHK_H_INCLUDED
#define RWSTD_MEMCHK_H_INCLUDED

#include <cstddef>

// checks if the first argument points to a readable region of memory
// of the given size; returns the size on success, -1 otherwise
std::size_t memchk (const void*, std::size_t);

// checks of the argument points to a valid NUL-terminated character
// string; returns the length of the string on success, -1 otherwise
std::size_t strchk (const char*);

#endif   // RWSTD_MEMCHK_H_INCLUDED
