// -*- C++ -*-
/***************************************************************************
 *
 * 20.meta.unary.cat.cpp - test exercising meta.unary.cat
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

#include <stddef.h>

/**************************************************************************/

static void
test_trait (int line,
            const char* trait_name, const char* type_name,
            size_t count, bool value)
{
    // verify that the expected trait is actually working

    if (rw_assert (value != false, 0, line,
                   "%s<%s>::value was %b, expected true",
                   trait_name, type_name, value))
    {

        // [meta.unary.cat p3] for any given type T, exactly one of the
        // primary type categories has a value member that evaluates to
        // true

        rw_assert (count == 1, 0, line,
                   "%s evaluated true for %zu traits, expected 1",
                   type_name, count);
    }
}

#define _TEST(Trait,Type)                                            \
    test_trait(__LINE__, #Trait, #Type,                              \
          std::is_void<Type>::value                                  \
        + std::is_integral<Type>::value                              \
        + std::is_floating_point<Type>::value                        \
        + std::is_array<Type>::value                                 \
        + std::is_pointer<Type>::value                               \
        + std::is_lvalue_reference<Type>::value                      \
        + std::is_rvalue_reference<Type>::value                      \
        + std::is_member_object_pointer<Type>::value                 \
        + std::is_member_function_pointer<Type>::value               \
        + std::is_enum<Type>::value                                  \
        + std::is_union<Type>::value                                 \
        + std::is_class<Type>::value                                 \
        + std::is_function<Type>::value,                             \
        Trait<Type>::value)

#if 0

#define TEST(Trait,Type)                                             \
    _TEST(Trait, Type);                                              \
    _TEST(Trait, std::add_const<Type>::type);                        \
    _TEST(Trait, std::add_volatile<Type>::type);                     \
    _TEST(Trait, std::add_cv<Type>::type)

#else

#define TEST(Trait,Type)                                             \
    _TEST(Trait, Type)

#endif

/**************************************************************************/

class class_t {
    int i_; float f_;
};

struct struct_t {
    int i_; float f_;
};

union union_t {
    int i_; float f_;
};

enum enum_t {
    E_a, E_b, E_c
};

/**************************************************************************/

static void test_is_void ()
{
    TEST(std::is_void, void);
}

static void test_is_integral ()
{
    TEST(std::is_integral, bool);
    TEST(std::is_integral, char);

#ifndef _RWSTD_NO_WCHAR_T
    TEST(std::is_integral, wchar_t);
#endif   // _RWSTD_NO_WCHAR_T

    TEST(std::is_integral,   signed char);
    TEST(std::is_integral, unsigned char);

    TEST(std::is_integral,   signed short);
    TEST(std::is_integral, unsigned short);

    TEST(std::is_integral,   signed int);
    TEST(std::is_integral, unsigned int);

    TEST(std::is_integral,   signed long);
    TEST(std::is_integral, unsigned long);

#ifndef _RWSTD_NO_LONG_LONG
    TEST(std::is_integral,   signed long long);
    TEST(std::is_integral, unsigned long long);
#endif   // _RWSTD_NO_LONG_LONG
}

static void test_is_floating_point ()
{
    TEST(std::is_floating_point, float);
    TEST(std::is_floating_point, double);
    TEST(std::is_floating_point, long double);
}

static void test_is_array ()
{
    TEST(std::is_array, char []);
    TEST(std::is_array, char [10]);
    TEST(std::is_array, char [][10]);
    TEST(std::is_array, char [10][10]);
}

static void test_is_pointer ()
{
    TEST(std::is_pointer, char* const);
    TEST(std::is_pointer, char const*);
    TEST(std::is_pointer, const char*);

    TEST(std::is_pointer, char*);
    TEST(std::is_pointer, char (*)[]);
    TEST(std::is_pointer, char (*)());
}

static void test_is_lvalue_reference ()
{
    TEST(std::is_lvalue_reference, char&);            // reference

    TEST(std::is_lvalue_reference, void (&)());       // function reference
    TEST(std::is_lvalue_reference, void (&)(int));    // function reference

#ifdef _ILL_FORMED
    TEST(std::is_lvalue_reference, long (&)[]);       // array reference
    TEST(std::is_lvalue_reference, long (&)[][10]);   // array reference
#endif

    TEST(std::is_lvalue_reference, long (&)[10]);     // array reference
    TEST(std::is_lvalue_reference, long (&)[10][10]); // array reference
}

static void test_is_rvalue_reference ()
{
#ifndef _RWSTD_NO_RVALUE_REFERENCES
    TEST(std::is_rvalue_reference, char&&);
#else
    rw_warn (0, 0, __LINE__,
             "test_is_rvalue_reference() disabled because "
             "_RWSTD_NO_RVALUE_REFERENCES is defined");
#endif   // _RWSTD_NO_RVALUE_REFERENCES
}

static void test_is_member_object_pointer ()
{
    TEST (std::is_member_object_pointer, long (class_t::*));
    TEST (std::is_member_object_pointer, long (struct_t::*));
    TEST (std::is_member_object_pointer, long (union_t::*));
}

static void test_is_member_function_pointer ()
{
    // arity 0
    TEST (std::is_member_function_pointer, long (class_t:: *)());
    TEST (std::is_member_function_pointer, long (struct_t::*)());
    TEST (std::is_member_function_pointer, long (union_t:: *)());

    // arity 1
    TEST (std::is_member_function_pointer, long (class_t:: *)(int));
    TEST (std::is_member_function_pointer, long (struct_t::*)(int));
    TEST (std::is_member_function_pointer, long (union_t:: *)(int));

    // arity 2
    TEST (std::is_member_function_pointer, 
             long (class_t:: *)(int, char));
    TEST (std::is_member_function_pointer,
             long (struct_t::*)(int, char));
    TEST (std::is_member_function_pointer,
             long (union_t:: *)(int, char));

    // arity 12
    TEST (std::is_member_function_pointer,
             long (class_t:: *)(char, short, int, long, float, double,
                                char, short, int, long, float, double));
    TEST (std::is_member_function_pointer,
             long (struct_t::*)(char, short, int, long, float, double,
                                char, short, int, long, float, double));
    TEST (std::is_member_function_pointer,
             long (union_t:: *)(char, short, int, long, float, double,
                                char, short, int, long, float, double));
}

static void test_is_enum ()
{
    TEST (std::is_enum, enum_t);
}

static void test_is_union ()
{
#if !defined (_RWSTD_TT_IS_CLASS) && !defined (_RWSTD_TT_IS_UNION)

    rw_warn (0, 0, __LINE__,
             "test_is_union() disabled because neither of "
             "_RWSTD_TT_IS_CLASS or _RWSTD_TT_IS_UNION is "
             "defined");

#else
    TEST (std::is_union, union_t);
#endif
}

static void test_is_class ()
{
    TEST (std::is_class, class_t);
    TEST (std::is_class, struct_t);
}

static void test_is_function ()
{
    TEST (std::is_function, void ());
    TEST (std::is_function, long (int));
    TEST (std::is_function, long (int, char));
    TEST (std::is_function, long (int, char));
}


/**************************************************************************/

static int run_test (int, char*[])
{
    test_is_void ();
    test_is_integral ();
    test_is_floating_point ();
    test_is_array ();
    test_is_pointer ();
    test_is_lvalue_reference ();
    test_is_rvalue_reference ();
    test_is_member_object_pointer ();
    test_is_member_function_pointer ();
    test_is_enum ();
    test_is_union ();
    test_is_class ();
    test_is_function ();

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
                    "meta.unary.cat",
                    0 /* no comment */,
                    run_test,
                    0);
}

