/**************************************************************************
 *
 * memfunc.cpp - Example program for mem_fun and other member function
 *               pointer wrappers.
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

#if defined (__IBMCPP__)
#  include <rw/_config.h>
   // disable implicit inclusion to work around 
   // a limitation in IBM VisualAge 5 (PR #26959) 
#  ifndef _RWSTD_NO_IMPLICIT_INCLUSION
#    define _RWSTD_NO_IMPLICIT_INCLUSION 
#  endif   // _RWSTD_NO_IMPLICIT_INCLUSION
#endif   // __IBMCPP__

#include <algorithm>   // for for_each, stable_sort
#include <cstddef>     // for size_t
#include <functional>  // for greater
#include <iomanip>     // for setprecision, setw
#include <ios>         // for fixed, left, right
#include <iostream>    // for cout
#include <vector>      // for vector

#include <examples.h>


// Very large city class
class Megapolis
{
public:

    Megapolis (const char *s = "", double n = 0)
        :population(n), cityName(s) { }

    // The following function cannot return void due to limitations in 
    // some current C++ implementations. 
    virtual std::size_t byPopulation () const {
        std::cout << std::setw (12) << std::left << cityName
                  << std::setw (12) << std::left << "Megapolis"
                  << std::setw (5) << std::right
                  << std::fixed << std::setprecision (1)
                  << population << '\n';
        return 0;
    }

    bool operator< ( const Megapolis* ptr_ ) {
        return population > ptr_->population;
    }

    double population;

protected:

    const char* cityName;
};


// City and surrounding area class
struct Metropolis: public Megapolis
{
    Metropolis (const char *s = "", double n = 0)
        : Megapolis (s, n){ }

    virtual std::size_t byPopulation () const {
        std::cout << std::setw (12) << std::left <<  cityName
                  << std::setw (12) << std::left << "Metropolis"
                  << std::setw (5) << std::right
                  << std::fixed << std::setprecision (1)
                  << population << '\n';
        return 0;
    }
};


namespace std {

// specialize the standard function object for Megapolis*
template<>
struct greater<Megapolis*>
{
    typedef const Megapolis* first_argument_type;
    typedef const Megapolis* second_argument_type;
    typedef bool             result_type;

    result_type
    operator() (first_argument_type x, second_argument_type y) const {
        return y->population < x->population;
    }
};

}   // namespace std


int main ()
{
    // Create a vector of very large cities
    std::vector<Megapolis*, std::allocator<Megapolis*> > cities;

    cities.push_back (new Megapolis ("Calcutta", 12));
    cities.push_back (new Megapolis ("Tokyo", 26.8));
    cities.push_back (new Megapolis ("Delhi", 10)); 
    cities.push_back (new Megapolis ("Bombay", 15));
  
    cities.push_back (new Metropolis ("Cairo", 12));
    cities.push_back (new Metropolis ("New York", 7.5));
    cities.push_back (new Metropolis ("Los Angeles", 3.7)); 
    cities.push_back (new Metropolis ("Jakarta", 8.2));
  
    // Now use mem_fun to pass byPopulation member function
    // of Megapolis to the for_each function.
    std::cout << "City        Type        Population (millions)\n";
    std::cout << "----------- ----------- ---------------------\n";

    std::for_each (cities.begin(), cities.end(),
                   std::mem_fun (&Megapolis::byPopulation));

    std::cout << "\nAfter sorting...\n\n";

    // Sort
    std::stable_sort (cities.begin (), cities.end (),
                      std::greater<Megapolis*>());

    std::for_each (cities.begin (), cities.end (),
                   std::mem_fun (&Megapolis::byPopulation));

    // Free allocated memory
    while (!cities.empty ()) {
        delete cities.back ();
        cities.pop_back ();
    }

    return 0;
}
