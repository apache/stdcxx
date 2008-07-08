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
#if    !defined (_RWSTD_NO_EXT_CXX_0X) \
    && !defined(_RWSTD_NO_RVALUE_REFERENCES)

#include <cstring>              // for strcmp
#include <tuple>

#include <rw_valcmp.h>          // for rw_dblcmp

#include "20.tuple.h"

/**************************************************************************/

static void
test_default_ctor ()
{
    rw_info (0, __FILE__, __LINE__, "default constructor");

    EmptyTuple et; _RWSTD_UNUSED (et);
    IntTuple it; _RWSTD_UNUSED (it);
    ConstIntTuple ct; _RWSTD_UNUSED (ct);
    // ill-formed for tuples with element types containing references
    PairTuple pt; _RWSTD_UNUSED (pt);
    NestedTuple nt; _RWSTD_UNUSED (nt);
    BigTuple bt; _RWSTD_UNUSED (bt);

    UserClass::reset_totals ();
    UserTuple ut; _RWSTD_UNUSED (ut);

    rw_assert (1 == UserClass::n_total_def_ctor_, __FILE__, __LINE__,
               "tuple<UserClass>::tuple() called %d default ctors, "
               "expected 1", UserClass::n_total_def_ctor_);
}

/**************************************************************************/

#define INT_VALUE       int ('a')

// assume that std::get() has been fully tested and works correctly
#define VERIFY_TUPLE(it) \
    rw_assert (std::get<0> (it) == INT_VALUE, __FILE__, __LINE__, \
               "std::get<0> (" #it "), got %d, expected %d", \
               std::get<0> (it), INT_VALUE);


#define LONG_VALUE      INT_VALUE
#define STRING_VALUE    "string"

static void
verify_tuple (const PairTuple& pt)
{
    rw_assert (std::get<0> (pt) == LONG_VALUE, __FILE__, __LINE__,
               "std::get<0> (pt), got %d, expected %d",
               std::get<0> (pt), LONG_VALUE);
    rw_assert (0 == std::strcmp (std::get<1> (pt), STRING_VALUE),
               __FILE__, __LINE__,
               "std::get<1> (pt), got %s, expected %s",
               std::get<1> (pt), STRING_VALUE);
}


#define USER_VALUE      user_val

static UserClass user_val;

typedef unsigned int uint_t;

static void
verify_tuple (const UserTuple& ut, int line,
              uint_t dflt = 0, uint_t copy = 0, uint_t asgn = 0)
{
    rw_assert (std::get<0> (ut) == USER_VALUE, __FILE__, line,
               "std::get<0> (ut), got %d, expected %d",
               (std::get<0> (ut)).data_.val_,
               USER_VALUE.data_.val_);

    rw_assert (dflt == UserClass::n_total_def_ctor_, __FILE__, line,
               "tuple<UserClass>::tuple() called %d default ctors, "
               "expected %d", UserClass::n_total_def_ctor_, dflt);
    rw_assert (copy == UserClass::n_total_copy_ctor_, __FILE__, line,
               "tuple<UserClass>::tuple() called %d copy ctors, "
               "expected %d", UserClass::n_total_copy_ctor_, copy);
    rw_assert (asgn == UserClass::n_total_op_assign_, __FILE__, line,
               "tuple<UserClass>::tuple() called %d assign ops, "
               "expected %d", UserClass::n_total_op_assign_, asgn);
}


#define BOOL_VALUE      true
#define CHAR_VALUE      'a'
#define DBL_VALUE       3.14159
#define PTR_VALUE       ptr_val

static void* ptr_val = 0;

static void
verify_tuple (const BigTuple& bt)
{
    rw_assert (std::get<0> (bt) == BOOL_VALUE, __FILE__, __LINE__,
               "std::get<0> (bt), got %b, expected %b",
               std::get<0> (bt), BOOL_VALUE);
    rw_assert (std::get<1> (bt) == CHAR_VALUE, __FILE__, __LINE__,
               "std::get<1> (bt), got %c, expected %c",
               std::get<1> (bt), CHAR_VALUE);
    rw_assert (std::get<2> (bt) == INT_VALUE, __FILE__, __LINE__,
               "std::get<2> (bt), got %d, expected %d",
               std::get<2> (bt), INT_VALUE);
    int result = rw_dblcmp (std::get<3> (bt), DBL_VALUE);
    rw_assert (0 == result, __FILE__, __LINE__,
               "std::get<3> (bt), got %f, expected %f",
               std::get<3> (bt), DBL_VALUE);
    rw_assert (std::get<4> (bt) == PTR_VALUE, __FILE__, __LINE__,
               "std::get<4> (bt), got %p, expected %p",
               std::get<4> (bt), PTR_VALUE);
    rw_assert ((std::get<5> (bt)) == USER_VALUE, __FILE__, __LINE__,
               "std::get<5> (bt), got %d, expected %d",
               (std::get<5> (bt)).data_.val_, INT_VALUE);
}

