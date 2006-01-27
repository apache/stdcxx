// -*- C++ -*-
/***************************************************************************
 *
 * stdint.h - definition of integer types [tr.c99.stdinth]
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright 2006 The Apache Software Foundation or its licensors,
 * as applicable.
 *
 * Copyright 2006 Rogue Wave Software.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 **************************************************************************/

#ifndef _RWSTD_TR1_STDINT_H_INCLUDED
#define _RWSTD_TR1_STDINT_H_INCLUDED


#include <tr1/cstdint>
#include <rw/_defs.h>


#ifdef _RWSTD_INT8_T

// optional exact-width types
_USING (std::tr1::int8_t);
_USING (std::tr1::uint8_t);

#endif   // _RWSTD_INT8_T

_USING (std::tr1::int_least8_t);
_USING (std::tr1::uint_least8_t);
_USING (std::tr1::int_fast8_t);
_USING (std::tr1::uint_fast8_t);


#ifdef _RWSTD_INT16_T

// optional exact-width types
_USING (std::tr1::int16_t);
_USING (std::tr1::uint16_t);

#endif   // _RWSTD_INT16_T

_USING (std::tr1::int_least16_t);
_USING (std::tr1::uint_least16_t);
_USING (std::tr1::int_fast16_t);
_USING (std::tr1::uint_fast16_t);


#ifdef _RWSTD_INT32_T

// optional exact-width types
_USING (std::tr1::int32_t);
_USING (std::tr1::uint32_t);

#endif   // _RWSTD_INT32_T

_USING (std::tr1::int_least32_t);
_USING (std::tr1::uint_least32_t);
_USING (std::tr1::int_fast32_t);
_USING (std::tr1::uint_fast32_t);


#ifdef _RWSTD_INT64_T

// optional exact-width types
_USING (std::tr1::int64_t);
_USING (std::tr1::uint64_t);

#endif   // _RWSTD_INT64_T

_USING (std::tr1::int_least64_t);
_USING (std::tr1::uint_least64_t);
_USING (std::tr1::int_fast64_t);
_USING (std::tr1::uint_fast64_t);

_USING (std::tr1::intmax_t);
_USING (std::tr1::uintmax_t);
_USING (std::tr1::intptr_t);
_USING (std::tr1::uintptr_t);


#endif   // _RWSTD_TR1_STDINT_H_INCLUDED
