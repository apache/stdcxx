/***************************************************************************
 *
 * 22.locale.synopsis.cpp - test exercising synopsis of header <locale>
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#include <rw/_defs.h>

// disable implicit inclusion to work around a limitation in
// IBM VAC++ 5.0.2.0 (PR #26959)
#if defined __IBMCPP__ && !defined _RWSTD_NO_IMPLICIT_INCLUSION
#  define _RWSTD_NO_IMPLICIT_INCLUSION
#endif

// make protected members accessible if possible
#if defined _RWSTD_NO_EXPORT && !defined _MSC_VER
#  define protected public
#endif

#include <iterator>
#include <locale>

#include <rw_driver.h>

static void
test_lc_defs ()
{
    rw_info (0, __FILE__, __LINE__, "checking LC_XXX constants");

    // check that <locale> doesn't introduce any LC_XXX constants
    // (not required, but it's good practice to keep headers clean)
    static const char* const lc_defs[] =
    {
#ifdef LC_ALL
        "LC_ALL",
#endif   // LC_ALL
#ifdef LC_COLLATE
        "LC_COLLATE",
#endif   // LC_COLLATE
#ifdef LC_CTYPE
        "LC_CTYPE",
#endif   // LC_CTYPE
#ifdef LC_MONETARY
        "LC_MONETARY",
#endif   // LC_MONETARY
#ifdef LC_NUMERIC
        "LC_NUMERIC",
#endif   // LC_NUMERIC
#ifdef LC_TIME
        "LC_TIME",
#endif   // LC_TIME
#ifdef LC_MESSAGES
        "LC_MESSAGES",
#endif   // LC_MESSAGES

        0
    };

    for (unsigned i = 0; i != sizeof lc_defs / sizeof *lc_defs; ++i) {
        rw_assert (!lc_defs [i], __FILE__, __LINE__,
                   "<locale> unexpectedly #defines %s", lc_defs [i]);
    }
}

static void
test_isxxx ()
{
    rw_info (0, __FILE__, __LINE__, "checking isxxx() macros");

    // check that <locale> doesn't introduce any isxxx() macros
    static const char* const isxxx[] =
    {
#ifdef isspace
        "isspace",
#endif   // isspace
#ifdef isprint
        "isprint",
#endif   // isprint
#ifdef iscntrl
        "iscntrl",
#endif   // iscntrl
#ifdef isupper
        "isupper",
#endif   // isupper
#ifdef islower
        "islower",
#endif   // islower
#ifdef isalpha
        "isalpha",
#endif   // isalpha
#ifdef isdigit
        "isdigit",
#endif   // isdigit
#ifdef ispunct
        "ispunct",
#endif   // ispunct
#ifdef isxdigit
        "isxdigit",
#endif   // isxdigit
#ifdef isalnum
        "isalnum",
#endif   // isalnum
#ifdef isgraph
        "isgraph",
#endif   // isgraph
#ifdef toupper
        "toupper",
#endif   // toupper
#ifdef tolower
        "tolower",
#endif   // tolower

        0
    };

    for (unsigned i = 0; i != sizeof isxxx / sizeof *isxxx; ++i) {
        rw_assert (!isxxx [i], __FILE__, __LINE__,
                   "<locale> unexpectedly #defines %s", isxxx [i]);
    }
}

/***************************************************************************/

#include <clocale>   // for LC_XXX macros
#include <cwchar>    // for mbstate_t

#if !defined LC_MESSAGES && defined _RWSTD_LC_MESSAGES
#  define LC_MESSAGES _RWSTD_LC_MESSAGES
#endif   // !LC_MESSAGES && _RWSTD_LC_MESSAGES

// exercise the signature of a nonmember (or static member) function
#define FUN(result, name, arg_list) do {                              \
        /* make name unique to prevent bogus gcc -Wshadow warnings */ \
        result (*_RWSTD_PASTE (pf, __LINE__)) arg_list = &name;       \
        _RWSTD_UNUSED (_RWSTD_PASTE (pf, __LINE__));                  \
    } while (0)

/***************************************************************************/

// exercise the signatures of std::{has,use}_facet<>()
template <class charT>
static void
test_have_use_facet ()
{
    rw_info (0, __FILE__, __LINE__,
             "std::has_facet<Facet>(std::locale&) synopsis");
    rw_info (0, __FILE__, __LINE__,
             "std::use_facet<Facet>(std::locale&) synopsis");

#if !defined __HP_aCC || __HP_aCC > 33100

// working around an HP aCC bug (see PR #23312)

#  if    !(defined __GNUG__ || __GNUG__ > 2 || __GNUC_MINOR__ > 96) \
      && !(defined _MSC_VER || _MSC_VER >= 0x1300)

    // working around a gcc 2.9{5,6} ICE
    // working around an MSVC 6.0 bug (PR #26307)
    FUN (const std::ctype<charT>&,
         std::use_facet<std::ctype<charT> >, (const std::locale&));
    FUN (bool, std::has_facet<std::ctype<charT> >, (const std::locale&));

#  endif   // gcc > 2.96 && MSVC > 6.0

#endif   // __HP_aCC > 33000
}

/***************************************************************************/

// exercise the signatures of the convenience locale interfaces
template <class charT>
static void
test_convenience_funs ()
{
    rw_info (0, __FILE__, __LINE__,
             "std::isxxx(std::locale&) convenience functions synopsis");

    FUN (bool, std::isspace, (charT, const std::locale&));
    FUN (bool, std::isprint, (charT, const std::locale&));
    FUN (bool, std::iscntrl, (charT, const std::locale&));
    FUN (bool, std::isupper, (charT, const std::locale&));
    FUN (bool, std::islower, (charT, const std::locale&));
    FUN (bool, std::isalpha, (charT, const std::locale&));
    FUN (bool, std::isdigit, (charT, const std::locale&));
    FUN (bool, std::ispunct, (charT, const std::locale&));
    FUN (bool, std::isxdigit, (charT, const std::locale&));
    FUN (bool, std::isalnum, (charT, const std::locale&));
    FUN (bool, std::isgraph, (charT, const std::locale&));
    FUN (charT, std::toupper, (charT, const std::locale&));
    FUN (charT, std::tolower, (charT, const std::locale&));
}

_RWSTD_NAMESPACE (std) {

#if !defined _MSC_VER

// verify that specializations are present in the lib
_RWSTD_SPECIALIZED_CLASS class ctype<char>;
_RWSTD_SPECIALIZED_CLASS class ctype_byname<char>;

#  ifndef _RWSTD_NO_WCHAR_T

_RWSTD_SPECIALIZED_CLASS class ctype<wchar_t>;
_RWSTD_SPECIALIZED_CLASS class ctype_byname<wchar_t>;

#  endif   // _RWSTD_NO_WCHAR_T

#endif   // _MSC_VER

}   // namespace std

/***************************************************************************/

