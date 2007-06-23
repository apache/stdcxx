/***************************************************************************
 *
 * aliases.h
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

#ifndef RWSTD_UTIL_ALIASES_H_INCLUDED
#define RWSTD_UTIL_ALIASES_H_INCLUDED

#include <string>   // for string
#include <vector>   // for vector


// get the codepage for a particular codeset
unsigned int get_codepage (const std::string&);

// case-insensitive comparison of strings
int ci_compare (const std::string&, const std::string&);


typedef std::vector<std::string> StringVector;

// aliases retrieval for locale name and for codeset name
void get_cname_aliases (const std::string&, StringVector&);
void get_lname_aliases (const std::string&, StringVector&);

// retrieves the C locale that will be used in getting the internal
// encoding for the locale if _RWSTD_NO_ISO_10646_WCHAR_T is defined
std::string get_C_encoding_locale (const std::string&);

// gets the corresponding C locale names
void get_same_encoding_C_locale (const std::string&,
                                 const std::string&,
                                 StringVector&);

// invalid value (hopefully) different from any LC_XXX constant
#define LC_INVALID_CAT  0xdeadbeef

// retrieves the names of locales that grep the pattern
char* get_installed_locales (int = LC_INVALID_CAT);

#endif   // RWSTD_UTIL_ALIASES_H_INCLUDED
