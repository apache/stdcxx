/***************************************************************************
 *
 * aliases.cpp
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

#ifdef _RWSTD_EDG_ECCP
   // disable error #450-D: the type "long long" is nonstandard
   // issued for uses of the type in Linux system headers (e.g.,
   // pthreadtypes.h)
#  pragma diag_suppress 450
#endif   // vanilla EDG eccp demo

#ifdef __linux__
   // on Linux define _XOPEN_SOURCE to get CODESET defined in <langinfo.h>
#  define _XOPEN_SOURCE 500   /* SUS conformance */
#  include <sys/types.h>
#endif   // __linux__

#include "diagnostic.h"

#include <cassert>   // for assert()
#include <cerrno>    // for errno
#include <cstdlib>
#include <cstdio>
#include <cstring>   // for memcpy(), strlen()
#include <clocale>   // for setlocale()
#include <locale>    // for tolower()
#include <iostream>
#include <string>
#include <vector>


#ifndef _WIN32
#  ifndef _RWSTD_NO_NL_LANGINFO
#    include <langinfo.h>
#  endif
#endif  // _WIN32

#include "aliases.h"

#if __GNUG__ == 2 && __GNUC_MINOR__ == 96
#  include <unistd.h>     // for close()

// declare the POSIX (but not ANSI C) function mkstemp()
extern "C" int mkstemp (char*) _LIBC_THROWS ();

#endif   // gcc 2.96


struct alias_t
{
    const char* name;          // canonical locale name
    const char* aliases [8];   // known aliases for the name
};


// codesets aliases
static const alias_t codeset_aliases [] = {
    { "ANSI_X3.4-1968", { "ascii", "usascii", "us-ascii", "us_ascii", 0 } },
    { "arabic8",        { "arabic-8", "arabic_8", 0 } },
    // chinese-t <=> chinese traditional
    { "BIG5",           { "big5", "chinese-t", 0 } },
    { "BIG5-HKSCS",     { "hkbig5", 0 } },
    { "CP1251",         { 0 } },
    { "CP1255",         { 0 } },
    // !ujis is coming from Unix JIS!
    { "EUC-JP",         { "eucjp", "eucJP", "euc_jp", "euc", "ujis", 0 } },
    { "EUC-KR",         { "euckr", "eucKR", "euc_kr", 0 } },
    { "EUC-TW",         { "euctw", "eucTW", "euc_tw", "cns-11643-1992",
                         "cns11643", 0 } },
    { "GB18030",        { "gb18030", 0 } },
    // chinese-s <=> chinese simplified
    { "GB2312",         { "gb2312", "hp15CN", "chinese-s", 0 } },
    { "GBK",            { "gbk", 0 } },
    { "GEORGIAN-PS",    { 0 } },
    { "greek8",         { "greek-8", "greek_8", 0 } },
    { "hebrew8",        { "hebrew-8", "hebrew_8", 0 } },
    { "IS-620",         { 0 } },

    { "ISO-8859-1",
      { "ISO-88591", "ISO_8859-1", "ISO8859-1", "iso88591", "iso1", 0 } },
    { "ISO-8859-2",
      { "ISO-88592", "ISO_8859-2", "ISO8859-2", "iso88592", "iso2", 0 } },
    { "ISO-8859-3",
      { "ISO-88593", "ISO_8859-3", "ISO8859-3", "iso88593", "iso3", 0 } },
    { "ISO-8859-5",
      { "ISO-88595", "ISO_8859-5", "ISO8859-5", "iso88595", "iso5", 0 } },
    { "ISO-8859-6",
      { "ISO-88596", "ISO_8859-6", "ISO8859-6", "iso88596", "iso6", 0 } },
    { "ISO-8859-7",
      { "ISO-88597", "ISO_8859-7", "ISO8859-7", "iso88597", "iso7",
        "sun_eu_greek", 0 } },
    { "ISO-8859-8",
      { "ISO-88598", "ISO_8859-8", "ISO8859-8", "iso88598", "iso8", 0 } },
    { "ISO-8859-9",
      { "ISO-88599", "ISO_8859-9", "ISO8859-9", "iso88599", "iso9", 0 } },
    { "ISO-8859-13",
      { "ISO-885913", "ISO_8859-13", "ISO8859-13", "iso885913", "iso13", 0 } },
    { "ISO-8859-14",
      { "ISO-885914", "ISO_8859-14", "ISO8859-14", "iso885914", "iso14", 0 } },
    { "ISO-8859-15",
      { "ISO-885915", "ISO_8859-15", "ISO8859-15", "iso885915", "iso15", 0 } },

    { "KOI8-R",         { "koi8r", "koi8_r", 0 } },
    { "KOI8-T",         { "koi8t", "koi8_t", 0 } },
    { "KOI8-U",         { "koi8u", "koi8_u", 0 } },
    { "PCK",            { 0 } },
    { "roman8",         { "roman-8", "roman_8", 0 } },
    { "Shift_JIS",      { "shift-jis", "sjis", 0 } },
    { "TIS-620",        { "tis620", "tis620.2533", 0 } },
    { "UTF-8",          { "utf8", "utf_8", 0 } },
    { 0, { 0 } }
};