// test for the presence of the required facet templates
// and their specializations (as required by Table 52)
template <class charT>
static void
test_facets ()
{
    // introduce a typedef to work around a SunPro 5.3 bug (see PR #25967)
    typedef charT char_type;

    const std::locale::facet *pfac;
    _RWSTD_UNUSED (pfac);

    {
        // collate category
        const std::collate<char_type>        *col        = 0;
        const std::collate_byname<char_type> *col_byname = 0;

        // verify that facet publicly derives from locale::facet
        pfac = col;
        col  = col_byname;
    }

    {
        // ctype category

        // force a link failure if required specializations aren't
        // provided (e.g., if only the primary template is defined)
        const std::ctype<char_type>        *ctp = 0;
        const std::ctype_byname<char_type> *ctp_byname = 0;

        // verify that facet publicly derives from locale::facet
        const std::ctype_base *ctpbase = ctp;
        pfac = ctp;
        ctp  = ctp_byname;

        _RWSTD_UNUSED (ctpbase);
    }

    {
        const std::codecvt<char_type, char, std::mbstate_t> *cvt = 0;
        const std::codecvt_byname<char_type, char, std::mbstate_t>
            *cvt_byname = 0;

        // verify that facet publicly derives from locale::facet
        pfac = cvt;

        const std::codecvt_base *pcvtbase = cvt;
        _RWSTD_UNUSED (pcvtbase);

        cvt = cvt_byname;
    }

    typedef std::char_traits<char_type>               CTr;
    typedef std::istreambuf_iterator<char_type, CTr > IIt;
    typedef std::ostreambuf_iterator<char_type, CTr > OIt;

    {
        // monetary category

        const std::moneypunct<char_type, false>        *mpun             = 0;
        const std::moneypunct<char_type, true>         *mpun_intl        = 0;
        const std::moneypunct_byname<char_type, false> *mpun_byname      = 0;
        const std::moneypunct_byname<char_type, true>  *mpun_intl_byname = 0;
        const std::money_get<char_type, IIt>           *mget             = 0;
        const std::money_put<char_type, OIt>           *mput             = 0;

        // verify that facets publicly derive from locale::facet
        pfac = mpun;
        pfac = mpun_intl;
        pfac = mget;
        pfac = mput;

        mpun      = mpun_byname;
        mpun_intl = mpun_intl_byname;

        // verify that punct facets publicly derive from money_base
        const std::money_base *pmbase = mpun;
        pmbase = mpun_intl;
        _RWSTD_UNUSED (pmbase);

#ifndef _RWSTD_NO_DEFAULT_TEMPLATE_ARGS

        const std::moneypunct<char_type> *pmpun_noarg = mpun;
        _RWSTD_UNUSED (pmpun_noarg);

        // verify default template arguments
        const std::money_get<char_type> *pmget = mget;
        const std::money_put<char_type> *pmput = mput;
        _RWSTD_UNUSED (pmget);
        _RWSTD_UNUSED (pmput);

#endif   // _RWSTD_NO_DEFAULT_TEMPLATE_ARGS

    }

    {
        // numeric category

        const std::num_get<char_type, IIt> *nget = 0;
        const std::num_put<char_type, OIt> *nput = 0;

        // verify that facets publicly derive from locale::facet
        pfac = nget;
        pfac = nput;

#ifndef _RWSTD_NO_DEFAULT_TEMPLATE_ARGS

        // verify default template arguments
        const std::num_get<char_type> *pnget = nget;
        const std::num_put<char_type> *pnput = nput;
        _RWSTD_UNUSED (pnget);
        _RWSTD_UNUSED (pnput);

#endif   // _RWSTD_NO_DEFAULT_TEMPLATE_ARGS

    }

    {
        // time category

        const std::time_get<char_type, IIt> *tget = 0;
        const std::time_put<char_type, OIt> *tput = 0;

        // verify that facets publicly derive from locale::facet
        pfac = tget;
        pfac = tput;

#ifndef _RWSTD_NO_DEFAULT_TEMPLATE_ARGS

        // verify default template arguments
        const std::time_get<char_type> *ptget = tget;
        const std::time_put<char_type> *ptput = tput;
        _RWSTD_UNUSED (ptget);
        _RWSTD_UNUSED (ptput);

#endif   // _RWSTD_NO_DEFAULT_TEMPLATE_ARGS

    }

    {
        // messages category

        const std::messages<char_type>        *msgs = 0;
        const std::messages_byname<char_type> *msgs_byname = 0;

        // verify that facets publicly derive from locale::facet
        pfac = msgs;

        const std::messages_base       *pmsgbase = msgs;
        const std::messages<char_type> *pmsgs    = msgs_byname;

        _RWSTD_UNUSED (pmsgbase);
        _RWSTD_UNUSED (pmsgs);
    }
}

/***************************************************************************/

// helper to verify that locale and facet ctors are explicit
// not defined since they must not be referenced if test is successful
void is_explicit (const std::locale&);
void is_explicit (const std::locale::facet&);

void is_explicit (const std::collate<char>&);
void is_explicit (const std::ctype<char>&);

#  ifndef _RWSTD_NO_WCHAR_T

void is_explicit (const std::collate<wchar_t>&);
void is_explicit (const std::ctype<wchar_t>&);

#  endif   // _RWSTD_NO_WCHAR_T

struct has_implicit_ctor
{
    // NOT explicit

    // for locale::facet::facet (size_t = 0) and derived facet ctors
    has_implicit_ctor (std::size_t) { /* empty */ }

    // for ctype<char>::ctype (const mask*, bool = false, size_t = 0)
    has_implicit_ctor (const std::ctype_base::mask*) { /* empty */ }

    // for locale::locale (const char*, size_t = 0)
    has_implicit_ctor (const char*) { /* empty */ }
};

// calls to the overloaded is_explicit() resolve to the function below
void is_explicit (const has_implicit_ctor&) { /* empty */ }

/***************************************************************************/

// local to test_locale
static bool dtor_called = false;

struct Facet: std::locale::facet
{
    typedef std::locale::facet Base;

    // default argument provided to work around a library limitation
    // that requires all facets to have default ctors
    Facet (std::size_t refs = 0): Base (refs) { /* empty */ }

    ~Facet () {
        dtor_called = true;
    }

    static std::locale::id id;
};


std::locale::id Facet::id;

// using a global in favor of a static local
// to work around an MSVC 6.0 bug (see PR #26305)
static bool is_dtor_virtual;


template <class charT>
struct StringTypes
{
    typedef std::char_traits<charT>                          traits_type;
    typedef std::allocator<charT>                            Allocator;
    typedef std::basic_string<charT, traits_type, Allocator> string_type;
};


