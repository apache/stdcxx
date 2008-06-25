/***************************************************************************
 *
 * 22.locale.codecvt.cpp - test exercising the std::codecvt<> facets
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

#ifdef __SUNPRO_CC
    // working around a SunPro/SunOS 5.8 bug (PR #26255)
#  include <time.h>
#endif   // __SUNPRO_CC

#include <locale>

#include <cassert>
#include <climits>
#include <clocale>
#include <csetjmp>   // for longjmp(), setjmp(), ...
#include <csignal>   // for SIGABRT, signal()
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cwchar>    // for mbstate_t

#include <sys/types.h>
#include <sys/stat.h>

#ifndef _WIN32
#  include <iconv.h>      // for iconv(), iconv_open(), iconv_close()
#  include <langinfo.h>   // for CODESET, nl_langinfo()
#  include <unistd.h>
#else
#  include <direct.h>
#endif

#define DEFINE_REPLACEMENT_NEW_AND_DELETE
#include <rw_driver.h>
#include <rw_file.h>
#include <rw_locale.h>

#ifndef PATH_MAX
#  define PATH_MAX 1024
#endif

// No dependency on C library min/max
#undef min
#define min(a, b)   ((a) < (b) ? (a) : (b))

/****************************************************************************/

// mbs/wcs helper functions
#if !defined _RWSTD_NO_WCHAR_T

template <class T>
/*static*/ std::size_t
rwtest_mbstowcs (std::mbstate_t& state, T* pi,
                 const char* pe, std::size_t srclen)
{
#  ifndef _RWSTD_NO_MBSRTOWCS

    return std::mbsrtowcs (pi, &pe, srclen, &state);

#  else   // if defined _RWSTD_NO_MBSRTOWCS

    _RWSTD_UNUSED (state);
    return std::mbstowcs (pi, pe, srclen);

#  endif   // _RWSTD_NO_MBSRTOWCS
}


_RWSTD_SPECIALIZED_FUNCTION
/*static*/ std::size_t
rwtest_mbstowcs<char> (std::mbstate_t& /*unused*/, char* /*unused*/,
                       const char* /*unused*/, std::size_t /*unused*/)
{
    return 0;
}

template <class charT>
inline std::size_t
rwtest_wcstombs (std::mbstate_t& state, const charT* pi,
                 char* pe, std::size_t srclen)
{
#  ifndef _RWSTD_NO_WCSRTOMBS

    return std::wcsrtombs (pe, &pi, srclen, &state);

#  else   // if defined _RWSTD_NO_WCSRTOMBS)

    _RWSTD_UNUSED (state);
    return std::wcstombs (pe, pi, srclen);

#  endif   // _RWSTD_NO_WCSRTOMBS
}

_RWSTD_SPECIALIZED_FUNCTION
inline std::size_t
rwtest_wcstombs<char>(std::mbstate_t& /*unused*/, const char* /*unused*/,
                      char* /*unused*/, std::size_t /*unused*/)
{
    return 0;
}

#endif // !defined _RWSTD_NO_WCHAR_T


static std::size_t
rwtest_mbslen (std::mbstate_t& state,
               const char*     pe,
               const char*     limit,
               std::size_t     cmax)
{
    std::size_t total = 0;
    std::size_t ret   = 0;

    while (total < cmax && pe < limit)
    {
#ifndef _RWSTD_NO_MBRLEN

        ret = std::mbrlen (pe, min (_RWSTD_MB_LEN_MAX, limit - pe), &state);

#else   // if defined _RWSTD_NO_MBRLEN

        _RWSTD_UNUSED (state);
        ret = std::mblen (pe, min (_RWSTD_MB_LEN_MAX, limit - pe));

#endif   // _RWSTD_NO_MBRLEN

        if (ret >= std::size_t (-2))
            break;

        pe += ret;
        ++total;
    }

    return total;
}

/****************************************************************************/

// The codecvt facets and their required specializations
typedef std::codecvt_base::result                         CodecvtResult;
typedef std::codecvt<char,char,std::mbstate_t>            Codecvt;
typedef std::codecvt_byname<char,char,std::mbstate_t>     CodecvtBn;

#ifndef _RWSTD_NO_WCHAR_T
typedef std::codecvt<wchar_t,char,std::mbstate_t>         CodecvtW;
typedef std::codecvt_byname<wchar_t,char,std::mbstate_t>  CodecvtBnW;
#endif   // _RWSTD_NO_WCHAR_T


// Type names used in assertion messages
template <class T>
struct TypeName
{
    static const char* name;
};

_RWSTD_SPECIALIZED_CLASS const char*
TypeName<char>::name = "char";

_RWSTD_SPECIALIZED_CLASS const char*
TypeName<Codecvt>::name = "std::codecvt<char, char, std::mbstate_t>";

_RWSTD_SPECIALIZED_CLASS const char*
TypeName<CodecvtBn>::name = "std::codecvt_byname<char, char, std::mbstate_t>";

#ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS const char*
TypeName<wchar_t>::name = "wchar_t";

_RWSTD_SPECIALIZED_CLASS const char*
TypeName<CodecvtW>::name = "std::codecvt<wchar_t, char, std::mbstate_t>";

_RWSTD_SPECIALIZED_CLASS const char*
TypeName<CodecvtBnW>::name =
    "std::codecvt_byname<wchar_t, char, std::mbstate_t>";

#endif   // _RWSTD_NO_WCHAR_T

/****************************************************************************/

// bitmasks for calls to virtual functions
enum {
    dtor                = 1 << 0,
    do_out              = 1 << 1,
    do_in               = 1 << 2,
    do_unshift          = 1 << 3,
    do_encoding         = 1 << 4,
    do_always_noconv    = 1 << 5,
    do_length           = 1 << 6,
    do_max_length       = 1 << 7
};

// bitset for all virtual function calls
int call = 0;

// template class for testing virtual calls
template <class Base>
struct CodecvtDerived: Base
{
    typedef Base                                base_type;
    typedef typename base_type::intern_type     itype;
    typedef typename base_type::extern_type     etype;
    typedef typename base_type::state_type      stype;
    typedef typename std::codecvt_base::result  result;

    CodecvtDerived (const char*);

    virtual ~CodecvtDerived () {
        call |= ::dtor;
    }

    result
    do_out (stype&, const itype*, const itype*, const itype*&,
            etype*, etype*, etype*&) const {
        call |= ::do_out;
        return result ();
    }

    result
    do_in (stype&, const etype*, const etype*, const etype*&,
           itype*, itype*, itype*&) const {
        call |= ::do_in;
        return result ();
    }

    result
    do_unshift (stype&, etype*, etype*, etype*&) const {
        call |= ::do_unshift;
        return result ();
    }

    int do_encoding () const _THROWS (()) {
        call |= ::do_encoding;
        return 0;
    }

    bool do_always_noconv () const _THROWS (()) {
        call |= ::do_always_noconv;
        return false;
    }

    int do_length (stype&, const etype*, const etype*,
                           std::size_t) const {
        call |= ::do_length;
        return 0;
    }

    int do_max_length () const _THROWS (()) {
        call |= ::do_max_length;
        return 0;
    }
};


// specialization for the class constructor in CodecvtDerived
_RWSTD_SPECIALIZED_CLASS
CodecvtDerived<Codecvt>::CodecvtDerived (const char* /*unused*/)
    : Codecvt () { }

_RWSTD_SPECIALIZED_CLASS
CodecvtDerived<CodecvtBn>::CodecvtDerived (const char* name)
    : CodecvtBn (name) { }

#ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS
CodecvtDerived<CodecvtW>::CodecvtDerived (const char* /*unused*/)
    : CodecvtW () { }

_RWSTD_SPECIALIZED_CLASS
CodecvtDerived<CodecvtBnW>::CodecvtDerived (const char* name)
    : CodecvtBnW (name) { }

#endif   // _RWSTD_NO_WCHAR_T


//  Tests the call of virtual member functions in codecvt facets
template <class CodeCvtT>
static void
test_virtuals (const char* locale_name)
{
    rw_info (0, __FILE__, __LINE__,
             "%s virtual functions", TypeName<CodeCvtT>::name);

    CodecvtDerived<CodeCvtT> pccvt (locale_name);
    CodeCvtT& cc = pccvt;

    typedef typename CodeCvtT::intern_type intern_type;
    typedef typename CodeCvtT::extern_type extern_type;

    const extern_type* pce = 0;
    extern_type*       pe  = 0;
    const intern_type* pci = 0;
    intern_type*       pi  = 0;

    std::mbstate_t st = std::mbstate_t ();

    // verify 22.2.1.5.1, p1
    cc.out (st, pci, pci, pci, pe, pe, pe );
    rw_assert ((call & do_out) != 0, __FILE__, __LINE__,
               "%s::do_out call failed.",  TypeName<CodeCvtT>::name);

    // verify 22.2.1.5.1, p2
    cc.in (st, pce, pce, pce, pi, pi, pi );
    rw_assert ((call & do_in) != 0, __FILE__, __LINE__,
               "%s::do_in call failed.",  TypeName<CodeCvtT>::name);

    // verify 22.2.1.5.1, p3
    cc.unshift (st, pe, pe, pe);
    rw_assert ((call & do_unshift) != 0, __FILE__, __LINE__,
               "%s::do_unshift call failed.",  TypeName<CodeCvtT>::name);

    // verify 22.2.1.5.1, p4
    cc.encoding ();
    rw_assert ((call & do_encoding) != 0, __FILE__, __LINE__,
               "%s::do_encoding call failed.",  TypeName<CodeCvtT>::name);

    // verify 22.2.1.5.1, p5
    cc.always_noconv ();
    rw_assert ((call & do_always_noconv) != 0, __FILE__, __LINE__,
               "%s::do_always_noconv call failed.",  TypeName<CodeCvtT>::name);

    // verify 22.2.1.5.1, p6
    cc.length (st, pce, pce, 0);
    rw_assert ((call & do_length) != 0, __FILE__, __LINE__,
               "%s::do_length call failed.",  TypeName<CodeCvtT>::name);

    // verify 22.2.1.5.1, p7
    cc.max_length ();
    rw_assert ((call & do_max_length) != 0, __FILE__, __LINE__,
               "%s::do_max_length call failed.",  TypeName<CodeCvtT>::name);
}

/****************************************************************************/

const Codecvt& create_Codecvt (const std::locale&)
{
    static Codecvt cc;
    return cc;
}

const CodecvtBn& create_CodecvtBn (const std::locale& loc)
{
    return _STD_USE_FACET (CodecvtBn, loc);
}

#ifndef _RWSTD_NO_WCHAR_T

const CodecvtW& create_CodecvtW (const std::locale&)
{
    static CodecvtW cc;
    return cc;
}

const CodecvtBnW& create_CodecvtBnW (const std::locale& loc)
{
    return _STD_USE_FACET (CodecvtBnW, loc);
}

#endif // !defined _RWSTD_NO_WCHAR_T


// facet operations results
static const char*
rwtest_codecvt_result (int res)
{
    // allow the function to be called up to 8 times in the same expression
    // without overwriting the static buffer (each call will return a pointer
    // to a new array)
    static char buf [8][16];

    static std::size_t index;

    std::size_t inx = index;

    index = (index + 1) % sizeof buf / sizeof *buf;

    std::sprintf (buf [inx], "%#x", res);

    return buf [inx];
}

static const char*
rwtest_codecvt_result (CodecvtResult res)
{
    static const struct {
        CodecvtResult res;
        const char*   str;
    } results [] = {
        { std::codecvt_base::ok,      "std::codecvt_base::ok" },
        { std::codecvt_base::partial, "std::codecvt_base::partial" },
        { std::codecvt_base::noconv,  "std::codecvt_base::noconv" },
        { std::codecvt_base::error,   "std::codecvt_base::error" }
    };

    for (unsigned i = 0; i != sizeof results / sizeof *results; ++i)
        if (res == results [i].res)
            return results [i].str;

    return rwtest_codecvt_result (int (res));
}

