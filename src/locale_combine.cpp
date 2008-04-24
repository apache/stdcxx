/***************************************************************************
 *
 * locale_combine.cpp - definition of std::locale::combine() and related
 *                      member functions
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

#include <locale.h>
#include <stdio.h>
#include <string.h>   // for memxxx() and strxxx() functions

#include <loc/_locale.h>

#include "access.h"
#include "locale_body.h"
#include "setlocale.h"

#if 6 == _RWSTD_HP_aCC_MAJOR && _RWSTD_HP_aCC_MINOR <= 1600
   // silence bogus HP aCC 6.16/cadvise warning #20200-D:
   // Potential null pointer dereference
#  pragma diag_suppress 20200
#endif


_RWSTD_NAMESPACE (__rw) {


// constructs a new locale name from two locale names, combining
// all but the category given by `cat' from `name_1' and the rest
// from `name_2'
static void
__rw_combine_names (__rw_chararray &locname,
                    const char *name_1, const char *name_2, int cat)
{
    _RWSTD_ASSERT (0 != name_1);
    _RWSTD_ASSERT (0 != name_2);

    // `cat' must be a locale::category (and not a LC_XXX value)
    _RWSTD_ASSERT (__rw_locale::_C_check_category (cat));

    for (_RWSTD_SIZE_T i = 0; i != __rw_n_cats; ++i) {
        const char *nxt1 = strchr (name_1, *_RWSTD_CAT_SEP);
        const char *nxt2 = strchr (name_2, *_RWSTD_CAT_SEP);

        if (!nxt1 || !*nxt1)
            nxt1 = name_1 + strlen (name_1);

        if (!nxt2 || !*nxt2)
            nxt2 = name_2 + strlen (name_2);

        // convert a LC_XXX constant to a locale::category value
        const int c = __rw_locale::_C_LC2category (__rw_cats [i].cat);

        const char    *from;   // copy from where
        _RWSTD_SIZE_T  len;    // this many chars

        if (cat & c) {   // category comes from `other'
            from = name_2;
            len  = nxt2 - name_2;
        }
        else {   // category comes from `one'
            from = name_1;
            len  = nxt1 - name_1;
        }

        if (locname.size ())
            locname.append (_RWSTD_CAT_SEP, 1);

        locname.append (from, len);

        // advance to the next category name in each name
        if (*nxt1)
            name_1 = nxt1 + 1;

        if (*nxt2)
            name_2 = nxt2 + 1;
    }
}


void __rw_locale::
_C_construct (const __rw_locale &rhs, const __rw_facet *pfacet)
{
    // called from ctor only, members are not initialized
    _C_ref = 1;

    // copy standard facets from other locale
#if defined (__i386__) || !defined (__GNUG__) || __GNUG__ < 3

    memcpy (_C_std_facets, rhs._C_std_facets,
            _C_n_std_facets * sizeof *_C_std_facets);

#else   // !i86  || !gcc 3.x

    // Working around a bug (most probably) identical to the one described 
    // and fixed in __rw_locale::__rw_locale, but related to the use of
    // memcpy.

    for(_RWSTD_SIZE_T i = 0; i != _C_n_std_facets; i++)
        _C_std_facets [i] = rhs._C_std_facets [i];

#endif   // i86/gcc 3.x

    _RWSTD_ASSERT (!pfacet || pfacet->_C_pid);

    // get facet's index into one of the facet arrays
    _RWSTD_SIZE_T inx = pfacet
        ? rhs._C_get_facet_inx (*pfacet->_C_pid) : (_RWSTD_SIZE_T)(-1);

    // extend size if facet isn't installed in `rhs'
    _C_n_usr_facets =   rhs._C_n_usr_facets
                      + (inx > rhs._C_n_usr_facets + _C_n_std_facets);

    // allocate a buffer for user facets if necesary
    _C_usr_facets = _C_n_usr_facets ? new __rw_facet* [_C_n_usr_facets] : 0;

    // copy facets from other locale
    memcpy (_C_usr_facets, rhs._C_usr_facets,
            rhs._C_n_usr_facets * sizeof *_C_usr_facets);

    _RWSTD_ASSERT (rhs._C_name);

    _C_name = 0;

    // copy facet bits from `rhs' and adjust below as necessary
    _C_std_facet_bits    = rhs._C_std_facet_bits;
    _C_byname_facet_bits = rhs._C_byname_facet_bits;

    if (pfacet) {
        __rw_facet* const pf = _RWSTD_CONST_CAST (__rw_facet*, pfacet);
        // overwrite an existing pointer or add a new one
        if (inx < _C_n_std_facets) {
            _C_std_facets [inx] = pf;

            const _RWSTD_SIZE_T facet_bit = 1UL << inx;

            // set or clear a corresponding bit based on whether
            // the facet is one of the standard facets or not
            // (user-defined objects even if they are of standard
            // facet types, are not considered standard facets)
            if (pf->_C_type > 0)
                _C_std_facet_bits |= facet_bit;
            else
                _C_std_facet_bits &= ~facet_bit;

            // ditto for named facet bitmap
            if (pf->_C_type & 1)   // even types are _byname
                _C_byname_facet_bits &= ~facet_bit;
            else
                _C_byname_facet_bits |= facet_bit;

            // construct a new name
            // _C_name =
            //     _C_make_name (_C_namebuf, sizeof _C_namebuf, rhs, *pfacet);
        }
        else if ((_RWSTD_SIZE_T)(-1) == inx)
            _C_usr_facets [_C_n_usr_facets - 1] = pf;
        else
            _C_usr_facets [inx - _C_n_std_facets] = pf;
    }

    if (!_C_name) {
        // simply copy name from `rhs'
        const _RWSTD_SIZE_T size = strlen (rhs._C_name) + 1;
        char* const name = size <= sizeof _C_namebuf
            ? _C_namebuf : new char [size];

        memcpy (name, rhs._C_name, size);
        _C_name = name;
    }

    // increment reference counts for facets (including the newly added one)
    for (inx = 0; inx != _C_n_std_facets; ++inx) {
        if (_C_std_facets [inx])
            _RWSTD_ATOMIC_PREINCREMENT (_C_std_facets [inx]->_C_ref,
                                        _C_std_facets [inx]->_C_mutex);
    }
    
    for (inx = 0; inx != _C_n_usr_facets; ++inx) {
        _RWSTD_ATOMIC_PREINCREMENT (_C_usr_facets [inx]->_C_ref,
                                    _C_usr_facets [inx]->_C_mutex);
    }
}


void __rw_locale::
_C_construct (const __rw_locale &one, const __rw_locale &other, int cat)
{
    // category must be valid at this point (checked by caller)
    _RWSTD_ASSERT (_C_check_category (cat));

    // called from ctor only, members are not initialized
    _C_ref = 1;

    // size is the same as that of `one'
    _C_n_usr_facets = one._C_n_usr_facets;

    // allocate only if internal buffer is insufficient
    _C_usr_facets = _C_n_usr_facets ?  new __rw_facet* [_C_n_usr_facets] : 0;

    // copy standard facets to `one' from `other'
    memcpy (_C_std_facets, one._C_std_facets,
            _C_n_std_facets * sizeof *_C_std_facets);

    // copy user-defined facets (if any)
    memcpy (_C_usr_facets, one._C_usr_facets,
            _C_n_usr_facets * sizeof *_C_usr_facets);

    // compute and assign facet bitmaps
    const int bits = _C_LC2facet_bits (cat);

    _C_std_facet_bits    =   one._C_std_facet_bits & ~bits
                           | other._C_std_facet_bits & bits;

    _C_byname_facet_bits =   one._C_byname_facet_bits & ~bits
                           | other._C_byname_facet_bits & bits;

    for (_RWSTD_SIZE_T i = 0; i != _C_n_std_facets; ++i) {

        // each facet is stored at an index equal to its (id - 1)
        // convert an LC_XXX constant to a locale::category value
        const int c = _C_LC2category (__rw_get_cat (int (i) + 1));

        if (cat & c) {
            // assign/overwrite corresponding facets
            _C_std_facets [i] = other._C_std_facets [i];
        }

        if (_C_std_facets [i]) {
            // bump up the facet's reference count of *this' facet
            _RWSTD_ATOMIC_PREINCREMENT (_C_std_facets [i]->_C_ref,
                                        _C_std_facets [i]->_C_mutex);
        }
    }

    // increment the ref count of user-defined facets (if any)
    for (_RWSTD_SIZE_T j = 0; j != _C_n_usr_facets; ++j) {

        _RWSTD_ASSERT (_C_usr_facets [j]);

        _RWSTD_ATOMIC_PREINCREMENT (_C_usr_facets [j]->_C_ref,
                                    _C_usr_facets [j]->_C_mutex);
    }

    __rw_chararray locname;

    __rw_combine_names (locname, one._C_name, other._C_name, cat);

    if (locname.size () >= one._C_namebuf_size)
        _C_name = locname.release ();
    else {
        _C_name = _C_namebuf;
        memcpy (_C_namebuf, locname.data (), locname.size () + 1);
    }
}


_RW::__rw_locale* __rw_locale::
_C_combine (const __rw_facet *pfacet) const
{
    _RWSTD_ASSERT (pfacet);

    // template ctor: locale::locale (const locale&, Facet*)
    // with `facet' being installed (e.g., adding or replacing
    // an existing one)

    // verify that Facet::id has been initialized
    _RWSTD_ASSERT (pfacet->_C_pid);
    _RWSTD_ASSERT (*pfacet->_C_pid);

    // get the locale name ("C" if it's NUL)
    const char* const one_locname = _C_name ? _C_name : "C";

    // get the name of the locale `*pfacet' belongs to
    const char* const facet_locname =
        pfacet->_C_name ? pfacet->_C_name : "C";

    // get the locale::category that `*pfacet' belongs to
    const int facet_cat =
        _C_LC2category (__rw_get_cat (int (*pfacet->_C_pid)));

    bool managed = false;

    if (pfacet->_C_type && !_C_n_usr_facets) {

        // facet is one of the (perhaps byname) standard facets

        // compute the number of the bit in _C_std_facet_bits or
        //_C_byname_facet_bits corresponding to the facet being
        // used (i.e., `*pfacet')
        const _RWSTD_SIZE_T facet_bit =
            (_RWSTD_SIZE_T)1U << (*pfacet->_C_pid - 1U);

        // a locale object is globally managed if all its facets are objects
        // of standard facet types (ordinary or byname) that were constructed
        // by the library and within each locale category all facets belong
        // to the same facet
        // managed locales have names that completely describe their facets
        // and from which they can be unambiguously and fully constructed
        //
        // note that in the code snippet below, x0 is not a managed locale
        // (its name is "*"), but y0 is a managed locale (on SunOS, its name
        // is "/de_DE/de_DE/de_DE/de_DE/de_DE/en_US"); x1 is the same as x0
        // and y1 is the same as de:
        //
        //   locale de ("de_DE");
        //   locale en ("en_US");
        //   locale x0 = de.combine<messages<char> >(en);
        //   locale y0 = x0.combine<messages<wchar_t> >(en);
        //   locale x1 = y0.combine<messages<wchar_t> >(de);
        //   locale y1 = x1.combine<messages<char> >(de);
        //   assert (de == y1);
        //   assert (x0 == x1);

        if (__rw_locale::_C_all == (_C_std_facet_bits | facet_bit)) {

            // compute the bitmask corresponding to all the facets in
            // the same category as the one to which `*pfacet' belongs
            const int cat_bits = _C_LC2facet_bits (facet_cat);

            // get the index into the _C_std_facets array for `pfacet'
            const _RWSTD_SIZE_T facet_inx =
                _C_get_facet_inx (*pfacet->_C_pid);

            managed = true;

            // iterate over all facets that belong the same category
            // as `*pfacet' and compare the name of each with the name
            // of `*pfacet' to determine if they all have the same name
            // _byname facets that are not installed yet (i.e., whose
            // pointers are 0 come from a locale whose name is given
            // by the name of the whole locale
            for (_RWSTD_SIZE_T i = 0; i != _C_n_std_facets; ++i) {

                // skip facets from other categories
                if (!((1 << i) & cat_bits))
                    continue;

                // skip the facet being replaced
                if (facet_inx == i)
                    continue;

                const char* const ones_facet_locname =
                      _C_std_facets [i] ? _C_std_facets [i]->_C_name
                    ? _C_std_facets [i]->_C_name : "C" : one_locname;

                if (strcmp (facet_locname, ones_facet_locname)) {
                    managed = false;
                    break;
                }
            }
        }
    }

    __rw_locale *plocale;

    if (managed) {

        __rw_chararray locname;
        __rw_combine_names (locname, one_locname, facet_locname, facet_cat);

        plocale = __rw_locale::_C_manage (0, locname.data ());

        _RWSTD_ASSERT (0 != plocale);
        _RWSTD_ASSERT (plocale->_C_is_managed (_STD::locale::none));
    }
    else {
        plocale = new __rw_locale (*this, pfacet);
        
        _RWSTD_ASSERT (0 != plocale);
        _RWSTD_ASSERT (!plocale->_C_is_managed (_STD::locale::none));
    }

    return plocale;
}


__rw_locale* __rw_locale::
_C_combine (const __rw_locale &other, int cat)
{
    // convert a possible LC_XXX constant to a locale::category value
    cat = _C_LC2category (cat);

    // verify that the category argument is valid
    if (!_C_check_category (cat))
        return 0;


    // combining ctor: locale (const locale&, const locale&, cat)

    if (this == &other || _STD::locale::none == cat) {
        // same as copy ctor (copying all facets from `*this')
        // body may or may not be globally managed
        _RWSTD_ATOMIC_PREINCREMENT (_C_ref, false);

        return this;
    }

#if 0   // disabled

    // 22.1.1.2, p14: only facets that implement categories in `cat' are
    // to be copied from `other', i.e., not necessarily user-defined facets

    if (_RWSTD_LC_ALL == cat || _STD::locale::all == cat) {
        // same as copy ctor (copying all facets from `other')
        // body may or may not be globally managed
        _RWSTD_ATOMIC_PREINCREMENT (other._C_ref, false);
        return &other;
    }

#endif   // 0/1

    if (   _C_is_managed (_STD::locale::all & ~cat)
        && other._C_is_managed (cat)) {
        
        // determine new locale name from `*this' and `other's, taking all of
        // this's category names except for those given by `cat', which come
        // from `other'

        const char *name_1 = _C_name;
        const char *name_2 = other._C_name;

        if (0 == strcmp (name_1, name_2)) {
            // if locale names are the same, return a globally managed body
            __rw_locale* const plocale = __rw_locale::_C_manage (0, name_1);

            _RWSTD_ASSERT (0 != plocale);
            _RWSTD_ASSERT (plocale->_C_is_managed (_STD::locale::none));

            return plocale;
        }

        __rw_chararray locname;

        // compose the new name
        __rw_combine_names (locname, name_1, name_2, cat);

        // return a globally managed body
        __rw_locale* const plocale =
            __rw_locale::_C_manage (0, locname.data ());

        _RWSTD_ASSERT (0 != plocale);
        _RWSTD_ASSERT (plocale->_C_is_managed (_STD::locale::none));

        return plocale;
    }

    // one or both locales are not globally managed (one or more of
    // their facets are of use-defined types, possibly derived from
    // the standard facets), and thus the combined locale cannot be
    // globally managed either
    __rw_locale *plocale = new __rw_locale (*this, other, cat);

    _RWSTD_ASSERT (!plocale->_C_is_managed (_STD::locale::none));

    return plocale;
}


/* static */ __rw_locale* __rw_locale::
_C_get_body (__rw_locale      *one,
             __rw_locale      *other,
             const char       *locname,
             int               cat,
             const __rw_facet *pfacet)
{
    __rw_chararray realname;

    // see if `locname' contains embedded "LC_XXX=..." sequences
    if (locname && !strncmp (locname, "LC_", 3)) {

        // convert a combined locale name (consisting of a sequence
        // of `sep'-separated LC_XXX=<name> substrings) to a normalized
        // form in the expected (internal) format and order

        const char *pcatnames [__rw_n_cats] = { 0 };

        // try the libc "native" separator first, semicolon next
        const char *sep = strchr (locname, *_RWSTD_CAT_SEP);
        if (!sep)
            sep = ";";

        for (const char *s = locname; *s; ) {

            const char *next = strchr (s, *sep);
            if (!next)
                next = s + strlen (s);

            const char* const endcat = strchr (s, '=');
            if (!endcat)
                return 0;   // error: missing `=' after LC_XXX

            // go through the LC_XXX category names, trying to find
            // a match; if found, remember its position (duplicates
            // are not allowed)

            for (_RWSTD_SIZE_T i = 0; i != __rw_n_cats; ++i) {
                if (!strncmp (__rw_cats [i].name, s, endcat - s)) {
                    if (pcatnames [i])
                        return 0;    // error: duplicate LC_XXX

                    pcatnames [i] = endcat + 1;
                    break;
                }
            }

            s = *next ? next + 1 : next;
        }

        // compose a normalized locale name out of category names
        _RWSTD_SIZE_T size = 0;
        for (_RWSTD_SIZE_T i = 0; i != __rw_n_cats; ++i) {

            if (!pcatnames [i]) {
                // use "C" for missing LC_XXX values
                pcatnames [i] = "C";
            }

            const char *endcat = strchr (pcatnames [i], *sep);

            if (!endcat)
                endcat = pcatnames [i] + strlen (pcatnames [i]);

            const _RWSTD_SIZE_T catsize = endcat - pcatnames [i];

            // append name followed by the libc "native" separator
            realname.append (pcatnames [i], catsize);
            realname.append (_RWSTD_CAT_SEP, 1);

            size += catsize;
        }

        locname = realname.data ();
    }
    else if (locname && *locname == *_RWSTD_CAT_SEP) {
        // advance past the leading category separator if present
        ++locname;
    }

    if (!one && !other && !locname && _STD::locale::none == cat && !pfacet) {
        // implements default ctor: locale::locale () throw ()
        __rw_locale* const global = __rw_locale::_C_manage (0, 0);

        _RWSTD_ASSERT (0 != global);

        return global;
    }

    if (one && !other && !locname && _STD::locale::none == cat && !pfacet) {
        // copy ctor: locale::locale (const locale&)
        // or combining ctor: locale::locale (const locale&, Facet*)
        // (in the second case with Facet* being 0)
        // body may or may not be globally managed
        // or locale::operator=(const locale&)
        _RWSTD_ATOMIC_PREINCREMENT (one->_C_ref, false);
        return one;
    }

    if (!one && !other && locname && _STD::locale::none == cat && !pfacet) {
        // ctor: locale::locale (const char*)
        // if successful, body is globally managed
         __rw_locale* const plocale = __rw_locale::_C_manage (0, locname);

         _RWSTD_ASSERT (!plocale || plocale->_C_is_managed (_STD::locale::none));

        return plocale;
    }

    if (one && !other && !locname && _STD::locale::none == cat && pfacet) {
        // template ctor: locale::locale (const locale&, Facet*)
        // with `pfacet' being installed (e.g., adding or replacing
        // an existing one)
        // template <class Facet> locale::combine(const locale&) is
        // trivially implemented inline in terms of the template ctor
        __rw_locale* const plocale = one->_C_combine (pfacet);

        _RWSTD_ASSERT (0 != plocale);

        return plocale;
    }

    if (one && !other && locname && !pfacet) {
        // combining ctor: locale (const locale&, const char*, category)

        // construct a temporary locale from locale name `locname'
        const _STD::locale tmp (locname);

        // construct a locale combined from `one' and the temporary
        __rw_locale* const plocale = one->_C_combine (*tmp._C_body, cat);

        return plocale;
    }

    if (one && other && !locname && !pfacet) {
        // combining ctor: locale (const locale&, const locale&, category)
        return one->_C_combine (*other, cat);
    }

    _RWSTD_ASSERT (!"logic error: bad combination of arguments");

    return 0;
}

}   // namespace __rw


