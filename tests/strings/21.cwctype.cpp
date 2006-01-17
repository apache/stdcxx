/***************************************************************************
 *
 * 21.cwctype.cpp - test exercising clause lib.string.c.strings,
 *                  header <cwctype>
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

#include <cwctype>
#include <driver.h>

/**************************************************************************/

// detect masking macros
const char* const cwctype_macros [] = {

#ifdef iswalnum
    "iswalnum",
#else
    "",
#endif

#ifdef iswalpha
    "iswalpha",
#else
    "",
#endif

#ifdef iswcntrl
    "iswcntrl",
#else
    "",
#endif

#ifdef iswctype
    "iswctype",
#else
    "",
#endif

#ifdef iswdigit
    "iswdigit",
#else
    "",
#endif

#ifdef iswgraph
    "iswgraph",
#else
    "",
#endif

#ifdef iswlower
    "iswlower",
#else
    "",

#ifdef iswprint
    "iswprint",
#else
    "",
#endif

#ifdef iswpunct
    "iswpunct",
#else
    "",
#endif

#ifdef iswspace
    "iswspace",
#else
    "",
#endif

#ifdef iswupper
    "iswupper",
#else
    "",
#endif

#ifdef iswxdigit
    "iswxdigit",
#else
    "",
#endif

#endif

#ifdef tolower
    "tolower",
#else
    "",
#endif

#ifdef toupper
    "toupper",
#else
    "",
#endif

#ifdef towctrans
    "towctrans",
#else
    "",
#endif

#ifdef wctrans
    "wctrans",
#else
    "",
#endif

#ifdef wctype
    "wctype",
#else
    "",
#endif

    0
};

/**************************************************************************/

#ifndef _RWSTD_NO_NAMESPACE

// check types

namespace Fallback {

struct wint_t {
    int i_;
    char dummy_ [256];   // make sure we're bigger than the real thing

    // this (fake) wint_t emulates a scalar type
    wint_t (int i): i_ (i) { }
    operator int () const { return i_; }
};

struct wctype_t {
    int i_;
    char dummy_ [256];   // make sure we're bigger than the real thing

    // this (fake) wctype_t emulates a scalar type
    wctype_t (int i): i_ (i) { }
    operator int () const { return i_; }
};

struct wctrans_t {
    int i_;
    char dummy_ [256];   // make sure we're bigger than the real thing

    // this (fake) wctrans_t emulates a scalar type
    wctrans_t (int i): i_ (i) { }
    operator int () const { return i_; }
};

}    // Namespace Fallback

namespace std {

// define test functions in namespace std to detect the presece
// or absence of the required types

namespace Nested {

using namespace Fallback;

// each test_xxx_t typedef aliases std::xxx_t if the corresponding
// type is defined in namespace std, or Fallback::xxx_t otherwise
typedef wint_t    test_wint_t;
typedef wctrans_t test_wctrans_t;
typedef wctype_t  test_wctype_t;

}   // namespace Nested

}   // namespace std

typedef std::Nested::test_wint_t    test_wint_t;
typedef std::Nested::test_wctrans_t test_wctrans_t;
typedef std::Nested::test_wctype_t  test_wctype_t;

const char std_name[] = "std";

static void
test_types ()
{
    rw_info (0, 0, 0,
             "types %s::wint_t, %1$s::wctrans_t, and %1$s::wctype_t",
             std_name);

    rw_assert (sizeof (test_wint_t) != sizeof (Fallback::wint_t), 0, 0,
               "%s::wint_t not defined", std_name);

    // TO DO: exercise wint_t requirements

    rw_assert (sizeof (test_wctrans_t) != sizeof (Fallback::wctrans_t), 0, 0,
               "%s::wctrans_t not defined", std_name);

    // TO DO: exercise wctrans_t requirements (must be a scalar type)

    rw_assert (sizeof (test_wctype_t) != sizeof (Fallback::wctype_t), 0, 0,
               "%s::wctype_t not defined", std_name);

    // TO DO: exercise wctype_t requirements (must be a scalar type)
}