/****************************************************************************/

static bool
rwtest_is_C_locale (const char* s)
{
    const char* old = std::setlocale (LC_ALL, s);
    if (old == 0)
        return false;

    std::setlocale (LC_ALL, old);
    return true;
}

/****************************************************************************/

// Test custom locales, built on the fly
template <class CodeCvtT>
static void
test_libstd_do_in (const char* locale_name, const CodeCvtT& cc,
                   const typename CodeCvtT::extern_type*  buffe,
                   typename CodeCvtT::intern_type*& buffi,
                   std::size_t sze, std::size_t& szi,
                   bool skip_C,
                   CodecvtResult& res, std::size_t& sz)
{
    // buffers types
    typedef typename CodeCvtT::intern_type      intern_t;
    typedef typename CodeCvtT::extern_type      extern_t;

    const extern_t* pe   = buffe;
    const extern_t* pen  = buffe;
    const extern_t* pe_limit = buffe + sze;

    szi = sze;
    buffi = new intern_t [szi];
    std::memset (buffi, 0, szi * sizeof (intern_t));

    intern_t* pi  = buffi;
    intern_t* pin = pi;
    intern_t* pi_limit = pi + sze;

    // the state
    std::mbstate_t state = std::mbstate_t ();

    // do the conversion and store results
    res = cc.in ( state, pe, pe_limit, pen, pi, pi_limit, pin );
    szi = sz = pin - pi;

    // if C library locale is available, then perform conversion and
    // compare the results
    if (skip_C || std::setlocale (LC_CTYPE, locale_name) == 0)
        return;

    intern_t* C_buffer = new intern_t [sze];
    std::memset (C_buffer, 0, sze * sizeof (intern_t));

    intern_t* C_pi = C_buffer;

    std::mbstate_t C_state = std::mbstate_t ();

    pen = pe;
    std::size_t C_res =
        rwtest_mbstowcs<intern_t> (C_state, C_pi, pen, sze);
    if (res != std::codecvt_base::ok || C_res == std::size_t(-1)) {
        delete [] C_buffer;
        return;
    }

    // test against C library results - length and content
    rw_assert (C_res == std::size_t(pin - pi), __FILE__, __LINE__,
               "(%s) %s::do_in Clib length check:\n"
               "  C   library conversion size : %d\n"
               "  std library conversion size : %d\n",
               locale_name, TypeName<CodeCvtT>::name, C_res, pin - pi);

    rw_assert (0 == std::memcmp (pi, C_pi, C_res), __FILE__, __LINE__,
               "(%s) %s::do_in data check failed against C library conversion",
               locale_name, TypeName<CodeCvtT>::name);

    // clean up the allocations
    delete [] C_buffer;
}

#define CCVT_MAX_LOCALE     5    // Max. number of locale to test
#define CCVT_MB_CUR_MIN     1    // Minimum and maximum no. of
#define CCVT_MB_CUR_MAX     6    // bytes per character

template <class CodeCvtT>
static void
test_libstd_do_out (const char* locale_name, const CodeCvtT& cc,
                    typename CodeCvtT::extern_type*& buffe,
                    const typename CodeCvtT::intern_type* buffi,
                    std::size_t& sze, std::size_t szi,
                    bool skip_C,
                    CodecvtResult& res, std::size_t& sz)
{
    // buffers types
    typedef typename CodeCvtT::intern_type      intern_t;
    typedef typename CodeCvtT::extern_type      extern_t;

    // the state
    std::mbstate_t state = std::mbstate_t ();

    buffe = new extern_t [CCVT_MB_CUR_MAX * szi];
    std::memset (buffe, 0, CCVT_MB_CUR_MAX * szi);
    extern_t* pe  = buffe;
    extern_t* pen = pe;
    extern_t* pe_limit = pe + CCVT_MB_CUR_MAX * szi;

    const intern_t* pi  = buffi;
    const intern_t* pin = buffi;
    const intern_t* pi_limit = pi + szi;

    // perform the conversion
    res = cc.out ( state, pi, pi_limit, pin, pe, pe_limit, pen );
    sze = sz = pen - pe;

    if (skip_C || std::setlocale (LC_CTYPE, locale_name) == 0) {
        return;
    }

    extern_t* C_buffer = new extern_t [CCVT_MB_CUR_MAX * szi];
    std::memset (C_buffer, 0, CCVT_MB_CUR_MAX * szi);
    extern_t* C_pe = C_buffer;

    std::mbstate_t C_state = std::mbstate_t ();

    std::size_t C_res =
        rwtest_wcstombs<intern_t> (C_state, pi, C_pe, CCVT_MB_CUR_MAX * szi);
    if (C_res == std::size_t(-1)) {
        delete [] C_buffer;
        return;
    }

    // test results against C library
    rw_assert (C_res == std::size_t(pen - pe), __FILE__, __LINE__,
               "(%s) %s::do_out Clib length check\n"
               "    C   library conversion size : %d\n"
               "    std library conversion size : %d\n",
               locale_name, TypeName<CodecvtBnW>::name,
               C_res, (pen - pe));

    rw_assert (0 == std::memcmp (pe, C_pe, C_res), __FILE__, __LINE__,
               "(%s) %s::do_out Clib data check failed "
               "against C library conversion",
               locale_name, TypeName<CodecvtBnW>::name);

    delete [] C_buffer;
}

template <class CodeCvtT>
static void
test_libstd_do_unshift (const CodeCvtT& cc,
                        std::mbstate_t& state,
                        typename CodeCvtT::extern_type* buffe,
                        std::size_t& sze,
                        CodecvtResult& res)
{
    // buffers types
    typedef typename CodeCvtT::intern_type      intern_t;
    typedef typename CodeCvtT::extern_type      extern_t;

    extern_t* pe  = buffe;
    extern_t* pen = buffe;
    extern_t* pe_limit = buffe + sze;

    res = cc.unshift (state, pe, pe_limit, pen);
    sze = pen - pe;
}

template <class CodeCvtT>
static void
test_libstd_do_always_noconv (const CodeCvtT& cc, bool& res)
{
    res = cc.always_noconv ();
}

// check do_max_length operation in facet
template <class CodeCvtT>
static void
test_libstd_do_max_length (const CodeCvtT& cc, std::size_t& res )
{
    res = cc.max_length ();
}

// check do_length operation in facet
template <class CodeCvtT>
static void
test_libstd_do_length (const char* locale_name, const CodeCvtT& cc,
                       const typename CodeCvtT::extern_type* buffe,
                       std::size_t& sze, bool skip_C)
{
    // buffers types
    typedef CodecvtBnW::intern_type     intern_t;
    typedef CodecvtBnW::extern_type     extern_t;

    // the state
    std::mbstate_t state = std::mbstate_t ();

    extern_t* pe       = _RWSTD_CONST_CAST(extern_t*, buffe);
    extern_t* pe_limit = _RWSTD_CONST_CAST(extern_t*, buffe + sze);

    sze = cc.length ( state, pe, pe_limit, sze );

    if (skip_C)
        return;

    std::mbstate_t C_state = std::mbstate_t ();

    std::size_t C_res =
        rwtest_mbslen ( C_state, pe, pe_limit, sze );
    // test the result against C library
    rw_assert (sze == C_res, __FILE__, __LINE__,
               "(%s) %s::do_length; "
               "check against C library result failed :\n"
               "   C   library result : %d\n"
               "   std library result : %d",
               locale_name, TypeName<CodeCvtT>::name,
               C_res, sze);
}

template <class CodeCvtT>
static void
test_libstd_do_encoding (const CodeCvtT& cc, std::size_t& sze)
{
    sze = cc.encoding ();
}

/****************************************************************************/

// Primary template for checking libstd locale codecvt facets
template <class CodeCvtT>
static void
test_libstd_codecvt (const char* locale_name, const CodeCvtT& cc,
                     const char* fname, bool skip_C,
                     CodecvtResult in_res,  std::size_t in_res_sz,
                     CodecvtResult out_res, std::size_t out_res_sz,
                     CodecvtResult unshift_res,
                     bool always_noconv_res,
                     std::size_t encoding_res,
                     std::size_t length_res,
                     std::size_t max_length_res)
{
    // buffers types
    typedef typename CodeCvtT::intern_type      intern_t;
    typedef typename CodeCvtT::extern_type      extern_t;

    // create a state object
    std::mbstate_t state = std::mbstate_t ();

    // pointers to buffers
    extern_t*  ebuffer   = 0;
    intern_t*  ibuffer   = 0;
    extern_t*  eebuffer  = 0;

    std::size_t elength  = 0;
    std::size_t ilength  = 0;
    std::size_t eelength = 0;

    // read source file
    ebuffer = _RWSTD_STATIC_CAST (extern_t*, rw_fread (fname, &elength, "rb"));

    if (!ebuffer || !elength) {
        rw_assert (false, __FILE__, __LINE__,
                   "failed to read from %s", fname);
        return;
    }

    // operations results
    CodecvtResult res;
    bool          bres;
    std::size_t   ires;
    std::size_t   res_sz;

    char fcall [256];
    std::sprintf (fcall, "%s::in(..., %p, %p, 0, %p, %p, 0)",
                  TypeName<CodeCvtT>::name,
                  (const void*)ebuffer, (const void*)(ebuffer + elength),
                  (const void*)ibuffer, (const void*)(ibuffer + ilength));

    // Test do_in conversion result and size
    test_libstd_do_in<CodeCvtT> (locale_name, cc,
                                 ebuffer, ibuffer, elength, ilength,
                                 skip_C, res, res_sz);

    // 22.2.1.5 p3, 22.2.1.5.2 p2 - test operation result
    rw_assert (res == in_res, __FILE__, __LINE__,
               "%s == %s, got %s in locale(\"%s\")",
               fcall,
               rwtest_codecvt_result (in_res),
               rwtest_codecvt_result (res),
               locale_name);

    // test the length of the internal buffer is correct; noconv does not do
    // any copying of the data
    if (res == std::codecvt_base::noconv) {
        rw_assert (res_sz == 0, __FILE__, __LINE__,
                   "%s expected size 0 for %s, got %d instead "
                   "in locale(\"%s\")", fcall,
                   rwtest_codecvt_result (res), res_sz, locale_name);

        // so copy all content from external buffer to internal buffer
        ilength = elength;
        for (std::size_t i = 0; i < elength; i++)
            ibuffer [i] = intern_t (ebuffer [i]);

    }
    else {
        rw_assert (ilength == in_res_sz && ilength == res_sz,
                   __FILE__, __LINE__,
                   "%s: conversion size: expected %d, got %d in locale(\"%s\")",
                   fcall, in_res_sz, ilength, locale_name);
    }

    // Test do_out conversion result and size
    test_libstd_do_out<CodeCvtT> (locale_name, cc,
                                  eebuffer, ibuffer, eelength, ilength,
                                  skip_C, res, res_sz);

    // 22.2.1.5 p3, 22.2.1.5.2 p2 - test operation result
    rw_assert (res == out_res, __FILE__, __LINE__,
               "(%s) %s::do_out() == %s, got %s",
               locale_name, TypeName<CodecvtBnW>::name,
               rwtest_codecvt_result (out_res), rwtest_codecvt_result (res));

    if (res == std::codecvt_base::noconv) {
        rw_assert (res_sz == 0, __FILE__, __LINE__,
                   "(%s) %s::do_out expected size 0 for %s, got %d instead",
                   locale_name, TypeName<CodeCvtT>::name,
                   rwtest_codecvt_result (res), res_sz);

        // so copy all content from internal buffer to external buffer
        eelength = ilength;
        for (std::size_t i = 0; i < eelength; i++)
            eebuffer [i] = extern_t (ibuffer [i]);

    } else {
        rw_assert (eelength == out_res_sz && eelength == res_sz,
                   __FILE__, __LINE__,
                   "(%s) %s::do_out conversion size: expected %d, got %d",
                   locale_name, TypeName<CodeCvtT>::name,
                   in_res_sz, ilength);
    }

    // Test the roundtrip conversion content and size
    std::size_t cmp = 0;
    if (std::memcmp (ebuffer, eebuffer, min (elength, eelength))) {
        for (cmp = 0; cmp < min (elength, eelength); cmp++)
            if (ebuffer [cmp] != eebuffer [cmp])
                break;
    }

    rw_assert (elength == eelength, __FILE__, __LINE__,
               "(%s) %s round-trip check size mismatch    : "
               " expected size %d , got size %d",
               locale_name, TypeName<CodeCvtT>::name,
               elength, eelength);

    rw_assert (0 == std::memcmp (ebuffer, eebuffer,
                                 min (elength, eelength)),
               __FILE__, __LINE__,
               "(%s) %s round-trip check content mismatch : at index %d",
               locale_name, TypeName<CodeCvtT>::name, cmp);

    // Test unshift operation
    extern_t    tmp [16];
    std::size_t tmp_len = 16;
    state = std::mbstate_t ();

    test_libstd_do_unshift<CodeCvtT> (cc, state, tmp, tmp_len, res);

    // 22.2.1.5.2 p5-6
    rw_assert (res == unshift_res, __FILE__, __LINE__,
               "(%s) %s::do_unshift; expected %s, got %s",
               locale_name, TypeName<CodeCvtT>::name,
               rwtest_codecvt_result (unshift_res),
               rwtest_codecvt_result (res));

    rw_assert (tmp_len == 0, __FILE__, __LINE__,
               "(%s) %s::do_unshift; expected 0 length unshift sequence, "
               "got %d", locale_name, TypeName<CodeCvtT>::name, tmp_len);

    // Test do_always_noconv operation
    test_libstd_do_always_noconv<CodeCvtT> (cc, bres);
    rw_assert (bres == always_noconv_res, __FILE__, __LINE__,
               "(%s) %s::do_always_noconv expected %s, got %s",
               locale_name, TypeName<CodeCvtT>::name,
               always_noconv_res?"true":"false", bres?"true":"false");

    // Test do_max_length operation
    test_libstd_do_max_length<CodeCvtT> (cc, ires);
    // 22.2.1.5.2 p11
    rw_assert (ires == max_length_res, __FILE__, __LINE__,
               "(%s) %s::do_max_length expected %d, got %d",
               locale_name, TypeName<CodeCvtT>::name,
               max_length_res, ires);

    // Test do_length operation
    ires = elength;
    test_libstd_do_length<CodeCvtT> (locale_name, cc,
                                     ebuffer, ires, skip_C);
    // 22.2.1.5.2 p10
    rw_assert (ires == length_res, __FILE__, __LINE__,
               "(%s) %s::do_length expected %d, got %d",
               locale_name, TypeName<CodeCvtT>::name,
               length_res, ires);

    ires = 0;
    test_libstd_do_encoding<CodeCvtT> (cc, ires);
    // 22.2.1.5.2 p6
    rw_assert (ires == encoding_res, __FILE__, __LINE__,
               "(%s) %s::do_encoding expected %d, got %d",
               locale_name, TypeName<CodeCvtT>::name,
               encoding_res, ires);

    delete [] ibuffer;
    delete [] eebuffer;
    // delete [] ebuffer;
}

