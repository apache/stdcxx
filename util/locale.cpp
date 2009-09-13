/***************************************************************************
 *
 * locale.cpp
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

#if defined (__linux__) && !defined (_XOPEN_SOURCE) 
   // on Linux define _XOPEN_SOURCE to get CODESET defined in <langinfo.h>
#  define _XOPEN_SOURCE 500   /* Single UNIX conformance */
#endif   // __linux__

#ifdef _RWSTD_EDG_ECCP
   // disable error #450-D: the type "long long" is nonstandard
   // issued for uses of the type in Linux system headers (e.g.,
   // pthreadtypes.h)
#  pragma diag_suppress 450
#endif   // vanilla EDG eccp demo

#include _RWSTD_SYS_TYPES_H

#if _RWSTD_PATH_SEP == '/'
#  define SLASH      "/"
#  define LS_1       "ls -1 "
#else
#  define SLASH      "\\"
#  define LS_1       "dir /B /A:D "
#endif

#ifndef _WIN32
#  include <sys/mman.h>   // for mmap()
#  include <unistd.h>     // for close ()
#  ifndef _RWSTD_NO_ICONV
#    include <iconv.h>
#  endif
#  ifndef _RWSTD_NO_NL_LANGINFO
#    include <langinfo.h>
#  endif
#else
#  include <io.h>         // for open()
#  include <windows.h>
#endif  // _WIN32

#include <limits.h>      // for INT_MAX, INT_MIN
#include <sys/stat.h>
#include <fcntl.h>

#include <locale>
#include <map>           // for map
#include <vector>        // for vector
#include <iostream>      // for cerr, cout
#include <iomanip>
#include <fstream>       // for fstream

// #include <cerrno>
#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include <loc/_localedef.h>

#include "aliases.h"
#include "charmap.h"
#include "loc_exception.h"
#include "memchk.h"
#include "diagnostic.h"

/**************************************************************************/

#ifndef LC_MESSAGES
#  ifdef _RWSTD_LC_MESSAGES
#    define LC_MESSAGES   _RWSTD_LC_MESSAGES
#  else
#    define LC_MESSAGES   -1
#  endif   // _RWSTD_LC_MESSAGES
#endif   // LC_MESSAGES


// set to true in response to the "-c" command line option requesting
// locale to print the name of each section before printing out its
// contents
static bool print_sect_names = false;

// set to true in response to the "-k" command line option requesting
// locale to print the name of each keyword before printing out its
// value
static bool print_keywords = false;

// set to true in response to the "-h" command line option requesting
// locale to try to use character names from the original charmap used
// to create the locale (if possible)
static bool decode = false;

// set to false in response to the "-l" command line option requesting
// locale to produce output suitable for processing by the localedef
// utility
static bool posix_output = true;

// set to true in response to the "-p" command line option requesting
// locale to produce output using the symbols from the POSIX Portable
// Character Set
static bool use_pcs = false;

static bool is_utf8 = true;


static const _RW::__rw_codecvt_t*  codecvt_st   = 0;
static const _RW::__rw_collate_t*  collate_st   = 0;
static const _RW::__rw_ctype_t*    ctype_st     = 0;
static const _RW::__rw_time_t*     time_st      = 0;
static const _RW::__rw_num_t*      num_st       = 0;
static const _RW::__rw_mon_t*      mon_st       = 0;
static const _RW::__rw_punct_t*    num_punct_st = 0;
static const _RW::__rw_punct_t*    mon_punct_st = 0;
static const _RW::__rw_messages_t* messages_st  = 0;


static const char sect_lc_all[]      = "LC_ALL";
static const char sect_lc_collate[]  = "LC_COLLATE";
static const char sect_lc_ctype[]    = "LC_CTYPE";
static const char sect_lc_messages[] = "LC_MESSAGES";
static const char sect_lc_monetary[] = "LC_MONETARY";
static const char sect_lc_numeric[]  = "LC_NUMERIC";
static const char sect_lc_time[]     = "LC_TIME";

// length of the largest string you want printed
#define MAX_LINE_LEN 50

// for convenience
typedef unsigned char UChar;

/**************************************************************************/

int validate (const void *addr, std::size_t nbytes)
{
    const std::size_t size = memchk (addr, nbytes);

    if (nbytes != size) {
        if (size)
            issue_diag (E_INVAL, true, 0,
                        "invalid size at address %p: %u, expected %u\n",
                        addr, size, nbytes);
        else
            issue_diag (E_INVAL, true, 0,
                        "invalid address: %p (%u bytes)\n", addr, nbytes);
            
        return -1;
    }

    return 0;
}


int validate (const char *str)
{
    if (std::size_t (-1) == strchk (str)) {

        const std::size_t first_byte = memchk (str, 1);

        if (1 == first_byte)
            issue_diag (E_INVAL, true, 0,
                        "invalid string address: %p\n", str);
        else
            issue_diag (E_INVAL, true, 0,
                        "invalid string at %p: no terminating NUL\n", str);
        
        return -1;
    }

    return 0;
}


// initialize the LC_CTYPE, LC_MONETARY, LC_COLLATE, LC_NUMERIC and LC_TIME
// structures by finding the locale database and mmaping the file to the
// structure.
// Note: if the locale is not found no error msg is printed, instead we
// silently use the hardcoded 'POSIX' locale

const void*
init_struct (const std::string &loc_path_root,
             const char *cat_name, const char *codeset_name)
{
    // check to see if the LC_ALL or LC_LANG environment variable
    // was set, if LC_LANG was not set it defaults to "C"
    static const char* const all_val = std::getenv ("LC_ALL");
    static const char*       lang_val   = std::getenv ("LANG");
    if (0 == lang_val)
        lang_val = "C";

    // if LC_ALL is set, use that value for this locale
    // otherwise, find out what the environment value is for this category
    // is and if it is not set use the value in LC_LANG
    const char* env_val = all_val;
    if (!all_val || '\0' == *all_val) {
        env_val = cat_name ? std::getenv (cat_name) : "";
        if (!env_val)
            env_val = lang_val;
    }

    // if env_val is POSIX or C then return NULL otherwise
    // try to map the locale category database into memory
    if (std::strcmp (env_val, "C") && std::strcmp (env_val, "POSIX")) {

        // construct the path to the locale database file
        std::string loc_path;

        // if the LC_XXX environment variable contains a slash
        // it is treated as a pathname (possibly relative to
        // the current working directory, when the slash is not
        // the first character), otherwise as a filename residing
        // under `loc_path_root'
        const char* const slash = std::strrchr (env_val, _RWSTD_PATH_SEP);
        if (!slash)
            loc_path = loc_path_root;

        if (codeset_name) {
            if (slash)
                loc_path = std::string (env_val, slash - env_val);

            (loc_path += _RWSTD_PATH_SEP) += codeset_name;
        }
        else
            ((loc_path += env_val) += _RWSTD_PATH_SEP) += cat_name;

        struct stat st;
        const int fd = open (loc_path.c_str (), O_RDONLY);

        // get the size of the file by using fstat
        if (fd != -1 && 0 == fstat (fd, &st)) {
            // map the file to a pointer and if it succeeds
            // return the pointer, otherwise return 0

#ifndef _WIN32
            void* const ret = mmap (0, st.st_size, PROT_READ | PROT_WRITE,
                                    MAP_PRIVATE, fd, 0);
            if (MAP_FAILED == ret)
                return 0;

#else
            HANDLE file = CreateFile (loc_path.c_str (), GENERIC_READ,
                                      FILE_SHARE_READ, 0,
                                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                      NULL);

            HANDLE mapping = CreateFileMapping (file, 0, PAGE_READONLY,
                                                0, 0, 0);

            void* const ret = MapViewOfFile (mapping, FILE_MAP_READ, 0,
                                             0, st.st_size);

#endif  // _WIN32

            return ret;
        }
    }

    return 0;
}


template <class FacetData>
const FacetData*
init_section (const std::string &loc_path_root,
              const char *cat_name, const char *codeset_name,
              FacetData*)
{
    const void* const ptr =
        init_struct (loc_path_root, cat_name, codeset_name);

    if (ptr && 0 == validate (ptr, sizeof (FacetData)))
        return static_cast<const FacetData*>(ptr);

    return 0;
}


static void
init_sections ()
{
    std::string loc_path_root;

    // if the locale root environment is not set
    // use the current directory as the locale root
    const char* const root = std::getenv ("RWSTD_LOCALE_ROOT");

    loc_path_root  = root ? root : ".";
    loc_path_root += _RWSTD_PATH_SEP;

    // points to the name of the codeset used by all non-empty
    // locale categories, otherwise 0
    const char *codeset = 0;

    // points to the name of the LC_XXX environment variable that refers
    // to the collate category whose codeset is described by codeset;
    // the value of the variable is used to determine the location of
    // the codeset conversion database file, e.g., when the environment
    // variable RWSTD_LOCALE_ROOT is not defined and locale is invoked
    // like so:
    //   LC_CTYPE=/foo/bar/somelocale locale --charmap
    const char *section = 0;

    // initialize collate_st
    collate_st = init_section (loc_path_root, sect_lc_collate, 0,
                               (_RW::__rw_collate_t*)0);

    if (collate_st) {
        validate (collate_st->codeset_name ());

        codeset = collate_st->codeset_name ();
        section = sect_lc_collate;
    }

    // initialize ctype_st
    ctype_st = init_section (loc_path_root, sect_lc_ctype, 0,
                           (_RW::__rw_ctype_t*)0);

    if (ctype_st) {
        validate (ctype_st->codeset_name ());

        if (codeset) {
            if (std::strcmp (codeset, ctype_st->codeset_name ()))
                codeset = 0;
        }
        else {
            codeset = ctype_st->codeset_name ();
            section = sect_lc_ctype;
        }
    }

    // initialize mon_st
    mon_punct_st = init_section (loc_path_root, sect_lc_monetary, 0,
                               (_RW::__rw_punct_t*)0);

    if (0 != mon_punct_st) {
        mon_st = _RWSTD_STATIC_CAST (const _RW::__rw_mon_t*,
                                     mon_punct_st->get_ext());
        validate (mon_st, sizeof *mon_st);
    }

    if (mon_st) {
        validate (mon_st->codeset_name ());

        if (codeset) {
            if (std::strcmp (codeset, mon_st->codeset_name ()))
                codeset = 0;
        }
        else {
            codeset = mon_st->codeset_name ();
            section = sect_lc_monetary;
        }
    }

    // initialize num_st
    num_punct_st = init_section (loc_path_root, sect_lc_numeric, 0,
                               (_RW::__rw_punct_t*)0);
    if (0 != num_punct_st) {
        num_st = _RWSTD_STATIC_CAST (const _RW::__rw_num_t*,
                                     num_punct_st->get_ext());
        validate (num_st, sizeof *mon_st);
    }

    if (num_st) {
        validate (num_st->codeset_name ());

        if (codeset) {
            if (std::strcmp (codeset, num_st->codeset_name ()))
                codeset = 0;
        }
        else {
            codeset = num_st->codeset_name ();
            section = sect_lc_numeric;
        }
    }

    // initialize time_st
    time_st = init_section (loc_path_root, sect_lc_time, 0,
                            (_RW::__rw_time_t*)0);
    if (time_st) {
        validate (time_st->codeset_name ());

        if (codeset) {
            if (std::strcmp (codeset, time_st->codeset_name ()))
                codeset = 0;
        }
        else {
            codeset = time_st->codeset_name ();
            section = sect_lc_time;
        }
    }

    // initialize messages_st
    messages_st = init_section (loc_path_root, sect_lc_messages, 0,
                              (_RW::__rw_messages_t*)0);

    if (messages_st) {
        validate (messages_st->codeset_name ());

        if (codeset) {
            if (std::strcmp (codeset, messages_st->codeset_name ()))
                codeset = 0;
        }
        else {
            codeset = messages_st->codeset_name ();
            section = sect_lc_messages;
        }
    }

    if (codeset) {
        // initialize codecvt_st
        codecvt_st = init_section (loc_path_root, section, codeset,
                                   (_RW::__rw_codecvt_t*)0);
    }
}


