/**************************************************************************
 *
 * version.cpp - library versioning information
 *
 * $Id: //stdlib/dev/source/stdlib/version.cpp#4 $
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


_RWSTD_NAMESPACE (__rw) {

#ifdef _RWSTD_VER_STR

extern const char __rw_ident[] = {
    "@(#) Rogue Wave C++ Standard Library version " _RWSTD_VER_STR
};

#endif   // _RWSTD_VER_STR

}   // namespace __rw


#ifdef __HP_aCC
#  pragma COPYRIGHT      "Quovadx, Inc"
#  pragma COPYRIGHT_DATE "1994-2004"
#  pragma VERSIONID      _RWSTD_VER_STR
#elif defined (__IBMCPP__)
#  pragma comment (copyright, "(C) 1994-2004, Quovadx, Inc")
#endif   // __HP_aCC
