/***************************************************************************
 *
 * ctype_bits.cpp - definitions of std::ctype_base constant data members
 *
 * $Id: //stdlib/dev/source/stdlib/ctype_bits.cpp#4 $
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

#include <rw/_defs.h>

#ifndef _RWSTD_NO_V3_LOCALE

#include <loc/_ctype.h>


_RWSTD_NAMESPACE (_V3_LOCALE) {


_RWSTD_DEFINE_STATIC_CONST (const ctype_base::mask ctype_base::space);
_RWSTD_DEFINE_STATIC_CONST (const ctype_base::mask ctype_base::print);
_RWSTD_DEFINE_STATIC_CONST (const ctype_base::mask ctype_base::cntrl);
_RWSTD_DEFINE_STATIC_CONST (const ctype_base::mask ctype_base::upper);
_RWSTD_DEFINE_STATIC_CONST (const ctype_base::mask ctype_base::lower);
_RWSTD_DEFINE_STATIC_CONST (const ctype_base::mask ctype_base::alpha);
_RWSTD_DEFINE_STATIC_CONST (const ctype_base::mask ctype_base::digit);
_RWSTD_DEFINE_STATIC_CONST (const ctype_base::mask ctype_base::punct);
_RWSTD_DEFINE_STATIC_CONST (const ctype_base::mask ctype_base::xdigit);
_RWSTD_DEFINE_STATIC_CONST (const ctype_base::mask ctype_base::alnum);
_RWSTD_DEFINE_STATIC_CONST (const ctype_base::mask ctype_base::graph);

_RWSTD_DEFINE_STATIC_CONST (const _RWSTD_SIZE_T ctype<char>::table_size);

}   // namespace _V3_LOCALE


#endif   // _RWSTD_NO_V3_LOCALE