static const char*
set_locale (int lc_cat, const char *locname, const std::string &charmap_name)
{
#ifndef _WIN32

    assert (0 != locname);

    locname = std::setlocale (lc_cat, locname);

    if (locname)
        return locname;

    std::string encoding (charmap_name.substr (0, charmap_name.rfind ('.')));

    std::vector<std::string> aliases;
    get_cname_aliases (encoding, aliases);

    std::vector<std::string>::iterator pos;

    for (pos = aliases.begin(); !is_utf8 && pos != aliases.end(); pos++)
        if (*pos == "utf8")
            is_utf8 = true;

    const char* names = get_installed_locales ();

    for (; *names; names += std::strlen (names) + 1) {

        if (0 != std::setlocale (LC_CTYPE, names)) {

            const char* const codeset = nl_langinfo (CODESET);

            for (pos = aliases.begin (); pos != aliases.end (); ++pos)
                if (codeset == *pos)
                    return names;
        }
    }

#endif  // _WIN32

    return 0;
}


/**************************************************************************/

static const Charmap*
get_charmap (int lc_cat)
{
    if (!decode)
        return 0;

    static const char* const lang        = std::getenv ("LANG");
    static const char* const lc_all      = std::getenv (sect_lc_all);
    static const char* const lc_collate  = std::getenv (sect_lc_collate);
    static const char* const lc_ctype    = std::getenv (sect_lc_ctype);
    static const char* const lc_messages = std::getenv (sect_lc_messages);
    static const char* const lc_monetary = std::getenv (sect_lc_monetary);
    static const char* const lc_numeric  = std::getenv (sect_lc_numeric);
    static const char* const lc_time     = std::getenv (sect_lc_time);

    static const Charmap* cmaps [6];
    static bool           cmaps_tried [sizeof cmaps / sizeof *cmaps];

    const char* charmap = 0;
    const char* locale  = 0;

    const Charmap** cmap = 0;

    switch (lc_cat) {

    case LC_COLLATE:
        charmap = collate_st ? collate_st->charmap_name () : "";
        locale  = lc_collate;
        cmap    = cmaps + 0;
        break;

    case LC_CTYPE:
        charmap = ctype_st ? ctype_st->charmap_name () : "";
        locale  = lc_ctype;
        cmap    = cmaps + 1;
        break;

    case LC_MESSAGES:
        charmap = messages_st ? messages_st->charmap_name () : "";
        locale  = lc_messages;
        cmap    = cmaps + 2;
        break;

    case LC_MONETARY:
        charmap = mon_st ? mon_st->charmap_name () : "";
        locale  = lc_monetary;
        cmap    = cmaps + 3;
        break;

    case LC_NUMERIC:
        charmap = num_st ? num_st->charmap_name () : "";
        locale  = lc_numeric;
        cmap    = cmaps + 4;
        break;

    case LC_TIME:
        charmap = time_st ? time_st->charmap_name () : "";
        locale  = lc_time;
        cmap    = cmaps + 5;
        break;

    default: assert (!"bad LC_XXX constant");
    }

    assert (0 != cmap);
    assert (0 != charmap);

    if (*cmap && (*cmap)->get_charmap_name () == charmap)
        return *cmap;

    if (cmaps_tried [cmap - cmaps])
        return *cmap;

    cmaps_tried [cmap - cmaps] = true;

    // LC_ALL overrides any other LC_XXX setting
    if (lc_all && *lc_all)
        locale = lc_all;

    // when neither LC_ALL or LC_XX is specified fall back on LANG
    if (0 == locale)
        locale = lang ? lang : "POSIX";

    set_locale (lc_cat, locale, charmap);

    // look for the charmap directory
    static const char* const src_root = std::getenv ("RWSTD_SRC_ROOT");

    if (0 == src_root) {
        static int warning_issued = 0;
        if (0 == warning_issued++)
            issue_diag (W_CHARMAP, false, 0, "RWSTD_SRC_ROOT not set\n");
    }
    else if ('\0' == *charmap) {
        static int warning_issued = 0;
        if (0 == warning_issued++)
            issue_diag (W_CHARMAP, false, 0, "no charmap name\n");
    }
    else {
        std::string charmap_path = src_root;

        charmap_path += _RWSTD_PATH_SEP;
        charmap_path += "charmaps";
        charmap_path += _RWSTD_PATH_SEP;
        charmap_path += charmap;

        // search for a C library locale that uses the same encoding
        std::string std_encoding (charmap);

# if !defined (_WIN32)
        std::string C_locale (get_C_encoding_locale (std_encoding));
# else
        std::string C_locale ("");
# endif

        *cmap = new Charmap (C_locale.c_str (), charmap_path.c_str(),
                             0 == std::strcmp ("utf8.cm", charmap),
                             false, true, false);
    }

    return cmap ? *cmap : 0;
}


static std::string
escape_value (unsigned val, bool always = false)
{
    // set of printable ASCII characters
    static const char printable [] = {
        //        ................
        //        0123456789abcdef
        /* 0. */ "                "
        /* 1. */ "                "
        /* 2. */ " ! #$%''()*+,-./"
        /* 3. */ "0123456789: <=>?"
        /* 4. */ "@ABCDEFGHIJKLMNO"
        /* 5. */ "PQRSTUVWXYZ[ ]^_"
        /* 6. */ "`abcdefghijklmno"
        /* 7. */ "pqrstuvwxyz{|}~ "
    };

    static const char hexdigit[] = "0123456789abcdef";

    std::string ret;

    if (   (UChar (';') == val || UChar ('"') == val || UChar ('\\') == val)
        && !always) {
        ret += '\\';
        ret += char (val);
    }
    else if (   use_pcs && val <= UChar ('~')
             && Charmap::portable_charset [val]) {
        // use symbols from the Portable Character Set
        ret += Charmap::portable_charset [val];
    }
    else if (val < sizeof printable - 1 && ' ' != printable [val] && !always) {
        // represent non-space printable characters as themselves
        ret += printable [val];
    }
    else if (UChar (' ') == val && !always) {
        // represent space as itself unless always escaping
        ret += ' ';
    }
    else {
        // non-printable character or escaping requested

        char buf [40];
        char *pbuf = buf;

        if (0xffU < val)
            *pbuf++ = '"';

        const unsigned mask = 0xffU << (sizeof mask - 1) * 8;

        while (val && 0 == (val & mask))
            val <<= 8;

        do {
            *pbuf++ = '\\';
            *pbuf++ = 'x';
            *pbuf++ = hexdigit [val >> (sizeof val * 8 - 4)];
            val   <<= 4;
            *pbuf++ = hexdigit [val >> (sizeof val * 8 - 4)];
        } while (val <<= 4);

        if ('"' == *buf)
            *pbuf++ = '"';

        *pbuf = '\0';

        ret = buf;
    }

    return ret;
}


static std::string
create_str (const char* str, const wchar_t* wstr, const Charmap *charmap)
{
    if (0 == str)
        str = "";

    std::string ret;

    if (0 == charmap) {

        // when no character map is provided format non-printable
        // characters using escape sequences
        for (; *str; ++str)
            ret += escape_value (UChar (*str));

        return ret;
    }

    typedef std::map <UChar, std::string>::const_iterator rn_cmap_iter;
    typedef std::map <wchar_t, std::string>::const_iterator rw_cmap_iter;

    if (0 == ctype_st || 1 == ctype_st->mb_cur_max) {
        // look up each character in the character map and (try
        // to) get the symbolic name associated with each, falling
        // back on ordinary formatting using escape sequences when
        // the character is not found in the map

        const rn_cmap_iter end = charmap->get_rn_cmap ().end ();

        for (; *str != '\0'; ++str) {
            const UChar uc = UChar (*str);
            const rn_cmap_iter it = charmap->get_rn_cmap ().find (uc);

            ret += (it == end) ? escape_value (uc) : it->second;
        }
    }
    else {
        const rw_cmap_iter end = charmap->get_rw_cmap ().end ();

        if (0 == wstr)
            wstr = L"";

        for ( ; *wstr != '\0'; ++wstr) {

            const unsigned uc = unsigned (*wstr);

            const rw_cmap_iter it = charmap->get_rw_cmap ().find (uc);

            ret += (it == end) ? escape_value (uc) : it->second;
        }
    }

    return ret;
}