// locale names aliases
static const alias_t locale_aliases [] = {
    { "af_ZA",       { "afrikaans", 0 } },
    { "ar_AE",       { 0 } },
    { "ar_BH",       { 0 } },
    { "ar_DZ",       { 0 } },
    { "ar_EG",       { 0 } },
    { "ar_IN",       { 0 } },
    { "ar_IQ",       { 0 } },
    { "ar_JO",       { 0 } },
    { "ar_KW",       { 0 } },
    { "ar_LB",       { 0 } },
    { "ar_LY",       { 0 } },
    { "ar_MA",       { 0 } },
    { "ar_OM",       { 0 } },
    { "ar_QA",       { 0 } },
    { "ar_SA",       { 0 } },
    { "ar_SD",       { 0 } },
    { "ar_SY",       { 0 } },
    { "ar_TN",       { 0 } },
    { "ar_YE",       { 0 } },
    { "be_BY",       { 0 } },
    { "bg_BG",       { "bulgarian", 0 } },
    { "br_FR",       { "br", 0 } },
    { "bs_BA",       { "bs", 0 } },
    { "ca_ES",       { 0 } },
    { "ca_ES@euro",  { 0 } },
    { "cs_CZ",       { "czech", 0 } },
    { "cy_GB",       { "cy", 0 } },
    { "da_DK",       { "danish", 0 } },
    { "de_AT",       { "austrian", 0 } },
    { "de_AT@euro",  { 0 } },
    { "de_BE",       { 0 } },
    { "de_BE@euro",  { 0 } },
    { "de_CH",       { 0 } },
    { "de_DE",       { "deutsch", "german", 0 } },
    { "de_DE@euro",  { 0 } },
    { "de_LU",       { 0 } },
    { "de_LU@euro",  { 0 } },
    { "el_GR",       { "greek", 0 } },
    { "en_AU",       { 0 } },
    { "en_BW",       { 0 } },
    { "en_CA",       { 0 } },
    { "en_DK",       { 0 } },
    { "en_GB",       { 0 } },
    { "en_HK",       { 0 } },
    { "en_IE",       { 0 } },
    { "en_IE@euro",  { 0 } },
    { "en_IN",       { 0 } },
    { "en_NZ",       { 0 } },
    { "en_PH",       { 0 } },
    { "en_SG",       { 0 } },
    { "en_US",       { 0 } },
    { "en_ZA",       { 0 } },
    { "en_ZW",       { 0 } },
    { "es_AR",       { 0 } },
    { "es_BO",       { 0 } },
    { "es_CL",       { 0 } },
    { "es_CO",       { 0 } },
    { "es_CR",       { 0 } },
    { "es_DO",       { 0 } },
    { "es_EC",       { 0 } },
    { "es_ES",       { "spanish", 0 } },
    { "es_ES@euro",  { 0 } },
    { "es_GT",       { 0 } },
    { "es_HN",       { 0 } },
    { "es_MX",       { 0 } },
    { "es_NI",       { 0 } },
    { "es_PA",       { 0 } },
    { "es_PE",       { 0 } },
    { "es_PR",       { 0 } },
    { "es_PY",       { 0 } },
    { "es_SV",       { 0 } },
    { "es_US",       { 0 } },
    { "es_UY",       { 0 } },
    { "es_VE",       { 0 } },
    { "et_EE",       { "estonian", 0 } },
    { "eu_ES",       { 0 } },
    { "eu_ES@euro",  { 0 } },
    { "fa_IR",       { "fa", 0 } },
    { "fi_FI",       { "finnish", 0 } },
    { "fi_FI@euro",  { "fi", 0 } },
    { "fo_FO",       { "fo", 0 } },
    { "fr_BE",       { 0 } },
    { "fr_BE@euro",  { 0 } },
    { "fr_CA",       { "canadian", 0 } },
    { "fr_CH",       { 0 } },
    { "fr_FR",       { "french", 0 } },
    { "fr_FR@euro",  { 0 } },
    { "fr_LU",       { 0 } },
    { "fr_LU@euro",  { 0 } },
    { "ga_IE",       { 0 } },
    { "ga_IE@euro",  { 0 } },
    { "gl_ES",       { 0 } },
    { "gl_ES@euro",  { 0 } },
    { "gv_GB",       { 0 } },
    { "he_IL",       { "hebrew", 0 } },
    { "hi_IN",       { 0 } },
    { "hr_HR",       { "croatian", 0 } },
    { "hu_HU",       { "hungarian", 0 } },
    { "id_ID",       { 0 } },
    { "is_IS",       { 0 } },
    { "it_CH",       { 0 } },
    { "it_IT",       { "italian", 0 } },
    { "it_IT@euro",  { 0 } },
    { "iw_IL",       { 0 } },
    { "ja_JP",       { "ja", "japanese", 0 } },
    { "ka_GE",       { 0 } },
    { "kl_GL",       { 0 } },
    { "ko_KR",       { "ko", 0 } },
    { "kw_GB",       { 0 } },
    { "lt_LT",       { "lithuanian", 0 } },
    { "lv_LV",       { "latvian", 0 } },
    { "mi_NZ",       { 0 } },
    { "mk_MK",       { "macedonian", 0 } },
    { "mr_IN",       { 0 } },
    { "ms_MY",       { 0 } },
    { "mt_MT",       { 0 } },
    { "nl_BE",       { 0 } },
    { "nl_BE@euro",  { 0 } },
    { "nl_NL",       { "dutch", 0 } },
    { "nl_NL@euro",  { 0 } },
    { "nn_NO",       { 0 } },
    { "no_NO",       { "no", 0 } },
    { "oc_FR",       { "occitan", 0 } },
    { "pl_PL",       { "polish", 0 } },
    { "pt_BR",       { 0 } },
    { "pt_PT",       { "portuguese", 0 } },
    { "pt_PT@euro",  { 0 } },
    { "ro_RO",       { "romanian", 0 } },
    { "ru_RU",       { "russian", 0 } },
    { "ru_UA",       { 0 } },
    { "sk_SK",       { "slovakian", "slovak", 0 } },
    { "sl_SI",       { "slovenian", "sloven", 0 } },
    { "sq_AL",       { "albanian", 0 } },
    { "sr_YU",       { "serbian", 0 } },
    { "sr_YU@cyrillic", { 0 } },
    { "sv_FI",       { 0 } },
    { "sv_FI@euro",  { 0 } },
    { "sv_SE",       { 0 } },
    { "ta_IN",       { 0 } },
    { "te_IN",       { 0 } },
    { "tg_TJ",       { 0 } },
    { "th_TH",       { "thai", 0 } },
    { "tl_PH",       { 0 } },
    { "tr_TR",       { "turkish", 0 } },
    { "uk_UA",       { "ukrainian", 0 } },
    { "ur_PK",       { 0 } },
    { "uz_UZ",       { 0 } },
    { "vi_VN",       { 0 } },
    { "yi_US",       { 0 } },
    { "zh_TW",       { "zh", "chinese", 0 } },
    { 0, { 0 } }
};

