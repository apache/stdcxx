/**************************************************************************
 *
 * num_put.cpp - Example program for the num_put facet. 
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

#include <iostream>   // for cout, endl
#include <iterator>   // for ostreambuf_iterator
#include <locale>     // for time_put

#include <examples.h>


// custom numeric punctuation facet
struct Punct: std::numpunct<char>
{
    char do_decimal_point () const {
        return ';';
    }

    string_type do_truename () const {
        return "numeric formatting";
    }

    char do_thousands_sep () const {
        return ':';
    }

    std::string do_grouping () const {
        return "\1\2\3";
    }
};


int main ()
{
    // construct a custom punctuation facet
    std::numpunct<char> *punct = new Punct;

    // construct a locale containing the custom facet
    const std::locale loc (std::cout.getloc (), punct);

    // imbue combined locale in cout
    std::cout.imbue (loc);

    // retrieve the standard numeric formatting facet
    const std::num_put<char> &nput =
        std::use_facet<std::num_put<char> >(std::cout.getloc ());
 
    // obtain a stream buffer iterator operating on cout's buffer
    std::ostreambuf_iterator<char> it = std::cout.rdbuf ();

    // write out bool values as strings
    std::cout.setf (std::ios::boolalpha);
    *nput.put (it, std::cout, ' ', true) = '\n';

    // set the field width and justification for the next operation
    std::cout.width (18);
    std::cout.setf (std::ios::fixed | std::ios::right);

    const double Pi = 3.14159265358979323846;

    // truncate Pi to an integer and write it out, padded on the left with periods
    *nput.put (it, std::cout, '.', long (Pi)) = '\n';

    // write out a number in hex notation using capital letters
    std::cerr.width (13);
    std::cerr.setf (std::ios::right | std::ios::uppercase);
    std::cerr.setf (std::ios::hex, std::ios::basefield);
    *nput.put (it, std::cerr, ' ', 3735928559UL) = '\n';

    std::cout.width (18);
    std::cout.setf (std::ios::internal, std::ios::adjustfield);
    std::cout.setf (std::ios::showpos);
    
    // write Pi out padded after the sign with spaces
    *nput.put (it, std::cout, ' ', Pi) = '\n';

    // write Pi out again, multiplied by a large number (to see grouping)
    *nput.put (it, std::cout, ' ', Pi * 1000000) = '\n';

    // and again, in scientific notation
    std::cout.precision (11);
    std::cout.setf (std::ios::scientific, std::ios::floatfield);
    *nput.put (it, std::cout, ' ', Pi * 1000000) = '\n';

    return 0;
}