// formats a character in human readable form
static std::string
create_str (unsigned ch, const Charmap *charmap)
{
    typedef std::map <wchar_t, std::string>::const_iterator rw_cmap_iter;

    std::string ret;

    if (charmap) {
        // look up character in the reverse wide character map
        const rw_cmap_iter ch_pos = charmap->get_rw_cmap ().find (ch);

        if (ch_pos == charmap->get_rw_cmap ().end ()) {
            // issue a warning when not found and format it below
            issue_diag (W_CHAR, true, 0, "wide character L'\\%x' "
                        "not found in character map\n", ch);
        }
        else
            return ch_pos->second;
    }

    return escape_value (ch);
}


static void
print_section (const char* name, bool end = false)
{
    assert (0 != name);

    if (print_sect_names)
        std::cout << (end ? "END " : "") << name << '\n';
}


static void
print_keyword (const char* str)
{
    assert (0 != str);

    if (print_keywords)
        std::cout << str << (posix_output ? '=' : ' ');
}

/**************************************************************************/

// place holder for function that prints the "C" locale LC_COLLATE category
static void
print_c_lc_collate ()
{
    assert (0);
}

/**************************************************************************/

static wchar_t
get_wchar_from_offset(unsigned int offset,
                      const unsigned int* wchar_map,
                      unsigned int num_wchars) {
    unsigned int beg = 0;
    unsigned int end = num_wchars;
    while (beg <= end) {
        unsigned int cur = (beg + end) / 2;
        if (*(wchar_map + cur * 2 + 1) == offset) {
            return *(const wchar_t*)(wchar_map + cur * 2);
        }
        else if (*(wchar_map + cur *2) > offset)
            end = cur - 1;
        else
            beg = cur + 1;
    }
    return 0;
}


static void
print_weight (const unsigned int* weightp, 
              unsigned int        num_weights,
              unsigned int        longest_weight)
{
    // FIXME: ignore the order of the element
    ++weightp;

    for (unsigned k = 0; k != num_weights; ++k) {

        for (unsigned x = 0; x != longest_weight; ++x, ++weightp) {

            if (*weightp != UINT_MAX) {
                if (0 == *weightp) 
                    std::cout << "IGNORE;";
                else
                    std::cout << escape_value (*weightp, true) << ';';
            }
        }
    }

    std::cout << '\n';
}


static void
write_coll_info (const std::string &ch, unsigned int idx,
                 unsigned int tab_num)
{
    if (collate_st->num_elms > 1) {
        const Charmap* const cmap = get_charmap (LC_COLLATE);

        typedef std::map <std::string, wchar_t>::const_iterator n_cmap2_iter;
        const unsigned int* tab = collate_st->get_n_tab (tab_num);
        unsigned int first = collate_st->get_first_char_in_n_tab(tab_num);
        for (unsigned int i = first; i <= UCHAR_MAX; i++) {
            std::string new_ch = ch;
            new_ch += UChar (i);
            if (tab[i - first] != UINT_MAX) {
                if (tab[i - first] & 0x80000000) {
                    // it's an offset to another table
                    write_coll_info (new_ch, idx + 1,
                                     tab[i - first] &~ 0x80000000);
                }
                else {
                    // we found the offset to the weight
                    if (cmap) {
                        // first we need to print the symbolic name
                        n_cmap2_iter n_cmap2_it =
                            cmap->get_mb_cmap().find(new_ch);
                        if (n_cmap2_it != cmap->get_mb_cmap().end())
                            std::cout << cmap->get_rw_cmap().find(
                                n_cmap2_it->second)->second;
                    }
                    else {
                        for (unsigned int j = 0; j < idx; j++) {
                            const UChar uc = UChar (new_ch [j]);

                            std::cout << "\\d" << int (uc);

                            if (j != idx - 1)
                                std::cout << ';';
                        }
                    }
                    std::cout << "  ";
                    print_weight (collate_st->get_weight (tab[i - first]),
                                  collate_st->num_weights,
                                  collate_st->longest_weight);

                }
            }
        }
    }
}


static void
print_ce_info (unsigned                             tab_num,
               const std::string                   &mb_str,
               std::map<std::string, unsigned int> &ce_map)
{
    static unsigned ce_num = 0;

    const unsigned* const tab = collate_st->get_n_ce_tab (tab_num);

    const unsigned first = collate_st->get_first_char_in_n_ce_tab (tab_num);
    const unsigned last  = collate_st->get_last_char_in_n_ce_tab (tab_num);

    std::string mbchar;

    const Charmap* const cmap = get_charmap (LC_COLLATE);

    for (unsigned i = first; i <= last; ++i) {

        mbchar = mb_str;
        mbchar += UChar (i);

        const unsigned elem = tab [i - first];

        if (elem != UINT_MAX) {

            if (elem & 0x80000000) {
                // it's an offset to another table. recursively call
                // print_ce_info with the new table number.

                print_ce_info (elem &~ 0x80000000, mbchar, ce_map);
            }
            else {
                // we found the end of the collating element
                // now we need to do a lookup in the narrow character maps
                // and find the symbolic names for the characters that make
                // up this collating element

                // sym is sized at 13 because there will never be more then
                // 99,999 collating elements
                char sym [13];
                std::sprintf (sym, "<RW_CE_%u>", ce_num++);

                ce_map.insert (std::make_pair (sym, elem));
                std::cout << "collating-element " << sym << " from \"";

                if (0 == cmap)
                    std::cout << mbchar << "\"\n";
                else {
                    const std::map<std::string, wchar_t> &cm =
                        cmap->get_mb_cmap ();

                    unsigned ch = 0;

                    for (unsigned j = 1; j <= mbchar.size (); ++j) {
                        const std::map<std::string, wchar_t>::const_iterator
                            it = cm.find (mbchar.substr (ch, j));

                        if (it != cm.end ()) {
                            std::cout << (cmap->get_rw_cmap ().find
                                          (it->second))->second;
                            ch = j;
                        }
                    }

                    std::cout << "\"\n";
                }
            }
        }
    }
}


static int
print_lc_collate (const char *section, int)
{
    print_section (section);

    if (collate_st) {

        if (decode) {

            // print out internal collate_st data
            std::cout <<   "# collate data:"
                      << "\n#   codeset: \""
                      << collate_st->codeset_name ()
                      << "\"\n#   charmap: \""
                      << collate_st->charmap_name ()
                      << "\"\n#   codeset_off: "
                      << collate_st->codeset_off
                      << "\n#   charmap_off: "
                      << collate_st->charmap_off
                      << "\n#   elm_size: "
                      << collate_st->elm_size
                      << "\n#   num_elms: "
                      << collate_st->num_elms
                      << "\n#   num_wchars: "
                      << collate_st->num_wchars
                      << "\n#   longest_weight: "
                      << int (collate_st->longest_weight)
                      << "\n#   num_weights: "
                      << int (collate_st->num_weights)
                      << "\n#   largest_ce: "
                      << int (collate_st->largest_ce)
                      << '\n';
        }

        std::map<std::string, unsigned int> ce_map;
        typedef std::map<std::string, unsigned int>::iterator ce_map_iter;

        if (collate_st->largest_ce > 1) {
            print_ce_info (0, "", ce_map);
        }

        std::cout << "\norder_start ";
        // if there are more then one weight then each should be seperated
        // by a semicolon.  Print out all the weights names in this format
        // except for the last one.
        for (int i = 0; i < collate_st->num_weights; i++) {
            switch (collate_st->weight_type[i]) {
            case 0:
                std::cout << "forward";
                break;
            case 1:
                std::cout << "backward";
                break;
            case 2:
                std::cout << "forward,position";
                break;
            case 3:
                std::cout << "backward,position";
                break;
            default:
                issue_diag (502, true, 0, "invalid order type found "
                            "in collate definition\n");
            }
            if (i == collate_st->num_weights - 1)
                std::cout << '\n';
            else
                std::cout << ';';
        }

        // Print out the characters and their weights in
        // decimal format


        // first print out the collating elements
        for (ce_map_iter it = ce_map.begin (); it != ce_map.end (); ++it) {
            std::cout << it->first << "  ";

            print_weight (collate_st->get_weight (it->second),
                          collate_st->num_weights,
                          collate_st->longest_weight);
        }

        write_coll_info ("", 0, 0);

        if (collate_st->undefined_optimization) {
            std::cout << "UNDEFINED ";

            const unsigned idx = collate_st->undefined_weight_idx;

            print_weight (collate_st->get_weight (idx),
                          collate_st->num_weights,
                          collate_st->longest_weight);
        }

        std::cout << "\norder_end\n";
    }

    print_section (section, true);

    return 0;
}

/**************************************************************************/

// print_mask takes in a ctype_base::mask value and prints all the
// characters in that mask category.  If a charmap was given on the
// command line then it will look up the symbolic name and print that instead
static int
print_mask (const char *section, int mask)
{
    std::string out;

    if (section)
        print_section (section);

    const char *keyword = 0;

    switch (mask) {
    case std::ctype_base::upper:  keyword = "upper"; break;
    case std::ctype_base::lower:  keyword = "lower"; break;
    case std::ctype_base::space:  keyword = "space"; break;
    case std::ctype_base::print:  keyword = "print"; break;
    case std::ctype_base::alpha:  keyword = "alpha"; break;
    case std::ctype_base::cntrl:  keyword = "cntrl"; break;
    case std::ctype_base::punct:  keyword = "punct"; break;
    case std::ctype_base::graph:  keyword = "graph"; break;
    case std::ctype_base::digit:  keyword = "digit"; break;
    case std::ctype_base::xdigit: keyword = "xdigit"; break;
    default: break;
    }

    if (keyword)
        print_keyword (keyword);

    int nchars = 0;

    if (0 != ctype_st) {
        const Charmap* const cmap = get_charmap (LC_CTYPE);

        // go through the entire mask_table and print out each
        // character that has the specified mask, counting the
        // matching characters in the process
        // avoid printing masks without a keyword while still
        // counting such characters
#if 1
        for (std::size_t i = 0; i != ctype_st->wmask_s; ++i) {

            if (ctype_st->wmask_tab (i).mask & mask) {

                ++nchars;

                if (keyword) {
                    std::cout << create_str (ctype_st->wmask_tab (i).ch, cmap);

                    if (i + 1 < ctype_st->wmask_s)
                        std::cout << ';';
                }
            }
        }
#else
        for (std::size_t k = 0; k < ctype_st->wmask_s; ++k) {

            if (ctype_st->wmask_tab (k).mask & mask) {

                if (keyword)
                    out += create_str (ctype_st->wmask_tab (k).ch, cmap);

                ++nchars;

                for (std::size_t j = k + 1; j < ctype_st->wmask_s; ++j) {

                    if (ctype_st->wmask_tab (j).mask & mask) {

                        if (keyword) {
                            out += ';';

                            if (out.size () >= MAX_LINE_LEN && !posix_output)
                                out += '\\';
                        }

                        break;
                    }
                }
            }

            if (keyword && MAX_LINE_LEN < out.size () && !posix_output) {
                std::cout << out << '\n';
                out.clear ();
            }
        }

        std::cout << out;
#endif

    }

    if (keyword)
        std::cout << '\n';

    return nchars;
}


