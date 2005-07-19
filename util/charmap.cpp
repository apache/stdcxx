/***************************************************************************
 *
 * charmap.cpp
 *
 * $Id: //stdlib/dev/source/stdlib/util/charmap.cpp#95 $
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

#include <rw/_defs.h>

// On Compaq Tru64 UNIX if included after assert.h, the definition of
// _XOPEN_SOURCE macro in assert.h selects a different declaration for 
// iconv than the one used in comp test.
#ifndef _MSC_VER
#  include <iconv.h>
#  include _RWSTD_CERRNO
#else
#  include <windows.h>
#endif  // _MSC_VER

#include <cassert>
#include <cctype>
#include <cerrno>     // for errno
#include <climits>
#include <clocale>    // for LC_CTYPE, setlocale()
#include <cstdio>
#include <cstdlib>
#include <cstring>    // for strerror()

#include <map>
#include <string>

#include <vector>
#include <iostream>
#include <fstream>

#include "aliases.h"
#include "scanner.h"
#include "charmap.h"
#include "loc_exception.h"
#include "diagnostic.h"

// This value specifies the largest allowed symbolic name length
// If necessary this can be increased, but it is very doubtful that 
// that would ever be necessary
#define MAX_SYM_NAME_LEN 256

// this is the maximum size of a single byte of a character in the 
// charmap file.  According to POSIX this cannot be larger then 5
// because all bytes are in the format "\x%x", "\d%x" or "\%o" and
// the numeric values cannot be greater then 3 digits long
#define MAX_BYTE_LEN 5

#ifndef _MSC_VER

static iconv_t
my_iconv_open (const char *to_codeset, const char *from_codeset)
{
    typedef std::vector<std::string> StrVec;

    StrVec aliases [2];

    const bool to_utf8   = !std::strcmp (to_codeset, "UTF-8");
    const bool from_utf8 = !to_utf8;

//     aliases [to_utf8].push_back (to_codeset);
//     aliases [from_utf8].push_back (from_codeset);

    get_cname_aliases (to_codeset, aliases [to_utf8]);
    get_cname_aliases (from_codeset, aliases [from_utf8]);

    typedef StrVec::iterator VecIter;

    std::string tried_names [2];

    for (VecIter i = aliases [to_utf8].begin (); i != aliases [to_utf8].end ();
         ++i) {

        for (VecIter j = aliases [from_utf8].begin ();
             j != aliases [from_utf8].end (); ++j) {

            const char* const to_code = (*i).c_str ();
            const char* const from_code = (*j).c_str ();

            const iconv_t ret = iconv_open (to_code, from_code);

            if (ret != iconv_t (-1))
                return ret;

            if (i == aliases [to_utf8].begin ()) {

                if (tried_names [from_utf8].size ()) {
                    tried_names [from_utf8] += ',';
                    tried_names [from_utf8] += ' ';
                }

                tried_names [from_utf8] += '"';
                tried_names [from_utf8] += *j;
                tried_names [from_utf8] += '"';
            }
        }

        if (tried_names [to_utf8].size ()) {
            tried_names [to_utf8] += ',';
            tried_names [to_utf8] += ' ';
        }
            
        tried_names [to_utf8] += '"';
        tried_names [to_utf8] += *i;
        tried_names [to_utf8] += '"';
    }

    assert (0 != aliases [0].size ());
    assert (0 != aliases [1].size ());

    issue_diag (W_ICONV, false, 0, 
                "iconv_open(\"%s\", \"%s\") failed; "
                "tried { %s } and { %s }\n",
                aliases [to_utf8][0].c_str (),
                aliases [from_utf8][0].c_str (),
                tried_names [to_utf8].c_str (),
                tried_names [from_utf8].c_str ());
    
    return iconv_t (-1);
}

// open an iconv file descriptor to convert from the codeset to utf8
iconv_t Charmap::open_iconv_to_utf8 () const
{
    if (in_utf8_)
        return 0;

    return my_iconv_open ("UTF-8", code_set_name_.c_str ());
}

#  ifndef _RWSTD_NO_ISO_10646_WCHAR_T

iconv_t Charmap::open_iconv_to_ext ()
{
    return my_iconv_open (code_set_name_.c_str (), "UTF-8");
}

#  endif   // _RWSTD_NO_ISO_10646_WCHAR_T
#endif  // _MSC_VER


// utf8_decode translates the UTF-8 encoded character (specified
// by the range [from, to) into an object of type wchar_t
// algorithm derived from RFC2279
static wchar_t utf8_decode (const char* from, const char* to)
{
    assert (from <= to);

    const unsigned char* const ch =
        _RWSTD_REINTERPRET_CAST (const unsigned char*, from);

    const unsigned char* const ch_end =
        _RWSTD_REINTERPRET_CAST (const unsigned char*, to);

    size_t num_bytes;

    wchar_t ret = 0;

    // if the first character is below 0x80 then the value of *ch is the
    // actual value of the character so return that value as a wchar_t
    if (*ch < 0x80) 
        return wchar_t (*ch);

    // if *ch is between 0xc2 and 0xe0 there are 2 bytes in the multi-byte
    // character
    if (*ch >= 0xc2 && *ch < 0xe0) {
        ret       = (*ch & 0x1f);
        num_bytes = 2;
    }

    // if *ch is between 0xe0 and 0xf0 there are 3 bytes in the multi-byte
    // character
    else if (*ch >= 0xe0 && *ch < 0xf0) {
        ret       = *ch & 0x0f;
        num_bytes = 3;
    }
    else if (*ch >= 0xf0 && *ch < 0xf8) {
        ret       = *ch & 0x07;
        num_bytes = 4;
    }
    else if (*ch >= 0xf8 && *ch < 0xfc) {
        ret       = *ch & 0x03;
        num_bytes = 5;
    }
    else if (*ch >= 0xfc && *ch < 0xfe) {
        ret       = *ch & 0x01;
        num_bytes = 6;
    }
    else {
        issue_diag (E_MBCHAR, true, 0, 
                    "illegal multibyte prefix '\\x%02x' in character "
                    "map file\n", *ch);
    }
    
    if (ch_end < ch + num_bytes - 1) {
        // the input doesn't have enough characters
        issue_diag (E_MBCHAR, true, 0, 
                    "incomplete multibyte character in character "
                    "map file: expecting %u bytes, found %u\n",
                    num_bytes, ch_end - ch);
    }

    // for each byte in the character extract the useful data by shifting
    // and bit or it into the wchar_t
    for (size_t i = 1; i < num_bytes; ++i)
        ret = (ret << 6) | (ch [i] & 0x3f);

    return ret;        
}


#if 0

// convert the first character of a string to an unsigned char 
unsigned char Charmap::
convert_escape (const char *str, const char **pend /* = 0 */) const
{
    assert (str != 0);

    if (!*str || *str != scanner_.escape_char ())
        issue_diag (E_CVT, true, &next,
                    "unable to convert character %s\n", str);

    long ch = 0;

    char *end;

    switch (str [1]) {
    case '%': ch = std::strtol (str + 2, &end, 8); break;
    case 'd': ch = std::strtol (str + 2, &end, 10); break;
    case 'x': ch = std::strtol (str + 2, &end, 16); break;
    default: ch = -1; end = _RWSTD_CONST_CAST (char*, str) + 1;
    }

    if (end == str + 2 || (ch < 0 || ch > UCHAR_MAX))
        issue_diag (E_IFMT, true, &next,
                    "unable to convert character %s\n", str);

    if (pend)
        *pend = end;

    typedef unsigned char UChar;

    return UChar (ch);
}

