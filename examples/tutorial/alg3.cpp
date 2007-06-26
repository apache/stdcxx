/**************************************************************************
 *
 * alg3.cpp - Sample programs for STL generic algorihtms that modify 
 *            their arguments in place.
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

#include <algorithm>    // for copy(), find(), generate(), ...
#include <functional>   // for greater
#include <iostream>     // for cout
#include <iterator>     // for ostream_iterator
#include <list>         // for list
#include <string>       // for string
#include <vector>       // for vector

#include <ctype.h>
#include <cstring>

#include <examples.h>


typedef std::ostream_iterator<int, char, std::char_traits<char> >
ostrm_iter_type;


struct iotaGenerator
{
    iotaGenerator (int iv): current (iv) { }

    int operator () () {
        return current++;
    }

private:
    int current;
};


struct RandomInteger
{
    static long seq [16];

    long operator () (long n) const {
        std::random_shuffle (seq, seq + sizeof seq / sizeof *seq);

        const long rnd =
            (seq [0] << 11) | (seq [1] << 8) | (seq [2] << 4) + seq [3];

        return rnd % n;
    }
};

long RandomInteger::seq [16] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf
};


bool isEven (int n)
{
    return 0 == (n % 2);
}

// Illustrate the use of the reverse function.
void reverse_example ()
{
    std::cout << "Illustrate std::reverse() algorithm\n";
    
    // Example 1, reversing a std::string.
    char text[] = "<Rats live on no evil star>";
    
    std::cout << text << '\n';

    std::reverse (text, text + sizeof text - 1);
    
    std::cout << text << '\n';
    
    // Example 2, reversing a std::list.
    std::list<int, std::allocator<int> > iList;
    
    std::generate_n (std::inserter (iList, iList.begin ()),
                     10, iotaGenerator (2));

    std::copy (iList.begin (), iList.end (),
               ostrm_iter_type (std::cout, " "));

    std::cout << '\n';

    std::reverse (iList.begin (), iList.end ());
    std::copy (iList.begin (), iList.end (),
               ostrm_iter_type (std::cout, " "));
    
    std::cout << '\n';
}


// Illustrate the use of the replace function.
void replace_example ()
{
    std::cout << "Illustrate std::replace() algorithm\n";
    
    // Make std::vector 0 1 2 3 4.
    std::vector<int, std::allocator<int> > numbers (11);    
    for (std::size_t i = 0; i < numbers.size (); i++)
        numbers [i] = int (i < 5U ? i : 10U - i);

    std::copy (numbers.begin (), numbers.end (),
               ostrm_iter_type (std::cout, " "));
    
    std::cout << '\n';
    
    // Replace 0 by 2.
    std::replace (numbers.begin (), numbers.end (), 3, 7);
    std::copy (numbers.begin (), numbers.end (),
               ostrm_iter_type (std::cout, " "));
    
    std::cout << '\n';
    
    // Replace even numbers by 9.
    std::replace_if (numbers.begin (),
                     numbers.end (), isEven, 9);
    
    std::copy (numbers.begin (), numbers.end (),
               ostrm_iter_type (std::cout, " "));
    
    std::cout << '\n';
    
    // Copy into a std::list, replastd::cing 9 by 3.
    int aList[] = { 2, 1, 4, 3, 2, 5 };
    int bList[6];
    int cList[6];
    
    std::replace_copy (aList, aList+6, &bList[0], 2, 7);
    std::replace_copy_if (bList, bList+6, &cList[0],
                          std::bind2nd (std::greater<int> (), 3), 8);
    
    std::copy (bList, bList + 6, ostrm_iter_type (std::cout, " "));
    std::cout << '\n';
    
    std::copy (cList, cList + 6, ostrm_iter_type (std::cout, " "));
    std::cout << '\n';
}


// Illustrate the use of the rotate function.
void rotate_example ()
{
    std::cout << "Illustrate std::rotate() algorithm\n";
    
    // Create the std::list 1 2 3 ... 10
    std::list<int, std::allocator<int> > iList;
    std::generate_n (std::inserter (iList, iList.begin ()),
                     10, iotaGenerator (1));
    
    // Find the location of the seven.
    std::list<int, std::allocator<int> >::iterator
        middle = std::find (iList.begin (), iList.end (), 7);
    
    // Now rotate around that location.
    std::rotate (iList.begin (), middle, iList.end ());
    std::copy (iList.begin (), iList.end (),
               ostrm_iter_type (std::cout, " "));
    
    std::cout << '\n';
    
    // Rotate again around the same location.
    std::list<int, std::allocator<int> > cList;
    
    std::rotate_copy (iList.begin (), middle, iList.end (),
                      std::inserter (cList, cList.begin ()));
    std::copy (cList.begin (), cList.end (),
               ostrm_iter_type (std::cout, " "));
    
    std::cout << '\n';
}


// Illustrate the use of the paration function.
void partition_example ()
{
    std::cout << "Illustrate std::partition() algorithm\n";
    
    // First make the std::vector 1 2 3 ... 10.
    std::vector<int, std::allocator<int> > numbers (10);
    
    std::generate (numbers.begin (),
                   numbers.end (), iotaGenerator (1));
    
    // Now put the odd values low, even values high.
    std::vector<int, std::allocator<int> >::iterator
        result = std::partition (numbers.begin (), numbers.end (),
                                 isEven);
    
    std::copy (numbers.begin (), numbers.end (),
               ostrm_iter_type (std::cout, " "));
    
    std::cout << "\nmiddle location " << result - numbers.begin () << '\n';
    
    // Now do a stable partition.
    std::generate (numbers.begin (),
                   numbers.end (), iotaGenerator (1));
    
    std::copy (numbers.begin (), numbers.end (),
               ostrm_iter_type (std::cout, " "));
    
    std::cout << '\n';
}


bool nameCompare (const char *a, const char *b)
{
    return std::strcmp (a, b) <= 0;
}


// Illustrate the use of the next_permutation function.
void permutation_example ()
{
    std::cout << "Illustrate std::next_permutation() algorithm\n";

    // Start with the values 1 2 3 in sequence.
    int start [] = { 1, 2, 3 };
    
    do {
        std::copy  (start, start + 3, ostrm_iter_type (std::cout, " "));
        std::cout << '\n';
    } while  (std::next_permutation (start, start + 3));
    
    const char alpha[] = "Alpha";
    const char beta[]  = "Beta";
    const char gamma[] = "Gamma";
    
    const char* names[] = { alpha, beta, gamma };
    
    typedef std::ostream_iterator<const char*, char, std::char_traits<char> >
        Iter;
    
    do {
        std::copy (names, names + 3, Iter (std::cout, " "));
        std::cout << '\n';
    } while (std::next_permutation (names, names + 3, nameCompare));
    
    std::cout << "Illustrate std::prev_permutation() algorithm\n";

    char word[] = "bela";
    
    do
        std::cout << word << ' ';
    while (std::prev_permutation (word,  &word[4]));
    
    std::cout << '\n';
}


// Illustrate the use of the inplace_merge function.
void inplace_merge_example ()
{
    std::cout << "Illustrate std::inplace_merge() algorithm\n";
    
    // First generate the numbers 0 2 4 6 8 1 3 5 7 9.
    std::vector<int, std::allocator<int> > numbers (10);
    
    for (std::size_t i = 0; i < numbers.size (); i++)
        numbers [i] = int (i < 5U ? 2U * i : 2U * (i - 5U) + 1U);

    std::copy (numbers.begin (), numbers.end (),
               ostrm_iter_type (std::cout, " "));
    std::cout << '\n';
    
    std::vector<int, std::allocator<int> >::iterator
        midvec = std::find (numbers.begin (), numbers.end (), 1);
    
    // Copy them into a std::list.
    std::list<int, std::allocator<int> > numList;
    std::copy (numbers.begin (), numbers.end (),
               std::inserter (numList, numList.begin ()));
    
    std::list<int, std::allocator<int> >::iterator
        midList = std::find (numList.begin (), numList.end (), 1);
    std::copy (numList.begin (), numList.end (),
               ostrm_iter_type (std::cout, " "));
    
    std::cout << '\n';
    
    // Now put them back together.
    std::inplace_merge (numbers.begin (), midvec, numbers.end ());
    std::inplace_merge (numList.begin (), midList, numList.end ());
    std::copy (numList.begin (), numList.end (),
               ostrm_iter_type (std::cout, " "));
    
    std::cout << '\n';
}


// Illustrate the use of the random_shuffle function.
void random_shuffle_example ()
{
    std::cout << "Illustrate std::generate() algorithm\n";

    // First make the std::vector 1 2 3 ... 10.
    std::vector<int, std::allocator<int> > numbers (10);
    
    std::generate (numbers.begin (),
                   numbers.end (), iotaGenerator (1));
    std::copy (numbers.begin (), numbers.end (),
               ostrm_iter_type (std::cout, " "));

    std::cout << "\nIllustrate std::random_shuffle() algorithm\n";

    // Randomly shuffle the elements.
    RandomInteger rnd;
    std::random_shuffle (numbers.begin (), numbers.end (), rnd);
    std::copy (numbers.begin (), numbers.end (),
               ostrm_iter_type (std::cout, " "));
    
    std::cout << '\n';
    
    // Do it again.
    std::random_shuffle (numbers.begin (), numbers.end (), rnd);
    std::copy (numbers.begin (), numbers.end (),
               ostrm_iter_type (std::cout, " "));
    
    std::cout << '\n';
}


int main ()
{
    std::cout << "STL generic algorithms -- in-place algorithms\n";
    
    reverse_example ();
    replace_example ();
    rotate_example ();
    partition_example ();
    permutation_example ();
    inplace_merge_example ();
    random_shuffle_example ();
    
    std::cout << "End of in-place algorithm sample program\n";
    
    return 0;
}
