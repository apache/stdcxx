/**************************************************************************
 *
 * valarray.h -- Header for valarray examples.
 *               Provides a valarray stream inserter.
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

#ifndef VALARRAY_H_INCLUDED
#define VALARRAY_H_INCLUDED


#include <valarray>
#include <ostream>

#include <examples.h>

template<class T> 
inline std::ostream&
insert_valarray (std::ostream &out, const std::valarray<T> &v)
{
    out << "[";
    for (std::size_t i = 0; i < v.size (); i++) {
        out << v [i];
        if (i < v.size () - 1)
            out << ",";
    }
    return out << "]";
}

// Non-templatized insertion operators necessary for implicit
// conversion from slice_array<T> (etc.) to valarray<T>. (the
// conversion won't work in template argument deduction).
inline std::ostream& operator<< (std::ostream &out, const std::valarray<int> &v)
{
    return insert_valarray (out, v);
}

std::ostream& operator<< (std::ostream &out, const std::valarray<bool> &v)
{
    return insert_valarray (out, v);
}

#endif   // VALARRAY_H_INCLUDED
