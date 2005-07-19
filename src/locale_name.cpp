/***************************************************************************
 *
 * locale_name.cpp - definition of std::locale::name()
 *
 * $Id: //stdlib/dev/source/stdlib/locale_name.cpp#6 $
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

#include <new>
#include <string.h>

#include <loc/_facet.h>
#include <loc/_locale.h>

#include "locale_body.h"


_RWSTD_NAMESPACE (_V3_LOCALE) { 

string locale::name () const
{
    // native locale may or may not have a name, depending
    // on the values of the LC_XXX environment variables at
    // the time of the construction of the locale object

    _RWSTD_ASSERT (0 != _C_body);
    _RWSTD_ASSERT (0 != _C_body->_C_name);

    // 22.1.1, p8: unnamed locales return "*"

    // locales that are not managed (i.e., combined locales) are unnamed
    if (!_C_body->_C_is_managed (none))
        return "*";

    if (strchr (_C_body->_C_name, *_RWSTD_CAT_SEP)) {
        // compose a combined locale name out of the names of the
        // individual locales that each category in *this comes from

        string result;

        _RWSTD_SIZE_T i = 0;

        for (const char *catnm = _C_body->_C_name; *catnm; ) {

            const char *next = strchr (catnm, *_RWSTD_CAT_SEP);
            if (!next)
                next = catnm + strlen (catnm);

            _RWSTD_ASSERT (i != _RW::__rw_n_cats);

            if (   result.size ()
                ||   _RW::__rw_facet::_C_opts
                   & _RW::__rw_facet::_C_prepend_cat_sep)
                result += *_RWSTD_CAT_SEP;

            if (_RW::__rw_facet::_C_opts & _RW::__rw_facet::_C_use_cat_names) {
                // prepend category name only if necessary
                result += _RW::__rw_cats [i++].name;
                result += '=';
            }

            result.append (catnm, next - catnm);

            catnm = next + !!*next;
        }

        return result;
    }

    return _C_body->_C_name;
}


}   // namespace _V3_LOCALE


#endif   // _RWSTD_NO_V3_LOCALE