#endif


// find the number of bytes in the multibyte string by counting the
// number of escape chars in the string
size_t Charmap::mbcharlen (const std::string &str) const
{
    std::size_t count = 0;

    std::size_t idx = str.find (scanner_.escape_char ());

    for (; std::string::npos != idx; ++count) {
        idx = str.find (scanner_.escape_char (), idx + 1);
    }

    return count;
}



// convert a string of narrow character into a wchar_t
bool Charmap::convert_to_wc (const std::string& sym_name,
                             const std::string& ext_enc, wchar_t& wc)
{

#ifndef _RWSTD_NO_ISO_10646_WCHAR_T

    // the internal wchar_t representation for all characters
    // in all locales is always ISO-10646 (UCS) on this system
    return convert_to_ucs (sym_name, ext_enc, wc);

#else   // if defined _RWSTD_NO_ISO_10646_WCHAR_T

    if (UCS4_internal_) {

        // translate the character to ISO-10646 (UCS)
        return convert_to_ucs (sym_name, ext_enc, wc);
    }

    // for a locale that has no corresponding C library locale
    // convert the character to ISO-10646 (UCS)
    if (Clocale_.empty ()) 
        return convert_to_ucs (sym_name, ext_enc, wc);

    // otherwise use libc to convert the multi-byte character
    // to its wchar_t value
    if (-1 == std::mbtowc (&wc, ext_enc.c_str (), ext_enc.size ())) {

        const char* const locname = std::setlocale (LC_CTYPE, 0);
        const char* const errtext = std::strerror (errno);

        // diagnose the failure to convert the character as just
        // a warning and (try to) convert it to ISO-10646 (UCS)
        issue_diag (W_CALL, true, &next,
                    "mbtowc failed to convert character in locale "
                    "\"%s\": %s\n", locname, errtext);

        return convert_to_ucs (sym_name, ext_enc, wc);
    }

    return true;

#endif   // _RWSTD_NO_ISO_10646_WCHAR_T
}


