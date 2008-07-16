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

#include <climits>              // for CHAR_MAX
#include <cstdlib>              // for rand
#include <cstring>              // for strcmp
#include <tuple>
#include <type_traits>          // for decay

#include <rw_valcmp.h>          // for rw_fltcmp

#include "20.tuple.h"

/**************************************************************************/

// general test function and underlying abstractions

template <class T, class U = T>
bool equal (const T& x, const U& y)
{
    return x == y;
}

template <>
bool equal (const float& x, const float& y)
{
    return 0 == rw_fltcmp (x, y);
}

template <>
bool equal (const char* const& x, const char* const& y)
{
    return 0 == std::strcmp (x, y);
}


template <class T, class U>
void assert (int line, unsigned index, const char* tuple_name,
             const T& t, const U& u)
{
    const char* fmtT = FMT_SPEC (T);
    const char* fmtU = FMT_SPEC (U);
    rw_assert (equal (t, u), __FILE__, line,
               "get<%d, %s> (); got %{@}, expected %{@}",
               index, tuple_name, fmtT, t, fmtU, u);
}

void assert (int line, unsigned index, const char* tuple_name,
             const UserDefined& t, const UserDefined& u)
{
    rw_assert (equal (t, u), __FILE__, line,
               "get<%d, %s> (); got %p [%d], expected %p [%d]",
               index, tuple_name, &t, t.value (), &u, u.value ());

    UserDefined::size_type a, e;

#undef CHECK
#define CHECK(N)          \
    a = UserDefined::actual.N; e = UserDefined::expect.N; \
    rw_assert (a == e, __FILE__, line, \
               "UserDefined::" #N "; got %u, expected %u", a, e)

    CHECK (dflt_ctor);
    CHECK (copy_ctor);
    CHECK (tmpl_ctor);
    CHECK (move_ctor);
    CHECK (copy_asgn);
    CHECK (tmpl_asgn);
    CHECK (move_asgn);
}

template <class T, class U>
void assert (int line, unsigned index, const char* tuple_name,
             const std::tuple<T>& t, const std::tuple<U>& u)
{
    const char* fmtT = FMT_SPEC (T);
    const char* fmtU = FMT_SPEC (U);
    rw_assert (equal (t, u), __FILE__, line,
               "get<%d, %s> (); got %{@}, expected %{@}",
               index, tuple_name,
               fmtT, std::get<0> (t), fmtU, std::get<0> (u));
}


template <unsigned Index, class Tuple, class... Elements>
/*static*/ void
test_impl (int line, const Tuple& tuple, const Elements&... values);

// terminating specialization
template <unsigned Index, class Tuple>
/*static*/ void
test_impl (int, const Tuple&) {}

// generic definition
template <unsigned Index, class Tuple, class Element, class... Elements>
/*static*/ void
test_impl (int line, const Tuple& tuple,
           const Element& value, const Elements&... values)
{
    // assume std::get() has been fully tested and works correctly
    assert (line, Index, TYPE_NAME (Tuple),
            std::get<Index> (tuple), value);

    test_impl<Index+1> (line, tuple, values...);
}

template <class Tuple, class... Elements>
/*static*/ void
test (int line, const Tuple& tuple, const Elements&... values)
{
    test_impl<0> (line, tuple, values...);
}

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

    UserDefined::reset ();
    UserTuple ut; _RWSTD_UNUSED (ut);
    rw_assert (1 == UserDefined::actual.dflt_ctor, __FILE__, __LINE__,
               "UserTuple::UserTuple (); called %d default ctors, "
               "expected 1", UserDefined::actual.dflt_ctor);
}

/**************************************************************************/

