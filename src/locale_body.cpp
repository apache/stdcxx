/***************************************************************************
 *
 * locale_body.cpp - implementation of the std::locale and __rw_locale class
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
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
 * Copyright 2001-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#include <rw/_defs.h>

#include <new>        // for placement new
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>   // for bsearch(), qsort()
#include <string.h>   // for memxxx() and strxxx() functions

#include <loc/_locale.h>

#include "access.h"
#include "locale_body.h"
#include "setlocale.h"


#ifdef _RWSTD_NO_LOCALE_NAME_FMAT

// pick an arbitrary order for categories in locale names
#  define _RWSTD_CAT_0(pfx) \
       { _RWSTD_LC_CTYPE, "LC_CTYPE", pfx::_C_ctype }
#  define _RWSTD_CAT_1(pfx) \
       { _RWSTD_LC_NUMERIC, "LC_NUMERIC", pfx::_C_numeric }
#  define _RWSTD_CAT_2(pfx) \
       { _RWSTD_LC_TIME, "LC_TIME", pfx::_C_time }
#  define _RWSTD_CAT_3(pfx) \
       { _RWSTD_LC_COLLATE, "LC_COLLATE", pfx::_C_collate }
#  define _RWSTD_CAT_4(pfx) \
       { _RWSTD_LC_MONETARY, "LC_MONETARY", pfx::_C_monetary }
#  define _RWSTD_CAT_5(pfx) \
       { _RWSTD_LC_MESSAGES, "LC_MESSAGES", pfx::_C_messages }

#endif   // _RWSTD_NO_LOCALE_NAME_FMAT


#if 6 == _RWSTD_HP_aCC_MAJOR && _RWSTD_HP_aCC_MINOR <= 1600
   // silence bogus HP aCC 6.16/cadvise warning #20200-D:
   // Potential null pointer dereference
#  pragma diag_suppress 20200
#endif


_RWSTD_NAMESPACE (__rw) {


// maps LC_XXX category values to their names
extern const __rw_cats_t
__rw_cats [] = {

    // expands to { LC_XXX, "LC_XXX", _RW::__rw_locale::_C_xxx }
    // the order of these is determined at config time and corresponds to
    // the order mixed/combined locale categories returned by setlocale()
    _RWSTD_CAT_0 (_RW::__rw_locale),
    _RWSTD_CAT_1 (_RW::__rw_locale),
    _RWSTD_CAT_2 (_RW::__rw_locale),
    _RWSTD_CAT_3 (_RW::__rw_locale),
    _RWSTD_CAT_4 (_RW::__rw_locale),

#ifdef _RWSTD_CAT_5
    _RWSTD_CAT_5 (_RW::__rw_locale)
#else   // if !defined (_RWSTD_CAT_5)
    // for Windoze (no LC_MESSAGES there)
    _RWSTD_CAT_0 (_RW::__rw_locale)
#endif   // _RWSTD_CAT_5

};


// fills provided buffer (if large anough) with the names of locales
// for each of the standard LC_XXX categories; if all categories come
// from the same locale, copies the name of the common locale into
// the provided buffer
//
// names of categories are in the following order:
//     LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME, LC_MESSAGES
//
// examples of output (assuming ';' as a separator):
//     "C"
//         all facets of all categories come from the "C" locale
//
//     "C;C;de_DE;C;C;C;"
//         all facets come from the "C" locale except those that belong
//         to the LC_MONETARY category, which all come from "de_DE"
// 
//     "C;en_US;*;*;*;*;"
//         facets the belong to the LC_COLLATE category come from the
//         "C" locale, those belonging to the LC_CTYPE category come
//         from "en_US", all others are from mixed locales within
//         their respective categories
//
//     "*"
//         facets belonging to each category come from all different
//         locales (e.g., ctype<char> might come from the "C" locale,
//         and ctype<wchar_t> from "ja_JP", etc.)

char* __rw_locale::
_C_get_cat_names (char *buf, size_t bufsize) const
{
    const size_t  savesize = bufsize;
    char         *savebuf  = buf;

    if (!buf)
        buf = new char [bufsize ? bufsize : (bufsize = 256)];

    *buf = '\0';

    // will point at the locale name common across all categories
    // if one such name exists; otherwise at "*" (mixed locale)
    const char *common_name = 0;

    // iterate over all categories, searching for all facets that belong
    // to that category and comparing their names. if all such facets come
    // from the same locale, the name of the category is the name of the
    // locale; otherwise "*"
    for (size_t i = 0; i != __rw_n_cats; ++i) {

        const char *cat_name = 0;

        for (size_t j = 0; j != _C_n_std_facets; ++j) {

            // verify that all facet pointers are set
            _RWSTD_ASSERT (_C_std_facets [j]);

            // ignore the non-localized {money,num}_{get,put} facets
            // and consider only moneypunct and numpunct for the
            // monetary category
            switch (_C_std_facets [j]->_C_type) {
            case __rw_facet::_C_num_get:
            case __rw_facet::_C_wnum_get:
            case __rw_facet::_C_num_put:
            case __rw_facet::_C_wnum_put:
            case __rw_facet::_C_money_get:
            case __rw_facet::_C_wmoney_get:
            case __rw_facet::_C_money_put:
            case __rw_facet::_C_wmoney_put:
                continue;
            default:
                break;
            }

            _RWSTD_ASSERT (0 != _C_std_facets [j]->_C_pid);

            // one category at a time
            if (   __rw_cats [i].cat
                != __rw_get_cat (int (*_C_std_facets [j]->_C_pid)))
                continue;

            // default facet name (if 0) is "C"
            const char *facet_name = _C_std_facets [j]->_C_name
                ? _C_std_facets [j]->_C_name : "C";

            if (cat_name && strcmp (cat_name, facet_name))
                cat_name = "*";
            else
                cat_name = facet_name;

            if (common_name) {
                // if the name of the locale for this category doesn't
                // match the common locale name so far, assign "*"
                if (strcmp (common_name, cat_name))
                    common_name = "*";
            }
            else
                common_name = cat_name;
        }

        // will be 0 iff all facets in a category are 0
        // (i.e., an internal logic error)
        _RWSTD_ASSERT (cat_name);

        if (   strlen (buf) + strlen (cat_name) + 2
            >= bufsize) {
            // reallocate buffer if necessary
            char *tmp = 0;

            _TRY {
                tmp = new char [bufsize *= 2];
            }
            _CATCH (...) {
                if (buf != savebuf)
                    delete[] buf;
                _RETHROW;
            }

            strcpy (tmp, buf);
            if (buf != savebuf)
                delete[] buf;
            buf = tmp;
        }

        strcat (buf, cat_name);
        strcat (buf, _RWSTD_CAT_SEP);
    }

    _RWSTD_ASSERT (common_name);

    if (__rw_facet::_C_opts & __rw_facet::_C_condensed_name) {
        // if all categories come from the same locale, copy the name
        // of that locale into the buffer
        if ('*' != common_name [0] || '\0' != common_name [1]) {
            if (   savebuf && buf != savebuf
                && savesize >= strlen (common_name)) {
                // delete allocated memory and repoint at original buffer
                delete[] buf;
                buf = savebuf;
            }
            strcpy (buf, common_name);
        }
    }

    // remove trailing separator
    const size_t len = strlen (buf);
    if (len && *_RWSTD_CAT_SEP == buf [len - 1])
        buf [len - 1] = '\0';

    return buf;
}


// returns a pointer to a character array containing locale names separated
// by _RWSTD_CAT_SEP corresponding to the named locale (typically all the
// names will be the same except possibly for the native locale which can
// consist of facets from different named locales specified by the LC_XXX
// environment variables)
static const char*
__rw_expand_name (__rw_chararray &namebuf, const char *name)
{
    const char *sep = strchr (name, *_RWSTD_CAT_SEP);

    if (sep == name) {
        // skip the first separator if it's at the beginning of `name'
        // (e.g., SunOS locale names have the format "/en/C/ja/C/it/de")
        sep = strchr (++name, *_RWSTD_CAT_SEP);
    }

    // true if all ctaegories are from the same locale, false otherwise
    bool same_cats = true;

    if (sep) {

        // `name' contains a locale category separator; iterate over
        // all locale categories to determine the name of the locale
        // for each one of them

        for (size_t i = 0; ; sep = strchr (name, *_RWSTD_CAT_SEP)) {

            if (!sep)
                sep = name + strlen (name);

            const size_t catlen = sep - name;

            // copy the read-only substring forming a locale name
            // into a writeable buffer so that it can be NUL-terminated
            const __rw_chararray shortname (name, catlen);

            __rw_chararray longname;

            // convert the (possibly abbreviated) name or a locale alias
            // into the full name uniquely identifying the locale (e.g.,
            // all of "de," "de_DE," and "german" can expand into the
            // full canonical name "de_DE.ISO-8869-1"; this is necessary
            // to guarantee that the internal locale repository doesn't
            // contain duplicate entries)
            const char* const fullname =
                __rw_locale_name (__rw_cats [i].cat, shortname.data (),
                                  longname);

            if (!fullname)
                return 0;

            const size_t namelen = strlen (fullname);

            _RWSTD_ASSERT (0 != namelen);

            // determine if the new category name is the same as the first
            // one (if it is, it is also the same as all subsequent ones,
            // otherwise `same_cats' is already set to false)
            if (same_cats && i)
                same_cats =    !memcmp (namebuf.data (), fullname, namelen)
                            && *_RWSTD_CAT_SEP == namebuf.data () [namelen];

            namebuf.append (fullname, namelen);

            if (++i == __rw_n_cats || !*sep)
                break;

            namebuf.append (_RWSTD_CAT_SEP, 1);

            name = sep + 1;
        }
    }
    else if ('\0' == *name) {

        // no separator found, set all categories
        const __rw_setlocale loc (name, _RWSTD_LC_ALL, 1 /* nothrow */);

        if (!loc.name ())
            return 0;

        // compose a string of category names separated by CAT_SEP
        for (size_t i = 0; ; ) {

            // retrieve the name of each idividual category
            const char* const fullname = setlocale (__rw_cats [i].cat, 0);

            // call to setlocale() must succeed if `loc' succeeded
            _RWSTD_ASSERT (0 != fullname);

            const size_t namelen = strlen (fullname);

            if (same_cats && i)
                same_cats =    !memcmp (namebuf.data (), fullname, namelen)
                            && *_RWSTD_CAT_SEP == namebuf.data () [namelen];

            namebuf.append (fullname, namelen);

            if (++i == __rw_n_cats)
                break;

            namebuf.append (_RWSTD_CAT_SEP, 1);
        }
    }
    
    if (*namebuf.data ()) {

        if (same_cats && __rw_facet::_C_opts & __rw_facet::_C_condensed_name) {

            // all categories were the same, store just the first one

            char* const  data   = namebuf.data ();
            const size_t sepinx = strchr (data, *_RWSTD_CAT_SEP) - data;

            // NUL-terminate at the first separator
            namebuf.acquire (data, sepinx);
            namebuf.append ("", 1);
        }
    }
    else {

        const char* const fullname =
            __rw_locale_name (_RWSTD_LC_ALL, name, namebuf);

        if (!fullname)
            return 0;
    }

    return namebuf.data ();

    
