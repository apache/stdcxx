/***************************************************************************
 *
 * limits.cc - definitions of numeric_limits static data members
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

_RWSTD_NAMESPACE (std) { 

#if    !defined (_RWSTD_NO_STATIC_CONST_MEMBER_INIT) \
    && !defined (_RWSTD_DEFINE_EXPORTS)


template <class _TypeT>
const bool numeric_limits<_TypeT>::is_specialized;

template <class _TypeT>
const int numeric_limits<_TypeT>::digits;

template <class _TypeT>
const int numeric_limits<_TypeT>::digits10;

template <class _TypeT>
const bool numeric_limits<_TypeT>::is_signed;

template <class _TypeT>
const bool numeric_limits<_TypeT>::is_integer;

template <class _TypeT>
const bool numeric_limits<_TypeT>::is_exact;

template <class _TypeT>
const int numeric_limits<_TypeT>::radix;

template <class _TypeT>
const int numeric_limits<_TypeT>::min_exponent;

template <class _TypeT>
const int numeric_limits<_TypeT>::min_exponent10;

template <class _TypeT>
const int numeric_limits<_TypeT>::max_exponent;

template <class _TypeT>
const int numeric_limits<_TypeT>::max_exponent10;

template <class _TypeT>
const bool numeric_limits<_TypeT>::has_infinity;

template <class _TypeT>
const bool numeric_limits<_TypeT>::has_quiet_NaN;

template <class _TypeT>
const bool numeric_limits<_TypeT>::has_signaling_NaN;

template <class _TypeT>
const float_denorm_style numeric_limits<_TypeT>::has_denorm;

template <class _TypeT>
const bool numeric_limits<_TypeT>::has_denorm_loss;

template <class _TypeT>
const bool numeric_limits<_TypeT>::is_iec559;

template <class _TypeT>
const bool numeric_limits<_TypeT>::is_bounded;

template <class _TypeT>
const bool numeric_limits<_TypeT>::is_modulo;

template <class _TypeT>
const bool numeric_limits<_TypeT>::traps;

template <class _TypeT>
const bool numeric_limits<_TypeT>::tinyness_before;

template <class _TypeT>
const float_round_style numeric_limits<_TypeT>::round_style;

#endif   // !_RWSTD_NO_STATIC_CONST_MEMBER_INIT && !_RWSTD_DEFINE_EXPORTS


}   // namespace std