#else   // if defined (_RWSTD_NO_NAMESPACE)

const char std_name[] = "";

static void
test_types ()
{
    rw_info (0, 0, 0,
             "types %s::wint_t, %1s::wctrans_t, and %1s::wctype_t", std_name);

    rw_note (0, 0, 0, "_RWSTD_NO_NAMESPACE defined, cannot test");
}

#endif   // _RWSTD_NO_NAMESPACE

/**************************************************************************/

// define function template overloads for the <cctype> functions with
// the same name to detect whether the C versions are declared or not
#define TEST_FUNCTION(name)                                             \
    template <class T> int name (T) { function_called = 0; return 0; }  \
    typedef void rw_unused_typedef

int function_called;

_RWSTD_NAMESPACE (std) {

TEST_FUNCTION (iswalnum);
TEST_FUNCTION (iswalpha);
TEST_FUNCTION (iswcntrl);
TEST_FUNCTION (iswdigit);
TEST_FUNCTION (iswgraph);
TEST_FUNCTION (iswlower);
TEST_FUNCTION (iswprint);
TEST_FUNCTION (iswpunct);
TEST_FUNCTION (iswspace);
TEST_FUNCTION (iswupper);
TEST_FUNCTION (iswxdigit);
TEST_FUNCTION (towlower);
TEST_FUNCTION (towupper);
TEST_FUNCTION (wctype);
TEST_FUNCTION (wctrans);

template <class T, class U>
int iswctype (T, U) { function_called = 0; return 0; }

template <class T, class U>
int towctrans (T, U) { function_called = 0; return 0; }

}   // namespace std

/**************************************************************************/

enum {
    bit_alnum  = 1,
    bit_alpha  = 1 << 1,
    bit_cntrl  = 1 << 2,
    bit_digit  = 1 << 3,
    bit_graph  = 1 << 4,
    bit_lower  = 1 << 5,
    bit_print  = 1 << 6,
    bit_punct  = 1 << 7,
    bit_space  = 1 << 8,
    bit_upper  = 1 << 9,
    bit_xdigit = 1 << 10
};


