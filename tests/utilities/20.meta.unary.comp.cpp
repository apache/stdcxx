// -*- C++ -*-
/***************************************************************************
 *
 * 20.meta.unary.comp.cpp - test exercising meta.unary.comp
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

class class_t {
    long l_; float f_;
};

struct struct_t {
    long l_; float f_;
};

union union_t {
    long l_; float f_;
};

enum enum_t {
    E_a, E_b, E_c
};

/**************************************************************************/

void test_trait (int line, bool value, bool expect,
                 const char* trait, const char* type)
{
    rw_assert (value == expect, 0, line,
               "%s<%s> is %b, expected %b",
               trait, type, value, expect);
}

#define _TEST(Trait,Type,Expect)                                     \
    test_trait (__LINE__, Trait<Type>::value, Expect, #Trait, #Type)                  

#define TEST(Trait,Type,Expect)                                      \
    _TEST(Trait, Type, Expect);                                      \
    _TEST(Trait, std::add_const<Type>::type, Expect);                \
    _TEST(Trait, std::add_volatile<Type>::type, Expect);             \
    _TEST(Trait, std::add_cv<Type>::type, Expect)

/**************************************************************************/

void test_is_arithmetic ()
{
    // void
    TEST (std::is_arithmetic, void, false);

    // integral
    TEST (std::is_arithmetic,          bool, true);

    TEST (std::is_arithmetic,          char, true);
    TEST (std::is_arithmetic,   signed char, true);
    TEST (std::is_arithmetic, unsigned char, true);

#ifndef _RWSTD_NO_WCHAR_T
    TEST (std::is_arithmetic,       wchar_t, true);
#endif   // _RWSTD_NO_WCHAR_T

    TEST (std::is_arithmetic,          short, true);
    TEST (std::is_arithmetic,   signed short, true);
    TEST (std::is_arithmetic, unsigned short, true);

    TEST (std::is_arithmetic,          int, true);
    TEST (std::is_arithmetic,   signed int, true);
    TEST (std::is_arithmetic, unsigned int, true);

    TEST (std::is_arithmetic,          long, true);
    TEST (std::is_arithmetic,   signed long, true);
    TEST (std::is_arithmetic, unsigned long, true);

#ifndef _RWSTD_NO_LONG_LONG
    TEST (std::is_arithmetic,          long long, true);
    TEST (std::is_arithmetic,   signed long long, true);
    TEST (std::is_arithmetic, unsigned long long, true);
#endif   // _RWSTD_NO_LONG_LONG

    // floating point
    TEST (std::is_arithmetic,       float, true);
    TEST (std::is_arithmetic,      double, true);
    TEST (std::is_arithmetic, long double, true);

    // array
    TEST (std::is_arithmetic, long [4], false);
    TEST (std::is_arithmetic, float [3][3], false);
    TEST (std::is_arithmetic, char* [4], false);
    TEST (std::is_arithmetic, class_t [], false);
    TEST (std::is_arithmetic, struct_t [][2], false);

    // pointer
    TEST (std::is_arithmetic, long*, false);
    TEST (std::is_arithmetic, char**, false);

    // reference
    TEST (std::is_arithmetic, int&, false);
    TEST (std::is_arithmetic, union_t&, false);
    TEST (std::is_arithmetic, class_t&, false);
    TEST (std::is_arithmetic, union_t&, false);
    TEST (std::is_arithmetic, class_t&, false);
    TEST (std::is_arithmetic, double&, false);

    // pointer to member
    TEST (std::is_arithmetic, long (class_t::*), false);
    TEST (std::is_arithmetic, int (struct_t::*), false);

    // pointer to member function
    TEST (std::is_arithmetic, double (class_t::*)(), false);
    TEST (std::is_arithmetic, long (class_t::*)(float), false);
    TEST (std::is_arithmetic, double (struct_t::*)(), false);
    TEST (std::is_arithmetic, long (struct_t::*)(float), false);
    
    // enumeration
    TEST (std::is_arithmetic, enum_t, false);

    // union
    TEST (std::is_arithmetic, union_t, false);

    // class
    TEST (std::is_arithmetic, class_t, false);
    TEST (std::is_arithmetic, struct_t, false);

    // function
    TEST (std::is_arithmetic, long (), false);
    TEST (std::is_arithmetic, float (char), false);

    // function pointer
    TEST (std::is_arithmetic, long (*)(), false);
    TEST (std::is_arithmetic, double (*)(int), false);

    // function reference
    TEST (std::is_arithmetic, long (&)(), false);

    // member function reference
    //TEST (std::is_arithmetic, long (class_t::&), false);
}

void test_is_fundamental ()
{
    // void
    TEST (std::is_fundamental, void, true);

    // integral
    TEST (std::is_fundamental,          bool, true);

    TEST (std::is_fundamental,          char, true);
    TEST (std::is_fundamental,   signed char, true);
    TEST (std::is_fundamental, unsigned char, true);

#ifndef _RWSTD_NO_WCHAR_T
    TEST (std::is_fundamental,       wchar_t, true);
#endif   // _RWSTD_NO_WCHAR_T

    TEST (std::is_fundamental,          short, true);
    TEST (std::is_fundamental,   signed short, true);
    TEST (std::is_fundamental, unsigned short, true);

    TEST (std::is_fundamental,          int, true);
    TEST (std::is_fundamental,   signed int, true);
    TEST (std::is_fundamental, unsigned int, true);

    TEST (std::is_fundamental,          long, true);
    TEST (std::is_fundamental,   signed long, true);
    TEST (std::is_fundamental, unsigned long, true);

#ifndef _RWSTD_NO_LONG_LONG
    TEST (std::is_fundamental,          long long, true);
    TEST (std::is_fundamental,   signed long long, true);
    TEST (std::is_fundamental, unsigned long long, true);
#endif   // _RWSTD_NO_LONG_LONG

    // floating point
    TEST (std::is_fundamental,       float, true);
    TEST (std::is_fundamental,      double, true);
    TEST (std::is_fundamental, long double, true);

    // array
    TEST (std::is_fundamental, long [4], false);
    TEST (std::is_fundamental, float [3][3], false);
    TEST (std::is_fundamental, char* [4], false);
    TEST (std::is_fundamental, class_t [], false);
    TEST (std::is_fundamental, struct_t [][2], false);

    // pointer
    TEST (std::is_fundamental, long*, false);
    TEST (std::is_fundamental, char**, false);

    // reference
    TEST (std::is_fundamental, int&, false);
    TEST (std::is_fundamental, union_t&, false);
    TEST (std::is_fundamental, class_t&, false);
    TEST (std::is_fundamental, union_t&, false);
    TEST (std::is_fundamental, class_t&, false);
    TEST (std::is_fundamental, double&, false);

    // pointer to member
    TEST (std::is_fundamental, long (class_t::*), false);
    TEST (std::is_fundamental, int (struct_t::*), false);

    // pointer to member function
    TEST (std::is_fundamental, double (class_t::*)(), false);
    TEST (std::is_fundamental, long (class_t::*)(float), false);
    TEST (std::is_fundamental, double (struct_t::*)(), false);
    TEST (std::is_fundamental, long (struct_t::*)(float), false);
    
    // enumeration
    TEST (std::is_fundamental, enum_t, false);

    // union
    TEST (std::is_fundamental, union_t, false);

    // class
    TEST (std::is_fundamental, class_t, false);
    TEST (std::is_fundamental, struct_t, false);

    // function
    TEST (std::is_fundamental, long (), false);
    TEST (std::is_fundamental, float (char), false);

    // function pointer
    TEST (std::is_fundamental, long (*)(), false);
    TEST (std::is_fundamental, double (*)(int), false);

    // function reference
    TEST (std::is_fundamental, long (&)(), false);

    // member function reference
    //TEST (std::is_fundamental, long (class_t::&), false);
}

void test_is_object ()
{
    // an object type is a (possibly cv-qualified) type that is not a
    // function type, not a reference type, and not a void type.

    // void
    TEST (std::is_object, void, false);

    // integral
    TEST (std::is_object,          bool, true);

    TEST (std::is_object,          char, true);
    TEST (std::is_object,   signed char, true);
    TEST (std::is_object, unsigned char, true);

#ifndef _RWSTD_NO_WCHAR_T
    TEST (std::is_object,       wchar_t, true);
#endif   // _RWSTD_NO_WCHAR_T

    TEST (std::is_object,          short, true);
    TEST (std::is_object,   signed short, true);
    TEST (std::is_object, unsigned short, true);

    TEST (std::is_object,          int, true);
    TEST (std::is_object,   signed int, true);
    TEST (std::is_object, unsigned int, true);

    TEST (std::is_object,          long, true);
    TEST (std::is_object,   signed long, true);
    TEST (std::is_object, unsigned long, true);

#ifndef _RWSTD_NO_LONG_LONG
    TEST (std::is_object,          long long, true);
    TEST (std::is_object,   signed long long, true);
    TEST (std::is_object, unsigned long long, true);
#endif   // _RWSTD_NO_LONG_LONG

    // floating point
    TEST (std::is_object,       float, true);
    TEST (std::is_object,      double, true);
    TEST (std::is_object, long double, true);

    // array
    TEST (std::is_object, long [4], true);
    TEST (std::is_object, float [3][3], true);
    TEST (std::is_object, char* [4], true);
    TEST (std::is_object, class_t [], true);
    TEST (std::is_object, struct_t [][2], true);

    // pointer
    TEST (std::is_object, long*, true);
    TEST (std::is_object, char**, true);

    // reference
    TEST (std::is_object, int&, false);
    TEST (std::is_object, union_t&, false);
    TEST (std::is_object, class_t&, false);
    TEST (std::is_object, union_t&, false);
    TEST (std::is_object, class_t&, false);
    TEST (std::is_object, double&, false);

    // pointer to member
    TEST (std::is_object, long (class_t::*), true);
    TEST (std::is_object, int (struct_t::*), true);

    // pointer to member function
    TEST (std::is_object, double (class_t::*)(), true);
    TEST (std::is_object, long (class_t::*)(float), true);
    TEST (std::is_object, double (struct_t::*)(), true);
    TEST (std::is_object, long (struct_t::*)(float), true);
    
    // enumeration
    TEST (std::is_object, enum_t, true);

    // union
    TEST (std::is_object, union_t, true);

    // class
    TEST (std::is_object, class_t, true);
    TEST (std::is_object, struct_t, true);

    // function
    TEST (std::is_object, long (), false);
    TEST (std::is_object, float (char), false);

    // function pointer
    TEST (std::is_object, long (*)(), true);
    TEST (std::is_object, double (*)(int), true);

    // function reference
    TEST (std::is_object, long (&)(), false);

    // member function reference
    //TEST (std::is_object, long (class_t::&), false);
}

void test_is_scalar ()
{
    // arithmetic types, enumeration types, pointer types, pointer to member
    // types and std::nullptr_t, and cv-qualified versions of these types
    // are collectively called scalar types.

    // void
    TEST (std::is_scalar, void, false);

    // integral
    TEST (std::is_scalar,          bool, true);

    TEST (std::is_scalar,          char, true);
    TEST (std::is_scalar,   signed char, true);
    TEST (std::is_scalar, unsigned char, true);

#ifndef _RWSTD_NO_WCHAR_T
    TEST (std::is_scalar,       wchar_t, true);
#endif   // _RWSTD_NO_WCHAR_T

    TEST (std::is_scalar,          short, true);
    TEST (std::is_scalar,   signed short, true);
    TEST (std::is_scalar, unsigned short, true);

    TEST (std::is_scalar,          int, true);
    TEST (std::is_scalar,   signed int, true);
    TEST (std::is_scalar, unsigned int, true);

    TEST (std::is_scalar,          long, true);
    TEST (std::is_scalar,   signed long, true);
    TEST (std::is_scalar, unsigned long, true);

#ifndef _RWSTD_NO_LONG_LONG
    TEST (std::is_scalar,          long long, true);
    TEST (std::is_scalar,   signed long long, true);
    TEST (std::is_scalar, unsigned long long, true);
#endif   // _RWSTD_NO_LONG_LONG

    // floating point
    TEST (std::is_scalar,       float, true);
    TEST (std::is_scalar,      double, true);
    TEST (std::is_scalar, long double, true);

    // array
    TEST (std::is_scalar, long [4], false);
    TEST (std::is_scalar, float [3][3], false);
    TEST (std::is_scalar, char* [4], false);
    TEST (std::is_scalar, class_t [], false);
    TEST (std::is_scalar, struct_t [][2], false);

    // pointer
    TEST (std::is_scalar, long*, true);
    TEST (std::is_scalar, char**, true);

    // reference
    TEST (std::is_scalar, int&, false);
    TEST (std::is_scalar, union_t&, false);
    TEST (std::is_scalar, class_t&, false);
    TEST (std::is_scalar, union_t&, false);
    TEST (std::is_scalar, class_t&, false);
    TEST (std::is_scalar, double&, false);

    // pointer to member
    TEST (std::is_scalar, long (class_t::*), true);
    TEST (std::is_scalar, int (struct_t::*), true);

    // pointer to member function
    TEST (std::is_scalar, double (class_t::*)(), true);
    TEST (std::is_scalar, long (class_t::*)(float), true);
    TEST (std::is_scalar, double (struct_t::*)(), true);
    TEST (std::is_scalar, long (struct_t::*)(float), true);
    
    // enumeration
    TEST (std::is_scalar, enum_t, true);

    // union
    TEST (std::is_scalar, union_t, false);

    // class
    TEST (std::is_scalar, class_t, false);
    TEST (std::is_scalar, struct_t, false);

    // function
    TEST (std::is_scalar, long (), false);
    TEST (std::is_scalar, float (char), false);

    // function pointer
    TEST (std::is_scalar, long (*)(), true);
    TEST (std::is_scalar, double (*)(int), true);

    // function reference
    TEST (std::is_scalar, long (&)(), false);

    // member function reference
    //TEST (std::is_scalar, long (class_t::&), false);
}

void test_is_compound ()
{
    // arrays, functions, pointers, function pointers,
    // references, function references, classes, unions,
    // enumerations, pointer to non-static class members

    // void
    TEST (std::is_compound, void, false);

    // integral
    TEST (std::is_compound,          bool, false);

    TEST (std::is_compound,          char, false);
    TEST (std::is_compound,   signed char, false);
    TEST (std::is_compound, unsigned char, false);

#ifndef _RWSTD_NO_WCHAR_T
    TEST (std::is_compound,       wchar_t, false);
#endif   // _RWSTD_NO_WCHAR_T

    TEST (std::is_compound,          short, false);
    TEST (std::is_compound,   signed short, false);
    TEST (std::is_compound, unsigned short, false);

    TEST (std::is_compound,          int, false);
    TEST (std::is_compound,   signed int, false);
    TEST (std::is_compound, unsigned int, false);

    TEST (std::is_compound,          long, false);
    TEST (std::is_compound,   signed long, false);
    TEST (std::is_compound, unsigned long, false);

#ifndef _RWSTD_NO_LONG_LONG
    TEST (std::is_compound,          long long, false);
    TEST (std::is_compound,   signed long long, false);
    TEST (std::is_compound, unsigned long long, false);
#endif   // _RWSTD_NO_LONG_LONG

    // floating point
    TEST (std::is_compound,       float, false);
    TEST (std::is_compound,      double, false);
    TEST (std::is_compound, long double, false);

    // array
    TEST (std::is_compound, long [4], true);
    TEST (std::is_compound, float [3][3], true);
    TEST (std::is_compound, char* [4], true);
    TEST (std::is_compound, class_t [], true);
    TEST (std::is_compound, struct_t [][2], true);

    // pointer
    TEST (std::is_compound, void*, true);
    TEST (std::is_compound, long*, true);
    TEST (std::is_compound, char**, true);

    // reference
    TEST (std::is_compound, int&, true);
    TEST (std::is_compound, union_t&, true);
    TEST (std::is_compound, class_t&, true);
    TEST (std::is_compound, union_t&, true);
    TEST (std::is_compound, class_t&, true);
    TEST (std::is_compound, double&, true);

#ifndef _RWSTD_NO_RVALUE_REFERENCES
    TEST (std::is_compound, union_t&&, true);
    TEST (std::is_compound, double&&, true);
#endif

    // pointer to member
    TEST (std::is_compound, long (class_t::*), true);
    TEST (std::is_compound, int (struct_t::*), true);

    // pointer to member function
    TEST (std::is_compound, double (class_t::*)(), true);
    TEST (std::is_compound, long (class_t::*)(float), true);
    TEST (std::is_compound, double (struct_t::*)(), true);
    TEST (std::is_compound, long (struct_t::*)(float), true);
    
    // enumeration
    TEST (std::is_compound, enum_t, true);

    // union
    TEST (std::is_compound, union_t, true);

    // class
    TEST (std::is_compound, class_t, true);
    TEST (std::is_compound, struct_t, true);

    // function
    TEST (std::is_compound, long (), true);
    TEST (std::is_compound, float (char), true);

    // function pointer
    TEST (std::is_compound, long (*)(), true);
    TEST (std::is_compound, double (*)(int), true);

    // function reference
    TEST (std::is_compound, long (&)(), true);

    // member function reference
    typedef long (class_t::*memfun_t_ptr)();
    typedef std::remove_pointer<memfun_t_ptr>::type memfun_t;
    typedef std::add_lvalue_reference<memfun_t>::type memfun_t_ref;
    TEST (std::is_compound, memfun_t_ref, true);
}

void test_is_member_pointer ()
{
    // void
    TEST (std::is_member_pointer, void, false);

    // integral
    TEST (std::is_member_pointer,          bool, false);

    TEST (std::is_member_pointer,          char, false);
    TEST (std::is_member_pointer,   signed char, false);
    TEST (std::is_member_pointer, unsigned char, false);

#ifndef _RWSTD_NO_WCHAR_T
    TEST (std::is_member_pointer,       wchar_t, false);
#endif   // _RWSTD_NO_WCHAR_T

    TEST (std::is_member_pointer,          short, false);
    TEST (std::is_member_pointer,   signed short, false);
    TEST (std::is_member_pointer, unsigned short, false);

    TEST (std::is_member_pointer,          int, false);
    TEST (std::is_member_pointer,   signed int, false);
    TEST (std::is_member_pointer, unsigned int, false);

    TEST (std::is_member_pointer,          long, false);
    TEST (std::is_member_pointer,   signed long, false);
    TEST (std::is_member_pointer, unsigned long, false);

#ifndef _RWSTD_NO_LONG_LONG
    TEST (std::is_member_pointer,          long long, false);
    TEST (std::is_member_pointer,   signed long long, false);
    TEST (std::is_member_pointer, unsigned long long, false);
#endif   // _RWSTD_NO_LONG_LONG

    // floating point
    TEST (std::is_member_pointer,       float, false);
    TEST (std::is_member_pointer,      double, false);
    TEST (std::is_member_pointer, long double, false);

    // array
    TEST (std::is_member_pointer, long [4], false);
    TEST (std::is_member_pointer, float [3][3], false);
    TEST (std::is_member_pointer, char* [4], false);
    TEST (std::is_member_pointer, class_t [], false);
    TEST (std::is_member_pointer, struct_t [][2], false);

    // pointer
    TEST (std::is_member_pointer, long*, false);
    TEST (std::is_member_pointer, char**, false);

    // reference
    TEST (std::is_member_pointer, int&, false);
    TEST (std::is_member_pointer, union_t&, false);
    TEST (std::is_member_pointer, class_t&, false);
    TEST (std::is_member_pointer, union_t&, false);
    TEST (std::is_member_pointer, class_t&, false);
    TEST (std::is_member_pointer, double&, false);

    // pointer to member
    TEST (std::is_member_pointer, long (class_t::*), true);
    TEST (std::is_member_pointer, int (struct_t::*), true);

    // pointer to member function
    TEST (std::is_member_pointer, double (class_t::*)(), true);
    TEST (std::is_member_pointer, long (class_t::*)(float), true);
    TEST (std::is_member_pointer, double (struct_t::*)(), true);
    TEST (std::is_member_pointer, long (struct_t::*)(float), true);
    
    // enumeration
    TEST (std::is_member_pointer, enum_t, false);

    // union
    TEST (std::is_member_pointer, union_t, false);

    // class
    TEST (std::is_member_pointer, class_t, false);
    TEST (std::is_member_pointer, struct_t, false);

    // function
    TEST (std::is_member_pointer, long (), false);
    TEST (std::is_member_pointer, float (char), false);

    // function pointer
    TEST (std::is_member_pointer, long (*)(), false);
    TEST (std::is_member_pointer, double (*)(int), false);

    // function reference
    TEST (std::is_member_pointer, long (&)(), false);

    // member function reference
    //TEST (std::is_member_pointer, long (class_t::&), false);    
}

/**************************************************************************/

static int run_test (int, char*[])
{
    test_is_arithmetic ();
    test_is_fundamental ();
    test_is_object ();
    test_is_scalar ();
    test_is_compound ();
    test_is_member_pointer ();

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

int main (int argc, char*argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "meta.unary.comp",
                    0 /* no comment*/,
                    run_test,
                    0);
}
