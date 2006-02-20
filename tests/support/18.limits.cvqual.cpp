// -*- C++ -*-
/***************************************************************************
 *
 * 18.limits.cvqual.cpp - test exercising lib.numeric.limits
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright 2006 The Apache Software Foundation or its licensors,
 * as applicable.
 *
 * Copyright 2006 Rogue Wave Software.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 **************************************************************************/

#include <limits>
#include <driver.h>

/**************************************************************************/

template <class T>
struct UserType
{
    const T val_;

    UserType (T val = T ()): val_ (val) { /* empty */}

    operator T () const { return val_; }
};


template <class T, class CVQualifiedT>
void test_limits (const volatile T*, CVQualifiedT*,
                  const char *tname,
                  const char *quals)
{
#ifndef _RWSTD_NO_EXT_CV_QUALIFIED_LIMITS

    typedef std::numeric_limits<T>            limits;
    typedef std::numeric_limits<CVQualifiedT> cv_limits;

#  define TEST(member)                                                      \
      rw_assert (limits::member == cv_limits::member, 0, __LINE__,          \
                 "numeric_limits<%s>::%s == numeric_limits<%s %1$s>::%2$s", \
                 tname, #member, quals)
#else   // if defined (_RWSTD_NO_EXT_CV_QUALIFIED_LIMITS)

    typedef std::numeric_limits<UserType<T> > limits;
    typedef std::numeric_limits<CVQualifiedT> cv_limits;

#  define TEST(member)                                                      \
      rw_assert (limits::member == cv_limits::member, 0, __LINE__,          \
                 "numeric_limits<%s>::%s == numeric_limits<%s %1$s>::%2$s", \
                 tname, #member, quals)
#endif   // _RWSTD_NO_EXT_CV_QUALIFIED_LIMITS

    TEST (is_specialized);
    TEST (min ());
    TEST (max ());
    TEST (digits);
    TEST (digits10);
    TEST (is_signed);
    TEST (is_integer);
    TEST (is_exact);
    TEST (radix);

    TEST (epsilon ());
    TEST (round_error ());

    TEST (min_exponent);
    TEST (min_exponent10);
    TEST (max_exponent);
    TEST (max_exponent10);

    TEST (has_infinity);
    TEST (has_quiet_NaN);
    TEST (has_signaling_NaN);
    TEST (has_denorm);
    TEST (has_denorm_loss);

    TEST (infinity ());

    if (limits::is_integer) {
        TEST (quiet_NaN ());
        TEST (signaling_NaN ());
    }
    else {
        // test NaN
    }

    TEST (denorm_min ());

    TEST (is_iec559);
    TEST (is_bounded);
    TEST (is_modulo);
    TEST (traps);
    TEST (tinyness_before);
    TEST (round_style);
}

/**************************************************************************/

template <class T>
void test_limits (const volatile T*, const char *tname)
{
    typedef const T          const_T;
    typedef volatile T       volatile_T;
    typedef const volatile T const_volatile_T;
    
    rw_info (0, 0, 0, "std::numeric_limits<T> with T = cv-qualified %s", tname);

    test_limits ((T*)0, (const_T*)0, tname, "const");
    test_limits ((T*)0, (volatile_T*)0, tname, "volatile");
    test_limits ((T*)0, (const_volatile_T*)0, tname, "const volatile");
}

/**************************************************************************/

static int
run_test (int, char*[])
{
#undef TEST
#define TEST(T)   test_limits ((T*)0, _RWSTD_STRSTR (T))

#ifndef _RWSTD_NO_NATIVE_BOOL
    TEST (bool);
#endif   // _RWSTD_NO_NATIVE_BOOL

    TEST (char);
    TEST (signed char);
    TEST (unsigned char);

    TEST (short);
    TEST (unsigned short);

    TEST (int);
    TEST (unsigned);

    TEST (long);
    TEST (unsigned long);

#ifdef _RWSTD_LONG_LONG
    TEST (_RWSTD_LONG_LONG);
    TEST (unsigned _RWSTD_LONG_LONG);
#endif

    TEST (float);
    TEST (double);

#ifndef _RWSTD_NO_LONG_DOUBLE
    TEST (long double);
#endif   // _RWSTD_NO_LONG_DOUBLE

#ifndef _RWSTD_NO_NATIVE_WCHAR_T
    TEST (wchar_t);
#endif   // _RWSTD_NO_NATIVE_WCHAR_T

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.numeric.limits",

#ifdef _RWSTD_NO_EXT_CV_QUALIFIED_LIMITS
                    "testing the absence of "
#endif   // _RWSTD_NO_EXT_CV_QUALIFIED_LIMITS
                    "specializations on cv-qualifed types",
                    run_test,
                    0,
                    0);
}