static int
print_toupper (const char *keyword, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_ctype);

    print_keyword (keyword);

    std::string out;

    // number of characters in the maps
    int nchars = 0;

    if (0 != ctype_st) {

        const Charmap* const cmap = get_charmap (LC_CTYPE);

        const std::size_t count = ctype_st->wtoupper_s ();

        nchars += int (count);

        // print out each pair in the toupper table.
        // the wide char table is used because it contains all characters
        for (std::size_t j = 0; j < count; ++j) {
            out += '(';
            out += create_str (ctype_st->wtoupper_tab (j).lower, cmap);
            out += ',';
            out += create_str (ctype_st->wtoupper_tab (j).upper, cmap);
            out += ')';

            if (j != count - 1) {
                if (out.size () >= MAX_LINE_LEN && !posix_output)
                    out += ";\\";
                else
                    out += ';';
            }

            if (out.size () > MAX_LINE_LEN && !posix_output) {
                std::cout << out << '\n';
                out.clear ();
            }
        }

        std::cout << out;
    }

    std::cout << '\n';

    return nchars;
}


static int
print_tolower (const char *keyword, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_ctype);

    print_keyword (keyword);

    std::string out;

    // number of characters in the maps
    int nchars = 0;

    if (0 != ctype_st) {

        const Charmap* const cmap = get_charmap (LC_CTYPE);

        const std::size_t count = ctype_st->wtolower_s ();

        nchars += int (count);

        for (std::size_t j = 0; j < count; ++j) {
            out += '(';
            out += create_str (ctype_st->wtolower_tab (j).upper, cmap);
            out += ',';
            out += create_str (ctype_st->wtolower_tab (j).lower, cmap);
            out += ')';

            if (j != count - 1) {
                if (out.size () >= MAX_LINE_LEN && !posix_output)
                    out += ";\\";
                else
                    out += ';';
            }
            if (out.size () > MAX_LINE_LEN && !posix_output) {
                std::cout << out << '\n';
                out.clear ();
            }
        }
        std::cout << out;
    }

    std::cout << '\n';

    return nchars;
}


static int
print_lc_ctype (const char *section, int)
{
    print_section (section);

    if (0 != ctype_st && decode) {

        // print out internal ctype_st data
        std::cout <<   "# ctype data:"
                  << "\n#   codeset: \""
                  << ctype_st->codeset_name ()
                  << "\"\n#   charmap: \""
                  << ctype_st->charmap_name ()
                  << "\"\n#   codeset_off: "
                  << ctype_st->codeset_off
                  << "\n#   charmap_off: "
                  << ctype_st->charmap_off
                  << "\n#   wmask_s: "
                  << ctype_st->wmask_s
                  << "\n#   wtoupper_off: "
                  << ctype_st->wtoupper_off
                  << "\n#   wtolower_off: "
                  << ctype_st->wtolower_off
                  << "\n#   wmask_off: "
                  << ctype_st->wmask_off
                  << "\n#   ctype_ext_off: "
                  << ctype_st->ctype_ext_off
                  << "\n#   mb_cur_max: "
                  << unsigned (ctype_st->mb_cur_max)
                  << '\n';
    }

    const std::size_t nupper = print_mask (0, std::ctype_base::upper);
    const std::size_t nlower = print_mask (0, std::ctype_base::lower);
    const std::size_t nspace = print_mask (0, std::ctype_base::space);
    const std::size_t nprint = print_mask (0, std::ctype_base::print);
    const std::size_t ncntrl = print_mask (0, std::ctype_base::cntrl);
    const std::size_t nalpha = print_mask (0, std::ctype_base::alpha);
    const std::size_t ndigit = print_mask (0, std::ctype_base::digit);
    const std::size_t npunct = print_mask (0, std::ctype_base::punct);
    const std::size_t ngraph = print_mask (0, std::ctype_base::graph);
    const std::size_t nxdigit = print_mask (0, std::ctype_base::xdigit);

    const int all_masks =
          std::ctype_base::upper | std::ctype_base::lower
        | std::ctype_base::space | std::ctype_base::print
        | std::ctype_base::cntrl | std::ctype_base::alpha
        | std::ctype_base::digit | std::ctype_base::punct
        | std::ctype_base::graph | std::ctype_base::xdigit;

    // count the number of all characters in all tables
    const std::size_t nall = print_mask (0, -1);

    // count the number of characters whose mask is other than
    // any of those specified (probably mistakes)
    const std::size_t nother = print_mask (0, ~all_masks);

    const std::size_t ntoupper = print_toupper ("toupper", 0);
    const std::size_t ntolower = print_tolower ("tolower", 0);

    if (0 != ctype_st && decode) {
        std::cout << "\n# ctype stats:"
                  << "\n#   total characters: " << nall
                  << "\n#   upper characters: " << nupper
                  << "\n#   lower characters: " << nlower
                  << "\n#   space characters: " << nspace
                  << "\n#   print characters: " << nprint
                  << "\n#   cntrl characters: " << ncntrl
                  << "\n#   alpha characters: " << nalpha
                  << "\n#   digit characters: " << ndigit
                  << "\n#   punct characters: " << npunct
                  << "\n#   graph characters: " << ngraph
                  << "\n#   xdigit characters: " << nxdigit
                  << "\n#   unclassified: " << nother
                  << "\n#   tolower pairs: " << ntolower
                  << "\n#   toupper pairs: " << ntoupper
                  << "\n";
    }

    print_section (section, true);

    return 0;
}

/**************************************************************************/

static int
print_decimal_point (const char *keyword, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_numeric);

    print_keyword (keyword);

    if (0 == num_st)
        std::cout << '"' << create_str (".", 0, 0) << "\"\n";
    else {
        const char* const dp =
            _RWSTD_STATIC_CAST (const char*, num_punct_st->decimal_point (0));

        const wchar_t* const wdp =
            _RWSTD_STATIC_CAST(const wchar_t*, num_punct_st->decimal_point (1));

        const Charmap* const cmap = get_charmap (LC_NUMERIC);

        std::cout <<  '"' << create_str (dp, wdp, cmap) << "\"\n";
    }

    return 0;
}


static int
print_thousands_sep (const char *keyword, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_numeric);

    print_keyword (keyword);

    if (0 == num_st)
        std::cout << "\"\"\n";
    else {
        const char* const ts =
            _RWSTD_STATIC_CAST (const char*, num_punct_st->thousands_sep (0));

        const wchar_t* const wts =
            _RWSTD_STATIC_CAST(const wchar_t*, num_punct_st->thousands_sep (1));

        const Charmap* const cmap = get_charmap (LC_NUMERIC);

        std::cout <<  '"' << create_str (ts, wts, cmap) << "\"\n";
    }

    return 0;
}


static int
print_grouping (const char *keyword, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_numeric);

    print_keyword (keyword);

    if (0 == num_st)
        std::cout << "-1\n";
    else {
        size_t i;
        for (i = 0;
             i < num_punct_st->punct_ext_off - num_punct_st->grouping_off - 2;
             i++)
            std::cout << (int)((const char*)num_punct_st->grouping())[i]
                      << ";";
        std::cout << (int)((const char*)num_punct_st->grouping())[i]
                  << '\n';
    }

    return 0;
}


static int
print_lc_numeric (const char *section, int)
{
    print_section (section);

    if (0 != num_st && decode) {

        // print out internal numeric_st data
        std::cout <<   "# numeric data:"
                  << "\n#   codeset: \""
                  << num_st->codeset_name ()
                  << "\"\n#   charmap: \""
                  << num_st->charmap_name ()
                  << "\"\n#   codeset_off: "
                  << num_st->codeset_off
                  << "\n#   charmap_off: "
                  << num_st->charmap_off
                  << '\n';
    }

    // keywords follow the established order on popular platforms
    print_decimal_point ("decimal_point", 0);
    print_thousands_sep ("thousands_sep", 0);
    print_grouping ("grouping", 0);

    print_section (section, true);

    return 0;
}

/**************************************************************************/

static int
print_int_curr_symbol (const char *keyword, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_monetary);

    print_keyword (keyword);

    if (0 == mon_st)
        std::cout << "\"\"\n";
    else {
        const char* const cs =
            _RWSTD_STATIC_CAST (const char*, mon_st->curr_symbol (1, 0));

        const wchar_t* const wcs =
            _RWSTD_STATIC_CAST (const wchar_t*, mon_st->curr_symbol (1, 1));

        const Charmap* const cmap = get_charmap (LC_MONETARY);

        std::cout << '"' << create_str (cs, wcs, cmap) << "\"\n";
    }

    return 0;
}


static int
print_currency_symbol (const char *keyword, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_monetary);

    print_keyword (keyword);

    if (0 == mon_st)
        std::cout << "\"\"\n";
    else {
        const char* const cs =
            _RWSTD_STATIC_CAST (const char*, mon_st->curr_symbol (0, 0));

        const wchar_t* const wcs =
            _RWSTD_STATIC_CAST (const wchar_t*, mon_st->curr_symbol (0, 1));

        const Charmap* const cmap = get_charmap (LC_MONETARY);

        std::cout << '"' << create_str (cs, wcs, cmap) << "\"\n";
    }

    return 0;
}


