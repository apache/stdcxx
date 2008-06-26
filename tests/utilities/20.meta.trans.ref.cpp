// -*- C++ -*-
/***************************************************************************
 *
 * 20.meta.trans.ref.cpp - test exercising meta.trans.ref
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
 * Copyright 1999-2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#include <rw_driver.h>
#include <rw/_defs.h>

// compile out all test code if extensions disabled
#ifndef _RWSTD_NO_EXT_CXX_0X

#include <type_traits>

/**************************************************************************/

class class_t;

void test_trait (int line, const char* trait,
                 const char* typeT, const char* typeU,
                 bool success)
{
    rw_assert (success, 0, line,
               "%s<%s>::type is %{?}exactly%{:}not%{;} %s",
               trait, typeT, success, typeU);
}

#define TEST(Trait,TypeT,TypeU)                                      \
    test_trait(__LINE__, #Trait, #TypeT, #TypeU,                     \
               std::is_same<Trait<TypeT>::type, TypeU>::value)

/**************************************************************************/

static void test_add_lvalue_reference ()
{
    TEST (std::add_lvalue_reference, int, int&);
    TEST (std::add_lvalue_reference, int&, int&);

    TEST (std::add_lvalue_reference, void, void);

    TEST (std::add_lvalue_reference, int const     [1], int const (&) [1]);
    TEST (std::add_lvalue_reference, int const (&) [1], int const (&) [1]);

    TEST (std::add_lvalue_reference, int const * volatile,
                                      int const * volatile &);
    TEST (std::add_lvalue_reference, int const * volatile &,
                                      int const * volatile &);

    TEST (std::add_lvalue_reference, int (), int (&)());
    TEST (std::add_lvalue_reference, int (&)(), int (&)());

    TEST (std::add_lvalue_reference, int (class_t::*), int (class_t::*&));
    TEST (std::add_lvalue_reference, int (class_t::*)(), int (class_t::*&)());
}

static void test_add_rvalue_reference ()
{
#ifndef _RWSTD_NO_RVALUE_REFERENCE

#else
    rw_warn (0, 0, __LINE__,
             "rvalue references are not supported");
#endif   // _RWSTD_NO_RVALUE_REFERENCE
}

static void test_remove_reference ()
{
    TEST(std::remove_reference, int, int);
    TEST(std::remove_reference, int&, int);
    
    TEST(std::remove_reference, int const [], int const []);
    TEST(std::remove_reference, int const (&) [1], int const [1]);

    TEST(std::remove_reference, int const * volatile,
                                 int const * volatile);
    TEST(std::remove_reference, int const * volatile &,
                                 int const * volatile);

    TEST (std::remove_reference, int (), int ());
    TEST (std::remove_reference, int (&)(), int ());

    TEST (std::remove_reference, int (class_t::*&), int (class_t::*));
    TEST (std::remove_reference, int (class_t::*&)(), int (class_t::*)());
}

/**************************************************************************/

static int run_test (int, char*[])
{
    test_add_lvalue_reference ();
    test_add_rvalue_reference ();
    test_remove_reference ();

    return 0;
}

/**************************************************************************/

#else // _RWSTD_NO_EXT_CXX_0X

/**************************************************************************/

static int run_test (int, char*[])
{
    rw_warn (0, 0, __LINE__,
             "test disabled because _RWSTD_NO_EXT_CXX_0X is defined");
    return 0;
}

#endif // !_RWSTD_NO_EXT_CXX_0X

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "meta.trans.ref",
                    0 /* no comment */,
                    run_test,
                    0);
}
