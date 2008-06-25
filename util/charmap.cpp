/***************************************************************************
 *
 * charmap.cpp
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
 * Copyright 2001-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <rw/_defs.h>

// On Compaq Tru64 UNIX if included after assert.h, the definition of
// _XOPEN_SOURCE macro in assert.h selects a different declaration for 
// iconv than the one used in comp test.
#ifndef _WIN32
#  ifndef _RWSTD_NO_ICONV
#    include <iconv.h>
#  endif
#  include _RWSTD_CERRNO
#else
#  include <windows.h>
#endif  // _WIN32

#include <cassert>
#include <cctype>
#include <cerrno>     // for errno
#include <climits>
#include <clocale>    // for LC_CTYPE, setlocale()
#include <cstdio>
#include <cstdlib>
#include <cstring>    // for strrchr(), strerror()

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

#ifndef _RWSTD_NO_ICONV

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
#endif   // _RWSTD_NO_ICONV


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

    size_t num_bytes = 0;

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


// count the number of bytes in a multibyte sequence denoted
// by the argument by counting the number of escape characters
std::size_t Charmap::mbcharlen (const std::string &str) const
{
    std::size_t count = 1;

    const char escape = scanner_.escape_char ();

    for (std::size_t idx = 0; ; ++idx, ++count) {
        idx = str.find (escape, idx);

        if (std::string::npos == idx)
            break;
    }

    return count;
}


/**************************************************************************/