#ifndef _MSC_VER
char* Charmap::convert_to_utf8 (const char *inbuf, size_t inbuf_s, 
                                char *outbuf, size_t outbuf_s) const
{
    if (ic_to_utf8_ == iconv_t (-1))
        return 0;

    char* outbufp = outbuf;

#ifndef _RWSTD_NO_ICONV_CONST_CHAR
    const char* inbufp = inbuf;
#else
    char* inbufp = _RWSTD_CONST_CAST(char*, inbuf);
#endif   // _RWSTD_NO_ICONV_CONST_CHAR
    
    if (std::size_t (-1) == 
        iconv (ic_to_utf8_, &inbufp, &inbuf_s, &outbufp, &outbuf_s)) {
        const char* const errtext = std::strerror (errno);

        issue_diag (W_ICONV, false, &next, 
                    "iconv failed to convert \"%s\" "
                    "to UTF-8: %s\n", inbuf, errtext);

        return 0;
    }

    return outbufp;
}
#endif  // _MSC_VER


std::string Charmap::get_charmap_name () const
{
    const std::string::size_type idx = charmap_name_.rfind (_RWSTD_PATH_SEP);

    if (idx != std::string::npos)
        return charmap_name_.substr (idx + 1);

    return charmap_name_;
}


wchar_t Charmap::increment_val (const wchar_t val) const
{
#ifndef _RWSTD_NO_ISO_10646_WCHAR_T
    // to increment a wchar_t value and keep the encoding all we have
    // to do is increment the val because the internal encoding is utf8
    return val + 1;

#else
    // to increment a wchar_t value and keep the encoding we have to
    // convert the wchar_t to the external encoding, increment that
    // string value, and convert back to the internal representation
    rn_cmap2_iter it = rn_cmap2_.find (val);

    if (it != rn_cmap2_.end ()) {

        n_cmap2_iter ret;

        std::string mb_str = it->second;
        // continue incrementing the multi-byte value until we get a valid 
        // character.  NOTE: this must be done for encodings such as SJIS where
        // \x7f in the last byte of a multibyte string is not a valid character
        // NOTE: this will not detect errors in the sequence, since the program
        // will continue until it finds a valid character
        do {
            int last_elm = mb_str.size() - 1;

            while (last_elm >= 0) {

                typedef unsigned char UChar;

                const unsigned ic = UChar (mb_str [last_elm]) + 1;

                // if incrementing the last element caused it to exceed
                // 0xff increment the next higher byte if there is one
                if (ic > 0xff)
                    mb_str [last_elm--] = 0;
                else {
                    mb_str [last_elm] = char (ic);
                    break;
                }
            }

            if (last_elm < 0)
                std::cerr << "cannot convert character\n";
        } while ((ret = n_cmap2_.find (mb_str)) == n_cmap2_.end ());

        return ret->second;
    }

    return -1;

#endif   // _RWSTD_NO_ISO_10646_WCHAR_T    
}

