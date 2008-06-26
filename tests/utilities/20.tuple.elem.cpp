/***************************************************************************
 *
 * 20.tuple.elem.cpp - tests exercising tuple element accessors
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

#include <rw_driver.h>
#include <rw/_defs.h>

// compile out all test code if extensions disabled
#ifndef _RWSTD_NO_EXT_CXX_0X

#include <tuple>

#include "20.tuple.h"

/**************************************************************************/

#include <rw_valcmp.h>

static void
test_get ()
{
    rw_info (0, __FILE__, __LINE__, "get");

    IntTuple it (4);
    rw_assert (std::get<0> (it) == 4, __FILE__, __LINE__,
               "get<0> (it), got %d, expected 4", std::get<0> (it));

    BigTuple bt (true, 'a', 256, 3.14159, &it, UserClass ());
    rw_assert (std::get<0> (bt) == true, __FILE__, __LINE__,
               "get<0>(bt), got %d, expected true", std::get<0> (bt));
    rw_assert (std::get<1> (bt) == 'a', __FILE__, __LINE__,
               "get<1>(bt), got %c, expected 'a'", std::get<1> (bt));
    rw_assert (std::get<2> (bt) == 256, __FILE__, __LINE__,
               "get<2>(bt), got %d, expected 256", std::get<2> (bt));
    rw_assert (0 == rw_dblcmp (std::get<3> (bt), 3.14159),
               __FILE__, __LINE__,
               "get<3>(bt), got %f, expected 3.14", std::get<3> (bt));
    rw_assert (std::get<4> (bt) == &it, __FILE__, __LINE__,
               "get<4>(bt), got %p, expected %p", std::get<4> (bt), &it);
}

/**************************************************************************/

static void
test_const_get ()
{
    rw_info (0, __FILE__, __LINE__, "get (const)");

}

/**************************************************************************/

static int
run_test (int /*unused*/, char* /*unused*/ [])
{
    test_get ();
    test_const_get ();

    return 0;
}

#else // !_RWSTD_NO_EXT_CXX_0X

static int
run_test (int, char*[])
{
    rw_warn (0, 0, __LINE__,
             "test disabled because _RWSTD_NO_EXT_CXX_0X is defined");
    return 0;
}

#endif // !_RWSTD_NO_EXT_CXX_0X

/*extern*/ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "[tuple.elem]",
                    "20.3.1.5  Element access",
                    run_test, "", 0);
}

