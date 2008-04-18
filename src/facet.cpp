/***************************************************************************
 *
 * facet.cpp
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
 * Copyright 2001-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#include <rw/_defs.h>

#include <locale.h>   // for LC_XXX costants
#include <stdlib.h>   // for bsearch(), getenv(), qsort()
#include <string.h>   // for memcpy(), strlen()

#include <iosfwd>         // for mbstate_t

#include "access.h"
#include "mman.h"

#include <loc/_facet.h>
#include <loc/_locale.h>
#include <loc/_localedef.h>

#include <rw/_mutex.h>

#include "locale_body.h"
#include "podarray.h"    // for __rw_chararry


_RWSTD_NAMESPACE (__rw) { 

// computes LC_XXX category from a numeric facet id, returns the
// LC_XXX category for standard facets, LC_ALL for all others
int __rw_get_cat (int);

// retrieves the literal name for a given category value
static inline 
const char* __rw_get_cat_name (int category)
{
    // FIXME: merge this struct and lookup with __rw_cats in rw_locale.cpp
    // maps LC_XXX category values to their names
    static const struct {
        int         cat;           // LC_XXX category
        const char *name;          // string corresponding to LC_XXX
    } cats [] = {
        { _RWSTD_LC_COLLATE,  "LC_COLLATE"},
        { _RWSTD_LC_CTYPE,    "LC_CTYPE"},
        { _RWSTD_LC_MONETARY, "LC_MONETARY"},
        { _RWSTD_LC_NUMERIC,  "LC_NUMERIC"},
        { _RWSTD_LC_TIME,     "LC_TIME"}

#ifdef _RWSTD_LC_MESSAGES
        , { _RWSTD_LC_MESSAGES, "LC_MESSAGES" }
#endif   // _RWSTD_LC_MESSAGES
    };

    // find facet's category name
    for (_RWSTD_SIZE_T i = 0; i != sizeof cats / sizeof *cats; ++i) {
        if (category == cats [i].cat)
            return cats [i].name;
    }

    return 0;
}


int __rw_facet::_C_opts = 0

#ifndef _RWSTD_NO_INITIAL_CAT_SEP
    | __rw_facet::_C_prepend_cat_sep
#endif   // _RWSTD_NO_INITIAL_CAT_SEP

#ifndef _RWSTD_NO_CAT_NAMES
    | __rw_facet::_C_use_cat_names
#endif   // _RWSTD_NO_CAT_NAMES

#ifndef _RWSTD_NO_CONDENSED_NAME
    | __rw_facet::_C_condensed_name
#endif   // _RWSTD_NO_CONDENSED_NAME

    | 0;



__rw_facet::__rw_facet (_RWSTD_SIZE_T __ref /* = 0 */)
    : _C_name (0), _C_buf (0), _C_impdata (0), _C_impsize (0),
    _C_type (_C_unknown), _C_ref (__ref), _C_pid (0 /* invalid */)
{
}


__rw_facet::~__rw_facet ()
{
    _RWSTD_ASSERT (!_C_name && !_C_buf || _C_name && _C_buf);

    static const char destroyed[] = "*** destroyed facet ***";

    // (try to) detect double deletion
    _RWSTD_ASSERT (destroyed != _C_name);
    _RWSTD_ASSERT (_C_invalid != _C_type);

    // mark invalid
    _C_type = _C_invalid;

    if (_C_name != _C_buf)
        //  MSVC requires that we cast away const before calling delete.
        delete[] _RWSTD_CONST_CAST (char*, _C_name);

    // mark destroyed
    _C_name = destroyed;

    // unmap only if both `impadata' and `impsize' are valid
    // (derived facets may want to set `impdata' to their own,
    // not necessarily memory mapped, data w/o setting `impsize'
    // to something meaningful
    if ((_RWSTD_SIZE_T)(-1) == _C_impsize)
        ::operator delete (_RWSTD_CONST_CAST (void*, _C_impdata));
    else if (_C_impdata && _C_impsize) 
        __rw_release_facet_data (_C_impdata, _C_impsize);
}


