/***************************************************************************
 *
 * rw_messages.cpp - Source for the Standard Library messages locale classes.
 *
 * $Id: //stdlib/dev/source/stdlib/messages.cpp#28 $
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


#include <new>   // for placement new

#include <string.h>    // for memcpy

#include <loc/_locale.h>
#include <loc/_messages.h>

#include <rw/_mutex.h>


#if !defined (_RWSTD_NO_NL_TYPES_H) && !defined (_RWSTD_NO_CATOPEN_CATGETS)
#  include <nl_types.h>
#else   // _RWSTD_NO_NL_TYPES_H || _RWSTD_NO_CATOPEN_CATGETS
#  include "catalog.h"
#endif   // !_RWSTD_NO_NL_TYPES_H && !_RWSTD_NO_CATOPEN_CATGETS


_RWSTD_NAMESPACE (__rw) { 


#define _RWSTD_BAD_CATD ((nl_catd)-1)


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
        void * _C_align;
        char _C_data[ sizeof(_V3_LOCALE::locale)];
    } loc;
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
    // a per-process array of catalog data structs
    static __rw_open_cat_data  catalog_buf [8];
    static __rw_open_cat_data* catalogs = catalog_buf;
    static _RWSTD_SIZE_T n_catalogs      = 0;
    static _RWSTD_SIZE_T catalog_bufsize =
        sizeof catalog_buf / sizeof *catalog_buf;
    static _RWSTD_SIZE_T largest_cat = 0;
    static int init = 0;

 
    if (!init) {
        for (_RWSTD_SIZE_T i = 0; i < catalog_bufsize; i++) {
	    catalogs[i].catd = _RWSTD_BAD_CATD;
	}
	init = 1;
    }

    if (cat == -1) {
        _RWSTD_ASSERT (0 != pcat_data);

        // append the locale and return a struct with the associated catalog

        if (pcat_data) {
             if (n_catalogs == catalog_bufsize) {

                // reallocate buffer of facet pointers
	         __rw_open_cat_data* const tmp =
		     new __rw_open_cat_data[n_catalogs * 2];
		 
		 ::memcpy (tmp, catalogs, n_catalogs * sizeof (__rw_open_cat_data));
		 
		 if (catalogs != catalog_buf)
		     delete[] catalogs;
		 
		 catalogs         = tmp;
		 catalog_bufsize *= 2;
		 for (_RWSTD_SIZE_T i = n_catalogs; i < catalog_bufsize; i++) {
		      catalogs[i].catd = _RWSTD_BAD_CATD;
		 }
		 cat = int (n_catalogs);
		::memcpy(&catalogs[cat].loc, &pcat_data->loc,
				 sizeof(_V3_LOCALE::locale));
		 catalogs[cat].catd = pcat_data->catd;
		 if (_RWSTD_STATIC_CAST (_RWSTD_SIZE_T, cat) > largest_cat)
		     largest_cat = _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, cat);
		 n_catalogs++;
	     }
	     else {
                 // find the first open slot and use it.
                 cat = 0;
                 while (catalogs[cat].catd != _RWSTD_BAD_CATD) {
                     (cat)++;
                 }
		 if (_RWSTD_STATIC_CAST (_RWSTD_SIZE_T, cat) > largest_cat)
		     largest_cat = _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, cat);
                 ::memcpy(&catalogs[cat].loc, &pcat_data->loc,
			      sizeof(_V3_LOCALE::locale));
                 catalogs[cat].catd = pcat_data->catd;
                 n_catalogs++;
	     }
	     return pcat_data;
	}
    }
    else {
        if (!pcat_data) {
	    // find struct and return it
	    if ((_RWSTD_SIZE_T)cat < catalog_bufsize)
                return &catalogs[cat];
	    return NULL;
        }
        else {
            // initialize the struct to an invalid state
 	    --n_catalogs;
	    catalogs[cat].catd = _RWSTD_BAD_CATD;
	    if ((_RWSTD_SIZE_T)cat == largest_cat) {
	        // find the next smallest valid slot
	        for (int i = cat; i >= 0; i--) {
		    if (catalogs[i].catd != _RWSTD_BAD_CATD) {
		        largest_cat = _RWSTD_STATIC_CAST (_RWSTD_SIZE_T, i);
		        break;
		    }
		}
		static const _RWSTD_SIZE_T bufsize =
                    sizeof catalog_buf / sizeof *catalog_buf;
		if ((largest_cat < bufsize / 2) && (catalogs != catalog_buf)) {

		    // when there are no more open catalogs indexed beyond
		    // second half of the statically allocated repository, copy
		    // the open catalogs back into the statically allocated
		    // repository.
		    
                    catalog_bufsize = bufsize;

		    ::memcpy (catalog_buf, catalogs,
				      catalog_bufsize * sizeof (*catalogs));
		    delete[] catalogs;
		    catalogs = catalog_buf;
		}
	    }
	}
    }
    return pcat_data;
}



// Open a message catalog and assign and return a handle for it.

int
__rw_cat_open (const _STD::string &cat_name, const _V3_LOCALE::locale &loc)
{
    
    _RWSTD_MT_STATIC_GUARD (__rw_open_cat_data);

    nl_catd catd = catopen (cat_name.c_str (), NL_CAT_LOCALE);
    if (_RWSTD_BAD_CATD == catd)
        return -1;

    int cat = -1;
    __rw_open_cat_data cat_data;
    cat_data.catd = catd;
    new (&cat_data.loc) _V3_LOCALE::locale(loc);
    __rw_manage_cat_data(cat, &cat_data);
    return cat;
}


// Get message text from catalog.
const char *__rw_get_message (int cat, int set_num, int msg_num)
{
    if (cat < 0)
        return 0;

    _RWSTD_MT_STATIC_GUARD (__rw_open_cat_data);

    __rw_open_cat_data *const pcat_data = __rw_manage_cat_data (cat, 0);
    if (pcat_data) {
        // verify 22.2.7.1.2, p3: `catalog' must be valid
        if (_RWSTD_BAD_CATD == pcat_data->catd)
            return 0;

        const char* const dflt = "";

	nl_catd catd = pcat_data->catd;
        const char* const text = catgets (catd, set_num, msg_num, dflt);

        if (text != dflt)
            return text;
    }

    return 0;
}


// Get locale to be used for character translation for this message catalog.

const _V3_LOCALE::locale&
__rw_get_locale (int cat)
{
    _RWSTD_MT_STATIC_GUARD (__rw_open_cat_data);
    _RWSTD_ASSERT (0 <= cat);
    __rw_open_cat_data* pcat_data = __rw_manage_cat_data(cat, NULL);

    _RWSTD_ASSERT (0 != pcat_data);
    return *(_RWSTD_REINTERPRET_CAST (_V3_LOCALE::locale*, &(pcat_data->loc)));
}

// Close a catalog and release its handle.

void __rw_cat_close (int cat)
{
    nl_catd catd;

    _RWSTD_MT_STATIC_GUARD (__rw_open_cat_data);
    
    // verify 22.2.7.1.2, p5: `catalog' must be valid
    _RWSTD_REQUIRES (cat >= 0, (_RWSTD_ERROR_INVALID_ARGUMENT,
				_RWSTD_FUNC ("__rw_cat_close (int cat)")));

    __rw_open_cat_data *pcat_data;
    pcat_data = __rw_manage_cat_data (cat, NULL);
    if (pcat_data) { 
        // verify 22.2.7.1.2, p5: `catalog' must be valid
        _RWSTD_REQUIRES (pcat_data->catd != _RWSTD_BAD_CATD, (_RWSTD_ERROR_INVALID_ARGUMENT,
							 _RWSTD_FUNC ("__rw_cat_close (int cat)")));
        catd = pcat_data->catd;
	catclose (catd);
	_V3_LOCALE::locale *ploc = _RWSTD_REINTERPRET_CAST (_V3_LOCALE::locale*, &pcat_data->loc);
	ploc->~locale ();
	__rw_manage_cat_data(cat, pcat_data);
    }
    else {
        // verify 22.2.7.1.2, p5: `catalog' must be valid
        _RWSTD_REQUIRES (0, (_RWSTD_ERROR_INVALID_ARGUMENT, _RWSTD_FUNC ("__rw_cat_close (int cat)")));
    }
}


}   // namespace __rw


#endif   // _RWSTD_NO_V3_LOCALE
