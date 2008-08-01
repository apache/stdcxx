/***************************************************************************
 *
 * 20.tuple.elem.cpp - tests exercising tuple element accessors
 *
 * $Id: 20.tuple.elem.cpp 677985 2008-07-18 18:05:55Z elemings $
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may otain  a copy of
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

#include <rw_valcmp.h>

#define BOOL_VAL    true
#define CHAR_VAL    'a'
#define INT_VAL     256
#define DBL_VAL     3.14159
#define PTR_VAL     ptr_val
#define USER_VAL    user_val

static void*        ptr_val;
static UserDefined  user_val;

typedef std::tuple<bool, char, int, double, void*, UserDefined> Tuple;

static void
test_const_get (const Tuple& t)
{
    rw_info (0, __FILE__, __LINE__, "get (const)");

    const bool& b = std::get<0> (t);
    rw_assert (b == BOOL_VAL, __FILE__, __LINE__, "get<0>(t), got %s, "
               "expected %s", b? "true": "false", BOOL_VAL? "true": "false");

    const char& c = std::get<1> (t);
    rw_assert (c == CHAR_VAL, __FILE__, __LINE__,
               "get<1>(t), got \'%c\', expected \'%c\'", c, CHAR_VAL);

    const int& i = std::get<2> (t);
    rw_assert (i == INT_VAL, __FILE__, __LINE__,
               "get<2>(t), got %d, expected %d", i, INT_VAL);

    const double& d = std::get<3> (t);
    rw_assert (0 == rw_dblcmp (d, DBL_VAL), __FILE__, __LINE__,
               "get<3>(t), got %f, expected %f", d, DBL_VAL);

    void* const& p = std::get<4> (t);
    rw_assert (p == PTR_VAL, __FILE__, __LINE__,
               "get<4>(t), got %p, expected %p", p, PTR_VAL);

    const UserDefined& uc = std::get<5> (t);
    rw_assert (uc == USER_VAL, __FILE__, __LINE__,
               "get<5>(t), got %d, expected %d",
               uc.value (), USER_VAL.value ());
}

/**************************************************************************/

static void
test_get (Tuple& t)
{
    rw_info (0, __FILE__, __LINE__, "get");

    bool& b = std::get<0> (t);
    rw_assert (b == BOOL_VAL, __FILE__, __LINE__,
               "get<0>(t), got %d, expected %b", b, BOOL_VAL);
    b = !BOOL_VAL;
    rw_assert (std::get<0> (t) == !BOOL_VAL, __FILE__, __LINE__,
               "get<0>(t), got %d, expected %b", std::get<0> (t),
               !BOOL_VAL);

    char& c = std::get<1> (t);
    rw_assert (c == 'a', __FILE__, __LINE__,
               "get<1>(t), got %c, expected \'%c\'", c, CHAR_VAL);
    c = '1';
    rw_assert (std::get<1> (t) == '1', __FILE__, __LINE__,
               "get<1>(t), got %c, expected \'1\'", std::get<1> (t));

    int& i = std::get<2> (t);
    rw_assert (i == INT_VAL, __FILE__, __LINE__,
               "get<2>(t), got %d, expected %d", i, INT_VAL);
    i = INT_VAL+1;
    rw_assert (std::get<2> (t) == INT_VAL+1, __FILE__, __LINE__,
               "get<1>(t), got %d, expected %d", std::get<2> (t),
               INT_VAL+1);

    double& d = std::get<3> (t);
    rw_assert (0 == rw_dblcmp (d, DBL_VAL), __FILE__, __LINE__,
               "get<3>(t), got %f, expected %f", d, DBL_VAL);
    d = DBL_VAL*DBL_VAL;
    rw_assert (0 == rw_dblcmp (std::get<3> (t), DBL_VAL*DBL_VAL),
               __FILE__, __LINE__,
               "get<3>(t), got %f, expected %f", std::get<3> (t),
               DBL_VAL*DBL_VAL);

    void* p = std::get<4> (t);
    rw_assert (p == PTR_VAL, __FILE__, __LINE__,
               "get<4>(t), got %p, expected %p", p, PTR_VAL);
    p = &d;
    rw_assert (std::get<4> (t) == &d, __FILE__, __LINE__,
               "get<4>(t), got %p, expected %p", std::get<4> (t), &d);

    UserDefined& uc = std::get<5> (t);
    rw_assert (uc == USER_VAL, __FILE__, __LINE__,
               "get<5>(t), got %d, expected %d",
               uc.value (), USER_VAL.value ());
    uc = UserDefined (INT_VAL);
    rw_assert ((std::get<5> (t)).value () == INT_VAL, __FILE__, __LINE__,
               "get<5>(t), got %d, expected %d",
               (std::get<5> (t)).value (), INT_VAL);
}

/**************************************************************************/

static int
run_test (int /*argc*/, char* argv [])
{
    ptr_val = static_cast<void*> (argv [0]);
    Tuple t (BOOL_VAL, CHAR_VAL, INT_VAL, DBL_VAL, PTR_VAL, USER_VAL);

    test_const_get (t);
    test_get (t);

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
                    "[tuple.elem]", "20.3.1.5  Element access",
                    run_test, "", 0);
}

