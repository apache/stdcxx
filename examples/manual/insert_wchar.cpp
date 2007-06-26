/**************************************************************************
 *
 * insert_wchar.cpp
 *
 * Example program demonstrating an implementation of an inserter
 * operator overloaded for arrays of wchar_t that performs codeset
 * conversion from wchar_t to mutlibyte characters.
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
 **************************************************************************/
 
#include <cassert>    // for assert()
#include <cwchar>     // for mbstate_t, size_t
#include <ios>        // for hex
#include <iostream>   // for cout
#include <locale>     // for codecvt, isalnum(), locale
#include <ostream>    // for basic_ostream
#include <sstream>    // for ostringstream


// inserts a wide character string into a stream buffer performing
// codeset conversion if necessary
template <class charT, class Traits>
void
streambuf_insert (std::basic_ostream<charT, Traits> &strm,
                  const wchar_t                     *s)
{
    typedef typename Traits::state_type                  StateT;
    typedef std::codecvt<wchar_t, charT, StateT>         Codecvt;

    const Codecvt &cvt = std::use_facet<Codecvt>(strm.getloc ());

    const std::size_t slen = std::char_traits<wchar_t>::length (s);

    // perform codeset conversion in chunks to avoid dynamic
    // memory allocation

    const std::size_t    xbufsize = 32;

    charT                xbuf [xbufsize];
    charT*               xbuf_end  = xbuf + xbufsize;
    charT*               to_next   = 0;
    const wchar_t*       from_next = 0;
    const wchar_t* const end       = s + slen;

    StateT state = StateT ();

    for (const wchar_t* base = s; from_next != end; base = from_next) {

        const std::codecvt_base::result res =
            cvt.out (state, base, end, from_next,
                     xbuf, xbuf_end, to_next);

        std::streamsize nbytes = to_next - xbuf;

        switch (res) {
        case Codecvt::error:
            // write out the sequence successfully converted up
            // to the point of the error in the internal sequence
            // and fail
            strm.rdbuf ()->sputn (xbuf, nbytes);
            strm.setstate (strm.badbit);

        case Codecvt::noconv:
            // write the entire sequence
            if (nbytes != strm.rdbuf ()->sputn (xbuf, nbytes)) {
                strm.setstate (strm.badbit);
                return;
            }

            from_next = end;   // effectively break
            break;

        default:
            assert (cvt.ok == res || cvt.partial == res);

            // partial conversion will result if there isn't enough
            // space in the conversion buffer to hold the converted
            // sequence, but we're O.K. since we'll be passing any
            // remaining unconverted characters (starting at
            // from_next) in the next iteration

            nbytes = to_next - xbuf;

            if (nbytes != strm.rdbuf ()->sputn (xbuf, nbytes)) {
                strm.setstate (strm.badbit);
                return;
            }
        }
    }
}


// stream insertion operator overloaded for arrays of wchar_t characters
template <class charT, class Traits>
std::basic_ostream<charT, Traits>&
operator<< (std::basic_ostream<charT, Traits> &strm,
            const wchar_t                     *s)
{
    const typename std::basic_ostream<charT, Traits>::sentry opfx (strm);

    if (opfx) {

        try {
            // try to insert character array into stream buffer
            streambuf_insert (strm, s);
        }
        catch (...) {
            bool threw;
            try {
                // set badbit on exception without throwing ios::failure
                strm.setstate (strm.badbit);
                threw = false;
            }
            catch (std::ios_base::failure&) {
                // make a note of the exception thrown from setstate()...
                threw = true;
            }
            if (threw) {
                // ...and rethrow the original exception
                throw;
            }
        }
    }

    return strm;
}


// examples of wide character strings
static const wchar_t* const wcs [] = {
    L"a", L"abc",
    // Greek letter Alpha:
    L"\x0391",   // "\xce\x91"
    // Greek letters Alpha Beta:
    L"\x0391\x0392",   // "\xce\x91\xce\x91\xce\x92"
    // Greek letters Alpha Beta Gamma:
    L"\x0391\x0392\x0393",   // "\xce\x91\xce\x92\xce\x93"
    // Tibetan digit zero:
    L"\x0f20",   // "\xe0\xbc\xa0"
    // Tibetan digits one, zero:
    L"\x0f21\x0f20",   // "\xe0\xbc\xa1\xe0\xbc\xa0"
    // Tibetan digits two, one, zero:
    L"\x0f22\x0f21\x0f20"   // "\xe0\xbc\xa2\xe0\xbc\xa1\xe0\xbc\xa0"
};


int main ()
{
    typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> Codecvt;

    // create a UCS/UTF-8 codecvt facet and install it in a locale
    const std::locale utf (std::cout.getloc (), new Codecvt ("UTF-8@UCS"));

    for (std::size_t i = 0; i != sizeof wcs / sizeof *wcs; ++i) {

        std::ostringstream strm;

        // imbue the UTF-8/UCS capable locale in a stringstream
        strm.imbue (utf);

        // insert each wide character string into the narrow stream
        // object relying on the inserter to convert each wide string
        // into the corresponding multibyte character string
        strm << wcs [i];

        // write out the wide character string in Unicode notation
        std::cout << "UCS-2: " << std::hex;

        for (const wchar_t *pwc = wcs [i]; *pwc != L'\0'; ++pwc)
            std::cout << "U+" << unsigned (*pwc) << ' ';

        const std::string str = strm.str ();

        std::cout << " ==> UTF-8: \"";

        typedef unsigned char UChar;

        // write out the the multibyte character sequence using
        // ordinary aphanumeric symbols or hex escape sequences
        // where necessary
        for (const char *pc = str.c_str (); *pc != '\0'; ++pc) {

            // parenthesize isalnum to prevent macro expension
            // in case the function happens to be (illegally)
            // shadowed by a macro
            if ((std::isalnum)(*pc, std::cout.getloc ()))
                std::cout << *pc;
            else
                std::cout << "\\x" << int (UChar (*pc));
        }

        std::cout << "\"\n";
    }
}
