/**************************************************************************
 *
 * codecvte.h - Example of user defined codecvt facet. 
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

#ifndef CODECVTE_H_INCLUDED
 
#include <locale>       // for codecvt
#include <cwchar>       // for mbstate_t, size_t

#include <examples.h>


#define RWSTD_TABLE_SIZE 59
 
// This facet performs a conversion from  Latin Alphabet No. 1 
// (ISO 8859-1) to U.S.  ASCII code page 437.  Some conversions are
// one way (from ISO to ASCII, but not back again) because this ASCII 
// code page has no equivilent to the ISO character.

class ex_codecvt: public std::codecvt<char, char, std::mbstate_t>
{
    static const char table_[RWSTD_TABLE_SIZE][3];
                           
protected:

    virtual result
    do_in (std::mbstate_t&,
           const char*  from,
           const char*  from_end,
           const char* &from_next,
           char*        to,
           char*        to_limit,
           char*       &to_next) const {

        const std::size_t from_size = std::size_t (from_end - from);
        const std::size_t to_size   = std::size_t (to_limit - to);
        
        std::size_t i = from_size < to_size ? from_size : to_size;

        from_next = from;
        to_next   = to;

        for (std::size_t j = 0; j < i; ++j, ++from_next, ++to_next) {

            *to_next = *from_next;

            for (int k = 0; k < RWSTD_TABLE_SIZE; ++k) {  
                if (   *from_next >= table_[k][0]
                    && *from_next <= table_[k][1]) {
                    *to_next = table_[k][2];
                    break;
                }
            }
        }

        return ok;
    }

    virtual result
    do_out (std::mbstate_t&,
            const char*  from,
            const char*  from_end,
            const char* &from_next,
            char*        to,
            char*        to_limit,
            char*       &to_next) const {

        const std::size_t from_size = std::size_t (from_end - from);
        const std::size_t to_size   = std::size_t (to_limit - to);
        
        std::size_t i = from_size < to_size ? from_size : to_size;

        from_next = from;
        to_next   = to;

        for (std::size_t j = 0; j < i; ++j, ++from_next, ++to_next) {

            *to_next = *from_next;

            for (std::size_t k = 0; k < RWSTD_TABLE_SIZE; ++k) {  
                if (   *from_next == table_[k][2]
                    && table_[k][0] == table_[k][1]) {
                    *to_next = table_[k][1];
                    break;
                }
            }
        }

        return ok;
    }

    virtual bool do_always_noconv () const _THROWS (()) {
        return false;
    }

    virtual int do_encoding () const _THROWS (()) {
        return 1;
    }
};

 
const char ex_codecvt::table_[RWSTD_TABLE_SIZE][3] = {
    { '\xa2', '\xa2', '\x9b' }, { '\xa3', '\xa3', '\x9c' },
    { '\xa5', '\xa5', '\x9d' }, { '\xa7', '\xa7', '\x15' },
    { '\xa8', '\xa8', '\x22' }, { '\xaa', '\xaa', '\xa6' },
    { '\xab', '\xab', '\xae' }, { '\xb5', '\xb5', '\xe6' },
    { '\xb6', '\xb6', '\x14' }, { '\xb7', '\xb7', '\xfa' },
    { '\xba', '\xba', '\xa7' }, { '\xbb', '\xbb', '\xaf' },
    { '\xbc', '\xbc', '\xac' }, { '\xbd', '\xbd', '\xab' },
    { '\xbf', '\xbf', '\xa8' }, { '\xc0', '\xc3', '\x41' },
    { '\xc4', '\xc4', '\x8e' }, { '\xc5', '\xc5', '\x41' },
    { '\xc6', '\xc6', '\x92' }, { '\xc7', '\xc7', '\x80' },
    { '\xc8', '\xc8', '\x45' }, { '\xc9', '\xc9', '\x90' },
    { '\xca', '\xcb', '\x45' }, { '\xcc', '\xcf', '\x49' },
    { '\xd1', '\xd1', '\xa5' }, { '\xd2', '\xd5', '\x4f' },
    { '\xd6', '\xd6', '\x99' }, { '\xd8', '\xd8', '\xed' },
    { '\xd9', '\xdb', '\x55' }, { '\xdc', '\xdc', '\x9a' },
    { '\xdd', '\xdd', '\x59' }, { '\xdf', '\xdf', '\xe1' },
    { '\xe0', '\xe0', '\x85' }, { '\xe1', '\xe1', '\xa0' },
    { '\xe2', '\xe2', '\x83' }, { '\xe3', '\xe3', '\x61' },
    { '\xe4', '\xe4', '\x84' }, { '\xe5', '\xe5', '\x86' },
    { '\xe6', '\xe6', '\x91' }, { '\xe7', '\xe7', '\x87' },
    { '\xe8', '\xe8', '\x8a' }, { '\xe9', '\xe9', '\x82' },
    { '\xea', '\xea', '\x88' }, { '\xeb', '\xeb', '\x89' },
    { '\xec', '\xec', '\x8d' }, { '\xed', '\xed', '\xa1' },
    { '\xee', '\xee', '\x8c' }, { '\xef', '\xef', '\x8b' },
    { '\xf1', '\xf1', '\xa4' }, { '\xf2', '\xf2', '\x95' },
    { '\xf3', '\xf3', '\xa2' }, { '\xf4', '\xf4', '\x93' },
    { '\xf5', '\xf5', '\x6f' }, { '\xf6', '\xf6', '\x94' },
    { '\xf9', '\xf9', '\x97' }, { '\xfa', '\xfa', '\xa3' },
    { '\xfb', '\xfb', '\x96' }, { '\xfc', '\xfc', '\x81' },
    { '\xff', '\xff', '\x98' }
};

#endif   // CODECVTE_H_INCLUDED