/**************************************************************************/

static void
test_value_copy_ctor ()
{
    rw_info (0, __FILE__, __LINE__, "value copy constructor");

    const int i = INT_VALUE;
    IntTuple it1 (i);
    VERIFY_TUPLE (it1);

    const IntTuple it2 (i);
    VERIFY_TUPLE (it2);

    ConstIntTuple ct (i);
    VERIFY_TUPLE (ct);

    int j = INT_VALUE;
    const IntRefTuple rt (j);
    VERIFY_TUPLE (rt);

    NestedTuple nt (it2);
    VERIFY_TUPLE (std::get<0> (nt));

    const long l = LONG_VALUE;
    const char* s = STRING_VALUE;
    PairTuple pt (l, s);
    verify_tuple (pt);

    UserClass::reset_totals ();
    const UserClass& uc = USER_VALUE;
    UserTuple ut (uc);
    verify_tuple (ut, __LINE__, 0, 1);

    const bool b = BOOL_VALUE; const char c = CHAR_VALUE;
    const double d = DBL_VALUE; void* const p = PTR_VALUE;
    BigTuple bt (b, c, i, d, p, uc);
    verify_tuple (bt);
}

/**************************************************************************/

static void
test_value_move_ctor ()
{
    rw_info (0, __FILE__, __LINE__, "value move constructor");

    IntTuple it1 (INT_VALUE); // literal constant
    VERIFY_TUPLE (it1);
    int i = INT_VALUE;
    IntTuple it2 (i); // temporary value
    VERIFY_TUPLE (it2);

    const IntTuple it3 (INT_VALUE);
    VERIFY_TUPLE (it3);
    const IntTuple it4 (i);
    VERIFY_TUPLE (it4);

    ConstIntTuple ct1 (INT_VALUE);
    VERIFY_TUPLE (ct1);
    ConstIntTuple ct2 (i);
    VERIFY_TUPLE (ct2);

    // ill-formed for tuples with element types containing references

    NestedTuple nt (ct1);
    VERIFY_TUPLE (std::get<0> (nt));

    PairTuple pt (LONG_VALUE, STRING_VALUE);
    verify_tuple (pt);

    UserClass uc (USER_VALUE);
    UserClass::reset_totals ();
    UserTuple ut (uc); // may alter temporary/source value
    // no move semantics in UserClass currently
    verify_tuple (ut, __LINE__, 0, 1);

    void* p = PTR_VALUE;
    uc = USER_VALUE;
    BigTuple bt (BOOL_VALUE, CHAR_VALUE, INT_VALUE, DBL_VALUE, p, uc);
    verify_tuple (bt);
}

/**************************************************************************/

static void
test_homo_copy_ctor ()
{
    rw_info (0, __FILE__, __LINE__,
             "copy constructor (homogenous tuples)");

    EmptyTuple et1, et2 (et1);
    _RWSTD_UNUSED (et2);

    const IntTuple it1 (INT_VALUE);
    IntTuple it2 (it1);
    VERIFY_TUPLE (it2);

    const ConstIntTuple& ct1 = it1;
    ConstIntTuple ct2 (ct1);
    VERIFY_TUPLE (it2);

    int i = INT_VALUE;
    const IntRefTuple rt1 (i);
    IntRefTuple rt2 (rt1);
    VERIFY_TUPLE (rt2);

    const NestedTuple nt1 (it1);
    NestedTuple nt2 (nt1);
    VERIFY_TUPLE (std::get<0> (nt2));

    const PairTuple pt1 (LONG_VALUE, STRING_VALUE);
    PairTuple pt2 (pt1);
    verify_tuple (pt2);

    const UserTuple ut1 (USER_VALUE);
    UserClass::reset_totals ();
    UserTuple ut2 (ut1);
    verify_tuple (ut2, __LINE__, 0, 1);

    const BigTuple bt1 (BOOL_VALUE, CHAR_VALUE, INT_VALUE, DBL_VALUE,
                        PTR_VALUE, USER_VALUE);
    BigTuple bt2 (bt1);
    verify_tuple (bt2);
}

