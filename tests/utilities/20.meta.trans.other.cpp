// -*- C++ -*-
/***************************************************************************
 *
 * 20.meta.trans.other.cpp - test exercising meta.trans.other
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

#include <rw_printf.h> // for rwsprintfa()

#include <stdlib.h>    // for free()
#include <stddef.h>    // for size_t

/**************************************************************************/

template <class T>
struct is_char
{
    enum { val = 0 };
};

_RWSTD_SPECIALIZED_CLASS
struct is_char<char>
{
    enum { val = 1 };
};

// this function should available _only_ if T is char
template <class T>
int
enabled_if_char (typename std::enable_if<is_char<T>::val>::type* = 0)
{
    return 1;
}

// this function should be available if T is _not_ char
template <class T>
int
enabled_if_char (typename std::enable_if<!is_char<T>::val>::type* = 0)
{
    return 0;
}

// must have external linkage
int _cond_if_char (void*)
{
    return 0;
}

// must have external linkage
int _cond_if_char (long*)
{
    return 1;
}

template <class T>
int cond_if_char ()
{
    typedef typename
    std::conditional<is_char<T>::val, long, void>::type selector;

    return _cond_if_char ((selector*)0);
}


/**************************************************************************/

void test_trait (int line,
                 bool select,
                 const char* trait,
                 const char* typeT,
                 const char* typeU,
                 bool success)
{
    rw_assert (success, 0, line,
               "%s<%b, %s, %s>::type is%{?}n't%{;} %s as expected",
               trait, select, typeT, typeU, !success, select ? typeT : typeU);
}

void test_trait (int line, const char* trait,
                 const char* typeT, const char* typeU,
                 bool success)
{
    rw_assert (success, 0, line,
               "%s<%s>::type is%{?}n't%{;} %s as expected",
               trait, typeT, !success, typeU);
}

void test_enable_if (int line, const char* typeT, int got, int exp)
{
    const bool success = got == exp;

    rw_assert (success, 0, line, "%s is%{?}n't%{;} %b as expected",
               typeT, !success, exp);
}

void test_aligned_storage(int line,
                          size_t exp_sz, size_t got_sz,
                          size_t exp_al, size_t got_al)
{
    //
    rw_assert (exp_sz <= got_sz, 0, line,
        "std::aligned_storage<%zu, %zu>::type size is %zu; "
        "expected %zu or more", exp_sz, exp_al, got_sz, exp_sz);

    if (exp_al != 0) {
        // alignment should be greater than or equal to expected
        rw_assert (exp_al <= got_al, 0, line,
            "std::aligned_storage<%zu, %zu>::type alignment is %zu; "
            "expected %zu or more", exp_sz, exp_al, got_al, exp_al);
    }
    else {
        // alignment should be less than or equal to the size of
        // the object
        rw_assert (got_al <= exp_sz, 0, line,
            "std::aligned_storage<%zu, %zu>::type alignment is %zu; "
            "expected %zu or less", exp_sz, exp_al, got_al, exp_sz);
    }

    const bool is_pow_2 = (got_al & (got_al - 1)) == 0;
    rw_assert (is_pow_2, 0, line,
        "std::aligned_storage<%zu, %zu>::type alignment expected "
        "to be a power-of-two; got %zu", exp_sz, exp_al, got_al);
}

/**************************************************************************/

static void test_aligned_storage ()
{
#ifdef _RWSTD_NO_ALIGN_TRAITS

    rw_warn (0, 0, __LINE__,
             "test_aligned_storage disabled because "
             "_RWSTD_NO_ALIGN_TRAITS is defined");

#else

#define TEST(Size,Align)                                           \
    {                                                              \
      typedef std::aligned_storage<Size, Align>::type storage_t;   \
      test_aligned_storage(__LINE__,                               \
          Size, sizeof (storage_t), Align,                         \
          std::alignment_of<storage_t>::value);                    \
    } typedef void __dummy

    TEST (1, 1);
    TEST (1, 2);
    TEST (1, 4);
    TEST (1, 8);

    TEST (9, 1);
    TEST (9, 2);
    TEST (9, 4);
    TEST (9, 8);

    TEST (55, 1);
    TEST (23, 2);
    TEST (17, 4);
    TEST (19, 8);

#undef  TEST
#define TEST(Size)                                                 \
    {                                                              \
      typedef std::aligned_storage<Size>::type storage_t;          \
      test_aligned_storage(__LINE__,                               \
          Size, sizeof (storage_t), 0,                             \
          std::alignment_of<storage_t>::value);                    \
    } typedef void __dummy

    // test default alignment
    TEST (1);
    TEST (2);
    TEST (4);
    TEST (8);

    TEST (3);
    TEST (5);
    TEST (7);
    TEST (9);

    TEST (55);
    TEST (23);
    TEST (17);
    TEST (19);

#undef  TEST

#endif // _RWSTD_NO_ALIGN_TRAITS
}

