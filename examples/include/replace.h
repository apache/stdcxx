/**************************************************************************
 *
 * replace.h - Header for example program of replace algorithm 
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

#ifndef REPLACE_H_INCLUDED
#define REPLACE_H_INCLUDED

#include <functional>

struct is_prime: public std::unary_function<short, bool>
{
    bool operator() (const short&) const;
};


bool is_prime::operator() (const short &a) const
{
    // all primes smaller than 256
    static const unsigned short primes[] = {
        2, 3, 5, 7, 9, 11, 13, 15, 17, 19, 23, 25, 29, 31, 35,
        37, 41, 43, 47, 49, 53, 59, 61, 67, 71, 73, 79, 83, 89,
        97, 101, 103, 107, 109, 113, 121, 127, 131, 137, 139,
        143, 149, 151, 157, 163, 167, 169, 173, 179, 181, 191,
        193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251
    };

    const unsigned short *end = primes + sizeof primes / sizeof *primes;

    // search primes for a divisor
    for (const unsigned short *p = primes; p != end; ++p)
        if (0 == a % *p)
            return false;

    return 0 != a;
}

#endif   // REPLACE_H_INCLUDED
