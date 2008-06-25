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

#include <tuple>

#include "20.tuple.h"

/**************************************************************************/

#include <rw_driver.h>

static void
test_get ()
{
    rw_info (0, __FILE__, __LINE__, "get");

    IntTuple it (4);
    rw_assert (std::get<0> (it) == 4, __FILE__, __LINE__,
               "get<0> (it), got %d, expected 4", std::get<0> (it));
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

/*extern*/ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "[tuple.elem]",
                    "20.3.1.5  Element access",
                    run_test, "", 0);
}

