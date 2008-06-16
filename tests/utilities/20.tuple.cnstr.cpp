/***************************************************************************
 *
 * 20.tuple.cnstr.cpp - tests exercising tuple constructors and other members
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
 * Copyright 1994-2008 Rogue Wave Software.
 * 
 **************************************************************************/

#include <tuple>

#include "20.tuple.h"

/**************************************************************************/

#include <rw_driver.h>

static void
test_default_ctor ()
{
    rw_info (0, __FILE__, __LINE__, "default constructor");

    NullTuple null_tuple;
    _RWSTD_UNUSED (null_tuple);

    IntTuple int_tuple;
    _RWSTD_UNUSED (int_tuple);

    UserClass::reset_totals ();
    UserTuple user_tuple;
    _RWSTD_UNUSED (user_tuple);
    rw_assert (UserClass::n_total_def_ctor_ == 1, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d default ctors, "
               "expected 1", UserClass::n_total_def_ctor_);

    BigTuple big_tuple;
    _RWSTD_UNUSED (big_tuple);

    NestedTuple nested_tuple;
    _RWSTD_UNUSED (nested_tuple);
}

/**************************************************************************/

static void
test_value_ctor ()
{
    rw_info (0, __FILE__, __LINE__, "value constructor");
/*
    // stored values are verified by get() tests in 20.tuple.elem
    IntTuple it1 (1);
    _RWSTD_UNUSED (it1);
    int i1 = 1;
    IntTuple it2 (i1);
    _RWSTD_UNUSED (it2);

    UserClass::reset_totals ();
    UserTuple ut1 (UserClass ());
    rw_assert (UserClass::n_total_def_ctor_ == 1, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d default ctors, "
               "expected 1", UserClass::n_total_def_ctor_);

    UserTuple ut1 (UserClass ());
    _RWSTD_UNUSED (ut1);

    // test constructor signature for reference and non-reference types
    // test return type for reference and non-reference types
*/
}

/**************************************************************************/

static int
run_test (int /*unused*/, char* /*unused*/ [])
{
    test_default_ctor ();
    test_value_ctor ();

    //test_homogenous_copy_ctor ();
    //test_heterogenous_copy_ctor ();
    //test_pair_copy_ctor ();

    //test_homogenous_copy_assignment ();
    //test_heterogenous_copy_assignment ();

    //test_pair_copy_ctor ();
    //test_pair_move_ctor ();
    //test_pair_copy_assign ();
    //test_pair_move_assign ();

    //test_alloc_ctors ();

    return 0;
}

/*extern*/ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "[tuple.cnstr]", "20.3.1.2 Construction",
                    run_test, "", 0);
}