// test class locale definition
static void
test_locale ()
{
    rw_info (0, __FILE__, __LINE__, "class std::locale synopsis");

    // 21.1.1.1.1
    const std::locale::category *pcat = (int*)0;

#ifndef _RWSTD_NO_STATIC_CONST_MEMBER_INIT

    // verify that category constants are defined
    pcat = &std::locale::none;
    pcat = &std::locale::collate;
    pcat = &std::locale::ctype;
    pcat = &std::locale::monetary;
    pcat = &std::locale::numeric;
    pcat = &std::locale::time;
    pcat = &std::locale::messages;
    pcat = &std::locale::all;

#endif   // _RWSTD_NO_STATIC_CONST_MEMBER_INIT

    // verify that category constants are constant integral expressions
    switch (*pcat) {
    case std::locale::none:
    case std::locale::collate:
    case std::locale::ctype:
    case std::locale::monetary:
    case std::locale::numeric:
    case std::locale::time:
    case std::locale::messages:
    case std::locale::all:
        break;
    }

    static const int c_cats [] = {
        LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME

#ifdef LC_MESSAGES
        , LC_MESSAGES
#endif   // LC_MESSAGES

    };

    static const std::locale::category cats[] = {
        std::locale::none, std::locale::collate, std::locale::ctype,
        std::locale::monetary, std::locale::numeric, std::locale::time,
        std::locale::messages, std::locale::all
    };

    for (pcat = cats; pcat != cats + sizeof cats / sizeof *cats - 1; ++pcat) {

        bool success = true;

        for (unsigned i = pcat - cats + 1; i < sizeof cats / sizeof *cats; ++i)
            if (   *pcat != std::locale::all
                && cats [i] != std::locale::all
                && (*pcat & cats [i]))
                rw_assert (success = false, __FILE__, __LINE__,
                           "(%{Lc} & %{Lc}) != 0", *pcat, cats [i]);

        for (unsigned j = 0; j != sizeof c_cats / sizeof *c_cats; ++j) {
            // locale::category values must not conflict with LC_XXX constants
            if (*pcat & c_cats [j])
                rw_assert (success = false, __FILE__, __LINE__,
                           "(%{Lc} & %{Lc}) != 0", *pcat, c_cats [j]);

            if (*pcat == c_cats [j])
                rw_assert (success = false, __FILE__, __LINE__,
                           "%{Lc} != %{Lc}", *pcat, c_cats [j]);
        }

        rw_assert (success, __FILE__, __LINE__, "%{Lc}", *pcat);
    }

    // 21.1.1.1.1, p1
    const int all =   std::locale::collate
                    | std::locale::ctype
                    | std::locale::monetary
                    | std::locale::numeric
                    | std::locale::time
                    | std::locale::messages;

    rw_assert (std::locale::all == all, __FILE__, __LINE__,
               "std::locale::all == %#x, got %#x", all, std::locale::all);

    // 22.1.1.1.2

    // verify that locale::facet::facet (size_t) is explicit
    is_explicit (std::size_t ());

    {
        // verify that ctor takes a default argument
        struct Facet: std::locale::facet {

            typedef std::locale::facet Base;

            Facet (): Base () { /* empty */ }
            Facet (std::size_t ref): Base (ref) {/* empty */  }
            ~Facet () {
                is_dtor_virtual = true;
            }

        };

        // delete a derived object using a pointer to the base
        delete (std::locale::facet*)new Facet ();
        rw_assert (is_dtor_virtual, __FILE__, __LINE__,
                   "locale::facet::~facet() not virtual");
    }

    // 22.1.1.1.3 - dfeault ctor and dtor are public
    std::locale::id id;
    _RWSTD_UNUSED (id);

    // 22.1.1.2, p1
    const std::locale l;

    // 22.1.1.2, p3
    std::locale l2 (l);

    // 22.1.1.2, p6
    (void)std::locale ("");

    // verify that locale::locale (const char*) is explicit
    is_explicit ((const char*)"");

    // 22.1.1.2, p9
    (void)std::locale (l, "", std::locale::all);

    Facet *pf = new Facet (1 /* ref count */);

    {
        // 22.1.1.2, p12
        // enclose unnamed temporary in its own scope to work around
        // a SunPro 5.4 bug which amkes it calls the dtor too late
        // see PR #27543
        (void)std::locale (l, pf);
    }

    // exercise 22.1.1.1.2, p2, refs == 1
    rw_assert (!dtor_called, __FILE__, __LINE__,
               "facet dtor called for ref count of 1");

    if (!dtor_called)
        delete pf;

    dtor_called = false;

    // exercise 22.1.1.1.2, p2, refs == 0
    pf = new Facet (0 /* ref count */);

    {
        // create an unnamed temporary object and force it to go out of
        // scope (in case the compiler, such as SunPro 5.4, fails to
        // detsroy it at the end of the expression) so that it is forced
        // to destroy the user-defined facet
        (void)std::locale (l, pf);
    }

    rw_assert (dtor_called, __FILE__, __LINE__,
               "UD facet dtor not called for ref count of 0");

    if (!dtor_called)
        delete pf;

    pf = 0;

    // 22.1.1.2, p14
    (void)std::locale (l, l, std::locale::all);

// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#define MEMFUN(result, name, arg_list)                                   \
        do {                                                             \
           /* jumping thru hoops to avoid bogus gcc -Wshadow warnings */ \
           result (std::locale::* _RWSTD_PASTE (pf, __LINE__))           \
               arg_list = &std::locale::name;                            \
           _RWSTD_UNUSED (_RWSTD_PASTE (pf, __LINE__));                  \
         }  while (0)

#if !defined __HP_aCC || __HP_aCC > 33100

    // working around an HP aCC bug (see PR #23312)

    // 22.1.1.2, p4
    MEMFUN (const std::locale&, operator=,(const std::locale&) _PTR_THROWS(()));

#endif   // __HP_aCC > 33000

#if !defined __HP_aCC || __HP_aCC > 33100

    // working around an HP aCC bug (see PR #23312)

    // 22.1.1.3, p1
    MEMFUN (std::locale, combine<Facet>, (const std::locale&) const);

#endif   // __HP_aCC > 33000

    // 22.1.1.3, p5
    MEMFUN (std::string, name, () const);

    // 22.1.1.4, p1
    MEMFUN (bool, operator==, (const std::locale&) const);

    // 22.1.1.4, p2
    MEMFUN (bool, operator!=, (const std::locale&) const);

    // 22.1.1.4, p3
    MEMFUN (bool, operator(), (const std::string&, const std::string&) const);

#ifndef _RWSTD_NO_WCHAR_T

    MEMFUN (bool, operator(), (const std::wstring&, const std::wstring&)const);

#endif   // _RWSTD_NO_WCHAR_T

    // exercise a specialization other than on the default char types
    typedef StringTypes<int>::string_type IntString;

    MEMFUN (bool, operator(), (const IntString&, const IntString&) const);

    // 22.1.1.5
    FUN (std::locale, std::locale::global, (const std::locale&));
    FUN (const std::locale&, std::locale::classic, ());
}

/***************************************************************************/

// test class template ctype definition

template <class charT>
struct CtypeDerived: std::ctype<charT>
{
    typedef std::ctype<charT>     CtypeT;
    typedef std::ctype_base::mask M;
    typedef const charT*          PtrT;

// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#undef MEMFUN
#define MEMFUN(result, name, arg_list)                       \
        do {                                                 \
            result (CtypeT::* pf) arg_list = &CtypeT::name;  \
            _RWSTD_UNUSED (pf);                              \
         }  while (0)

    static void test_protected_members () {
        // protected members (see `#define protected public' above)
        MEMFUN (bool, do_is, (M, charT) const);
        MEMFUN (PtrT, do_is, (PtrT, PtrT, M*) const);

        // lwg issue 124
        MEMFUN (PtrT, do_scan_is, (M, PtrT, PtrT) const);
        MEMFUN (PtrT, do_scan_not, (M, PtrT, PtrT) const);
        MEMFUN (charT, do_toupper, (charT) const);
        MEMFUN (PtrT, do_toupper, (charT*, PtrT) const);
        MEMFUN (charT, do_tolower, (charT) const);
        MEMFUN (PtrT, do_tolower, (charT*, PtrT) const);
        MEMFUN (charT, do_widen, (char) const);
        MEMFUN (const char*,
                do_widen, (const char*, const char*, charT*) const);
        MEMFUN (char, do_narrow, (charT, char) const);
        MEMFUN (PtrT, do_narrow,
                (PtrT, PtrT, char, char*) const);
    }
};


// exercise the primary template
template <class charT>
/*static*/ void
test_ctype_specialization ()
{
    CtypeDerived<charT>::test_protected_members ();
}


struct CtypeCharDerived: std::ctype<char>
{
    typedef std::ctype<char>      CtypeT;
    typedef std::ctype_base::mask Mask;

    // verify that all required ctors (or one with default args) exist
    CtypeCharDerived (): CtypeT () { /* empty */ }
    CtypeCharDerived (const Mask *tbl): CtypeT (tbl) { /* empty */ }
    CtypeCharDerived (const Mask *tbl, bool del): CtypeT (tbl, del) { /* empty */ }
    CtypeCharDerived (const Mask *tbl, bool del, std::size_t ref)
        : CtypeT (tbl, del, ref) { /* empty */ }

    static void test_protected_members () {

#ifndef _RWSTD_NO_STATIC_CONST_MEMBER_INIT

        // verify that table_size is properly defined
        const std::size_t *psz = &std::ctype<char>::table_size;

#else

        const std::size_t dummy = std::ctype<char>::table_size;
        const std::size_t *psz  = &dummy;

#endif   // _RWSTD_NO_STATIC_CONST_MEMBER_INIT

        // verify that table_size is an integral constant expression
        switch (*psz) {
        case std::ctype<char>::table_size: break;
        }

        MEMFUN (char, do_toupper, (char) const);
        MEMFUN (const char*, do_toupper, (char*, const char*) const);
        MEMFUN (char, do_tolower, (char) const);
        MEMFUN (const char*, do_tolower, (char*, const char*) const);
        MEMFUN (char, do_widen, (char) const);
        MEMFUN (const char*, do_widen,
                (const char*, const char*, char*) const);
        MEMFUN (char, do_narrow, (char, char) const);
        MEMFUN (const char*, do_narrow,
                (const char*, const char*, char, char*) const);

        // 22.2.1.3.2, p12
        MEMFUN (const std::ctype_base::mask*,
                table, () const _PTR_THROWS (()));

        // 22.2.1.3.3
        FUN (const std::ctype_base::mask*,
             CtypeT::classic_table, () _PTR_THROWS (()));
    }
};