#if 0

    // points to the next category name
    const char *next = strchr (name, *_RWSTD_CAT_SEP);

    if (next && *next) {

            // save the end of the first name to detect combined locale
            // names composed of categories referring to the same locale
            char *first_end = bufbeg;

            // name is a string of locale category names; compose
            // an equivalent string obtained by concatenating the
            // results of calling setlocale() on each
            // substring

            const char *ps = name;

            size_t __i = 0;
            do {
                if (!next)
                    next = ps + strlen (ps);

                memcpy (pbuf, ps, next - ps);
                pbuf [next - ps] = '\0';

                if (__i < __rw_n_cats) {

                    const int cat = __rw_cats [__i++].cat;

                    // get the actual locale name for each category
                    // and append it to the end of the name buffer
                    if (!__rw_locale_name (cat, pbuf, pbuf, pbuf - bufbeg))
                        return 0;   // name not recognized
                }

                const size_t len = strlen (pbuf);

                if (first_end == bufbeg)
                    first_end = pbuf + len;
                else if (   first_end
                         && (   size_t (first_end - bufbeg) != len
                             || memcmp (bufbeg, pbuf, len))) {
                    // category name differs from the first (i.e., category
                    // names will be cobined) no need to check further
                    first_end = 0;
                }

                // advance to the next name
                pbuf    += len;
                *pbuf++  = *_RWSTD_CAT_SEP;

                // reached the end of the (not `sep'-terminated) list
                if ('\0' == *next)
                    break;

                ps    = next + 1;
                next  = strchr (ps, *_RWSTD_CAT_SEP);
            } while (*ps);

            *pbuf = '\0';

            // assume the native locale for all missing categories
            while (__i < __rw_n_cats) {

                const int cat = __rw_cats [__i++].cat;

                const __rw_setlocale loc ("", cat, 1 /* nothrow */);

                if (!loc.name ())
                    return 0;

                const char* catname = setlocale (cat, 0);

                _RWSTD_ASSERT (0 != catname);

                strcat (pbuf, catname);

                // compare the category name with the first one and remember
                // if they differ (to detect all the same category names)
                if (first_end && memcmp (bufbeg, name, first_end - name))
                    first_end = 0;

                pbuf    += strlen (pbuf);
                *pbuf++  = *_RWSTD_CAT_SEP;
            }

            // remove the last separator
            if (pbuf [-1] == *_RWSTD_CAT_SEP)
                pbuf [-1] = '\0';

            // if non-0, all categories come from the same locale
            // keep the first name and chop off the rest
            if (   __rw_facet::_C_opts & __rw_facet::_C_condensed_name
                && first_end)
                bufbeg [first_end - bufbeg] = '\0';
        }
        else {
            // all categories have the same name in named locales

            name = __rw_locale_name (_RWSTD_LC_ALL, name, bufbeg, bufsize);
            if (!name)
                return 0;
        }
    }

    return bufbeg;

