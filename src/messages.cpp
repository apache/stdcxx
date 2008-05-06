/***************************************************************************
 *
 * messages.cpp
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
 * Copyright 2000-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC
#include <rw/_defs.h>

#include <new>         // for placement new

#include <string.h>    // for memcpy, size_t

#include <loc/_locale.h>
#include <loc/_messages.h>

#include <rw/_mutex.h>


#if !defined (_RWSTD_NO_NL_TYPES_H) && !defined (_RWSTD_NO_CATOPEN_CATGETS)
#  include <nl_types.h>
#else   // _RWSTD_NO_NL_TYPES_H || _RWSTD_NO_CATOPEN_CATGETS
#  include "catalog.h"
#endif   // !_RWSTD_NO_NL_TYPES_H && !_RWSTD_NO_CATOPEN_CATGETS


#define _RWSTD_BAD_CATD ((nl_catd)-1)


_RWSTD_NAMESPACE (__rw) { 


// Implementation structure private to this module -- __rw_open_cat_data.
// Keeps track of the association between an open message catalog (as
// identified by the nl_catd handle returned by the C library catopen
// function) and the locale specified by the user for character translation
// in the call to messages<_CharT>::open.
// 
// Also used as a parameter to rw_get_static_mutex to ensure uniqueness
// of the mutex object used in sync'ing the threads using __rw_cat...
// (see RWSTD_MT_STATIC_GUARD below)
struct __rw_open_cat_data
{
    nl_catd catd;
    union {
        void *_C_align;
        char _C_data [sizeof (_STD::locale)];
    } loc;
};

struct __rw_open_cat_page
{
    static const size_t _C_size = 8;

    __rw_open_cat_page* _C_next; // next page
    __rw_open_cat_data _C_data [_C_size];
};


// manages a global, per-process repository of open catalogs according
// to the following table:

//        cat   _rw_open_cat_data     algorithm
//        -1         0                invalid
//        -1       non-0              append the struct into the repository and 
//                                    return a pointer to a struct containing
//                                    the new catalog and locale
//      > -1       0                  return the a pointer to a struct
//                                    containing the locale associated with
//                                    this catalog
//      > -1     non-0                find the struct containg  the locale and
//                                    catalog in the repository and invalidate
//                                    it.
//                                    Return a pointer to the struct

static __rw_open_cat_data*
__rw_manage_cat_data (int &cat,  __rw_open_cat_data *pcat_data)
{
    // a per-process array of pointers to catalog data structs
    static __rw_open_cat_data* catalog_buf [__rw_open_cat_page::_C_size];
    static __rw_open_cat_data** catalogs = 0;

    // first page of a per-process list of pages of catalog data structs
    static __rw_open_cat_page  catalog_page;

    static size_t n_catalogs      = 0;
    static size_t catalog_bufsize = sizeof catalog_buf / sizeof *catalog_buf;
    static size_t largest_cat     = 0;

    if (!catalogs) {
        
        for (size_t i = 0; i < catalog_bufsize; ++i) {
            catalog_page._C_data [i].catd = _RWSTD_BAD_CATD;
            catalog_buf [i] = &catalog_page._C_data [i];
        }

        catalogs = catalog_buf;
    }

    if (-1 == cat) {
        _RWSTD_ASSERT (0 != pcat_data);

        // append the locale and return a struct with the associated catalog

        if (pcat_data) {
             if (n_catalogs == catalog_bufsize) {

                // allocate new page of catalog data
                __rw_open_cat_page* const page =
                    new __rw_open_cat_page;

                // insert new page into singly-linked page list
                page->_C_next = catalog_page._C_next;
                catalog_page._C_next = page;

                // initialize new page
                for (size_t i = 0; i < __rw_open_cat_page::_C_size; ++i) {
                    page->_C_data [i].catd = _RWSTD_BAD_CATD;
                }

                // rwallocate buffer of catalog data pointers
                __rw_open_cat_data** const data =
                    new __rw_open_cat_data* [n_catalogs + __rw_open_cat_page::_C_size];

                memcpy (data, catalogs, n_catalogs * sizeof *data);
                 
                if (catalogs != catalog_buf)
                    delete[] catalogs;

                catalogs         = data;
                catalog_bufsize += __rw_open_cat_page::_C_size;

                for (size_t i = 0; i < __rw_open_cat_page::_C_size; ++i) {
                    catalogs [n_catalogs + i] = &page->_C_data [i];
                }

                cat = int (n_catalogs);

                catalogs [cat]->catd = pcat_data->catd;
                memcpy (&catalogs [cat]->loc, &pcat_data->loc,
                        sizeof (_STD::locale));

                if (size_t (cat) > largest_cat)
                    largest_cat = size_t (cat);

                ++n_catalogs;
            }
            else {
                // find the first open slot and use it.
                cat = 0;
                while (catalogs [cat]->catd != _RWSTD_BAD_CATD) {
                    ++cat;
                }

                catalogs [cat]->catd = pcat_data->catd;
                memcpy (&catalogs [cat]->loc, &pcat_data->loc,
                        sizeof (_STD::locale));

                if (size_t (cat) > largest_cat)
                    largest_cat = size_t (cat);

                ++n_catalogs;
            }
        }
    }
    else {

        if (0 == pcat_data) {
            // find struct and return it
            if (size_t (cat) < catalog_bufsize)
                return catalogs [cat];

            return 0;
        }

        // initialize the struct to an invalid state
        --n_catalogs;
        catalogs [cat]->catd = _RWSTD_BAD_CATD;

        if (size_t (cat) == largest_cat) {

            // find the next smallest valid slot
            largest_cat = 0;

            for (int i = cat; i >= 0; --i) {
                if (catalogs [i]->catd != _RWSTD_BAD_CATD) {
                    largest_cat = size_t (i);
                    break;
                }
            }

            static const size_t bufsize =
                sizeof catalog_buf / sizeof *catalog_buf;

            if ((largest_cat < bufsize / 2) && (catalogs != catalog_buf)) {
                // when there are no more open catalogs indexed beyond
                // second half of the static pointer repository, copy
                // the open catalog pointers back into the repository.

                catalog_bufsize = bufsize;

                memcpy (catalog_buf, catalogs,
                        catalog_bufsize * sizeof (*catalogs));

                delete[] catalogs;

                catalogs = catalog_buf;

                // remove all pages, they're not in use
                while (catalog_page._C_next)
                {
                    // remove next page from page list
                    __rw_open_cat_page* page = catalog_page._C_next;
                    catalog_page._C_next = page->_C_next;

                    // deallocate that page
                    delete page;
                }
            }
        }
    }

    return pcat_data;
}



// Open a message catalog and assign and return a handle for it.

int __rw_cat_open (const _STD::string &cat_name, const _STD::locale &loc)
{
    _RWSTD_MT_CLASS_GUARD (__rw_open_cat_data);

    const nl_catd catd = catopen (cat_name.c_str (), NL_CAT_LOCALE);
    if (_RWSTD_BAD_CATD == catd)
        return -1;

    __rw_open_cat_data cat_data;
    cat_data.catd = catd;
    new (&cat_data.loc) _STD::locale (loc);

    int cat = -1;
    __rw_manage_cat_data (cat, &cat_data);

    _RWSTD_ASSERT (0 <= cat);

    return cat;
}


// Get message text from catalog.
const char* __rw_get_message (int cat, int set_num, int msg_num)
{
    if (cat < 0)
        return 0;

    _RWSTD_MT_CLASS_GUARD (__rw_open_cat_data);

    __rw_open_cat_data *const pcat_data = __rw_manage_cat_data (cat, 0);

    if (pcat_data && _RWSTD_BAD_CATD != pcat_data->catd) {
        // 22.2.7.1.2, p3: `catalog' must be valid

        const char dflt[] = "";

        const nl_catd catd = pcat_data->catd;

        const char* const text = catgets (catd, set_num, msg_num, dflt);

        if (text != dflt)
            return text;
    }

    return 0;
}


// Get locale to be used for character translation for this message catalog.
const _STD::locale& __rw_get_locale (int cat)
{
    _RWSTD_MT_CLASS_GUARD (__rw_open_cat_data);

    _RWSTD_ASSERT (0 <= cat);
    __rw_open_cat_data* const pcat_data = __rw_manage_cat_data (cat, 0);

    _RWSTD_ASSERT (0 != pcat_data);

    return *(_RWSTD_REINTERPRET_CAST (_STD::locale*, &(pcat_data->loc)));
}


// Close a catalog and release its handle.
void __rw_cat_close (int cat)
{
    bool cat_closed = false;

    {
        // guard is local to this scope to avoid reacquiring the mutex
        // if have to generate an exception message string below.
        _RWSTD_MT_CLASS_GUARD (__rw_open_cat_data);
    
        __rw_open_cat_data* const pcat_data =
            cat < 0 ? 0 : __rw_manage_cat_data (cat, 0);

        // 22.2.7.1.2, p5: `catalog' must be valid
        if (pcat_data && pcat_data->catd != _RWSTD_BAD_CATD) {

            catclose (pcat_data->catd);

            _STD::locale* const ploc =
                _RWSTD_REINTERPRET_CAST (_STD::locale*, &pcat_data->loc);

            ploc->~locale ();

            __rw_manage_cat_data (cat, pcat_data);

            cat_closed = true;
        }
        else {
            cat_closed = false;
        }
    }

    _RWSTD_REQUIRES (cat_closed, (_RWSTD_ERROR_INVALID_ARGUMENT,
                                  _RWSTD_FUNC ("__rw_cat_close (int cat)")));
}


}   // namespace __rw