static void
test_value_copy_ctor ()
{
    rw_info (0, __FILE__, __LINE__, "value copy constructor");

    const int i = std::rand ();
    IntTuple it1 (i);
    test (__LINE__, it1, i);

    const IntTuple it2 (i);
    test (__LINE__, it2, i);

    ConstIntTuple ct (i);
    test (__LINE__, ct, i);

    int j = std::rand ();
    const IntRefTuple rt (j);
    test (__LINE__, rt, j);

    NestedTuple nt (it2);
    //std::get<0> (it2) = std::rand (); // diliberately cause assertion
    test (__LINE__, nt, it2);

    const long l = std::rand ();
    PairTuple pt (l, "string");
    test (__LINE__, pt, l, (const char*) "string");

    const UserDefined ud (i);
    UserDefined::reset ();
    UserTuple ut (ud);
    UserDefined::expect.copy_ctor = 1;
    test (__LINE__, ut, ud);

    const bool b = true; const char c = 'a';
    const double d = 3.14159; void* const p = (void*) &i;
    BigTuple bt (b, c, i, d, p, ud);
    ++UserDefined::expect.copy_ctor;
    test (__LINE__, bt, b, c, i, d, p, ud);
}

/**************************************************************************/

static void
test_value_move_ctor ()
{
    rw_info (0, __FILE__, __LINE__, "value move constructor");

#define INTEGER_CONSTANT        256

    IntTuple it1 (INTEGER_CONSTANT);
    test (__LINE__, it1, INTEGER_CONSTANT);
    const int c = std::rand ();
    int i = c;   // move semantics can alter source value
    IntTuple it2 (i);   // temporary source value
    test (__LINE__, it2, c);

    const IntTuple it3 (INTEGER_CONSTANT);
    test (__LINE__, it3, INTEGER_CONSTANT);
    i = c;
    const IntTuple it4 (i);
    test (__LINE__, it4, c);

    ConstIntTuple ct1 (INTEGER_CONSTANT);
    test (__LINE__, ct1, INTEGER_CONSTANT);
    i = c;
    ConstIntTuple ct2 (i);
    test (__LINE__, ct2, c);

    // ill-formed for tuples with element types containing references

    NestedTuple nt (it1);
    test (__LINE__, nt, it1);

    PairTuple pt (123456789L, "string");
    test (__LINE__, pt, 123456789L, (const char*) "string");

    const UserDefined src (c);
    UserDefined tmp (src);
    UserDefined::reset ();
    UserTuple ut (tmp);
    UserDefined::expect.move_ctor = 1;
    test (__LINE__, ut, src);

    tmp = src;  ++UserDefined::expect.copy_asgn;
    BigTuple bt (true, 'a', INTEGER_CONSTANT, 3.14159, (void*) 0, tmp);
    ++UserDefined::expect.move_ctor;
    test (__LINE__, bt,
          true, 'a', INTEGER_CONSTANT, 3.14159, (void*) 0, src);
}

/**************************************************************************/

static void
test_homo_copy_ctor ()
{
    rw_info (0, __FILE__, __LINE__,
             "copy constructor (homogenous tuples)");

    EmptyTuple et1, et2 (et1);
    _RWSTD_UNUSED (et2);

    const int ci = std::rand ();
    const IntTuple it1 (ci);
    IntTuple it2 (it1);
    test (__LINE__, it2, ci);

    const ConstIntTuple& ct1 = it1; // same as copy ctor
    ConstIntTuple ct2 (ct1);
    test (__LINE__, ct2, ci);

    int i = ci;
    const IntRefTuple rt1 (i);
    IntRefTuple rt2 (rt1);
    test (__LINE__, rt2, ci);

    const NestedTuple nt1 (it1);
    NestedTuple nt2 (nt1);
    test (__LINE__, nt2, it1);

    const PairTuple pt1 (1234567890L, "string");
    PairTuple pt2 (pt1);
    test (__LINE__, pt2, 1234567890L, (const char*) "string");

    UserDefined ud (ci);
    const UserTuple ut1 (ud);
    UserDefined::reset ();
    UserTuple ut2 (ut1);
    ++UserDefined::expect.copy_ctor;
    test (__LINE__, ut2, ud);

    const BigTuple bt1 (true, 'a', ci, 3.14159, (void* const) &i, ud);
    ++UserDefined::expect.move_ctor; // moved ud to bt1
    BigTuple bt2 (bt1);
    ++UserDefined::expect.copy_ctor; // copied to bt2
    test (__LINE__, bt2, true, 'a', ci, 3.14159, (void* const) &i, ud);
}