#endif
}


size_t __rw_locale::
_C_get_facet_inx (size_t id) const
{
    // verify that facet's id is initialized
    _RWSTD_ASSERT (id);

    // standard facet's id are one greater than their index
    if (id <= _C_n_std_facets)
        return id - 1;

    _RWSTD_ASSERT (!_C_n_usr_facets || _C_usr_facets);

    // linear search through user-defined facets (if any)
    for (size_t i = 0; i != _C_n_usr_facets; ++i) {

        _RWSTD_ASSERT (_C_usr_facets [i]->_C_pid);

        if (*_C_usr_facets [i]->_C_pid == id)
            return i + _C_n_std_facets;
    }

    return _RWSTD_SIZE_MAX;
}


__rw_locale::
__rw_locale (const char *name)
    : _C_usr_facets (0), _C_n_usr_facets (0), _C_ref (1),
      _C_std_facet_bits (0), _C_byname_facet_bits (0)
{
    _RWSTD_ASSERT (name);

    // verify that name doesn't refer to an unnamed (combined) locale
    _RWSTD_ASSERT (!strchr (name, '*'));

    if ('\0' == *name) {

        __rw_chararray fullname;
        fullname.acquire (_C_namebuf, 0);

        // get the names of locales for all standard categories
        // avoiding dynamic memory allocation if possible
        _C_name = __rw_expand_name (fullname, name);

        fullname.release ();

        // caller must guarantee that `name' is valid
        _RWSTD_ASSERT (0 != _C_name);

        // note that `name' may not be the same as `_C_name' at this point
    }
    else {
        const size_t namelen = strlen (name) + 1;

        // assume name is well-formed and valid (checked by caller)
        char* const tmp = namelen < sizeof _C_namebuf
            ? _C_namebuf : new char [namelen];

        memcpy (tmp, name, namelen);

        _C_name = tmp;
    }

   // all facets are standard
   _C_std_facet_bits = _C_all;

   // byname facets determined below
   _C_byname_facet_bits = 0;

   if (strchr (_C_name, *_RWSTD_CAT_SEP)) {

       // name consist of multiple locale names; iterate over them all
       // setting byname facet bits for each named category
       size_t inx = 0;

       for (const char *nm = _C_name; *nm && inx != __rw_n_cats; ++nm) {

           if ('C' != nm [0] || *_RWSTD_CAT_SEP != nm [1])
               _C_byname_facet_bits |= __rw_cats [inx].facet_bits;

           nm = strchr (nm, *_RWSTD_CAT_SEP);
           if (!nm)
               break;

           ++inx;
       }
   }
   else {
       // all facets (except for num_{get,put}) are byname in non-C locales
       if (!__rw_is_C (_C_name))
           _C_byname_facet_bits = _C_all;
   }

#if defined (__i386__) || !defined (__GNUG__) || __GNUG__ < 3

   memset (_C_std_facets, 0, sizeof _C_std_facets);

#else   // !i86  || !gcc 3.x

   // Working around a gcc 3.1 optimizer bug on SPARC (and possibly
   // other RISC architectures) where, when generating PIC, _C_std_facets
   // ends up being 4-byte aligned and gcc replaces memset() with a series
   // of std %o1, [_C_std_facets + i] instructions with %o1 being 0 and i
   // being 0, 8, 16, etc. and std operating on 64-bit double words. The
   // first such misaligned std operation causes a SIGBUS

   for (size_t i = 0; i != _C_n_std_facets; ++i)
       _C_std_facets [i] = 0;

#endif   // i86/gcc 3.x

}