const char* Charmap::increment_strval (const char* str)
{
    static char s  [64];
    static char sd [64];
    int         i = 0;

    char* ps = s;

    // zero the first element of string s
    *ps = 0;

    if (str == 0 || *str == 0)
        return s;

    for (i = 0; *str && i < 64; i++) {

        *ps++ = scanner_.convert_escape (str, (const char**)&str);
    }

    *ps = 0;

    //  now  attempt to  increment the  last character  in the  string  if the
    //  character gets incremented above  /xff then we increment the next char
    if (ps == s) {
        return s;
    }

    for (ps--; ps >= s; ps--) 
        if ((unsigned char)(*ps) == 0xff) {
            *ps = 0;
            continue;
        } else {
            (*ps)++;
            break;
        }

    const char fmt [] = {scanner_.escape_char (), 
                         'x', '%', '0', '2', 'x', '\0'};
    ps = s;
    for (i = 0; *ps; i += 4, ps++)
        std::sprintf (&sd [i], fmt, *(unsigned char*)ps);

    return sd;
}


std::string Charmap::parse_ext_strval (const std::string &strval) const
{
    std::string ext_enc;

    for (const char *str = strval.c_str (); str && *str; )
        ext_enc += char (scanner_.convert_escape (str, &str));

    return ext_enc;
}


// convert the locale's encoded character to UCS4 wchar_t
wchar_t Charmap::convert_sym_to_ucs (const std::string& s) const
{
    std::string::const_iterator it (s.begin ());

    if (s.size () < 4 || *it != '<' || *++it != 'U') {
        issue_diag (E_UCS, true, 0, 
                    "Attempt to convert symbolic name to UCS value failed. "
                    "Name %s not in <Uxxxxxxxx> form.\n", s.c_str ());
    }

    long w = std::strtol (&*++it, (char**)0, 16);
    if (w == _RWSTD_LONG_MIN || w == _RWSTD_LONG_MAX ||
        w >  _RWSTD_WCHAR_T_MAX )
        issue_diag (E_UCS, true, 0, 
                    "Attempt to convert symbolic name to UCS value failed. "
                    "Value of %s out of range.\n", s.c_str ());

    return wchar_t (w);
}

// convert the locale's encoded character to UCS4/UCS2 wchar_t
bool Charmap::convert_to_ucs (const std::string &sym_name, 
                              const std::string &ext_enc, wchar_t& wc)
{
#ifndef _MSC_VER

    if (in_utf8_) {
        wc = utf8_decode (ext_enc.c_str (), &*(ext_enc.end () - 1));
        return true;
    }

    char utf8_enc [_RWSTD_MB_LEN_MAX + 1];

    const char* const ch_end =
        convert_to_utf8  (ext_enc.c_str (), ext_enc.size (),
                          utf8_enc, sizeof utf8_enc);
    if (ch_end)
        // only if conversion to utf8 succeeded
        wc = utf8_decode (utf8_enc, ch_end);
    else
        // if not, try to convert the symbolic name directly
        wc = convert_sym_to_ucs (sym_name);

    return true;

#else

    if (0 != codepage_) {
        wchar_t ret[2] = {0};
        MultiByteToWideChar (codepage_, 0, ext_enc.c_str(), -1, ret, 2);
        if (ret[1] != 0)
            return false;

        wc = ret[0];
        return true;
    } else {
        wc = convert_sym_to_ucs (sym_name);
        return true;
    }

    return false;

#endif  // _MSC_VER
}


