/***************************************************************************
 *
 * wctype.h
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

#include <rw/_defs.h>

#ifndef _RWSTD_NO_DEPRECATED_C_HEADERS
#  ifndef _RWSTD_WCTYPE_H_INCLUDED
#    define _RWSTD_WCTYPE_H_INCLUDED

#    include <cwctype>

#    ifndef _RWSTD_NO_NAMESPACE

using std::wctrans_t;
using std::wctype_t;
using std::wint_t;

using std::iswalnum;
using std::iswalpha;
using std::iswcntrl;
using std::iswctype;
using std::iswdigit;
using std::iswgraph;
using std::iswlower;
using std::iswprint;
using std::iswpunct;
using std::iswspace;
using std::iswupper;
using std::iswxdigit;
using std::towctrans;
using std::towlower;
using std::towupper;
using std::wctrans;
using std::wctype;

#    endif   // _RWSTD_NO_NAMESPACE
#  endif   // _RWSTD_WCTYPE_H_INCLUDED
#else
#  include _RWSTD_ANSI_C_WCTYPE_H
#endif   // _RWSTD_NO_DEPRECATED_C_HEADERS