// convert a LC_XXX constant to a locale::category value
/* static */ int __rw_locale::
_C_LC2category (int cat)
{
    switch (cat) {
    case _RWSTD_LC_ALL:      cat = __rw_cat_all; break;
    case _RWSTD_LC_COLLATE:  cat = __rw_cat_collate; break;
    case _RWSTD_LC_CTYPE:    cat = __rw_cat_ctype; break;
    case _RWSTD_LC_MONETARY: cat = __rw_cat_monetary; break;
    case _RWSTD_LC_NUMERIC:  cat = __rw_cat_numeric; break;
    case _RWSTD_LC_TIME:     cat = __rw_cat_time; break;
#ifdef _RWSTD_LC_MESSAGES
    case _RWSTD_LC_MESSAGES: cat = __rw_cat_messages; break;
#endif

    default:
        // assume `cat' is already a locale::category value
        break;
    }
    return cat;
}


// convert a LC_XXX constant to an internal bitset of facets
/* static */ int __rw_locale::
_C_LC2facet_bits (int cat)
{
    int bits;

    switch (cat) {
    case _RWSTD_LC_ALL:      bits = __rw_locale::_C_all; break;
    case _RWSTD_LC_COLLATE:  bits = __rw_locale::_C_collate; break;
    case _RWSTD_LC_CTYPE:    bits = __rw_locale::_C_ctype; break;
    case _RWSTD_LC_MONETARY: bits = __rw_locale::_C_monetary; break;
    case _RWSTD_LC_NUMERIC:  bits = __rw_locale::_C_numeric; break;
    case _RWSTD_LC_TIME:     bits = __rw_locale::_C_time; break;
#ifdef _RWSTD_LC_MESSAGES
    case _RWSTD_LC_MESSAGES: bits = __rw_locale::_C_messages; break;
#endif

    default: {

        // assume `cat' is a locale::category value
        bits = 0;

        if (cat & __rw_cat_collate)
            bits |= _RW::__rw_locale::_C_collate;
        if (cat & __rw_cat_ctype)
            bits |= _RW::__rw_locale::_C_ctype;
        if (cat & __rw_cat_monetary)
            bits |= _RW::__rw_locale::_C_monetary;
        if (cat & __rw_cat_numeric)
            bits |= _RW::__rw_locale::_C_numeric;
        if (cat & __rw_cat_time)
            bits |= _RW::__rw_locale::_C_time;
        if (cat & __rw_cat_messages)
            bits |= _RW::__rw_locale::_C_messages;
        break;
    }

    }
    return bits;
}


