/***************************************************************************
 *
 * 20.tuple.helpers.cpp - tests exercising tuple helpers
 *
 * $Id: 20.tuple.helpers.cpp 677985 2008-07-18 18:05:55Z elemings $
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

#include <rw_driver.h>

// compile out all test code if extensions disabled
#if    !defined (_RWSTD_NO_EXT_CXX_0X) \
    && !defined (_RWSTD_NO_RVALUE_REFERENCES)

#include <tuple>

#include "20.tuple.h"

/**************************************************************************/

static void
test_tuple_size ()
{
    rw_info (0, __FILE__, __LINE__, "tuple_size");

#define STRING(x)   #x
#define TEST(T, S)  \
    rw_assert (std::tuple_size<T>::value == S, __FILE__, __LINE__, \
               "tuple_size<" #T ">::value, got %d, expected " \
               STRING (S), std::tuple_size<T>::value)

    TEST (std::tuple<>, 0);

    TEST (std::tuple<int>, 1);
    TEST (std::tuple<const int>, 1);
    TEST (std::tuple<UserDefined>, 1);
    TEST (std::tuple<std::tuple<int> >, 1);

#undef TUPLE
#define TUPLE std::tuple<long, const char*>

    TEST (TUPLE, 2);

#undef TUPLE
#define TUPLE std::tuple<bool, char, int, double, void*, UserDefined>

    TEST (TUPLE, 6);
}

/**************************************************************************/

#include <rw/_meta_rel.h>

#include <rw_any.h>

class any_t : public rw_any_t
{
private:
    enum {
        inherited,
        const_int,
        user_defined,
        int_tuple
    } type_id;

public:

    template <class Type>
    any_t (Type)
        : rw_any_t (Type ()), type_id (inherited) { }

    any_t (const int)
        : rw_any_t (char ()), type_id (const_int) { }

    any_t (UserDefined)
        : rw_any_t (char ()), type_id (user_defined) { }

    any_t (std::tuple< int >)
        : rw_any_t (char ()), type_id (int_tuple) { }

    const char* type_name () const
    {
        return type_id == const_int? "const int":
               type_id == user_defined? "UserDefined":
               type_id == int_tuple? "std::tuple<int>":
               rw_any_t::type_name ();
    }
};

static void
test_tuple_element ()
{
    rw_info (0, __FILE__, __LINE__, "tuple_element");

#undef IS_SAME
#define IS_SAME(T,U) \
        _RW::__rw_is_same<T, U>::value

#undef TYPE_NAME
#define TYPE_NAME(T) \
        (any_t (T ())).type_name ()

#undef TEST
#define TEST(N, T, E) \
{ \
    typedef std::tuple_element<N, T>::type elem_type; \
    rw_assert (IS_SAME(elem_type, E), __FILE__, __LINE__, \
               "tuple_element<0, " #T ">::type, got type \"%s\", " \
               "expected type \"" #E "\"", TYPE_NAME (elem_type)); \
}

    TEST (0, std::tuple<int>, int)
    TEST (0, std::tuple<const int>, const int)
    TEST (0, std::tuple<std::tuple<int> >, std::tuple<int>)
    TEST (0, std::tuple<UserDefined>, UserDefined)

#undef TUPLE
#define TUPLE std::tuple<long, const char*>

    TEST (0, TUPLE, long)
    TEST (1, TUPLE, const char*)

#undef TUPLE
#define TUPLE std::tuple<bool, char, int, double, void*, UserDefined>

    TEST (0, TUPLE, bool)
    TEST (1, TUPLE, char)
    TEST (2, TUPLE, int)
    TEST (3, TUPLE, double)
    TEST (4, TUPLE, void*)
    TEST (5, TUPLE, UserDefined)
}

/**************************************************************************/

static int
run_test (int /*argc*/, char* /*argv*/ [])
{
    test_tuple_size ();
    test_tuple_element ();

    return 0;
}

#else // _RWSTD_NO_EXT_CXX_0X || _RWSTD_NO_RVALUE_REFERENCES

static int
run_test (int, char*[])
{

#if defined (_RWSTD_NO_EXT_CXX_0X)

    rw_warn (0, 0, __LINE__,
             "test disabled because _RWSTD_NO_EXT_CXX_0X is defined");

#elif defined (_RWSTD_NO_RVALUE_REFERENCES)

    rw_warn (0, 0, __LINE__,
             "test disabled because _RWSTD_NO_RVALUE_REFERENCES is "
			 "defined");

#endif

    return 0;
}

#endif // _RWSTD_NO_EXT_CXX_0X || _RWSTD_NO_RVALUE_REFERENCES

/*extern*/ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "[tuple.helper]",
                    "20.3.1.4  Tuple helper classes",
                    run_test, "", 0);
}

