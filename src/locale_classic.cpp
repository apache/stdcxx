/***************************************************************************
 *
 * locale_classic.cpp - definition of std::locale::classic()
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#include <new>
#include <rw/_defs.h>

#include <loc/_locale.h>

#include "locale_body.h"


_RWSTD_NAMESPACE (std) { 


/* static */ const locale& locale::classic ()
{
    // classic locale will not be destroyed during program runtime
    static _RW::__rw_locale *pbody = _RW::__rw_locale::_C_manage (0, "C");

    static union {
        void* _C_align;
        char  _C_buf [sizeof (locale)];
    } classic_locale;

    _RWSTD_ASSERT (0 != pbody);

    // multiple initialization by multiple threads is benign
    static locale *pclassic = new (&classic_locale) locale (*pbody);

    return *pclassic;
}


}   // namespace std
