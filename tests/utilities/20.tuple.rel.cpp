/***************************************************************************
 *
 * 20.tuple.rel.cpp - tests exercising tuple relational operators
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
test_eq ()
{
    rw_info (0, __FILE__, __LINE__, "operator==");

    std::tuple<> nt1, nt2;
    // special case
    rw_assert (nt1 == nt1, __FILE__, __LINE__,
               "nt1 == nt1, got false, expected true");

#undef TEST
#define TEST(expr)      \
    rw_assert (expr, __FILE__, __LINE__, #expr \
               "; got %b, expected %b", !(expr), expr)

    TEST (nt1 == nt2);

    std::tuple<int> it1 (1), it2 (1), it3 (2);
    TEST (it1 == it1);
    TEST (it1 == it2);
    TEST (!(it1 == it3));

    UserDefined ud1 (1), ud2 (2);
    std::tuple<UserDefined> ut1 (ud1), ut2 (ud1), ut3 (ud2);
    TEST (ut1 == ut1);
    TEST (ut1 == ut2);
    TEST (!(ut1 == ut3));

    std::tuple<bool, char, int, double, void*, UserDefined>
        bt1 (true, 'a', 255, 3.14159, &nt1, ud1),
        bt2 (true, 'a', 255, 3.14159, &nt1, ud1),
        bt3 (true, 'a', 256, 3.14159, &nt1, ud1);
    TEST (bt1 == bt1);
    TEST (bt1 == bt2);
    TEST (!(bt1 == bt3));
}

/**************************************************************************/

static void
test_lt ()
{
    rw_info (0, __FILE__, __LINE__, "operator<");

    std::tuple<> nt1, nt2;
    TEST (!(nt1 < nt1));
    TEST (!(nt1 < nt2));

    std::tuple<int> it1 (1), it2 (2);
    TEST (!(it1 < it1));
    TEST (it1 < it2);

    UserDefined ud1 (1), ud2 (2);
    std::tuple<UserDefined> ut1 (ud1), ut2 (ud2);
    TEST (!(ut1 < ut1));
    TEST (ut1 < ut2);

    std::tuple<long, const char*> pt1 (1234L, "string");
    TEST (!(pt1 < pt1));
    std::tuple<long, const char*> pt2 (1235L, "string");
    TEST (pt1 < pt2);
    std::tuple<long, const char*> pt3 (1234L, "strings");
    TEST (pt1 < pt3);

    std::tuple<bool, char, int, double, void*, UserDefined>
        bt1 (true, 'a', 255, 3.14159, &nt1, ud1),
        bt2 (true, 'a', 256, 3.14159, &nt1, ud1),
        bt3 (true, 'a', 255, 3.14159, &nt1, ud2);
    rw_assert (!(bt1 < bt1), __FILE__, __LINE__,
               "bt1 < bt1, got true, expected false");
    rw_assert (bt1 < bt2, __FILE__, __LINE__,
               "bt1 < bt2, got false, expected true");
    rw_assert (bt1 < bt3, __FILE__, __LINE__,
               "bt1 < bt3, got false, expected true");
}

/**************************************************************************/

static int
run_test (int /*argc*/, char* /*argv*/ [])
{
    test_eq ();
    test_lt ();

    //test_ne ();
    //test_gt ();
    //test_le ();
    //test_ge ();

    return 0;
}

#else // _RWSTD_NO_EXT_CXX_0X || _RWSTD_NO_RVALUE_REFERENCES

static int
run_test (int, char*[])
{

#if defined (_RWSTD_NO_RVALUE_REFERENCES)

    rw_warn (0, 0, __LINE__,
             "test disabled because _RWSTD_NO_EXT_CXX_OX is defined");

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
                    "[tuple.rel]", "20.3.1.6  Relational operators",
                    run_test, "", 0);
}

