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

    EmptyTuple et; _RWSTD_UNUSED (et);
    IntTuple it; _RWSTD_UNUSED (it);
    ConstIntTuple ct; _RWSTD_UNUSED (ct);
    PairTuple pt; _RWSTD_UNUSED (pt);
    NestedTuple nt; _RWSTD_UNUSED (nt);
    BigTuple bt; _RWSTD_UNUSED (bt);

    UserClass::reset_totals ();
    UserTuple ut; _RWSTD_UNUSED (ut);

    rw_assert (UserClass::n_total_def_ctor_ == 1, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d default ctors, "
               "expected 1", UserClass::n_total_def_ctor_);
}

/**************************************************************************/

static void
test_value_copy_ctor ()
{
    rw_info (0, __FILE__, __LINE__, "value copy constructor");

    const int i = 1;
    const IntTuple it (i);
    ConstIntTuple ct (i); _RWSTD_UNUSED (ct);
    NestedTuple nt (it);

    const long l = 1;
    const char* s = "string";
    PairTuple pt (l, s);

    UserClass::reset_totals ();
    const UserClass uc;
    UserTuple ut (uc); _RWSTD_UNUSED (ut);

    rw_assert (1 == UserClass::n_total_def_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d default ctors, "
               "expected 1", UserClass::n_total_def_ctor_);

    const bool b = true; const char c = 'a';
    const double d = 1.2; void* const p = 0;
    BigTuple bt (b, c, i, d, p, uc); _RWSTD_UNUSED (bt);
}

/**************************************************************************/

static void
test_value_move_ctor ()
{
    rw_info (0, __FILE__, __LINE__, "value move constructor");

}

/**************************************************************************/

static void
test_homo_copy_ctor ()
{
    rw_info (0, __FILE__, __LINE__, "copy constructor (homogenous tuples)");

    EmptyTuple et1, et2 (et1);
    _RWSTD_UNUSED (et2);

    const IntTuple it1;
    IntTuple it2 (it1); _RWSTD_UNUSED (it2);

    const ConstIntTuple ct1;
    ConstIntTuple ct2 (ct1); _RWSTD_UNUSED (ct2);

    PairTuple pt1;
    PairTuple pt2 (pt1); _RWSTD_UNUSED (pt2);

    const NestedTuple nt1;
    NestedTuple nt2 (nt1); _RWSTD_UNUSED (nt2);

    UserClass::reset_totals ();
    const UserTuple ut1; UserTuple ut2 (ut1);
    _RWSTD_UNUSED (ut1);

    rw_assert (UserClass::n_total_def_ctor_ == 1, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d default ctors, "
               "expected 1", UserClass::n_total_def_ctor_);

    const BigTuple bt1; BigTuple bt2 (bt1);
    _RWSTD_UNUSED (bt1); _RWSTD_UNUSED (bt2);
}

/**************************************************************************/

static int
run_test (int /*unused*/, char* /*unused*/ [])
{
    test_default_ctor ();

    test_value_copy_ctor ();
    test_value_move_ctor ();

    test_homo_copy_ctor ();
    //test_homo_move_ctor ();
    //test_homo_copy_assign ();
    //test_homo_move_assign ();

    //test_hetero_copy_ctor ();
    //test_hetero_move_ctor ();
    //test_hetero_copy_assign ();
    //test_hetero_move_assign ();

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

