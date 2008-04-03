/***************************************************************************
 *
 * rw_setlocale.cpp - implementation of the __rw_setlocale class
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
 * Copyright 2001-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#include <rw/_defs.h>

#if defined (__linux__) && !defined (_XOPEN_SOURCE)
   // need S_IFDIR on Linux
#  define _XOPEN_SOURCE 
#endif   // __linux__ && !_XOPEN_SOURCE

#include <locale.h>   // for setlocale()
#include <stdlib.h>   // for getenv()
#include <string.h>   // for memcpy(), strcmp()
#include <time.h>     // for struct tm

#include <rw/_error.h>
#include <rw/_mutex.h>

#include "podarray.h"   // for __rw_charray
#include "setlocale.h"

#ifndef _MSC_VER
#  include <limits.h>   // for PATH_MAX
#  if defined (__INTERIX) && !defined (NAME_MAX)
#    define NAME_MAX 255
#  endif   // __INTERIX && !NAME_MAX
#else   // if defined (_MSC_VER)
#  include <windows.h>
#endif   // _MSC_VER

#include <sys/stat.h>


#ifndef PATH_MAX
#  ifdef MAX_PATH
#    define PATH_MAX   MAX_PATH
#  else   // if defined (MAX_PATH)
#    define PATH_MAX   1024
#  endif   // MAX_PATH
#endif   // PATH_MAX


_RWSTD_NAMESPACE (__rw) { 

// "setlocale" synchronization (internal, not exported)
__rw_mutex __rw_setlocale_mutex;

// ctor changes the C library locale, saving the old locale to be 
// restored by the dtor
__rw_setlocale::__rw_setlocale (const char *locname, int cat, int nothrow)
    : _C_name (0), _C_guard (1), _C_cat (cat)
{
    // acquire global per-process lock
    __rw_setlocale_mutex._C_acquire ();

    // retrieve previous locale name and check if it is already set
    const char* const curname = ::setlocale (cat, 0);

    if (curname) {

        // try to optimize away the following potential
        // memory allocation if the locale is already set
        if (locname && !nothrow && !::strcmp (locname, curname))
            return;

        // save the name of the current locale before making the call below
        // even though it may fail, since a successful call may change the
        // string pointed to by curname
        const _RWSTD_SIZE_T size = ::strlen (curname) + 1;
        _C_name = size <= sizeof _C_namebuf ? _C_namebuf : new char [size];

        ::memcpy (_C_name, curname, size);

        if (::setlocale (cat, locname)) {

            // a NULL name is only used when we want to use the object
            // as a retriever for the name of the current locale; 
            // no need for a lock then
            if (!locname) {
                _C_guard = 0;
                __rw_setlocale_mutex._C_release ();
            }

            return;
        }

        if (_C_name != _C_namebuf)
            delete[] _C_name;

        _C_name = 0;
    }

    // bad locales OR bad locale names OR bad locale categories
    _C_guard = 0;
    __rw_setlocale_mutex._C_release ();

    // failure in setting the locale
    _RWSTD_REQUIRES (nothrow, (_RWSTD_ERROR_LOCALE_BAD_NAME,
                               _RWSTD_FUNC ("std::setlocale(const char*)"),
                               locname));
}


// dtor restores the C library locale that 
// was in effect when the object was constructed
__rw_setlocale::~__rw_setlocale ()
{
    // if guard is set, constructor changed the locale
    if (_C_guard) {

        // restore the locale
        ::setlocale (_C_cat, _C_name);

        // release the lock
        __rw_setlocale_mutex._C_release ();
    }

    if (_C_name != _C_namebuf)
        delete [] _C_name;
}


char* __rw_locale_name (int cat, const char *locname, __rw_chararray &namebuf)
{
    static char locale_root [PATH_MAX];

    _RWSTD_ASSERT (0 != locname);

    if (!*locale_root) {

#ifndef _RWSTD_LOCALE_ROOT_ENVVAR
#  define _RWSTD_LOCALE_ROOT_ENVVAR "RWSTD_LOCALE_ROOT"
#endif

        const char* const envvar = getenv (_RWSTD_LOCALE_ROOT_ENVVAR);

        if (envvar && strlen (envvar) < sizeof locale_root)
            strcpy (locale_root, envvar);
    }

    // construct the pathname of the locale database
    char locpath [PATH_MAX];
    *locpath = '\0';

    _RWSTD_SIZE_T namelen = strlen (locname);

    _RWSTD_SIZE_T pathlen = 0;

    if (*locale_root) {

        if (namelen + strlen (locale_root) + 1 < sizeof locpath) {
            strcpy (locpath, locale_root);

            pathlen = strlen (locpath);

            locpath [pathlen++] = _RWSTD_PATH_SEP;
            locpath [pathlen]   = '\0';
        }
    }

    if (strlen (locpath) + namelen < sizeof locpath) {

        strcpy (locpath + pathlen, locname);

        // see if the directory containing the locale database exists
        struct stat sb;
        if (0 == stat (locpath, &sb) && sb.st_mode & S_IFDIR) {

            namebuf.assign (locname);

            return namebuf.data ();
        }
    }

    // if the named locale is not found in the locale directory
    // try to see if the locale refers to a libc locale
    const __rw_setlocale tmp (locname, cat, 1 /* nothrow */);

    if (!tmp.name ())
        return 0;

    // get the libc locale name in case locname is an abbreviation
    const char* const fullname = setlocale (cat, locname);

    _RWSTD_ASSERT (0 != fullname);

    namebuf.assign (fullname);

    return namebuf.data ();
}


}   // namespace __rw