static int
print_mon_decimal_point (const char *keyword, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_monetary);

    print_keyword (keyword);

    if (0 == mon_st)
        std::cout << "\"\"\n";
    else {
        const char* const dp =
            _RWSTD_STATIC_CAST (const char*, mon_punct_st->decimal_point (0));

        const wchar_t* const wdp =
            _RWSTD_STATIC_CAST(const wchar_t*, mon_punct_st->decimal_point (1));

        const Charmap* const cmap = get_charmap (LC_MONETARY);

        std::cout << '"' << create_str (dp, wdp, cmap) << "\"\n";
    }

    return 0;
}


static int
print_mon_thousands_sep (const char *keyword, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_monetary);

    print_keyword (keyword);

    if (0 == mon_st)
        std::cout << "\"\"\n";
    else {
        const char* const ts =
            _RWSTD_STATIC_CAST (const char*, mon_punct_st->thousands_sep (0));

        const wchar_t* const wts =
            _RWSTD_STATIC_CAST(const wchar_t*, mon_punct_st->thousands_sep (1));

        const Charmap* const cmap = get_charmap (LC_MONETARY);

        std::cout << '"' << create_str (ts, wts, cmap) << "\"\n";
    }

    return 0;
}


static int
print_mon_grouping (const char *keyword, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_monetary);

    print_keyword (keyword);

    if (0 == mon_st)
        std::cout << "-1\n";
    else {
        size_t i;
        for (i = 0;
             i < mon_punct_st->punct_ext_off - mon_punct_st->grouping_off - 2;
             i++)
            std::cout << (int)((const char*)mon_punct_st->grouping())[i] << ";";
        std::cout << (int)((const char*)mon_punct_st->grouping())[i]
                  << '\n';
    }

    return 0;
}


static int
print_positive_sign (const char *keyword, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_monetary);

    print_keyword (keyword);

    if (0 == mon_st)
        std::cout << "\"\"\n";
    else {
        const char* const ps =
            _RWSTD_STATIC_CAST (const char*, mon_st->positive_sign (0));

        const wchar_t* const wps =
            _RWSTD_STATIC_CAST (const wchar_t*, mon_st->positive_sign (1));

        const Charmap* const cmap = get_charmap (LC_MONETARY);

        std::cout << '"' << create_str (ps, wps, cmap) << "\"\n";
    }

    return 0;
}


static int
print_negative_sign (const char *keyword, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_monetary);

    print_keyword (keyword);

    if (0 == mon_st)
        std::cout << "\"\"\n";
    else {
        const char* const ns =
            _RWSTD_STATIC_CAST (const char*, mon_st->negative_sign (0));

        const wchar_t* const wns =
            _RWSTD_STATIC_CAST (const wchar_t*, mon_st->negative_sign (1));
        
        const Charmap* const cmap = get_charmap (LC_MONETARY);

        std::cout << '"' << create_str (ns, wns, cmap) << "\"\n";
    }

    return 0;
}


static void
print_int (const char *keyword, int val, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_monetary);

    print_keyword (keyword);

    std::cout << val << '\n';
}


#define MON_INT(member)   \
    (mon_st && CHAR_MAX != (mon_st->member) ? int (UChar (mon_st->member)) : -1)

static int
print_int_frac_digits (const char *keyword, int print_cat)
{
    print_int (keyword, MON_INT (frac_digits [1]), print_cat);

    return 0;

}


static int
print_frac_digits (const char *keyword, int print_cat)
{
    print_int (keyword, MON_INT (frac_digits [0]), print_cat);

    return 0;
}


static int
print_p_cs_precedes (const char *keyword, int print_cat)
{
    print_int (keyword, MON_INT (p_cs_precedes [0]), print_cat);

    return 0;
}


static int
print_n_sep_by_space (const char *keyword, int print_cat)
{
    print_int (keyword, MON_INT (n_sep_by_space [0]), print_cat);

    return 0;
}


static int
print_p_sep_by_space (const char *keyword, int print_cat)
{
    print_int (keyword, MON_INT (p_sep_by_space [0]), print_cat);

    return 0;
}


static int
print_n_cs_precedes (const char *keyword, int print_cat)
{
    print_int (keyword, MON_INT (n_cs_precedes [0]), print_cat);

    return 0;
}


static int
print_p_sign_posn (const char *keyword, int print_cat)
{
    print_int (keyword, MON_INT (p_sign_posn [0]), print_cat);

    return 0;
}


static int
print_n_sign_posn (const char *keyword, int print_cat)
{
    print_int (keyword, MON_INT (n_sign_posn [0]), print_cat);

    return 0;
}


static int
print_int_p_cs_precedes (const char *keyword, int print_cat)
{
    print_int (keyword, MON_INT (p_cs_precedes [1]), print_cat);

    return 0;
}


static int
print_int_n_sep_by_space (const char *keyword, int print_cat)
{
    print_int (keyword, MON_INT (n_sep_by_space [1]), print_cat);

    return 0;
}


static int
print_int_p_sep_by_space (const char *keyword, int print_cat)
{
    print_int (keyword, MON_INT (p_sep_by_space [1]), print_cat);

    return 0;
}


static int
print_int_n_cs_precedes (const char *keyword, int print_cat)
{
    print_int (keyword, MON_INT (n_cs_precedes [1]), print_cat);

    return 0;
}


static int
print_int_p_sign_posn (const char *keyword, int print_cat)
{
    print_int (keyword, MON_INT (p_sign_posn [1]), print_cat);

    return 0;
}


static int
print_int_n_sign_posn (const char *keyword, int print_cat)
{
    print_int (keyword, MON_INT (n_sign_posn [1]), print_cat);

    return 0;
}


static int
print_lc_monetary (const char *section, int)
{
    print_section (section);

    if (mon_st && decode) {

        // print out internal monetary_st data
        std::cout <<   "# monetary data:"
                  << "\n#   codeset: \""
                  << mon_st->codeset_name ()
                  << "\"\n#   charmap: \""
                  << mon_st->charmap_name ()
                  << "\"\n#   codeset_off: "
                  << mon_st->codeset_off
                  << "\n#   charmap_off: "
                  << mon_st->charmap_off
                  << '\n';
    }

    // keywords follow the established order on popular platforms
    print_int_curr_symbol ("int_curr_symbol", 0);
    print_currency_symbol ("currency_symbol", 0);
    print_mon_decimal_point ("mon_decimal_point", 0);
    print_mon_thousands_sep ("mon_thousands_sep", 0);
    print_mon_grouping ("mon_grouping", 0);
    print_positive_sign ("positive_sign", 0);
    print_negative_sign ("negative_sign", 0);

    print_int_frac_digits ("int_frac_digits", 0);
    print_frac_digits ("frac_digits", 0);

    print_p_cs_precedes ("p_cs_precedes", 0);
    print_p_sep_by_space ("p_sep_by_space", 0);
    print_n_cs_precedes ("n_cs_precedes", 0);
    print_n_sep_by_space ("n_sep_by_space", 0);
    print_p_sign_posn ("p_sign_posn", 0);
    print_n_sign_posn ("n_sign_posn", 0);

    print_int_p_cs_precedes ("int_p_cs_precedes", 0);
    print_int_p_sep_by_space ("int_p_sep_by_space", 0);
    print_int_n_cs_precedes ("int_n_cs_precedes", 0);
    print_int_n_sep_by_space ("int_n_sep_by_space", 0);
    print_int_p_sign_posn ("int_p_sign_posn", 0);
    print_int_n_sign_posn ("int_n_sign_posn", 0);

    print_section (section, true);

    return 0;
}

/**************************************************************************/

static void
print_time_keyword (const char *keyword, int print_cat,
                    const void *vstr, const void *vwstr)
{
    if (print_cat)
        print_section (sect_lc_monetary);

    if (keyword)
        print_keyword (keyword);

    const Charmap* const cmap = get_charmap (LC_TIME);

    const char*    const str  = _RWSTD_STATIC_CAST (const char*, vstr);
    const wchar_t* const wstr = _RWSTD_STATIC_CAST (const wchar_t*, vwstr);

    std::cout << '"' << create_str (str, wstr, cmap) << "\"";
}


static int
print_abday (const char *keyword, int print_cat)
{
    static const char str[][4] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    for (std::size_t i = 0; ; ) {

        print_time_keyword (keyword, print_cat,
                            time_st ? time_st->abday (i, 0) : str [i],
                            time_st ? time_st->abday (i, 1) : 0);

        keyword   = 0;
        print_cat = 0;

        if (sizeof str / sizeof *str == ++i)
            break;

        std::cout << ';';
    }

    std::cout << '\n';

    return 0;
}


static int
print_day (const char *keyword, int print_cat)
{
    static const char str[][10] = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    };

    for (std::size_t i = 0; ; ) {

        print_time_keyword (keyword, print_cat,
                            time_st ? time_st->day (i, 0) : str [i],
                            time_st ? time_st->day (i, 1) : 0);

        keyword   = 0;
        print_cat = 0;

        if (sizeof str / sizeof *str == ++i)
            break;

        std::cout << ';';
    }

    std::cout << '\n';

    return 0;
}