static char*
get_bitmask (int mask, char *str)
{
    char *str_save = str;

#define APPEND(bit)                                     \
    if (mask & bit_ ## bit) {                           \
        for (const char *s = #bit; *s; ++s, ++str)      \
            *str = *s;                                  \
        *str++ = '|';                                   \
    } else (void)0

    APPEND (alnum);
    APPEND (alpha);
    APPEND (cntrl);
    APPEND (digit);
    APPEND (graph);
    APPEND (lower);
    APPEND (print);
    APPEND (punct);
    APPEND (space);
    APPEND (upper);
    APPEND (xdigit);

    if (str == str_save)
        *str = '\0';
    else
        str [-1] = '\0';

    return str_save;
}


static const int
char_mask [256] = {

#undef  LETTER
#define LETTER(bits) bit_alpha | bit_alnum | bit_print | bit_graph | bits

#if 'A' == 0x41

    /* 0x00 */ bit_cntrl,
    /* 0x01 */ bit_cntrl,
    /* 0x02 */ bit_cntrl,
    /* 0x03 */ bit_cntrl,
    /* 0x04 */ bit_cntrl,
    /* 0x05 */ bit_cntrl,
    /* 0x06 */ bit_cntrl,
    /* 0x07 */ bit_cntrl,
    /* 0x08 */ bit_cntrl,
    /* 0x09 */ bit_space | bit_cntrl,
    /* 0x0a */ bit_space | bit_cntrl,
    /* 0x0b */ bit_space | bit_cntrl,
    /* 0x0c */ bit_space | bit_cntrl,
    /* 0x0d */ bit_space | bit_cntrl,
    /* 0x0e */ bit_cntrl,
    /* 0x0f */ bit_cntrl,
    /* 0x10 */ bit_cntrl,
    /* 0x11 */ bit_cntrl,
    /* 0x12 */ bit_cntrl,
    /* 0x13 */ bit_cntrl,
    /* 0x14 */ bit_cntrl,
    /* 0x15 */ bit_cntrl,
    /* 0x16 */ bit_cntrl,
    /* 0x17 */ bit_cntrl,
    /* 0x18 */ bit_cntrl,
    /* 0x19 */ bit_cntrl,
    /* 0x1a */ bit_cntrl,
    /* 0x1b */ bit_cntrl,
    /* 0x1c */ bit_cntrl,
    /* 0x1d */ bit_cntrl,
    /* 0x1e */ bit_cntrl,
    /* 0x1f */ bit_cntrl,
    /* ' '  */ bit_space | bit_print,
    /* '!'  */ bit_print | bit_punct | bit_graph,
    /* '"'  */ bit_print | bit_punct | bit_graph,
    /* '#'  */ bit_print | bit_punct | bit_graph,
    /* '$'  */ bit_print | bit_punct | bit_graph,
    /* '%'  */ bit_print | bit_punct | bit_graph,
    /* '&'  */ bit_print | bit_punct | bit_graph,
    /* '\'' */ bit_print | bit_punct | bit_graph,
    /* '('  */ bit_print | bit_punct | bit_graph,
    /* ')'  */ bit_print | bit_punct | bit_graph,
    /* '*'  */ bit_print | bit_punct | bit_graph,
    /* '+'  */ bit_print | bit_punct | bit_graph,
    /* ','  */ bit_print | bit_punct | bit_graph,
    /* '-'  */ bit_print | bit_punct | bit_graph,
    /* '.'  */ bit_print | bit_punct | bit_graph,
    /* '/'  */ bit_print | bit_punct | bit_graph,
    /* '0'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '1'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '2'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '3'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '4'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '5'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '6'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '7'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '8'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '9'  */ bit_alnum | bit_print | bit_digit | bit_graph | bit_xdigit,
    /* ':'  */ bit_print | bit_punct | bit_graph,
    /* ';'  */ bit_print | bit_punct | bit_graph,
    /* '<'  */ bit_print | bit_punct | bit_graph,
    /* '='  */ bit_print | bit_punct | bit_graph,
    /* '>'  */ bit_print | bit_punct | bit_graph,
    /* '?'  */ bit_print | bit_punct | bit_graph,
    /* '@'  */ bit_print | bit_punct | bit_graph,

    /* 'A'  */ LETTER (bit_upper | bit_xdigit),
    /* 'B'  */ LETTER (bit_upper | bit_xdigit),
    /* 'C'  */ LETTER (bit_upper | bit_xdigit),
    /* 'D'  */ LETTER (bit_upper | bit_xdigit),
    /* 'E'  */ LETTER (bit_upper | bit_xdigit),
    /* 'F'  */ LETTER (bit_upper | bit_xdigit),
    /* 'G'  */ LETTER (bit_upper),
    /* 'H'  */ LETTER (bit_upper),
    /* 'I'  */ LETTER (bit_upper),
    /* 'J'  */ LETTER (bit_upper),
    /* 'K'  */ LETTER (bit_upper),
    /* 'L'  */ LETTER (bit_upper),
    /* 'M'  */ LETTER (bit_upper),
    /* 'N'  */ LETTER (bit_upper),
    /* 'O'  */ LETTER (bit_upper),
    /* 'P'  */ LETTER (bit_upper),
    /* 'Q'  */ LETTER (bit_upper),
    /* 'R'  */ LETTER (bit_upper),
    /* 'S'  */ LETTER (bit_upper),
    /* 'T'  */ LETTER (bit_upper),
    /* 'U'  */ LETTER (bit_upper),
    /* 'V'  */ LETTER (bit_upper),
    /* 'W'  */ LETTER (bit_upper),
    /* 'X'  */ LETTER (bit_upper),
    /* 'Y'  */ LETTER (bit_upper),
    /* 'Z'  */ LETTER (bit_upper),
    /* '['  */ bit_print | bit_punct | bit_graph,
    /* '\\' */ bit_print | bit_punct | bit_graph,
    /* ']'  */ bit_print | bit_punct | bit_graph,
    /* '^'  */ bit_print | bit_punct | bit_graph,
    /* '_'  */ bit_print | bit_punct | bit_graph,
    /* '`'  */ bit_print | bit_punct | bit_graph,
    /* 'a'  */ LETTER (bit_lower | bit_xdigit),
    /* 'b'  */ LETTER (bit_lower | bit_xdigit),
    /* 'c'  */ LETTER (bit_lower | bit_xdigit),
    /* 'd'  */ LETTER (bit_lower | bit_xdigit),
    /* 'e'  */ LETTER (bit_lower | bit_xdigit),
    /* 'f'  */ LETTER (bit_lower | bit_xdigit),
    /* 'g'  */ LETTER (bit_lower),
    /* 'h'  */ LETTER (bit_lower),
    /* 'i'  */ LETTER (bit_lower),
    /* 'j'  */ LETTER (bit_lower),
    /* 'k'  */ LETTER (bit_lower),
    /* 'l'  */ LETTER (bit_lower),
    /* 'm'  */ LETTER (bit_lower),
    /* 'n'  */ LETTER (bit_lower),
    /* 'o'  */ LETTER (bit_lower),
    /* 'p'  */ LETTER (bit_lower),
    /* 'q'  */ LETTER (bit_lower),
    /* 'r'  */ LETTER (bit_lower),
    /* 's'  */ LETTER (bit_lower),
    /* 't'  */ LETTER (bit_lower),
    /* 'u'  */ LETTER (bit_lower),
    /* 'v'  */ LETTER (bit_lower),
    /* 'w'  */ LETTER (bit_lower),
    /* 'x'  */ LETTER (bit_lower),
    /* 'y'  */ LETTER (bit_lower),
    /* 'z'  */ LETTER (bit_lower),
    /* '{'  */ bit_print | bit_punct | bit_graph,
    /* '|'  */ bit_print | bit_punct | bit_graph,
    /* '}'  */ bit_print | bit_punct | bit_graph,
    /* '~'  */ bit_print | bit_punct | bit_graph,
    /* 0x7f */ bit_cntrl

#elif 'A' == 0xc1

    /* NUL */ bit_cntrl,
    /* SOH */ bit_cntrl,
    /* STX */ bit_cntrl,
    /* ETX */ bit_cntrl,
    /* PF  */ bit_cntrl,
    /* HT  */ bit_cntrl,
    /* LC  */ bit_cntrl,
    /* DEL */ bit_cntrl,
    /*     */ 0,
    /*     */ 0,
    /* SMM */ bit_cntrl,
    /* VT  */ bit_cntrl,
    /* FF  */ bit_cntrl,
    /* CR  */ bit_cntrl,
    /* SO  */ bit_cntrl,
    /* SI  */ bit_cntrl,
    /* DLE */ bit_cntrl,
    /* DC1 */ bit_cntrl,
    /* DC2 */ bit_cntrl,
    /* TM  */ bit_cntrl,
    /* RES */ bit_cntrl,
    /* NL  */ bit_cntrl,
    /* BS  */ bit_cntrl,
    /* IL  */ bit_cntrl,
    /* CAN */ bit_cntrl,
    /* EM  */ bit_cntrl,
    /* CC  */ bit_cntrl,
    /* CU1 */ bit_cntrl,
    /* IFS */ bit_cntrl,
    /* IGS */ bit_cntrl,
    /* IRS */ bit_cntrl,
    /* IUS */ bit_cntrl,
    /* DS  */ bit_cntrl,
    /* SOS */ bit_cntrl,
    /* FS  */ bit_cntrl,
    /*     */ 0,
    /* BYP */ bit_cntrl,
    /* LF  */ bit_cntrl,
    /* ETB */ bit_cntrl,
    /* ESC */ bit_cntrl,
    /*     */ 0,
    /*     */ 0,
    /* SM  */ bit_cntrl,
    /* CU2 */ bit_cntrl,
    /*     */ 0,
    /* ENQ */ bit_cntrl,
    /* ACK */ bit_cntrl,
    /* BEL */ bit_cntrl,
    /*     */ 0,
    /*     */ 0,
    /* SYN */ bit_cntrl,
    /*     */ 0,
    /* PN  */ bit_cntrl,
    /* RS  */ bit_cntrl,
    /* UC  */ bit_cntrl,
    /* EOT */ bit_cntrl,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* CU3 */ bit_cntrl,
    /* DC4 */ bit_cntrl,
    /* NAK */ bit_cntrl,
    /*     */ 0,
    /* SUB */ bit_cntrl,
    /* SP  */ bit_cntrl,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* ct. */ bit_print | bit_punct | bit_graph,
    /* '.' */ bit_print | bit_punct | bit_graph,
    /* '<' */ bit_print | bit_punct | bit_graph,
    /* '(' */ bit_print | bit_punct | bit_graph,
    /* '+' */ bit_print | bit_punct | bit_graph,
    /* '|' */ bit_print | bit_punct | bit_graph,
    /* '&' */ bit_print | bit_punct | bit_graph,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* '!' */ bit_print | bit_punct | bit_graph,
    /* '$' */ bit_print | bit_punct | bit_graph,
    /* '*' */ bit_print | bit_punct | bit_graph,
    /* ')' */ bit_print | bit_punct | bit_graph,
    /* ';' */ bit_print | bit_punct | bit_graph,
    /* '~' */ bit_print | bit_punct | bit_graph,
    /* '-' */ bit_print | bit_punct | bit_graph,
    /* '/' */ bit_print | bit_punct | bit_graph,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* ',' */ bit_print | bit_punct | bit_graph,
    /* '%' */ bit_print | bit_punct | bit_graph,
    /* '_' */ bit_print | bit_punct | bit_graph,
    /* '>' */ bit_print | bit_punct | bit_graph,
    /* '?' */ bit_print | bit_punct | bit_graph,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* ':' */ bit_print | bit_punct | bit_graph,
    /* '#' */ bit_print | bit_punct | bit_graph,
    /* '@' */ bit_print | bit_punct | bit_graph,
    /* ''' */ bit_print | bit_punct | bit_graph,
    /* '=' */ bit_print | bit_punct | bit_graph,
    /* '"' */ bit_print | bit_punct | bit_graph,
    /*     */ 0,
    /* 'a' */ LETTER (bit_lower | bit_xdigit),
    /* 'b' */ LETTER (bit_lower | bit_xdigit),
    /* 'c' */ LETTER (bit_lower | bit_xdigit),
    /* 'd' */ LETTER (bit_lower | bit_xdigit),
    /* 'e' */ LETTER (bit_lower | bit_xdigit),
    /* 'f' */ LETTER (bit_lower | bit_xdigit),
    /* 'g' */ LETTER (bit_lower),
    /* 'h' */ LETTER (bit_lower),
    /* 'i' */ LETTER (bit_lower),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* 'j' */ LETTER (bit_lower),
    /* 'k' */ LETTER (bit_lower),
    /* 'l' */ LETTER (bit_lower),
    /* 'm' */ LETTER (bit_lower),
    /* 'n' */ LETTER (bit_lower),
    /* 'o' */ LETTER (bit_lower),
    /* 'p' */ LETTER (bit_lower),
    /* 'q' */ LETTER (bit_lower),
    /* 'r' */ LETTER (bit_lower),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* 's' */ LETTER (bit_lower),
    /* 't' */ LETTER (bit_lower),
    /* 'u' */ LETTER (bit_lower),
    /* 'v' */ LETTER (bit_lower),
    /* 'w' */ LETTER (bit_lower),
    /* 'x' */ LETTER (bit_lower),
    /* 'y' */ LETTER (bit_lower),
    /* 'z' */ LETTER (bit_lower),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* '`' */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* 'A' */ LETTER (bit_upper | bit_xdigit),
    /* 'B' */ LETTER (bit_upper | bit_xdigit),
    /* 'C' */ LETTER (bit_upper | bit_xdigit),
    /* 'D' */ LETTER (bit_upper | bit_xdigit),
    /* 'E' */ LETTER (bit_upper | bit_xdigit),
    /* 'F' */ LETTER (bit_upper | bit_xdigit),
    /* 'G' */ LETTER (bit_upper),
    /* 'H' */ LETTER (bit_upper),
    /* 'I' */ LETTER (bit_upper),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* 'J' */ LETTER (bit_upper),
    /* 'K' */ LETTER (bit_upper),
    /* 'L' */ LETTER (bit_upper),
    /* 'M' */ LETTER (bit_upper),
    /* 'N' */ LETTER (bit_upper),
    /* 'O' */ LETTER (bit_upper),
    /* 'P' */ LETTER (bit_upper),
    /* 'Q' */ LETTER (bit_upper),
    /* 'R' */ LETTER (bit_upper),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* 'S' */ LETTER (bit_upper),
    /* 'T' */ LETTER (bit_upper),
    /* 'U' */ LETTER (bit_upper),
    /* 'V' */ LETTER (bit_upper),
    /* 'W' */ LETTER (bit_upper),
    /* 'X' */ LETTER (bit_upper),
    /* 'Y' */ LETTER (bit_upper),
    /* 'Z' */ LETTER (bit_upper),
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /* '0' */ bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '1' */ bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '2' */ bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '3' */ bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '4' */ bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '5' */ bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '6' */ bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '7' */ bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '8' */ bit_print | bit_digit | bit_graph | bit_xdigit,
    /* '9' */ bit_print | bit_digit | bit_graph | bit_xdigit,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0,
    /*     */ 0

#else   // 'A' != 0x41 && 'A' != 0xc1
#  error unknown character set (neither ASCII nor EBCDIC)
#endif   // ASCII or EBCDIC

};

// set to true if and only if the corresponding function
// is determined to be declared in <cwctype>
static bool iswalnum_declared;
static bool iswalpha_declared;
static bool iswcntrl_declared;
static bool iswdigit_declared;
static bool iswgraph_declared;
static bool iswlower_declared;
static bool iswprint_declared;
static bool iswpunct_declared;
static bool iswspace_declared;
static bool iswupper_declared;
static bool iswxdigit_declared;
static bool towlower_declared;
static bool towupper_declared;


static void
test_behavior ()
{
    rw_info (0, 0, 0, "behavior of <cwctype> functions in the \"C\" locale");

    char extra_str [80];
    char missing_str [80];

    for (int i = 0; i != 256; ++i) {

        const test_wint_t c = i;

        int mask = 0;

// invoke each function that is found to be declared in the header
// and exercise its return value; avoid testing functions that are
// not found to be declared in the header to prevent unnecessary
// noise
#define SET_MASK_BIT(bitno)                                     \
    if (isw ## bitno ## _declared)                               \
        mask |= std::isw ## bitno (c) ? bit_ ## bitno : 0;      \
    else                                                        \
        mask |= char_mask [i] & bit_ ## bitno

        SET_MASK_BIT (alnum);
        SET_MASK_BIT (alpha);
        SET_MASK_BIT (cntrl);
        SET_MASK_BIT (digit);
        SET_MASK_BIT (graph);
        SET_MASK_BIT (lower);
        SET_MASK_BIT (print);
        SET_MASK_BIT (punct);
        SET_MASK_BIT (space);
        SET_MASK_BIT (upper);
        SET_MASK_BIT (xdigit);

        const int extra_bits   = mask & ~char_mask [i];
        const int missing_bits = ~mask & char_mask [i];

        rw_assert (mask == char_mask [i], 0, 0,
                   "%#c mask%{?} missing bits %s (%#x)%{;}"
                   "%{?} extra bits %s (%#x)%{;}",
                   i,
                   missing_bits,
                   get_bitmask (missing_bits, missing_str), missing_bits,
                   extra_bits,
                   get_bitmask (extra_bits, extra_str), extra_bits);
    }
}

/**************************************************************************/

static int rw_opt_no_behavior;   // for --no-behavior

static int
run_test (int, char**)
{
    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, 0, "checking for the absence of masking macros");

    // verify the shadow macros are not #defined (explicitly
    // disallowed by 17.4.1.2, p6 and Footnote 159 of C++ '03)
    for (unsigned i = 0; cwctype_macros [i]; ++i) {
        rw_assert ('\0' == cwctype_macros [i][0], 0, 0,
                   "macro %s unexpectedly #defined", cwctype_macros [i]);
    }

    //////////////////////////////////////////////////////////////////
#ifndef WEOF
    const int weof_defined = 0;
#else    // if defined (WEOF)
    const int weof_defined = 1;
#endif   // WEOF

    rw_assert (weof_defined, 0, 0, "macro WEOF unexpectedly not #defined");

    //////////////////////////////////////////////////////////////////
    test_types ();

    //////////////////////////////////////////////////////////////////
    // verify that each function is defined

#define TEST(function)                                          \
    do {                                                        \
        rw_info (0, 0, 0, "%s::%s (%s::wint_t) definition",     \
                 std_name, #function, std_name);                \
        std::function (test_wint_t (function_called = 1));      \
        function ## _declared = 1 == function_called;           \
        rw_assert (function ## _declared, 0, __LINE__,          \
                   "%s::%s (%s::wint_t) not defined",           \
                   std_name, #function, std_name);              \
    } while (0)

    TEST (iswalnum);
    TEST (iswalpha);
    TEST (iswcntrl);
    TEST (iswdigit);
    TEST (iswgraph);
    TEST (iswlower);
    TEST (iswprint);
    TEST (iswspace);
    TEST (iswpunct);
    TEST (iswupper);
    TEST (iswxdigit);
    TEST (towlower);
    TEST (towupper);

    // exercise std::wctype(const char*)
    rw_info (0, 0, 0,
             "%s::wctype (const char*) definition", std_name);

    function_called = 1;

    std::wctype ("");

    rw_assert (1 == function_called, 0, __LINE__,
               "%s::wctype (const char*) not defined", std_name);


    // exercise std::iswctype(std::wint_t, std::wctype_t)
    rw_info (0, 0, 0,
             "%s::iswctype (%1$s::wint_t, %1$s::wctype_t) definition",
             std_name);

    function_called = 1;

    const test_wint_t wc = 0;
    const test_wctype_t desc = 0;
    
    std::iswctype (wc, desc);

    rw_assert (1 == function_called, 0, __LINE__,
               "%s::iswctype (%1$s::wint_t, %1$s::wctype_t) not defined",
               std_name);

    // exercise std::wctrans(const char*)
    rw_info (0, 0, 0,
             "%s::wctrans (const char*) definition", std_name);

    function_called = 1;

    std::wctrans ("");

    rw_assert (1 == function_called, 0, __LINE__,
               "%s::wctrans (const char*) not defined", std_name);

    //////////////////////////////////////////////////////////////////
    if (rw_opt_no_behavior)
        rw_note (0, 0, 0, "behavior test disabled (use --test-behavior)");
    else
        test_behavior ();

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.c.strings",
                    "header <cwctype>",
                    run_test,
                    "|-no-behavior#0-1 ",
                    &rw_opt_no_behavior);
}