/**************************************************************************/

static void
test_homo_move_ctor ()
{
    rw_info (0, __FILE__, __LINE__,
             "move constructor (homogenous tuples)");

    EmptyTuple et (EmptyTuple ()); _RWSTD_UNUSED (et);

    IntTuple it1 (INT_VALUE);
    IntTuple it2 (std::move (it1));
    VERIFY_TUPLE (it2);

    ConstIntTuple ct1 (INT_VALUE);
    ConstIntTuple ct2 = std::move (ct1);
    VERIFY_TUPLE (ct2);

    NestedTuple nt1 (it1);
    NestedTuple nt2 = std::move (nt1);
    VERIFY_TUPLE (std::get<0> (nt2));

    PairTuple pt1 (LONG_VALUE, STRING_VALUE);
    PairTuple pt2 (std::move (pt1));
    verify_tuple (pt2);

    BigTuple bt1 (BOOL_VALUE, CHAR_VALUE, INT_VALUE, DBL_VALUE,
                  PTR_VALUE, USER_VALUE);
    BigTuple bt2 (std::move (bt1));
    verify_tuple (bt2);

    const UserClass& uc = USER_VALUE;
    UserTuple ut1 (uc);
    UserClass::reset_totals ();
    // no move semantics in UserClass currently so it uses copy ctor
    UserTuple ut2 (std::move (ut1));
    verify_tuple (ut2, __LINE__, 0, 1);
}

/**************************************************************************/

static void
test_homo_copy_assign ()
{
    rw_info (0, __FILE__, __LINE__,
             "copy assignment operator (homogenous tuples)");

    const EmptyTuple et1 = EmptyTuple ();
    EmptyTuple et2;
    et2 = et1;
    _RWSTD_UNUSED (et2);

    const IntTuple it1 (INT_VALUE);
    IntTuple it2;
    it2 = it1;
    VERIFY_TUPLE (it2);

    // copy assignment ill-formed for constant element types

    int i = INT_VALUE;
    const IntRefTuple rt1 (i);
    int j = 0;
    IntRefTuple rt2 (j); // note, different reference
    rt2 = rt1;
    VERIFY_TUPLE (rt2);

    NestedTuple nt1 (it1);
    NestedTuple nt2;
    nt2 = nt1;
    VERIFY_TUPLE (std::get<0> (nt2));

    const PairTuple pt1 (LONG_VALUE, STRING_VALUE);
    PairTuple pt2;
    pt2 = pt1;
    verify_tuple (pt2);

    const BigTuple bt1 (BOOL_VALUE, CHAR_VALUE, INT_VALUE, DBL_VALUE,
                        PTR_VALUE, USER_VALUE);
    BigTuple bt2;
    bt2 = bt1;
    verify_tuple (bt2);

    const UserTuple ut1 (USER_VALUE);
    UserTuple ut2;
    UserClass::reset_totals ();
    ut2 = ut1;
    verify_tuple (ut2, __LINE__, 0, 0, 1);
}

/**************************************************************************/

static void
test_homo_move_assign ()
{
    rw_info (0, __FILE__, __LINE__,
             "move assignment operator (homogenous tuples)");

    EmptyTuple et1, et2;
    et2 = std::move (et1);
    _RWSTD_UNUSED (et2);

    IntTuple it1 (INT_VALUE);
    IntTuple it2;
    it2 = std::move (it1);
    VERIFY_TUPLE (it2);

    // move assignment ill-formed for constant element types

    NestedTuple nt1 (it2);
    NestedTuple nt2;
    nt2 = std::move (nt1);
    VERIFY_TUPLE (std::get<0> (nt2));

    PairTuple pt1 (LONG_VALUE, STRING_VALUE);
    PairTuple pt2;
    pt2 = std::move (pt1);
    verify_tuple (pt2);

    BigTuple bt1 (BOOL_VALUE, CHAR_VALUE, INT_VALUE, DBL_VALUE,
                  PTR_VALUE, USER_VALUE);
    BigTuple bt2;
    bt2 = std::move (bt1);
    verify_tuple (bt2);

    const UserClass& uc = USER_VALUE;
    UserTuple ut1 (uc);
    UserTuple ut2;
    UserClass::reset_totals ();
    ut2 = std::move (ut1);
    verify_tuple (ut2, __LINE__, 0, 0, 1);
}