static int
print_abmon (const char *keyword, int print_cat)
{
    static const char str[][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    for (std::size_t i = 0; ; ) {

        print_time_keyword (keyword, print_cat,
                            time_st ? time_st->abmon (i, 0) : str [i],
                            time_st ? time_st->abmon (i, 1) : 0);

        keyword   = 0;
        print_cat = 0;

        if (sizeof str / sizeof *str == ++i)
            break;

        std::cout << ';';
    }

    std::cout << '\n';

    return 0;
}


static int
print_mon (const char *keyword, int print_cat)
{
    static const char str[][10] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    for (std::size_t i = 0; ; ) {

        print_time_keyword (keyword, print_cat,
                            time_st ? time_st->mon (i, 0) : str [i],
                            time_st ? time_st->mon (i, 1) : 0);

        keyword   = 0;
        print_cat = 0;

        if (sizeof str / sizeof *str == ++i)
            break;

        std::cout << ';';
    }

    std::cout << '\n';

    return 0;
}


static int
print_am_pm (const char *keyword, int print_cat)
{
    static const char str[][3] = { "AM", "PM" };
    
    print_time_keyword (keyword, print_cat,
                        time_st ? time_st->am_pm (0, 0) : str [0],
                        time_st ? time_st->am_pm (0, 1) : str [1]);

    std::cout << ';';

    print_time_keyword (0, 0,
                        time_st ? time_st->am_pm (1, 0) : str [0],
                        time_st ? time_st->am_pm (1, 1) : str [1]);

    std::cout << '\n';

    return 0;
}


static int
print_era (const char *keyword, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_monetary);

    print_keyword (keyword);

    if (0 == time_st)
        std::cout << '"' << create_str ("", 0, 0)
                  << "\"\n";
    else if (0 == time_st->era_count ()) {
        // if there are no eras, print out the empty string
        std::cout << "\"\"\n";
    }
    else {
        const Charmap* const cmap = get_charmap (LC_TIME);

        // print each era in the database according to the format
        // specified by POSIX
        for (unsigned int i = 0; i < time_st->era_count(); i++) {
            const _RW::__rw_time_t::era_t *era_p = time_st->era (i);
            // first print the direction and the offset
            if (era_p->offset >= 0)
                std::cout << "\"+:" << era_p->offset << ":";
            else {
                std::cout << "\"-:" << era_p->offset * -1 << ":";
            }
            // now print the start year, if it's negative, make sure the
            // negative sign gets in front
            if (era_p->year[0] < 0)
                std::cout << '-' << std::setw(4) << std::setfill ('0')
                          << era_p->year[0] * -1;
            else
                std::cout << std::setw(4) << std::setfill ('0')
                          << era_p->year[0];

            // now print the rest of the start date
            std::cout << _RWSTD_PATH_SEP << std::setw(2)
                      << int(era_p->month[0]) + 1 << _RWSTD_PATH_SEP 
                      << std::setw(2) << int(era_p->day[0]) << ":";

            // now print the end year, INT_MIN is the beginning of time
            // and INT_MAX is the end of time
            if (era_p->year[1] == INT_MIN)
                std::cout << "-*:";
            else if (era_p->year[1] == INT_MAX)
                std::cout << "+*:";
            else {
                if (era_p->year[1] < 0)
                    std::cout << '-' << std::setw(4) << era_p->year[1] * -1;
                else
                    std::cout << std::setw(4) << era_p->year[1];

                // now print out the rest of the end date
                std::cout << _RWSTD_PATH_SEP << std::setw(2) 
                          << int(era_p->month[1]) + 1 << _RWSTD_PATH_SEP 
                          << std::setw(2) << int(era_p->day[1]) << ":";
            }

            const char* const era =
                _RWSTD_STATIC_CAST (const char*, time_st->era_name (i, 0));

            const wchar_t* const wera =
                _RWSTD_STATIC_CAST (const wchar_t*, time_st->era_name (i, 1));

            const char* const fmt =
                _RWSTD_STATIC_CAST (const char*, time_st->era_fmt (i, 0));

            const wchar_t* const wfmt =
                _RWSTD_STATIC_CAST (const wchar_t*, time_st->era_fmt (i, 1));

            std::cout << create_str (era, wera, cmap) << ":"
                      << create_str (fmt, wfmt, cmap);

            if (i != time_st->era_count () - 1)
                std::cout << "\";";
            else
                std::cout << "\"\n";
        }
    }

    return 0;
}


static int
print_d_t_fmt (const char *keyword, int print_cat)
{
    static const char str[] = "%a %b %e %H:%M:%S %Y";

    print_time_keyword (keyword, print_cat,
                        time_st ? time_st->d_t_fmt (0) : str,
                        time_st ? time_st->d_t_fmt (1) : 0);

    std::cout << '\n';

    return 0;
}


static int
print_era_d_t_fmt (const char *keyword, int print_cat)
{
    print_time_keyword (keyword, print_cat,
                        time_st ? time_st->era_d_t_fmt (0) : 0,
                        time_st ? time_st->era_d_t_fmt (1) : 0);

    std::cout << '\n';

    return 0;
}


static int
print_d_fmt (const char *keyword, int print_cat)
{
    static const char str[] = "%m/%d/%y";

    print_time_keyword (keyword, print_cat,
                        time_st ? time_st->d_fmt (0) : str,
                        time_st ? time_st->d_fmt (1) : 0);

    std::cout << '\n';

    return 0;
}


static int
print_era_d_fmt (const char *keyword, int print_cat)
{
    print_time_keyword (keyword, print_cat,
                        time_st ? time_st->era_d_fmt (0) : 0,
                        time_st ? time_st->era_d_fmt (1) : 0);

    std::cout << '\n';

    return 0;
}


static int
print_t_fmt (const char *keyword, int print_cat)
{
    static const char str[] = "%H:%M:%S";

    print_time_keyword (keyword, print_cat,
                        time_st ? time_st->t_fmt (0) : str,
                        time_st ? time_st->t_fmt (1) : 0);

    std::cout << '\n';

    return 0;
}


static int
print_era_t_fmt (const char *keyword, int print_cat)
{
    print_time_keyword (keyword, print_cat,
                        time_st ? time_st->era_t_fmt (0) : 0,
                        time_st ? time_st->era_t_fmt (1) : 0);

    std::cout << '\n';

    return 0;
}


static int
print_t_fmt_ampm (const char *keyword, int print_cat)
{
    static const char str[] = "%I:%M:%S %p";

    print_time_keyword (keyword, print_cat,
                        time_st ? time_st->t_fmt_ampm (0) : str,
                        time_st ? time_st->t_fmt_ampm (1) : 0);

    std::cout << '\n';

    return 0;
}


static int
print_alt_digits (const char *keyword, int print_cat)
{
    if (time_st) {
        const unsigned ndigits = time_st->alt_digits_count ();

        for (unsigned i = 0; ; ) {

            print_time_keyword (keyword, print_cat,
                                i < ndigits ? time_st->alt_digits (i, 0) : 0,
                                i < ndigits ? time_st->alt_digits (i, 1) : 0);

            keyword   = 0;
            print_cat = 0;

            if (ndigits <= ++i)
                break;

            std::cout << ';';
        }

        std::cout << '\n';
    }
    else {
        print_time_keyword (keyword, print_cat, 0, 0);
    }

    return 0;
}


static int
print_lc_time (const char *section, int)
{
    print_section (section);

    if (time_st && decode) {

        // print out internal time_st data
        std::cout <<   "# time data:"
                  << "\n#   codeset: \""
                  << time_st->codeset_name ()
                  << "\"\n#   charmap: \""
                  << time_st->charmap_name ()
                  << "\"\n#   codeset_off: "
                  << time_st->codeset_off
                  << "\n#   charmap_off: "
                  << time_st->charmap_off
                  << '\n';
    }

    print_abday ("abday", 0);
    print_day ("day", 0);
    print_abmon ("abmon", 0);
    print_mon ("mon", 0);
    print_am_pm ("am_pm", 0);
    print_d_t_fmt ("d_t_fmt", 0);
    print_d_fmt ("d_fmt", 0);
    print_t_fmt ("t_fmt", 0);
    print_t_fmt_ampm ("t_fmt_ampm", 0);
    print_era ("era", 0);
    print_era_d_t_fmt ("era_d_t_fmt", 0);
    print_era_d_fmt ("era_d_fmt", 0);
    print_era_t_fmt ("era_t_fmt", 0);
    print_alt_digits ("alt_digits", 0);

    print_section (section, true);

    return 0;
}

/**************************************************************************/

static int
print_noexpr (const char *keyword, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_messages);

    print_keyword (keyword);

    if (0 == messages_st)
        std::cout << '"' << create_str ("^[nN]", 0, 0) << "\"\n";
    else {
        const char* const no
            = _RWSTD_STATIC_CAST (const char*, messages_st->noexpr (0));

        const wchar_t* const wno
            = _RWSTD_STATIC_CAST (const wchar_t*, messages_st->noexpr (1));

        const Charmap* const cmap = get_charmap (LC_MESSAGES);

        std::cout << '"' << create_str (no, wno, cmap) << "\"\n";
    }

    return 0;
}


static int
print_yesexpr (const char *keyword, int print_cat)
{
    if (print_cat)
        print_section (sect_lc_messages);

    print_keyword (keyword);

    if (0 == messages_st)
        std::cout << '"' << create_str ("^[yY]", 0, 0) << "\"\n";
    else {
        const char* const yes
            = _RWSTD_STATIC_CAST (const char*, messages_st->yesexpr (0));

        const wchar_t* const wyes
            = _RWSTD_STATIC_CAST (const wchar_t*, messages_st->yesexpr (1));

        const Charmap* const cmap = get_charmap (LC_MESSAGES);

        std::cout << '"' << create_str (yes, wyes, cmap) << "\"\n";
    }

    return 0;
}


static int
print_lc_messages (const char *section, int)
{
    print_section (section);

    if (messages_st && decode) {

        // print out internal time_st data
        std::cout <<   "# messages data:"
                  << "\n#   codeset: \""
                  << messages_st->codeset_name ()
                  << "\"\n#   charmap: \""
                  << messages_st->charmap_name ()
                  << "\"\n#   codeset_off: "
                  << messages_st->codeset_off
                  << "\n#   charmap_off: "
                  << messages_st->charmap_off
                  << '\n';
    }

    print_yesexpr ("yesexpr", 0);
    print_noexpr ("noexpr", 0);

    print_section (section, true);

    return 0;
}

/**************************************************************************/

static int
print_lc_all (const char*, int)
{
    print_lc_collate (sect_lc_collate, 0);
    std::cout << '\n';

    print_lc_ctype (sect_lc_ctype, 0);
    std::cout << '\n';

    print_lc_monetary (sect_lc_monetary, 0);
    std::cout << '\n';

    print_lc_numeric (sect_lc_numeric, 0);
    std::cout << '\n';

    print_lc_time (sect_lc_time, 0);
    std::cout << '\n';

    print_lc_messages (sect_lc_messages, 0);

    return 0;
}

/**************************************************************************/

