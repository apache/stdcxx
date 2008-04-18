/***************************************************************************
 *
 * locale_core.cpp - definitions of core members of class std::locale
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
 * Copyright 2001-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#include <rw/_defs.h>

#ifdef __SUNPRO_CC
   // working around SunPro bug 568
#  include <time.h>
#endif

#include <loc/_facet.h>
#include <loc/_locale.h>

#include "locale_body.h"

#include <stdlib.h>     // for getenv(), size_t
#include <string.h>     // for strchr()
#include <sys/stat.h>   // for stat()


_RWSTD_NAMESPACE (std) { 


// outlined to hide implementation details
locale::locale () _THROWS (())
    : _C_body (_RW::__rw_locale::_C_manage (0, 0))
{
    _RWSTD_ASSERT (0 != _C_body);
}


// outlined to hide implementation details
locale::locale (const locale &rhs) _THROWS (())
    : _C_body (rhs._C_body)
{
    _RWSTD_ATOMIC_PREINCREMENT (_C_body->_C_ref, false);

    _RWSTD_ASSERT (0 != _C_body);
}


locale::~locale () _THROWS (())
{
    _RWSTD_ASSERT (0 != _C_body);

    if (_C_body->_C_is_managed (_RW::__rw_cat_none)) {
        // body is globally managed, let manager dispose of it
        _RW::__rw_locale::_C_manage (_C_body, _C_body->_C_name);
    }
    else {
        // body is not globally managed, delete it
        // only when its ref count drops down to 0
        if (!_RWSTD_ATOMIC_PREDECREMENT (_C_body->_C_ref, false)) 
            delete _C_body;

    }
}


const locale::facet*
locale::_C_get_std_facet (facet::_C_facet_type  type,
                          facet::_C_ctor_t     *ctor) const
{
    _RWSTD_ASSERT (0 != _C_body);

    const size_t inx = size_t (type / 2);

    // for standard facets, the index also identifies the facet's type
    // (including its specialization, but excluding whether its an ordinary
    // or byname facet), and the facet's LC_XXX category

    _RWSTD_ASSERT (0 != _C_body->_C_name);

    // determine whether locale name is simple or combined
    const char *nm   = _C_body->_C_name;
    const char *next = strchr (nm, *_RWSTD_CAT_SEP);

    if (*_RWSTD_CAT_SEP == _RWSTD_PATH_SEP && next) {

#ifndef _RWSTD_LOCALE_ROOT_ENVVAR
#  define _RWSTD_LOCALE_ROOT_ENVVAR "RWSTD_LOCALE_ROOT"
#endif
        _RW::__rw_chararray pathname;

        if (next != nm) {
            // look up the value of the environment variable
            const char *root = getenv (_RWSTD_LOCALE_ROOT_ENVVAR);

            // use the cwd if none given
            if (!root || !*root)
                root = "." _RWSTD_CAT_SEP;

            // compose the absolute pathname
            pathname.append (root);
            if (pathname.data () [pathname.size () - 1] != _RWSTD_PATH_SEP)
                pathname.append (_RWSTD_CAT_SEP, 1);

            pathname.append (nm);
            nm = pathname.data ();
        }

        struct stat sb;
        if (-1 != stat (nm, &sb))
            next = 0;
    }

    nm = _C_body->_C_name;

    _RW::__rw_chararray locname;

    if (next && *next) {

        // compute the facet category from the value of its id
        const int facet_cat = _RW::__rw_get_cat (int (inx + 1));

        // determine the locale name of the given category
        size_t i = 0;
        for (; i != sizeof _RW::__rw_cats / sizeof *_RW::__rw_cats; ++i) {
            if (_RW::__rw_cats [i].cat == facet_cat) {

                const size_t nmlen = size_t (next - nm);

                locname.assign (nm, nmlen);
                locname.append ("", 1);   // append a terminating NUL
                break;
            }
            nm = next + !!*next;
            next = ::strchr (nm, *_RWSTD_CAT_SEP);
            if (!next)
                next = nm + ::strlen (nm);
        }

        nm = locname.data ();
    }

    _RWSTD_ASSERT (0 != nm);

    const bool byname = !_RW::__rw_is_C (nm);

    type = facet::_C_facet_type (type + byname);

    if (   type == 1 + _RW::__rw_facet::_C_money_get
        || type == 1 + _RW::__rw_facet::_C_money_put
        || type == 1 + _RW::__rw_facet::_C_num_get
        || type == 1 + _RW::__rw_facet::_C_num_put
        || type == 1 + _RW::__rw_facet::_C_wmoney_get
        || type == 1 + _RW::__rw_facet::_C_wmoney_put
        || type == 1 + _RW::__rw_facet::_C_wnum_get
        || type == 1 + _RW::__rw_facet::_C_wnum_put) {
        type = facet::_C_facet_type (type - 1);
        nm = 0;
    }

    // add facet for the simple named locale (or retrieve existing)
    typedef _RW::__rw_facet::_C_facet_type FacetType;

    facet* const pfacet =
        _RW::__rw_facet::_C_manage (0, FacetType (type), nm, ctor);

    _RWSTD_ASSERT (0 != pfacet);

#ifndef _RWSTD_REENTRANT

    // verify that initialization happens exactly once per thread
    // i.e., multiple threads may safely assign the same `pfacet'
    // to the same slot
    _RWSTD_ASSERT (!_C_body->_C_std_facets [inx]);

#endif   // _RWSTD_REENTRANT

    _C_body->_C_std_facets [inx] = pfacet;

    return pfacet;
}


}   // namespace std