/****************************************************************************/

#if !defined _WIN32

static unsigned int endian_test = 1;
static bool big_endian = *(unsigned char*)&endian_test == 0;

static iconv_t
rwtest_iconv_open (bool ucs4, const char* enc)
{
    static const char* const ucs4_aliases [] = {
        "UCS-4LE", "UCS-4", "UCS4", "ucs4", 0
    };

    static const char* const ucs2_aliases [] = {
        "UCS-2LE", "UCS-2", "UCS2", "ucs2", 0
    };

    const char* const* pucs = ucs4 ? ucs4_aliases : ucs2_aliases;

    // that accomodates Linux with its extra modifiers LE and BE
    // (UCS-4 is synonym with UCS-4BE)
    if (big_endian)
        pucs++;

    iconv_t id = iconv_t (-1);

    for (; id == iconv_t (-1) && *pucs && **pucs; pucs++)
        id = iconv_open (*pucs, enc);

    return id;
}


static void
rwtest_iconv_convert (iconv_t      fd,
                      const char*  in,  std::size_t  in_sz,
                      wchar_t*     out, std::size_t& out_sz)
{
    out_sz *= sizeof (wchar_t);

    // Two issues:
    //  - iconv shipped with the C library on RedHat 6.2 if reaches the end of
    //  the input  buffer (out_sz  == 0) reports  E2BIG as  if it has  had not
    //  enuff space to store the result of the conversion
    //  - iconv shipped with C library on RedHat 6.2 apparently uses UCS-4 big
    //  endian on little endian machines too
    std::size_t tmp = out_sz++ + 1;

#  if defined _RWSTD_NO_ICONV_CONST_CHAR
    char* par1 = _RWSTD_CONST_CAST (char*, in);
#  else
    const char* par1 = in;
#  endif //  defined _RWSTD_NO_ICONV_CONST_CHAR
    char* par2 = _RWSTD_REINTERPRET_CAST (char*, out);

    std::size_t res = iconv (fd, &par1, &in_sz, &par2, &out_sz);
    if (res == std::size_t (-1))
        out_sz = 0;
    else
        out_sz = (tmp + 1 - out_sz) / sizeof (wchar_t);
}

#endif // defined _WIN32


enum InternalEncoding {
    use_libc,   // use libc's native internal wchar_t encoding
    use_UCS4,   // use UCS-4 as the internal wchar_t encoding
    use_UCS2    // use UCS-2 as the internal wchar_t encoding
};


static bool
rwtest_convert_to_internal (InternalEncoding conv,
                            const char*      native_locname,
                            const char*      ebuffer, std::size_t  elength,
                            wchar_t*         ibuffer, std::size_t& ilength)
{
    ilength = 0;

    if (!native_locname)
        return false;

    const char* const old_locale = std::setlocale (LC_CTYPE, native_locname);

    if (!old_locale)
        return false;

    switch (conv) {

#ifdef _WIN32

    case use_UCS2:
        // fall through...

#endif   // _WIN32

    case use_libc: {

        std::mbstate_t state = std::mbstate_t ();

        ilength = rwtest_mbstowcs (state, ibuffer, ebuffer, elength);

        break;
    }

#ifndef _WIN32

    case use_UCS2:
    case use_UCS4: {
        // use iconv

        const char* const codeset = nl_langinfo (CODESET);
        if (!codeset) {
            ilength = std::size_t (-1);
            break;
        }

        const iconv_t id = rwtest_iconv_open (true, codeset);

        if (iconv_t (-1) == id)
            ilength = std::size_t (-1);
        else {
            rwtest_iconv_convert (id, ebuffer, elength, ibuffer, ilength);

            iconv_close (id);
        }

        break;
    }

#endif   // _WIN32

    }

    // restore the previous locale
    std::setlocale (LC_CTYPE, old_locale);

    if (std::size_t (-1) == ilength) {
        ilength = 0;
        return false;
    }

    return true;
}


static const char*
rwtest_internal_type (InternalEncoding enc)
{
    static const char* const encs []  = {
        "native", "UCS-4", "UCS-2", "invalid"
    };

    switch (enc) {
    case use_libc:
        return encs [0];
    case use_UCS4:
        return encs [1];
    case use_UCS2:
        return encs [2];
    default:
        return encs [3];
    }
}


static std::size_t
bmatch (const char* mem1, const char* mem2, std::size_t nchars)
{
    if (std::memcmp (mem1, mem2, nchars)) {

        for (std::size_t i = 0; i != nchars; ++i)
            if (mem1 [i] != mem2 [i])
                return i;
    }
    return nchars;
}


static std::size_t
bmatch (const wchar_t* mem1, const wchar_t* mem2, std::size_t nchars)
{
    if (std::size_t (-1) == nchars) {
        // avoid warnings about unused overload above
        return bmatch ((char*)0, (char*)0, 0);
    }

    if (std::memcmp (mem1, mem2, nchars * sizeof *mem1)) {

        for (std::size_t i = 0; i != nchars; ++i)
            if (mem1 [i] != mem2 [i])
                return i;
    }
    return nchars;
}


static void
test_ucs_modifier (const char*       locale_name,
                   InternalEncoding  conv,
                   const char*       native_locname,
                   const CodecvtBnW& cc,
                   const char*       fname,
                   CodecvtResult     in_res,
                   std::size_t       in_res_sz)
{
    // pointers to buffers
    char*    ebuf        = 0;
    wchar_t* ibuf        = 0;
    wchar_t* ibuf_native = 0;

    // buffer sizes
    std::size_t elen   = 0;
    std::size_t ilen   = 0;
    std::size_t ilen_native = 0;

    // read source file
    ebuf = _RWSTD_STATIC_CAST (char*, rw_fread (fname, &elen, "rb"));
    if (!ebuf || !elen)
        return;

    // allocate and zero the internal buffer
    ilen = elen;
    ibuf = new wchar_t [ilen];
    std::memset (ibuf, 0, ilen * sizeof *ibuf);

    ilen_native = elen;

    // to accommodate iconv shipped with RedHat 6.2, allocate one more wchar_t
    ibuf_native = new wchar_t [ilen_native + 1];
    std::memset (ibuf_native, 0, (ilen_native + 1) * sizeof *ibuf_native);

    const char* from       = ebuf;
    const char* from_next  = ebuf;
    const char* from_limit = ebuf + elen;
    wchar_t*    to         = ibuf;
    wchar_t*    to_next    = ibuf;
    wchar_t*    to_limit   = ibuf + elen;

    // create a state object
    std::mbstate_t state = std::mbstate_t ();

    rw_info (0, __FILE__, __LINE__, "locale(\"%s\")", locale_name);

    const std::codecvt_base::result res =
        cc.in (state, from, from_limit, from_next, to, to_limit, to_next);

    ilen = to_next - to;

    if (res != in_res || in_res_sz != std::size_t (to_next - to)) {
        rw_assert (false, __FILE__, __LINE__,
                   "(%s) EXT -> INT conversion using codecvt failed:\n"
                   "  result : %s\n  expected size %d, got %d",
                   locale_name, rwtest_codecvt_result (res),
                   in_res_sz, to_next - to);

        delete [] ibuf_native;
        delete [] ibuf;
        return;

    }

    // test against the libc mbstowcs() or iconv() conversion
    if (!rwtest_convert_to_internal (conv, native_locname, ebuf, elen,
                                     ibuf_native, ilen_native)) {
        delete [] ibuf_native;
        delete [] ibuf;
        return;
    }

    if (!ilen_native) {
        rw_assert (false, __FILE__, __LINE__,
                   "(%s) EXT -> INT conversion using %s "
                   "(%s internally) failed",
                   locale_name,
                   (conv == use_libc) ? "mbstowcs()" : "iconv()",
                   rwtest_internal_type (conv));

        delete [] ibuf_native;
        delete [] ibuf;
        return;
    }

    // Conversion and check against mbstowcs / iconv
    rw_assert (ilen_native == ilen, __FILE__, __LINE__,
               "(%s) EXT -> INT length check (%s internally):\n"
               "  size of native encoding: %d\n"
               "  size of codecvt encoding: %d\n",
               locale_name, rwtest_internal_type (conv),
               ilen, ilen_native);

    if (ilen_native == ilen) {
        const std::size_t diff = bmatch (to, ibuf_native, ilen);

        rw_assert (diff >= ilen, __FILE__, __LINE__,
                   "codecvt<wchar_t, char, mbstate_t>::in() mismatch "
                   "at character %u of %u: '%s' != '%s'; in locale(\"%s\")",
                   diff, ilen, ibuf_native [diff],
                   to [diff], locale_name);
    }

    delete [] ibuf_native;
    delete [] ibuf;
}