// exercise the char specialization
_RWSTD_SPECIALIZED_FUNCTION
/*static*/ void
test_ctype_specialization<char> ()
{
    // prevent multiple assertions
    static int called /* = false */;
    if (called++)
        return;

    // 22.2.1.3.2, p3 - verify that ctor is explicit
    is_explicit ((const std::ctype_base::mask*)0);

    CtypeCharDerived::test_protected_members ();
}


// skip POSIX-dependent behavior
static int opt_enable_posix      = 0;   // --enable-posix


template <class charT>
static void
test_ctype (const char* cname)
{
    rw_info (0, __FILE__, __LINE__,
             "std::ctype<%s> synopsis", cname);

    std::ctype_base::mask m = std::ctype_base::space;
    _RWSTD_UNUSED (m);

    const std::ctype_base::mask *pm = 0;
    _RWSTD_UNUSED (pm);

    // verify that mask constants are constant integral expressions
    // whose address can be taken
    switch (m) {

#ifndef _RWSTD_NO_STATIC_CONST_MEMBER_INIT
   // see lwg issue 339
#  define ADDR(name)     pm = &std::ctype_base::name
#else
#  define ADDR(ignore)   (void)0
#endif

#if !defined __IBMCPP__ || __IBMCPP__ > 800
    // working around an IBM VAC++ bug #559
    case std::ctype_base::mask (): break;
#endif   // !VAC+ || VAC++ > 7.0

    case std::ctype_base::space:  ADDR (space); break;
    case std::ctype_base::print:  ADDR (print); break;
    case std::ctype_base::cntrl:  ADDR (cntrl); break;
    case std::ctype_base::upper:  ADDR (upper); break;
    case std::ctype_base::lower:  ADDR (lower); break;
    case std::ctype_base::alpha:  ADDR (alpha); break;
    case std::ctype_base::digit:  ADDR (digit); break;
    case std::ctype_base::punct:  ADDR (punct); break;
    case std::ctype_base::xdigit: ADDR (xdigit); break;
    case std::ctype_base::alnum:  ADDR (alnum); break;
    case std::ctype_base::graph:  ADDR (graph); break;

    default: break;
    }

    // prevent unnecessary assertions
    if (sizeof (charT) == sizeof (char)) {
        rw_assert (std::ctype_base::alnum ==
                   (std::ctype_base::alpha | std::ctype_base::digit),
                   __FILE__, __LINE__,
                   "std::ctype_base::alnum = %#x, expected %#x",
                   std::ctype_base::alnum,
                   std::ctype_base::alpha | std::ctype_base::digit);

        if (opt_enable_posix) {
            // POSIX allows graph characters that aren't alnum or punct
            // 22.2.1 says the numeric values are for exposition only
            // which presumably applies to all the constants
            rw_assert (std::ctype_base::graph
                       == (std::ctype_base::alnum | std::ctype_base::punct),
                       __FILE__, __LINE__,
                       "std::ctype_base::graph = %#x, expected %#x",
                       std::ctype_base::graph,
                       std::ctype_base::alnum | std::ctype_base::punct);
        }
    }

    typedef std::ctype<charT>     CtypeT;
    typedef typename CtypeT::mask Mask;

    charT *pc = (typename CtypeT::char_type*)0;
    _RWSTD_UNUSED (pc);

    typedef const charT* PtrT;

    MEMFUN (bool, is, (Mask, charT) const);
    MEMFUN (PtrT, is, (PtrT, PtrT, Mask*) const);
    MEMFUN (PtrT, scan_is, (Mask, PtrT, PtrT) const);
    MEMFUN (PtrT, scan_not, (Mask, PtrT, PtrT) const);
    MEMFUN (charT, toupper, (charT) const);
    MEMFUN (PtrT, toupper, (charT*, PtrT) const);
    MEMFUN (charT, tolower, (charT) const);
    MEMFUN (PtrT, tolower, (charT*, PtrT) const);
    MEMFUN (charT, widen, (char) const);
    MEMFUN (const char*, widen, (const char*, const char*, charT*) const);
    MEMFUN (char, narrow, (charT, char) const);
    MEMFUN (PtrT, narrow,
            (PtrT, PtrT, char, char*) const);

    std::locale::id *pid = &CtypeT::id;
    _RWSTD_UNUSED (pid);

    // exercise virtuals and other parts that differ between
    // the primary template and the char specialization
    test_ctype_specialization<charT> ();
}

/***************************************************************************/

template <class charT>
struct CodecvtDerived: std::codecvt<charT, char, std::mbstate_t>
{
    typedef std::codecvt<charT, char, std::mbstate_t> Codecvt;
    typedef std::codecvt_base::result                 Result;
    typedef typename Codecvt::intern_type             InternT;
    typedef typename Codecvt::extern_type             ExternT;
    typedef typename Codecvt::state_type              StateT;

// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#undef MEMFUN
#define MEMFUN(result, name, arg_list)                        \
        do {                                                  \
           result (Codecvt::*pf) arg_list = &Codecvt::name;   \
           _RWSTD_UNUSED (pf);                                \
         }  while (0)

    void test_protected_members () {
            // protected virtuals
            MEMFUN (Result, do_out,
                    (StateT&, const InternT*, const InternT*, const InternT*&,
                     ExternT*, ExternT*, ExternT*&) const);

            MEMFUN (Result, do_unshift,
                    (StateT&, ExternT*, ExternT*, ExternT*&) const);

            MEMFUN (Result, do_in,
                    (StateT&, const ExternT*, const ExternT*, const ExternT*&,
                     InternT*, InternT*, InternT*&) const);

            MEMFUN (int, do_encoding, () const _PTR_THROWS (()));
            MEMFUN (bool, do_always_noconv, () const _PTR_THROWS (()));

            // lwg issue 75 changes first argument from const StateT& to State&
            MEMFUN (int, do_length,
                    (StateT&, const ExternT*, const ExternT*,
                     std::size_t) const);

            MEMFUN (int, do_max_length, () const _PTR_THROWS (()));
        }
    };


template <class charT>
static void
test_codecvt (const char* cname)
{
    rw_info (0, __FILE__, __LINE__,
             "std::codecvt<%s, char, std::mbstate_t> synopsis", cname);

    // verify base class
    std::codecvt_base b;
    std::codecvt_base::result r = b.ok;

    // prevent a bogus MSVC warning 4101 (see PR #24178)
    _RWSTD_UNUSED (b);

    // verify that result is a constant integral expression
    switch (r) {
    case std::codecvt_base::ok:
    case std::codecvt_base::partial:
    case std::codecvt_base::error:
    case std::codecvt_base::noconv:
        break;
    }

    typedef std::codecvt<charT, char, std::mbstate_t> Codecvt;

    // verify that nested types exist
    typedef typename Codecvt::intern_type  InternT;
    typedef typename Codecvt::extern_type  ExternT;
    typedef typename Codecvt::state_type   StateT;

    // verify nested types
    const InternT *pint = (const charT*)0;
    const ExternT *pext = (const char*)0;
    const StateT  *pst  = (const std::mbstate_t*)0;

    _RWSTD_UNUSED (pint);
    _RWSTD_UNUSED (pext);
    _RWSTD_UNUSED (pst);

    typedef std::codecvt_base::result Result;

    // public non-virtuals
    MEMFUN (Result, out,
            (StateT&, const InternT*, const InternT*, const InternT*&,
             ExternT*, ExternT*, ExternT*&) const);

    MEMFUN (Result, unshift, (StateT&, ExternT*, ExternT*, ExternT*&) const);

    MEMFUN (Result, in,
            (StateT&, const ExternT*, const ExternT*, const ExternT*&,
             InternT*, InternT*, InternT*&) const);

    MEMFUN (int, encoding, () const _PTR_THROWS (()));
    MEMFUN (bool, always_noconv, () const _PTR_THROWS (()));

    // lwg issue 75 changes the first argument from const StateT& to State&
    MEMFUN (int, length,
            (StateT&, const ExternT*, const ExternT*, std::size_t) const);

    MEMFUN (int, max_length, () const _PTR_THROWS (()));

    std::locale::id *pid = &Codecvt::id;
    _RWSTD_UNUSED (pid);

    CodecvtDerived<charT>().test_protected_members ();
}