static int
print_charmap_name (const char *section, int)
{
    if (0 != ctype_st) {
        print_section (section);

        print_keyword ("charmap");
        std::cout << '"'<< ctype_st->charmap_name () << "\"\n";
    }

    return 0;
}


#ifdef _WIN32
static BOOL CALLBACK
EnumLocales (char* locale_id)
{
    const LCID lcid = std::strtoul (locale_id, 0, 16);

    char buf [80];
    const int bufsize = sizeof (buf) / sizeof (*buf);

    std::string name;

    if (GetLocaleInfo (lcid, LOCALE_SENGLANGUAGE, buf, bufsize))
        name = buf;

    if (GetLocaleInfo (lcid, LOCALE_SENGCOUNTRY, buf, bufsize)) {
        name += '_';
        name += buf;
    }

    if (   GetLocaleInfo (lcid, LOCALE_IDEFAULTANSICODEPAGE , buf, bufsize)
        && std::strtoul (buf, 0, 10)) {
        name += '.';
        name += buf;
    }

    if (const char* locname = std::setlocale (LC_ALL, name.c_str ()))
        std::cout << locname << '\n';

    return TRUE;
}
#endif

// print the available locales
static void
print_locale_names ()
{
    std::cout << "C\n";

    const char* const locale_root = std::getenv ("RWSTD_LOCALE_ROOT");

    if (0 != locale_root) {
        const std::string cmd = std::string (LS_1) + locale_root;

        std::system (cmd.c_str ());
    }
    else {
#ifndef _WIN32
        std::system ("/usr/bin/locale -a");
#else
        EnumSystemLocales (EnumLocales, LCID_INSTALLED);
#endif
    }
}


// print the available charmaps
static void
print_charmap_names ()
{
    const char* const locale_root = std::getenv ("RWSTD_SRC_ROOT");

    if (0 != locale_root) {
        const std::string cmd =
            std::string (LS_1) + locale_root + SLASH "charmaps";

        std::system (cmd.c_str());
    }
}

/**************************************************************************/

static void
print_help ()
{
    static const char* msg =
        "NAME\n\tlocale - get locale-specific information\n\n"
        "SYNOPSIS\n"
        "\tlocale [-a | -m | -? | --help | --charmap]\n\n"
        "\tlocale [-ckhlw] name ...\n\n"
        "DESCRIPTION\n"
        "\tThe locale utility writes information about the current\n"
        "\tlocale environment, or all public locales, to the standard\n"
        "\toutput. For the purposes of this section, a public locale is\n"
        "\tone provided by the implementation that is accessible to the\n"
        "\tapplication.\n\n"
        "\tWhen locale is invoked without any arguments, it summarizes\n"
        "\tthe current locale environment for each locale category as\n"
        "\tdetermined by the settings of the environment variables.\n\n"
        "\tWhen invoked with operands, it writes values that have been\n"
        "\tassigned to the keywords in the locale categories.\n\n"
        "OPTIONS\n"
        "\tThe following options are supported:\n\n"
        "\t-a\tWrite information about all available public locales.\n"
        "\t\tThe available locales include C, representing the\n"
        "\t\tPOSIX locale.\n\n"
        "\t-c\tWrite the names of selected locale categories.\n\n"
        "\t-k\tWrite the names and values of selected keywords.\n\n"
        "\t-m\tWrite names of available charmaps.\n\n"
        "\t-h\tIf available, use the original character map and \n"
        "\t\twrite the symbolic names instead of the character values\n\n"
        "\t-l\tWrite the locale information in a format readable by\n"
        "\t\tthe localedef utility\n\n"
        "\t-w\tTurn off all warning messages\n\n"
        "\t-w###\tTurn off a specific warning message\n\n"
        "\t-?\tWrite this message\n\n"
        "\t--help\tWrite this message\n\n"
        "\t--charmap\tWrite the character map.\n\n"
        "OPERANDS\n"
        "\tThe following operand is supported:\n\n"
        "\tname\tThe name of a locale category, the name of a keyword\n"
        "\t\tin a locale category, or the reserved name charmap.\n"
        "\t\tWhen charmap is specified, the utility writes the name\n"
        "\t\tof the character set description file or files used\n"
        "\t\tto create the current locale environment.\n\n";

    std::cout << msg;
}

/**************************************************************************/

static void
print_ellipsis (const char *mbchar,
                unsigned    last_byte [2],
                unsigned    last_ucs4 [2],
                unsigned    last_wchar [2])
{
    // close the ellipsis
    const int open_width  = last_ucs4 [0] < 0x10000 ? 4 : 8;
    const int close_width = last_ucs4 [1] < 0x10000 ? 4 : 8;

    // the number of spaces between the symbolic character name
    // and its multibyte character representation
    int pad = 28 - open_width;

    const unsigned nchars = last_byte [1] - last_byte [0] + 1;

    if (1 < nchars) {

        pad -= close_width + 8;

        // print the UCS code, followed by the MB character,
        // followed by the libc wchar_t value of the character
        std::printf ("<U%0*X>..<U%0*X>%*s%s\\x%02x   "
                     "# L'\\x%02x' (%u characters)\n",
                     open_width, last_ucs4 [0],
                     close_width, last_ucs4 [1], pad, " ",
                     mbchar, last_byte [0], last_wchar [0],
                     nchars);
    }
    else {
        pad -= 3;

        std::printf ("<U%0*X>%*s%s\\x%02x   "
                     "# L'\\x%02x'\n",
                     open_width, last_ucs4 [0], pad, " ",
                     mbchar, last_byte [0], last_wchar [0]);
    }

    last_byte [0]  = last_byte [1] = UINT_MAX;
    last_ucs4 [0]  = last_ucs4 [1] = 0;
    last_wchar [0] = last_wchar [1] = 0;
}

// traverses codeset conversion tables and prints out the character
// map in POSIX format with the internal wchar_t encoding in comments
// collects space utilization statistics
static void
print_charmap (const __rw::__rw_codecvt_t *cvt,
               char           *mbchar,
               const unsigned *tab,
               unsigned       *ntables,
               unsigned       *nchars,
               unsigned       *nunused)
{
    const unsigned* const table = cvt->n_to_w_tab ();

    if (!tab)
        tab = table;

    // the length of the multibyte character (prefix) formatted
    // in a human-readable form (e.g., "\x80\xff" or similar)
    const std::size_t mbchar_len = std::strlen (mbchar);

    // index into the array of counters corresponding to the
    // table being processed at this level of recursion
    const std::size_t count_inx = mbchar_len / 4 + 1;

    if (ntables) {
        // increment the grand total of all tables
        ++ntables [0];
        // increment the number of tables for characters
        // of this length
        ++ntables [count_inx];
    }

    unsigned last_byte [2]  = { UINT_MAX, 0 };
    unsigned last_ucs4 [2]  = { 0, 0 };
    unsigned last_wchar [2] = { 0, 0 };

    // print out all multibyte characters in this map
    for (unsigned i = 0; i != UCHAR_MAX + 1U; ++i) {

        if (tab [i] & 0x80000000) {

            if (last_byte [0] <= UCHAR_MAX) {
                // print the last character or ellipsis
                print_ellipsis (mbchar, last_byte, last_ucs4, last_wchar);
            }

            // skip invalid character or next table
            continue;
        }

        // look up the UCS-4 and the wchar_t value of the MB character
        const unsigned ucs4  = cvt->get_ucs4_at_offset (tab [i]);
        const unsigned wchar = cvt->get_internal_at_offset (tab [i]);

        if (UCHAR_MAX < last_byte [0]) {
            last_byte [0]  = i;
            last_ucs4 [0]  = ucs4;
            last_wchar [0] = wchar;
        }
        else if (1 < ucs4 - last_ucs4 [1] || 1 < wchar - last_wchar [1]) {
            // print the last character or ellipsis
            print_ellipsis (mbchar, last_byte, last_ucs4, last_wchar);

            last_byte [0]  = i;
            last_ucs4 [0]  = ucs4;
            last_wchar [0] = wchar;
        }

        last_byte [1]  = i;
        last_ucs4 [1]  = ucs4;
        last_wchar [1] = wchar;

        if (nchars) {
            // increment the grand total of all characters
            ++nchars [0];

            // increment the number of multibyte characters
            // of this length
            ++nchars [count_inx];
        }
    }

    if (last_byte [0] <= UCHAR_MAX)
        print_ellipsis (mbchar, last_byte, last_ucs4, last_wchar);

    // process subsequent maps
    for (unsigned i = 0; i != UCHAR_MAX + 1U; ++i) {

        if (UINT_MAX == tab [i]) {
            // invalid multibyte sequence (i.e., unused slot)

            if (nunused) {
                // increment counters
                ++nunused [0];
                ++nunused [count_inx];
            }

            continue;
        }

        if (!(tab [i] & 0x80000000)) {
            // valid multibyte characater already printed out above
            continue;
        }

        // invoke self recursively to print out the contents
        // of the next map
        std::sprintf (mbchar + mbchar_len, "\\x%02x", i);

        print_charmap (cvt, mbchar, table + 256 * (tab [i] & 0x7fffffff),
                       ntables, nchars, nunused);

        mbchar [mbchar_len] = '\0';
    }
}