_RWSTD_NAMESPACE (std) {


// outlined to hide implementation details
locale::
locale (const char *locname)
{
    // verify 22.1.1.2, p7: `locname' must not be null
    _RWSTD_REQUIRES (locname,
                     (_RWSTD_ERROR_LOCALE_BAD_NAME,
                      _RWSTD_FUNC ("locale::locale (const char*)"), "(null)"));

    _C_body = _RW::__rw_locale::_C_get_body (0, 0, locname, none, 0);

    // verify 22.1.1.2, p7: `locname' must be valid
    _RWSTD_REQUIRES (_C_body,
                     (_RWSTD_ERROR_LOCALE_BAD_NAME,
                      _RWSTD_FUNC ("locale::locale (const char*)"), locname));
}


// outlined to hide implementation details
locale::
locale (const locale &rhs, const char *locname, category cat)
{
    // verify 22.1.1.2, p10: `locname' must not ne null
    _RWSTD_REQUIRES (locname,
                     (_RWSTD_ERROR_LOCALE_BAD_NAME,
                      _RWSTD_FUNC ("locale::locale (const locale&, "
                                   "const char*, category)"), "(null"));

    _C_body = _RW::__rw_locale::_C_get_body (rhs._C_body, 0, locname, cat, 0);

    // verify 22.1.1.2, p10: `locname' must be valid
    _RWSTD_REQUIRES (_C_body,
                     (_RWSTD_ERROR_LOCALE_BAD_NAME,
                      _RWSTD_FUNC ("locale::locale (const locale&, "
                                   "const char*, category)"), locname));
}


// outlined to hide implementation details
locale::
locale (const locale &one, const locale &other, category cat)
    : _C_body (_RW::__rw_locale::_C_get_body (one._C_body, other._C_body,
                                              0, cat, 0))
{
    // verify 22.1.1.2, p14 (missing requirement): category must be valid
    _RWSTD_REQUIRES (_C_body,
                     (_RWSTD_ERROR_LOCALE_BAD_CAT,
                      _RWSTD_FUNC ("locale::locale (const locale&, "
                                   "const locale&, category)"), cat));
}


// outlined to hide implementation details
/* private */ locale::
locale (_RW::__rw_locale &rhs, const facet *pfacet)
    : _C_body (_RW::__rw_locale::_C_get_body (&rhs, 0, 0, none, pfacet))
{
    // empty
}


const locale::facet* locale::
_C_get_facet (const id &fid) const
{
    // initialize id if not yet initialized
    if (!fid._C_id)
        fid._C_init ();

    // properly initialized facet id's are non-zero
    _RWSTD_ASSERT (0 != fid._C_id);

    _RWSTD_ASSERT (0 != _C_body);

    // find the index at which the facet is stored in one of the facet arrays
    const _RWSTD_SIZE_T inx = _C_body->_C_get_facet_inx (fid._C_id);
    if ((_RWSTD_SIZE_T)(-1) == inx)
        return 0;

    const locale::facet* const pfacet = inx < _RW::__rw_locale::_C_n_std_facets
        ? _C_body->_C_std_facets [inx]
        : _C_body->_C_usr_facets [inx - _RW::__rw_locale::_C_n_std_facets];

    if (pfacet) {
        // facet id must have been initialized to point
        // to the numeric member of the static Facet::id
        _RWSTD_ASSERT (pfacet->_C_pid);

        // make sure the static Facet::id matches its (non-static) id member
        _RWSTD_ASSERT (*pfacet->_C_pid == fid._C_id);
    }

    return pfacet;
}


}   // namespace std
