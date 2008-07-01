/***************************************************************************
 *
 * 20.tuple.cnstr.cpp - tests exercising tuple constructors and operators
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

#include <tuple>

#include "20.tuple.h"

/**************************************************************************/

static void
test_default_ctor ()
{
    rw_info (0, __FILE__, __LINE__, "default constructor");

    EmptyTuple et; _RWSTD_UNUSED (et);
    IntTuple it; _RWSTD_UNUSED (it);
    ConstIntTuple ct; _RWSTD_UNUSED (ct);
    //IntRefTuple rt; _RWSTD_UNUSED (rt); // ill-formed for references
    PairTuple pt; _RWSTD_UNUSED (pt);
    NestedTuple nt; _RWSTD_UNUSED (nt);
    BigTuple bt; _RWSTD_UNUSED (bt);

    UserClass::reset_totals ();
    UserTuple ut; _RWSTD_UNUSED (ut);

    rw_assert (1 == UserClass::n_total_def_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d default ctors, "
               "expected 1", UserClass::n_total_def_ctor_);
    rw_assert (0 == UserClass::n_total_copy_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d copy ctors, "
               "expected 0", UserClass::n_total_copy_ctor_);
}

/**************************************************************************/

static void
test_value_copy_ctor ()
{
    rw_info (0, __FILE__, __LINE__, "value copy constructor");

    int i = 1;
    IntTuple it1 (i); _RWSTD_UNUSED (it1);
    const IntTuple it2 (i); _RWSTD_UNUSED (it2);
    ConstIntTuple ct (i); _RWSTD_UNUSED (ct);
    IntRefTuple rt (i); _RWSTD_UNUSED (rt);

    NestedTuple nt (it2); _RWSTD_UNUSED (nt);

    const long l = 1;
    const char* s = "string";
    PairTuple pt (l, s);

    UserClass::reset_totals ();
    const UserClass uc;
    UserTuple ut (uc); _RWSTD_UNUSED (ut);

    rw_assert (1 == UserClass::n_total_def_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d default ctors, "
               "expected 1", UserClass::n_total_def_ctor_);
    rw_assert (1 == UserClass::n_total_copy_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d copy ctors, "
               "expected 1", UserClass::n_total_copy_ctor_);

    const bool b = true; const char c = 'a';
    const double d = 1.2; void* const p = 0;
    BigTuple bt (b, c, i, d, p, uc); _RWSTD_UNUSED (bt);
}

/**************************************************************************/

static void
test_value_move_ctor ()
{
    rw_info (0, __FILE__, __LINE__, "value move constructor");

    IntTuple it1 (1); _RWSTD_UNUSED (it1);
    IntTuple it2 (int ()); _RWSTD_UNUSED (it2);

    const IntTuple it3 (1); _RWSTD_UNUSED (it3);
    const IntTuple it4 (const IntTuple ()); _RWSTD_UNUSED (it4);

    ConstIntTuple ct1 (1); _RWSTD_UNUSED (ct1);
    ConstIntTuple ct2 (ConstIntTuple ()); _RWSTD_UNUSED (ct2);

    IntRefTuple rt2 (int ()); _RWSTD_UNUSED (rt2);

    NestedTuple nt (ct1); _RWSTD_UNUSED (nt);

    PairTuple pt (1L, "string"); _RWSTD_UNUSED (pt);

    BigTuple bt (true, 'a', 1, 1.0, (void*)0, UserClass ());
    _RWSTD_UNUSED (bt);

    UserClass::reset_totals ();
    UserTuple ut (UserClass ()); _RWSTD_UNUSED (ut);

    rw_assert (0 == UserClass::n_total_def_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d default ctors, "
               "expected 0", UserClass::n_total_def_ctor_);
    rw_assert (0 == UserClass::n_total_copy_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d copy ctors, "
               "expected 0", UserClass::n_total_copy_ctor_);
}

/**************************************************************************/

static void
test_homo_copy_ctor ()
{
    rw_info (0, __FILE__, __LINE__,
             "copy constructor (homogenous tuples)");

    EmptyTuple et1, et2 (et1);
    _RWSTD_UNUSED (et2);

    const IntTuple it1;
    IntTuple it2 (it1); _RWSTD_UNUSED (it2);

    const ConstIntTuple ct1;
    ConstIntTuple ct2 (ct1); _RWSTD_UNUSED (ct2);

    int i; const IntRefTuple rt1 (i);
    IntRefTuple rt2 (rt1); _RWSTD_UNUSED (rt2);

    const PairTuple pt1;
    PairTuple pt2 (pt1); _RWSTD_UNUSED (pt2);

    const NestedTuple nt1;
    NestedTuple nt2 (nt1); _RWSTD_UNUSED (nt2);

    UserClass::reset_totals ();
    const UserTuple ut1; UserTuple ut2 (ut1);
    _RWSTD_UNUSED (ut1);

    rw_assert (1 == UserClass::n_total_def_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d default ctors, "
               "expected 1", UserClass::n_total_def_ctor_);
    rw_assert (1 == UserClass::n_total_copy_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d copy ctors, "
               "expected 1", UserClass::n_total_copy_ctor_);

    const BigTuple bt1; BigTuple bt2 (bt1);
    _RWSTD_UNUSED (bt1); _RWSTD_UNUSED (bt2);
}

/**************************************************************************/