/**************************************************************************/

static void
test_homo_move_ctor ()
{
    rw_info (0, __FILE__, __LINE__,
             "move constructor (homogenous tuples)");

    EmptyTuple et (EmptyTuple ()); _RWSTD_UNUSED (et);

    const int ci = std::rand ();

    IntTuple it1 (ci);
    IntTuple it2 (std::move (it1));
    test (__LINE__, it2, ci);

    ConstIntTuple ct1 (ci);
    ConstIntTuple ct2 = std::move (ct1);
    test (__LINE__, ct2, ci);

    NestedTuple nt1 (it1);
    NestedTuple nt2 = std::move (nt1);
    test (__LINE__, nt2, it1);

    PairTuple pt1 (1234567890L, "string");
    PairTuple pt2 (std::move (pt1));
    test (__LINE__, pt2, 1234567890L, (const char*) "string");

    const UserDefined ud (ci);
    UserTuple ut1 (ud);
    UserDefined::reset ();
    UserTuple ut2 (std::move (ut1));
    ++UserDefined::expect.move_ctor;
    test (__LINE__, ut2, ud);

    BigTuple bt1 (true, 'a', ci, 3.14159, (void*) &ci, ud);
    ++UserDefined::expect.copy_ctor;
    BigTuple bt2 (std::move (bt1));
    ++UserDefined::expect.move_ctor;
    test (__LINE__, bt2, true, 'a', ci, 3.14159, (void*) &ci, ud);
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

    int i = std::rand ();
    const IntTuple it1 (i);
    IntTuple it2;
    it2 = it1;
    test (__LINE__, it2, i);

    // copy assignment ill-formed for constant element types

    const IntRefTuple rt1 (i);
    int j = -1; // outside range of rand()
    IntRefTuple rt2 (j); // note, different reference
    rt2 = rt1;
    test (__LINE__, rt2, i);

    NestedTuple nt1 (it1);
    NestedTuple nt2;
    nt2 = nt1;
    test (__LINE__, nt2, it1);

    const PairTuple pt1 (long (i), "string");
    PairTuple pt2;
    pt2 = pt1;
    test (__LINE__, pt2, long (i), (const char*) "string");

    const UserDefined ud (i);
    const UserTuple ut1 (ud);
    UserTuple ut2;
    UserDefined::reset ();
    ut2 = ut1;  ++UserDefined::expect.copy_asgn;
    test (__LINE__, ut2, ud);

    const BigTuple bt1 (true, 'a', i, 3.14159, (void* const) &i, ud);
    ++UserDefined::expect.copy_ctor;
    BigTuple bt2;  ++UserDefined::expect.dflt_ctor;
    bt2 = bt1;  ++UserDefined::expect.copy_asgn;
    test (__LINE__, bt2, true, 'a', i, 3.14159, (void* const) &i, ud);
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

    int i = std::rand ();

    IntTuple it1 (i);
    IntTuple it2;
    it2 = std::move (it1);
    test (__LINE__, it2, i);

    // move assignment ill-formed for constant element types

    NestedTuple nt1 (it2);
    NestedTuple nt2;
    nt2 = std::move (nt1);
    test (__LINE__, nt2, it2);

    PairTuple pt1 (1234567890L, "string");
    PairTuple pt2;
    pt2 = std::move (pt1);
    test (__LINE__, pt2, 1234567890L, (const char*) "string");

    const UserDefined ud (i);
    UserTuple ut1 (ud);
    UserTuple ut2;
    UserDefined::reset ();
    ut2 = std::move (ut1);  ++UserDefined::expect.move_asgn;
    test (__LINE__, ut2, ud);

    BigTuple bt1 (true, 'a', i, 3.14159, (void* const) &i, ud);
    BigTuple bt2;
    UserDefined::reset ();
    bt2 = std::move (bt1);  ++UserDefined::expect.move_asgn;
    test (__LINE__, bt2, true, 'a', i, 3.14159, (void* const) &i, ud);
}

