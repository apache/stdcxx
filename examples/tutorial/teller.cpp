/**************************************************************************
 *
 * teller.cpp - Bank teller sample program.
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

#include <queue>
#include <iostream>
#include <vector>

#include <teller.h>

//  A very unsatisfactory, but very portable implementation
//  of a linear congruential pseudorandom number generator.
//  (Apologies to D. Lehmer, D. Knuth, R. Sedgewick, et al.)
static unsigned long int lcg_seed = 0;
inline int lcg_rand (int n)
{
    lcg_seed = ((lcg_seed * 11321) + 1) % 32575;
    return (int) (((double) lcg_seed / 32575.0) * n);
}

int main ()
{
    const int numberOfTellers = 5;
    const int numberOfMinutes = 60;
    double totalWait          = 0;
    int numberOfCustomers     = 0;

    std::vector<Teller> teller (numberOfTellers);
    std::queue<Customer> line;

    for (int t = 0; t < numberOfMinutes; t++) {
        if (lcg_rand (10) < 9)
            line.push (Customer (t));
        
        for (int i = 0; i < numberOfTellers; i++) {
            if (teller[i].isFree () && !line.empty ()) {
                Customer& frontCustomer = line.front ();
                numberOfCustomers++;
                totalWait += t - frontCustomer.arrivalTime;
                teller[i].addCustomer (frontCustomer);
                line.pop ();
            }
        }
    }

    std::cout << "average wait: "
              << (totalWait / numberOfCustomers) << std::endl;

    return 0;
}




