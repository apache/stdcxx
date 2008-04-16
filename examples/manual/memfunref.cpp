/**************************************************************************
 *
 * memfunref.cpp - Example program for mem_fun and other member function
 *                 reference wrappers.
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

#include <algorithm>    // for for_each
#include <functional>   // for mem_fun_ref
#include <iostream>     // for cout, endl

#include <examples.h>
#include <memfunref.h>

typedef sublist<int>    list_type;

int main ()
{
    list_type::value_type a1[] = { 2, 1, 5, 6, 4 };
    list_type::value_type a2[] = { 11, 4, 67, 3, 14 };

    list_type s1 (a1, a1 + sizeof a1 / sizeof *a1);
    list_type s2 (a2, a2 + sizeof a2 / sizeof *a2);
    
    // Build a list of sublists
    typedef std::list<list_type, std::allocator<list_type> > list_list;

    list_list l;
    l.insert (l.begin (), s1);
    l.insert (l.begin (), s2);

    // Sort each sublist in the list
    std::for_each (l.begin (), l.end (), std::mem_fun_ref (&list_type::sort));
    
    // Display the contents of list
    std::for_each (l.begin (), l.end (), std::mem_fun_ref (&list_type::disp));

    return 0;
}

