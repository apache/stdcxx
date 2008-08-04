// -*- C++ -*-
/***************************************************************************
 *
 * 20.meta.rel.cpp - test exercising meta.rel
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

#include <type_traits>

#include <rw_driver.h>
#include <rw/_defs.h>

// compile out all test code if extensions disabled
#ifndef _RWSTD_NO_EXT_CXX_0X

/**************************************************************************/

enum enum_A { E_a = 0 };
enum enum_B { E_b = 0 };

struct struct_A {
    int i_; float f_;
};

struct struct_AA {
    int i_; float f_;
};

class class_B {
    int i_; float f_;
};

class class_BB {
    int i_; float f_;
};

struct convertible_to_A
{
    operator struct_A ();
};

struct constructible_from_B
{
    constructible_from_B (class_B);
};

template <class T>
struct derived_t : T
{
};

template <class T>
struct derived_private_t : private T
{
};

template <class T>
struct derived_protected_t : protected T
{
};

template <class T>
struct derived_with_conversion_t : T
{
    derived_with_conversion_t (const T&);
};

class incomplete_t;

union union_C {
    int i_; float f_;
};

union union_CC {
    int i_; float f_;
};

/**************************************************************************/

void test_trait (int line, bool value, bool expect,
                 const char* trait, const char* typeT, const char* typeU)
{
    rw_assert (value == expect, 0, line,
               "%s<%s, %s>::type is%{?}n't%{;} %b as expected",
               trait, typeT, typeU, value != expect, expect);
}

