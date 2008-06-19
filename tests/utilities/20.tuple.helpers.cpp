/***************************************************************************
 *
 * 20.tuple.helpers.cpp - tests exercising tuple helpers
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
 * Copyright 2008 Rogue Wave Software.
 * 
 **************************************************************************/

#include <tuple>

#include "20.tuple.h"

/**************************************************************************/

#include <rw_driver.h>

static void
test_tuple_size ()
{
    rw_info (0, __FILE__, __LINE__, "tuple_size");

    using std::tuple_size;

    rw_assert (tuple_size<EmptyTuple>::value == 0, __FILE__, __LINE__,
               "tuple_size<EmptyTuple>::value != 0");

    rw_assert (tuple_size<IntTuple>::value == 1, __FILE__, __LINE__,
               "tuple_size<IntTuple>::value != 1");
    rw_assert (tuple_size<ConstIntTuple>::value == 1, __FILE__, __LINE__,
               "tuple_size<ConstIntTuple>::value != 1");
    rw_assert (tuple_size<UserTuple>::value == 1, __FILE__, __LINE__,
               "tuple_size<UserTuple>::value != 1");
    rw_assert (tuple_size<NestedTuple>::value == 1, __FILE__, __LINE__,
               "tuple_size<NestedTuple>::value != 1");

    rw_assert (tuple_size<PairTuple>::value == 2, __FILE__, __LINE__,
               "tuple_size<Tuple>::value == 2");

    rw_assert (tuple_size<BigTuple>::value == BigListSize,
                __FILE__, __LINE__, "tuple_size<>::value == 0");
}

/**************************************************************************/

#include <type_traits>          // for is_same

#include <rw_any.h>

class any_t : public rw_any_t
{
private:
    enum {
        inherited,
        const_int,
        user_class,
        int_tuple
    } type_id;

public:

    template <class Type>
    any_t (Type)
        : rw_any_t (Type ()), type_id (inherited) { }

    any_t (const int)
        : rw_any_t (char ()), type_id (const_int) { }

    any_t (UserClass)
        : rw_any_t (char ()), type_id (user_class) { }

    any_t (std::tuple< int >)
        : rw_any_t (char ()), type_id (int_tuple) { }

    const char* type_name () const
    {
        return type_id == const_int? "const int":
               type_id == user_class? "UserClass":
               type_id == int_tuple? "std::tuple<int>":
               rw_any_t::type_name ();
    }
};

static void
test_tuple_element ()
{
    rw_info (0, __FILE__, __LINE__, "tuple_element");

    using std::is_same;
    using std::tuple_element;

#define TYPE_NAME(T) \
        (any_t (T ())).type_name ()

#define TEST(N, T, E) \
    typedef tuple_element<N, T>::type T ## N; \
    rw_assert (is_same<T ## N, E>::value, __FILE__, __LINE__, \
               "tuple_element<0, " #T ">::type, got type \"%s\", " \
               "expected type \"" #E "\"", TYPE_NAME (T##N))

    TEST (0, IntTuple, int);
    TEST (0, ConstIntTuple, const int);
    TEST (0, NestedTuple, std::tuple<int>);
    TEST (0, UserTuple, UserClass);

    TEST (0, PairTuple, long);
    TEST (1, PairTuple, const char*);

    TEST (0, BigTuple, bool);
    TEST (1, BigTuple, char);
    TEST (2, BigTuple, int);
    TEST (3, BigTuple, double);
    TEST (4, BigTuple, void*);
    TEST (5, BigTuple, UserClass);
}

/**************************************************************************/

static int
run_test (int /*argc*/, char* /*argv*/ [])
{
    test_tuple_size ();
    test_tuple_element ();

    return 0;
}

/*extern*/ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "[tuple.helper]",
                    "20.3.1.4  Tuple helper classes",
                    run_test, "", 0);
}