static void
test_homo_move_ctor ()
{
    rw_info (0, __FILE__, __LINE__,
             "move constructor (homogenous tuples)");

    EmptyTuple et (EmptyTuple ()); _RWSTD_UNUSED (et);
    IntTuple it (IntTuple ()); _RWSTD_UNUSED (it);
    ConstIntTuple ct (ConstIntTuple ()); _RWSTD_UNUSED (ct);
    PairTuple pt (PairTuple ()); _RWSTD_UNUSED (pt);
    NestedTuple nt (NestedTuple ()); _RWSTD_UNUSED (nt);
    BigTuple bt (BigTuple ());

    UserClass::reset_totals ();
    UserTuple ut (UserTuple ());
    rw_assert (0 == UserClass::n_total_def_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d default ctors, "
               "expected 0", UserClass::n_total_def_ctor_);
    rw_assert (0 == UserClass::n_total_copy_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d copy ctors, "
               "expected 0", UserClass::n_total_copy_ctor_);
}

/**************************************************************************/

static void
test_homo_copy_assign ()
{
    rw_info (0, __FILE__, __LINE__,
             "copy assignment operator (homogenous tuples)");

    EmptyTuple et1, et2; et2 = et1;
    IntTuple it1, it2; it2 = it1;
    //ConstIntTuple ct1, ct2; ct2 = ct1;  // Can't assign to const element.
    PairTuple pt1, pt2; pt2 = pt1;
    NestedTuple nt1, nt2; nt2 = nt1;
    BigTuple bt1, bt2; bt2 = bt1;

    UserClass::reset_totals ();
    UserTuple ut1, ut2; ut1 = ut2;
    rw_assert (2 == UserClass::n_total_def_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d default ctors, "
               "expected 2", UserClass::n_total_def_ctor_);
    rw_assert (0 == UserClass::n_total_copy_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d copy ctors, "
               "expected 0", UserClass::n_total_copy_ctor_);
    rw_assert (1 == UserClass::n_total_op_assign_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d assign ops, "
               "expected 1", UserClass::n_total_op_assign_);
}

/**************************************************************************/

static void
test_homo_move_assign ()
{
    rw_info (0, __FILE__, __LINE__,
             "move assignment operator (homogenous tuples)");

    EmptyTuple et1, et2; et2 = et1;
    IntTuple it1, it2; it2 = it1;
    //ConstIntTuple ct1, ct2; ct2 = ct1;  // Can't assign to const element.
    PairTuple pt1, pt2; pt2 = pt1;
    NestedTuple nt1, nt2; nt2 = nt1;
    BigTuple bt1, bt2; bt2 = bt1;

    UserClass::reset_totals ();
    UserTuple ut1, ut2; ut1 = ut2;
    rw_assert (2 == UserClass::n_total_def_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d default ctors, "
               "expected 2", UserClass::n_total_def_ctor_);
    rw_assert (0 == UserClass::n_total_copy_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d copy ctors, "
               "expected 0", UserClass::n_total_copy_ctor_);
    rw_assert (1 == UserClass::n_total_op_assign_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d assign ops, "
               "expected 1", UserClass::n_total_op_assign_);
}

/**************************************************************************/

static void
test_hetero_copy_ctor ()
{
    rw_info (0, __FILE__, __LINE__,
             "copy constructor (heterogenous tuples)");

    const int i1 = 0; const char c = 'a'; const double d = 1.2;
    void* const p = 0; const UserClass uc;
    BigTuple bt1 (i1, c, i1, d, p, uc); _RWSTD_UNUSED (bt1);

    const bool b = true; const int i2 = 'a';
    BigTuple bt2 (b, i2, i1, d, p, uc); _RWSTD_UNUSED (bt2);

    const float f = 1.2;
    BigTuple bt3 (b, c, i1, f, p, uc); _RWSTD_UNUSED (bt3);

    //UserTuple
}

/**************************************************************************/

static void
test_hetero_move_ctor ()
{
    rw_info (0, __FILE__, __LINE__,
             "move constructor (heterogenous tuples)");

    //EmptyTuple
    //IntTuple
    //ConstIntTuple;
    //PairTuple
    //NestedTuple

    //UserTuple
    // BigTuple
}

/**************************************************************************/

static void
test_hetero_copy_assign ()
{
    rw_info (0, __FILE__, __LINE__,
             "copy assignment operator (heterogenous tuples)");

    //EmptyTuple
    //IntTuple
    //ConstIntTuple;
    //PairTuple
    //NestedTuple

    //UserTuple
    // BigTuple
}

/**************************************************************************/

static void
test_hetero_move_assign ()
{
    rw_info (0, __FILE__, __LINE__,
             "move assignment operator (heterogenous tuples)");

    //EmptyTuple
    //IntTuple
    //ConstIntTuple;
    //PairTuple
    //NestedTuple

    //UserTuple
    // BigTuple
}

/**************************************************************************/

#include <rw_allocator.h>           // for UserAlloc

static void
test_alloc_ctors ()
{
    rw_info (0, __FILE__, __LINE__,
             "user-defined allocator constructors");

}

/**************************************************************************/

static int
run_test (int /*unused*/, char* /*unused*/ [])
{
    test_default_ctor ();

    test_value_copy_ctor ();
    test_value_move_ctor ();

    test_homo_copy_ctor ();
    test_homo_move_ctor ();
    test_homo_copy_assign ();
    test_homo_move_assign ();

    test_hetero_copy_ctor ();
    test_hetero_move_ctor ();
    test_hetero_copy_assign ();
    test_hetero_move_assign ();

    test_alloc_ctors ();

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
                    "[tuple.cnstr]", "20.3.1.2 Construction",
                    run_test, "", 0);
}