/**************************************************************************/

#ifndef _RWSTD_NO_ALIGN_TRAITS

struct null_t { };

// get the maximum of 8 values
size_t max8(size_t a1, size_t a2, size_t a3, size_t a4,
            size_t a5, size_t a6, size_t a7, size_t a8)
{
    size_t r = a1;

    if (r < a2) r = a2;
    if (r < a3) r = a3;
    if (r < a4) r = a4;
    if (r < a5) r = a5;
    if (r < a6) r = a6;
    if (r < a7) r = a7;
    if (r < a8) r = a8;

    return r;
}

template <size_t Len,
          class T1         , class T2 = null_t,
          class T3 = null_t, class T4 = null_t,
          class T5 = null_t, class T6 = null_t,
          class T7 = null_t, class T8 = null_t>
struct aligned_union_tester
{
    static void test (int line,
               const char* t1    , const char* t2 = 0,
               const char* t3 = 0, const char* t4 = 0,
               const char* t5 = 0, const char* t6 = 0,
               const char* t7 = 0, const char* t8 = 0)
    {
        const char* arr[] = {
            t1, t2, t3, t4, t5, t6, t7, t8
        };

        char* pbuf = 0;
        size_t bufsz = 0;

        // get the list of template arguments in one buffer
        rw_asnprintf(&pbuf, &bufsz, "%zu", Len);
        for (size_t i = 0; i < sizeof (arr) / sizeof (*arr); ++i)
        {
            if (!arr [i])
                break;

            rw_asnprintf (&pbuf, &bufsz, "%{+}, %s", arr [i]);
        }

        typedef std::aligned_union<Len,T1,T2,T3,T4,T5,T6,T7,T8> aligned_t;
        typedef typename aligned_t::type aligned_type_t;

        const bool pass1 =    std::alignment_of<aligned_type_t>::value
                           == aligned_t::alignment_value;
        rw_assert (pass1, 0, line,
                   "std::aligned_union<%s>::alignment_value is %zu; "
                   "expected %zu",
                   pbuf, aligned_t::alignment_value, 
                   std::alignment_of<aligned_type_t>::value); 

        const size_t exp_al = max8(std::alignment_of<T1>::value,
                                   std::alignment_of<T2>::value,
                                   std::alignment_of<T3>::value,
                                   std::alignment_of<T4>::value,
                                   std::alignment_of<T5>::value,
                                   std::alignment_of<T6>::value,
                                   std::alignment_of<T7>::value,
                                   std::alignment_of<T8>::value);
        const size_t got_al = std::alignment_of<aligned_type_t>::value;

        const bool pass2 = exp_al == got_al;
        rw_assert (pass2, 0, line,
                   "std::aligned_union<%s>::type alignment is %zu; "
                   "expected %zu",
                   pbuf, got_al, exp_al); 

        const size_t min_sz = Len;
        const size_t got_sz = sizeof (aligned_type_t);

        const bool pass3 = min_sz <= got_sz;

        rw_assert (pass3, 0, __LINE__,
                   "std::aligned_union<%s>::type size is %zu; expected "
                   "at least %zu",
                   pbuf, got_sz, min_sz);

        free (pbuf);
    }
};

struct struct_t { };

#endif // !_RWSTD_NO_ALIGN_TRAITS


