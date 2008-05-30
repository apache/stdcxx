// -*- C++ -*-
/***************************************************************************
 *
 * stdint.h - definition of integer types [tr.c99.stdinth]
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
 * Copyright 2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <rw/_defs.h>

#ifndef _RWSTD_NO_DEPRECATED_C_HEADERS

#  ifndef _RWSTD_STDINT_H_INCLUDED
#    define _RWSTD_STDINT_H_INCLUDED

#    include <ansi/cstdint>

#    ifndef _RWSTD_NO_NAMESPACE

#ifdef _RWSTD_INT8_T

// optional exact-width types
_USING (std::int8_t);
_USING (std::uint8_t);

#endif   // _RWSTD_INT8_T

_USING (std::int_least8_t);
_USING (std::uint_least8_t);
_USING (std::int_fast8_t);
_USING (std::uint_fast8_t);


#ifdef _RWSTD_INT16_T

// optional exact-width types
_USING (std::int16_t);
_USING (std::uint16_t);

#endif   // _RWSTD_INT16_T

_USING (std::int_least16_t);
_USING (std::uint_least16_t);
_USING (std::int_fast16_t);
_USING (std::uint_fast16_t);


#ifdef _RWSTD_INT32_T

// optional exact-width types
_USING (std::int32_t);
_USING (std::uint32_t);

#endif   // _RWSTD_INT32_T

_USING (std::int_least32_t);
_USING (std::uint_least32_t);
_USING (std::int_fast32_t);
_USING (std::uint_fast32_t);


#ifdef _RWSTD_INT64_T

// optional exact-width types
_USING (std::int64_t);
_USING (std::uint64_t);

#endif   // _RWSTD_INT64_T

_USING (std::int_least64_t);
_USING (std::uint_least64_t);
_USING (std::int_fast64_t);
_USING (std::uint_fast64_t);

_USING (std::intmax_t);
_USING (std::uintmax_t);
_USING (std::intptr_t);
_USING (std::uintptr_t);

#    endif   // _RWSTD_NO_NAMESPACE
#  endif   // _RWSTD_STDINT_H_INCLUDED
#else
#  include _RWSTD_ANSI_C_STDINT_H
#endif   // _RWSTD_NO_DEPRECATED_C_HEADERS