__rw_locale::
~__rw_locale ()
{
    // verify that object isn't being destroyed prematurely
    _RWSTD_ASSERT (0 == _C_ref);

    if (_C_name != _C_namebuf)
        delete[] _RWSTD_CONST_CAST (char*, _C_name);

    // decrement the reference count of each installed facet
    // and delete each facet whose refcount drops to zero
    for (size_t i = 0; i != _C_n_std_facets; ++i) {

        if (!_C_std_facets [i])
            continue;

        _RWSTD_ASSERT (_C_std_facets [i]->_C_type != __rw_facet::_C_invalid);

        if (_C_std_facets [i]->_C_type) {

            // only standard facets (non-zero type) are managed
            // decrement the facet's reference count and remove
            // it from the global repository if it is 0
            const __rw_facet* const pfacet =
                __rw_facet::_C_manage (_C_std_facets [i],
                                       _C_std_facets [i]->_C_type,
                                       _C_std_facets [i]->_C_name,
                                       0);

            // verify that facet has been disposed of
            _RWSTD_ASSERT (0 == pfacet);
            _RWSTD_UNUSED (pfacet);
        }
        else {
            // decrement ref count (must not drop below 0)
            const size_t ref = _C_remove_ref (*_C_std_facets [i]);

            _RWSTD_ASSERT (ref + 1U != 0);

            if (0 == ref)
                delete _C_std_facets [i];
        }
    }

    // verify that either both are 0 or neither is 0
    _RWSTD_ASSERT (!_C_n_usr_facets == !_C_usr_facets);

    // decrement ref counts of user-defined facets and delete if necessary
    for (size_t j = 0; j != _C_n_usr_facets; ++j) {

        _RWSTD_ASSERT (_C_usr_facets [j]);

        _RWSTD_ASSERT (_C_usr_facets [j]->_C_type != __rw_facet::_C_invalid);

        // decrement ref count (must not drop below 0)
        const size_t ref = _C_remove_ref (*_C_usr_facets [j]);

        _RWSTD_ASSERT (ref + 1U != 0);

        if (0 == ref)
            delete _C_usr_facets [j];
    }

    delete[] _C_usr_facets;
}


extern "C" {

// compares two locales, returns 0 if equal, -1 if less, +1 otherwise
static int
cmplocales (const void *pv1, const void *pv2)
{
    _RWSTD_ASSERT (0 != pv1);
    _RWSTD_ASSERT (0 != pv2);

    const __rw_locale* const pl1 =
        *_RWSTD_STATIC_CAST (const __rw_locale* const*, pv1);
    const __rw_locale* const pl2 =
        *_RWSTD_STATIC_CAST (const __rw_locale* const*, pv2);

    _RWSTD_ASSERT (0 != pl1->_C_get_name ());
    _RWSTD_ASSERT (0 != pl1->_C_get_name ());

    return strcmp (pl1->_C_get_name (), pl2->_C_get_name ());
}


// compares a key to a locale, returns 0 if equal, -1 if less, +1 otherwise
static int
cmplocale (const void *pv1, const void *pv2)
{
    _RWSTD_ASSERT (0 != pv1);
    _RWSTD_ASSERT (0 != pv2);

    const char* const nm1 = _RWSTD_STATIC_CAST (const char*, pv1);

    const __rw_locale* const plocale =
        *_RWSTD_STATIC_CAST (const __rw_locale* const*, pv2);

    _RWSTD_ASSERT (0 != nm1);
    _RWSTD_ASSERT (0 != plocale->_C_get_name ());

    return strcmp (nm1, plocale->_C_get_name ());
}

}   // extern "C"


