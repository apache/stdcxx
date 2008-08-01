/***************************************************************************
 *
 * 20.tuple.h - framework for tuple tests
 *
 * $Id: 20.tuple.h 677985 2008-07-18 18:05:55Z elemings $
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
 * Copyright 2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#ifndef RW_20_TUPLE_H_INCLUDED
#define RW_20_TUPLE_H_INCLUDED

#include <ostream>              // for std::ostream
#include <tuple>
#include <type_traits>          // for std::decay
#include <utility>              // for std::move


// user-defined class for tuple testing (similar to UserClass)

class UserDefined
{

    int                 value_;         // some arbitrary value

public:

    // types

    typedef int         value_type;
    typedef unsigned    size_type;

    struct count_type   // number of constructors/operators called
    {
        size_type   dflt_ctor;
        size_type   copy_ctor;
        size_type   tmpl_ctor;
        size_type   move_ctor;
        size_type   copy_asgn;
        size_type   tmpl_asgn;
        size_type   move_asgn;
    };

    static count_type   actual;         // since last reset()
    static count_type   expect;         // modified by expect()


    // constructors

    UserDefined ()
        : value_ () { ++actual.dflt_ctor; }

    UserDefined (const UserDefined& src)
        : value_ (src.value_) { ++actual.copy_ctor; }

#if !defined _RWSTD_NO_RVALUE_REFERENCES

    UserDefined (UserDefined&& src)
        : value_ (std::move (src.value_))
        { ++actual.move_ctor; }

    // Requires: U is convertible to `value_type'.
    template <class U >
    explicit UserDefined (U&& u)
        : value_ (std::move (u)) { ++actual.tmpl_ctor; }

#endif   // !defined _RWSTD_NO_RVALUE_REFERENCES


    // assignment

    UserDefined& operator= (const UserDefined& src)
    {
        value_ = src.value_;
        ++actual.copy_asgn;
        return *this;
    }

    template <class U>
    UserDefined& operator= (const U& src)
    {
        value_ = src;
        ++actual.tmpl_asgn;
        return *this;
    }

#if !defined _RWSTD_NO_RVALUE_REFERENCES

    UserDefined& operator= (const UserDefined&& src)
    {
        value_ = std::move (src.value_);
        ++actual.move_asgn;
        return *this;
    }

#endif   // !defined _RWSTD_NO_RVALUE_REFERENCES


    // accessors

    value_type value () const { return value_; }


    // testing

    static void reset ()
    {
        actual.dflt_ctor = actual.copy_ctor = actual.tmpl_ctor =
        actual.move_ctor = actual.copy_asgn = actual.move_asgn =
        actual.tmpl_asgn = 0;

        expect.dflt_ctor = expect.copy_ctor = expect.tmpl_ctor =
        expect.move_ctor = expect.copy_asgn = expect.move_asgn =
        expect.tmpl_asgn = 0;
    }


    // implicit value_type conversion?
};

UserDefined::count_type
UserDefined::actual = { 0, 0, 0, 0, 0, 0, 0 };

UserDefined::count_type
UserDefined::expect = { 0, 0, 0, 0, 0, 0, 0 };

inline bool
operator== (const UserDefined& lhs, const UserDefined& rhs)
{
    return lhs.value () == rhs.value ();
}

inline bool
operator< (const UserDefined& lhs, const UserDefined& rhs)
{
    return lhs.value () < rhs.value ();
}

/**************************************************************************/

// tuple names

template <class T>
struct type_name_ { static const char* value; };

#define TYPE_NAME(T)    \
template <> const char* type_name_<T>::value = #T;

TYPE_NAME (std::tuple<>)
TYPE_NAME (std::tuple<int>)
TYPE_NAME (std::tuple<const int>)
TYPE_NAME (std::tuple<int&>)
TYPE_NAME (std::tuple<std::tuple<int> >)
TYPE_NAME (std::tuple<UserDefined>)

#undef TYPE_NAME
#define TYPE_NAME(T, S) \
template <> const char* type_name_<T>::value = S;

#undef TUPLE
#define TUPLE           std::tuple<long, const char*>
TYPE_NAME (TUPLE, "std::tuple<long, const char*>")

#undef TUPLE
#define TUPLE           \
std::tuple<bool, char, int, double, void*, UserDefined>
TYPE_NAME (TUPLE, "std::tuple<bool, char, int, double, void*, " \
                  "UserDefined>")

#undef TUPLE

#undef TYPE_NAME
#define TYPE_NAME(T)    type_name_<T>::value

/**************************************************************************/

// formatting specifiers

template <class T>
struct fmt_spec { static const char* value; };

#define FMT_SPEC(T, S)  \
template <> const char* fmt_spec<T>::value = #S;

FMT_SPEC (bool, %b)
FMT_SPEC (char, %c)
FMT_SPEC (int, %d)
FMT_SPEC (unsigned, %u)
FMT_SPEC (long, %ld)
FMT_SPEC (float, %f)
FMT_SPEC (double, %f)
FMT_SPEC (char*, %s)
FMT_SPEC (char const*, %s)
FMT_SPEC (void*, %p)
FMT_SPEC (void const*, %p)

#undef DECAY
#define DECAY(T)        typename std::decay<T>::type

// ignore cv-qualifiers and references
#undef FMT_SPEC
#define FMT_SPEC(T)     fmt_spec<DECAY (T)>::value


#endif   // define RW_20_TUPLE_H_INCLUDED
