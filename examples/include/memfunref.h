/**************************************************************************
 *
 * memfunref.h - Header for example program for mem_fun and other 
 * member function reference wrappers.  See Class Reference Section
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

#ifndef MEMFUNREF_H_INCLUDED
#define MEMFUNREF_H_INCLUDED


#include <algorithm>   // for copy()
#include <list>        // for list
#include <iterator>    // for ostream_iterator
#include <iostream>    // for cout, ostream


template <class T>
class sublist  
{
    typedef std::list<T, std::allocator<T> >    list_type;

public:

    typedef typename list_type::value_type      value_type;
    typedef typename list_type::reference       reference;
    typedef typename list_type::const_reference const_reference;
    typedef typename list_type::pointer         pointer;
    typedef typename list_type::const_pointer   const_pointer;
    typedef typename list_type::difference_type difference_type;

    sublist () { }

    sublist (const_pointer begin, const_pointer end): l (begin, end) { }

    int sort () {
        l.sort ();
        return 1;
    }

    int disp () const {
        typedef std::char_traits<char> traits_type;
        typedef std::ostream_iterator<value_type, char, traits_type> os_iter;

        std::copy (l.begin (), l.end (), os_iter (std::cout, " "));
        std::cout << std::endl;

        return 1;
    }

private:
    list_type l;
};


#endif   // MEMFUNREF_H_INCLUDED