void __rw_facet::_C_set_name (const char *name, char *buf, _RWSTD_SIZE_T size)
{
    _RWSTD_ASSERT (0 != name);
    _RWSTD_ASSERT (0 != buf || 0 == size);

    // expected to be called only from the ctor
    _RWSTD_ASSERT (0 == _C_name);
    _RWSTD_ASSERT (0 == _C_buf);

    _RWSTD_SIZE_T len = ::strlen (name) + 1;

    char *tmp = len <= size ? buf : new char [len];

    ::memcpy (tmp, name, len);
    _C_name = tmp;
    _C_buf  = buf;
}


const void* __rw_facet::_C_get_data ()
{
    // check initialization
    if (_C_impsize)
        return _C_impdata;

    // `pid' may be 0 if the facet has not been obtained
    // by a call to use_facet (but instead constructed
    // and used directly without having been installed
    // in a locale object); in that case, locale database
    // mapping for the facet is not available
    if (!_C_pid)
        return _C_impdata;

    // lock the object
    _RWSTD_MT_GUARD (&_C_mutex);

    // check again for initialization
    if (_C_impsize)
        return _C_impdata;

    __rw_chararray locname (_C_name && _C_name [0] ? _C_name : "C");

    // implementation database mapping data
    const void* pdata = 0;
    _RWSTD_SIZE_T sz = 0;

    _RWSTD_ASSERT (0 != _C_pid);
    const int cat = __rw_get_cat (int (*_C_pid));

    // use facet's id and name to determine whether the type of the facet is
    // codecvt_byname<wchar_t, char, mbstate_t> (_C_type is not sufficiently
    // reliable since it is set to _C_unknown for all facet objects contructed
    // outside of the library)
    const bool is_wcodecvt_byname =
        _C_wcodecvt_byname == __rw_locale::_C_get_facet_type (*this);

    // map the database; first try with the full name (including
    // any modifiers) since such a database may actually exist
    pdata = __rw_get_facet_data (cat, sz, locname.data ());
    if (!pdata) {

        // the mapping failure can be attributed to the use of a
        // special name, eg <locale>.<codeset>@UCS-4 which calls
        // for the stripping of the special suffix
        _RW::__rw_chararray strip_name;

        const char*  pstr = strstr (locname.data (), "@UCS");
        const size_t plen = size_t (pstr - locname.data ());

        if (pstr && !pstr [4]) {

            // @UCS suffix is recognized irrespective of the size of wchar_t,
            strip_name.append (locname.data (), plen);
            pdata = __rw_get_facet_data (cat, sz, strip_name.data ());
        }
        else if (   pstr && pstr [4] == '-' && !pstr [6]
                 && (   '4' == pstr [5] && sizeof (wchar_t) == 4
                     || '2' == pstr [5] && sizeof (wchar_t) == 2)) {

            // @UCS-4 is only recognized where sizeof (wchar_t) == 4
            // @UCS-2 is only recognized where sizeof (wchar_t) == 2
            // no other modifier is recognized
            strip_name.append (locname.data (), plen);
            pdata = __rw_get_facet_data (cat, sz, strip_name.data ());
        }

        // if the type of the facet is codecvt_byname<wchar_t,char,mbstate_t>
        // then the name is possibly in the format codeset | codeset@modifier
        if (!pdata && is_wcodecvt_byname) {

            pstr = pstr ? strip_name.data () : locname.data ();
            pdata = __rw_get_facet_data (cat, sz, 0, pstr /* codeset */);
        }
        else
            pstr = 0;

        // this is the definitive result for facets other than 
        // codecvt_byname<wchar_t, char, mbstate_t>
        if (!pdata) {
            _RWSTD_ASSERT (0 == _C_impdata);

            // set `impsize' to non-zero value to avoid subsequent
            // attempts at reinitialization
            _C_impsize = (_RWSTD_SIZE_T)-1;
            return 0;
        }

        if (!is_wcodecvt_byname || pstr && pdata) {
            _C_impdata = pdata; 
            _C_impsize = sz;
            return _C_impdata;
        }
    }
    else if (!is_wcodecvt_byname) {
        _C_impdata = pdata;
        _C_impsize = sz;
        return _C_impdata;
    }

    // for codecvt_byname facet map the database indicated by 
    // codeset_name offset in LC_CTYPE database
    const _RW::__rw_ctype_t* pctype  = 
        _RWSTD_STATIC_CAST (const _RW::__rw_ctype_t*, pdata);
 
    _RWSTD_ASSERT (0 != pctype);

    // the facet name may be an ordinary filename or a (relative
    // or absolute) pathname; unless it's the former, prepend
    // the name of the directory containing the facet to the
    // name of the codeset before trying to map it
    __rw_chararray codeset_fname;
    const char* codeset = pctype->codeset_name ();

    const char* const slash = strrchr (locname.data (), _RWSTD_PATH_SEP);
    if (slash) {
        const size_t dirlen = size_t (slash - locname.data ()) + 1;

        codeset_fname.assign (locname.data (), dirlen);
        codeset_fname.append (codeset);
        codeset = codeset_fname.data ();
    }

    // map the codecvt database
    _C_impdata = __rw_get_facet_data (cat, _C_impsize, 0, codeset);
    
    // unmap the LC_CTYPE database
    __rw_munmap (pdata, sz);

    return _C_impdata;
}