#ifdef _WIN32

struct codepage_t
{
    const char*  name;       // standard codeset name
    unsigned int codepage;   // code page number
};

static const codepage_t codepages [] = {
    { "ANSI_X3.4-1968", 20127 },
    { "BIG5",             950 },
    { "GBK",              936 },
    { "GB2312",         20936 },
    { "ISO-8859-1",     28591 },
    { "ISO-8859-2",     28592 },
    { "ISO-8859-3",     28593 },
    { "ISO-8859-4",     28594 },
    { "ISO-8859-5",     28595 },
    { "ISO-8859-6",     28596 },
    { "ISO-8859-7",     28597 },
    { "ISO-8859-8",     28598 },
    { "ISO-8859-9",     28599 },
    { "ISO-8859-15",    28605 },
    { "KOI8-R",         20866 },
    { "KOI8-U",         21866 },
    { "Shift_JIS",        932 },
    { "UTF-8",          65001 },
    { 0, 0 }
};

#endif  // _WIN32

/****************************************************************************/
#ifdef _WIN32

unsigned int get_codepage (const std::string& cname)
{
    for (std::size_t i = 0; codepages [i].name; i++) {
        if (0 == ci_compare (codepages [i].name, cname.c_str ()))
            return codepages[i].codepage;
    }
    return 0;
}

#endif  // _WIN32

/*****************************************************************************/