static void test_aligned_union ()
{
#ifdef _RWSTD_NO_ALIGN_TRAITS

    rw_warn (0, 0, __LINE__,
             "test_aligned_union disabled because "
             "_RWSTD_NO_ALIGN_TRAITS is defined");

#else

#define TEST(Len,T1) \
    aligned_union_tester<Len,T1>::test(__LINE__,#T1)

    TEST (1, char);
    TEST (1, long);
    TEST (1, void*);
    TEST (1, void (struct_t::*)());

    TEST (2, char);
    TEST (2, long);
    TEST (2, void*);
    TEST (2, void (struct_t::*)());

    TEST (12, char);
    TEST (12, long);
    TEST (12, void*);
    TEST (12, void (struct_t::*)());

#undef  TEST
#define TEST(Len,T1,T2) \
    aligned_union_tester<Len,T1,T2>::test(__LINE__,#T1,#T2)

    TEST (11, char, long);
    TEST (134, long, long);
    TEST (7, void*, long);
    TEST (1, void (struct_t::*)(), long);

    TEST (2, char, long);
    TEST (2, long, long);
    TEST (2, void*, long);
    TEST (2, void (struct_t::*)(), long);

#undef  TEST
#define TEST(Len,T1,T2,T3,T4,T5) \
    aligned_union_tester<Len,T1,T2,T3,T4,T5>::test(__LINE__,#T1,#T2,#T3,#T4,#T5)

    TEST (12, char, short, int, long, void*);
    TEST (13, void*, long, long, long, int);
    TEST (17, void (struct_t::*)(), long, int, void*, char);

#undef  TEST

#endif // _RWSTD_NO_ALIGN_TRAITS
}

/**************************************************************************/

static void test_decay ()
{
#define TEST(Trait,TypeT,TypeU)                                \
    test_trait(__LINE__, #Trait, #TypeT, #TypeU,               \
               std::is_same<Trait<TypeT>::type, TypeU>::value)

    // equal to remove_extent<remove_reference<T>::type>::type* for arrays
    TEST (std::decay, int[ ], int*);
    TEST (std::decay, int[2], int*);
    TEST (std::decay, int[2][2], int (*)[2]);

    // equal to add_pointer<remove_reference<U>::type>::type for functions
    TEST (std::decay, int(), int (*)());
    TEST (std::decay, int(char, long), int (*)(char, long));

    TEST (std::decay, int(&)(), int (*)());
    TEST (std::decay, int(&)(char, long), int (*)(char, long));

    // otherwise equal to remove_cv<remove_reference<U>::type>::type
    TEST (std::decay,                int, int);
    TEST (std::decay, const          int, int);
    TEST (std::decay,       volatile int, int);
    TEST (std::decay, const volatile int, int);

    TEST (std::decay,                int&, int);
    TEST (std::decay, const          int&, int);
    TEST (std::decay,       volatile int&, int);
    TEST (std::decay, const volatile int&, int);

#undef TEST
}

/**************************************************************************/

enum enum_t { E_a = 1 };

static void test_enable_if ()
{
    typedef std::enable_if<true, int>::type int_type;

#define TEST(Cond,Expect) \
    test_enable_if(__LINE__, #Cond, Cond, Expect)

    TEST (enabled_if_char<         char>(), 1);
    TEST (enabled_if_char<  signed char>(), 0);
    TEST (enabled_if_char<unsigned char>(), 0);

    TEST (enabled_if_char<         short>(), 0);
    TEST (enabled_if_char<  signed short>(), 0);
    TEST (enabled_if_char<unsigned short>(), 0);

    TEST (enabled_if_char<         int>(), 0);
    TEST (enabled_if_char<  signed int>(), 0);
    TEST (enabled_if_char<unsigned int>(), 0);

    TEST (enabled_if_char<         long>(), 0);
    TEST (enabled_if_char<  signed long>(), 0);
    TEST (enabled_if_char<unsigned long>(), 0);

    // miscellaneous other stuff to try and expose problems
    TEST (enabled_if_char<char    ()>(), 0);
    TEST (enabled_if_char<char (*)()>(), 0);
    TEST (enabled_if_char<char (&)()>(), 0);

    TEST (enabled_if_char<char    [1]>(), 0);
    TEST (enabled_if_char<char (*)[1]>(), 0);
    TEST (enabled_if_char<char (&)[1]>(), 0);

    TEST (enabled_if_char<enum_t>(), 0);

#undef TEST
}

/**************************************************************************/

static void test_conditional ()
{
#define TEST(Cond,Expect) \
    test_enable_if(__LINE__, #Cond, Cond, Expect)

    TEST (cond_if_char<void>(), 0);
    TEST (cond_if_char<char>(), 1);

#undef  TEST
#define TEST(Trait,Select,TypeT,TypeU,TypeV)                             \
    test_trait(__LINE__, Select, #Trait, #TypeT, #TypeU,                 \
               std::is_same<Trait<Select,TypeT,TypeU>::type, TypeV>::value)

    TEST (std::conditional,  true, char, long, char);
    TEST (std::conditional, false, char, long, long);

#undef TEST
}

/**************************************************************************/

static int run_test (int, char*[])
{
    test_aligned_storage ();
    test_aligned_union ();
    test_decay ();
    test_enable_if ();
    test_conditional ();

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
                    "meta.trans.other",
                    0 /* no comment */,
                    run_test,
                    0);
}