/****************************************************************************/

std::jmp_buf jmp_env;

// called in response to abort() or failed assertions
extern "C"
void SIGABRT_handler (int /*unused*/)
{
    std::longjmp (jmp_env, 1);
}


// exercise the facets' treatment of an invalid mbstate_t argument
template <class CodeCvtT>
static void
test_mbstate_t (const CodeCvtT*, const char* name)
{
    const std::locale loc (name);

    const CodeCvtT &cvt = _STD_USE_FACET (CodeCvtT, loc);

    typedef typename CodeCvtT::intern_type intern_type;
    typedef typename CodeCvtT::extern_type extern_type;
    typedef typename CodeCvtT::state_type  state_type;

    state_type state;

    // initialize `state' to an invalid value
    std::memset (&state, -1, sizeof state);

    intern_type in = intern_type ();
    intern_type* in_0 = &in;
    intern_type* in_1 = &in + 1;
    intern_type* in_2 = 0;
    const intern_type* const_in_2 = 0;

    extern_type ext = extern_type ();
    extern_type* ext_0 = &ext;
    extern_type* ext_1 = &ext + 1;
    extern_type* ext_2 = 0;
    const extern_type* const_ext_2 = 0;

#ifndef _RWSTDDEBUG
    // when debugging is disabled, expect member functions to
    // detect and indicate an invalid argument by setting the
    // return value to codecvt_base::error
    const bool expect_abort = false;
#else
    // when debugging is enabled, expect member functions to
    // detect and indicate an invalid argument by calling abort()
    const bool expect_abort = true;
#endif

#undef TEST
#define TEST(resultT, result, fname, args)                                \
    if (0 == setjmp (jmp_env)) {                                          \
        std::signal (SIGABRT, SIGABRT_handler);                           \
        const resultT res = cvt.fname args;                               \
        rw_assert (!expect_abort && result == res, __FILE__, __LINE__,    \
                   "%s::%s (state_type&, ...) failed to "                 \
                   "detect an invalid state argument",                    \
                   TypeName<CodeCvtT>::name, #fname);                     \
    } else                                                                \
        rw_assert (expect_abort, __FILE__, __LINE__,                      \
                   "%s::%s (state_type&, ...) called abort() instead "    \
                   "of gracefully indicating an invalid state argument",  \
                   TypeName<CodeCvtT>::name, #fname)

    const std::codecvt_base::result error = std::codecvt_base::error;

    TEST (std::codecvt_base::result, error,
          in, (state, ext_0, ext_1, const_ext_2, in_0, in_1, in_2));

    TEST (std::codecvt_base::result, error,
          out, (state, in_0, in_1, const_in_2, ext_0, ext_1, ext_2));

    TEST (std::codecvt_base::result, error,
          unshift, (state, ext_0, ext_1, ext_2));

    TEST (int, 0,
          length,  (state, ext_0, ext_1, 0));

    // verify that functions gracefully handle null pointers
#undef TEST
#define TEST(resultT, result, fname, args)                                \
    if (0 == setjmp (jmp_env)) {                                          \
        std::signal (SIGABRT, SIGABRT_handler);                           \
        const resultT res = cvt.fname args;                               \
        rw_assert (result == res, __FILE__, __LINE__,                     \
                   "%s::%s (state_type&, (char_type*)0, ...) == %s, "     \
                   "got %s (NULL pointer test)",                          \
                   TypeName<CodeCvtT>::name, #fname,                      \
                   rwtest_codecvt_result (result),                        \
                   rwtest_codecvt_result (res));                          \
    } else                                                                \
        rw_assert (expect_abort, __FILE__, __LINE__,                      \
                   "%s::%s (state_type&, (char_type*)0, ...) "            \
                   "unexpectedly called abort() (NULL pointer test)",     \
                   TypeName<CodeCvtT>::name, #fname)

    // initialize `state' to a valid value
    std::memset (&state, 0, sizeof state);

    // const below commented out to work around an HP aCC bug (PR #29666)
    /* const */ std::codecvt_base::result result =
          sizeof (intern_type) == sizeof (char)
        ? std::codecvt_base::noconv : std::codecvt_base::ok;

    TEST (std::codecvt_base::result, result,
          in, (state, 0, 0, const_ext_2, 0, 0, in_2));

    TEST (std::codecvt_base::result, result,
          out, (state, 0, 0, const_in_2, 0, 0, ext_2));

    TEST (std::codecvt_base::result, std::codecvt_base::noconv,
          unshift, (state, 0, 0, ext_2));

    TEST (int, 0,
          length,  (state, 0, 0, 0));
}

/****************************************************************************/

static void
test_invalid_args (const char* locname)
{
    // exercise the behavior of the base facets
    rw_info (0, __FILE__, __LINE__,
             "Testing %s with invalid arguments", TypeName<Codecvt>::name);
    test_mbstate_t ((Codecvt*)0, "C");

    rw_info (0, __FILE__, __LINE__,
             "Testing %s with invalid arguments", TypeName<CodecvtW>::name);
    test_mbstate_t ((CodecvtW*)0, "C");

    char non_c_locname [256];
    *non_c_locname = '\0';

    for (const char *loc = rw_locale_query (); *loc;
         loc += std::strlen (loc) + 1) {
        if (   std::strcmp ("C", loc) && std::strcmp ("POSIX", loc)
            && std::setlocale (LC_ALL, loc)) {
            std::setlocale (LC_ALL, "C");
            std::strcpy (non_c_locname, loc);
            break;
        }
    }

    if (*non_c_locname) {
        // exercise the behavior of the derived facets when using
        // libc locale names other than "C" or "POSIX"

        rw_info (0, __FILE__, __LINE__,
                 "Testing libc based %s with invalid arguments "
                 "in locale(\"%s\")",
                 TypeName<CodecvtBn>::name, non_c_locname);

        test_mbstate_t ((CodecvtBn*)0, non_c_locname);

        rw_info (0, __FILE__, __LINE__,
                 "Testing libc based %s with invalid arguments "
                 "in locale(\"%s\")",
                 TypeName<CodecvtBnW>::name, non_c_locname);
        test_mbstate_t ((CodecvtBnW*)0, non_c_locname);
    }

    if (locname) {

        // exercise the behavior of the byname facets when using own
        // locale databases and when passed an invalid mbstate_t object
        rw_info (0, __FILE__, __LINE__,
                 "Testing libstd %s with invalid arguments",
                 TypeName<CodecvtBn>::name);

        test_mbstate_t ((CodecvtBn*)0, locname);

        rw_info (0, __FILE__, __LINE__,
                 "Testing libstd %s with invalid arguments",
                 TypeName<CodecvtBnW>::name);

        test_mbstate_t ((CodecvtBnW*)0, locname);
    }
}

/****************************************************************************/

template <class internT>
static void
test_inout (int which, int line, const char* name,
            const char*    sext, std::size_t ext_len, std::size_t ext_num,
            const internT* sint, std::size_t int_len, std::size_t int_num,
            std::codecvt_base::result expect)
{
    _RWSTD_UNUSED (line);

    char namebuf [256];
    assert (255 >= std::strlen (name));
    std::strcpy (namebuf, name);

    // when non-0, the name the locale object is constructed with
    const char* locale_name = 0;

    // when non-0, the name the codevt_byname facet is constructed
    // with before being installed in the locale object
    const char* codecvt_name = 0;

    // look for a locale name modifier
    const char* const mod = std::strrchr (name, '@');

    if (mod && 'U' == mod [1] && 'C' == mod [2] && 'S' == mod [3]) {

        namebuf [mod - name] = '\0';
        locale_name  = namebuf;
        codecvt_name = namebuf + (mod - name) + 1;
    }

    // check for the special names "UTF-8" and "utf-8" indicating
    // a codecvt_byname facet with UTF-8 as the external encoding
    // and UCS (either UCS-4 or UCS-2, depending on the width of
    // wchar_t) as the internal encoding (i.e., synonyms for
    // "UTF-8@UCS" and "utf-8@UCS" respectively)
    if (   !std::strcmp (namebuf, "UTF-8")
        || !std::strcmp (namebuf, "utf-8")) {
        codecvt_name = namebuf;
        locale_name  = 0;
    }
    else
        locale_name = namebuf;

    typedef std::codecvt<internT, char, std::mbstate_t>        Cvt;
    typedef std::codecvt_byname<internT, char, std::mbstate_t> CvtByname;

    std::locale loc;

    // try to construct a locale with the given name
    if (locale_name) {
        _TRY {
            loc = std::locale (locale_name);
        }
        _CATCH (...) {
            const char* const envvar = std::getenv (LOCALE_ROOT_ENVAR);

            rw_assert (false, __FILE__, __LINE__,
                       "locale(\"%s\") unexpectedly threw an exception; "
                       LOCALE_ROOT_ENVAR "=%s", locale_name,
                       envvar ? envvar : "(null)");

            return;
        }
    }

    // if `codecvt_name' is non-0, try to construct a codecvt_byname
    // facet with that name and install it in the locale object
    // constructed above
    if (codecvt_name) {

        const Cvt* pcvt = 0;

        _TRY {
            // first try to construct the facet
            pcvt = new CvtByname (codecvt_name);
        }
        _CATCH (...) {
            const char* const envvar = std::getenv (LOCALE_ROOT_ENVAR);

            rw_assert (false, __FILE__, __LINE__,
                       "codecvt_byname<%s, char, mbstate_t>(\"%s\") "
                       "unexpectedly threw an exception; "
                       LOCALE_ROOT_ENVAR "=%s", TypeName<internT>::name,
                       codecvt_name, envvar ? envvar : "(null)");

            return;
        }
        _TRY {
            // next try to install the facet in the locale object
            loc = std::locale (loc, pcvt);
        }
        _CATCH (...) {
            rw_assert (false, __FILE__, __LINE__,
                       "locale combining ctor unexpectedly threw "
                       "an exception; ");
            delete pcvt;
            return;
        }
    }

    // compute the length of the source sequence if it isn't specified
    if (std::size_t (-1) == ext_len)
        ext_len = sext ? std::strlen (sext) : 0;

    // compute the length of the destination sequence if it isn't specified
    if (std::size_t (-1) == int_len)
        int_len = sint ? std::char_traits<internT>::length (sint) : 0;

    // retrieve the codecvt or codecvt_byname facet from the locale object
    const Cvt &cvt = std::use_facet<Cvt>(loc);

    std::mbstate_t state;
    std::memset (&state, 0, sizeof state);

    // for convenience: fully qualified name of the function,
    // including the values of function arguments
    static char fcall [4096];

    if (   ('I' == which || 'i' == which
        || 'o' == which) && std::codecvt_base::ok == expect) {
        // exercise do_in():
        // *  the type of `sext' is char
        // *  ext_len is the number of bytes in the external sequence
        // *  the type of `sint' is internT (i.e., char or wchar_t)
        // *  int_len is the number of internT characters
        //    in the expected internal sequence

        const char* ext_lo  = sext;
        const char* ext_hi  = sext + ext_len;
        const char* ext_nxt = 0;

        internT* int_lo  = new internT [int_len + 1];
        internT* int_hi  = int_lo + int_len;
        internT* int_nxt = 0;

        std::memset (int_lo, 0, (int_len + 1) * sizeof *int_lo);

        // is the `sext' array is not NUL-terminated, create a copy
        // and NUL-terminate it
        const char* pextstr = sext;
        if (sext [ext_len]) {
            char* const tmp = new char [ext_len + 1];
            std::memcpy (tmp, sext, ext_len);
            tmp [ext_len] = '\0';
            pextstr = tmp;
        }

        std::sprintf (fcall, "codecvt<%s, char, mbstate_t>::in"
                      "(..., %p = %s, %p, %p, %p, %p, %p)",
                      TypeName<internT>::name,
                      (const void*)ext_lo, pextstr,
                      (const void*)ext_hi, (const void*)ext_nxt,
                      (const void*)int_lo, (const void*)int_hi,
                      (const void*)int_nxt);

        if (pextstr != sext) {
            // const_cast works around an MSVC 6.0 bug (PR #30092)
            delete[] _RWSTD_CONST_CAST (char*, pextstr);
        }

        std::codecvt_base::result res;

        _TRY {
            res = cvt.in (state,
                          ext_lo, ext_hi, ext_nxt,
                          int_lo, int_hi, int_nxt);
        }
        _CATCH (...) {
            rw_assert (false, __FILE__, __LINE__,
                       "%d. %s unexpectedly threw an exception",
                       __LINE__, fcall);
        }

        // verify the value returned from the function
        if (expect != res) {
            rw_assert (false, __FILE__, __LINE__,
                       "%d. %s == %s, got %s", __LINE__, fcall,
                       rwtest_codecvt_result (expect),
                       rwtest_codecvt_result (res));
        }

        // verify the value of the from_next pointer modified by the function
        if (ext_nxt != ext_lo + ext_num) {

            rw_assert (false, __FILE__, __LINE__,
                       "%d. %s; from_next == %p, got %p (at offset %d)",
                       __LINE__, fcall, ext_lo + ext_num, ext_nxt,
                       ext_nxt - ext_lo);
        }

        // verify the value of the to_next pointer modified by the function
        if (int_nxt != int_lo + int_num) {

            rw_assert (false, __FILE__, __LINE__,
                       "%d. %s; to_next == %p, got %p (at offset %d)",
                       __LINE__, fcall, int_lo + int_num, int_nxt,
                       int_nxt - int_lo);
        }

        if (std::codecvt_base::error != res) {

            // verify that the contents of the destination sequence
            // matches the expected sequence
            const int cmp = std::memcmp (int_lo, sint, int_len);

            if (cmp) {
                rw_assert (false, __FILE__, __LINE__,
                           "%d. %s -> %s, expected %s", __LINE__, fcall,
                           int_lo, sint);
            }
        }

        delete[] int_lo;
    }

    if (   ('O' == which || 'o' == which
        || 'i' == which) && std::codecvt_base::ok == expect) {
        // exercise do_out():
        // *  the type of `sint' is internT (i.e., either char or wchar_t)
        // *  int_len is the number of internT characters
        // *  the type of `ext' is char
        // *  ext_len is the number of bytes in the expected sequence
        //    of external characters

        char* ext_lo  = new char [ext_len + 1];
        char* ext_hi  = ext_lo + ext_len;
        char* ext_nxt = 0;

        std::memset (ext_lo, 0, ext_len + 1);

        const internT* int_lo  = sint;
        const internT* int_hi  = int_lo + int_len;
        const internT* int_nxt = 0;

        const char* fmt;
        if (sizeof (internT) == sizeof (char))
            fmt = "codecvt<char, char, mbstate_t>::out"
                  "(..., %p = %s, %p, %p, %p, %p, %p)";
        else if (sizeof (internT) == sizeof (wchar_t))
            fmt = "codecvt<wchar_t, char, mbstate_t>::out"
                  "(..., %p = L%ls, %p, %p, %p, %p, %p)";

        std::sprintf (fcall, fmt, (const void*)int_lo,
                      int_lo, (const void*)int_hi,
                      (const void*)int_nxt, (const void*)ext_lo,
                      (const void*)ext_hi, (const void*)ext_nxt);

        std::codecvt_base::result res;

        _TRY {
            res = cvt.out (state,
                           int_lo, int_hi, int_nxt,
                           ext_lo, ext_hi, ext_nxt);
        }
        _CATCH (...) {
            rw_assert (false, __FILE__, __LINE__,
                       "%d. %s unexpectedly threw an exception",
                       __LINE__, fcall);
        }

        // verify the value returned from the function
        if (expect != res) {
            rw_assert (false, __FILE__, __LINE__,
                       "%d. %s == %s, got %s", __LINE__, fcall,
                       rwtest_codecvt_result (expect),
                       rwtest_codecvt_result (res));
        }

        // verify the value of the from_next pointer modified by the function
        if (int_nxt != int_lo + int_num) {

            rw_assert (false, __FILE__, __LINE__,
                       "%d. %s; from_next == %p, got %p (at offset %d)",
                       __LINE__, fcall, int_lo + int_num, int_nxt,
                       int_nxt - int_lo);
        }

        // verify the value of the to_next pointer modified by the function
        if (ext_nxt != ext_lo + ext_num) {

            rw_assert (false, __FILE__, __LINE__,
                       "%d. %s; to_next == %p, got %p (at offset %d)",
                       __LINE__, fcall, ext_lo + ext_num, ext_nxt,
                       ext_nxt - ext_lo);
        }

        if (std::codecvt_base::error != res) {

            // verify that the contents of the destination sequence
            // matches the expected sequence
            const int cmp = std::memcmp (ext_lo, sext, ext_len);

            if (cmp) {

                rw_assert (false, __FILE__, __LINE__,
                           "%d. %s -> %s, expected %s", __LINE__, fcall,
                           ext_lo, sext);
            }
        }

        delete[] ext_lo;
    }
}


// the root of the locale directory (RWSTD_LOCALE_ROOT)
// set in main() instead of here to avoid Solaris 7 putenv() bug (PR #30017)
const char* locale_root;


// creates a codecvt database from `charmap'
static const char*
create_codecvt (unsigned    mb_min,
                unsigned    mb_max,
                const char* charmap,
                const char* charset_name = 0)
{
    // keeps track of successful calls and generates a unique
    // name for each codeset conversion database and locale
    static int call_no;

    // construct the absolute pathname of a charmap file
    char cm_pathname [PATH_MAX];

    if (charset_name)
        std::sprintf (cm_pathname, "%s" SLASH "%s.cm",
                      locale_root, charset_name);
    else
        std::sprintf (cm_pathname,
                      "%s" SLASH "codecvt-%u-%u.%d.cm",
                      locale_root, mb_min, mb_max, call_no);

    // create the charmap file
    std::FILE* pf = std::fopen (cm_pathname, "w");
    if (!pf)
        return 0;

    if (charset_name)
        std::fprintf (pf, "<code_set_name> %s\n", charset_name);
    else
        std::fprintf (pf, "<code_set_name> codecvt-%u-%u.%d\n",
                      mb_min, mb_max, call_no);

    std::fprintf (pf, "<escape_char>   /\n");

    std::fprintf (pf, "<mb_cur_min>    %u\n", mb_min);
    std::fprintf (pf, "<mb_cur_max>    %u\n\n\n", mb_max);
    std::fprintf (pf, "CHARMAP\n");
    std::fprintf (pf, charmap);
    std::fprintf (pf, "\nEND CHARMAP\n");

    std::fclose (pf);

    // construct the absolute pathname of a dummy locale definition file
    char src_pathname [PATH_MAX];
    std::strcpy (src_pathname, locale_root);
    std::strcat (src_pathname, SLASH "dummy.src");

    // create the locale definition file
    if (!(pf = std::fopen (src_pathname, "w")))
        return 0;

    // dummy source file must contain the LC_CTYPE section
    // in order for localedef to generate the codecvt database
    std::fprintf (pf, "LC_CTYPE\nEND LC_CTYPE\n");

    std::fclose (pf);

    // construct the filename of the locale
    char locale_fname[32];
    std::sprintf (locale_fname,
                  "locale-%u.%u-%d",
                  mb_min, mb_max, call_no);

    // invoke localedef to create the named locale from the dummy
    // locale definition file and the character set description file
    // silence the following warnings:
    // 701: no compatible locale found
    // 702: member of portable character set <x> not found
    //      in the character map
    // 706: iconv_open() failed
    // : unknown symbol found in LC_COLLATE definition
    // : some characters in the codeset were not explicitly
    //   given a collation value
    const char* const locname =
        rw_localedef ("-w701 -w702 -w706",
                      src_pathname, cm_pathname, locale_fname);

    call_no += !!locname;

    return locname;
}


// given a canonical locale name, `locname', tries to find a platform
// specific locale name that corresponds to the name; when `codeset'
// is non-0, also checks to see that the platform-specific locale's
// codeset is the same as that specified by the argument
// on success, returns the name of the platform-specific locale,
// otherwise 0
static const char*
find_locale (const char* locname, const char* codeset)
{
    static const struct {
        const char* locnames [16];
        const char* codesets [16];
    } aliases [] = {
        { {
            // canonical locale name, followed by its common aliases
            "de_DE.ISO-8859-1",
            "de_DE.iso88591", "de_DE.ISO8859-1", "de_DE", "DEU", "de", 0
        }, {
            // well-known codeset name, followed by its common aliases
            "ISO-8859-1",
            "ISO-88591", "ISO_8859-1", "ISO8859-1", "iso88591", "iso1", 0
            // Windows Codepage 1252 (Latin I)
            // http://www.microsoft.com/globaldev/reference/sbcs/1252.htm
        } },
        { {
            "fr_FR.ISO-8859-1",
            "fr_FR.iso88591", "fr_FR.ISO8859-1", "fr_FR", "FRA", "fr", 0
        }, {
            "ISO-8859-1",
            "ISO-88591", "ISO_8859-1", "ISO8859-1", "iso88591", "iso1", 0
        } },
        { {
            "ja_JP.Shift_JIS",
            "ja_JP.SJIS", "japanese.sjis", "JPN", 0
        }, {
            "Shift_JIS",
            "Shift-JIS", "SJIS", "SHIFT-JIS", "SHIFT_JIS", "shift-jis",
            "sjis", 0
            // Windows Codepage 932 (Japanese Shift-JIS)
            // http://www.microsoft.com/globaldev/reference/dbcs/932.htm
        } },
        { {
            "ja_JP.EUC-JP",
            "ja_JP.eucJP", "ja_JP.EUC", "ja_JP.eucjp", 0
        }, {
            "EUC-JP",
            "eucjp", "eucJP", "euc_jp", "euc", "ujis", 0
        } },
        { { 0 }, { 0 } }
    };

    const char* name = std::setlocale (LC_ALL, locname);

    std::size_t inx = std::size_t (-1);

    if (!name) {

        // find the index, `inx', of `locname' in the table
        for (std::size_t i = 0; i != sizeof aliases / sizeof *aliases; ++i) {
            if (!aliases [i].locnames [0])
                break;

            if (!std::strcmp (locname, aliases [i].locnames [0])) {
                inx = i;
                break;
            }
        }

        if (std::size_t (-1) != inx) {

            // go through the locale name's aliases until one
            // that's a valid argument to setlocale() is found
            for (std::size_t j = 1; aliases [inx].locnames [j]; ++j) {
                if (!aliases [inx].locnames [j])
                    break;

                if (std::setlocale (LC_ALL, aliases [inx].locnames [j])) {
                    name = aliases [inx].locnames [j];
                    break;
                }
            }
        }
    }

#ifndef _RWSTD_NO_NL_LANGINFO

    if (name) {
        const char* const cset = nl_langinfo (CODESET);

        if (codeset && std::strcmp (codeset, cset)) {

            // if `codeset' is specified, verify that it matches
            // the codeset used to encode the libc locale found
            // and set above

            for (std::size_t j = 0; aliases [inx].codesets [j]; ++j) {

                if (!std::strcmp (aliases [inx].codesets [j], cset)) {
                    std::setlocale (LC_ALL, "C");
                    return name;
                }
            }
            name = 0;
        }

        std::setlocale (LC_ALL, "C");
    }

#endif   // _RWSTD_NO_NL_LANGINFO

    return name;
}


const char ext_ascii[] = {
    // basic ASCII
    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
    "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
    "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
    "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
    "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
    "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
    "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"
    "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"
    // extended ASCII
    "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
    "\x90\x91\x92\x93\x94\x95\x96\x97\x89\x99\x9a\x9b\x9c\x9d\x9e\x9f"
    "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
    "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
    "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
    "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
    "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
    "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"
};


// tries to find a locale whose internal (wchar_t) representation
// of the Extended ASCII character set does not match the ASCII
// character values; if such locale is found, fills up to `bufsize'
// elements of the `buf' array with such characters and returns
// the name of the locale
static const char*
find_interesting_locale (char* buf, std::size_t bufsize)
{
    std::memset (buf, 0, bufsize);

    const char* name = 0;

    const char* const buf_save = buf;

    for (name = rw_locale_query (); *name; name += std::strlen (name) + 1) {

        if (!std::setlocale (LC_ALL, name))
            continue;

        wchar_t wide_ascii [sizeof ext_ascii] = { 0 };

        // convert the array of narrow Extended ASCII characters
        // to their corresponding wchar_t values using mbstowcs()
        const std::size_t nchars =
            std::mbstowcs (wide_ascii + 1, ext_ascii + 1, sizeof ext_ascii - 2);

        // continue on error
        if (!nchars || nchars > sizeof ext_ascii - 2)
            continue;

        // look for a wide character with value different
        // from the narrow single-byte character
        for (std::size_t i = 0; i != nchars; ++i) {

            typedef unsigned char UChar;

            // convert to unsigned char before comparing
            const UChar uc = UChar (ext_ascii [i]);

            if (unsigned (uc) != unsigned (wide_ascii [i])) {
                if (bufsize) {
                    *buf++ = ext_ascii [i];
                    --bufsize;
                }
            }
        }

        if (buf != buf_save)
            break;
    }

    return name;
}


template <class internT>
static void
test_inout (const char* tname)
{
    // for convenience
    const std::codecvt_base::result error   = std::codecvt_base::error;
    const std::codecvt_base::result noconv  = std::codecvt_base::noconv;
    const std::codecvt_base::result ok      = std::codecvt_base::ok;
    const std::codecvt_base::result partial = std::codecvt_base::partial;

    _RWSTD_UNUSED (noconv);

    const char* locname = 0;

    //////////////////////////////////////////////////////////////////
    // exercise both codecvt::in() and codecvt::out()

    rw_info (0, __FILE__, __LINE__,
             "std::codecvt<%s, char, mbstate_t>::in() "
             "in a generated locale", tname);

    rw_info (0, __FILE__, __LINE__,
             "std::codecvt<%s, char, mbstate_t>::out() "
             "in a generated locale", tname);

    // exercise a single-byte external encoding

    rw_info (0, __FILE__, __LINE__, "<mb_cur_max> 1");

    locname = create_codecvt (1, 1,   // MB_CUR_MAX = 1 (min = 1)
        //        internal     external (single-byte)
        /* NUL */ "<U0000>     /x30\n"   // L"\0" <-> "0"
        /* <A> */ "<U0041>     /x31\n"   // L"A"  <-> "1"
        /* <B> */ "<U0042>     /x32\n"   // L"B"  <-> "2"
        /* <C> */ "<U0043>     /x33\n"   // L"C"  <-> "3"
        /* <D> */ "<U0044>     /x34\n"   // L"D"  <-> "4"
        /* <E> */ "<U0045>     /x35\n"   // L"E"  <-> "5"
        /* <F> */ "<U0046>     /x36\n"   // L"F"  <-> "6"
        /* <Z> */ "<U005A>     /x00\n"   // L"Z"  <-> "\0"

#if _RWSTD_WCHAR_T_MAX > 0xffffU
                  "<U12345678> /x78\n"   // 0x12345678 <-> "x"
                  "<U0DEDBEEF> /x79\n"   // 0x0dedbeef <-> "y"
#else   // if (WCHAR_T_MAX <= 0xffffU)
                  "<U1234> /x78\n"   // 0x1234 <-> "x"
                  "<UBEEF> /x79\n"   // 0xbeef <-> "y"
#endif   // _RWSTD_WCHAR_T_MAX > 0xffffU
        );

    if (!locname) {
        rw_assert (false, __FILE__, __LINE__,
                   "failed to generate locale");
        return;
    }

#define DO_TEST(io, src, ext_len, ext_num, dst, int_len, int_num, res)  \
    test_inout (io, __LINE__, locname, src, ext_len, ext_num,           \
                dst, int_len, int_num, res)

// TEST_IN: tests only codecvt::in()
#define TEST_IN(src, ext_len, ext_num, dst, int_len, int_num, res)  \
    DO_TEST ('I', src, ext_len, ext_num, dst, int_len, int_num, res)

// TEST_OUT: tests only codecvt::out()
#define TEST_OUT(src, ext_len, ext_num, dst, int_len, int_num, res)  \
    DO_TEST ('O', src, ext_len, ext_num, dst, int_len, int_num, res)

// TEST_IN_OUT: tests codecvt::in() and codecvt::out() only
//              if the expected result is codecvt_base::ok
#define TEST_IN_OUT(src, ext_len, ext_num, dst, int_len, int_num, res)  \
    DO_TEST ('i', src, ext_len, ext_num, dst, int_len, int_num, res)

// TEST_OUT_IN: tests codecvt::out() and codecvt::in() only
//              if the expected result is codecvt_base::ok
#define TEST_OUT_IN(src, ext_len, ext_num, dst, int_len, int_num, res)  \
    DO_TEST ('o', src, ext_len, ext_num, dst, int_len, int_num, res)

    //           +------------------------- contents of external (source) buffer
    //           |    +-------------------- size of external (source) sequence
    //           |    |  +----------------- expected size of consumed input
    //           |    |  |  +-------------- expected contents of internal buffer
    //           |    |  |  |     +-------- size of dest. buffer (in chars)
    //           |    |  |  |     |  +----- expected size of output (in chars)
    //           |    |  |  |     |  |  +-- expected result
    //           V    V  V  V     V  V  V
    TEST_IN_OUT ("",  0, 0, L"",  0, 0, ok);
    TEST_IN_OUT ("0", 1, 1, L"",  1, 1, ok);   // "0" maps to wchar_t (0)
    TEST_IN_OUT ("1", 1, 1, L"A", 1, 1, ok);
    TEST_IN_OUT ("2", 1, 1, L"B", 1, 1, ok);
    TEST_IN_OUT ("3", 1, 1, L"C", 1, 1, ok);
    TEST_IN_OUT ("4", 1, 1, L"D", 1, 1, ok);
    TEST_IN_OUT ("5", 1, 1, L"E", 1, 1, ok);
    TEST_IN_OUT ("6", 1, 1, L"F", 1, 1, ok);
    TEST_IN_OUT ("7", 1, 0, L"",  1, 0, error);
    TEST_IN_OUT ("0", 1, 0, L"",  0, 0, partial);   // not enough space

    TEST_IN_OUT ("12",    2, 2, L"AB",              2, 2, ok);
    TEST_IN_OUT ("123",   3, 3, L"ABC",             3, 3, ok);
    TEST_IN_OUT ("0123",  4, 4, L"\0ABC",           4, 4, ok);
    TEST_IN_OUT ("0123Z", 5, 4, L"\0ABC",           5, 4, error);
    TEST_IN_OUT ("01234", 5, 4, L"\0ABC",           4, 4, partial);

#if _RWSTD_WCHAR_T_MAX > 0xffffU

    TEST_IN_OUT ("x", 1, 1, L"\x12345678", 1, 1, ok);
    TEST_IN_OUT ("y", 1, 1, L"\x0dedbeef", 1, 1, ok);

    TEST_IN_OUT ("0123x", 5, 5, L"\0ABC\x12345678", 5, 5, ok);
    TEST_IN_OUT ("123y",  4, 4, L"ABC\x0dedbeef",   4, 4, ok);

#else

    TEST_IN_OUT ("x", 1, 1, L"\x1234", 1, 1, ok);
    TEST_IN_OUT ("y", 1, 1, L"\xbeef", 1, 1, ok);

    TEST_IN_OUT ("0123x", 5, 5, L"\0ABC\x1234", 5, 5, ok);
    TEST_IN_OUT ("123y",  4, 4, L"ABC\xbeef",   4, 4, ok);

#endif   // _RWSTD_WCHAR_T_MAX > 0xffffU


    // exercise multi-byte external encodings

    rw_info (0, __FILE__, __LINE__, "<mb_cur_max> 2");

    locname = create_codecvt (1, 2,   // MB_CUR_MAX = 2 (min = 1)
        //        internal     external (multi-byte)
        /* NUL */ "<U0000>     /x30\n"        // '\0' <-> "0"
        /* <A> */ "<U0041>     /x31/x61\n"    // "A" <--> "1a"
        /* <B> */ "<U0042>     /x32\n"        // "B" <--> "2"
        /* <C> */ "<U0043>     /x33/x62\n"    // "C" <--> "3b"
        /* <D> */ "<U0044>     /x34\n"        // "D" <--> "4"
        /* <E> */ "<U0045>     /x35/x63\n"    // "E" <--> "3c"
        /* <F> */ "<U0046>     /x36\n"        // "F" <--> "4"
        /* <Z> */ "<U005A>     /x00/x00");    // "Z" <--> "\0\0"

    if (!locname) {
        rw_assert (false, __FILE__, __LINE__,
                   "failed to generate locale");
        return;
    }

    TEST_IN_OUT ("",   0, 0, L"",  0, 0, ok);
    TEST_IN_OUT ("0",  1, 1, L"",  1, 1, ok);   // "0" maps to wchar_t (0)
    TEST_IN_OUT ("1a", 2, 2, L"A", 1, 1, ok);
    TEST_IN_OUT ("2",  1, 1, L"B", 1, 1, ok);
    TEST_IN_OUT ("3b", 2, 2, L"C", 1, 1, ok);
    TEST_IN_OUT ("4",  1, 1, L"D", 1, 1, ok);
    TEST_IN_OUT ("5c", 2, 2, L"E", 1, 1, ok);
    TEST_IN_OUT ("6",  1, 1, L"F", 1, 1, ok);
    TEST_IN_OUT ("7",  1, 0, L"",  1, 0, error);
    TEST_IN_OUT ("1",  1, 0, L"",  0, 0, partial);   // not enough space

    // exercise only in()
    // result is partial due to the incomplete multibyte character
    TEST_IN ("3",  1, 0, L"",  1, 0, partial);

    TEST_IN_OUT ("01a23b",    6, 6, L"\0ABC",   4, 4, ok);
    TEST_IN_OUT ("01a23b4",   7, 7, L"\0ABCD",  5, 5, ok);
    TEST_IN_OUT ("01a23b45",  8, 7, L"\0ABCD",  5, 5, partial);
    TEST_IN_OUT ("01a23b45c", 9, 9, L"\0ABCDE", 6, 6, ok);
    TEST_IN_OUT ("01a23c45c", 9, 4, L"\0ABCDE", 6, 3, error);


    rw_info (0, __FILE__, __LINE__, "<mb_cur_max> 6");

    locname = create_codecvt (1, 6,   // MB_CUR_MAX = 6 (min = 1)
        //        internal     external (multi-byte)
        /* NUL */ "<U0000>  /x35\n"                       // "\0" <-> "5"
        /* <A> */ "<U0041>  /x30/x31\n"                   // "A" <--> "01"
        /* <B> */ "<U0042>  /x31/x32/x33\n"               // "B" <--> "123"
        /* <C> */ "<U0043>  /x32/x33/x34/x35\n"           // "C" <--> "2345"
        /* <D> */ "<U0044>  /x33/x34/x35/x36/x37\n"       // "D" <--> "34567"
        /* <E> */ "<U0045>  /x34/x35/x36/x37/x38/x39\n"   // "E" <--> "456789"
        /* <Z> */ "<U005A>  /x00/x00/x00");               // "Z" <--> "\0\0\0"

    if (!locname) {
        rw_assert (false, __FILE__, __LINE__, "failed to generate locale");
        return;
    }

    TEST_IN_OUT ("",              0,  0, L"",    0, 0, ok);
    TEST_IN_OUT ("01",            2,  2, L"A",   1, 1, ok);
    TEST_IN_OUT ("012",           3,  2, L"A",   1, 1, partial);
    TEST_IN_OUT ("0123",          4,  2, L"A",   1, 1, partial);
    TEST_IN_OUT ("01234",         5,  2, L"A",   1, 1, partial);
    TEST_IN_OUT ("012345",        6,  6, L"AC",  2, 2, ok);
    TEST_IN_OUT ("123456789",     9,  9, L"BE",  3, 2, ok);
    TEST_IN_OUT ("456789012345", 12, 12, L"EAC", 3, 3, ok);

    // the first one below is partial even though the external sequence
    // ends in the invalid byte '6' since there is no room in the dest.
    // buffer (this could be either partial or error, the latter if the
    // function verifies the validity of the character before checking
    // whether there is sufficient room in the destination buffer)
    TEST_IN_OUT ("0123456", 7, 6, L"AC", 2, 2, partial /* or error */);
    TEST_IN_OUT ("0123456", 7, 6, L"AC", 3, 2, error);

#ifndef _RWSTD_NO_NL_LANGINFO

    char chars [256];
    const char* const interesting_locale_name =
        find_interesting_locale (chars, sizeof chars);

    if (interesting_locale_name) {

        std::setlocale (LC_ALL, interesting_locale_name);
        const char* const codeset = nl_langinfo (CODESET);

        rw_info (0, __FILE__, __LINE__,
                 "<code_set_name> %s (locale(\"%s\"))",
                 codeset, interesting_locale_name);

        typedef unsigned char UChar;

        char charmap [sizeof chars * 40];

        charmap [0] = '\0';

        for (const char* pc = chars; *pc; ++pc) {
            std::sprintf (charmap + std::strlen (charmap),
                          "<U%04X> /x%02x\n", UChar (*pc), UChar (*pc));
        }

        locname = create_codecvt (1, 1, charmap, codeset);

        if (!locname) {
            rw_assert (false, __FILE__, __LINE__,
                       "failed to generate locale");
            return;
        }

        wchar_t wchars [sizeof chars];
        std::mbstowcs (wchars, chars, std::strlen (chars));

        std::setlocale (LC_ALL, "C");

        for (std::size_t i = 0; chars [i]; ++i) {

            const char    cs [] = { chars [i], '\0' };
            const wchar_t ws [] = { wchars [i], 0 };

            TEST_IN_OUT (cs, 1, 1, ws, 1, 1, ok);
        }
    }

#endif   // _RWSTD_NO_NL_LANGINFO

    // exercise codecvt_byname constructed with the special names below
    const char* const names[] = {
        // "UTF" prefix in all caps causes strict checking of UTF
        // sequences at some loss in performance; "utf" prefix (all
        // lowercase) allows for a faster implementation but accepts
        // some sequences that are strictly speaking invalid (see
        // below)

        "UTF-8", "UTF-8@UCS",

#if _RWSTD_WCHAR_T_MAX > 0xffffU
        "UTF-8@UCS-4",
#else   // if _RWSTD_WCHAR_T_MAX <= 0xffffU
        "UTF-8@UCS-2",
#endif   // WCHAR_T_MAX > 0xffffU

        "utf-8", "utf-8@UCS",

#if _RWSTD_WCHAR_T_MAX > 0xffffU
        "utf-8@UCS-4",
#else   // if _RWSTD_WCHAR_T_MAX <= 0xffffU
        "utf-8@UCS-2",
#endif   // WCHAR_T_MAX > 0xffffU
    };

    for (std::size_t i = 0; i != sizeof names / sizeof *names; ++i) {
        locname = names [i];

        rw_info (0, __FILE__, __LINE__,
                 "codecvt_byname<wchar_t, char, mbstate_t>"
                 "(\"%s\")", locname);

        // strict conformance mode
        const bool strict_mode = 'U' == *locname;

        TEST_IN_OUT ("",     0, 0, L"",     0, 0, ok);
        TEST_IN_OUT ("\0",   1, 1, L"\0",   1, 1, ok);
        TEST_IN_OUT ("a",    1, 1, L"a",    1, 1, ok);
        TEST_IN_OUT ("b",    1, 1, L"b",    1, 1, ok);
        TEST_IN_OUT ("z",    1, 1, L"z",    1, 1, ok);
        TEST_IN_OUT ("\x7f", 1, 1, L"\x7f", 1, 1, ok);

        TEST_IN     ("\x80", 1, 0, L"",     1, 0, error);
        TEST_IN     ("\x81", 1, 0, L"",     1, 0, error);
        TEST_IN     ("\x82", 1, 0, L"",     1, 0, error);
        TEST_IN     ("\xc1", 1, 0, L"",     1, 0, error);

        TEST_IN_OUT ("\xc2\x80", 2, 2, L"\x80", 1, 1, ok);
        TEST_IN_OUT ("\xc2\x81", 2, 2, L"\x81", 1, 1, ok);
        TEST_IN_OUT ("\xc2\x82", 2, 2, L"\x82", 1, 1, ok);
        TEST_IN_OUT ("\xc2\xbf", 2, 2, L"\xbf", 1, 1, ok);

        TEST_IN_OUT ("\xc3\x80", 2, 2, L"\xc0", 1, 1, ok);
        TEST_IN_OUT ("\xc3\x81", 2, 2, L"\xc1", 1, 1, ok);

        TEST_IN_OUT ("\xde\xb0", 2, 2, L"\x07b0", 1, 1, ok);

        TEST_IN_OUT ("\xe0\xa4\x81", 3, 3, L"\x0901", 1, 1, ok);
        TEST_IN_OUT ("\xe3\x8f\xbe", 3, 3, L"\x33fe", 1, 1, ok);
        TEST_IN_OUT ("\xe3\x90\x80", 3, 3, L"\x3400", 1, 1, ok);
        TEST_IN_OUT ("\xe3\x91\x80", 3, 3, L"\x3440", 1, 1, ok);

        if (strict_mode) {
            // otherwise benign invalid patterns detected only in strict mode

            // test the ability to detect and diagnose overlong sequences

            // exercise, e.g., alternative encodings of U+0000 (i.e., <NUL>)
            TEST_IN ("\xc0\x80",                 2, 0, L"", 1, 0, error);
            TEST_IN ("\xe0\x80\x80",             3, 0, L"", 1, 0, error);
            TEST_IN ("\xf0\x80\x80\x80",         4, 0, L"", 1, 0, error);
            TEST_IN ("\xf8\x80\x80\x80\x80",     5, 0, L"", 1, 0, error);
            TEST_IN ("\xfc\x80\x80\x80\x80\x80", 6, 0, L"", 1, 0, error);

            // exercise, e.g., alternative encodings of U+000A (i.e., <LF>)
            TEST_IN ("\xc0\x8a",                 2, 0, L"", 1, 0, error);
            TEST_IN ("\xe0\x80\x8a",             3, 0, L"", 1, 0, error);
            TEST_IN ("\xf0\x80\x80\x8a",         4, 0, L"", 1, 0, error);
            TEST_IN ("\xf8\x80\x80\x80\x8a",     5, 0, L"", 1, 0, error);
            TEST_IN ("\xfc\x80\x80\x80\x80\x8a", 6, 0, L"", 1, 0, error);

            // test the ability to detect and diagnose UTF-16 surrogate
            // pairs and the special code points U+FFFE and U+FFFF

            TEST_OUT ("", 8, 0, L"\xd800", 1, 0, error);
            TEST_OUT ("", 8, 0, L"\xd801", 1, 0, error);
            TEST_OUT ("", 8, 0, L"\xdffe", 1, 0, error);
            TEST_OUT ("", 8, 0, L"\xdfff", 1, 0, error);
            TEST_OUT ("", 8, 0, L"\xfffe", 1, 0, error);
            TEST_OUT ("", 8, 0, L"\xffff", 1, 0, error);

            // the second and subsequent bytes of a multibyte UTF-8
            // sequence must have the following bit pattern: 10xxxxxx
            TEST_IN ("\xc2\x01", 2, 0, L"", 1, 0, error);
            TEST_IN ("\xc2\xff", 2, 0, L"", 1, 0, error);

        }
        else {
            // in relaxed mode the UTF-8 sequences below convert
            // into the UCS characters U+3081 and U+30BF even
            // though the UCS characters convert into 3-byte
            // UTF-8 characters each (see below)
            TEST_IN ("\xc2\x01", 2, 2, L"\x3081", 1, 1, ok);
            TEST_IN ("\xc2\xff", 2, 2, L"\x30bf", 1, 1, ok);
        }

        TEST_OUT ("\xe3\x82\x81", 3, 3, L"\x3081", 1, 1, ok);
        TEST_OUT ("\xe3\x82\xbf", 3, 3, L"\x30bf", 1, 1, ok);

        TEST_IN_OUT ("\x00",      1, 1, L"\x00000000", 1, 1, ok);
        TEST_IN_OUT ("\x7f",      1, 1, L"\x0000007f", 1, 1, ok);

        TEST_IN_OUT ("\xc2\x80",  2, 2, L"\x00000080", 1, 1, ok);
        TEST_IN_OUT ("\xdf\xbf",  2, 2, L"\x000007ff", 1, 1, ok);

        TEST_IN_OUT ("\xe0\xa0\x80", 3, 3, L"\x00000800", 1, 1, ok);

#if _RWSTD_WCHAR_T_MAX > 0xffffU

        TEST_IN_OUT ("\xf0\x90\x8c\x80", 4, 4, L"\x00010300", 1, 1, ok);
        TEST_IN_OUT ("\xf0\x90\x8c\x81", 4, 4, L"\x00010301", 1, 1, ok);
        TEST_IN_OUT ("\xf4\x8f\xbf\x80", 4, 4, L"\x0010ffc0", 1, 1, ok);

        TEST_IN_OUT ("\xf0\x90\x80\x80",         4, 4, L"\x0010000", 1, 1, ok);
        TEST_IN_OUT ("\xf8\x88\x80\x80\x80",     5, 5, L"\x0200000", 1, 1, ok);
        TEST_IN_OUT ("\xfc\x84\x80\x80\x80\x80", 6, 6, L"\x4000000", 1, 1, ok);

#endif   // _RWSTD_WCHAR_T_MAX > 0xffffU
    }
}

/****************************************************************************/

static void
test_ascii (const char *locname, const char* native_locname)
{
    _RWSTD_ASSERT (0 != locname);
    _RWSTD_ASSERT (0 != native_locname);

    rw_info (0, __FILE__, __LINE__,
             "Testing conversion of Extended ASCII range in "
             "locale(\"%s\") against the native locale \"%s\"",
             locname, native_locname);

    wchar_t wascii [sizeof ext_ascii];

    const std::size_t ext_num = sizeof ext_ascii - 2;
          std::size_t int_num = ext_num;

    // try to convert the whole extended ascii range
    if (rwtest_convert_to_internal (use_libc, native_locname,
                                    ext_ascii + 1, ext_num,
                                    wascii, int_num)) {

        wascii [int_num] = L'\0';

        test_inout ('i', __LINE__, locname,
                    ext_ascii + 1, ext_num, ext_num, wascii, int_num, int_num,
                    std::codecvt_base::ok);

        return;
    }

    // conversion of the whole range failed,
    // convert one character at a time

    for (std::size_t i = 1; i != UCHAR_MAX + 1; ++i) {

        if (rwtest_convert_to_internal (use_libc, native_locname,
                                        ext_ascii + i, 1,
                                        wascii, int_num)) {
            wascii [int_num] = L'\0';

            test_inout ('i', __LINE__, locname,
                        ext_ascii + i, 1, 1,
                        wascii, int_num, int_num,
                        std::codecvt_base::ok);
        }
    }
}

/****************************************************************************/
 
// - checks the conversion for each the locales in the locales array
static int
run_test (int /*unused*/, char* /*unused*/ [])
{
    // set up RWSTD_LOCALE_ROOT and other environment variables
    // here as opposed to at program startup to work around a
    // SunOS 5.7 bug in putenv() (PR ##30017)
    locale_root = rw_set_locale_root ();

#if 0   // FIXME

    // codecvt_byname<char, char, mbstate_t> behaves just like
    // the base, codecvt<char, char, mbstate_t>, i.e., it does
    // no conversion
    test_inout (*this, char ());

#endif   // 0/1

    test_inout<wchar_t> ("wchar_t");

    test_virtuals<Codecvt> (0);
    test_virtuals<CodecvtW> ("C");

#ifndef _RWSTD_NO_WCHAR_T

    test_virtuals<CodecvtBn> ("C");
    test_virtuals<CodecvtBnW> ("C");

#endif   // _RWSTD_NO_WCHAR_T

#define TEST_UCS_MODIFIER(T, mod, enc, locname, f, i, is)                    \
    do {                                                                     \
        char locname_mod [256];   /* <locale-name>@<modifier>*/              \
        if (*(mod))                                                          \
            std::sprintf (locname_mod, "%s@%s", locname, (mod));             \
        else                                                                 \
            std::sprintf (locname_mod, "%s", locname);                       \
                                                                             \
        _TRY {                                                               \
            const CodecvtBnW cc (locname_mod, 1);                            \
            std::locale loc = std::locale (std::locale::classic (), &cc);    \
                                                                             \
            cc._C_opts |= cc._C_use_libstd;                                  \
            cc._C_opts &= ~cc._C_use_libc;                                   \
            test_ucs_modifier (locname_mod, enc, s, cc, f, i, is);           \
                                                                             \
        } _CATCH (...) {                                                     \
            rw_assert (false, __FILE__, __LINE__,                            \
                       "unexpected exception in locale test %s",             \
                       locname_mod);                                         \
        }                                                                    \
    } while (0)

#define TEST_LIBSTD(L,T,C,c,f,s,i,is,o,os,ur,ar,er,lr,mr)                  \
    _TRY {                                                                 \
        test_libstd_codecvt<T>(L,c,f,s,i,is,o,os,ur,ar,er,lr,mr);          \
    } _CATCH (...) {                                                       \
            rw_assert (false, __FILE__, __LINE__,                          \
                       "unexpected exception in conversion using %s "      \
                       "(locale %s)", C ? "C library" : "std library", L); \
    }

#define TEST_CONVERSION(T,L,C,f,s,i,is,o,os,ur,er,ar,lr,mr)             \
    do {                                                                \
        std::locale loc;                                                \
        _TRY {                                                          \
            loc = std::locale (L);                                      \
        } _CATCH (...) {                                                \
            rw_assert (false, __FILE__, __LINE__,                       \
                       "locale(\"%s\") unexpectedly threw an "          \
                       "exception; " LOCALE_ROOT_ENVAR " = %s", L,      \
                       locale_root);                                    \
            break;                                                      \
        }                                                               \
        _TRY {                                                          \
            const T &cc = create_##T (loc);                             \
                                                                        \
            cc._C_opts |=  cc._C_use_libstd;                            \
            cc._C_opts &= ~cc._C_use_libc;                              \
            TEST_LIBSTD (L,T,C,cc,f,s,i,is,o,os,ur,ar,er,lr,mr);        \
                                                                        \
            if (C == true && rwtest_is_C_locale (f)) {                  \
                cc._C_opts &= ~cc._C_use_libstd;                        \
                cc._C_opts |=  cc._C_use_libc;                          \
                TEST_LIBSTD(L,T,C,cc,f,s,i,is,o,os,ur,ar,er,lr,mr);     \
            }                                                           \
        } _CATCH (...) {                                                \
            rw_assert (false, __FILE__, __LINE__,                       \
                       "unexpected exception in locale test %s", L);    \
        }                                                               \
    } while (0)


    static const struct {
        const char* src_name;     // locale definition file name
        const char* charmap_name; // charset description file name
        const char* locale_name;  // name of the locale database
        std::size_t nbytes;       // size of test buffer
        std::size_t nchars;       // number of characters in buffer
        std::size_t encoding;     // result of codecvt::encoding()
        std::size_t max_length;   // result of codecvt::max_length()
    } locales [] = {
        // single-width encodings where the number of characters
        // equals the number of bytes (and max_length == 1)
        { "de_DE", "ISO-8859-1", "de_DE.ISO-8859-1",  6826,  6826, 1, 1 },
        { "fr_FR", "ISO-8859-1", "fr_FR.ISO-8859-1",  3920,  3920, 1, 1 },
        // multi-byte encodings (variable width, and max_length > 1)
        { "ja_JP", "Shift_JIS" , "ja_JP.Shift_JIS", 25115, 13001, 0, 2 },
#if !defined _WIN32
        { "ja_JP", "EUC-JP"    , "ja_JP.EUC-JP",    20801, 14299, 0, 3 },
        { "ja_JP", "UTF-8"     , "ja_JP.UTF-8",     25056, 12000, 0, 6 },
#endif // !defined _WIN32

        // terminate the array
        { 0, 0, 0, 0, 0, 0, 0 }
    };


    // Testing actual locale databases with input files
    rw_info (0, __FILE__, __LINE__,
             "Testing locale databases - file conversions.");

    // index of the first successfully built locale database
    int first_good_locale = -1;

    const char* const topdir = std::getenv ("TOPDIR");
    if (!topdir || !*topdir) {
        rw_assert (false, __FILE__, __LINE__,
                   "environment variable TOPDIR not set or empty");
    }

    for (std::size_t i = 0; locales [i].src_name; i++) {

        // Testing actual locale databases with input files

        // try to build a locale database and get its name
        const char* const locname =
            rw_localedef ("-w",
                          locales [i].src_name,
                          locales [i].charmap_name,
                          locales [i].locale_name);

        if (!locname)
            continue;

        // find the native locale name correspomding to the canonical
        // locale name hardcoded in the table above
        const char* const native_locname =
            find_locale (locales [i].locale_name, locales [i].charmap_name);

        if (native_locname)
            test_ascii (locales [i].locale_name, native_locname);
        else {
            std::fprintf (stderr, "*** failed to find a native locale to match"
                          " locale(\"%s\"), skipping Extended ASCII conversion"
                          " test\n", locales [i].locale_name);
        }

#define TESTS_ETC_PATH  "tests" SLASH "etc"

        // determine the full pathname of test files in the source tree
        char pathname [PATH_MAX];
        std::strcpy (pathname, topdir);
        std::strcat (pathname, SLASH TESTS_ETC_PATH SLASH "codecvt.");
        std::strcat (pathname, locales [i].locale_name);
        std::strcat (pathname, ".in");

        const char* const s = pathname;

        // exercise the behavior of codecvt<char, char, mbstate_t>
        TEST_CONVERSION (Codecvt, locales [i].locale_name, true, s, true,
                         std::codecvt_base::noconv, locales [i].nbytes,
                         std::codecvt_base::noconv, locales [i].nbytes,
                         std::codecvt_base::noconv, 1,
                         true, locales [i].nbytes, 1);

        // exercise the behavior of codecvt_byname<char, char, mbstate_t>
        TEST_CONVERSION (CodecvtBn, locales [i].locale_name, true, s, true,
                         std::codecvt_base::noconv, locales [i].nbytes,
                         std::codecvt_base::noconv, locales [i].nbytes,
                         std::codecvt_base::noconv, 1,
                         true, locales [i].nbytes, 1);

#ifndef _RWSTD_NO_WCHAR_T

        // exercise the behavior of codecvt<wchar_t, char, mbstate_t>
        TEST_CONVERSION (CodecvtW, locales [i].locale_name, true, s, true,
                         std::codecvt_base::ok, locales [i].nbytes,
                         std::codecvt_base::ok, locales [i].nbytes,
                         std::codecvt_base::noconv, 1,
                         false, locales [i].nbytes, 1);

        // exercise the behavior of codecvt_byname<wchar_t, char, mbstate_t>
        TEST_CONVERSION (CodecvtBnW, locales [i].locale_name, true, s, false,
                         std::codecvt_base::ok,  locales [i].nchars,
                         std::codecvt_base::ok,  locales [i].nbytes,
                         std::codecvt_base::noconv, locales [i].encoding,
                         false, locales [i].nchars, locales [i].max_length);



        // save the index of the first locale database successfully built
        if (-1 == first_good_locale)
            first_good_locale = _RWSTD_STATIC_CAST (int, i);

        // FIXME: Until the LE (little endian) and BE (big endian) extra
        // modifiers are implemented, the test is skipped for Linux distros
        // such as Red Hat 6.2 that ship with glibc 2.1.1-6

#  if !defined __GLIBC__ \
      || __GLIBC__ == 2 && __GLIBC_MINOR__ > 1 \
      || __GLIBC__ > 2

        // exercise the behavior of codecvt_byname<wchar_t, char, mbstate_t>
        // in constructed with one of the additional @UCS name modifiers
        TEST_UCS_MODIFIER (CodecvtBnW, "", use_libc,
                           locales [i].locale_name, s, std::codecvt_base::ok,
                           locales [i].nchars);

        const InternalEncoding which_ucs =
            sizeof (wchar_t) == 4 ? use_UCS4 : use_UCS2;

        TEST_UCS_MODIFIER (CodecvtBnW,
                           use_UCS4 == which_ucs ? "UCS-4" : "UCS-2", which_ucs,
                           locales [i].locale_name, s, std::codecvt_base::ok,
                           locales [i].nchars);

        TEST_UCS_MODIFIER (CodecvtBnW, "UCS", which_ucs,
                           locales [i].locale_name, s, std::codecvt_base::ok,
                           locales [i].nchars);

#  endif   // GLIBC version > 2.1
#endif   // _RWSTD_NO_WCHAR_T

    }

    // exercise the behavior of the facets when passed
    // an uninitialized or corrupt mbstate_t object
    test_invalid_args (-1 == first_good_locale ?
                       (char*)0 : locales [first_good_locale].locale_name);

    return 0;
}


/*extern*/ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "[lib.locale.codecvt]",
                    "",   // no comment
                    run_test, "", 0);
}