// manages a global, per-process repository of locale bodies according
// to the following table:

//     plocale   locname   algorithm
//        0         0      retrieve the global locale
//        0       non-0    find named locale in repository and increment
//                         its ref count if found, or construct it with
//                         ref count of 1 and add it otherwise
//      non-0       0      make `plocale' the new global locale and
//                         return the previous global locale
//      non-0     non-0    find locale in repository, decrement its ref
//                         count, and remove it if the ref count is 0

/* static */ __rw_locale*
__rw_locale::
_C_manage (__rw_locale *plocale, const char *locname)
{
    // a per-process array of locale body pointers
    static __rw_locale*  locale_buf [8];
    static __rw_locale** locales        = locale_buf;
    static __rw_locale*  classic        = 0;
    static size_t        n_locales      = 0;
    static size_t        locale_bufsize =
        sizeof locale_buf / sizeof *locale_buf;

    if (!locname) {

        // manage the global locale

        static __rw_locale* global = 0;

        if (!global) {

            // volatile to prevent optimizers from turning
            // the while statement below into an infinite loop
            static volatile int ginit /* = 0 */;

            // cast ginit to int& (STDCXX-792)
            // casting should be removed after fixing STDCXX-794
            if (!ginit && 1 == _RWSTD_ATOMIC_PREINCREMENT (
                    _RWSTD_CONST_CAST (int&, ginit), false)) {
                global  = _C_manage (0, "C");
                ginit  += 1000;
            }
            else {
                // ginit must be volatile to prevent optimizers
                // from turning this into an infinite loop
                while (ginit < 1000) { /* empty */ }
            }
        }

        // re-entrant
        _RWSTD_MT_STATIC_GUARD (_RW::__rw_locale);

        __rw_locale* const tmp = global;

        _RWSTD_ASSERT (0 != tmp);

        if (plocale) {
            // replace the global locale with `plocale'

            // increment the future global locale's ref count first
            _RWSTD_ATOMIC_PREINCREMENT (plocale->_C_ref, false);

            // and then set the global locale
            global = plocale;
        }
        else {

            // retrieve the global locale

            // always increment a ref count, even if `global' is being set
            // (and its ref count is > 0), to prevent its ref count from
            // dropping to 0
            _RWSTD_ATOMIC_PREINCREMENT (tmp->_C_ref, false);
        }

        return tmp;
    }


    // re-entrant to protect static local data structures
    // (not the locales themselves)
    _RWSTD_MT_STATIC_GUARD (_RW::__rw_locale);

    // determine whether `locname' refers to the classic C locale
    // (e.g., it could be any of "C", "POSIX", "C C C C C C", etc.)
    bool is_C_locale = __rw_is_C (locname);

    // substitute the fully expanded libc name for the classic C locale,
    // i.e., the result of calling std::setlocale ("C")
    if (is_C_locale)
        locname = __rw_C_locale_name;

    __rw_chararray fullname;

    if (!plocale && !is_C_locale) {
        // to avoid unnecessary calls to setlocale() only get cat names
        // when composing a locale name, otherwise the locale name is
        // assumed to be correct and complete
        locname = __rw_expand_name (fullname, locname);

        if (!locname)
            return 0;   // program error

        is_C_locale = __rw_is_C (locname);

        if (is_C_locale)
            locname = __rw_C_locale_name;
    }

    if (plocale) {
        // remove locale from the locale repository (if it is found) and
        // destroy it (the classic C locale is never destroyed)

        _RWSTD_ASSERT (0 != n_locales);

        const void* const pl =
            bsearch (locname, locales, n_locales, sizeof *locales, &cmplocale);
        if (pl) {

            const size_t inx =
                  _RWSTD_STATIC_CAST (const __rw_locale* const*, pl)
                - _RWSTD_CONST_CAST (const __rw_locale* const*, locales);

            plocale = locales [inx];

            const size_t ref =
                _RWSTD_ATOMIC_PREDECREMENT (plocale->_C_ref, false);

            _RWSTD_ASSERT (_RWSTD_SIZE_MAX != ref);

            if (0 == ref) {

                static const size_t bufsize =
                    sizeof locale_buf / sizeof *locale_buf;

                --n_locales;

                if (n_locales < bufsize / 2 && locales != locale_buf) {

                    _RWSTD_ASSERT (inx < bufsize);

                    // when the number of locales in the dynamically
                    // allocated repository drops below half the capacity
                    // of the statically allocated buffer, copy locales
                    // from the former into the latter and deallocate the
                    // former
                    memcpy (locale_buf, locales, inx * sizeof (*locales));

                    memcpy (locale_buf + inx, locales + inx + 1,
                            (n_locales - inx) * sizeof (*locales));

                    delete[] locales;

                    // point at the statically allocated buffer and reset
                    // the current capacity to the original capacity
                    locales        = locale_buf;
                    locale_bufsize = bufsize;
                }
                else {
                    // move facet pointers back
                    memmove (locales + inx, locales + inx + 1,
                             (n_locales - inx) * sizeof (*locales));
                }

                _RWSTD_ASSERT (0 != plocale->_C_name);

                // only named locales (i.e., other than the C locale) 
                // are deleted
                if (!__rw_is_C (plocale->_C_name))
                    delete plocale;
            }
        }
        else {

            // object is the classic C locale which is never destroyed
            _RWSTD_ASSERT (plocale->_C_name);
            _RWSTD_ASSERT (__rw_is_C (plocale->_C_name));

            const size_t ref =
                _RWSTD_ATOMIC_PREDECREMENT (plocale->_C_ref, false);

            _RWSTD_ASSERT (ref + 1U != 0);
            _RWSTD_UNUSED (ref);
         }
        
        plocale = 0;
    }
    else {

        // find locale in the repository or construct it if not found

        const void* const pcl =
            bsearch (locname, locales, n_locales, sizeof *locales, &cmplocale);

        if (pcl) {
            // facet already exists in the repository, bump up its ref count

            void* const pl = _RWSTD_CONST_CAST (void*, pcl);

            plocale  = *_RWSTD_STATIC_CAST (__rw_locale**, pl);

            _RWSTD_ASSERT (0 != plocale);

            _RWSTD_ATOMIC_PREINCREMENT (plocale->_C_ref, false);
        }
        else {

            if (n_locales == locale_bufsize) {

                // reallocate buffer of facet pointers
                __rw_locale** const tmp =
                    new __rw_locale* [n_locales * 2];

                memcpy (tmp, locales, n_locales * sizeof *tmp);

                if (locales != locale_buf)
                    delete[] locales;

                locales         = tmp;
                locale_bufsize *= 2;
            }

            if (__rw_is_C (locname)) {

                if (!classic) {

                    // the classic C locale is statically allocated
                    // and not destroyed during the lifetime of the process
                    static union {
                        void* _C_align;
                        char  _C_buf [sizeof (__rw_locale)];
                    } classic_body;

                    // construct a locale body in place
                    // with the initial reference count of 1
                    classic = new (&classic_body) __rw_locale (locname);

                    _RWSTD_ASSERT (1 == classic->_C_ref);
                }
                else {
                    _RWSTD_ASSERT (0 != classic);

                    _RWSTD_ATOMIC_PREINCREMENT (classic->_C_ref, false);
                }

                plocale = classic;
            }
            else {
                plocale = new __rw_locale (locname);

                _RWSTD_ASSERT (1 == plocale->_C_ref);
            }

            _RWSTD_ASSERT (plocale);

            // add locale body pointer to the end of the repository
            locales [n_locales++] = plocale;

            // sort pointers for fast access
            qsort (locales, n_locales, sizeof *locales, &cmplocales);
        }
    }

    return plocale;
}