/**************************************************************************/

// heterogenous tests do not apply to empty tuples so no tests required

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
typedef std::tuple<int, int, short, float,
                   char*, UserDefined>  CompatBigTuple;

static void
test_hetero_copy_ctor ()
{
    rw_info (0, __FILE__, __LINE__,
             "copy constructor (heterogenous tuples)");

    int i = std::rand () % CHAR_MAX;

    const CompatIntTuple cit (static_cast<char> (i));
    IntTuple it (cit);
    test (__LINE__, it, i);

    CompatPairTuple cpt (12345U, "string");
    PairTuple pt (cpt);
    test (__LINE__, pt, 12345U, (const char*) "string");

    char s [] = "string"; const UserDefined ud (i);
    const CompatBigTuple cbt (int (true), int ('a'), short (i),
                              3.14159f, s, ud);
    UserDefined::reset ();
    BigTuple bt (cbt);  ++UserDefined::expect.copy_ctor;
    test (__LINE__, bt, true, 'a', i, 3.14159f, s, ud);
}

/**************************************************************************/

static void
test_hetero_move_ctor ()
{
    rw_info (0, __FILE__, __LINE__,
             "move constructor (heterogenous tuples)");

    int i = std::rand () % CHAR_MAX;

    CompatIntTuple cit (static_cast<char> (i));
    IntTuple it (std::move (cit));
    test (__LINE__, it, i);

    CompatPairTuple cpt (12345U, "string");
    PairTuple pt (std::move (cpt));
    test (__LINE__, pt, 12345U, (const char*) "string");

    char s [] = "string"; const UserDefined ud (i);
    CompatBigTuple cbt (int (true), int ('a'), short (i),
                        3.14159f, s, ud);
    UserDefined::reset ();
    BigTuple bt (std::move (cbt));  ++UserDefined::expect.move_ctor;
    test (__LINE__, bt, true, 'a', i, 3.14159f, s, ud);
}

/**************************************************************************/

static void
test_hetero_copy_assign ()
{
    rw_info (0, __FILE__, __LINE__,
             "copy assignment operator (heterogenous tuples)");

    int i = std::rand () % CHAR_MAX;

    CompatIntTuple cit (static_cast<char> (i));
    IntTuple it;
    it = cit;
    test (__LINE__, it, i);

    CompatPairTuple cpt (12345U, "string");
    PairTuple pt;
    pt = cpt;
    test (__LINE__, pt, 12345U, (const char*) "string");

    char s [] = "string"; const UserDefined ud (i);
    CompatBigTuple cbt (int (true), int ('a'), short (i),
                        3.14159f, s, ud);
    BigTuple bt;
    UserDefined::reset ();
    bt = cbt;  ++UserDefined::expect.copy_asgn;
    test (__LINE__, bt, true, 'a', i, 3.14159f, s, ud);
}

/**************************************************************************/

static void
test_hetero_move_assign ()
{
    rw_info (0, __FILE__, __LINE__,
             "move assignment operator (heterogenous tuples)");

    int i = std::rand () % CHAR_MAX;

    CompatIntTuple cit (i);
    IntTuple it;
    it = std::move (cit);
    test (__LINE__, it, i);

    CompatPairTuple cpt (12345U, "string");
    PairTuple pt;
    pt = std::move (cpt);
    test (__LINE__, pt, 12345U, (const char*) "string");

    char s [] = "string"; const UserDefined ud (i);
    CompatBigTuple cbt (int (true), int ('a'), short (i),
                        3.14159f, s, ud);
    BigTuple bt;  ++UserDefined::expect.move_ctor;
    UserDefined::reset ();
    bt = std::move (cbt);  ++UserDefined::expect.move_asgn;
    test (__LINE__, bt, true, 'a', i, 3.14159f, s, ud);
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
run_test (int /*argc*/, char* /*argv*/ [])
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

