/**************************************************************************
 *
 * alg5.cpp - Example programs for STL generic algorithms those 
 *            producing scalar values.
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
 
#include <algorithm>    // for copy
#include <functional>   // for multiplies
#include <iterator>     // for ostream_iterator
#include <list>         // for list
#include <numeric>      // for accumulate
#include <iostream>     // for cout, endl
   
#include <examples.h>

typedef std::ostream_iterator<int, char, std::char_traits<char> >
        ostrm_iter_type;

// Forward declarations.
bool isVowel (char);
void count_example ();
void accumulate_example ();
void inner_product_example ();
void equal_example ();


bool isVowel (char c) {
    
    switch (c)
    {
        case 'a': case 'A':
        case 'e': case 'E':
        case 'i': case 'I':
        case 'o': case 'O':
        case 'u': case 'U':
            return true;
    }
    return false;
}


// Illustrate the use of the count function.
void count_example () {
    
    int ecount     = 0;
    int vowelCount = 0;
    
    char text[] = "Now is the time to begin";
    
#ifdef _RWSTD_STRICT_ANSI

    ecount = std::count (text, text + sizeof text, 'e');
    vowelCount = std::count_if (text, text + sizeof text, isVowel);

#else

    std::count (text, text + sizeof text, 'e', ecount);
    std::count_if (text, text + sizeof text, isVowel, vowelCount);

#endif   // _RWSTD_STRICT_ANSI
    
    std::cout << "There are "           << ecount
              << " letter e's "         << std::endl
              << "and "                 << vowelCount
              << " vowels in the text:" << text
              << std::endl;
}


// Add n to 1 to std::list.
std::list<int, std::allocator<int> >&
intReplicate (std::list<int, std::allocator<int> >& nums, int n) {

    while (n)
        nums.push_back (n--);
    return nums;
}


// Illustrate the use of the accumulate function.
void accumulate_example () {
    
    int numbers[] = { 1, 2, 3, 4, 5 };
    int sum     = std::accumulate (numbers, numbers+5, 0);
    int product = std::accumulate (numbers,
                                   numbers+5, 1, std::multiplies<int> ());

    std::cout << "The sum of the first five numbers is "
              << sum
              << std::endl;
    std::cout << "The product of the first five numbers is "
              << product
              << std::endl;

    // Example with different types for init.
    std::list<int, std::allocator<int> > nums;
    nums = std::accumulate (numbers, numbers+5, nums, intReplicate);

    std::copy (nums.begin (), nums.end (), ostrm_iter_type (std::cout, " ")); 

    std::cout << std::endl;
}


// Illustrate the use of the inner_product function.
void inner_product_example () {
    
    int a[] = { 4, 3, -2 };
    int b[] = { 7, 3, 2  };

    // Example 1, simple inner product.
    int in1 = std::inner_product (a, a+3, b, 0);
    std::cout << "Inner product is " << in1 << std::endl;

    // Example 2, using different operations.
    bool anyequal = std::inner_product (a, a+3, b, true,
                                        std::logical_or<bool> (),
                                        std::equal_to<int> ());

    std::cout << "any equal? " << anyequal << std::endl;  
}


// Illustrate the use of the equal function.
void equal_example () {
    
    int a[] = { 4, 5, 3 };
    int b[] = { 4, 3, 3 };
    int c[] = { 4, 5, 3 };
    
    std::cout << "a = b is:" << std::equal (a, a+3, b) << std::endl;
    std::cout << "a = c is:" << std::equal (a, a+3, c) << std::endl;
    std::cout << "a pair-wise-greater_equal b is"
              << std::equal (a, a+3, b, std::greater_equal<int> ())
              << std::endl;
}


// Illustrate the use of the lexical_comparison function.
void lexical_comparison_example () {

    char wordOne[] = "everything";
    char wordTwo[] = "everybody";
    
    std::cout << "compare everybody to everything "
              << std::lexicographical_compare (wordTwo,
                                               wordTwo + sizeof wordTwo,
                                               wordOne,
                                               wordOne + sizeof wordOne)
              << std::endl;
            
    int a[] = { 3, 4, 5, 2 };
    int b[] = { 3, 4, 5 };
    int c[] = { 3, 5 };
    
    std::cout << "compare a to b: "
              << std::lexicographical_compare (a, a+4, b, b+3) << std::endl;
    std::cout << "compare a to c: "
              << std::lexicographical_compare (a, a+4, c, c+2) << std::endl;
}

int main () {
    
    std::cout << "STL generic algorithms -- "\
              << "algorithms that produce scalar results" << std::endl;

    count_example ();
    accumulate_example ();
    inner_product_example ();
    equal_example ();
    lexical_comparison_example ();
    
    std::cout << "End of scalar algorithms test"  << std::endl;

    return 0;
}
