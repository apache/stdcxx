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
#if    !defined(_RWSTD_NO_EXT_CXX_0X) \
    && !defined(_RWSTD_NO_RVALUE_REFERENCES)

#include <functional>           // for reference_wrapper
#include <tuple>

#include "20.tuple.h"

/**************************************************************************/

typedef std::reference_wrapper<int>         IntRefWrap;

static void
test_make_tuple ()
{
    rw_info (0, __FILE__, __LINE__, "make_tuple");

    std::tuple<int> it1 = std::make_tuple (1);

    int i = 2;
    std::tuple<int, int> it2 = std::make_tuple (1, i);

    const int j = 3;
    std::tuple<int, int, int> it3 = std::make_tuple (1, i, j);

    std::tuple<int&> rt1 = std::make_tuple (std::ref (i));

    std::tuple<const int&> rt2 = std::make_tuple (std::cref (i));
}

/**************************************************************************/

#include <cstring>

static void
test_tie ()
{
    rw_info (0, __FILE__, __LINE__, "tie");

    int i = 0; double d = 0.0; const char* s = 0;
    std::tie (i, std::ignore, s)
        = std::make_tuple (256, 3.14159, "string");

    rw_assert (i == 256, __FILE__, __LINE__,
               "i == 256, got false, expected true");
    rw_assert (d == 0.0, __FILE__, __LINE__,
               "d == 0.0, got false, expected true");
    rw_assert (0 == std::strcmp (s, "string"), __FILE__, __LINE__,
               "s == \"string\", got false, expected true");
}

/**************************************************************************/

static void
test_tuple_cat ()
{
    rw_info (0, __FILE__, __LINE__, "tuple_cat");

#define FirstHalf       bool, char, int, double
#define SecondHalf      void*, UserDefined

    typedef std::tuple<FirstHalf>   FirstTuple;
    FirstTuple t1 (true, 'a', 256, 3.14159);

    typedef std::tuple<SecondHalf>  SecondTuple;
    SecondTuple t2 (&t1, UserDefined ());

    typedef std::tuple<FirstHalf, SecondHalf>   WholeTuple;
    //WholeTuple t3 (tuple_cat (t1, t2));
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
                    "[tuple.creation]",
                    "20.3.1.3  Tuple creation functions",
                    run_test, "", 0);
}

