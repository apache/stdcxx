/***************************************************************************
 *
 * _numpunct.cc - definition of std::moneypunct members
 *
 * $Id$
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


_RWSTD_NAMESPACE (_V3_LOCALE) {


// #ifndef _RWSTD_NO_EXT_NUMPUNCT_PRIMARY

template <class _CharT>
_RW::__rw_facet_id numpunct<_CharT>::id;


template <class _CharT>
/* virtual */ numpunct<_CharT>::~numpunct ()
{
    // no-op
}

// #endif   // _RWSTD_NO_EXT_NUMPUNCT_PRIMARY


}   // namespace _V3_LOCALE