const char* const Charmap::
portable_charset[] = {
    /* 0x00       */ "<NUL>",
    /* 0x01   SOH */ 0,
    /* 0x02   STX */ 0,
    /* 0x03   ETX */ 0,
    /* 0x04   EOT */ 0,
    /* 0x05   ENQ */ 0,
    /* 0x06   ACK */ 0,
    /* 0x07   BEL */ "<alert>",
    /* 0x08       */ "<backspace>",
    /* 0x09   TAB */ "<tab>",
    /* 0x0a       */ "<newline>", 
    /* 0x0b       */ "<vertical-tab>",
    /* 0x0c       */ "<form-feed>",
    /* 0x0d       */ "<carriage-return>",
    /* 0x0e   SO  */ 0,
    /* 0x0f   SI  */ 0,
    /* 0x10   DLE */ 0,
    /* 0x11   DC1 */ 0,
    /* 0x12   DC2 */ 0,
    /* 0x13   DC3 */ 0,
    /* 0x14   DC4 */ 0,
    /* 0x15   NAK */ 0,
    /* 0x16   SYN */ 0,
    /* 0x17   ETB */ 0,
    /* 0x18   CAN */ 0,
    /* 0x19   EM  */ 0,
    /* 0x1a   SUB */ 0,
    /* 0x1b   ESC */ 0,
    /* 0x1c   IS4 */ 0,
    /* 0x1d   IS3 */ 0,
    /* 0x1e   IS2 */ 0,
    /* 0x1f   IS1 */ 0,
    /* 0x20   SPC */ "<space>",
    /* 0x21    !  */ "<exclamation-mark>",
    /* 0x22    '  */ "<quotation-mark>",
    /* 0x23    #  */ "<number-sign>", 
    /* 0x24    $  */ "<dollar-sign>",
    /* 0x25    %  */ "<percent-sign>",
    /* 0x26    &  */ "<ampersand>",
    /* 0x27    '  */ "<apostrophe>",
    /* 0x28    (  */ "<left-parenthesis>",
    /* 0x29    )  */ "<right-parenthesis>",
    /* 0x2a    *  */ "<asterisk>", 
    /* 0x2b    +  */ "<plus-sign>",
    /* 0x2c    ,  */ "<comma>",
    /* 0x2d    -  */ "<hyphen>",   // "<hyphen-minus>",
    /* 0x2e    .  */ "<period>",   // "<full-stop>",
    /* 0x2f    /  */ "<slash>",    // "<solidus>",
    /* 0x30    0  */ "<zero>",
    /* 0x31    1  */ "<one>",
    /* 0x32    2  */ "<two>",
    /* 0x33    3  */ "<three>",
    /* 0x34    4  */ "<four>",
    /* 0x35    5  */ "<five>",
    /* 0x36    6  */ "<six>",
    /* 0x37    7  */ "<seven>",
    /* 0x38    8  */ "<eight>",
    /* 0x39    9  */ "<nine>",
    /* 0x3a    :  */ "<colon>",
    /* 0x3b    ;  */ "<semicolon>",
    /* 0x3c    <  */ "<less-than-sign>", 
    /* 0x3d    =  */ "<equals-sign>",
    /* 0x3e    >  */ "<greater-than-sign>",
    /* 0x3f    ?  */ "<question-mark>",
    /* 0x40    @  */ "<commercial-at>",
    /* 0x41    A  */ "<A>",
    /* 0x42    B  */ "<B>",
    /* 0x43    C  */ "<C>",
    /* 0x44    D  */ "<D>",
    /* 0x45    E  */ "<E>",
    /* 0x46    F  */ "<F>",
    /* 0x47    G  */ "<G>",
    /* 0x48    H  */ "<H>",
    /* 0x49    I  */ "<I>",
    /* 0x4a    J  */ "<J>",
    /* 0x4b    K  */ "<K>",
    /* 0x4c    L  */ "<L>",
    /* 0x4d    M  */ "<M>",
    /* 0x4e    N  */ "<N>",
    /* 0x4f    O  */ "<O>",
    /* 0x50    P  */ "<P>",
    /* 0x51    Q  */ "<Q>",
    /* 0x52    R  */ "<R>",
    /* 0x53    S  */ "<S>",
    /* 0x54    T  */ "<T>",
    /* 0x55    U  */ "<U>",
    /* 0x56    V  */ "<V>",
    /* 0x57    W  */ "<W>",
    /* 0x58    X  */ "<X>",
    /* 0x59    Y  */ "<Y>",
    /* 0x5a    Z  */ "<Z>",
    /* 0x5b    [  */ "<left-square-bracket>",
    /* 0x5c    \  */ "<backslash>",    // "<reverse-solidus>",
    /* 0x5d    ]  */ "<right-square-bracket>",
    /* 0x5e    ^  */ "<circumflex>",   // "<circumflex-accent>",
    /* 0x5f    _  */ "<underscore>",   // "<low-line>",
    /* 0x60    `  */ "<grave-accent>",
    /* 0x61    a  */ "<a>",
    /* 0x62    b  */ "<b>",
    /* 0x63    c  */ "<c>",
    /* 0x64    d  */ "<d>",
    /* 0x65    e  */ "<e>",
    /* 0x66    f  */ "<f>",
    /* 0x67    g  */ "<g>",
    /* 0x68    h  */ "<h>",
    /* 0x69    i  */ "<i>",
    /* 0x6a    j  */ "<j>",
    /* 0x6b    k  */ "<k>",
    /* 0x6c    l  */ "<l>",
    /* 0x6d    m  */ "<m>",
    /* 0x6e    n  */ "<n>",
    /* 0x6f    o  */ "<o>",
    /* 0x70    p  */ "<p>",
    /* 0x71    q  */ "<q>",
    /* 0x72    r  */ "<r>",
    /* 0x73    s  */ "<s>",
    /* 0x74    t  */ "<t>",
    /* 0x75    u  */ "<u>",
    /* 0x76    v  */ "<v>",
    /* 0x77    w  */ "<w>",
    /* 0x78    x  */ "<x>",
    /* 0x79    y  */ "<y>",
    /* 0x7a    z  */ "<z>",
    /* 0x7b    {  */ "<left-brace>",    // "<left-curly-bracket>",
    /* 0x7c    |  */ "<vertical-line>", 
    /* 0x7d    }  */ "<right-brace>",   // "<right-curly-bracket>",
    /* 0x7e    ~  */ "<tilde>",
    /* 0x7f       */ 0
};