/***************************************************************************/

// user-defined character type; used to exercise requirements in Table 52
// i.e., specializations of facets on types other than char and wchar_t
#define UDC int

// numeric category

namespace std {

// provide an explicit specialization of numpunct on UDC necessary
// in order to exercise num_get<UDC> and num_put<UDC>

_RWSTD_SPECIALIZED_CLASS
struct numpunct<UDC>: locale::facet
{
    typedef UDC                                 char_type;
    typedef StringTypes<char_type>::string_type string_type;

    // working around an MSVC 6.0 bug
    typedef locale::facet           Base;

    explicit numpunct (size_t refs = 0)
        : Base (refs) { /* empty */ }

    static locale::id id;

    char_type decimal_point () const { return char_type (); }
    char_type thousands_sep () const { return char_type (); }
    string grouping () const { return ""; }
    string_type falsename () const { return string_type (); }
    string_type truename () const { return string_type (); }
};

locale::id numpunct<UDC>::id;

}   // namespace std


template <class charT>
struct NumPunctDerived: std::numpunct<charT>
{
    typedef std::numpunct<charT>                     NumPunct;
    typedef typename StringTypes<charT>::string_type StringT;

// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#undef MEMFUN
#define MEMFUN(result, name, arg_list)                          \
        do {                                                    \
           result (NumPunct::*pf) arg_list = &NumPunct::name;   \
           _RWSTD_UNUSED (pf);                                  \
         }  while (0)

    static void test_protected_members () {
        // protected virtuals
        MEMFUN (charT, do_decimal_point, () const);
        MEMFUN (charT, do_thousands_sep, () const);
        MEMFUN (std::string, do_grouping, () const);
        MEMFUN (StringT, do_truename, () const);
        MEMFUN (StringT, do_falsename, () const);
    }
};


template <class charT>
static void
test_numpunct (const char* cname)
{
    rw_info (0, __FILE__, __LINE__,
             "std::numpunct<%s> synopsis", cname);

    typedef std::numpunct<charT>                     NumPunct;
    typedef typename StringTypes<charT>::string_type StringT;

    // verify nested types
    typename NumPunct::char_type   *pc   = (charT*)0;
    typename NumPunct::string_type *pstr = (StringT*)0;

    _RWSTD_UNUSED (pc);
    _RWSTD_UNUSED (pstr);

    // public interface
    MEMFUN (charT, decimal_point, () const);
    MEMFUN (charT, thousands_sep, () const);
    MEMFUN (std::string, grouping, () const);
    MEMFUN (StringT, truename, () const);
    MEMFUN (StringT, falsename, () const);

    // verify that id is defined and non-const
    std::locale::id *pid = &NumPunct::id;
    _RWSTD_UNUSED (pid);

    NumPunctDerived<charT>::test_protected_members ();
}


typedef std::ios_base          IosBase;
typedef std::ios_base::iostate IoState;


template <class charT, class IterT>
struct NumGetDerived: std::num_get<charT, IterT>
{
    typedef std::num_get<charT, IterT> NumGet;

// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#undef MEMFUN
#define MEMFUN(result, name, arg_list)                      \
        do {                                                \
           result (NumGet::*pf) arg_list = &NumGet::name;   \
           _RWSTD_UNUSED (pf);                              \
         }  while (0)

    static void test_protected_members () {
        // protected virtuals
        MEMFUN (IterT, do_get,
                (IterT, IterT, IosBase&, IoState&, bool&) const);
        MEMFUN (IterT, do_get,
                (IterT, IterT, IosBase& ,IoState&, long&) const);
        MEMFUN (IterT, do_get,
                (IterT, IterT, IosBase&, IoState&, unsigned short&) const);
        MEMFUN (IterT, do_get,
                (IterT, IterT, IosBase&, IoState&, unsigned&) const);
        MEMFUN (IterT, do_get,
                (IterT, IterT, IosBase&, IoState&, unsigned long&) const);

#ifdef _RWSTD_LONG_LONG

        // extensions disabled in strict ANSI mode
        MEMFUN (IterT, do_get,
                (IterT, IterT, IosBase&, IoState&, _RWSTD_LONG_LONG&) const);
        MEMFUN (IterT, do_get,
                (IterT, IterT, IosBase&, IoState&,
                 unsigned _RWSTD_LONG_LONG&) const);

#endif   // _RWSTD_LONG_LONG

        MEMFUN (IterT, do_get,
                (IterT, IterT, IosBase&, IoState&, float&) const);
        MEMFUN (IterT, do_get,
                (IterT, IterT, IosBase&, IoState&, double&) const);
        MEMFUN (IterT, do_get,
                (IterT, IterT, IosBase&, IoState&, long double&) const);
        MEMFUN (IterT, do_get,
                (IterT, IterT, IosBase&, IoState&, void*&) const);
    }
};


template <class charT, class IterT>
static void
test_num_get (const char* cname, const char* iname)
{
    rw_info (0, __FILE__, __LINE__,
             "std::num_get<%s, %s > synopsis", cname, iname);

    typedef std::num_get<charT, IterT> NumGet;

    // verify nested types
    typename NumGet::char_type *pc = (charT*)0;
    typename NumGet::iter_type *pi = (IterT*)0;

    _RWSTD_UNUSED (pc);
    _RWSTD_UNUSED (pi);

    // public interface
    MEMFUN (IterT, get, (IterT, IterT, IosBase&, IoState&, bool&) const);
    MEMFUN (IterT, get, (IterT, IterT, IosBase&, IoState&, long&) const);
    MEMFUN (IterT, get,
            (IterT, IterT, IosBase&, IoState&, unsigned short&) const);
    MEMFUN (IterT, get,
            (IterT, IterT, IosBase&, IoState&, unsigned&) const);
    MEMFUN (IterT, get,
            (IterT, IterT, IosBase&, IoState&, unsigned long&) const);

#ifdef _RWSTD_LONG_LONG

    // extensions disabled in strict ANSI mode
    MEMFUN (IterT, get,
            (IterT, IterT, IosBase&, IoState&, _RWSTD_LONG_LONG&) const);
    MEMFUN (IterT, get,
            (IterT, IterT, IosBase&, IoState&,
             unsigned _RWSTD_LONG_LONG&) const);

#endif   // _RWSTD_LONG_LONG

    MEMFUN (IterT, get,
            (IterT, IterT, IosBase&, IoState&, float&) const);
    MEMFUN (IterT, get,
            (IterT, IterT, IosBase&, IoState&, double&) const);
    MEMFUN (IterT, get,
            (IterT, IterT, IosBase&, IoState&, long double&) const);
    MEMFUN (IterT, get,
            (IterT, IterT, IosBase&, IoState&, void*&) const);

    std::locale::id *pid = &NumGet::id;
    _RWSTD_UNUSED (pid);

    NumGetDerived<charT, IterT>::test_protected_members ();
}


template <class charT, class IterT>
struct NumPutDerived: std::num_put<charT, IterT>
{
    typedef std::num_put<charT, IterT> NumPut;

// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#undef MEMFUN
#define MEMFUN(result, name, arg_list)                      \
        do {                                                \
           result (NumPut::*pf) arg_list = &NumPut::name;   \
           _RWSTD_UNUSED (pf);                              \
         }  while (0)

