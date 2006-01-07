/***************************************************************************
 *
 * limits.traps.cpp - test exercising std::numeric_limits::traps
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#include <limits>

#include <csetjmp>    // for longjmp, setjmp
#include <csignal>    // for SIGFPE, signal

#include <any.h>      // for rw_any_t
#include <driver.h>   // for rw_test(), ...

/**************************************************************************/

std::jmp_buf jmp_env;

extern "C" {

void handle_fpe (int)
{
    std::longjmp (jmp_env, 1);
}

}   // extern "C"

/**************************************************************************/

#ifdef _MSC_VER
   // silence useless MSVC warnings:
   // 4800: 'int' : forcing value to bool 'true' or 'false'
   // 4804: '/' : unsafe use of type 'bool' in operation
#  pragma warning (disable: 4800 4804)

   // use Structured Exception Handling to detect arithmetic exceptions
#  define TRY           __try
#  define EXCEPT(arg)   __except (arg)
#else
#  define TRY              if (1)
#  define EXCEPT(ignore)   else if (0)
#endif   // _MSC_VER


template <class numT>
numT test_traps (numT, int lineno, bool)
{
    static const char* const tname = rw_any_t (numT ()).type_name ();

    rw_info (0, 0, 0, "std::numeric_limits<%s>::traps", tname);

    const bool traps = std::numeric_limits<numT>::traps;

#ifdef SIGFPE
    std::signal (SIGFPE, handle_fpe);
#else   // if !defined (SIGFPE)
    if (!rw_warn (!traps, 0, lineno,
                  "SIGFPE not #defined and numeric_limits<%s>::traps == true, "
                  "cannot test", tname)) {
        return numT ();
    }
#endif   // SIGFPE

    numT result = numT ();

    const int jumped = setjmp (jmp_env);

    volatile numT zero = numT (jumped);
    volatile numT one  = numT (1);

    bool trapped = false;

    if (jumped) {
        result = zero / one;

        trapped = true;
    }
    else {
        TRY {
            result = one / zero;
        }
        EXCEPT (1) {
            trapped = true;
        }
    }

    rw_assert (trapped == traps, 0, lineno,
               "numeric_limits<%s>::traps == %b, got %b",
               tname,  trapped, traps);

    return result;
}

/**************************************************************************/

static int
run_test (int, char*[])
{
#define TEST(T, floating)   test_traps ((T)0, __LINE__, floating)

#ifndef _RWSTD_NO_NATIVE_BOOL
    TEST (bool, false);
#endif   // _RWSTD_NO_NATIVE_BOOL

    TEST (char, false);
    TEST (signed char, false);
    TEST (unsigned char, false);

    TEST (short, false);
    TEST (unsigned short, false);
    TEST (int, false);
    TEST (unsigned int, false);
    TEST (long, false);
    TEST (unsigned long, false);

#ifndef _RWSTD_NO_LONG_LONG
    TEST (_RWSTD_LONG_LONG, false);
    TEST (unsigned _RWSTD_LONG_LONG, false);
#endif   // _RWSTD_NO_LONG_LONG

#ifndef _RWSTD_NO_NATIVE_WCHAR_T
    TEST (wchar_t, false);
#endif   // _RWSTD_NO_NATIVE_WCHAR_T

    TEST (float, true);
    TEST (double, true);

#ifndef _RWSTD_NO_LONG_DOUBLE
    TEST (long double, true);
#endif   // _RWSTD_NO_LONG_DOUBLE

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.numeric.limits.members.traps",
                    0 /* no comment */, run_test, 0, 0);
}
