/***************************************************************************
 *
 * locale_global.cpp - definition of std::locale::global()
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

#include <rw/_defs.h>
#include <rw/_mutex.h>

#include <locale.h>
#include <string.h>

#include <loc/_facet.h>
#include <loc/_locale.h>

#include "locale_body.h"


_RWSTD_NAMESPACE (__rw) {

extern __rw_mutex __rw_setlocale_mutex;

}   // namespace __rw

_RWSTD_NAMESPACE (std) { 

/* static */ locale locale::global (const locale &rhs)
{
    _RWSTD_ASSERT (rhs._C_body);
    _RWSTD_ASSERT (rhs._C_body->_C_name);

    if (!strchr (rhs.name ().c_str (), '*')) {

        // if the global locale has a name, call setlocale()

        _RWSTD_MT_GUARD (_RW::__rw_setlocale_mutex);

        // assumes all locale names (i.e., including those of combined
        // locales) are in a format understandable by setlocale()
        setlocale (_RWSTD_LC_ALL, rhs.name ().c_str ());

        // FIXME:
        // handle unnamed locales (i.e., locales containing non-standard
        // facets)
    }

    // set the global locale to `body', retrieve the previous value
    _RW::__rw_locale* const tmp = _RW::__rw_locale::_C_manage (rhs._C_body, 0);

    _RWSTD_ASSERT (0 != tmp);

    // do not decrement ref count here, done in dtor of the returned temporary
    return locale (*tmp);
}


}   // namespace std