    static void test_protected_members () {
        // protected virtuals
        MEMFUN (IterT, do_put, (IterT, IosBase&, charT, bool) const);
        MEMFUN (IterT, do_put, (IterT, IosBase&, charT, long) const);

        // no overloads for unsigned short or unsigned int
        MEMFUN (IterT, do_put, (IterT, IosBase&, charT, unsigned long) const);

#ifdef _RWSTD_LONG_LONG

        // extensions disabled when _RWSTD_NO_LONG_LONG is #defined
        MEMFUN (IterT, do_put,
                (IterT, IosBase&, charT, _RWSTD_LONG_LONG) const);
        MEMFUN (IterT, do_put,
                (IterT, IosBase&, charT, unsigned _RWSTD_LONG_LONG) const);

#endif   // _RWSTD_LONG_LONG

        // no float overload
        MEMFUN (IterT, do_put, (IterT, IosBase&, charT, double) const);
        MEMFUN (IterT, do_put, (IterT, IosBase&, charT, long double) const);
        MEMFUN (IterT, do_put, (IterT, IosBase&, charT, const void*) const);
    }
};


template <class charT, class IterT>
static void
test_num_put (const char* cname, const char* iname)
{
    rw_info (0, __FILE__, __LINE__,
             "std::num_put<%s, %s > synopsis", cname, iname);

    typedef std::num_put<charT, IterT> NumPut;

    // verify nested types
    typename NumPut::char_type *pc = (charT*)0;
    typename NumPut::iter_type *pi = (IterT*)0;

    _RWSTD_UNUSED (pc);
    _RWSTD_UNUSED (pi);

    // public interface
    MEMFUN (IterT, put, (IterT, IosBase&, charT, bool) const);
    MEMFUN (IterT, put, (IterT, IosBase&, charT, long) const);
    MEMFUN (IterT, put, (IterT, IosBase&, charT, unsigned long) const);

#ifdef _RWSTD_LONG_LONG

    // extensions disabled in strict ANSI mode
    MEMFUN (IterT, put,
            (IterT, IosBase&, charT, _RWSTD_LONG_LONG) const);
    MEMFUN (IterT, put,
            (IterT, IosBase&, charT, unsigned _RWSTD_LONG_LONG) const);

#endif   // _RWSTD_LONG_LONG

    MEMFUN (IterT, put, (IterT, IosBase&, charT, double) const);
    MEMFUN (IterT, put, (IterT, IosBase&, charT, long double) const);
    MEMFUN (IterT, put, (IterT, IosBase&, charT, const void*) const);

    std::locale::id *pid = &NumPut::id;
    _RWSTD_UNUSED (pid);

    NumPutDerived<charT, IterT>::test_protected_members ();
}

/***************************************************************************/

// collate category

template <class charT>
struct CollateDerived: std::collate<charT>
{
    typedef const charT*                             PtrT;
    typedef std::collate<charT>                      Collate;
    typedef typename StringTypes<charT>::string_type StringT;

// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#undef MEMFUN
#define MEMFUN(result, name, arg_list)                         \
        do {                                                   \
           result (Collate::* pf) arg_list = &Collate::name;   \
           _RWSTD_UNUSED (pf);                                 \
         }  while (0)

    static void test_protected_members () {

        MEMFUN (int, compare, (PtrT, PtrT, PtrT, PtrT) const);

        MEMFUN (StringT, transform, (PtrT, PtrT) const);
        MEMFUN (long, hash, (PtrT, PtrT) const);

        std::locale::id *pid = &Collate::id;
        _RWSTD_UNUSED (pid);

        // protected virtuals (see `#define protected public' above)
        MEMFUN (int, do_compare, (PtrT, PtrT, PtrT, PtrT) const);

        MEMFUN (StringT, do_transform, (PtrT, PtrT) const);
        MEMFUN (long, do_hash, (PtrT, PtrT) const);
    }
};


template <class charT>
static void
test_collate (const char* cname)
{
    rw_info (0, __FILE__, __LINE__,
             "std::collate<%s> synopsis", cname);

    typedef std::collate<charT> Collate;
    typedef typename StringTypes<charT>::string_type StringT;

    // verify nested types
    typename Collate::char_type   *pc   = (charT*)0;
    typename Collate::string_type *pstr = (StringT*)0;

    _RWSTD_UNUSED (pc);
    _RWSTD_UNUSED (pstr);

    CollateDerived<charT>::test_protected_members ();
}

/***************************************************************************/

// time category

template <class charT, class IterT>
struct TimeGetDerived: std::time_get<charT, IterT>
{
    typedef std::time_get<charT, IterT> TimeGet;

// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#undef MEMFUN
#define MEMFUN(result, name, arg_list)                         \
        do {                                                   \
           result (TimeGet::* pf) arg_list = &TimeGet::name;   \
           _RWSTD_UNUSED (pf);                                 \
         }  while (0)

    static void test_protected_members () {
        // protected virtuals
        MEMFUN (std::time_base::dateorder, do_date_order, () const);
        MEMFUN (IterT, do_get_time,
                (IterT, IterT, IosBase&, IoState&, std::tm*) const);
        MEMFUN (IterT, do_get_date,
                (IterT, IterT, IosBase&, IoState&, std::tm*) const);
        MEMFUN (IterT, do_get_weekday,
                (IterT, IterT, IosBase&, IoState&, std::tm*) const);
        MEMFUN (IterT, do_get_monthname,
                (IterT, IterT, IosBase&, IoState&, std::tm*) const);
        MEMFUN (IterT, do_get_year,
                (IterT, IterT, IosBase&, IoState&, std::tm*) const);
    }
};


template <class charT, class IterT>
static void
test_time_get (const char* cname, const char* iname)
{
    rw_info (0, __FILE__, __LINE__,
             "std::time_get<%s, %s > synopsis", cname, iname);

    typedef std::time_get<charT, IterT> TimeGet;

    // verify that dateorder constants are constant integral expressions
    std::time_base::dateorder ord = std::time_base::no_order;
    switch (ord) {
    case std::time_base::no_order:
    case std::time_base::dmy: case std::time_base::mdy:
    case std::time_base::ymd: case std::time_base::ydm:
        break;
    }

    // verify values
    rw_assert (0 == std::time_base::no_order, __FILE__, __LINE__,
               "std::time_base::no_order");
    rw_assert (1 == std::time_base::dmy, __FILE__, __LINE__,
               "std::time_base::dmy");
    rw_assert (2 == std::time_base::mdy, __FILE__, __LINE__,
               "std::time_base::mdy");
    rw_assert (3 == std::time_base::ymd, __FILE__, __LINE__,
               "std::time_base::ymd");
    rw_assert (4 == std::time_base::ydm, __FILE__, __LINE__,
               "std::time_base::ydm");

    // verify nested types
    typename TimeGet::char_type *pc = (charT*)0;
    typename TimeGet::iter_type *pi = (IterT*)0;

    _RWSTD_UNUSED (pc);
    _RWSTD_UNUSED (pi);

    // public non-virtuals
    MEMFUN (std::time_base::dateorder, date_order, () const);
    MEMFUN (IterT, get_time,
            (IterT, IterT, IosBase&, IoState&, std::tm*) const);
    MEMFUN (IterT, get_date,
            (IterT, IterT, IosBase&, IoState&, std::tm*) const);
    MEMFUN (IterT, get_weekday,
            (IterT, IterT, IosBase&, IoState&, std::tm*) const);
    MEMFUN (IterT, get_monthname,
            (IterT, IterT, IosBase&, IoState&, std::tm*) const);
    MEMFUN (IterT, get_year,
            (IterT, IterT, IosBase&, IoState&, std::tm*) const);

    std::locale::id *pid = &TimeGet::id;
    _RWSTD_UNUSED (pid);

    TimeGetDerived<charT, IterT>::test_protected_members ();
}


template <class charT, class IterT>
struct TimePutDerived: std::time_put<charT, IterT>
{
    typedef std::time_put<charT, IterT> TimePut;

// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#undef MEMFUN
#define MEMFUN(result, name, arg_list)                        \
        do {                                                  \
           result (TimePut::*pf) arg_list = &TimePut::name;   \
           _RWSTD_UNUSED (pf);                                \
         }  while (0)

