/***************************************************************************
 *
 * 27.basic.ios.cpp - test exercising the class template basic_ios
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
 * Copyright 1994-2008 Rogue Wave Software.
 *
 **************************************************************************/

// prevent the inclusion of non-essential public headers; only headers
// necessary to compile each header #included here are #included
// will produce errors if any templates defined in the header are
// instantiated without the names they use having been completely defined
#if !defined (_MSC_VER) || _MSC_VER > 1300
#  define _RWSTD_NO_REDUNDANT_DEFINITIONS
#endif

// prevent out-of-line template definitions in .cc files from being explicitly
// #included during the processing of library headers (faster compilation)
// assumes that the test doesn't instantiate those templates on types other
// than those they were explcitly instantiated on (will have no effect if
// explicit instantiation is disabled or unsupported)
#define _RWSTD_NO_TEMPLATE_DEFINITIONS

#include <ios>

// IMPORTANT: do not #include any other header before the declarations
//            below in order not to accidentally bring in any missing
//            definitions from the #included header

#ifndef _RWSTD_NO_NAMESPACE
#  define std std
#else
#  define std
#endif   // _RWSTD_NO_NAMESPACE

/**************************************************************************/

// compilation tests

// use defs from <iosfwd> to verify that it is #included

struct S { /* empty */ };

typedef std::char_traits<S>                      Traits;
typedef std::char_traits<char>                   char_Traits;

#ifndef _RWSTD_NO_WCHAR_T

typedef std::char_traits<wchar_t>                wchar_t_Traits;

#endif   // _RWSTD_NO_WCHAR_T

typedef std::allocator<S>                        Alloc;

#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES

// make sure that default template arguments are specified

typedef std::basic_ios<S>                        S_basic_ios;
typedef std::basic_streambuf<S>                  S_basic_streambuf;
typedef std::basic_istream<S>                    S_basic_istream;
typedef std::basic_ostream<S>                    S_basic_ostream;
typedef std::basic_iostream<S>                   S_basic_iostream;
typedef std::basic_stringbuf<S>                  S_basic_stringbuf;
typedef std::basic_istringstream<S>              S_basic_istringstream;
typedef std::basic_ostringstream<S>              S_basic_ostringstream;
typedef std::basic_stringstream<S>               S_basic_stringstream;

typedef std::basic_filebuf<S>                    S_basic_filebuf;
typedef std::basic_ifstream<S>                   S_basic_ifstream;
typedef std::basic_ofstream<S>                   S_basic_ofstream;
typedef std::basic_fstream<S>                    S_basic_fstream;

typedef std::istreambuf_iterator<S>              S_istreambuf_iterator;
typedef std::ostreambuf_iterator<S>              S_ostreambuf_iterator;

#else   // if defined _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES

// specify default template argument for broken compilers

typedef std::basic_ios<S, Traits>                  S_basic_ios;
typedef std::basic_streambuf<S, Traits>            S_basic_streambuf;
typedef std::basic_istream<S, Traits>              S_basic_istream;
typedef std::basic_ostream<S, Traits>              S_basic_ostream;
typedef std::basic_iostream<S, Traits>             S_basic_iostream;
typedef std::basic_stringbuf<S, Traits, Alloc>     S_basic_stringbuf;
typedef std::basic_istringstream<S, Traits, Alloc> S_basic_istringstream;
typedef std::basic_ostringstream<S, Traits, Alloc> S_basic_ostringstream;
typedef std::basic_stringstream<S, Traits, Alloc>  S_basic_stringstream;

typedef std::basic_filebuf<S, Traits>              S_basic_filebuf;
typedef std::basic_ifstream<S, Traits>             S_basic_ifstream;
typedef std::basic_ofstream<S, Traits>             S_basic_ofstream;
typedef std::basic_fstream<S, Traits>              S_basic_fstream;

typedef std::istreambuf_iterator<S, Traits>        S_istreambuf_iterator;
typedef std::ostreambuf_iterator<S, Traits>        S_ostreambuf_iterator;

#endif   // _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES

typedef std::fpos<S>       S_fpos;

// test whether all required typedefs are defined

typedef std::ios           X_ios;
typedef std::streambuf     X_streambuf;
typedef std::istream       X_istream;
typedef std::ostream       X_ostream;
typedef std::iostream      X_iostream;
typedef std::stringbuf     X_stringbuf;
typedef std::istringstream X_istringstream;
typedef std::ostringstream X_ostringstream;
typedef std::stringstream  X_stringstream;
typedef std::filebuf       X_filebuf;
typedef std::ifstream      X_ifstream;
typedef std::ofstream      X_ofstream;
typedef std::fstream       X_fstream;

typedef std::streampos     X_streampos;

#ifndef _RWSTD_NO_WCHAR_T

