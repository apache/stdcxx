/***************************************************************************
 *
 * locale_eq.cpp - definition of std::locale::operator==()
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

#include <string.h>

#include <loc/_locale.h>

#include "locale_body.h"


_RWSTD_NAMESPACE (std) {


bool locale::operator== (const locale &rhs) const
{
    _RWSTD_ASSERT (_C_body);
    _RWSTD_ASSERT (_C_body->_C_name);

    _RWSTD_ASSERT (rhs._C_body);
    _RWSTD_ASSERT (rhs._C_body->_C_name);

    // same bodies (i.e., *this is a copy of `rhs') which implies that
    // the locale objects must consist entirely of standard facets,
    // each of which must come from the same locale
    if (_C_body == rhs._C_body)
        return true;

    // different bodies may compare equal if they both contain only
    // ordinary or named standard facet objects, the same sets of
    // which having been replaced with user-defined or constructed
    // facet objects
    if (!_C_body->_C_n_usr_facets && !rhs._C_body->_C_n_usr_facets) {

        // in order to compare equal, both bodies must have the same
        // sets of facets (some slots may still be uninitialized) and
        // not have different names
        const bool eql =
           _C_body->_C_std_facet_bits == rhs._C_body->_C_std_facet_bits
        && _C_body->_C_byname_facet_bits == rhs._C_body->_C_byname_facet_bits
        && !memcmp (_C_body->_C_std_facets, rhs._C_body->_C_std_facets,
                    _C_body->_C_n_std_facets * sizeof *_C_body->_C_std_facets)
        && !strcmp (_C_body->_C_name, rhs._C_body->_C_name);

        // at least some standard facets must have been replaced in order
        // for two "equal" locale objects not to share the same body, or
        // the locales' names must be different
        _RWSTD_ASSERT (   eql
                       || _C_body->_C_std_facet_bits        != _C_body->_C_all
                       || rhs._C_body->_C_std_facet_bits    != _C_body->_C_all
                       || _C_body->_C_byname_facet_bits     != _C_body->_C_all
                       || rhs._C_body->_C_byname_facet_bits != _C_body->_C_all
                       || strcmp (_C_body->_C_name, rhs._C_body->_C_name));

        // facet bits of both objects must be the same if the two objects
        // share the exact same facets (otherwise the bits may or may not
        // be the same)
        _RWSTD_ASSERT (   !eql
                       ||    _C_body->_C_std_facet_bits
                          == rhs._C_body->_C_std_facet_bits
                       &&    _C_body->_C_byname_facet_bits
                          == rhs._C_body->_C_byname_facet_bits);

        // the names of both objects must be the same if the two objects
        // share the exact same facets (otherwise the names may or may not
        // be the same
        // e.g., locale (locale ("C"), new F)
        // and   locale (locale ("C"), new G)
        // can both be named "C;C;C;C;C;C" and distinct if F and G are
        // distinct type neither of which defines the static member id,
        // if both are derived from the same standard facet
        _RWSTD_ASSERT (   !eql
                       || !strcmp (_C_body->_C_name, rhs._C_body->_C_name));

        return eql;
    }

    // locales are unequal if *this has a different number of facets than `rhs'
    if (_C_body->_C_n_usr_facets != rhs._C_body->_C_n_usr_facets)
        return false;

    // locales are unequal if their standard facets don't all come
    // from the same named locales
    if (strcmp (_C_body->_C_name, rhs._C_body->_C_name))
        return false;

    // highest valid index
    const size_t maxinx =
        rhs._C_body->_C_n_usr_facets + _RW::__rw_locale::_C_n_std_facets;

    // locales with the same (non-0) number of user-defined
    // facets compare equal iff all the facets are identical
    for (_RWSTD_SIZE_T i = 0; i != _C_body->_C_n_usr_facets; ++i) {

        const _RW::__rw_facet* const pf = _C_body->_C_usr_facets [i];

        _RWSTD_ASSERT (0 != pf);
        _RWSTD_ASSERT (0 != pf->_C_pid);
        _RWSTD_ASSERT (0 != *pf->_C_pid);

        // if the facets at the same index aren't the same but their
        // numeric id's are the locales are not equal; this check short
        // circuits the linear lookup of the facet in rhs done below
        if (   pf != rhs._C_body->_C_usr_facets [i]
            && pf->_C_pid == rhs._C_body->_C_usr_facets [i]->_C_pid)
            return false;

        // find the index of the facet in rhs if it's installed there
        size_t inx = rhs._C_body->_C_get_facet_inx (*pf->_C_pid);
        if (maxinx <= inx || inx < _RW::__rw_locale::_C_n_std_facets)
            return false;

        // the index starts at _C_n_std_facets
        inx -= _RW::__rw_locale::_C_n_std_facets;

        // if the two facets aren't the same objects the locales aren't
        // equal (since they may each behave differently)
        if (pf != rhs._C_body->_C_usr_facets [inx])
            return false;
    }

    return true;
}


}   // namespace std
