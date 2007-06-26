/**************************************************************************
 *
 * mbsrtowcs.cpp
 *
 * Example program to demonstrate an implementation of the C Standard
 * Library function mbsrtowcs() in terms of the C++ Standard Library
 * codecvt facet.
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
#include <cerrno>     // for EILSEQ, errno
#include <cstring>    // for strlen()
#include <cwchar>     // for mbstate_t
#include <ios>        // for hex
#include <iostream>   // for cout
#include <locale>     // for codecvt, locale

#include <examples.h>


// my_mbsrtowcs() behaves 
std::size_t
my_mbsrtowcs (std::mbstate_t *pstate,
              wchar_t        *dst,
              const char     *src,
              std::size_t     size)
{
    const std::locale global;

    typedef std::codecvt<wchar_t, char, std::mbstate_t> CodeCvt;

    // retrieve the codecvt facet from the global locale
    const CodeCvt &cvt = std::use_facet<CodeCvt>(global);

    // use a small local buffer when dst is null and ignore size
    wchar_t buf [4];
    if (0 == dst) {
        dst  = buf;
        size = sizeof buf / sizeof *buf;
    }

    // set up pointers into the source sequence
    const char*       from      = src;
    const char* const from_end  = from + std::strlen (from);
    const char*       from_next = from;

    // set up pointers into the destination sequence
    wchar_t*       to     = dst;
    wchar_t* const to_end = to + size;
    wchar_t*       to_next;

    // number of non-NUL wide characters stored in destination buffer
    std::size_t nconv = 0;

    // use a local state when pstate is null (i.e., emulate mbstowcs)
    std::mbstate_t state = std::mbstate_t ();
    if (0 == pstate)
        pstate = &state;

    for ( ; from_next != from_end && to != to_end;
          from = from_next, to = dst == buf ? dst : to_next) {

        // convert a (sub)sequence of the source buffer into
        // the destination buffer
        const std::codecvt_base::result res =
            cvt.in (*pstate,
                    from, from_end, from_next,
                    to, to_end, to_next);

        // verify the consistency of the xxx_next pointers
        assert (from <= from_next && from_next <= from_end);
        assert (to <= to_next && to_next <= to_end);

        // process conversion result
        switch (res) {

        case std::codecvt_base::error:
            // conversion error
            errno = EILSEQ;
            return std::size_t (-1);

        case std::codecvt_base::noconv:
            // only codecvt<T, T> (i.e., facets where intern_type and
            // extern_type are identical) is allowed to return noconv
            // treat this case as an error even though it indicates
            // a bad (incorrectly implemented) codecvt facet
            return std::size_t (-1);

        case std::codecvt_base::partial:
            // partial conversion (incomplete character or not enough
            // room in destination buffer to convert the entire source
            // sequence)
            if (dst != buf || std::size_t (to_next - to) < size) {
                errno = EILSEQ;
                return std::size_t (-1);
            }

            nconv += to_next - to;
            break;

        case std::codecvt_base::ok:
            // complete conversion of an initial subsequence (but not
            // necessarily all) of the source buffer
            nconv += to_next - to;

            if (dst == buf && from_next == from_end)
                return nconv;

            break;
        }
    }

    return nconv;
}


int main ()
{
    static const char* const mbs [] = {
        "a", "abc",
        // <U0391>: Greek letter Alpha
        "\xce\x91",
        // <U0391><U0392>: Greek letters Alpha Beta
        "\xce\x91\xce\x92",
        // <U0391><U0392><U0393>: Greek letters Alpha Beta Gamma
        "\xce\x91\xce\x92\xce\x93",
        // <U0966>: Devangari digit 0
        "\xe0\xa5\xa6",
        // <U0967><U0966>: Devangari digits 10
        "\xe0\xa5\xa7\xe0\xa5\xa6",
        // <U0968><U0967><U0966>: Devangari digits 210
        "\xe0\xa5\xa8\xe0\xa5\xa7\xe0\xa5\xa6"
    };

    typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> CodeCvt;

    // create a UCS/UTF-8 codecvt facet and install it in a locale
    const std::locale utf (std::locale (""), new CodeCvt ("UTF-8@UCS"));

    // set the global locale to use the UCS/UTF-8 codecvt facet
    std::locale::global (utf);

    // iterate over examples of UTF-8 sequences and output the wide
    // character sequence each converts to
    for (std::size_t i = 0; i != sizeof mbs / sizeof *mbs; ++i) {

        wchar_t *dst = 0;

        // initialize state to the initial shift state
        std::mbstate_t state = std::mbstate_t ();

        // obtain the length of the wide character sequence
        // corresponding to the multibyte source sequence,
        // not including the terminating NUL
        const std::size_t length =
            my_mbsrtowcs (&state, 0, mbs [i], std::size_t (-1));

        if (std::size_t (-1) == length) {
            std::cerr << "Error computing length of destination sequence.\n";
            continue;
        }

        // allocate a wide character buffer large enough to hold
        // the converted sequence including the terminating NUL
        dst = new wchar_t [length + 1];

        // reset state to the initial shift state
        state = std::mbstate_t ();

        // convert the narrow character source sequence into
        // the wide character buffer
        const std::size_t nconv =
            my_mbsrtowcs (&state, dst, mbs [i], length + 1);

        if (length != nconv) {
            std::cerr << "Error converting source sequence.\n";
            continue;
        }

        // NUL-terminate the converted string
        dst [nconv] = L'\0';

        // write out the wide and the narrow sequences
        std::cout << "UCS-2 (" << std::dec << length << "): " << std::hex;

        for (const wchar_t *pwc = dst; *pwc != L'\0'; ++pwc)
            std::cout << "U+" << unsigned (*pwc) << ' ';

        std::cout << " ==> UTF-8: ";

        typedef unsigned char UChar;

        for (const char *pc = mbs [i]; *pc; ++pc)
            std::cout << "\\x" << int (UChar (*pc));

        std::cout << "\"\n";

        delete[] dst;
    }

    return 0;
}
