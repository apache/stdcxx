/**************************************************************************
 *
 * version.cpp - library versioning information
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
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

#define _RWSTD_LIB_SRC
#include <rw/_defs.h>


_RWSTD_NAMESPACE (__rw) {

#ifdef _RWSTD_VER_STR

extern const char __rw_ident[] = {
    // %Z% keyword ("@(#)") retrieved by the POSIX what utility
    "@(#) Apache C++ Standard Library version " _RWSTD_VER_STR
};

#endif   // _RWSTD_VER_STR

}   // namespace __rw


#ifdef __HP_aCC
#  pragma COPYRIGHT      "Rogue Wave Software, Inc."
#  pragma COPYRIGHT_DATE "1994-2008"
#  if 37300 <= __HP_aCC
#    pragma VERSIONID      _RWSTD_VER_STR
#  elif 0x04020000 == _RWSTD_VER
     // work around an HP aCC 3.63 and prior ICE (see STCXXX-98)
#    pragma VERSIONID      "4.2.0"
#  elif 0x04020100 == _RWSTD_VER
#    pragma VERSIONID      "4.2.1"
#  elif 0x04020200 == _RWSTD_VER
#    pragma VERSIONID      "4.2.2"
#  elif 0x04020300 == _RWSTD_VER
#    pragma VERSIONID      "4.2.3"
#  elif 0x04030000 == _RWSTD_VER
#    pragma VERSIONID      "4.3.0"
#  elif 0x04030100 == _RWSTD_VER
#    pragma VERSIONID      "4.3.1"
#  elif 0x04030200 == _RWSTD_VER
#    pragma VERSIONID      "4.3.2"
#  elif 0x05000000 == _RWSTD_VER
#    pragma VERSIONID      "5.0.0"
#  endif   // HP aCC < 3.73
#elif defined (__IBMCPP__)
#  pragma comment (copyright, "(C) 1994-2008, Rogue Wave Software, Inc.")
#endif   // __HP_aCC
