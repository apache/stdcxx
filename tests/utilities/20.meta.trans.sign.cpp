// -*- C++ -*-
/***************************************************************************
 *
 * 20.meta.trans.sign.cpp - test exercising meta.trans.sign
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

template <class T, class U>
struct test_is_same
{
    enum { value = 0 };
};

template <class T>
struct test_is_same<T,T>
{
    enum { value = 1 };
};

template <class T>
int test_cv_qual (T&) {
    return 0;
}

template <class T>
int test_cv_qual (const T&) {
    return 1;
}

template <class T>
int test_cv_qual (volatile T&) {
    return 2;
}

template <class T>
int test_cv_qual (const volatile T&) {
    return 3;
}

enum enum_SS { Ess_v = _RWSTD_SCHAR_MAX };
enum enum_MS { Ems_v = _RWSTD_INT_MAX   };
enum enum_LS { Els_v = _RWSTD_LONG_MAX  };

enum enum_SU { Esu_v = _RWSTD_UCHAR_MAX };
enum enum_MU { Emu_v = _RWSTD_UINT_MAX  };
enum enum_LU { Elu_v = _RWSTD_ULONG_MAX };

/**************************************************************************/

const char* get_type_name (const volatile void*)
{
    return "unknown";
}

#define _RETURN_TYPE_NAME(T)                                         \
    const char* get_type_name (T*) { return #T; }

#define _RETURN_TYPE_NAME_CV(T)                                      \
    _RETURN_TYPE_NAME(T)                                             \
    _RETURN_TYPE_NAME(const T)                                       \
    _RETURN_TYPE_NAME(volatile T)                                    \
    _RETURN_TYPE_NAME(const volatile T)

_RETURN_TYPE_NAME_CV(bool)
_RETURN_TYPE_NAME_CV(char)

#ifndef _RWSTD_NO_WCHAR_T
_RETURN_TYPE_NAME_CV(wchar_t)
#endif // _RWSTD_NO_WCHAR_T

_RETURN_TYPE_NAME_CV(  signed char)
_RETURN_TYPE_NAME_CV(unsigned char)

_RETURN_TYPE_NAME_CV(  signed short)
_RETURN_TYPE_NAME_CV(unsigned short)

_RETURN_TYPE_NAME_CV(  signed int)
_RETURN_TYPE_NAME_CV(unsigned int)

_RETURN_TYPE_NAME_CV(  signed long)
_RETURN_TYPE_NAME_CV(unsigned long)

#ifndef _RWSTD_NO_LONG_LONG
_RETURN_TYPE_NAME_CV(  signed long long)
_RETURN_TYPE_NAME_CV(unsigned long long)
#endif // _RWSTD_NO_LONG_LONG

void test_trait (int line, bool same,
                 const char* trait,
                 const char* typeT,
                 const char* typeU,
                 const char* typeV)
{
    rw_assert (same, 0, line,
               "%s<%s>::type is%{?}n't%{;} %s; got %s",
               trait, typeT, !same, typeU, typeV);
}

#define _TEST(Trait,TypeT,TypeU)                                     \
    test_trait(__LINE__,                                             \
               std::is_same<Trait<TypeT>::type, TypeU>::value,       \
               #Trait, #TypeT, #TypeU, get_type_name((Trait<TypeT>::type*)0))

#define TEST(Trait,TypeT,TypeU)                                      \
    _TEST(Trait, TypeT, TypeU);                                      \
    _TEST(Trait, const TypeT, const TypeU);                          \
    _TEST(Trait, volatile TypeT, volatile TypeU);                    \
    _TEST(Trait, const volatile TypeT, const volatile TypeU)



void test_sign_and_cv (int line,
                       const char* trait_name,
                       const char* type_name,
                       size_t got_size, size_t exp_size,
                       bool got_sign, bool exp_sign,
                       bool got_c, bool exp_c,
                       bool got_v, bool exp_v)
{
    const char* qualifiers[] = {
        "unqualified", "const", "volatile", "cv-qualified"
    };

    const unsigned got_cv = got_c + (got_v << 1);
    const unsigned exp_cv = exp_c + (exp_v << 1);

    const bool success =    got_size == exp_size
                         && got_cv   == exp_cv
                         && got_sign == exp_sign;

    rw_assert (success, 0, line,
        "%s<%s>::type is a %zu-byte %s %{?}un%{;}signed type, "
               "expected a %zu-byte %s %{?}un%{;}signed type.",
        trait_name, type_name,
        got_size, qualifiers[got_cv], !got_sign,
        exp_size, qualifiers[exp_cv], !exp_sign);
}

#define _TEST_SIGN_AND_CV(Trait,Type,Signed)                         \
    test_sign_and_cv(__LINE__, #Trait, #Type,                        \
        sizeof (Trait<Type>::type),                                  \
        sizeof (Type),                                               \
        std::is_signed<Trait<Type>::type>::value,                    \
        Signed,                                                      \
        std::is_const<Trait<Type>::type>::value,                     \
        std::is_const<Type>::value,                                  \
        std::is_volatile<Trait<Type>::type>::value,                  \
        std::is_volatile<Type>::value)

#define TEST_MAKE_SIGNED(Type)                                         \
    _TEST_SIGN_AND_CV (std::make_signed, Type, true);                  \
    _TEST_SIGN_AND_CV (std::make_signed, const Type, true);            \
    _TEST_SIGN_AND_CV (std::make_signed, volatile Type, true);         \
    _TEST_SIGN_AND_CV (std::make_signed, const volatile Type, true)

#define TEST_MAKE_UNSIGNED(Type)                                       \
    _TEST_SIGN_AND_CV (std::make_unsigned, Type, false);               \
    _TEST_SIGN_AND_CV (std::make_unsigned, const Type, false);         \
    _TEST_SIGN_AND_CV (std::make_unsigned, volatile Type, false);      \
    _TEST_SIGN_AND_CV (std::make_unsigned, const volatile Type, false)

/**************************************************************************/

static void test_make_signed ()
{
#if (_RWSTD_CHAR_MIN < 0)
    TEST (std::make_signed,   char, char);
#else
    TEST (std::make_signed,   char, signed char);
#endif

    TEST (std::make_signed,   signed char, signed char);
    TEST (std::make_signed, unsigned char, signed char); // unsure

    TEST (std::make_signed,   signed short, signed short);
    TEST (std::make_signed, unsigned short, signed short);

    TEST (std::make_signed,   signed int, signed int);
    TEST (std::make_signed, unsigned int, signed int);

    TEST (std::make_signed,   signed long, signed long);
    TEST (std::make_signed, unsigned long, signed long);

#ifndef _RWSTD_NO_LONG_LONG
    TEST (std::make_signed,   signed long long, signed long long);
    TEST (std::make_signed, unsigned long long, signed long long);
#endif   // _RWSTD_NO_LONG_LONG

    TEST_MAKE_SIGNED (char);

#ifndef _RWSTD_NO_NATIVE_WCHAR_T
    TEST_MAKE_SIGNED (wchar_t);
#endif   // _RWSTD_NO_NATIVE_WCHAR_T

    TEST_MAKE_SIGNED (enum_SS);
    TEST_MAKE_SIGNED (enum_MS);
    TEST_MAKE_SIGNED (enum_LS);
    TEST_MAKE_SIGNED (enum_SU);
    TEST_MAKE_SIGNED (enum_MU);
    TEST_MAKE_SIGNED (enum_LU);
}

static void test_make_unsigned ()
{
#if (_RWSTD_CHAR_MIN < 0)
    TEST (std::make_unsigned,   char, unsigned char);
#else
    TEST (std::make_unsigned,   char, char);
#endif

    TEST (std::make_unsigned,   signed char, unsigned char); // unsure
    TEST (std::make_unsigned, unsigned char, unsigned char);

    TEST (std::make_unsigned,   signed short, unsigned short);
    TEST (std::make_unsigned, unsigned short, unsigned short);

    TEST (std::make_unsigned,   signed int, unsigned int);
    TEST (std::make_unsigned, unsigned int, unsigned int);

    TEST (std::make_unsigned,   signed long, unsigned long);
    TEST (std::make_unsigned, unsigned long, unsigned long);

#ifndef _RWSTD_NO_LONG_LONG
    TEST (std::make_unsigned,   signed long long, unsigned long long);
    TEST (std::make_unsigned, unsigned long long, unsigned long long);
#endif   // _RWSTD_NO_LONG_LONG

    TEST_MAKE_UNSIGNED (char);

#ifndef _RWSTD_NO_NATIVE_WCHAR_T
    TEST_MAKE_UNSIGNED (wchar_t);
#endif   // _RWSTD_NO_NATIVE_WCHAR_T

    TEST_MAKE_UNSIGNED (enum_SS);
    TEST_MAKE_UNSIGNED (enum_MS);
    TEST_MAKE_UNSIGNED (enum_LS);
    TEST_MAKE_UNSIGNED (enum_SU);
    TEST_MAKE_UNSIGNED (enum_MU);
    TEST_MAKE_UNSIGNED (enum_LU);
}

/**************************************************************************/

static int run_test (int, char*[])
{
    test_make_signed ();
    test_make_unsigned ();

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
                    "meta.trans.sign",
                    0 /* no comment */,
                    run_test,
                    0);
}
