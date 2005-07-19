/***************************************************************************
 *
 * locale_global.cpp - definition of std::locale::global()
 *
 * $Id: //stdlib/dev/source/stdlib/locale_global.cpp#5 $
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

#include <locale.h>
#include <string.h>

#include <loc/_facet.h>
#include <loc/_locale.h>

#include "locale_body.h"



_RWSTD_NAMESPACE (_V3_LOCALE) { 

/* static */ locale locale::global (const locale &rhs)
{
    _RWSTD_ASSERT (rhs._C_body);
    _RWSTD_ASSERT (rhs._C_body->_C_name);

    if (!strchr (rhs.name ().c_str (), '*')) {

        // if the global locale has a name, call setlocale()

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


}   // namespace _V3_LOCALE


#endif   // _RWSTD_NO_V3_LOCALE