#define TEST(Trait,TypeT,TypeU,Expect)                              \
    { const bool value = Trait< TypeT, TypeU >::value;              \
      test_trait (__LINE__, value, Expect, #Trait, #TypeT, #TypeU); \
    } typedef void __dummy

static void test_is_same ()
{
    TEST (std::is_same, bool, bool, true);
    TEST (std::is_same, char, char, true);
    TEST (std::is_same, short, short, true);
    TEST (std::is_same, int, int, true);
    TEST (std::is_same, long, long, true);
    TEST (std::is_same, float, float, true);
    TEST (std::is_same, double, double, true);
    
    TEST (std::is_same, enum_A, enum_A, true);
    TEST (std::is_same, enum_B, enum_B, true);
   
    TEST (std::is_same, struct_A, struct_A, true);

    typedef derived_t<struct_A> derived_A;
    TEST (std::is_same, derived_A, derived_A, true);

    TEST (std::is_same, class_B, class_B, true);

    typedef derived_t<class_B> derived_B;
    TEST (std::is_same, derived_B, derived_B, true);

    // other combinations should fail
    TEST (std::is_same, signed char, char, false);
    TEST (std::is_same, char, signed char, false);
    TEST (std::is_same, unsigned char, char, false);
    TEST (std::is_same, char, unsigned char, false);

    TEST (std::is_same, signed char, unsigned char, false);
    TEST (std::is_same, unsigned char, signed char, false);
    TEST (std::is_same, signed short, unsigned short, false);
    TEST (std::is_same, unsigned short, signed short, false);
    TEST (std::is_same, signed int, unsigned int, false);
    TEST (std::is_same, unsigned int, signed int, false);
    TEST (std::is_same, signed long, unsigned long, false);
    TEST (std::is_same, unsigned long, signed long, false);

    TEST (std::is_same, long, const long, false);
    TEST (std::is_same, const long, long, false);

    TEST (std::is_same, long, volatile long, false);
    TEST (std::is_same, volatile long, long, false);

    TEST (std::is_same, long, const volatile long, false);
    TEST (std::is_same, const volatile long, long, false);

    TEST (std::is_same, enum_A, char, false);
    TEST (std::is_same, enum_A, short, false);
    TEST (std::is_same, enum_A, int, false);
    TEST (std::is_same, enum_A, long, false);

    TEST (std::is_same, enum_A, unsigned char, false);
    TEST (std::is_same, enum_A, unsigned short, false);
    TEST (std::is_same, enum_A, unsigned int, false);
    TEST (std::is_same, enum_A, unsigned long, false);

    TEST (std::is_same, struct_A, derived_t<struct_A>, false);
    TEST (std::is_same, class_B, derived_t<class_B>, false);

    TEST (std::is_same, int[], int*, false);
    TEST (std::is_same, int*, int[], false);
}

#define TEST(Trait,TypeT,TypeU,Expect)                              \
    { const bool value = Trait< TypeT, TypeU >::value;              \
      test_trait (__LINE__, value, Expect, #Trait, #TypeT, #TypeU); \
    } typedef void __dummy

static void test_is_base_of ()
{
    TEST (std::is_base_of, bool, bool, false);
    TEST (std::is_base_of, char, char, false);
    TEST (std::is_base_of, short, short, false);
    TEST (std::is_base_of, int, int, false);
    TEST (std::is_base_of, long, long, false);
    TEST (std::is_base_of, float, float, false);
    TEST (std::is_base_of, double, double, false);
    
    TEST (std::is_base_of, enum_A, enum_A, false);
    TEST (std::is_base_of, enum_B, enum_B, false);
    
    TEST (std::is_base_of, struct_A, struct_A, true);
    TEST (std::is_base_of, derived_t<struct_A>,
                           derived_t<struct_A>, true);

    TEST (std::is_base_of, class_B, class_B, true);
    TEST (std::is_base_of, derived_t<class_B>,
                           derived_t<class_B>, true);

#if    defined (_RWSTD_TT_IS_BASE_OF) \
    || defined (_RWSTD_TT_IS_CLASS)   \
    || defined (_RWSTD_TT_IS_UNION)
    // without one of the above, we can't reliably implement
    // this trait for union type
    TEST (std::is_base_of, union_C, union_C, false);
#else
    rw_warn (0, 0, __LINE__,
             "portions of test_is_base_of() have been disabled due "
             "to lack of compiler support.");
#endif

    // public inheritance
    TEST (std::is_base_of, struct_A, derived_t<struct_A>, true);
    TEST (std::is_base_of, class_B, derived_t<class_B>, true);

#if defined (__SUNPRO_CC) && (__SUNPRO_CC <= 0x590)

    // for some reason the trick used to detect private and protected
    // inheritance doesn't work on sunpro-5.9
    rw_warn (0, 0, __LINE__,
             "unable to detect private or protected base classes.");

#else
    // protected inheritance
    TEST (std::is_base_of, struct_A, derived_protected_t<struct_A>, true);

    // private inheritance
    TEST (std::is_base_of, struct_A, derived_private_t<struct_A>, true);
#endif

    // cv-qualified
    TEST (std::is_base_of, const struct_A, struct_A, true);
    TEST (std::is_base_of, struct_A, const struct_A, true);
    TEST (std::is_base_of, volatile struct_A, struct_A, true);
    TEST (std::is_base_of, struct_A, volatile struct_A, true);
    TEST (std::is_base_of, const volatile struct_A, struct_A, true);
    TEST (std::is_base_of, struct_A, const volatile struct_A, true);

    TEST (std::is_base_of, const struct_A, derived_t<struct_A>, true);
    TEST (std::is_base_of, struct_A, const derived_t<struct_A>, true);
    TEST (std::is_base_of, volatile struct_A, derived_t<struct_A>, true);
    TEST (std::is_base_of, struct_A, volatile derived_t<struct_A>, true);
    TEST (std::is_base_of, const volatile struct_A, derived_t<struct_A>, true);
    TEST (std::is_base_of, struct_A, const volatile derived_t<struct_A>, true);

    // other combinations should fail
    TEST (std::is_base_of, signed char, char, false);
    TEST (std::is_base_of, char, signed char, false);
    TEST (std::is_base_of, unsigned char, char, false);
    TEST (std::is_base_of, char, unsigned char, false);

    TEST (std::is_base_of, signed char, unsigned char, false);
    TEST (std::is_base_of, unsigned char, signed char, false);
    TEST (std::is_base_of, signed short, unsigned short, false);
    TEST (std::is_base_of, unsigned short, signed short, false);
    TEST (std::is_base_of, signed int, unsigned int, false);
    TEST (std::is_base_of, unsigned int, signed int, false);
    TEST (std::is_base_of, signed long, unsigned long, false);
    TEST (std::is_base_of, unsigned long, signed long, false);

    TEST (std::is_base_of, enum_A, char, false);
    TEST (std::is_base_of, enum_A, short, false);
    TEST (std::is_base_of, enum_A, int, false);
    TEST (std::is_base_of, enum_A, long, false);

    TEST (std::is_base_of, enum_A, unsigned char, false);
    TEST (std::is_base_of, enum_A, unsigned short, false);
    TEST (std::is_base_of, enum_A, unsigned int, false);
    TEST (std::is_base_of, enum_A, unsigned long, false);

    TEST (std::is_base_of, int[], int*, false);
    TEST (std::is_base_of, int*, int[], false);

    TEST (std::is_base_of, struct_A, class_B, false);
    TEST (std::is_base_of, class_B, struct_A, false);

    TEST (std::is_base_of, derived_t<struct_A>, struct_A, false);
    TEST (std::is_base_of, derived_t<class_B>, class_B, false);
}

static void test_is_convertible ()
{
    TEST (std::is_convertible, derived_t<struct_A> , struct_A , true); // slice
    TEST (std::is_convertible, derived_t<struct_A>*, struct_A*, true);
    TEST (std::is_convertible, derived_t<struct_A>&, struct_A&, true);

    TEST (std::is_convertible, derived_t<class_B> , class_B , true); // slice
    TEST (std::is_convertible, derived_t<class_B>*, class_B*, true);
    TEST (std::is_convertible, derived_t<class_B>&, class_B&, true);

    TEST (std::is_convertible, convertible_to_A, struct_A, true);
    TEST (std::is_convertible, struct_A, convertible_to_A, false);
    TEST (std::is_convertible, convertible_to_A*, struct_A*, false);
    TEST (std::is_convertible, convertible_to_A&, struct_A&, false);

    TEST (std::is_convertible, constructible_from_B, class_B, false);
    TEST (std::is_convertible, class_B, constructible_from_B, true);
    TEST (std::is_convertible, class_B*, constructible_from_B*, false);
    TEST (std::is_convertible, class_B&, constructible_from_B&, false);

    //
    // pointer to pointer conversions
    //

    TEST (std::is_convertible,                int*,                int*, true);
    TEST (std::is_convertible, const          int*, const          int*, true);
    TEST (std::is_convertible,       volatile int*,       volatile int*, true);
    TEST (std::is_convertible, const volatile int*, const volatile int*, true);

    // add cv-qualifiers
    TEST (std::is_convertible, int*, const          int*, true);
    TEST (std::is_convertible, int*,       volatile int*, true);
    TEST (std::is_convertible, int*, const volatile int*, true);

    // strip cv-qualifiers
    TEST (std::is_convertible, const          int*, int*, false);
    TEST (std::is_convertible,       volatile int*, int*, false);
    TEST (std::is_convertible, const volatile int*, int*, false);

    //
    // special case void conversions
    //

    TEST (std::is_convertible, void, void, true);
    TEST (std::is_convertible, void, long, false);
    TEST (std::is_convertible, long, void, false);

    //
    // array to pointer conversions
    //

    TEST (std::is_convertible,                int[],                int*, true);
    TEST (std::is_convertible, const          int[], const          int*, true);
    TEST (std::is_convertible,       volatile int[],       volatile int*, true);
    TEST (std::is_convertible, const volatile int[], const volatile int*, true);

    // add cv-qualifiers
    TEST (std::is_convertible, int[], const          int*, true);
    TEST (std::is_convertible, int[],       volatile int*, true);
    TEST (std::is_convertible, int[], const volatile int*, true);

    // strip cv-qualifiers
    TEST (std::is_convertible, const          int[], int*, false);
    TEST (std::is_convertible,       volatile int[], int*, false);
    TEST (std::is_convertible, const volatile int[], int*, false);

    //
    // pointer to array conversions should all fail
    //

    TEST (std::is_convertible,                int*,                int[], false);
    TEST (std::is_convertible, const          int*, const          int[], false);
    TEST (std::is_convertible,       volatile int*,       volatile int[], false);
    TEST (std::is_convertible, const volatile int*, const volatile int[], false);

    // add cv-qualifiers
    TEST (std::is_convertible, int*, const          int[], false);
    TEST (std::is_convertible, int*,       volatile int[], false);
    TEST (std::is_convertible, int*, const volatile int[], false);

    // strip cv-qualifiers
    TEST (std::is_convertible, const          int*, int[], false);
    TEST (std::is_convertible,       volatile int*, int[], false);
    TEST (std::is_convertible, const volatile int*, int[], false);

    //// from an abstract type is allowed
    //TEST (std::is_convertible, abstract_, int, false);
    //TEST (std::is_convertible, derived_t<abstract_>, abstract_, false);

    TEST (std::is_convertible, long*, void*, true);
    TEST (std::is_convertible, void*, long*, false);

    // function to function conversions fail
    TEST (std::is_convertible, int (), int (), false);
    TEST (std::is_convertible, int (), int (char, long), false);

    // function to function pointer
    TEST (std::is_convertible, int (), int (*)(), true);
    TEST (std::is_convertible, int (char, long), int (*)(char, long), true);
    TEST (std::is_convertible, int (), int (*)(char), false);

    // function to function reference
    TEST (std::is_convertible, int (), int (&)(), true);
    TEST (std::is_convertible, int (char, long), int (&)(char, long), true);
    TEST (std::is_convertible, int (), int (&)(char), false);

    TEST (std::is_convertible, int*, void*, true);

#if defined (_MSC_VER) || defined (__IBMCPP__)
    // microsoft language extension allows this conversion, and that
    // extension is enabled by default.
    TEST (std::is_convertible, int (*)(), void*, true);
#else
    TEST (std::is_convertible, int (*)(), void*, false);
#endif

    TEST (std::is_convertible,
          int (*)(derived_t<struct_A>*),
          int (*)(struct_A*), false);

    TEST (std::is_convertible,
          int (*)(struct_A*),
          int (*)(derived_t<struct_A>*), false);

    // pointer to derived member convertible to
    // pointer to base member
    TEST (std::is_convertible,
          int derived_t<struct_A>::*,
          int struct_A::*, false);

    // pointer to base member convertible to
    // pointer to derived member
    TEST (std::is_convertible,
          int struct_A::*,
          int derived_t<struct_A>::*, true);

    TEST (std::is_convertible, int, double, true);
    TEST (std::is_convertible, const int, double, true);
}

/**************************************************************************/

static int run_test (int, char*[])
{
    test_is_same ();
    test_is_base_of ();
    test_is_convertible ();

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
                    "meta.rel",
                    0 /* no comment */,
                    run_test,
                    0);
}