extern "C" {

// compares two facets, returns 0 if equal, -1 if less, +1 otherwise
static int cmpfacets (const void *pv1, const void *pv2)
{
    _RWSTD_ASSERT (0 != pv1);
    _RWSTD_ASSERT (0 != pv2);

    const __rw_facet* const pf1 =
        *_RWSTD_STATIC_CAST (const __rw_facet* const*, pv1);
    const __rw_facet* const pf2 =
        *_RWSTD_STATIC_CAST (const __rw_facet* const*, pv2);

    if (__rw_access::_C_get_type(*pf1) == __rw_access::_C_get_type(*pf2)) {
        return ::strcmp (__rw_access::_C_get_name(*pf1) ? 
                         __rw_access::_C_get_name(*pf1) : "C",
                         __rw_access::_C_get_name(*pf2) ? 
                         __rw_access::_C_get_name(*pf2) : "C");
    }

    return __rw_access::_C_get_type(*pf2) - __rw_access::_C_get_type(*pf1);
}


struct __rw_facet_info
{
    const char                *_C_name;
    __rw_facet::_C_facet_type  _C_type;
};

// compares a key to a facet, returns 0 if equal, -1 if less, +1 otherwise
static int cmpfacet (const void *pv1, const void *pv2)
{
    _RWSTD_ASSERT (0 != pv1);
    _RWSTD_ASSERT (0 != pv2);

    const __rw_facet_info* const pinfo =
        _RWSTD_STATIC_CAST (const __rw_facet_info*, pv1);

    const __rw_facet* const pfacet =
        *_RWSTD_STATIC_CAST (const __rw_facet* const*, pv2);

    if (pinfo->_C_type == __rw_access::_C_get_type(*pfacet)) {
        return ::strcmp (pinfo->_C_name  ? pinfo->_C_name   : "C",
                         __rw_access::_C_get_name(*pfacet) ? 
                         __rw_access::_C_get_name(*pfacet) : "C");
    }

    return __rw_access::_C_get_type(*pfacet) - pinfo->_C_type;
}

}   // extern "C"