/**************************************************************************/

// heterogenous tests do not apply to empty tuples so no tests required

// UserClass does not currently contain any constructors or assignment
// operators for heterogenous types so no user tuple tests performed

#include <string>

// need a string class with implicit conversion to type `const char*'
struct String: public std::string
{
    String (): std::string () {}
    String (const char* s): std::string (s) {}
    operator const char* () const { return this->data (); }
};

typedef std::tuple<char>                CompatIntTuple;
typedef std::tuple<unsigned, String>    CompatPairTuple;
typedef std::tuple<int, int, short, float, char*,
                   UserClass>           CompatBigTuple;

static void
test_hetero_copy_ctor ()
{
    rw_info (0, __FILE__, __LINE__,
             "copy constructor (heterogenous tuples)");

    const CompatIntTuple ct1 (INT_VALUE);
    IntTuple it (ct1);
    VERIFY_TUPLE (it);

    CompatPairTuple ct2 (LONG_VALUE, STRING_VALUE);
    PairTuple pt (ct2);
    verify_tuple (pt);

    const CompatBigTuple ct3 (BOOL_VALUE, CHAR_VALUE, INT_VALUE,
        DBL_VALUE, (char*) PTR_VALUE, USER_VALUE);
    BigTuple bt (ct3);
    verify_tuple (bt);
}

/**************************************************************************/

static void
test_hetero_move_ctor ()
{
    rw_info (0, __FILE__, __LINE__,
             "move constructor (heterogenous tuples)");

    CompatIntTuple ct1 (INT_VALUE);
    IntTuple it (std::move (ct1));
    VERIFY_TUPLE (it);

    CompatPairTuple ct2 (LONG_VALUE, STRING_VALUE);
    PairTuple pt (std::move (ct2));
    verify_tuple (pt);

    CompatBigTuple ct3 (BOOL_VALUE, CHAR_VALUE, INT_VALUE,
        DBL_VALUE, (char*) PTR_VALUE, USER_VALUE);
    BigTuple bt (std::move (ct3));
    verify_tuple (bt);
}

/**************************************************************************/

static void
test_hetero_copy_assign ()
{
    rw_info (0, __FILE__, __LINE__,
             "copy assignment operator (heterogenous tuples)");
    CompatIntTuple ct1 (INT_VALUE);
    IntTuple it;
    it = ct1;
    VERIFY_TUPLE (it);

    CompatPairTuple ct2 (LONG_VALUE, STRING_VALUE);
    PairTuple pt;
    pt = ct2;
    verify_tuple (pt);

    CompatBigTuple ct3 (BOOL_VALUE, CHAR_VALUE, INT_VALUE,
        DBL_VALUE, (char*) PTR_VALUE, USER_VALUE);
    BigTuple bt;
    bt = ct3;
    verify_tuple (bt);
}

/**************************************************************************/

static void
test_hetero_move_assign ()
{
    rw_info (0, __FILE__, __LINE__,
             "move assignment operator (heterogenous tuples)");

    CompatIntTuple ct1 (INT_VALUE);
    IntTuple it;
    it = std::move (ct1);
    VERIFY_TUPLE (it);

    CompatPairTuple ct2 (LONG_VALUE, STRING_VALUE);
    PairTuple pt;
    pt = std::move (ct2);
    verify_tuple (pt);

    CompatBigTuple ct3 (BOOL_VALUE, CHAR_VALUE, INT_VALUE,
        DBL_VALUE, (char*) PTR_VALUE, USER_VALUE);
    BigTuple bt;
    bt = std::move (ct3);
    verify_tuple (bt);
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
run_test (int /*argc*/, char* argv [])
{
    test_default_ctor ();

    ptr_val = argv [0];
    user_val.data_.val_ = INT_VALUE;

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

#else // _RWSTD_NO_EXT_CXX_0X || _RWSTD_NO_RVALUE_REFERENCES

static int
run_test (int, char*[])
{
#if defined (_RWSTD_NO_EXT_CXX_OX)

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
                    "[tuple.cnstr]", "20.3.1.2 Construction",
                    run_test, "", 0);
}