typedef std::wios           X_wios;
typedef std::wstreambuf     X_wstreambuf;
typedef std::wistream       X_wistream;
typedef std::wostream       X_wostream;
typedef std::wiostream      X_wiostream;
typedef std::wstringbuf     X_wstringbuf;
typedef std::wistringstream X_wistringstream;
typedef std::wostringstream X_wostringstream;
typedef std::wstringstream  X_wstringstream;
typedef std::wfilebuf       X_wfilebuf;
typedef std::wifstream      X_wifstream;
typedef std::wofstream      X_wofstream;
typedef std::wfstream       X_wfstream;

typedef std::wstreampos     X_wstreampos;

#endif   // _RWSTD_NO_WCHAR_T

/**************************************************************************/

// additional compilation tests

#include <rw_driver.h>

static void
test_synopsis ()
{
    rw_info (0, 0, __LINE__, "27.4.4 Class template basic_ios");

    // exercise types
    std::streamoff off = 0;
    _RWSTD_UNUSED (off);

    std::streamsize size = 0;
    _RWSTD_UNUSED (size);

    std::fpos<int> fpi;
    _RWSTD_UNUSED (fpi);

    std::ios_base *iob = 0;
    _RWSTD_UNUSED (iob);

    // exercise a default argument
    std::basic_ios<char, std::char_traits<char> > *bios_2 = 0;
    std::basic_ios<char /*, char_traits<char> */> *bios_1 = bios_2;

    _RWSTD_UNUSED (bios_2);
    _RWSTD_UNUSED (bios_1);

#ifndef _RWSTD_NO_WCHAR_T

    // exercise a default argument
    std::basic_ios<wchar_t, std::char_traits<wchar_t> > *wbios_2 = 0;
    std::basic_ios<wchar_t /*, char_traits<wchar_t> */> *wbios_1 = wbios_2;

    _RWSTD_UNUSED (wbios_2);
    _RWSTD_UNUSED (wbios_1);

#endif   // _RWSTD_NO_WCHAR_T

#define TEST_MANIP(name) {                              \
        std::ios_base& (*pf)(std::ios_base&) = &name;   \
        _RWSTD_UNUSED (pf);                             \
    }

    // 27.4.5, manipulators:
    TEST_MANIP (std::boolalpha);
    TEST_MANIP (std::noboolalpha);
    TEST_MANIP (std::noshowbase);
    TEST_MANIP (std::showpoint);
    TEST_MANIP (std::noshowpoint);
    TEST_MANIP (std::noshowpos);
    TEST_MANIP (std::skipws);
    TEST_MANIP (std::noskipws);
    TEST_MANIP (std::uppercase);
    TEST_MANIP (std::nouppercase);
    TEST_MANIP (std::internal);
    TEST_MANIP (std::left);
    TEST_MANIP (std::right);
    TEST_MANIP (std::dec);
    TEST_MANIP (std::hex);
    TEST_MANIP (std::oct);
    TEST_MANIP (std::fixed);
    TEST_MANIP (std::scientific);
}

/**************************************************************************/

// invariant tests

template <class charT>
static void
test_ctors (charT /* unused */, const char* cname)
{
    rw_info (0, 0, __LINE__, "27.4.4.1 basic_ios<%s> constructors", cname);

    // 27.4.4.1, p1
    std::basic_ios<charT, std::char_traits<charT> > io0 (0);

    rw_assert (0 == io0.rdbuf (), __FILE__, __LINE__,
               "basic_ios<%s>::rdbuf () == 0", cname);

    rw_assert (0 == io0.tie (), __FILE__, __LINE__,
               "basic_ios<%s>::tie () == 0", cname);

    rw_assert (io0.badbit == io0.rdstate (), __FILE__, __LINE__,
               "basic_ios<%s>::rdstate () == %{If}, got %{If}",
               cname, io0.badbit, io0.flags ());

    rw_assert ((io0.skipws | io0.dec) == io0.flags (),
               __FILE__, __LINE__,
               "basic_ios<%s>::flags () == %{If}, got %{If}",
               cname, io0.skipws | io0.dec, io0.flags ());

    rw_assert (0 == io0.width (), __FILE__, __LINE__,
               "basic_ios<%s>::width () == 0, got %l",
               cname, io0.width ());

    rw_assert (6 == io0.precision (), __FILE__, __LINE__,
               "basic_ios<%s>::precision () == 6, got %l",
               cname, io0.precision ());

    rw_assert (io0.widen (' ') == io0.fill (), __FILE__, __LINE__,
               "basic_ios<%s>::fill ()", cname);
}

/**************************************************************************/

// command-line options
static int opt_no_basic_ios_ctors = 0;

static int
run_test (int /* unused */, char* /* unused */ [])
{
    test_synopsis ();

    if (opt_no_basic_ios_ctors) {
        rw_note (0, 0, 0, "basic_ios<T> ctors disabled");
    }
    else {
        test_ctors (char (), "char");
    }

    return 0;
}

/* extern */ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.iostream.basic_ios",
                    0,   // no comment
                    run_test,
                    "|-no-basic_ios-ctors# ",
                    &opt_no_basic_ios_ctors);
}