/* static */ __rw_facet*
__rw_facet::_C_manage (__rw_facet            *pfacet,
                       _C_facet_type          type,
                       const char            *name,
                       __rw_facet::_C_ctor_t *ctor)
{
    // a per-process array of facet pointers sufficiently large
    // to hold (pointers to) all standard facets for 8 locales
    static __rw_facet*  std_facet_buf [__rw_facet::_C_last_type * 8];
    static __rw_facet** std_facets = std_facet_buf;
    static _RWSTD_SIZE_T n_std_facets = 0;
    static _RWSTD_SIZE_T std_facet_bufsize =
        sizeof std_facet_buf / sizeof *std_facet_buf;

    // acquire lock
    _RWSTD_MT_STATIC_GUARD (_RW::__rw_facet);

    if (pfacet) {
        // remove facet from the facet repository (if it is found) and
        // destroy it (ordinary unnamed standard facets are never destroyed)

        if (!name) {
            // find facet by its name
            name = __rw_locale::_C_get_cat_name (*pfacet);
        }

        _RWSTD_ASSERT (0 != n_std_facets);

        const void *pf =
            ::bsearch (&pfacet, std_facets, n_std_facets,
                       sizeof pfacet, &cmpfacets);
        if (pf) {

            const _RWSTD_SIZE_T inx =
                  _RWSTD_STATIC_CAST (const __rw_facet* const*, pf)
                - _RWSTD_CONST_CAST (const __rw_facet* const*, std_facets);

            pfacet = std_facets [inx];

            if (0 == __rw_locale::_C_remove_ref (*pfacet)) {

                static const _RWSTD_SIZE_T bufsize =
                    sizeof std_facet_buf / sizeof *std_facet_buf;

                --n_std_facets;

                if (n_std_facets < bufsize / 2 && std_facets != std_facet_buf) {

                    _RWSTD_ASSERT (inx < bufsize);

                    // when the number of facets in the dynamically allocated
                    // repository drops below half the capacity of the
                    // statically allocated buffer, copy facets from the
                    // former into the latter and deallocate the former
                    ::memcpy (std_facet_buf, std_facets,
                              inx * sizeof (*std_facets));

                    ::memcpy (std_facet_buf + inx,
                              std_facets + inx + 1,
                              (n_std_facets - inx)
                              * sizeof (*std_facets));

                    delete[] std_facets;
                    std_facets = std_facet_buf;
                }
                else {

                    // move facet pointers back
                    ::memmove (std_facets + inx,
                               std_facets + inx + 1,
                               (n_std_facets - inx)
                               * sizeof (*std_facets));
                }

                // delete facet unless it's one of the ordinary (unnamed)
                // standard factes which are statically allocated
                if (pfacet->_C_name)
                    delete pfacet;
            }
        }
        else {

            // facet is an ordinary (unnamed) standard facet
            const _RWSTD_SIZE_T ref = __rw_locale::_C_remove_ref (*pfacet);
            _RWSTD_ASSERT (ref + 1U != 0);
            _RWSTD_UNUSED (ref);
        }

        pfacet = 0;
    }
    else {

        // find facet in the repository or construct it if not found

        const __rw_facet_info info = { name, type };

        const void *pcf = ::bsearch (&info, std_facets, n_std_facets,
                                     sizeof pfacet, &cmpfacet);

        if (pcf) {
            // facet already exists in the repository, bump up its ref count

            void *pf = _RWSTD_CONST_CAST (void*, pcf);
            pfacet = *_RWSTD_STATIC_CAST (__rw_facet**, pf);

            _RWSTD_ASSERT (0 != pfacet);

            __rw_locale::_C_add_ref (*pfacet);
        }
        else {

            if (n_std_facets == std_facet_bufsize) {

                // reallocate buffer of facet pointers
                __rw_facet **tmp = new __rw_facet* [n_std_facets * 2];

                ::memcpy (tmp, std_facets, n_std_facets * sizeof *tmp);

                if (std_facets != std_facet_buf)
                    delete[] std_facets;

                std_facets         = tmp;
                std_facet_bufsize *= 2;
            }

            // construct a facet with the initial reference count of 1
            // if no such facet exists yet or bump up the facet's ref
            // count by 1 if it does

            _RWSTD_ASSERT (0 != ctor);

            pfacet = ctor (1, type & 1 ? 0 : name);

            _RWSTD_ASSERT (0 != pfacet);

            // set the facet's numeric id
            *__rw_access::_C_get_pid (*pfacet) =
                _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, (type + 1) / 2);

            if (__rw_access::_C_get_type (*pfacet) != type) {
                // set the type of the facet if not already set
                __rw_access::_C_get_type (*pfacet) = type;
            }

            if (__rw_access::_C_get_ref (*pfacet) != 1) {
                // set the refcount of the facet if 0

                // the refcount is set during construction which occurs
                // at most once in a program, but may decrease to 0 if
                // all locales containing that facet are destroyed

                _RWSTD_ASSERT (0 == __rw_access::_C_get_ref (*pfacet));

                __rw_access::_C_get_ref (*pfacet) = 1;
            }

            // add facet pointer to the end of the repository
            std_facets [n_std_facets++] = pfacet;

            // sort facet pointers for fast access
            ::qsort (std_facets, n_std_facets, sizeof pfacet, &cmpfacets);
            
        }
    }

    return pfacet;
}