    static void test_protected_members () {
        // protected virtuals
        MEMFUN (IterT, do_put, (IterT, IosBase&, charT, const std::tm*,
                                char, char) const);
    }
};


template <class charT, class IterT>
static void
test_time_put (const char* cname, const char* iname)
{
    rw_info (0, __FILE__, __LINE__,
             "std::time_put<%s, %s > synopsis", cname, iname);

    typedef std::time_put<charT, IterT> TimePut;

    // verify nested types
    typename TimePut::char_type *pc = (charT*)0;
    typename TimePut::iter_type *pi = (IterT*)0;

    _RWSTD_UNUSED (pc);
    _RWSTD_UNUSED (pi);

    // public non-virtuals
    MEMFUN (IterT, put, (IterT, IosBase&, charT, const std::tm*,
                          const charT*, const charT*) const);
    MEMFUN (IterT, put, (IterT, IosBase&, charT, const std::tm*,
                         char, char) const);

    std::locale::id *pid = &TimePut::id;
    _RWSTD_UNUSED (pid);

    TimePutDerived<charT, IterT>::test_protected_members ();
}

/***************************************************************************/

// monetary category

namespace std {

// (in the absence of support for partial specialization) provide explicit
// specializations of moneypunct on UDC necessary in order to exercise
// money_get<UDC> and money_put<UDC>

_RWSTD_SPECIALIZED_CLASS
struct moneypunct<UDC, false>: locale::facet, money_base
{
    typedef UDC                                 char_type;
    typedef StringTypes<char_type>::string_type string_type;

    // working around an MSVC 6.0 bug
    typedef locale::facet           Base;

    explicit moneypunct (size_t refs = 0)
        : Base (refs), money_base () { /* empty */ }

    static locale::id id;

    enum { intl = false };

    char_type decimal_point () const { return char_type (); }
    char_type thousands_sep () const { return char_type (); }
    string grouping () const { return ""; }
    string_type curr_symbol () const { return string_type (); }
    string_type positive_sign () const { return string_type (); }
    string_type negative_sign () const { return string_type (); }
    int frac_digits () const { return 0; }
    pattern pos_format () const { return pattern (); }
    pattern neg_format () const { return pattern (); }
};

locale::id moneypunct<UDC, false>::id;


_RWSTD_SPECIALIZED_CLASS
struct moneypunct<UDC, true>: locale::facet, money_base
{
    typedef UDC                                 char_type;
    typedef StringTypes<char_type>::string_type string_type;

    // working around an MSVC 6.0 bug
    typedef locale::facet           Base;

    explicit moneypunct (size_t refs = 0)
        : Base (refs), money_base () { /* empty */ }

    static locale::id id;

    enum { intl = true };

    char_type decimal_point () const { return char_type (); }
    char_type thousands_sep () const { return char_type (); }
    string grouping () const { return ""; }
    string_type curr_symbol () const { return string_type (); }
    string_type positive_sign () const { return string_type (); }
    string_type negative_sign () const { return string_type (); }
    int frac_digits () const { return 0; }
    pattern pos_format () const { return pattern (); }
    pattern neg_format () const { return pattern (); }
};

locale::id moneypunct<UDC, true>::id;

}   // namespace std


template <class charT, bool Intl>
struct MoneyPunctDerived: std::moneypunct<charT, Intl>
{
    typedef std::moneypunct<charT, Intl>             MoneyPunct;
    typedef typename StringTypes<charT>::string_type StringT;

// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#undef MEMFUN
#define MEMFUN(result, name, arg_list)                               \
        do {                                                         \
           result (MoneyPunct::* pf) arg_list = &MoneyPunct::name;   \
           _RWSTD_UNUSED (pf);                                       \
         }  while (0)

    static void test_protected_members () {

        // protected virtuals
        MEMFUN (charT, do_decimal_point, () const);
        MEMFUN (charT, do_thousands_sep, () const );
        MEMFUN (std::string, do_grouping, () const );
        MEMFUN (StringT, do_curr_symbol, () const);
        MEMFUN (StringT, do_positive_sign, () const);
        MEMFUN (StringT, do_negative_sign, () const);
        MEMFUN (int, do_frac_digits, () const);
        MEMFUN (std::money_base::pattern, do_pos_format, () const);
        MEMFUN (std::money_base::pattern, do_neg_format, () const);
    }
};


template <class charT>
static void
test_moneypunct (const char* cname)
{
    rw_info (0, __FILE__, __LINE__,
             "std::moneypunct<%s, bool> synopsis", cname);

    typedef std::moneypunct<charT, false>            MoneyPunct;
    typedef typename StringTypes<charT>::string_type StringT;

    typename MoneyPunct::char_type *pc = (charT*)0;
    typename MoneyPunct::string_type *ps = (StringT*)0;

    _RWSTD_UNUSED (pc);
    _RWSTD_UNUSED (ps);

    MEMFUN (charT, decimal_point, () const );
    MEMFUN (charT, thousands_sep, () const );
    MEMFUN (std::string, grouping, () const );
    MEMFUN (StringT, curr_symbol, () const);
    MEMFUN (StringT, positive_sign, () const);
    MEMFUN (StringT, negative_sign, () const);
    MEMFUN (int, frac_digits, () const);
    MEMFUN (std::money_base::pattern, pos_format, () const);
    MEMFUN (std::money_base::pattern, neg_format, () const);

    std::locale::id *pid = &MoneyPunct::id;
    _RWSTD_UNUSED (pid);

    MoneyPunctDerived<charT, false>::test_protected_members ();
}


template <class charT, class IterT>
struct MoneyGetDerived: std::money_get<charT, IterT>
{
    typedef std::money_get<charT, IterT>             MoneyGet;
    typedef typename StringTypes<charT>::string_type StringT;

// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#undef MEMFUN
#define MEMFUN(result, name, arg_list)                          \
        do {                                                    \
           result (MoneyGet::*pf) arg_list = &MoneyGet::name;   \
           _RWSTD_UNUSED (pf);                                  \
         }  while (0)

    static void test_protected_members () {
        // protected virtuals

#ifndef _RWSTD_NO_LONG_DOUBLE

        MEMFUN (IterT, do_get, (IterT, IterT, bool, IosBase&,
                                IoState&, long double&) const);

#else   // if defined _RWSTD_NO_LONG_DOUBLE

        MEMFUN (IterT, do_get, (IterT, IterT, bool, IosBase&,
                                IoState&, double&) const);

#endif   // _RWSTD_NO_LONG_DOUBLE

        MEMFUN (IterT, do_get, (IterT, IterT, bool, IosBase&,
                                IoState&, StringT&) const);
    }
};


template <class charT, class IterT>
static void
test_money_get (const char* cname, const char* iname)
{
    rw_info (0, __FILE__, __LINE__,
             "std::money_get<%s, %s > synopsis", cname, iname);

    typedef std::money_get<charT, IterT>             MoneyGet;
    typedef typename StringTypes<charT>::string_type StringT;

    // verify nested types
    typename MoneyGet::char_type *pc = (charT*)0;
    typename MoneyGet::iter_type *pi = (IterT*)0;
    typename MoneyGet::string_type *ps = (StringT*)0;

    _RWSTD_UNUSED (pc);
    _RWSTD_UNUSED (pi);
    _RWSTD_UNUSED (ps);

    // public interface
    MEMFUN (IterT, get, (IterT, IterT, bool, IosBase&,
                        IoState&, long double&) const);
    MEMFUN (IterT, get, (IterT, IterT, bool, IosBase&,
                        IoState&, StringT&) const);

    std::locale::id *pid = &MoneyGet::id;
    _RWSTD_UNUSED (pid);

    MoneyGetDerived<charT, IterT>::test_protected_members ();
}


template <class charT, class IterT>
struct MoneyPutDerived: std::money_put<charT, IterT>
{
    typedef std::money_put<charT, IterT>             MoneyPut;
    typedef typename StringTypes<charT>::string_type StringT;

// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#undef MEMFUN
#define MEMFUN(result, name, arg_list)                          \
        do {                                                    \
           result (MoneyPut::*pf) arg_list = &MoneyPut::name;   \
           _RWSTD_UNUSED (pf);                                  \
         }  while (0)