void Charmap::add_to_cmaps (const std::string &sym_name, 
                            const std::string &strval)
{
    strval_map_.insert (std::make_pair (strval, sym_name));

    if (mbcharlen (strval) == 1) {

        const unsigned char ch = scanner_.convert_escape (strval.c_str ());

        if (forward_maps)
            n_cmap_.insert (std::make_pair (sym_name, ch));

        if (reverse_maps)
            rn_cmap_.insert (std::make_pair (ch, sym_name));

        if (ch > largest_nchar_)
            largest_nchar_ = ch;
    }

    const std::string ext_enc = parse_ext_strval (strval);

    wchar_t ch;

    if (convert_to_wc (sym_name, ext_enc, ch)) {

        if (forward_maps)
            w_cmap_.insert (std::make_pair (sym_name, ch));

        if (reverse_maps)
            rw_cmap_.insert (std::make_pair (ch, sym_name));

        std::string n_strval = parse_ext_strval (strval);

        n_cmap2_.insert (std::make_pair (n_strval, ch));
        rn_cmap2_.insert (std::make_pair (ch, n_strval));

        assert (n_strval.size () != 0);

        for (std::string::size_type i = n_strval.size (); --i; )
            valid_mb_set_.insert (n_strval.substr (0, i));
    }

    wchar_t ucs_val;

    if (convert_to_ucs (sym_name, ext_enc, ucs_val)) {
        ucs4_cmap_.insert (std::make_pair (sym_name, ucs_val));
        rucs4_cmap_.insert (std::make_pair (ucs_val, sym_name));
    }
}


// process the characters implicitly defined by using ellipsis between
// two explicitly defined characters
void Charmap::
process_ellipsis (const Scanner::token_t &beg_tok, int num_ellipsis)
{
    const Scanner::token_t end_tok = scanner_.next_token ();
    std::string strval = scanner_.next_token ().name;

    // first add the beg_tok symbol name to the maps
    add_to_cmaps (beg_tok.name, strval);
    
    // seperate the numeric portion of the symbolic name from the
    // character portion in order to dynamically create symbolic
    // names with increasing numeric values
    std::string begin;
    size_t idx = 0;

    int base;           // numeric base
    const char *fmat;   // sprintf() format specifier

    // determine the value of the beginning of the range
    // denoted by the ellipsis
    if (num_ellipsis == 2) {
        base = 16;
        fmat = "%s%0*lX>";

        // advance to the first hex digit
        while (   idx < beg_tok.name.size ()
               && !(std::isxdigit)(beg_tok.name [idx]))
            begin += beg_tok.name [idx++];
    }
    else {
        base = 10;
        fmat = "%s%0*ld>";

        // advance to the first decimal digit
        while (   idx < beg_tok.name.size ()
               && !(std::isdigit)(beg_tok.name [idx]))
            begin += beg_tok.name [idx++];
    }

    std::string num_str;  // the numeric portion of the sym name

    // get the numeric portion of the sym_name, this is the portion
    // that will be different for each sym_name within the ellipsis
    while (idx < beg_tok.name.size () && beg_tok.name [idx] != '>')
        num_str += beg_tok.name [idx++];

    const int num_size = int (num_str.size ());
    
    // convert the num_str to a long
    unsigned long start_num = std::strtoul (num_str.c_str(), (char**)0, base);

    // increment the start_num once because we already added the 
    // beg_tok symbol name to the cmaps
    ++start_num;

    // find the end of the range denoted by the ellipsis
    idx = 0;

    if (num_ellipsis == 2) {
        // advance to the next hex digit
        while (   idx < end_tok.name.size ()
               && !(std::isxdigit)(end_tok.name [idx]))
            ++idx;
    }
    else {
        // advance to the next dec digit
        while (   idx < end_tok.name.size ()
               && (std::isdigit)(end_tok.name [idx]))
            ++idx;
    }

    num_str.clear ();

    // advance to the closing bracket ('>')
    while (idx < end_tok.name.size() && end_tok.name[idx] != '>')
        num_str += end_tok.name [idx++];
    
    unsigned long end_num = std::strtoul (num_str.c_str(), (char**)0, base);

    // the ending numeric value should be greater then the start numeric value
    if (end_num < start_num)
        issue_diag (E_RANGE, true, &end_tok, 
                    "invalid range found in character map file\n");
    
    char next_name [MAX_SYM_NAME_LEN];

    for (; start_num <= end_num; ++start_num) {

        std::sprintf (next_name, fmat, begin.c_str (), num_size, start_num);
        
        // increment the string value to the next encoded character value
        strval = increment_strval (strval.c_str ());

        // and finally add the generated name and string value to the maps
        add_to_cmaps (next_name, strval);
    }
}


