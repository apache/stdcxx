/***************************************************************************
 *
 * 20.tuple.creation.cpp - tests exercising tuple creation functions
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
 #ifndef _RWSTD_NO_EXT_CXX_0X

#include <functional>           // for reference_wrapper
#include <tuple>

#include "20.tuple.h"

/**************************************************************************/

typedef std::reference_wrapper<int>         IntRefWrap;

static void
test_make_tuple ()
{
    rw_info (0, __FILE__, __LINE__, "make_tuple");

    IntTuple it1 = std::make_tuple (1);

    typedef std::tuple<int, int>        IntTuple2;
    int i = 2;
    IntTuple2 it2 = std::make_tuple (1, i);

    typedef std::tuple<int, int, int>   IntTuple3;
    const int j = 3;
    IntTuple3 it3 = std::make_tuple (1, i, j);

}

/**************************************************************************/

static void
test_tie ()
{
    rw_info (0, __FILE__, __LINE__, "tie");

}

/**************************************************************************/

#define Big1stPart  bool, char, int, double
#define Big2ndPart  void*, UserClass

static void
test_tuple_cat ()
{
    rw_info (0, __FILE__, __LINE__, "tuple_cat");

#define Big1stPart  bool, char, int, double
#define Big2ndPart  void*, UserClass

    typedef std::tuple<Big1stPart> Big1stTuple;
    Big1stTuple bt1 (true, 'a', 256, 3.14159);

    typedef std::tuple<Big2ndPart> Big2ndTuple;
    Big2ndTuple bt2 (&bt1, UserClass ());

    //BigTuple bt (tuple_cat (bt1, bt2));
}

/**************************************************************************/

static int
run_test (int /*argc*/, char* /*argv*/ [])
{
    const void* p = _RWSTD_STATIC_CAST (const void*, &std::ignore);
    _RWSTD_UNUSED (p);

    test_make_tuple ();
    test_tie ();
    test_tuple_cat ();

    return 0;
}

#else // !_RWSTD_NO_EXT_CXX_0X

static int
run_test (int, char*[])
{
    rw_info (0, 0, __LINE__,
             "tests for C++0x tuple extension disabled");
    return 0;
}

#endif // !_RWSTD_NO_EXT_CXX_0X

/*extern*/ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "[tuple.creation]",
                    "20.3.1.3  Tuple creation functions",
                    run_test, "", 0);
}