static void store_aliases (StringVector& v, const alias_t &a)
{
    v.push_back (a.name);

    for (std::size_t i = 0; a.aliases [i]; i++)
        v.push_back (a.aliases [i]);
}

/*****************************************************************************/

// case insensitive comparison of initial parts of the strings
static int ci_pre_compare (const std::string& s1, const std::string& s2)
{
    std::string::const_iterator it1 = s1.begin ();
    std::string::const_iterator it2 = s2.begin ();
    for (; it1 != s1.end () && it2 != s2.end (); it1++, it2++)
        if (   (std::tolower)(*it1, std::locale ())
            != (std::tolower)(*it2, std::locale ()))
            return (*it1 - *it2);

    return 0;
}


int ci_compare (const char *s1, const char *s2)
{
    assert (0 != s1);
    assert (0 != s2);

    typedef unsigned char UChar;

    for ( ; ; ++s1, ++s2) {
        if (*s1 != *s2)
            return UChar (*s1) - UChar (*s2);

        if ('\0' == *s1)
            break;
    }

    return 0;
}

// case insensitive comparison between strings
int ci_compare (const std::string& s1, const std::string& s2)
{
    return ci_compare (s1.c_str (), s2.c_str ());
}


void get_cname_aliases (const std::string& name,
                        StringVector& aliases)
{
    for (std::size_t i = 0; codeset_aliases [i].name; i++) {
        if (0 == ci_compare (codeset_aliases [i].name, name.c_str ())) {
            // store the whole structure data
            store_aliases (aliases, codeset_aliases [i]);
            break;
        }

        // check the entry's aliases as well
        for (std::size_t j = 0; codeset_aliases [i].aliases [j]; j++) {
            if (0 == ci_compare (codeset_aliases [i].aliases [j],
                                 name.c_str ())) {
                // store the whole structure data
                store_aliases (aliases, codeset_aliases [i]);
                break;
            }
        }
    }
 
    if (aliases.empty ())
        aliases.push_back (name);
}

#ifndef _WIN32

void get_lname_aliases (const std::string& name,
                        StringVector& aliases)
{
    for (std::size_t i = 0; locale_aliases [i].name; i++) {
        if (0 == ci_compare (locale_aliases [i].name, name.c_str ())) {
            // store the whole structure data
            store_aliases (aliases, locale_aliases [i]);
            break;
        }

        // check the entry's aliases as well
        for (std::size_t j = 0; locale_aliases [i].aliases [j]; j++) {
            if (0 == ci_compare (locale_aliases [i].aliases [j],
                                 name.c_str ())) {
                // store the whole structure data
                store_aliases (aliases, locale_aliases [i]);
                break;
            }
        }
    }

    if (aliases.empty ())
        aliases.push_back (name);
}


void get_same_encoding_C_locale (const std::string &lname,
                                 const std::string &cname,
                                 StringVector      &libc_locales)
{
    // get the aliases for both name and codeset
    StringVector la;
    StringVector ca;

    get_lname_aliases (lname, la);
    get_cname_aliases (cname, ca);

    // find all the installed C library locales that are equivalent
    // to the locale named by `lname' and that use the encoding given
    // by `cname'

    char* locname = get_installed_locales ();

    for (; *locname; locname += std::strlen (locname) + 1) {

        bool match_found = false;

        typedef StringVector::iterator Iterator;

        // iterate through the aliases and see if any one of them
        // starts with the same sequence
        for (Iterator it = la.begin (); it != la.end (); ++it) {

            if (0 != ci_pre_compare (locname, (*it).c_str ()))
                continue;

            // this is a good match, test to see if the encodings match
            if (0 == std::setlocale (LC_CTYPE, locname))
                continue;

            const char* const cs = nl_langinfo (CODESET);

            // compare the codeset to the aliases
            for (Iterator it1 = ca.begin (); it1 != ca.end (); ++it1) {
                if (0 != ci_compare (cs, (*it1).c_str ()))
                    continue;

                // found a match and save it
                libc_locales.push_back (std::string (locname));
                match_found = true;
            }

            if (match_found)
                break;
        }
    }
}


std::string get_C_encoding_locale (const std::string &codeset)
{
    // retrieve the set of aliases for this codeset
    StringVector ca;
    get_cname_aliases (codeset, ca);

    // find a C library locale that uses the same encoding
    char* locname = get_installed_locales ();

    for (; *locname; locname += std::strlen (locname) + 1) {

        // set the C locale and get the codeset name
        if (0 == std::setlocale (LC_CTYPE, locname))
            continue;

#ifdef _RWSTD_OS_HP_UX

        // skip the limited C.utf8 locale 
        if (0 == std::strcmp ("C.utf8", locname))
            continue;

#endif   // _RWSTD_OS_HPUX

        
        const char* const cs = nl_langinfo (CODESET);

        typedef StringVector::iterator Iterator;

        // compare the name with this codeset's aliases
        for (Iterator it = ca.begin (); it != ca.end (); ++it)
            if (0 == ci_compare (cs, (*it).c_str ())) {
                return locname;
            }
    }

    return std::string ();
}


