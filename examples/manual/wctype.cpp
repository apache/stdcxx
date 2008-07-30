/**************************************************************************
 *
 * wctype.cpp - Example program showcasing the wchar_t specialization
 *              of the std::ctype_byname facet, inspired by a Sun C++
 *              forum post:
 *              http://forums.sun.com/thread.jspa?threadID=5315069
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
 * Copyright 2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <cwctype>     // for iswxxx(), wint_t
#include <stdexcept>   // for runtime_error
#include <iomanip>     // for setfill, setw
#include <ios>         // for oct
#include <iostream>    // for cerr, cout
#include <locale>      // for isxxx(), locale


static std::locale
make_named_locale ()
{
    static const char* const
    locale_names [] = {
        "es_ES.ISO8859-1",       // AIX, Solaris, Tru64
        "es_ES.iso88591",        // HP-UX, Linux
        "es_ES.88591",
        "De_DE.88591",           // Reliant
        "es_ES",
        "es",                    // Linux, Solaris
        "Spanish",
        "spanish",               // Linux
        "espanol",               // Linux
        "spanish_spain.1252",    // Windows
        "POSIX",                 // POSIX systems
        "C",                     // all C/C++ systems
        0                        // (sentinel)
    };

    std::locale german;

    // iterate over the know locale names above until a valid one
    // is found (i.e., one that doesn't cause locale to throw)
    for (const char* const *names = locale_names; ; ) {
        try {
            german = std::locale (names [0]);
            break;
        }
        catch (std::runtime_error&) {
            // continue trying until the next name is null
            if (0 == *++names)
                throw;
        }
        catch (...) {
            throw;
        }
    }

    return german;
}


int main ()
{
    std::locale locale;

    try {
        // try to contruct a named locale
        locale = make_named_locale ();
    }
    catch (std::runtime_error& e) {
        // a runtime_error will be thrown if the locale cannot be constructed
        std::cerr << "Caught runtime_error:\n"; 
        std::cerr << e.what () << '\n';

        return 1;
    }
    catch (...) {
        std::cerr << "Caught an unknown exception\n"; 

        return 2;
    }

    // set the global C/C++ locale to be used by the C classification
    // functions (such as iswalpha())
    std::locale::global (locale);

    // imbue the named locale in wcout
    std::wcout.imbue (locale);

    std::wcout << "Wide character classification in "
               << locale.name ().c_str () << " locale.\n";


    // number of mismatched classifications between C and C++
    // (expect zero for 100% conforming implementation)
    int mismatch_count = 0;

    // iterate over all characters in the extended ASCII range
    // printing out the value of each along with its character
    // class using the letters A, a, C, D, G, L, P, p, S, U,
    // and X to denote each of the C/C++ classes corresponding
    // to the C and C++ classification functions
    for (std::wint_t wi = 0; wi < std::wint_t (256); ++wi) {

        // convert the wint_t to wchar_t
        const wchar_t wc (wi);

        char cxx_class [11];   // C++ classification letters
        char c_class   [11];   // C classification letters

        std::size_t i = 0;

        // assign the appropriate letter for the C++ classification
        cxx_class [i++] = (std::isalnum)(wc, locale)  ? 'A' : '-';
        cxx_class [i++] = (std::isalpha)(wc, locale)  ? 'a' : '-';
        cxx_class [i++] = (std::iscntrl)(wc, locale)  ? 'C' : '-';
        cxx_class [i++] = (std::isdigit)(wc, locale)  ? 'D' : '-';
        cxx_class [i++] = (std::isgraph)(wc, locale)  ? 'G' : '-';
        cxx_class [i++] = (std::islower)(wc, locale)  ? 'L' : '-';
        cxx_class [i++] = (std::isprint)(wc, locale)  ? 'P' : '-';
        cxx_class [i++] = (std::ispunct)(wc, locale)  ? 'p' : '-';
        cxx_class [i++] = (std::isspace)(wc, locale)  ? 'S' : '-';
        cxx_class [i++] = (std::isupper)(wc, locale)  ? 'U' : '-';
        cxx_class [i++] = (std::isxdigit)(wc, locale) ? 'X' : '-';

        i = 0;

        // assign the appropriate letter for the C classification
        c_class [i++] = (std::iswalnum)(wc)  ? 'A' : '-';
        c_class [i++] = (std::iswalpha)(wc)  ? 'a' : '-';
        c_class [i++] = (std::iswcntrl)(wc)  ? 'C' : '-';
        c_class [i++] = (std::iswdigit)(wc)  ? 'D' : '-';
        c_class [i++] = (std::iswgraph)(wc)  ? 'G' : '-';
        c_class [i++] = (std::iswlower)(wc)  ? 'L' : '-';
        c_class [i++] = (std::iswprint)(wc)  ? 'P' : '-';
        c_class [i++] = (std::iswpunct)(wc)  ? 'p' : '-';
        c_class [i++] = (std::iswspace)(wc)  ? 'S' : '-';
        c_class [i++] = (std::iswupper)(wc)  ? 'U' : '-';
        c_class [i++] = (std::iswxdigit)(wc) ? 'X' : '-';

        // print the value of the character and its glyph only when
        // it's printable, otherwise print a space
        std::wcout << "  '\\" << std::setw (3)
                   << std::setfill (L'0') << std::oct << wi
                   << "' ('" << ((std::isprint)(wc, locale) ? wc : L' ')
                   << "')   ";

        bool mismatch = false;

        // print out the letters denoting each character class
        for (i = 0; i != sizeof cxx_class; ++i) {

            if (cxx_class [i] == c_class [i])
                std::wcout << cxx_class [i] << ' ';
            else {
                std::wcout << cxx_class [i] << ':' << c_class [i] << ' ';
                mismatch = true;
            }
        }

        if (mismatch) {
            std::wcout << "   *** C/C++ mismatch ***";

            // increment the number of mimatched character classes
            ++mismatch_count;
        }

        std::wcout << '\n';
    }

    return mismatch_count ? 1 : 0;
}