const void* __rw_get_facet_data (int cat, _RWSTD_SIZE_T &impsize, 
                                 const char* name, const char* encoding) 
{
    if (__rw_facet::_C_use_libc == (__rw_facet::_C_opts & 
                                    (__rw_facet::_C_use_libc | 
                                     __rw_facet::_C_use_libstd))) {
        // fail but allow further calls to this function
        return 0;
    }

    __rw_chararray  pathname;
    const char     *path;

    // null name means C locale
    const char *locname = name ? name : "C";

    if ('/' == *locname) {
        // facet name is an absolute pathname to a locale/category database
        path = locname;
    }
    else {

#ifndef _RWSTD_LOCALE_ROOT_ENVVAR
#  define _RWSTD_LOCALE_ROOT_ENVVAR "RWSTD_LOCALE_ROOT"
#endif

        // look up the value of the environment variable
        const char *root = getenv (_RWSTD_LOCALE_ROOT_ENVVAR);

        // use the cwd if none given
        if (!root || !*root)
            root = "./";

        // compose the pathname of the locale/category database
        pathname.append (root);
        if (pathname.data () [pathname.size () - 1] != '/')
            pathname.append ("/", 1);

        // the database name for the facet will hold either the
        // name of the category (ie LC_CTYPE, etc.) or the name
        // of the encoding (codecvt database)
        const char* db;

        if (encoding)
            db = encoding;
        else {
            // only append the locname if we are not going to be 
            // appending an encoding
            pathname.append (locname).append ("/", 1);
            db = __rw_get_cat_name (cat);
        }
        if (!db)
            return 0;

        // append database name to the end of the pathname
        // to form a complete name of the locale/category database
        pathname.append (db);
        path = pathname.data ();
    }

    // map the database
    return __rw_mmap (path, &impsize);
}

// Its counterpart - does the database unmapping
void __rw_release_facet_data (const void* pv, _RWSTD_SIZE_T sz)
{
    __rw_munmap (pv, sz);
}


// standard facet id's (i.e., those instantiated and installed
// by the library are in the range [1, __rw_facet::_C_last_type]
// all others (i.e., id's of all user-defined facet types or
// specializations of standard facets on user-defined types)
// are in the range (__rw_facet::_C_last_type, ...)
static _RWSTD_SIZE_T __rw_id_gen = __rw_facet::_C_last_type + 1;


_RWSTD_SIZE_T __rw_facet_id::_C_init () const
{
    // return immediately if already initialized
    if (_C_id)
        return _C_id;

    // atomically increment id generator
    // the above initialization is already guarded and the initialization
    // of standard facets has a chance of completing at the step above

    // generate a unique id
    const _RWSTD_SIZE_T new_id =
        _RWSTD_ATOMIC_PREINCREMENT (__rw_id_gen, false);

#ifndef _RWSTD_NO_MUTABLE

    _C_id = new_id;

#else   // if defined (_RWSTD_NO_MUTABLE)

    _RWSTD_CONST_CAST (__rw_facet_id*, this)->_C_id = new_id;

#endif   // _RWSTD_NO_MUTABLE

    return _C_id;
}

}   // namespace __rw
