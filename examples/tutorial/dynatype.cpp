/**************************************************************************
 *
 * dynatype.cpp - Example program of map. See Class Reference Section
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
 * Copyright 2001-2005, 2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <iostream>
#include <map>
#include <typeinfo>

#include <examples.h>


// ordinary class whose objects are dynamically typed at runtime
class dynatype
{
    // member template class that encapsulates a per-specialization copy
    // of an associative container (map) used to bind a specific instance
    // of dynatype to its value
    template <class T>
    struct map {
        typedef std::map<const dynatype*, T> map_type;
        
        static map_type& get ();
    };

    // helper: removes this instance of dynatype from map
    template <class T>
    void remove () {
        map<T>::get ().erase (this);
    }

    // helper: copies one instance of dynatype to another
    template <class T>
    void copy (const dynatype &rhs) {
        // cast to <const T&> instead of <T> to avoid error on gcc 3.4.4/cygwin:
        // error: invalid static_cast from type `const dynatype' to type `int'
        *this = static_cast<const T&>(rhs);
    }

    // pointers to the helpers (do not depend on a template parameter)
    void (dynatype::*p_remove)();
    void (dynatype::*p_copy)(const dynatype&);

    // provides access to the mapped data value or throws bad_cast
    template <class T>
    T& retrieve (const T*) {
        if (map<T>::get ().end () == map<T>::get ().find (this))
            throw std::bad_cast ();
        return map<T>::get () [this];
    }

    // overload throws away const on template parameter
    template <class T>
    T& retrieve (const T*) const {
        return const_cast<dynatype*>(this)->retrieve ((T*)0);
    }

public:

    dynatype ();

    // construct a dynatype object from a value of any type
    template <class T>
    dynatype (const T&);

    // copy one dynatype object to another
    dynatype (const dynatype &rhs)
        : p_remove (rhs.p_remove),
          p_copy (rhs.p_copy) {
        (this->*p_copy)(rhs);
    }

    // assign one dynatype object to another
    dynatype& operator= (const dynatype &rhs);

    // remove `this' from the map destroy the object
    ~dynatype () {
        (this->*p_remove)();
    }

    // retrieve a reference to the concrete type from an instance of
    // dynatype throws std::bad_cast if the types don't match exactly
    template <class T>
    operator T& () {
        return retrieve ((T*)0);
    }

    // retrieve a const reference to the concrete type from an instance of
    // dynatype throws std::bad_cast if the types don't match exactly
    template <class T>
    operator const T& () const {
        return retrieve ((T*)0);
    }

    // assign a value of any type to an instance of dynatype
    template <class T>
    dynatype& operator= (const T &t);
};


// 14.7.3, p6 - explicit specializations must be defined before first use
template <>
void dynatype::
remove<void> ()
{ /* no-op */ }


template <>
void dynatype::
copy<void> (const dynatype&)
{ /* no-op */ }


// initialize with pointers to no-ops
dynatype::
dynatype ()
    : p_remove (&dynatype::remove<void>),
      p_copy (&dynatype::copy<void>)
{
}


template <class T>
typename dynatype::map<T>::map_type&
dynatype::map<T>::
get ()
{
    static map_type m;

    return m;
}


// construct a dynatype object from a value of any type
template <class T>
dynatype::
dynatype (const T &t)
    : p_remove (&dynatype::remove<T>),
      p_copy (&dynatype::copy<T>)
{
    *this = t;
}


// assign one dynatype object to another
dynatype& dynatype::
operator= (const dynatype &rhs)
{
    if (this != &rhs) {
        // remove `this' from the associated map
        (this->*p_remove)();

        // copy pointers to helpers from the right-hand-side
        p_remove = rhs.p_remove;
        p_copy   = rhs.p_copy;

        // copy value of unknown type from rhs to `*this'
        (this->*p_copy)(rhs);
    }
    return *this;
}


// assign a value of any type to an instance of dynatype
template <class T>
dynatype& dynatype::
operator= (const T &t)
{
    // remove `this' from the map of the corresponding type
    (this->*p_remove)();

    // insert `t' into the map specialized on `T'
    map<T>::get () [this] = t;

    // assign pointers to the helpers
    p_remove = &dynatype::remove<T>;
    p_copy   = &dynatype::copy<T>;

    return *this;
}


int main ()
{
    try {

        // create an instance of dynatype an initialized it with an int
        dynatype v1 = 1;

        std::cout << "static_cast<const int&>(v1 = 1) = ";
        std::cout <<  static_cast<const int&>(v1) << '\n';

        // assign a double to an instance of dynatype
        v1 = 3.14;

        std::cout << "static_cast<const double&>(v1 = 3.14) = ";
        std::cout <<  static_cast<const double&>(v1) << '\n';

        // copy construct an instance of dynatype
        dynatype v2 = v1;

        std::cout << "static_cast<const double&>(v2 = v1) = ";
        std::cout <<  static_cast<const double&>(v2) << '\n';

        // assign a const char* literal to an instance of dynatype
        const char* const literal = "abc";
        v2 = literal;

        std::cout << "static_cast<const char* const&>(v2 = \"abc\") = ";
        std::cout <<  static_cast<const char* const&>(v2) << '\n';

        // assign one instance of dynatype to another
        v1 = v2;

        std::cout << "static_cast<const char* const&>(v1 = v2) = ";
        std::cout <<  static_cast<const char* const&>(v1) << '\n';

        // create uninitialized (untyped) instances of dynatype
        dynatype v3, v4;

        // assign one uninitialized instance to another
        v3 = v4;

        // attempt to extract any value from an unitialized dynatype fails
        std::cout << "static_cast<const char&>(v3) = ";
        std::cout <<  static_cast<const char&>(v3) << '\n';
        
    }
    catch (...) {
        std::cerr << "exception\n";
    }

    return 0;
}
