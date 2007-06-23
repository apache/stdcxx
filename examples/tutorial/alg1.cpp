/**************************************************************************
 *
 * alg1.cpp - Example program for STL generic algorithms that initialize 
 *            sequences.
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

#include <vector>      // for vector
#include <list>        // for list
#include <algorithm>   // for copy(), fill(), generate(), swap()
#include <string>      // for string
#include <iostream>    // for cout
#include <iterator>    // for ostream_iterator
#include <sstream>     // for stringstream

#include <examples.h>


typedef std::ostream_iterator<int, char,
                              std::char_traits<char> > int_ostrm_iter;
typedef std::ostream_iterator<std::string, char,
                              std::char_traits<char> > str_ostrm_iter;
typedef std::ostream_iterator<char, char,
                              std::char_traits<char> > char_ostrm_iter;

class iotaGen
{
public:
    iotaGen (int iv) : current (iv)
        { }
    int operator () () {
        return current++;
    }
private:
    int current;
};


// Illustrate the use of the fill and fill_n functions.
void fill_example ()
{
    std::cout << "Illustrate fill function\n";
    
    // Example 1, fill an array with initial values
    char buffer [100];
    char *bufferp = buffer;
    
    std::fill (bufferp, (bufferp + 100), '\0');
    std::fill_n (bufferp, 10, 'x');
    
    std::cout << buffer << '\n';
    
    // Example 2, use fill to initialize a std::list.
    std::list<std::string, std::allocator<std::string> > aList;
    
    std::fill_n (std::inserter (aList, aList.begin ()), 10, "empty");
    std::copy (aList.begin (), aList.end (),
               str_ostrm_iter (std::cout, " "));
    
    std::cout << '\n';
    
    // Example 3, use fill to overwrite values in a std::list.
    std::fill (aList.begin (), aList.end (), "full");
    std::copy (aList.begin (), aList.end (),
               str_ostrm_iter (std::cout, " "));
    
    std::cout << '\n';
    
    // Example 4, fill in a portion of a std::list.
    std::vector<int, std::allocator<int> > iVec (10);
    
    std::generate (iVec.begin (), iVec.end (), iotaGen (1));
    std::vector<int, std::allocator<int> >::iterator
        seven = std::find (iVec.begin (), iVec.end (), 7);
    std::fill (iVec.begin (), seven, 0);
    std::copy (iVec.begin (), iVec.end (), int_ostrm_iter (std::cout));
    
    std::cout << '\n';
}


// Illustrate the use of the copy function.
void copy_example ()
{
    std::cout << "Illustrate copy function\n";
    
    // Example 1, a simple copy.
    const char source[]  = "reprise";
    const char surpass[] = "surpass";
    char buffer [120];
    char *bufferp = buffer;
    
    std::copy (&source [0],
               source + std::char_traits<char>::length (source) + 1, bufferp);
    
    // Example 2, self copies.
    *std::copy (bufferp + 2,
                bufferp + std::char_traits<char>::length (buffer),
                bufferp) = '\0';

    std::size_t buflen = std::char_traits<char>::length (buffer) + 1;
    std::copy_backward (bufferp,
                        bufferp + buflen, bufferp + buflen + 3);
    std::copy (&surpass [0], surpass + 3, bufferp);
    
    // Example 3, copy to output.
    std::copy (bufferp, bufferp + std::char_traits<char>::length (buffer),
               char_ostrm_iter (std::cout));
    
    std::cout << '\n';
    
    // Example 4, use copy to convert type.
    std::list<char, std::allocator<char> > char_list;
    const char big[] = "big ";
    char buffer2 [200];
    char *buffer2p = buffer2;
    
    std::copy (bufferp,  bufferp + std::char_traits<char>::length (buffer),
               std::inserter (char_list, char_list.end ()));
    std::copy (&big [0], big + 4,
               std::inserter (char_list, char_list.begin ()));
    *std::copy (char_list.begin (),
                char_list.end (), buffer2p) = '\0';
    
    std::cout << buffer2 << '\n';
}


std::string generateLabel ()
{
    // Generate a label std::string of the form L_ddd.
    static int lastLabel = 0;
    std::ostringstream ost;
    
    ost << "L_" << lastLabel++;
    
    return ost.str ();
}


// Illustrate the use of the generate and genrate_n functions.
void generate_example ()
{
    std::cout << "Illustrate generate algorithm\n";
    
    // Example 1, generate a std::list of label numbers.
    std::list<std::string, std::allocator<std::string> > labelList;
    
    std::generate_n (std::inserter (labelList, labelList.begin ()),
                     4, generateLabel);  
    std::copy (labelList.begin (), labelList.end (),
               str_ostrm_iter (std::cout, " "));
    
    std::cout << '\n';
    
    // Example 2, generate an arithmetic progression.
    std::vector<int, std::allocator<int> > iVec (10);
    
    std::generate (iVec.begin (), iVec.end (), iotaGen (2));
    std::generate_n (iVec.begin (), 5, iotaGen (7));
    std::copy (iVec.begin (), iVec.end (), int_ostrm_iter (std::cout, " "));
    
    std::cout << '\n';
}


// Illustrate the use of the algorithm swap_ranges.
void swap_example ()
{
    std::cout << "Illustrate swap_ranges algorithm\n";
    
    // First make two parallel sequences.
    int data[] = {12, 27, 14, 64}, *datap = data;
    std::vector<int, std::allocator<int> > aVec (4);
    
    std::generate (aVec.begin (), aVec.end (), iotaGen (1));
    
    // Illustrate swap and iter_swap.
    std::swap (data [0], data [2]);
    std::copy (data, data + 4, int_ostrm_iter (std::cout, " "));
    
    std::cout << '\n';
    
    std::vector<int, std::allocator<int> >::iterator last =
        aVec.end ();

    --last;
    
    std::iter_swap (aVec.begin (),  last);
    std::copy  (aVec.begin (), aVec.end (),
                int_ostrm_iter (std::cout, " "));
    
    std::cout << '\n';
    
    // Now swap the entire sequence.
    std::swap_ranges (aVec.begin (), aVec.end (), datap);
    std::copy (data, data+4, int_ostrm_iter (std::cout, " "));
    
    std::cout << '\n';
    
    std::copy (aVec.begin (), aVec.end (),
               int_ostrm_iter (std::cout, " "));
    
    std::cout << '\n';
}


int main ()
{
    std::cout << "STL generic algorithms -- initialization algorithms\n";
    
    fill_example ();
    copy_example ();
    generate_example ();
    swap_example ();
    
    std::cout << "End of initialization tests\n";
    
    return 0;
}
