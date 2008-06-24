/***************************************************************************
 *
 * catalog.h - Declarations for XPG4 messages
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _RWSTD_CATALOG_H_INCLUDED
#define _RWSTD_CATALOG_H_INCLUDED

#include <map>
#include <string>

#include <rw/_defs.h>

#ifdef _WIN32
#  define _RWSTD_USE_RC 1
#  include <windows.h>
#endif


#define NL_SETD                 1    /* XPG3 Conformant Default set number. */
#define NL_CAT_LOCALE           (-1) /* XPG4 requirement */
#define NL_CAT_LOCALE           (-1) /* XPG4 requirement */

#define _RWCAT_COOKIE            0xAB34BC56


_RWSTD_NAMESPACE (__rw) { 


#ifndef _RWSTD_USE_RC

// dummy implementation (does nothing)
class __rw_cat
{
public:
    __rw_cat(const char*, int) { }

    ~__rw_cat() { }

    __rw_cat* id () {
        return this;
    }

    bool good () const {
        return false;
    }

    const char* catgets (int, int, const char *__s) {
        return __s;
    }
};

#else   // if defined (_RWSTD_USE_RC)

#define NL_TEXTMAX 8024 

// Catalog type hold relationship between cat id and file
class __rw_cat
{
public:
    typedef _STD::map<int, _STD::string, _STD::less<int>,
                      _STD::allocator<_STD::pair<const int, _STD::string> > >
    __cat_type;

    typedef __cat_type::value_type __msg_type;
    
protected:
    HINSTANCE __cat;
    __cat_type __msgs;

public:
    __rw_cat (const char *__fname, int)
        : __cat (__fname ? LoadLibrary (__fname) : GetModuleHandle (0)) { }

    ~__rw_cat () {
        if (__cat)
            FreeLibrary (__cat);
    }

    __rw_cat* id () {
        return this;
    }

    bool good () {
        return !!__cat;
    }

    HINSTANCE cat () {
        return __cat;
    }

    const char* catgets (int, int, const char*);
};


#endif   //_RWSTD_USE_RC


}   // namespace __rw


typedef _RW::__rw_cat* nl_catd;
typedef int            nl_item;

int         catclose (nl_catd);
const char* catgets (nl_catd, int, int, const char*);
nl_catd     catopen (const char*, int);


#endif  // _RWSTD_CATALOG_H_INCLUDED
