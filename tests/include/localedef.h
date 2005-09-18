/************************************************************************
 *
 * localedef.h - declarations of locale testsuite helpers
 *
 * $Id:$
 *
 ************************************************************************
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

#ifndef RW_LOCALEDEF_H_INCLUDED
#define RW_LOCALEDEF_H_INCLUDED


#include <testdefs.h>


#define _UNUSED_CAT 69


_TEST_EXPORT char*
rw_locales (int loc_cat, const char* grep_exp);


_TEST_EXPORT int
rw_locale (const char *args, const char *fname);


// creates a temporary directory and defines the RWSTD_LOCALE_ROOT
// environment variable to the name of the directory; if the environment
// variable RW_PUTENV is defined, defines any additional environment
// variable specified by it (by calling rw_putenv(0))
// returns the absolute pathname of the directory
#define LOCALE_ROOT_ENVAR   "RWSTD_LOCALE_ROOT"

_TEST_EXPORT const char*
rw_set_locale_root ();

// invokes localedef to create a locale database named by the last argument,
// if non-0, or in a directory specified by the RWSTD_LOCALE_ROOT environment
// variable otherwise, if it is defined, otherwise in the current working
// directory
// returns the name of the locale; successive calls to the function may
// change the contents of the character array pointed to by the pointer
// returned from prior calls
_TEST_EXPORT const char*
rw_localedef (const char*, const char*, const char*, const char*);


#endif   // RW_LOCALEDEF_H_INCLUDED