    static void test_protected_members () {
            // protected virtuals

#ifndef _RWSTD_NO_LONG_DOUBLE

        MEMFUN (IterT, do_put, (IterT, bool, IosBase&, charT,
                                long double) const);

#else   // if defined _RWSTD_NO_LONG_DOUBLE

        MEMFUN (IterT, do_put, (IterT, bool, IosBase&, charT, double) const);

#endif   // _RWSTD_NO_LONG_DOUBLE

        MEMFUN (IterT, do_put, (IterT, bool, IosBase&, charT,
                                const StringT&) const);
    }
};


template <class charT, class IterT>
static void
test_money_put (const char* cname, const char* iname)
{
    rw_info (0, __FILE__, __LINE__,
             "std::money_put<%s, %s > synopsis", cname, iname);

    typedef std::money_put<charT, IterT>             MoneyPut;
    typedef typename StringTypes<charT>::string_type StringT;

     //verify nested types
    typename MoneyPut::char_type *pc = (charT*)0;
    typename MoneyPut::iter_type *pi = (IterT*)0;
    typename MoneyPut::string_type *ps = (StringT*)0;

    _RWSTD_UNUSED (pc);
    _RWSTD_UNUSED (pi);
    _RWSTD_UNUSED (ps);

    // public interface
    MEMFUN (IterT, put, (IterT, bool, IosBase&, charT,
                        long double) const);
    MEMFUN (IterT, put, (IterT, bool, IosBase&, charT,
                        const StringT&) const);

    std::locale::id *pid = &MoneyPut::id;
    _RWSTD_UNUSED (pid);

    MoneyPutDerived<charT, IterT>::test_protected_members ();
}

/***************************************************************************/

// messges category

template <class charT>
struct MessagesDerived: std::messages<charT>
{
    typedef std::messages<charT>                     Messages;
    typedef typename StringTypes<charT>::string_type StringT;

// verify that a member function is accessible and has the appropriate
// signature, including return type and exception specification
#undef MEMFUN
#define MEMFUN(result, name, arg_list)                           \
        do {                                                     \
           result (Messages::* pf) arg_list = &Messages::name;   \
           _RWSTD_UNUSED (pf);                                   \
         }  while (0)


    static void test_protected_members () {
        // protected virtuals
        MEMFUN (int,  do_open,
                (const std::string&, const std::locale&) const);
        MEMFUN (StringT, do_get, (int, int, int, const StringT&) const);
        MEMFUN (void, do_close, (int) const);
    }
};


template <class charT>
static void
test_messages (const char* cname)
{
    rw_info (0, __FILE__, __LINE__,
             "std::messages<%s> synopsis", cname);

    typedef std::messages<charT>                     Messages;
    typedef typename StringTypes<charT>::string_type StringT;

    // verify nested types
    std::messages_base::catalog    *pcat = (int*)0;
    typename Messages::char_type   *pc   = (charT*)0;
    typename Messages::string_type *pstr = (StringT*)0;

    _RWSTD_UNUSED (pcat);
    _RWSTD_UNUSED (pc);
    _RWSTD_UNUSED (pstr);

    // public interface
    MEMFUN (int,  open, (const std::string&, const std::locale&) const);
    MEMFUN (StringT, get, (int, int, int, const StringT&) const);
    MEMFUN (void, close, (int) const);

    std::locale::id *pid = &Messages::id;
    _RWSTD_UNUSED (pid);

    MessagesDerived<charT>::test_protected_members ();
}

/***************************************************************************/

static int
run_test (int /*unused*/, char* /*unused*/ [])
{
    test_lc_defs ();
    test_isxxx ();

    // exercise the signatures of std::{has,use}_facet<>()
    test_have_use_facet<char> ();

    // exercise the signatures of the convenience locale interfaces
    test_convenience_funs<char> ();

    // test for the presence of the required facet templates
    test_facets<char> ();

    test_locale ();

// don't use _RWSTD_STRSTR so type macros (e.g. UDC) are not expanded
#undef FUN1
#define FUN1(fun, charT) \
        fun< charT > (#charT)

    // std::locale::collate category
    FUN1 (test_collate, char);

    // std::locale::ctype category
    FUN1 (test_codecvt, char);
    FUN1 (test_ctype, char);

#undef FUN2
#define FUN2(fun, charT, IterT) \
        fun< charT, IterT > (#charT, #IterT)

    // std::locale::monetary category
    FUN1 (test_moneypunct, char);
    FUN2 (test_money_get, char, std::istreambuf_iterator<char>);
    FUN2 (test_money_get, char, const char*);
    FUN2 (test_money_put, char, std::ostreambuf_iterator<char>);
    FUN2 (test_money_put, char, char*);

    // std::locale::numeric category
    FUN1 (test_numpunct, char);
    FUN2 (test_num_get, char, std::istreambuf_iterator<char>);
    FUN2 (test_num_get, char, const char*);
    FUN2 (test_num_put, char, std::ostreambuf_iterator<char>);
    FUN2 (test_num_put, char, char*);

    // std::locale::time category
    FUN2 (test_time_get, char, std::istreambuf_iterator<char>);
    FUN2 (test_time_get, char, const char*);
    FUN2 (test_time_put, char, std::ostreambuf_iterator<char>);
    FUN2 (test_time_put, char, char*);

    // std::locale::messages category
    FUN1 (test_messages, char);

#ifndef _RWSTD_NO_WCHAR_T

    // same as above but for wchar_t
    test_have_use_facet<wchar_t> ();
    test_convenience_funs<wchar_t> ();
    test_facets<wchar_t> ();

    FUN1 (test_collate, wchar_t);

    FUN1 (test_ctype, wchar_t);
    FUN1 (test_codecvt, wchar_t);

    FUN1 (test_moneypunct, wchar_t);
    FUN2 (test_money_get, wchar_t, std::istreambuf_iterator<wchar_t>);
    FUN2 (test_money_get, wchar_t, const wchar_t*);
    FUN2 (test_money_put, wchar_t, std::ostreambuf_iterator<wchar_t>);
    FUN2 (test_money_put, wchar_t, wchar_t*);

    FUN1 (test_numpunct, wchar_t);
    FUN2 (test_num_get, wchar_t, std::istreambuf_iterator<wchar_t>);
    FUN2 (test_num_get, wchar_t, const wchar_t*);
    FUN2 (test_num_put, wchar_t, std::ostreambuf_iterator<wchar_t>);
    FUN2 (test_num_put, wchar_t, wchar_t*);

    FUN2 (test_time_get, wchar_t, std::istreambuf_iterator<wchar_t>);
    FUN2 (test_time_get, wchar_t, const wchar_t*);
    FUN2 (test_time_put, wchar_t, std::ostreambuf_iterator<wchar_t>);
    FUN2 (test_time_put, wchar_t, wchar_t*);

    FUN1 (test_messages, wchar_t);

#endif   // _RWSTD_NO_WCHAR_T

    // test facet specializations on character types other than char and wchar_t
    // and iterator types other than istreambuf_iterator and ostreambuf_iterator
    // as per requirements in Table 52

    FUN2 (test_money_get, UDC, std::istreambuf_iterator<UDC>);
    FUN2 (test_money_get, UDC, const UDC*);

    FUN2 (test_money_put, UDC, std::ostreambuf_iterator<UDC>);
    FUN2 (test_money_put, UDC, UDC*);

    FUN2 (test_num_get, UDC, std::istreambuf_iterator<UDC>);
    FUN2 (test_num_get, UDC, const UDC*);

    FUN2 (test_num_put, UDC, std::ostreambuf_iterator<UDC>);
    FUN2 (test_num_put, UDC, UDC*);

    FUN2 (test_time_get, char, const char*);
    FUN2 (test_time_put, char, char*);

    return 0;
}

/* extern */ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.locales", "header <locale> synopsis",
                    run_test, "|-enable-posix# ", &opt_enable_posix);
}