// returns true iff all the facets in the given locale category
// installed in `*this' are being globally managed by the library
// if (cat == locale::none) holds, returns true iff the entire
// locale body is being managed
bool __rw_locale::
_C_is_managed (int cat) const
{
    // `cat' must be a valid category
    _RWSTD_ASSERT (_C_check_category (_C_LC2category (cat)));

    if (cat == __rw_cat_none) {

        // locales containing any user-defined facets are not globally managed
        if (_C_n_usr_facets) {
            _RWSTD_ASSERT (0 != _C_usr_facets);
            return false;
        }

        // unless all facets in the same category come either from
        // the C locale or from some named locale the locale object
        // containing the facets is not managed (this test doesn't
        // detect categores of all byname facets not all of which
        // belong to the same named locale)
        if (   (_C_std_facet_bits & _C_all) != _C_all
            || (_C_byname_facet_bits & _C_collate)
            && (_C_byname_facet_bits & _C_collate)  != _C_collate
            || (_C_byname_facet_bits & _C_ctype)
            && (_C_byname_facet_bits & _C_ctype)    != _C_ctype
            || (_C_byname_facet_bits & _C_monetary)
            && (_C_byname_facet_bits & _C_monetary) != _C_monetary
            || (_C_byname_facet_bits & _C_numeric)
            && (_C_byname_facet_bits & _C_numeric)  != _C_numeric
            || (_C_byname_facet_bits & _C_time)
            && (_C_byname_facet_bits & _C_time)     != _C_time
            || (_C_byname_facet_bits & _C_messages)
            && (_C_byname_facet_bits & _C_messages) != _C_messages)
            return false;

        // check whether all _byname facets in each category belong
        // to the same named locale; facets witout byname forms are
        // excluded from this test
        // note that the locale may have a combined name such as
        // "/de/en/fr/it/ja/ru"
        const char* locname = _C_name ? _C_name : "C";

        if (*_RWSTD_CAT_SEP == *locname)
            ++locname;

        // find the first category separator (if any) and compute
        // the length of the name of the first category's locale
        // (don't bother when combined locales don't exist, i.e.,
        // when no locale separator is defined)
        const char* next =
            *_RWSTD_CAT_SEP ? strchr (locname, *_RWSTD_CAT_SEP) : 0;

        size_t loclen = next ? next - locname : strlen (locname);

        // iterate over all standard facets, comparing the name of the
        // locale each comes from with the name of the category encoded
        // in the locale name for equality; a mismatch indicates that
        // the locale is not managed
        for (size_t i = 0, catinx = 0; i != _C_n_std_facets; ++i) {

            if (0 == _C_std_facets [i]) {
                // skip facets that are not installed
                continue;
            }

            const __rw_facet::_C_facet_type facet_type =
                _C_get_facet_type (*_C_std_facets [i]);

            switch (facet_type) {
            case __rw_facet::_C_money_get:
            case __rw_facet::_C_money_put:
            case __rw_facet::_C_num_get:
            case __rw_facet::_C_num_put:
            case __rw_facet::_C_wmoney_get:
            case __rw_facet::_C_wmoney_put:
            case __rw_facet::_C_wnum_get:
            case __rw_facet::_C_wnum_put:
                // skip facets with no _byname forms
                continue;
            default:
                break;
            }

            // iterate through the names of categories in the combined
            // locale name looking for one that the facet belongs to
            while (next && !(__rw_cats [catinx].facet_bits & (1 << i))) {

                if (*next) {
                    // advance past the category separator to the name
                    // of the next category in the (potentially combined)
                    // locale name
                    _RWSTD_ASSERT (*next == *_RWSTD_CAT_SEP);

                   locname = ++next;
                    ++catinx;
                }
                else {
                    // reset to the name of the first category
                    locname = _C_name ? _C_name : "C";
                    catinx  = 0;
                }

                // skip over the category separator
                if (*_RWSTD_CAT_SEP == *locname)
                    ++locname;

                next = strchr (locname, *_RWSTD_CAT_SEP);
                if (next)
                    loclen = next - locname;
                else {
                    loclen = strlen (locname);
                    next   = "";
                }
            }

            // get the facet name (or "C" if null)
            const char* const facet_name =
                _C_std_facets [i]->_C_get_name ();

            _RWSTD_ASSERT (0 != facet_name);
            _RWSTD_ASSERT (0 != locname);

            // mismatch indicates a combined category
            if (   strlen (facet_name) != loclen
                || memcmp (locname, facet_name, loclen)) {
                // locales with heterogeneous categories are not managed
                return false;
            }
        }

        // locales with all heterogeneous categories are managed
        return true;
    }

    // in order for the entire category to be considered globally managed,
    // all its facets must come from the same (named or unnamed) locale
    // i.e., if the LC_CTYPE category contains codecvt from "de_DE" and
    // ctype from "en_US", the category is not considered globally managed
    // because it doesn't have a unambiguous name
    const int bits = _C_LC2facet_bits (cat);

    const int std_bits = bits & int (_C_std_facet_bits);

    if (std_bits ? bits == std_bits
                 : bits == (bits & int (_C_byname_facet_bits)))
        return true;

    return false;
}


}   // namespace __rw


/***************************************************************************/


#include <memory>
#include <loc/_locale.h>

#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 


const locale&
locale::
operator= (const locale &rhs) _THROWS (())
{
    // copy body and bump up a ref count in `rhs'
    _RW::__rw_locale* const body = rhs._C_body;

    _RWSTD_ATOMIC_PREINCREMENT (rhs._C_body->_C_ref, false);


    // _C_body is never 0 except in objects still being constructed by
    // the combining ctor locale::locale<Facet>(const locale&, Facet*)

    if (_C_body) {
        // dispose of old body (dtor decrements ref count and possibly deletes)
        locale destroy (*_C_body);

        _RWSTD_UNUSED (destroy);
    }

    // assign new body
    _C_body = body;

    return *this;
}


}   // namespace std