// print out the contents of the __rw_codecvt_t structure
static void
print_charmap (const __rw::__rw_codecvt_t *cvt = 0)
{
    if (0 == cvt) {
        // use the codecvt map when none is specified
        cvt = codecvt_st;
    }

    if (0 == cvt)
        return;

    std::cout <<   "<escape_char>   \\"
              << "\n<comment_char>  #"
              << "\n<code_set_name> " << cvt->codeset_name ()
              << "\n<mb_cur_max>    " << int (cvt->mb_cur_max)
              << "\nCHARMAP\n";

    if (decode) {
        std::cout << "# charmap data:"
                  << "\n#   charmap name: "
                  << cvt->charmap_name ()
                  << "\n#   n_to_w_tab_off: "
                  << cvt->n_to_w_tab_off
                  << "\n#   w_to_n_tab_off: "
                  << cvt->w_to_n_tab_off
                  << "\n#   utf8_to_ext_tab_off: "
                  << cvt->utf8_to_ext_tab_off
                  << "\n#   xliteration_off: "
                  << cvt->xliteration_off
                  << "\n#   wchar_off: "
                  << cvt->wchar_off
                  << "\n#   codeset_off: "
                  << cvt->codeset_off
                  << "\n#   charmap_off: "
                  << cvt->charmap_off
                  << "\n#   codecvt_ext_off: "
                  << cvt->codecvt_ext_off
                  << '\n';
    }
            
    char buf [1024];
    *buf = '\0';

    // total number of conversion tables for characters
    // of each length up to MB_CUR_MAX with ntables being
    // the sum of all of them
    unsigned ntables [64] = { 0 };

    // total number of unused slots in tables at each level
    // (i.e., slots not used either to encode character or
    // to store an offset to the next table)
    unsigned nunused [64] = { 0 };

    // total numbers of multibyte characters of each lenght
    // up to MB_CUR_MAX with nchars [0] being the sum of all
    // of them
    unsigned nchars [64] = { 0 };

    print_charmap (cvt, buf, 0, ntables, nchars, nunused);

    std::cout << "END CHARMAP\n"
              << "\n# charmap stats:";

    for (std::size_t i = 1; i != sizeof nchars / sizeof *nchars; ++i) {
        if (0 != nchars [i] || 0 != ntables [i]) {

            const double waste =
                (100.0 * nunused [i]) / (ntables [i] * UCHAR_MAX);

            std::cout << "\n#   ";
            std::cout << i << " byte characters: " << nchars [i]
                      << " (" << ntables [i] << " tables, "
                      << unsigned (waste) << "% waste)";
        }
    }

    const double waste = (100.0 * nunused [0]) / (ntables [0] * UCHAR_MAX);

    std::cout << "\n#   total characters:  " << nchars [0]
              << " (" << ntables [0] << " tables, "
              << unsigned (waste) << "% waste)\n";
}

/**************************************************************************/

// print the value of a single localization environment variable
// according to POSIX rules
static void
print_lc_var (const char* cat_name, const char* val,
              const char *lc_all, const char *lang)
{
    std::cout << cat_name << '=';

    if (0 == val)
        std::cout << '"';

    const char* const strval = 
        (lc_all ? lc_all : val ? val : lang ? lang : "");

    // POSIX requires that environment variable "values be properly
    // quoted for possible later reentry to the shell"
    for (const char* s = strval; *s; ++s) {
        switch (*s) {
        case '$': case '"':
            // escape unconditionally
            std::cout << '\\' << *s; break;

        case '|': case '&': case '\\': case '<': case '>':
        case '`': case ' ':
            // escape only when not quoted
            if (val) std::cout << '\\' << *s;
            
        default:
            // do not escape
            std::cout << *s;
        }
    }

    if (0 == val)
        std::cout << '"';

    std::cout << '\n';
}

/**************************************************************************/

// print the values of the LANG and LC_XXX environment variables
// according to POSIX rules
static void
print_lc_vars ()
{
    const char* const lang        = std::getenv ("LANG");
    const char* const lc_all      = std::getenv (sect_lc_all);
    const char* const lc_collate  = std::getenv (sect_lc_collate);
    const char* const lc_ctype    = std::getenv (sect_lc_ctype);
    const char* const lc_messages = std::getenv (sect_lc_messages);
    const char* const lc_monetary = std::getenv (sect_lc_monetary);
    const char* const lc_numeric  = std::getenv (sect_lc_numeric);
    const char* const lc_time     = std::getenv (sect_lc_time);

    print_lc_var ("LANG",           lang ? lang : "", 0, 0);
    print_lc_var (sect_lc_ctype,    lc_ctype,    lc_all, lang);
    print_lc_var (sect_lc_collate,  lc_collate,  lc_all, lang);
    print_lc_var (sect_lc_time,     lc_time,     lc_all, lang);
    print_lc_var (sect_lc_monetary, lc_monetary, lc_all, lang);
    print_lc_var (sect_lc_numeric,  lc_numeric,  lc_all, lang);
    print_lc_var (sect_lc_messages, lc_messages, lc_all, lang);
    print_lc_var (sect_lc_all,      lc_all ? lc_all : "", 0, 0);
}

/**************************************************************************/

static const struct {
    const char *name;
    int       (*print)(const char*, int);
    int         arg;
} handlers[] = {

    { sect_lc_time,     print_lc_time, 0 },
    { sect_lc_monetary, print_lc_monetary, 0 },
    { sect_lc_numeric,  print_lc_numeric, 0 },
    { sect_lc_ctype,    print_lc_ctype, 0 },
    { sect_lc_collate,  print_lc_collate, 0 },
    { sect_lc_messages, print_lc_messages, 0 },
    { sect_lc_all,      print_lc_all, 0 },

    { "abday", print_abday, -1 },
    { "day",  print_day, -1 },
    { "abmon", print_abmon, -1 },
    { "mon", print_mon, -1 },
    { "am_pm", print_am_pm, -1 },
    { "d_t_fmt", print_d_t_fmt, -1 },
    { "t_fmt", print_t_fmt, -1 },
    { "d_fmt", print_d_fmt, -1 },
    { "t_fmt_ampm", print_t_fmt_ampm, -1 },
    { "era_d_t_fmt", print_era_d_t_fmt, -1 },
    { "era_t_fmt", print_era_t_fmt, -1 },
    { "era_d_fmt", print_era_d_fmt, -1 },
    { "era", print_era, -1 },
    { "alt_digits", print_alt_digits, -1 },
    { "yesexpr", print_yesexpr, -1 },
    { "noexpr", print_noexpr, -1 },
    { "int_curr_symbol", print_int_curr_symbol, -1 },
    { "currency_symbol", print_currency_symbol, -1 },
    { "mon_decimal_point", print_mon_decimal_point, -1 },
    { "mon_thousands_sep", print_mon_thousands_sep, -1 },
    { "mon_grouping", print_mon_grouping, -1 },
    { "positive_sign", print_positive_sign, -1 },
    { "negative_sign", print_negative_sign, -1 },
    { "int_frac_digits", print_int_frac_digits, -1 },
    { "frac_digits", print_frac_digits, -1 },
    { "p_cs_precedes", print_p_cs_precedes, -1 },
    { "p_sep_by_space", print_p_sep_by_space, -1 },
    { "n_cs_precedes", print_n_cs_precedes, -1 },
    { "n_sep_by_space", print_n_sep_by_space, -1 },
    { "p_sign_posn", print_p_sign_posn, -1 },
    { "n_sign_posn", print_n_sign_posn, -1 },
    { "int_p_cs_precedes", print_int_p_cs_precedes, -1 },
    { "int_p_sep_by_space", print_int_p_sep_by_space, -1 },
    { "int_n_cs_precedes", print_int_n_cs_precedes, -1 },
    { "int_n_sep_by_space", print_int_n_sep_by_space, -1 },
    { "int_p_sign_posn", print_int_p_sign_posn, -1 },
    { "int_n_sign_posn", print_int_n_sign_posn, -1 },
    { "decimal_point", print_decimal_point, -1 },
    { "thousands_sep", print_thousands_sep, -1 },
    { "grouping", print_grouping, -1 },
    { "upper", print_mask, std::ctype_base::upper },
    { "lower", print_mask, std::ctype_base::lower },
    { "space", print_mask, std::ctype_base::space },
    { "print", print_mask, std::ctype_base::print },
    { "cntrl", print_mask, std::ctype_base::cntrl },
    { "alpha", print_mask, std::ctype_base::alpha },
    { "digit", print_mask, std::ctype_base::digit },
    { "punct", print_mask, std::ctype_base::punct },
    { "graph", print_mask, std::ctype_base::graph },
    { "xdigit", print_mask, std::ctype_base::xdigit },
    { "tolower", print_tolower, -1 },
    { "toupper", print_toupper, -1 },
    { "charmap", print_charmap_name, 0 },

    // sentinel
    { 0, 0, 0 }
};


int locale_main (int argc, char *argv[])
{
    const char* const program_name = argv [0];

    if (1 == argc) {
        // print all localization environment variables
        print_lc_vars ();
    }
    else {
        init_sections ();

        --argc;

        while (0 != *++argv && 0 < argc-- && '-' == **argv) {

            switch (*++*argv) {

            case 'a':
                // -a: print the names of all installed locales
                print_locale_names ();
                return EXIT_SUCCESS;

            case 'k':
                // -k: print the names of keywords when printing
                //     their values
                print_keywords = true;
                break;

            case 'c':
                // -c[k]: print the name of each section
                print_sect_names = true;
                if (*(*argv + 1) == 'k')
                    print_keywords = true;
                break;

            case 'm':
                print_charmap_names ();
                return EXIT_SUCCESS;

            case 'h':
                // -h: use character names from the original charmap
                //     used by localedef to build the locale (if possible)
                decode = true;
                break;

            case 'l':
                // -l: produce output suitable as input for processing
                //     by the localedef utility
                posix_output = false;
                break;

            case 'p':
                // -p: produce output using symbols from the Portable
                //     Character Set whenever possible
                use_pcs      = true;
                posix_output = false;
                break;

            case 'w':
                if (std::strlen (*argv) == 1)
                    issue_diag (W_DISABLE, 0, 0, 0);
                else
                    issue_diag (std::atoi (++*argv), 0, 0, 0);
                break;

            case '?':
                print_help ();
                return EXIT_SUCCESS;

            case '-':
                if (0 == std::strcmp (*argv, "-help")) {
                    print_help ();
                    return EXIT_SUCCESS;
                }

                if (0 == std::strcmp (*argv, "-charmap")) {
                    print_charmap ();
                    return EXIT_SUCCESS;
                }

                // fall through...

            default:
                issue_diag (504, true, 0, "%s: invalid option "
                            "-%s\n", program_name, *argv);
            }
        }

        if (0 == *argv) {
            issue_diag (505, true, 0, "%s: missing argument\n Try '"
                        "%s --help'\n", program_name, program_name);
        }

        for (; 0 != *argv; ++argv) {

            for (std::size_t i = 0; handlers [i].name; ++i) {
                if (0 == std::strcmp (*argv, handlers [i].name)) {

                    const int arg =
                        -1 == handlers [i].arg ? print_sect_names
                        : handlers [i].arg;

                    handlers [i].print (handlers [i].name, arg);
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
