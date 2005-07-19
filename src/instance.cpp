/***************************************************************************
 *
 * instance.cpp - Instantiations of the C++ Standard Library templates.
 *
 * $Id: //stdlib/dev/source/stdlib/instance.cpp#50 $
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

#define _RWSTD_LIB_SRC

#define _RWSTD_INSTANTIATE_TEMPLATES      1
#define _RWSTD_INSTANTIATE_STATIC_MUTEX   1
#define _RWSTD_INSTANTIATE_INT            1

#include <string>
#include <rw/_mutex.h>

#ifdef _RWSTD_NO_COLLAPSE_TEMPLATE_STATICS

_RWSTD_NAMESPACE (__rw) {

typedef __string_ref<char, _STD::char_traits<char>, _STD::allocator<char> >
    StringRef;

// this is wrong -- it won't work at all for other (non-char) specializations
_RWSTD_EXPORT unsigned long __nullref [sizeof (StringRef) / 4 + 2];

}   // namespace __rw

#endif   // _RWSTD_NO_COLLAPSE_TEMPLATE_STATICS