// process all the characters in the character map file.
void Charmap::process_chars()
{
    next = scanner_.next_token();
    Scanner::token_t nextnext;

    // loop until we find the closing charmap token
    while (next.token != Scanner::tok_charmap) {

        switch (next.token){

        case Scanner::tok_nl:
        case Scanner::tok_end:
            break;

        case Scanner::tok_sym_name:
            // the next token may be either ellipsis if this line of the
            // charmap is in the form: 
            // "%s...%s %s\n", <sym_name>, <sym_name>, <encoding>
            // or an encoding if this line is in the format:
            // "%s %s\n", <sym_name>, <encoding>
            nextnext = scanner_.next_token();

            switch (nextnext.token) {

            case Scanner::tok_ellipsis:
                process_ellipsis (next, 3);
                break;

            case Scanner::tok_dellipsis:
                process_ellipsis (next, 2);
                break;

            case Scanner::tok_decimal_value:
            case Scanner::tok_hex_value:
            case Scanner::tok_octal_value:
                add_to_cmaps (next.name, nextnext.name);
                break;

            default:
                issue_diag (E_SYNTAX, true, &next,
                            "byte value expected following symbolic "
                            "name in character map file\n");
            }

            scanner_.ignore_line();
            break;

        default:
            issue_diag (E_SYNTAX, true, &next,
                        "symbolic name expected in character map file\n");
            break;
        }

        next = scanner_.next_token();

    }
    
    // make sure that all characters in the portable character set
    // are in the charmap
    if (forward_maps)
        verify_portable_charset();
}


void Charmap::verify_portable_charset()
{
    static const char* const charset[] = {
        "<NUL>","<alert>", "<backspace>", "<tab>", "<newline>", 
        "<vertical-tab>", "<form-feed>", "<carriage-return>", "<space>",
        "<exclamation-mark>", "<quotation-mark>", "<number-sign>", 
        "<dollar-sign>", "<percent-sign>", "<ampersand>", "<apostrophe>",
        "<left-parenthesis>", "<right-parenthesis>", "<asterisk>", 
        "<plus-sign>", "<comma>", "<hyphen>", "<hyphen-minus>", "<period>",
        "<full-stop>", "<slash>", "<solidus>", "<zero>", "<one>", "<two>",
        "<three>", "<four>", "<five>", "<six>", "<seven>", "<eight>",
        "<nine>", "<colon>", "<semicolon>", "<less-than-sign>", 
        "<equals-sign>", "<greater-than-sign>", "<question-mark>",
        "<commercial-at>", "<A>", "<B>", "<C>", "<D>", "<E>", "<F>", "<G>",
        "<H>", "<I>", "<J>", "<K>", "<L>","<M>", "<N>", "<O>", "<P>","<Q>",
        "<R>", "<S>", "<T>", "<U>", "<V>", "<W>", "<X>", "<Y>", "<Z>",
        "<left-square-bracket>", "<backslash>", "<reverse-solidus>",
        "<right-square-bracket>", "<circumflex>", "<circumflex-accent>",
        "<underscore>", "<low-line>", "<grave-accent>",
        "<a>", "<b>", "<c>", "<d>", "<e>", "<f>", "<g>",
        "<h>", "<i>", "<j>", "<k>", "<l>","<m>", "<n>", "<o>", "<p>","<q>",
        "<r>", "<s>", "<t>", "<u>", "<v>", "<w>", "<x>", "<y>", "<z>",
        "<left-brace>", "<left-curly-bracket>", "<vertical-line>", 
        "<right-brace>", "<right-curly-bracket>", "<tilde>"
    };

    for (std::size_t i = 0; i < sizeof charset / sizeof (char*); ++i)
        if (n_cmap_.find (charset [i]) == n_cmap_.end ())
            issue_diag (W_NOPCS, false, 0, 
                        "member of portable character set %s not found "
                        "in the character map\n", charset[i]);
}