// returns a character array consisting of NUL-separated names
// of locales intalled on a system; if `loc_cat' is other than
// LC_INVALID_CAT, will eliminate names that do not refer to
// valid locales (i.e., those for which setlocale(loc_cat,
// name) will return 0
char* get_installed_locales (int loc_cat /* = LC_INVALID_CAT */)
{
    static char* slocname = 0;

    static std::size_t size       = 0;      // number of elements in array
    static std::size_t total_size = 5120;   // the size of the array

    // allocate first time through
    if (!slocname) {
        slocname = new char [16384];
        *slocname = '\0';
    }

    char* locname = slocname;

    // save the current locale setting and set the locale to "C"
    const char* const save_localename = std::setlocale (LC_ALL, 0);
    std::setlocale (LC_ALL, "C");

#if __GNUG__ == 2 && __GNUC_MINOR__ == 96

    // create a temporary file for the output of `locale -a'
    char fname_buf [L_tmpnam] = "/tmp/tmpfile-XXXXXX";
    const char *fname = fname_buf;

    // avoid using tmpnam() to prevent the bogus gcc 2.96 warning:
    //     the use of `tmpnam' is dangerous, better use `mkstemp'
    const int fd = mkstemp (fname_buf);

    if (-1 == fd) {
        std::perror ("mkstemp() failed");
        std::abort();
    }

    close (fd);

#else   // if !defined (gcc 2.96)

    // create a temporary file for the output of `locale -a'
    char fname_buf [L_tmpnam];
    const char *fname = std::tmpnam (fname_buf);

#endif   // gcc 2.96

    if (!fname) {
        std::perror ("tmpnam() failed");
        std::abort ();
    }

    // create a shell command and redirect its output into the file

    // cmd must be at least this large:
    //   sizeof ("locale -a | grep \"\" > ")   // 22
    // + strlen (fname)                        // must be <= L_tmpnam

    char cmd [80 + L_tmpnam];

    std::sprintf (cmd, "LC_ALL=C /usr/bin/locale -a >%s 2>/dev/null", fname);

    const int ret = std::system (cmd);

    if (ret)
        issue_diag (W_NOTSUP, false, 0, "call to system(\"%s\") failed: %s\n",
                    cmd, std::strerror (errno));

    // open file containing the list of installed locales
    std::FILE *f = std::fopen (fname, "r");

    if (f) {
        // even simple locale names can be very long (e.g., on HP-UX,
        // where a locale name always consists of the names of all
        // categories, such as "C C C C C C")
        char last_name [256];
        *last_name = '\0';

        // if successful, construct a char array with the locales
        while (std::fgets (cmd, int (sizeof cmd), f)) {
            cmd [std::strlen (cmd) - 1] = '\0';

            // if our buffer is full then dynamically allocate a new one
            if ((size += (std::strlen (cmd) + 1)) > total_size) {
                total_size += 5120;
                char* newBuf = new char[total_size];
                std::memcpy (newBuf, slocname, total_size - 5120);
                std::free (slocname);
                slocname = newBuf;
                locname = slocname + size - std::strlen (cmd) - 1;
            }

#ifdef _WIN64

            // prevent a hang (OS/libc bug?)
            std::strcpy (locname, cmd);
            locname += std::strlen (cmd) + 1;

#else

            if (loc_cat != int (LC_INVALID_CAT)) {

                // set the C locale to verify that the name is valid
                const char *name = std::setlocale (loc_cat, cmd);

                // if it is and if the actual locale name different
                // from the last one, append it to the list
                if (name && std::strcmp (last_name, name)) {
                    std::strcpy (locname, cmd);
                    locname += std::strlen (cmd) + 1;

                    // save the last locale name
                    assert (std::strlen (name) < sizeof last_name);
                    std::strcpy (last_name, name);
                }
            }
            else {
                std::strcpy (locname, cmd);
                locname += std::strlen (cmd) + 1;
            }

#endif   // _WIN64

        }
        *locname = '\0';

    }

    // restore the original locale
    if (save_localename)
        std::setlocale (LC_ALL, save_localename);

    std::fclose (f);
    std::remove (fname);
    return slocname;
}

#endif   // _WIN32
