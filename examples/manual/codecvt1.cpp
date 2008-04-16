/**************************************************************************
 *
 * codecvt1.cpp - Example program of codecvt facet. 
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

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <locale>
#include <numeric>

#include <cstring>      // for memcmp(), ptrdiff_t
#include <cwchar>       // for mbstate_t
#include <examples.h>


#define TMP_BUFFER_SIZE    1024

//  Japanese week days (short/abbreviated and long/normal form) in EUC-JP,
//  Shift_JIS and UTF-8 encodings;
const char wd_eucjp [] = {
    "\xc6\xfc\x0a\xc6\xfc\xcd\xcb\xc6\xfc\x0a\xb7\xee\x0a\xb7\xee\xcd"
    "\xcb\xc6\xfc\x0a\xb2\xd0\x0a\xb2\xd0\xcd\xcb\xc6\xfc\x0a\xbf\xe5"
    "\x0a\xbf\xe5\xcd\xcb\xc6\xfc\x0a\xcc\xda\x0a\xcc\xda\xcd\xcb\xc6"
    "\xfc\x0a\xb6\xe2\x0a\xb6\xe2\xcd\xcb\xc6\xfc\x0a\xc5\xda\x0a\xc5"
    "\xda\xcd\xcb\xc6\xfc\x0a"
};

const char wd_sjis [] = {
    "\x93\xfa\x0a\x93\xfa\x97\x6a\x93\xfa\x0a\x8c\x8e\x0a\x8c\x8e\x97"
    "\x6a\x93\xfa\x0a\x89\xce\x0a\x89\xce\x97\x6a\x93\xfa\x0a\x90\x85"
    "\x0a\x90\x85\x97\x6a\x93\xfa\x0a\x96\xd8\x0a\x96\xd8\x97\x6a\x93"
    "\xfa\x0a\x8b\xe0\x0a\x8b\xe0\x97\x6a\x93\xfa\x0a\x93\x79\x0a\x93"
    "\x79\x97\x6a\x93\xfa\x0a"
};

const char wd_utf8 [] = {
    "\xe6\x97\xa5\x0a\xe6\x97\xa5\xe6\x9b\x9c\xe6\x97\xa5\x0a\xe6\x9c"
    "\x88\x0a\xe6\x9c\x88\xe6\x9b\x9c\xe6\x97\xa5\x0a\xe7\x81\xab\x0a"
    "\xe7\x81\xab\xe6\x9b\x9c\xe6\x97\xa5\x0a\xe6\xb0\xb4\x0a\xe6\xb0"
    "\xb4\xe6\x9b\x9c\xe6\x97\xa5\x0a\xe6\x9c\xa8\x0a\xe6\x9c\xa8\xe6"
    "\x9b\x9c\xe6\x97\xa5\x0a\xe9\x87\x91\x0a\xe9\x87\x91\xe6\x9b\x9c"
    "\xe6\x97\xa5\x0a\xe5\x9c\x9f\x0a\xe5\x9c\x9f\xe6\x9b\x9c\xe6\x97"
    "\xa5\x0a"
};

const struct {
    const char* name;
    const char* buffer;
} locales [] = {
    { "ja_JP.UTF-8"    , wd_utf8  },
    { "ja_JP.Shift_JIS", wd_sjis  },
    { "ja_JP.EUC-JP"   , wd_eucjp }
};

std::mbstate_t init = std::mbstate_t ();

// Conversion from external representation of characters to internal
// representation of characters and back to external representation 
// and checking of the transformation correctness after round-trip.
void do_roundtrip_conversion (const char* locname, const char* buffer)
{
    typedef std::codecvt_byname<wchar_t,char,std::mbstate_t> wcodecvt_byname_t;

    wchar_t wtmp [TMP_BUFFER_SIZE];
    char    tmp  [TMP_BUFFER_SIZE];

    std::mbstate_t state = init;
    std::codecvt_base::result res;

    // create the named facet
    std::locale loc (locname);
    const wcodecvt_byname_t& cc = 
        std::use_facet<wcodecvt_byname_t> (loc);
        
    // set the pointers
    const char* cpe       = buffer;
    const char* cpen      = buffer;
    const char* cpe_limit = buffer + std::strlen (buffer);
        
    wchar_t* pi        = wtmp;
    wchar_t* pin       = wtmp;
    wchar_t* pi_limit  = wtmp + TMP_BUFFER_SIZE;
        
    // convert characters in external buffer to internal representation 
    res = cc.in (state, cpe, cpe_limit, cpen, pi, pi_limit, pin);

    // set the pointers and adjust the pi_limit pointer to after 
    // the last successfully converted character
    const wchar_t* cpi       = pi;
    const wchar_t* cpin      = pi;
    const wchar_t* cpi_limit = pin;

    char* pe        = tmp;
    char* pen       = tmp;
    char* pe_limit  = tmp + TMP_BUFFER_SIZE;

    // convert the characters in internal representation to external 
    // representation and compare the result with the original buffer
    res = cc.out (state, cpi, cpi_limit, cpin, pe, pe_limit, pen);
    std::cout << locname << " -> INT -> " << locname << '\n';

    std::cout << "  Size comparison of buffers yields "
              << (std::codecvt_base::ok == res && (cpen - cpe == pen - pe)
                  ? "equal\n" : "not equal\n");

    const int cmp = std::memcmp (cpe, pe, std::min ((cpen - cpe), (pen - pe)));
    std::cout << "  Content comparison of buffers yields "
              << (cmp ? "not equal\n" : "equal\n");
}


void do_diff_conversion () 
{
    typedef std::codecvt_byname<wchar_t,char,std::mbstate_t> wcodecvt_byname_t;
 
    wchar_t wtmp [TMP_BUFFER_SIZE];
    char    tmp  [TMP_BUFFER_SIZE];

    std::codecvt_base::result res;

    // Conversion from external representation of characters to internal
    // representation of characters using two different facets obtained 
    // from two different locales:
    // 1. External to internal: codecvt_byname facet from ja_JP.EUC-JP locale
    //    transforms the content of wd_eucjp to internal representation;
    // 2. Internal to external: codecvt_byname facet from ja_JP.UTF-8  locale
    //    transforms the internal representation of wd_eucjp to an external
    //    representation that uses UTF-8 encoding;
    // 3. Compare the size and content of the final result with size and 
    //    content of the wd_utf8 buffer.
    const std::locale loc1 ("ja_JP.EUC-JP");
    const std::locale loc2 ("ja_JP.UTF-8");

    const wcodecvt_byname_t& cc1 = std::use_facet<wcodecvt_byname_t> (loc1);
    const wcodecvt_byname_t& cc2 = std::use_facet<wcodecvt_byname_t> (loc2);
    
    std::mbstate_t state1 = init;
    std::mbstate_t state2 = init;

    const std::size_t len = std::strlen (wd_utf8);

    // set the pointers
    const char* cpe       = wd_eucjp;
    const char* cpen      = wd_eucjp;
    const char* cpe_limit = wd_eucjp + std::strlen (wd_eucjp);
        
    wchar_t* pi        = wtmp;
    wchar_t* pin       = wtmp;
    wchar_t* pi_limit  = wtmp + TMP_BUFFER_SIZE;
        
    // convert external buffer to internal representation 
    res = cc1.in (state1, cpe, cpe_limit, cpen, pi, pi_limit, pin);

    // set the pointers and adjust the pi_limit pointer to after 
    // the last converted character
    const wchar_t* cpi       = pi;
    const wchar_t* cpin      = pi;
    const wchar_t* cpi_limit = pin;

    char* pe        = tmp;
    char* pen       = tmp;
    char* pe_limit  = tmp + TMP_BUFFER_SIZE;

    res = cc2.out (state2, cpi, cpi_limit, cpin, pe, pe_limit, pen);

    std::cout << "\nEUC-JP -> INT -> UTF-8 conversion\n";
    std::cout << "  Size    comparison of buffers yields "
              << (std::codecvt_base::ok == res && std::size_t (pen - pe) == len
                  ? " " : "not ")
              << "equal\n";
    
    const int cmp =
        std::memcmp (pe, wd_utf8, std::min ((pen - pe), std::ptrdiff_t (len)));

    std::cout << "  Content comparison of buffers yields "
              << (cmp ? "not equal\n" : "equal\n");
}


int main ()
{
    try {
       for (int i = 0; i != sizeof locales / sizeof *locales; i++) 
           do_roundtrip_conversion (locales [i].name, locales [i].buffer);

       do_diff_conversion ();
    }
    catch (const std::exception &e) {
        std::cout << "Caught an exception: " << e.what () << std::endl;

        return 1;   // Indicate failure.
    }
    catch (...) {
        std::cout << "Caught an unknown exception" << std::endl;

        return 1;   // Indicate failure.
    }

    return 0;
}