Charmap::Charmap(const char* Clocale, 
                 const char* fname, 
                 bool in_utf8, bool create_forward_maps,
                 bool create_reverse_maps, bool use_UCS4)
    :  mb_cur_max_(1), 
       charmap_name_ (fname),
       Clocale_ (Clocale),
       largest_nchar_(0),
       in_utf8_(in_utf8),
       forward_maps (create_forward_maps), 
       reverse_maps (create_reverse_maps),
       UCS4_internal_ (use_UCS4)
{
#ifndef _MSC_VER
    ic_to_utf8_ = 0;
    ic_to_ext_ = 0;
#endif  // _MSC_VER

    scanner_.open (fname, '#', '\\');

    // loop until we reach the end of the file
    while ((next = scanner_.next_token()).token  != Scanner::tok_end_tokens) {

        switch (next.token) {

        case Scanner::tok_code_set_name:
            next = scanner_.next_token ();

            if (next.token == Scanner::tok_string) {
                code_set_name_ = next.name.substr (1, next.name.size () - 2);
            }
            else if (next.token == Scanner::tok_ndef) {
                code_set_name_ = next.name;
            }
            else 
                issue_diag (E_SYNTAX, true, &next,
                            "string expected following <code_set_name>\n");

            // we always need a iconv to utf8 so that we can create
            // the utf8_charmap unless we are on windows
#if !defined (_MSC_VER)
            if (!in_utf8_) {
                ic_to_utf8_ = open_iconv_to_utf8 ();
#  if !defined (_RWSTD_NO_ISO_10646_WCHAR_T)
                ic_to_ext_ = open_iconv_to_ext ();
#  endif   // _RWSTD_NO_ISO_10646_WCHAR_T
            }
#else
            codepage_ = get_codepage (code_set_name_);
            if (codepage_ == 0) {
                issue_diag (W_ICONV, false, 0, 
                            "iconv_open (%s to UTF-8) failed\n",
                            code_set_name_.c_str());
            }
#endif     // _MSC_VER

            scanner_.ignore_line ();
            break;

        case Scanner::tok_mb_cur_max:
            mb_cur_max_ = std::atoi (scanner_.next_token ().name.c_str ());
            scanner_.ignore_line ();
            break;

        case Scanner::tok_mb_cur_min:
            scanner_.ignore_line ();
            break;

        case Scanner::tok_charmap:
            scanner_.ignore_line ();
            process_chars();
            break;
        case Scanner::tok_width:
            // ignore the width section of the character map
            while ((next = scanner_.next_token ()).token != Scanner::tok_width);
            break;

        case Scanner::tok_nl:
            break;

        default:
            issue_diag (E_SYNTAX, false, &next, 
                        "unknown token %s in character map file\n",
                        next.name.c_str ());
        }
    }
}