// convert a string of narrow character into a wchar_t
bool Charmap::convert_to_wc (const std::string& sym_name,
                             const std::string& ext_enc, wchar_t& wc)
{
#ifndef _RWSTD_NO_ISO_10646_WCHAR_T

    // the internal wchar_t representation for all characters
    // in all locales is always ISO-10646 (UCS) on this system
    return convert_to_ucs (sym_name, ext_enc, wc);

#else   // if defined _RWSTD_NO_ISO_10646_WCHAR_T

    if (UCS4_internal_ || Clocale_.empty ()) {

        // when using UCS as the internal encoding or for a locale
        // that has no corresponding C library locale convert the
        // character to ISO-10646 (UCS)
        return convert_to_ucs (sym_name, ext_enc, wc);
    }

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


char* Charmap::convert_to_utf8 (const char *inbuf, size_t inbuf_s, 
                                char *outbuf, size_t outbuf_s) const
{
#ifndef _RWSTD_NO_ICONV

    if (ic_to_utf8_ == iconv_t (-1))
        return 0;

    char* outbufp = outbuf;

#  ifndef _RWSTD_NO_ICONV_CONST_CHAR
    const char* inbufp = inbuf;
#  else
    char* inbufp = _RWSTD_CONST_CAST(char*, inbuf);
#  endif   // _RWSTD_NO_ICONV_CONST_CHAR
    
    if (std::size_t (-1) == 
        iconv (ic_to_utf8_, &inbufp, &inbuf_s, &outbufp, &outbuf_s)) {
        const char* const errtext = std::strerror (errno);

        issue_diag (W_ICONV, false, &next, 
                    "iconv failed to convert \"%s\" "
                    "to UTF-8: %s\n", inbuf, errtext);

        return 0;
    }

    return outbufp;

#else   // if defined (_RWSTD_NO_ICONV)

    return 0;

#endif   // _RWSTD_NO_ICONV

}



std::string Charmap::get_charmap_name () const
{
    const std::string::size_type idx = charmap_name_.rfind (_RWSTD_PATH_SEP);

    if (idx != std::string::npos)
        return charmap_name_.substr (idx + 1);

    return charmap_name_;
}


wchar_t Charmap::increment_wchar (wchar_t val) const
{
#ifndef _RWSTD_NO_ISO_10646_WCHAR_T

    // to increment a wchar_t value and keep the encoding all we have
    // to do is increment the val because the internal encoding is UCS
    return val + 1;

#else
    // to increment a wchar_t value and keep the encoding we have to
    // convert the wchar_t to the external encoding, increment that
    // string value, and convert back to the internal representation
    const rmb_cmap_iter it = rmb_cmap_.find (val);

    if (it != rmb_cmap_.end ()) {

        mb_cmap_iter ret;

        // multibyte character corresponding to the wchar_t value
        std::string encoding = it->second;

        // continue incrementing the multi-byte value until we get a valid 
        // character.  NOTE: this must be done for encodings such as SJIS where
        // \x7f in the last byte of a multibyte string is not a valid character
        // NOTE: this will not detect errors in the sequence, since the program
        // will continue until it finds a valid character
        do {
            int last_elm = int (encoding.size ()) - 1;

            while (last_elm >= 0) {

                typedef unsigned char UChar;

                const unsigned ic = UChar (encoding [last_elm]) + 1;

                // if incrementing the last element caused it to exceed
                // UCHAR_MAX increment the next higher byte if there is
                // one
                if (UCHAR_MAX < ic)
                    encoding [last_elm--] = '\0';
                else {
                    encoding [last_elm] = char (ic);
                    break;
                }
            }

            if (last_elm < 0)
                return -1;   // error

        } while ((ret = mb_cmap_.find (encoding)) == mb_cmap_.end ());

        return ret->second;
    }

    return -1;   // error

#endif   // _RWSTD_NO_ISO_10646_WCHAR_T    

}


bool Charmap::
increment_encoding (std::string &encoding)
{
    // find the last escape character in the human readable representation
    // of the encoding (i.e., in the multibyte character such as "/xf0/x80")
    const std::string::size_type pos =
        encoding.rfind (scanner_.escape_char ());

    // the escape character must be there (guaranteed by the scanner)
    assert (pos < encoding.size ());

    const char* end = 0;

    // convert the last character in the multibyte character to a numeric
    // value representing the last byte of the sequence
    unsigned last_byte =
        unsigned (scanner_.convert_escape (encoding.c_str () + pos, &end));

    // POSIX requires that the incremented value be non-NUL
    if (UCHAR_MAX <= last_byte || *end)
        return false;

    // increment the last byte
    ++last_byte;

    // format the last byte in the same notation (octal, decimal,
    // or hexadecimal escape sequence)
    static const char xdigits[] = "0123456789ABCDEF";

    char byte_str [5];
    char *pdig = byte_str;

    switch (encoding [pos + 1]) {
    case 'd': {   // decimal escape
        const unsigned hundreds = last_byte / 100;
        const unsigned tens     = (last_byte - hundreds) / 10;
        const unsigned units    = last_byte % 10;

        *pdig++ = 'd';

        if (hundreds)
            *pdig++ = xdigits [hundreds];

        *pdig++ = xdigits [tens];
        *pdig++ = xdigits [units];
        *pdig   = '\0';
        break;
    }

    case 'x': {   // hex escape
        const unsigned hi = last_byte >> 4;
        const unsigned lo = last_byte & 0xfU;

        *pdig++ = 'x';
        *pdig++ = xdigits [hi];
        *pdig++ = xdigits [lo];
        *pdig   = '\0';
        break;
    }
    default: {   // octal escape
        const unsigned hi  = last_byte >> 6;
        const unsigned mid = (last_byte >> 3) & 07U;
        const unsigned lo  = last_byte & 07U;

        if (hi)
            *pdig++ = xdigits [hi];

        *pdig++ = xdigits [mid];
        *pdig++ = xdigits [lo];
        *pdig   = '\0';
    }
    }   // switch

    // replace the last escape sequence with the new one
    encoding.replace (pos + 1, std::string::npos, byte_str);

    return true;
}


std::string Charmap::
encoding_to_mbchar (const std::string &encoding) const
{
    std::string mbchar;

    for (const char *pbyte = encoding.c_str (); pbyte && *pbyte; )
        mbchar += char (scanner_.convert_escape (pbyte, &pbyte));

    return mbchar;
}


// convert the locale's encoded character to UCS4 wchar_t
wchar_t Charmap::
convert_sym_to_ucs (const std::string &sym) const
{
    std::string::const_iterator it (sym.begin ());

    if (   sym.size () < 4 || *it != '<' || *++it != 'U'
        || !(std::isxdigit)(*++it)) {
        issue_diag (E_UCS, true, 0,
                    "Unable to convert symbolic name %s to UCS.\n",
                    sym.c_str ());
    }

    const unsigned long val = std::strtoul (&*it, (char**)0, 16);

    if (_RWSTD_WCHAR_MAX <= val)
        issue_diag (E_UCS, true, 0, 
                    "UCS value %lu of symbolic character %s out of range.\n",
                    val, sym.c_str ());

    return wchar_t (val);
}


// convert the locale's encoded character to UCS4/UCS2 wchar_t
bool Charmap::convert_to_ucs (const std::string &sym_name, 
                              const std::string &encoding, wchar_t& wc)
{
#ifndef _WIN32

    if (in_utf8_) {
        wc = utf8_decode (encoding.c_str (), &*(encoding.end () - 1));
        return true;
    }

    // allocate enough space for the longest possible UTF-8 character
    char utf8_enc [8 + 1 /* NUL */];

    const char* const ch_end =
        convert_to_utf8  (encoding.c_str (), encoding.size (),
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
        const int res = MultiByteToWideChar (codepage_, 0,
                                             encoding.c_str(), -1,
                                             ret, 2);
        if (!res && ERROR_INVALID_PARAMETER == GetLastError ()) {
            // the required codepage conversion table is not installed
            wc = convert_sym_to_ucs (sym_name);
            return true;
        }

        if (!res || ret[1] != 0)
            return false;

        wc = ret[0];
        return true;
    }

    wc = convert_sym_to_ucs (sym_name);
    return true;

#endif  // _WIN32
}


void Charmap::add_to_cmaps (const std::string &sym_name, 
                            const std::string &encoding,
                            bool               is_mbchar /* = false */)
{
    // compute the external (multibyte) encoding of the character
    // if necessary (i.e., unless already done by the caller)
    const std::string mbchar =
        is_mbchar ? encoding : encoding_to_mbchar (encoding);

    symnames_list_.push_back (sym_name);

    if (1 == mbchar.size ()) {
        // strval is a single-byte character

        const unsigned char ch = mbchar [0];

        // add the wide character and its symbolic name to the narrow
        // character maps
        if (forward_maps) {
            // the locale utility doesn't need reverse maps
            n_cmap_.insert (std::make_pair (sym_name, ch));
        }

        if (reverse_maps)
            rn_cmap_.insert (std::make_pair (ch, sym_name));

        if (ch > largest_nchar_)
            largest_nchar_ = ch;
    }

    // (try to) compute the wide character value of the character
    wchar_t wch;

    if (convert_to_wc (sym_name, mbchar, wch)) {

        // add the wide character and its symbolic name to the wide
        // character maps
        if (forward_maps) {
            // the locale utility doesn't need forward maps
            w_cmap_.insert (std::make_pair (sym_name, wch));
        }

        if (reverse_maps)
            rw_cmap_.insert (std::make_pair (wch, sym_name));

        // add the corresponding multibyte character to the multibyte
        // character maps
        mb_cmap_.insert (std::make_pair (mbchar, wch));
        rmb_cmap_.insert (std::make_pair (wch, mbchar));
    }

    // compute the UCS value of the character
    wchar_t uch;

    if (convert_to_ucs (sym_name, mbchar, uch)) {

        // add UCS character and its symbolic name to the UCS
        // character maps
        ucs4_cmap_.insert (std::make_pair (sym_name, uch));
        rucs4_cmap_.insert (std::make_pair (uch, sym_name));
    }
}


// process the characters implicitly defined by using ellipsis between
// two explicitly defined characters
std::size_t Charmap::
process_ellipsis (const Scanner::token_t &beg_tok, int num_ellipsis)
{
    // get the upper end of the range denoted by the ellipsis
    const Scanner::token_t end_tok = scanner_.next_token ();

    // get the human readabale encoding of the character
    // denoted by the lower end of the ellipsis
    const std::string encoding = scanner_.next_token ().name;

    // convert the encoding to a multibyte character
    std::string mbchar = encoding_to_mbchar (encoding);

    // add the beg_tok symbol name to the maps
    add_to_cmaps (beg_tok.name, mbchar, true);
    
    // extract the numeric portion of the symbolic character name
    // denoted by the lower end of the ellipsis
    std::size_t idx = 0;

    int base;           // numeric base
    const char *fmat;   // sprintf() format specifier

    const std::size_t beg_len = beg_tok.name.size ();

    // determine the value of the beginning of the range
    // denoted by the ellipsis
    if (2 == num_ellipsis) {
        base = 16;
        fmat = "%.*s%0*lX>";

        // advance to the first hex digit
        while (idx < beg_len && !(std::isxdigit)(beg_tok.name [idx]))
            ++idx;
    }
    else {
        base = 10;
        fmat = "%.*s%0*ld>";

        // advance to the first decimal digit
        while (idx < beg_len && !(std::isdigit)(beg_tok.name [idx]))
            ++idx;
    }

    // length of non-numeric prefix of the symbolic character name
    const std::size_t pfx_len = idx;

    // get the character value plus one (since the first value
    // has already been added to the map earlier)
    char *num_end;
    const unsigned long beg_val =
        1 + std::strtoul (beg_tok.name.c_str () + pfx_len, &num_end, base);

    // the length of the numeric portion
    const std::size_t num_size =
        num_end - (beg_tok.name.c_str () + pfx_len);

    // find the end of the range denoted by the ellipsis
    idx = 0;

    const std::size_t end_len = end_tok.name.size ();

    if (2 == num_ellipsis) {
        // advance to the next hex digit
        while (idx < end_len && !(std::isxdigit)(end_tok.name [idx]))
            ++idx;
    }
    else {
        // advance to the next dec digit
        while (idx < end_len && !(std::isdigit)(end_tok.name [idx]))
            ++idx;
    }

    const unsigned long end_val =
        std::strtoul (end_tok.name.c_str () + idx, (char**)0, base);

    // the ending numeric value must be greater than or equal
    // to the beginning numeric value
    if (end_val < beg_val)
        issue_diag (E_RANGE, true, &end_tok, 
                    "invalid range found in character map file\n");
    
    char next_name [MAX_SYM_NAME_LEN];

    std::size_t nchars = 0;

    const char* const pfx = beg_tok.name.c_str ();

    for (unsigned long val = beg_val; val <= end_val; ++val, ++nchars) {

        std::sprintf (next_name, fmat, pfx_len, pfx, num_size, val);
        
        // increment the last byte of the multibyte character
        // and if the result is valid (i.e., doesn't contain
        // an embedded NUL) add the generated name and the
        // multibyte character to the maps
        const unsigned char last_byte = mbchar [mbchar.size () - 1];
        if (last_byte < UCHAR_MAX) {
            mbchar [mbchar.size () - 1] = last_byte + 1;
            add_to_cmaps (next_name, mbchar, true);
        }
        else {
            // an ellipsis must not specify a range that includes
            // an encoding with an embedded NUL
            issue_diag (E_RANGE, true, &beg_tok, 
                        "encoding of an element in range contains NUL\n");
        }
    }

    // return the number of characters denoted by the ellipsis
    return nchars;
}


// process all the characters in the character map file.
void Charmap::process_chars()
{
    issue_diag (I_STAGE, false, 0, "processing CHARMAP section\n");

    std::size_t ntokens = 0;
    std::size_t nellips = 0;
    std::size_t nchars  = 0;

    next = scanner_.next_token();
    Scanner::token_t nextnext;

    // loop until we find the closing charmap token
    for ( ; next.token != Scanner::tok_charmap; ++ntokens) {

        switch (next.token) {

        case Scanner::tok_nl:
        case Scanner::tok_end:
            break;

        case Scanner::tok_sym_name:
            // the next token may be either ellipsis if this line
            // of the charmap is in the form:
            // "%s...%s %s\n", <sym_name>, <sym_name>, <encoding>
            // or an encoding if this line is in the format:
            // "%s %s\n", <sym_name>, <encoding>
            nextnext = scanner_.next_token ();
            ntokens += 3;

            switch (nextnext.token) {

            case Scanner::tok_abs_ellipsis:
                // absolute ellipsis (see ISO/IEC TR 14652)
                nchars += process_ellipsis (next, 3);
                ++nellips;
                break;

            case Scanner::tok_hex_ellipsis:
                // hexadecimal symbolic ellipsis (see ISO/IEC TR 14652)
                nchars += process_ellipsis (next, 2);
                ++nellips;
                break;

            case Scanner::tok_char_value:
                // character represented as a numeric constant
                add_to_cmaps (next.name, nextnext.name);
                ++nchars;
                break;

            default:
                issue_diag (E_SYNTAX, true, &next,
                            "byte value expected following symbolic "
                            "name in character map file\n");
            }

            scanner_.ignore_line ();
            break;

        default:
            issue_diag (E_SYNTAX, true, &next,
                        "symbolic name expected in character map file\n");
            break;
        }

        next = scanner_.next_token();
    }

    issue_diag (I_STAGE, false, 0,
                "done processing CHARMAP section (%lu tokens, "
                "%lu ellipses, %lu characters)\n",
                ntokens, nellips, nchars);

    // make sure that all characters in the portable character set
    // are in the charmap
    if (forward_maps)
        verify_portable_charset();
}


void Charmap::verify_portable_charset () const
{
    const std::size_t nchars =
        sizeof portable_charset / sizeof *portable_charset;

    for (std::size_t i = 0; i < nchars; ++i) {
        if (0 == portable_charset [i])
            continue;

        if (n_cmap_.find (portable_charset [i]) == n_cmap_.end ())
            issue_diag (W_NOPCS, false, 0, 
                        "member of portable character set %s not found "
                        "in the character map\n", portable_charset [i]);
    }
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
#ifndef _RWSTD_NO_ICONV
    ic_to_utf8_ = 0;
    ic_to_ext_ = 0;
#endif   // _RWSTD_NO_ICONV

    scanner_.open (fname, '#', '\\');

    // set code_set_name to the name of the character set description
    // file by default, in case it's not explicitly specified
    const char* const slash = std::strrchr (fname, _RWSTD_PATH_SEP);
    code_set_name_ = slash ? slash + 1 : fname;

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
#ifndef _RWSTD_NO_ICONV
            if (!in_utf8_) {
                ic_to_utf8_ = open_iconv_to_utf8 ();
#  if !defined (_RWSTD_NO_ISO_10646_WCHAR_T)
                ic_to_ext_ = open_iconv_to_ext ();
#  endif   // _RWSTD_NO_ISO_10646_WCHAR_T
            }

#else   // if defined (_RWSTD_NO_ICONV)

#  ifdef _WIN32
            codepage_ = get_codepage (code_set_name_);
            if (codepage_ == 0) {
                issue_diag (W_ICONV, false, 0, 
                            "iconv_open (%s to UTF-8) failed\n",
                            code_set_name_.c_str());
            }

#  endif   // _WIN32
#endif   // _RWSTD_NO_ICONV

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
