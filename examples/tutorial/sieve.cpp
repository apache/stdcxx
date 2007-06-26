/**************************************************************************
 *
 * sieve.cpp - Prime numbers sieve program.
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

#include <vector>
#include <iostream>

#include <examples.h>
    
int main ()
{

    std::cout << "Prime Sieve program, a test of vectors" << std::endl;

    // Create a sieve of bits, initially set to 1.
    const int sievesize = 100;
    std::vector<int, std::allocator<int> > sieve ((std::size_t)sievesize, 1);

    // At positions that are multiples of i, set value to zero.
    for (int i = 2; i * i < sievesize; i++)
        if (sieve[i])
            for (int j = i + i; j < sievesize; j += i)
                sieve[j] = 0;

    // Now output all the values that are still set.
    for (int j = 2; j < sievesize; j++)
        if (sieve[j]) 
            std::cout << j << " ";
    
    std::cout << std::endl << "End of Prime Sieve program" << std::endl;

    return 0;
}
