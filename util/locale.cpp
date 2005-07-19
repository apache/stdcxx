/***************************************************************************
 *
 * locale.cpp
 *
 * $Id: //stdlib/dev/source/stdlib/util/locale.cpp#110 $
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

#if defined(__linux__) && !defined (_XOPEN_SOURCE) 
   // on Linux define _XOPEN_SOURCE to get CODESET defined in <langinfo.h>
#  define _XOPEN_SOURCE 500   /* Single UNIX conformance */
#endif   // __linux__

#include <rw/_defs.h>
#include _RWSTD_SYS_TYPES_H

#ifndef _MSC_VER
#  include <sys/mman.h>
#  include <unistd.h>    // for close ()
#  include <iconv.h>
#  include <langinfo.h>
#else
#  include <io.h>        // for open()
#  include <windows.h>
#endif  // _MSC_VER

#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <locale>
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <cerrno>
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


bool print_cat_names = false;
bool print_keywords = false;
bool decode = false;
bool is_utf8 = true;
bool POSIX_output = true;

const _RW::__rw_codecvt_t* codecvt_st   = 0;
const _RW::__rw_collate_t* collate_st   = 0;
const _RW::__rw_ctype_t*   ctype_st     = 0;
const _RW::__rw_time_t*    time_st      = 0;
const _RW::__rw_num_t*     num_st       = 0;
const _RW::__rw_mon_t*     mon_st       = 0;
const _RW::__rw_punct_t*   num_punct_st = 0;
const _RW::__rw_punct_t*   mon_punct_st = 0;
const _RW::__rw_messages_t* messages_st = 0;

Charmap *collate_charmap  = 0;
Charmap *ctype_charmap    = 0;
Charmap *time_charmap     = 0;
Charmap *num_charmap      = 0;
Charmap *mon_charmap      = 0;
Charmap *messages_charmap = 0;

const char* current_locales[8];
bool is_env_set[8];

// length of the largest string you want printed
#define MAX_LINE_LEN 50


int validate (const void *addr, std::size_t nbytes)
{
    const std::size_t size = memchk (addr, nbytes);

    if (nbytes != size) {
        if (size)
            issue_diag (E_INVAL, true, 0,
                        "invalid size at %p: %u, expected %u\n",
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
initialize_struct (const std::string &loc_path_root,
                   const char *all_val, const char *lang_val,
                   const char *cat_name, const char *codeset_name)
{
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

#ifndef _MSC_VER
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

#endif  // _MSC_VER

            return ret;
        }
    }

    return 0;
}


template <class FacetData>
const FacetData*
initialize (const std::string &loc_path_root,
            const char *all_val, const char *lang_val,
            const char *cat_name, const char *codeset_name,
            FacetData*)
{
    const void* const ptr =
        initialize_struct (loc_path_root, all_val, lang_val,
                           cat_name, codeset_name);

    if (0 == validate (ptr, sizeof (FacetData)))
        return static_cast<const FacetData*>(ptr);

    return 0;
}


void initialize_structs ()
{
    std::string loc_path_root;

    // if the locale root environment is not set
    // use the current directory as the locale root
    const char* const root = std::getenv ("RWSTD_LOCALE_ROOT");

    loc_path_root  = root ? root : ".";
    loc_path_root += _RWSTD_PATH_SEP;

    // check to see if the LC_ALL or LC_LANG environment was set.
    // if LC_LANG was not set it defaults to "C"
    const char* const all_val  = std::getenv ("LC_ALL");
    const char*       lang_val = std::getenv ("LANG");
    if (0 == lang_val)
        lang_val = "C";

    // points to the name of the codeset used by all non-empty
    // locale categories, otherwise 0
    const char *codeset_name = 0;

    // points to the name of the LC_XXX environment variable
    // that refers to the collate category whose codeset is
    // described by codeset_name; the value of the variable
    // is used to determine the location of the codeset
    // conversion database file, e.g., when the environment
    // variable RWSTD_LOCALE_ROOT is not defined and locale
    // is invoked like so:
    // LC_CTYPE=/foo/bar/somelocale locale --charmap
    const char *lc_cat_envvar = 0;

    // initialize collate_st
    collate_st = initialize (loc_path_root,
                             all_val, lang_val,
                             "LC_COLLATE", 0,
                             (_RW::__rw_collate_t*)0);

    if (collate_st) {
        validate (collate_st->codeset_name ());

        codeset_name  = collate_st->codeset_name ();
        lc_cat_envvar = "LC_COLLATE";
    }

    // initialize ctype_st
    ctype_st = initialize (loc_path_root,
                           all_val, lang_val,
                           "LC_CTYPE", 0,
                           (_RW::__rw_ctype_t*)0);

    if (ctype_st) {
        validate (ctype_st->codeset_name ());

        if (codeset_name) {
            if (std::strcmp (codeset_name, ctype_st->codeset_name ()))
                codeset_name = 0;
        }
        else {
            codeset_name  = ctype_st->codeset_name ();
            lc_cat_envvar = "LC_CTYPE";
        }
    }

    // initialize mon_st
    mon_punct_st = initialize (loc_path_root,
                               all_val, lang_val,
                               "LC_MONETARY", 0,
                               (_RW::__rw_punct_t*)0);

    if (0 != mon_punct_st) {
        mon_st = _RWSTD_STATIC_CAST (const _RW::__rw_mon_t*,
                                     mon_punct_st->get_ext());
        validate (mon_st, sizeof *mon_st);
    }

    if (mon_st) {
        validate (mon_st->codeset_name ());

        if (codeset_name) {
            if (std::strcmp (codeset_name, mon_st->codeset_name ()))
                codeset_name = 0;
        }
        else {
            codeset_name  = mon_st->codeset_name ();
            lc_cat_envvar = "LC_MONETARY";
        }
    }

    // initialize num_st
    num_punct_st = initialize (loc_path_root,
                               all_val, lang_val,
                               "LC_NUMERIC", 0,
                               (_RW::__rw_punct_t*)0);
    if (0 != num_punct_st) {
        num_st = _RWSTD_STATIC_CAST (const _RW::__rw_num_t*,
                                     num_punct_st->get_ext());
        validate (num_st, sizeof *mon_st);
    }

    if (num_st) {
        validate (num_st->codeset_name ());

        if (codeset_name) {
            if (std::strcmp (codeset_name, num_st->codeset_name ()))
                codeset_name = 0;
        }
        else {
            codeset_name  = num_st->codeset_name ();
            lc_cat_envvar = "LC_NUMERIC";
        }
    }

    // initialize time_st
    time_st = initialize (loc_path_root,
                          all_val, lang_val,
                          "LC_TIME", 0,
                          (_RW::__rw_time_t*)0);
    if (time_st) {
        validate (time_st->codeset_name ());

        if (codeset_name) {
            if (std::strcmp (codeset_name, time_st->codeset_name ()))
                codeset_name = 0;
        }
        else {
            codeset_name  = time_st->codeset_name ();
            lc_cat_envvar = "LC_TIME";
        }
    }

    // initialize messages_st
    messages_st = initialize (loc_path_root,
                              all_val, lang_val,
                              "LC_MESSAGES", 0,
                              (_RW::__rw_messages_t*)0);

    if (messages_st) {
        validate (messages_st->codeset_name ());

        if (codeset_name) {
            if (std::strcmp (codeset_name, messages_st->codeset_name ()))
                codeset_name = 0;
        }
        else {
            codeset_name  = messages_st->codeset_name ();
            lc_cat_envvar = "LC_MESSAGES";
        }
    }

    if (codeset_name) {

        // initialize codecvt_st
        codecvt_st = initialize (loc_path_root,
                                 all_val, lang_val,
                                 lc_cat_envvar,
                                 codeset_name,
                                 (_RW::__rw_codecvt_t*)0);
    }
}


Charmap* get_charmap (const char* name)
{
    if (ctype_charmap && ctype_charmap->get_charmap_name () == name)
        return ctype_charmap;

    if (collate_charmap && collate_charmap->get_charmap_name () == name)
        return collate_charmap;

    if (time_charmap && time_charmap->get_charmap_name () == name)
        return time_charmap;

    if (mon_charmap && mon_charmap->get_charmap_name () == name)
        return mon_charmap;

    if (num_charmap && num_charmap->get_charmap_name () == name)
        return num_charmap;

    // look for the charmap directory
    const char* const src_root = std::getenv ("RWSTD_SRC_ROOT");
    if (0 != src_root) {
        std::string charmap_path = src_root;
        charmap_path = charmap_path + "/charmaps/" + name;

        // search for a C library locale that uses the same encoding
        std::string std_encoding (name);

# if !defined(_MSC_VER)
        std::string C_locale (get_C_encoding_locale (std_encoding));
# else
        std::string C_locale ("");
# endif

        return new Charmap (C_locale.c_str (), charmap_path.c_str(),
                            0 == std::strcmp ("utf8.cm", name),
                            false, true, false);
    }

    issue_diag (503, true, 0, "RWSTD_SRC_ROOT not set, giving up\n");
    return 0;
}


std::string create_str (const char* str, const wchar_t* w_str,
                        Charmap* charmap)
{
    assert (0 != str);

    typedef std::map <unsigned char, std::string>::const_iterator rn_cmap_iter;
    typedef std::map <wchar_t, std::string>::const_iterator rw_cmap_iter;

    if (0 == charmap)
        return str;

    std::string ret;
    if (0 == ctype_st || ctype_st->mb_cur_max == 1) {
        // if we were given a charmap file we will look up each character in
        // the charmap to get the symbolic name associated with the value
        while (*str != '\0'){
            rn_cmap_iter ch_pos = charmap->get_rn_cmap().find
                ((unsigned char)*str);
            if (ch_pos != charmap->get_rn_cmap().end()) {
                ret += ch_pos->second;
            }
                str++;
        }
    }
    else {
        while (*w_str != '\0') {
            rw_cmap_iter ch_wpos = charmap->get_rw_cmap().find (*w_str);
            if (ch_wpos != charmap->get_rw_cmap().end()) {
                ret += ch_wpos->second;
            }
            w_str++;
        }
    }
    return ret;
}

std::string create_str (wchar_t ch, Charmap* charmap)
{
    typedef std::map <wchar_t, std::string>::const_iterator rw_cmap_iter;

    std::string ret;

    if (charmap) {
            rw_cmap_iter ch_pos = charmap->get_rw_cmap().find (ch);
            if (ch_pos != charmap->get_rw_cmap().end())
                ret += ch_pos->second;
            else {
                issue_diag (501, true, 0, "%d character in locale database "
                            "could not be found in character map file\n", ch);
            }
    }
    else {
        ret += (unsigned char)ch;
    }
    return ret;

}

std::string create_keyword (const char* str)
{
    std::string keyword_str = str;
    if (POSIX_output)
        keyword_str += '=';
    else
        keyword_str += ' ';

    return (print_keywords ? keyword_str : "");
}

const char* set_locale (const std::string& charmap_name)
{
#ifndef _MSC_VER
    std::string encoding (charmap_name.substr (0, charmap_name.rfind ('.')));

    std::vector<std::string>aliases;
    get_cname_aliases (encoding, aliases);

    std::vector<std::string>::iterator pos;

    for (pos = aliases.begin(); !is_utf8 && pos != aliases.end(); pos++)
        if (*pos == "utf8")
            is_utf8 = true;

    const char* names = get_installed_locales ();

    for (; *names; names += std::strlen (names) + 1) {

        if (0 != std::setlocale (LC_CTYPE, names)) {
            std::string codeset = nl_langinfo(CODESET);
            for (pos = aliases.begin(); pos != aliases.end(); pos++)
                if (codeset == *pos)
                    return names;
        }
    }
#endif  // _MSC_VER
    return 0;

}


// place holder for function that prints the "C" locale LC_COLLATE category
void print_c_lc_collate ()
{
    assert (0);
}

wchar_t get_wchar_from_offset(unsigned int offset,
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

void print_weight (const unsigned int* weightp, 
                   unsigned int        num_weights,
                   unsigned int        longest_weight)
{
    // FIXME: ignore the order of the element
    weightp++;

    for (unsigned int k = 0; k < num_weights; k++) {
        bool quoted = false;

        if (*weightp && longest_weight > 1) {
            quoted = true;
            std::cout << "\"";
        }

        for (unsigned int x = 0; x < longest_weight; x++){
            if (*weightp != UINT_MAX) {
                if (*weightp == 0) 
                    std::cout << "IGNORE";
                else
                    std::cout << "\\d" << *weightp;
            }
            weightp++;
        }

        if (quoted)
            std::cout << "\"";

        if (k != num_weights)
            std::cout << ";";
    }

    std::cout << '\n';
}

void write_coll_info (const std::string &ch, unsigned int idx,
                      unsigned int tab_num)
{
    if (collate_st->num_elms > 1) {
        typedef std::map <std::string, wchar_t>::const_iterator n_cmap2_iter;
        const unsigned int* tab = collate_st->get_n_tab (tab_num);
        unsigned int first = collate_st->get_first_char_in_n_tab(tab_num);
        for (unsigned int i = first; i <= UCHAR_MAX; i++) {
            std::string new_ch = ch;
            new_ch += (unsigned char)i;
            if (tab[i - first] != UINT_MAX) {
                if (tab[i - first] & 0x80000000) {
                    // it's an offset to another table
                    write_coll_info (new_ch, idx + 1,
                                     tab[i - first] &~ 0x80000000);
                }
                else {
                    // we found the offset to the weight
                    if (collate_charmap) {
                        // first we need to print the symbolic name
                        n_cmap2_iter n_cmap2_it =
                            collate_charmap->get_n_cmap2().find(new_ch);
                        if (n_cmap2_it != collate_charmap->get_n_cmap2().end())
                            std::cout << collate_charmap->get_rw_cmap().find(
                                n_cmap2_it->second)->second;
                        else
                            ;
                    }
                    else {
                        for (unsigned int j = 0; j < idx; j++) {
                            std::cout << "\\d" << (unsigned int)
                                (unsigned char)new_ch[j];
                            if (j != idx - 1)
                                std::cout << ";";
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

void print_ce_info (unsigned int tab_num, const std::string& mb_str,
                    std::map<std::string, unsigned int> &ce_map) {
    static unsigned int ce_num = 0;
    const unsigned int *tab = collate_st->get_n_ce_tab (tab_num);
    unsigned char first = collate_st->get_first_char_in_n_ce_tab (tab_num);
    unsigned char last = collate_st->get_last_char_in_n_ce_tab (tab_num);
    std::string cur_mb;
    for (unsigned int i = first; i <= last; i++) {
        cur_mb = mb_str;
        cur_mb += (unsigned char) i;
        if (tab[i-first] != UINT_MAX) {
            if (tab[i-first] & 0x80000000) {
                // it's an offset to another table. recursively call
                // print_ce_info with the new table number.

                print_ce_info (tab[i-first] &~ 0x80000000, cur_mb, ce_map);
            }
            else {
                // we found the end of the collating element
                // now we need to do a lookup in the narrow character maps to
                // find the symbolic names for the characters that make up
                // this collating element.

                // sym is sized at 13 because there will never be more then
                // 99,999 collating elements
                char sym[13];
                std::sprintf (sym, "<RW_CE_%d>", ce_num++);

                ce_map.insert (std::make_pair (sym, tab[i-first]));
                std::cout << "collating-element " << sym << " from \"";

                if (0 == collate_charmap)
                    std::cout << cur_mb << "\"\n";
                else {
                    unsigned int first_char = 0;
                    std::map<std::string, wchar_t>::const_iterator it;
                    for (unsigned int j = 1; j <= cur_mb.size(); j++) {
                        if ((it = collate_charmap->get_n_cmap2().find
                             (cur_mb.substr (first_char, j)))
                            != collate_charmap->get_n_cmap2().end()) {
                            std::cout << (collate_charmap->get_rw_cmap().find
                                          (it->second))->second;
                            first_char = j;
                        }

                    }
                    std::cout << "\"\n";
                }
            }
        }
    }
}


void print_lc_collate ()
{
    if (print_cat_names)
        std::cout << "LC_COLLATE\n";

    if (0 == collate_st) {
        //print_c_lc_collate();
    }
    else {
        // if the collate_charmap has not been initialized then initialize it
        if (0 == collate_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[1]))
                set_locale (collate_st->charmap_name());
#endif  // _MSC_VER
            collate_charmap = get_charmap (collate_st->charmap_name());
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
                std::cout << std::endl;
            else
                std::cout << ';';
        }

        // Print out the characters and their weights in
        // decimal format


        // first print out the collating elements
        for (ce_map_iter ce_map_it = ce_map.begin();
             ce_map_it != ce_map.end(); ce_map_it++) {
            std::cout << ce_map_it->first << "  ";
            print_weight (collate_st->get_weight (ce_map_it->second),
                          collate_st->num_weights,
                          collate_st->longest_weight);
        }

        std::string ch;
        write_coll_info (ch, 0, 0);
        if (collate_st->undefined_optimization) {
            std::cout << "UNDEFINED ";
            print_weight (collate_st->get_weight (collate_st->undefined_weight_idx),
                          collate_st->num_weights, collate_st->longest_weight);
        }

        std::cout << "\norder_end\n";
    }

    if (print_cat_names)
        std::cout << "END LC_COLLATE\n";

}

//  print_mask takes in a ctype_base::mask value and prints all the
// characters in that mask category.  If a charmap was given on the
// command line then it will look up the symbolic name and print that instead
void print_mask(std::ctype_base::mask m, bool print_cat)
{
    std::string out;

    if (print_cat)
        std::cout << "LC_CTYPE\n";
    switch (m){
    case std::ctype_base::upper:
        std::cout << create_keyword("upper");
        break;
    case std::ctype_base::lower:
        std::cout << create_keyword("lower");
        break;
    case std::ctype_base::space:
        std::cout << create_keyword("space");
        break;
    case std::ctype_base::print:
        std::cout << create_keyword("print");
        break;
    case std::ctype_base::alpha:
        std::cout << create_keyword("alpha");
        break;
    case std::ctype_base::cntrl:
        std::cout << create_keyword("cntrl");
        break;
    case std::ctype_base::punct:
        std::cout << create_keyword("punct");
        break;
    case std::ctype_base::graph:
        std::cout << create_keyword("graph");
        break;
    case std::ctype_base::digit:
        std::cout << create_keyword("digit");
        break;
    case std::ctype_base::xdigit:
        std::cout << create_keyword("xdigit");
        break;
    default:
        break;
    }
    if (0 != ctype_st) {
        // if the ctype_charmap has not been initialized then initialize it
        if (0 == ctype_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[0]))
                set_locale (ctype_st->charmap_name());
#endif  // _MSC_VER
            ctype_charmap = get_charmap (ctype_st->charmap_name());
        }
        // go through the entire mask_table and print out each
        // character that has this mask value.
        for (size_t k = 0; k < ctype_st->wmask_s; k++) {
            if (ctype_st->wmask_tab(k).mask & m) {
                out += create_str (ctype_st->wmask_tab(k).ch, ctype_charmap);
                for (size_t j = k + 1; j < ctype_st->wmask_s; j++)
                    if (ctype_st->wmask_tab(j).mask & m) {
                        if (out.size() >= MAX_LINE_LEN && !POSIX_output)
                            out += ";\\";
                        else
                            out += ';';
                        break;
                    }
            }
            if (out.size() > MAX_LINE_LEN && !POSIX_output) {
                std::cout << out << '\n';
                out.clear();

            }
        }
        std::cout << out;
    }

    std::cout << '\n';
}


void print_toupper (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_CTYPE\n";

    std::cout << create_keyword ("toupper");

    std::string out;
    if (0 != ctype_st) {
        // if the ctype_charmap has not been initialized then initialize it
        if (0 == ctype_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[0]))
                set_locale (ctype_st->charmap_name());
#endif  // _MSC_VER
            ctype_charmap = get_charmap (ctype_st->charmap_name());
        }
        // print out each pair in the toupper table.
        // the wide char table is used because it contains all characters
        for (size_t j = 0; j < ctype_st->wtoupper_s(); j++) {
            out = out + "(" + create_str (ctype_st->wtoupper_tab(j).lower,
                                          ctype_charmap)
                + "," + create_str (ctype_st->wtoupper_tab(j).upper,
                                    ctype_charmap) + ")";
            if (j != ctype_st->wtoupper_s() - 1) {
                if (out.size() >= MAX_LINE_LEN && !POSIX_output)
                    out += ";\\";
                else
                    out += ';';
            }
            if (out.size() > MAX_LINE_LEN && !POSIX_output) {
                std::cout << out << '\n';
                out.clear();
            }
        }
        std::cout << out;
    }

    std::cout << '\n';
}


void print_tolower (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_CTYPE\n";

    std::cout << create_keyword ("tolower");

    std::string out;

    if (0 != ctype_st) {

        // if the ctype_charmap has not been initialized then initialize it
        if (0 == ctype_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[0]))
                set_locale (ctype_st->charmap_name());
#endif  // _MSC_VER
            ctype_charmap = get_charmap (ctype_st->charmap_name());
        }
        for (size_t j = 0; j < ctype_st->wtolower_s(); j++) {
            out = out + "(" + create_str (ctype_st->wtolower_tab(j).upper,
                                          ctype_charmap)
                + "," + create_str (ctype_st->wtolower_tab(j).lower,
                                    ctype_charmap) + ")";
            if (j != ctype_st->wtolower_s() - 1) {
                if (out.size() >= MAX_LINE_LEN && !POSIX_output)
                    out += ";\\";
                else
                    out += ';';
            }
            if (out.size() > MAX_LINE_LEN && !POSIX_output) {
                std::cout << out << '\n';
                out.clear();
            }
        }
        std::cout << out;
    }

    std::cout << '\n';
}


void print_lc_ctype ()
{
    if (print_cat_names)
        std::cout << "LC_CTYPE\n";

    print_mask (std::ctype_base::upper, false);
    print_mask (std::ctype_base::lower, false);
    print_mask (std::ctype_base::space, false);
    print_mask (std::ctype_base::print, false);
    print_mask (std::ctype_base::cntrl, false);
    print_mask (std::ctype_base::alpha, false);
    print_mask (std::ctype_base::digit, false);
    print_mask (std::ctype_base::punct, false);
    print_mask (std::ctype_base::graph, false);
    print_mask (std::ctype_base::xdigit, false);
    print_toupper (false);
    print_tolower (false);

    if (print_cat_names)
        std::cout << "END LC_CTYPE\n";
}


void print_decimal_point (bool print_cat)
{
    if (print_cat)
      std::cout << "LC_NUMERIC\n";
    std::cout << create_keyword ("decimal_point");
    if (0 == num_st)
        std::cout << "\"" << create_str(".", 0, 0) << "\"\n";
    else {
        // if the num_charmap has not been initialized then initialize it
        if (0 == num_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[4]))
                set_locale (num_st->charmap_name());
#endif  // _MSC_VER
            num_charmap = get_charmap (num_st->charmap_name());
        }
        std::cout <<  "\""
                  << create_str (_RWSTD_STATIC_CAST (const char*,
                                     num_punct_st->decimal_point (0)),
                                 _RWSTD_STATIC_CAST (const wchar_t*,
                                     num_punct_st->decimal_point (1)),
                                 num_charmap)
                  << "\"\n";
    }

}
void print_thousands_sep (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_NUMERIC\n";
    std::cout << create_keyword ("thousands_sep");
    if (0 == num_st)
        std::cout << "\"\"\n";
    else {
        // if the num_charmap has not been initialized then initialize it
        if (0 == num_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[4]))
                set_locale (num_st->charmap_name());
#endif  // _MSC_VER
            num_charmap = get_charmap (num_st->charmap_name());
        }

        std::cout <<  "\""
                  << create_str (_RWSTD_STATIC_CAST
                                 (const char*,
                                  num_punct_st->thousands_sep(false)),
                                 _RWSTD_STATIC_CAST
                                 (const wchar_t*,
                                  num_punct_st->thousands_sep(true)),
                                 num_charmap)
                  << "\"\n";
    }
}

void print_grouping (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_NUMERIC\n";
    std::cout << create_keyword ("grouping");
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
}
void print_lc_numeric ()
{
    if (print_cat_names)
        std::cout << "LC_NUMERIC\n";

        print_decimal_point (false);
        print_thousands_sep (false);
        print_grouping (false);

    if (print_cat_names)
        std::cout << "END LC_NUMERIC\n";

}

void print_int_curr_symbol (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("int_curr_symbol");
    if (0 == mon_st)
        std::cout << "\"\"\n";
    else {
        // if the mon_charmap has not been initialized then initialize it
        if (0 == mon_charmap && decode)
            mon_charmap = get_charmap (mon_st->charmap_name());
        std::cout << "\"" << create_str (_RWSTD_STATIC_CAST (const char*,
                                             mon_st->curr_symbol (1, 0)),
                                         _RWSTD_STATIC_CAST (const wchar_t*,
                                             mon_st->curr_symbol (1, 1)),
                                         mon_charmap)
                  << "\"\n";
    }
}

void print_currency_symbol (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("currency_symbol");
    if (0 == mon_st)
        std::cout << "\"\"\n";
    else {
        // if the mon_charmap has not been initialized then initialize it
        if (0 == mon_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encodin
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[3]))
                set_locale (mon_st->charmap_name());
#endif  // _MSC_VER
            mon_charmap = get_charmap (mon_st->charmap_name());
        }

        std::cout << "\"" << create_str (_RWSTD_STATIC_CAST (const char*,
                                             mon_st->curr_symbol (0, 0)),
                                         _RWSTD_STATIC_CAST (const wchar_t*,
                                             mon_st->curr_symbol (0, 1)),
                                         mon_charmap)
                  << "\"\n";
    }
}

void print_mon_decimal_point (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout <<  create_keyword ("mon_decimal_point");
    if (0 == mon_st)
        std::cout << "\"\"\n";
    else {
        // if the mon_charmap has not been initialized then initialize it
        if (0 == mon_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[3]))
                set_locale (mon_st->charmap_name());
#endif  // _MSC_VER
            mon_charmap = get_charmap (mon_st->charmap_name());
        }

        std::cout << "\""
                  << create_str (_RWSTD_STATIC_CAST (const char*,
                                     mon_punct_st->decimal_point (0)),
                                 _RWSTD_STATIC_CAST (const wchar_t*,
                                     mon_punct_st->decimal_point (1)),
                                 mon_charmap)
                  << "\"\n";
    }
}

void print_mon_thousands_sep (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("mon_thousands_sep");
    if (0 == mon_st)
        std::cout << "\"\"\n";
    else {
        // if the mon_charmap has not been initialized then initialize it
        if (0 == mon_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[3]))
                set_locale (mon_st->charmap_name());
#endif  // _MSC_VER
            mon_charmap = get_charmap (mon_st->charmap_name());
        }

        std::cout << "\""
                  << create_str (_RWSTD_STATIC_CAST
                                 (const char*,
                                  mon_punct_st->thousands_sep(false)),
                                 _RWSTD_STATIC_CAST
                                 (const wchar_t*,
                                  mon_punct_st->thousands_sep(true)),
                                 mon_charmap)
                  << "\"\n";
    }
}

void print_mon_grouping (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("mon_grouping");
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
}

void print_positive_sign (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("positive_sign");
    if (0 == mon_st)
        std::cout << "\"\"\n";
    else {
        // if the mon_charmap has not been initialized then initialize it
        if (0 == mon_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[3]))
                set_locale (mon_st->charmap_name());
#endif  // _MSC_VER
            mon_charmap = get_charmap (mon_st->charmap_name());
        }

        std::cout << "\"" << create_str (_RWSTD_STATIC_CAST (const char*,
                                             mon_st->positive_sign (0)),
                                         _RWSTD_STATIC_CAST (const wchar_t*,
                                             mon_st->positive_sign (1)),
                                         mon_charmap)
                  << "\"\n";
    }
}

void print_negative_sign (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("negative_sign");
    if (0 == mon_st)
        std::cout << "\"\"\n";
    else {
        // if the mon_charmap has not been initialized then initialize it
        if (0 == mon_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[3]))
                set_locale (mon_st->charmap_name());
#endif  // _MSC_VER
            mon_charmap = get_charmap (mon_st->charmap_name());
        }

        std::cout << "\"" << create_str (_RWSTD_STATIC_CAST (const char*,
                                             mon_st->negative_sign (0)),
                                         _RWSTD_STATIC_CAST (const wchar_t*,
                                             mon_st->negative_sign (1)),
                                         mon_charmap)
                  << "\"\n";
    }

}

void print_int_frac_digits (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("int_frac_digits");

    signed char c = 0;

    if (0 == mon_st)
        c = -1;
    else
        c = mon_st->frac_digits [1];
    
    std::cout << int(c) << '\n';
}

void print_frac_digits (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("frac_digits");

    signed char c = 0;

    if (0 == mon_st)
        c = -1;
    else
        c = int(mon_st->frac_digits [0]);

    std::cout << int(c) << '\n';
}

void print_p_cs_precedes (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("p_cs_precedes");

    signed char c = 0;

    if (0 == mon_st)
        c = -1;
    else
        c = int(mon_st->p_cs_precedes [0]);

    std::cout << int(c) << '\n';
}

void print_n_sep_by_space (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("n_sep_by_space");

    signed char c = 0;

    if (0 == mon_st)
        c = -1;
    else
        c = int(mon_st->n_sep_by_space [0]);

    std::cout << int(c) << '\n';
}

void print_p_sep_by_space (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("p_sep_by_space");

    signed char c = 0;

    if (0 == mon_st)
        c = -1;
    else
        c = int(mon_st->p_sep_by_space [0]);

    std::cout << int(c) << '\n';
}

void print_n_cs_precedes (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("n_cs_precedes");

    signed char c = 0;

    if (0 == mon_st)
        c = -1;
    else
        c = int(mon_st->n_cs_precedes [0]);

    std::cout << int(c) << '\n';
}

void print_p_sign_posn (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";

    signed char c = 0;

    std::cout << create_keyword ("p_sign_posn");
    if (0 == mon_st)
        c = -1;
    else
        c = int(mon_st->p_sign_posn [0]);

    std::cout << int(c) << '\n';
}

void print_n_sign_posn (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";

    signed char c = 0;

    std::cout << create_keyword ("n_sign_posn");
    if (0 == mon_st)
        c = -1;
    else
        c = int(mon_st->n_sign_posn [0]);

    std::cout << int(c) << '\n';
}

void print_int_p_cs_precedes (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("int_p_cs_precedes");

    signed char c = 0;

    if (0 == mon_st)
        c = -1;
    else
        c = int(mon_st->p_cs_precedes [1]);

    std::cout << int(c) << '\n';
}

void print_int_n_sep_by_space (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("int_n_sep_by_space");

    signed char c = 0;

    if (0 == mon_st)
        c = -1;
    else
        c = int(mon_st->n_sep_by_space [1]);

    std::cout << int(c) << '\n';
}

void print_int_p_sep_by_space (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("int_p_sep_by_space");

    signed char c = 0;

    if (0 == mon_st)
        c = -1;
    else
        c = int(mon_st->p_sep_by_space [1]);

    std::cout << int(c) << '\n';
}

void print_int_n_cs_precedes (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("int_n_cs_precedes");

    signed char c = 0;

    if (0 == mon_st)
        c = -1;
    else
        c = int(mon_st->n_cs_precedes [1]);

    std::cout << int(c) << '\n';
}

void print_int_p_sign_posn (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("int_p_sign_posn");

    signed char c = 0;

    if (0 == mon_st)
        c = -1;
    else
        c = int(mon_st->p_sign_posn [1]);

    std::cout << int(c) << '\n';
}

void print_int_n_sign_posn (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MONETARY\n";
    std::cout << create_keyword ("int_n_sign_posn");

    signed char c = 0;

    if (0 == mon_st)
        c = -1;
    else
        c = int(mon_st->n_sign_posn [1]);

    std::cout << int(c) << '\n';
}

void print_lc_monetary ()
{
    if (print_cat_names)
        std::cout << "LC_MONETARY\n";

        print_int_curr_symbol (false);
        print_currency_symbol (false);
        print_mon_decimal_point (false);
        print_mon_thousands_sep (false);
        print_mon_grouping (false);
        print_positive_sign (false);
        print_negative_sign (false);
        print_int_frac_digits (false);
        print_frac_digits (false);
        print_p_cs_precedes (false);
        print_p_sep_by_space (false);
        print_n_cs_precedes (false);
        print_n_sep_by_space (false);
        print_p_sign_posn (false);
        print_n_sign_posn (false);
        print_int_p_cs_precedes (false);
        print_int_p_sep_by_space (false);
        print_int_n_cs_precedes (false);
        print_int_n_sep_by_space (false);
        print_int_p_sign_posn (false);
        print_int_n_sign_posn (false);


    if (print_cat_names)
        std::cout << "END LC_MONETARY\n";
}

void print_abday (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_TIME\n";
    std::cout << create_keyword ("abday");
    if (0 == time_st)
        std::cout << "\"" << create_str ("Sun", 0, 0) << "\";"
                  << "\"" << create_str ("Mon", 0, 0) << "\";"
                  << "\"" << create_str ("Tue", 0, 0) << "\";\\\n"
                  << "\"" << create_str ("Wed", 0, 0) << "\";"
                  << "\"" << create_str ("Thu", 0, 0) << "\";"
                  << "\"" << create_str ("Fri", 0, 0) << "\";"
                  << "\"" << create_str ("Sat", 0, 0) << "\"\n";
    else {
        // if the time_charmap has not been initialized then initialize it
        if (0 == time_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[2]))
                set_locale (time_st->charmap_name());
#endif  // _MSC_VER
            time_charmap = get_charmap (time_st->charmap_name());
        }
        for (unsigned int i = 0; i <= 6; i++) {
            std::cout << "\""
                      << create_str (_RWSTD_STATIC_CAST
                                     (const char*, time_st->abday (i, 0)),
                                     _RWSTD_STATIC_CAST
                                     (const wchar_t*, time_st->abday (i, 1)),
                                     time_charmap);
            if (i != 6)
                std::cout << "\";";
            else
                std::cout << "\"\n";
        }
    }

}

void print_day (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_TIME\n";
    std::cout << create_keyword ("day");
    if (0 == time_st)
        std::cout << "\"" << create_str ("Sunday", 0, 0) << "\";"
                  << "\"" << create_str ("Monday", 0, 0) << "\";"
                  << "\"" << create_str ("Tuesday", 0, 0) << "\";\\\n"
                  << "\"" << create_str ("Wednesday", 0, 0) << "\";"
                  << "\"" << create_str ("Thursday", 0, 0) << "\";"
                  << "\"" << create_str ("Friday", 0, 0) << "\";"
                  << "\"" << create_str ("Saturday", 0, 0) << "\"\n";

    else {
        // if the time_charmap has not been initialized then initialize it
        if (0 == time_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[2]))
                set_locale (time_st->charmap_name());
#endif  // _MSC_VER
            time_charmap = get_charmap (time_st->charmap_name());
        }
        for (unsigned int i = 0; i <= 6; i++) {
            std::cout << "\""
                      << create_str (_RWSTD_STATIC_CAST
                                     (const char*, time_st->day (i, 0)),
                                     _RWSTD_STATIC_CAST
                                     (const wchar_t*, time_st->day (i, 1)),
                                     time_charmap);
            if (i != 6)
                std::cout << "\";";
            else
                std::cout << "\"\n";
        }
    }
}


void print_abmon (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_TIME\n";
    std::cout << create_keyword ("abmon");
    if (0 == time_st) {
        std::cout << "\"" << create_str ("Jan", 0, 0) << "\";"
                  << "\"" << create_str ("Feb", 0, 0) << "\";"
                  << "\"" << create_str ("Mar", 0, 0) << "\";";
        if (!POSIX_output)
            std::cout << "\\\n";
        std::cout << "\"" << create_str ("Apr", 0, 0) << "\";"
                  << "\"" << create_str ("May", 0, 0) << "\";"
                  << "\"" << create_str ("Jun", 0, 0) << "\";";
        if (!POSIX_output)
            std::cout << "\\\n";
        std::cout << "\"" << create_str ("Jul", 0, 0) << "\";"
                  << "\"" << create_str ("Aug", 0, 0) << "\";"
                  << "\"" << create_str ("Sep", 0, 0) << "\";";
        if (!POSIX_output)
            std::cout << "\\\n";
        std::cout << "\"" << create_str ("Oct", 0, 0) << "\";"
                  << "\"" << create_str ("Nov", 0, 0) << "\";"
                  << "\"" << create_str ("Dec", 0, 0) << "\"\n";
    }
    else {
        // if the time_charmap has not been initialized then initialize it
        if (0 == time_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[2]))
                set_locale (time_st->charmap_name());
#endif  // _MSC_VER
            time_charmap = get_charmap (time_st->charmap_name());
        }

        for (unsigned int i = 0; i <= 11; i++) {
            std::cout << "\""
                      << create_str (_RWSTD_STATIC_CAST
                                     (const char*, time_st->abmon (i, 0)),
                                     _RWSTD_STATIC_CAST
                                     (const wchar_t*, time_st->abmon (i, 1)),
                                     time_charmap);
            if (i != 11)
                std::cout << "\";";
            else
                std::cout << "\"\n";

            if (!(i % 2) && !POSIX_output)
                std::cout << "\\\n" ;
        }
    }
}

void print_mon (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_TIME\n";
    std::cout << create_keyword ("mon");
    if (0 == time_st) {
        std::cout << "\"" << create_str ("January", 0, 0) << "\";"
                  << "\"" << create_str ("February", 0, 0) << "\";"
                  << "\"" << create_str ("March", 0, 0) << "\";";
        if (!POSIX_output)
            std::cout << "\\\n";
        std::cout << "\"" << create_str ("April", 0, 0) << "\";"
                  << "\"" << create_str ("May", 0, 0) << "\";"
                  << "\"" << create_str ("June", 0, 0) << "\";";
        if (!POSIX_output)
            std::cout << "\\\n";
        std::cout << "\"" << create_str ("July", 0, 0) << "\";"
                  << "\"" << create_str ("August", 0, 0) << "\";"
                  << "\"" << create_str ("September", 0, 0) << "\";";
        if (!POSIX_output)
            std::cout << "\\\n";
        std::cout << "\"" << create_str ("October", 0, 0) << "\";"
                  << "\"" << create_str ("November", 0, 0) << "\";"
                  << "\"" << create_str ("December", 0, 0) << "\"\n";
    }
    else {
        // if the time_charmap has not been initialized then initialize it
        if (0 == time_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[2]))
                set_locale (time_st->charmap_name());
#endif  // MSC_VER
            time_charmap = get_charmap (time_st->charmap_name());
        }

        for (unsigned int i = 0; i <= 11; i++) {
            std::cout << "\""
                      << create_str (_RWSTD_STATIC_CAST
                                     (const char*, time_st->mon (i, 0)),
                                     _RWSTD_STATIC_CAST
                                     (const wchar_t*, time_st->mon (i, 1)),
                                     time_charmap);
            if (i != 11)
                std::cout << "\";";
            else
                std::cout << "\"\n";

            if (!(i % 2) && !POSIX_output)
                std::cout << "\\\n" ;
        }
    }
}

void print_am_pm (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_TIME\n";
    std::cout << create_keyword ("am_pm");
    if (0 == time_st)
        std::cout << "\"" << create_str ("AM", 0, 0) << "\";\""
                  << create_str ("PM", 0, 0)
                  << "\"\n";
    else {
        // if the time_charmap has not been initialized then initialize it
        if (0 == time_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[2]))
                set_locale (time_st->charmap_name());
#endif  // _MSC_VER
            time_charmap = get_charmap (time_st->charmap_name());
        }

        std::cout << "\""
                  << create_str (_RWSTD_STATIC_CAST
                                 (const char*, time_st->am_pm (0,0)),
                                 _RWSTD_STATIC_CAST
                                 (const wchar_t*, time_st->am_pm (0, 1)),
                                 time_charmap) << "\";\""
                  << create_str (_RWSTD_STATIC_CAST
                                 (const char*, time_st->am_pm (1, 0)),
                                 _RWSTD_STATIC_CAST
                                 (const wchar_t*, time_st->am_pm (1, 1)),
                                 time_charmap)
                  << "\"\n";
    }
}


void print_era (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_TIME\n";

    std::cout << create_keyword ("era");

    if (0 == time_st)
        std::cout << "\"" << create_str ("", 0, 0)
                  << "\"\n";
    else if (0 == time_st->era_count ()) {
        // if there are no eras, print out the empty string
        std::cout << "\"\"\n";
    }
    else {
        // if the time_charmap has not been initialized then initialize it
        if (0 == time_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[2]))
                set_locale (time_st->charmap_name());
#endif  // _MSC_VER
            time_charmap = get_charmap (time_st->charmap_name());
        }

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

            // now print the end year, _RWSTD_INT_MIN is the beginning of time
            // and _RWSTD_INT_MAX is the end of time
            if (era_p->year[1] == _RWSTD_INT_MIN)
                std::cout << "-*:";
            else if (era_p->year[1] == _RWSTD_INT_MAX)
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

            // now print out the name of the era
            std::cout << create_str (_RWSTD_STATIC_CAST
                                     (const char*, time_st->era_name (i,0)),
                                     _RWSTD_STATIC_CAST
                                     (const wchar_t*, time_st->era_name(i, 1)),
                                     time_charmap) << ":";

            // finally print the format
            std::cout << create_str (_RWSTD_STATIC_CAST
                                     (const char*, time_st->era_fmt (i,0)),
                                     _RWSTD_STATIC_CAST
                                     (const wchar_t*, time_st->era_fmt(i, 1)),
                                     time_charmap);

            if (i != time_st->era_count() - 1)
                std::cout << "\";";
            else
                std::cout << "\"\n";
        }
    }
}


void print_d_t_fmt (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_TIME\n";
    std::cout << create_keyword ("d_t_fmt");
    if (0 == time_st)
        std::cout << "\"" << create_str ("%a %b %e %H:%M:%S %Y", 0, 0)
                  << "\"\n";
    else {
        // if the time_charmap has not been initialized then initialize it
        if (0 == time_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[2]))
                set_locale (time_st->charmap_name());
#endif  // _MSC_VER
            time_charmap = get_charmap (time_st->charmap_name());
        }

        std::cout << "\""
                  << create_str (_RWSTD_STATIC_CAST
                                 (const char*, time_st->d_t_fmt(0)),
                                 _RWSTD_STATIC_CAST
                                 (const wchar_t*, time_st->d_t_fmt(1)),
                                 time_charmap)
                  << "\"\n";
    }
}

void print_era_d_t_fmt (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_TIME\n";
    std::cout << create_keyword ("era_d_t_fmt");
    if (0 == time_st)
        std::cout << "\"" << create_str ("", 0, 0)
                  << "\"\n";
    else {
        // if the time_charmap has not been initialized then initialize it
        if (0 == time_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[2]))
                set_locale (time_st->charmap_name());
#endif  // _MSC_VER
            time_charmap = get_charmap (time_st->charmap_name());
        }

        std::cout << "\""
                  << create_str (_RWSTD_STATIC_CAST
                                 (const char*, time_st->era_d_t_fmt(0)),
                                 _RWSTD_STATIC_CAST
                                 (const wchar_t*, time_st->era_d_t_fmt(1)),
                                 time_charmap)
                  << "\"\n";
    }
}

void print_d_fmt (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_TIME\n";
    std::cout << create_keyword ("d_fmt");
    if (0 == time_st)
        std::cout << "\"" << create_str ("%m/%d/%y", 0, 0) << "\"\n";
    else {
        // if the time_charmap has not been initialized then initialize it
        if (0 == time_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[2]))
                set_locale (time_st->charmap_name());
#endif  // _MSC_VER
            time_charmap = get_charmap (time_st->charmap_name());
        }

        std::cout << "\""
                  << create_str (_RWSTD_STATIC_CAST
                                 (const char*, time_st->d_fmt(0)),
                                 _RWSTD_STATIC_CAST
                                 (const wchar_t*, time_st->d_fmt(1)),
                                 time_charmap)
                  << "\"\n";
    }
}

void print_era_d_fmt (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_TIME\n";
    std::cout << create_keyword ("era_d_fmt");
    if (0 == time_st)
        std::cout << "\"" << create_str ("", 0, 0) << "\"\n";
    else {
        // if the time_charmap has not been initialized then initialize it
        if (0 == time_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[2]))
                set_locale (time_st->charmap_name());
#endif  // _MSC_VER
            time_charmap = get_charmap (time_st->charmap_name());
        }

        std::cout << "\""
                  << create_str (_RWSTD_STATIC_CAST
                                 (const char*, time_st->era_d_fmt(0)),
                                 _RWSTD_STATIC_CAST
                                 (const wchar_t*, time_st->era_d_fmt(1)),
                                 time_charmap)
                  << "\"\n";
    }
}



void print_t_fmt (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_TIME\n";
    std::cout << create_keyword ("t_fmt");
    if (0 == time_st)
        std::cout << "\"" << create_str ("%H:%M:%S", 0, 0) << "\"\n";
    else {
        // if the time_charmap has not been initialized then initialize it
        if (0 == time_charmap && decode){
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[2]))
                set_locale (time_st->charmap_name());
#endif  // _MSC_VER
            time_charmap = get_charmap (time_st->charmap_name());
        }

        std::cout << "\""
                  << create_str (_RWSTD_STATIC_CAST
                                 (const char*, time_st->t_fmt(0)),
                                 _RWSTD_STATIC_CAST
                                 (const wchar_t*, time_st->t_fmt(1)),
                                 time_charmap)
                  << "\"\n";
    }
}

void print_era_t_fmt (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_TIME\n";
    std::cout << create_keyword ("era_t_fmt");
    if (0 == time_st)
        std::cout << "\"" << create_str ("", 0, 0) << "\"\n";
    else {
        // if the time_charmap has not been initialized then initialize it
        if (0 == time_charmap && decode){
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[2]))
                set_locale (time_st->charmap_name());
#endif  // _MSC_VER
            time_charmap = get_charmap (time_st->charmap_name());
        }

        std::cout << "\""
                  << create_str (_RWSTD_STATIC_CAST
                                 (const char*, time_st->era_t_fmt(0)),
                                 _RWSTD_STATIC_CAST
                                 (const wchar_t*, time_st->era_t_fmt(1)),
                                 time_charmap)
                  << "\"\n";
    }
}

void print_t_fmt_ampm (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_TIME\n";
    std::cout << create_keyword ("t_fmt_ampm");
    if (0 == time_st)
        std::cout << "\"" << create_str ("%I:%M:%S %p", 0, 0) << "\"\n";
    else {
        // if the time_charmap has not been initialized then initialize it
        if (0 == time_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[2]))
                set_locale (time_st->charmap_name());
#endif  // _MSC_VER
            time_charmap = get_charmap (time_st->charmap_name());
        }

        std::cout << "\""
                  << create_str (_RWSTD_STATIC_CAST
                                 (const char*, time_st->t_fmt_ampm(0)),
                                 _RWSTD_STATIC_CAST
                                 (const wchar_t*, time_st->t_fmt_ampm(1)),
                                 time_charmap)
                  << "\"\n";
    }

}

void print_alt_digits (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_TIME\n";
    std::cout << create_keyword ("alt_digits");
    if (0 == time_st)
        std::cout << "\"" << create_str ("", 0, 0) << "\"\n";
    else {
        // if the time_charmap has not been initialized then initialize it
        if (0 == time_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[2]))
                set_locale (time_st->charmap_name());
#endif  // _MSC_VER
            time_charmap = get_charmap (time_st->charmap_name());
        }

        std::cout << '"';

        unsigned digits_count = time_st->alt_digits_count ();

        for (unsigned i = 0; i != digits_count; i++) {

            const char* const str =
                _RWSTD_STATIC_CAST (const char*, time_st->alt_digits (i, 0));

            const wchar_t* const wstr =
                _RWSTD_STATIC_CAST (const wchar_t*, time_st->alt_digits (i, 1));

            std:: cout << create_str (str, wstr, time_charmap);

            if (i != digits_count - 1)
                std::cout << "\";\"";
        }
        std::cout << "\"\n";
    }
}


void print_noexpr (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MESSAGES\n";
    std::cout << create_keyword ("noexpr");
    if (0 == messages_st)
        std::cout << "\"" << create_str ("^[nN]", 0, 0) << "\"\n";
    else {
        // if the messages_charmap has not been initialized then initialize it
        if (0 == messages_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[5]))
                set_locale (messages_st->charmap_name());
#endif  // _MSC_VER
            messages_charmap = get_charmap (messages_st->charmap_name());

        }
        std:: cout << "\""
                   << create_str (_RWSTD_STATIC_CAST
                                  (const char*, messages_st->noexpr(0)),
                                  _RWSTD_STATIC_CAST
                                  (const wchar_t*, messages_st->noexpr(1)),
                                  messages_charmap) << "\"\n";
    }
}

void print_yesexpr (bool print_cat)
{
    if (print_cat)
        std::cout << "LC_MESSAGES\n";
    std::cout << create_keyword ("yesexpr");
    if (0 == messages_st)
        std::cout << "\"" << create_str ("^[yY]", 0, 0) << "\"\n";
    else {
        // if the messages_charmap has not been initialized then initialize it
        if (0 == messages_charmap && decode) {
            // try to set the locale to the locale we are dumping
            // if we can't then try to set the locale to any locale
            // that uses the same encoding
#ifndef _MSC_VER
            if (0 == std::setlocale (LC_CTYPE, current_locales[5]))
                set_locale (messages_st->charmap_name());
#endif  // _MSC_VER
            messages_charmap = get_charmap (messages_st->charmap_name());

        }
        std::cout << "\""
                  << create_str (_RWSTD_STATIC_CAST
                                 (const char*, messages_st->yesexpr(0)),
                                 _RWSTD_STATIC_CAST
                                 (const wchar_t*, messages_st->yesexpr(1)),
                                 messages_charmap) << "\"\n";
    }
}

void print_lc_messages ()
{
    if (print_cat_names)
        std::cout << "LC_MESSAGES\n";
    print_yesexpr (false);
    print_noexpr (false);

    if (print_cat_names)
        std::cout << "END LC_MESSAGES\n";
}

void print_lc_time ()
{
    if (print_cat_names)
        std::cout << "LC_TIME\n";

    print_abday (false);
    print_day (false);
    print_abmon (false);
    print_mon (false);
    print_am_pm (false);
    print_d_t_fmt (false);
    print_d_fmt (false);
    print_t_fmt (false);
    print_t_fmt_ampm (false);
    print_era (false);
    print_era_d_t_fmt (false);
    print_era_d_fmt (false);
    print_era_t_fmt (false);
    print_alt_digits (false);

    if (print_cat_names)
        std::cout << "END LC_TIME\n";
}

void print_charmap_name ()
{
    if (0 != ctype_st) {
        if (print_cat_names)
            std::cout << "CHARMAP\n";
        std::cout << create_keyword ("charmap") << "\""
                  << ctype_st->charmap_name() << "\"\n";
    }
}

// print the available locales
void print_locale_names()
{
    std::cout << "C\n";
    const char* locale_root = std::getenv ("RWSTD_LOCALE_ROOT");
    if (0 != locale_root) {
        std::string cmd;
        cmd = cmd + "ls -1 " + locale_root;
        std::system (cmd.c_str());
    }
}

// print the available charmaps
void print_charmap_names ()
{
    const char* locale_root = std::getenv ("RWSTD_SRC_ROOT");
    if (0 != locale_root) {
        std::string cmd;
        cmd = cmd + "ls -1 " + locale_root + "/charmaps";
        std::system (cmd.c_str());
    }
}


static void print_help_msg ()
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

void initialize_env_vars ()
{
    current_locales[6] = std::getenv ("LANG");
    if (0 == current_locales[6])
        current_locales[6] = "POSIX";

    current_locales[7] = std::getenv("LC_ALL");
    if (0 != current_locales[7] && '\0' != *current_locales[7]) {
        current_locales[0] = current_locales[7];
        current_locales[4] = current_locales[7];
        current_locales[2] = current_locales[7];
        current_locales[1] = current_locales[7];
        current_locales[3] = current_locales[7];
        current_locales[5] = current_locales[7];
    }
    else {
        current_locales[7] = "";

        current_locales[0] = std::getenv ("LC_CTYPE");
        if (0 == current_locales[0])
            current_locales[0] = current_locales[6];
        else
            is_env_set[0] = true;

        current_locales[4] = std::getenv ("LC_NUMERIC");
        if (0 == current_locales[4])
            current_locales[4] = current_locales[6];
        else
            is_env_set[4] = true;

        current_locales[2] = std::getenv ("LC_TIME");
        if (0 == current_locales[2])
            current_locales[2] = current_locales[6];
        else
            is_env_set[2] = true;

        current_locales[1] = std::getenv ("LC_COLLATE");
        if (0 == current_locales[1])
            current_locales[1] = current_locales[6];
        else
            is_env_set[1] = true;

        current_locales[3] = std::getenv ("LC_MONETARY");
        if (0 == current_locales[3])
            current_locales[3] = current_locales[6];
        else
            is_env_set[3] = true;

        current_locales[5] = std::getenv ("LC_MESSAGES");
        if (0 == current_locales[5])
            current_locales[5] = current_locales[6];
        else
            is_env_set[5] = true;

    }

}

void print_locale_name (const char* cat_name, const char* loc_str,
                        bool env_set) {
    std::cout << cat_name;
    if (!env_set)
        std::cout << "\"";
    std::cout << loc_str;
    if (!env_set)
        std::cout << "\"";
    std::cout << std::endl;
}


// print out the character map in the POSIX format
static void
print_charmap (const __rw::__rw_codecvt_t *cvt,
               char *mbchar,  const unsigned *tab,
               unsigned *ntables, unsigned *nchars, unsigned *nunused)
{
    const unsigned* const table = cvt->n_to_w_tab ();

    if (!tab)
        tab = table;

    unsigned i;

    if (ntables)
        ++*ntables;

    for (i = 0; i != UCHAR_MAX + 1U; ++i) {
        
        if (tab [i] & 0x80000000)
            continue;

        // look up the UCS-4 and the wchar_t value of the MB character
        const unsigned ucs4  = cvt->get_ucs4_at_offset (tab [i]);
        const unsigned wchar = cvt->get_internal_at_offset (tab [i]);

        const int width = ucs4 < 0x10000 ? 4 : 8;
        const int pad   = 10 - width;

        // print the UCS code, followed by the MB character,
        // followed by the libc wchar_t value of the character
        std::printf ("<U%0*X>%*s%s\\x%02x   # L'\\x%02x'\n",
                     width, ucs4, pad, " ", mbchar, i, wchar);

        // increment the total number of characters
        if (nchars)
            ++*nchars;
    }

    for (i = 0; i != UCHAR_MAX + 1U; ++i) {

        if (UINT_MAX == tab [i]) {

            if (nunused)
                ++*nunused;

            continue;
        }

        if (!(tab [i] & 0x80000000))
            continue;

        const std::size_t len = std::strlen (mbchar);
        std::sprintf (mbchar + len, "\\x%02x", i);

        print_charmap (cvt, mbchar, table + 256 * (tab [i] & 0x7fffffff),
                       ntables, nchars, nunused);

        mbchar [len] = '\0';
    }
}


// print out the contents of the __rw_codecvt_t structure
static void
print_charmap (const __rw::__rw_codecvt_t *cvt)
{
    if (!cvt)
        cvt = codecvt_st;

    if (!cvt)
        return;

    std::cout <<   "<escape_char>   \\"
              << "\n<comment_char>  #"
              << "\n<code_set_name> " << cvt->codeset_name ()
              << "\n<mb_cur_max>    " << int (cvt->mb_cur_max)
              << "\n\n# charmap data:"
              << "\n#   charmap name        = " << cvt->charmap_name ()
              << "\n#   n_to_w_tab_off      = " << cvt->n_to_w_tab_off
              << "\n#   w_to_n_tab_off      = " << cvt->w_to_n_tab_off
              << "\n#   utf8_to_ext_tab_off = " << cvt->utf8_to_ext_tab_off
              << "\n#   xliteration_off     = " << cvt->xliteration_off
              << "\n#   wchar_off           = " << cvt->wchar_off
              << "\n#   codeset_off         = " << cvt->codeset_off
              << "\n#   charmap_off         = " << cvt->charmap_off
              << "\n#   codecvt_ext_off     = " << cvt->codecvt_ext_off
              << "\n\nCHARMAP\n";

    char buf [1024];
    *buf = '\0';

    unsigned ntables = 0;
    unsigned nchars  = 0;
    unsigned nunused = 0;

    print_charmap (cvt, buf, 0, &ntables, &nchars, &nunused);

    std::cout << "END CHARMAP\n"
              << "\n# charmap stats:"
              << "\n#   number of tables       = " << ntables
              << "\n#   number of characters   = " << nchars
              << "\n#   number of unused slots = " << nunused
              << '\n';
}


int main (int argc, char *argv[])
{
    try {
        const char* const program_name = argv [0];

        initialize_env_vars ();

        if (1 == argc) {
            std::cout << "LANG=" << current_locales[6] << '\n';
            print_locale_name ("LC_CTYPE=", current_locales[0], is_env_set[0]);
            print_locale_name ("LC_COLLATE=", current_locales[1],
                               is_env_set[1]);
            print_locale_name ("LC_TIME=", current_locales[2], is_env_set[2]);
            print_locale_name ("LC_MONETARY=", current_locales[3],
                               is_env_set[3]);
            print_locale_name ("LC_NUMERIC=", current_locales[4],
                               is_env_set[4]);
            print_locale_name ("LC_MESSAGES=", current_locales[5],
                               is_env_set[5]);
            print_locale_name ("LC_ALL=", current_locales[7], true);
        }
        else {
            initialize_structs ();

            --argc;

            while (0 != *++argv && 0 < argc-- && '-' == **argv) {

                switch (*++*argv) {

                case 'a':
                    print_locale_names();
                    return EXIT_SUCCESS;

                case 'k':
                    print_keywords = true;
                    break;

                case 'c':
                    print_cat_names = true;
                    if (*(*argv + 1) == 'k')
                        print_keywords = true;
                    break;

                case 'm':
                    print_charmap_names();
                    return EXIT_SUCCESS;

                case 'h':
                    decode = true;
                    break;

                case 'l':
                    POSIX_output = false;
                    break;

                case 'w':
                    if (std::strlen (*argv) == 1)
                        issue_diag (W_DISABLE, 0, 0, 0);
                    else
                        issue_diag (std::atoi (++*argv), 0, 0, 0);
                    break;

                case '?':
                    print_help_msg();
                    return EXIT_SUCCESS;

                case '-':
                    if (0 == std::strcmp (*argv, "-help")) {
                        print_help_msg ();
                        return EXIT_SUCCESS;
                    }

                    if (0 == std::strcmp (*argv, "-charmap")) {
                        print_charmap (0);
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

            while (0 != *argv) {
                if (0 == std::strcmp (*argv, "LC_TIME"))
                    print_lc_time ();
                else if (0 == std::strcmp (*argv, "LC_MONETARY"))
                    print_lc_monetary();
                else if (0 == std::strcmp (*argv, "LC_NUMERIC"))
                    print_lc_numeric ();
                else if (0 == std::strcmp (*argv, "LC_CTYPE"))
                    print_lc_ctype ();
                else if (0 == std::strcmp (*argv, "LC_COLLATE"))
                    print_lc_collate ();
                else if (0 == std::strcmp (*argv, "LC_MESSAGES"))
                    print_lc_messages ();
                else if (0 == std::strcmp (*argv, "LC_ALL")) {
                    print_lc_time ();
                    std::cout << '\n';
                    print_lc_monetary ();
                    std::cout << '\n';
                    print_lc_numeric ();
                    std::cout << '\n';
                    print_lc_ctype ();
                    std::cout << '\n';
                    print_lc_collate ();
                    std::cout << '\n';
                    print_lc_messages ();
                }

                else if (0 ==std::strcmp (*argv, "abday"))
                    print_abday (print_cat_names);
                else if (0 == std::strcmp(*argv, "day"))
                    print_day(print_cat_names);
                else if (0 == std::strcmp (*argv, "abmon"))
                    print_abmon (print_cat_names);
                else if (0 == std::strcmp (*argv, "mon"))
                    print_mon (print_cat_names);
                else if (0 == std::strcmp (*argv, "am_pm"))
                    print_am_pm (print_cat_names);
                else if (0 == std::strcmp (*argv, "d_t_fmt"))
                    print_d_t_fmt (print_cat_names);
                else if (0 == std::strcmp (*argv, "t_fmt"))
                    print_t_fmt (print_cat_names);
                else if (0 == std::strcmp (*argv, "d_fmt"))
                    print_d_fmt (print_cat_names);
                else if (0 == std::strcmp (*argv, "t_fmt_ampm"))
                    print_t_fmt_ampm (print_cat_names);
                else if (0 == std::strcmp (*argv, "era_d_t_fmt"))
                    print_era_d_t_fmt (print_cat_names);
                else if (0 == std::strcmp (*argv, "era_t_fmt"))
                    print_era_t_fmt (print_cat_names);
                else if (0 == std::strcmp (*argv, "era_d_fmt"))
                    print_era_d_fmt (print_cat_names);
                else if (0 == std::strcmp (*argv, "era"))
                    print_era (print_cat_names);
                else if (0 == std::strcmp (*argv, "alt_digits"))
                    print_alt_digits (print_cat_names);

                else if (0 == std::strcmp (*argv, "yesexpr"))
                    print_yesexpr (print_cat_names);
                else if (0 == std::strcmp (*argv, "noexpr"))
                    print_noexpr (print_cat_names);

                else if (0 == std::strcmp (*argv, "int_curr_symbol"))
                    print_int_curr_symbol (print_cat_names);
                else if (0 == std::strcmp (*argv, "currency_symbol"))
                    print_currency_symbol (print_cat_names);
                else if (0 == std::strcmp (*argv, "mon_decimal_point"))
                    print_mon_decimal_point (print_cat_names);
                else if (0 == std::strcmp (*argv, "mon_thousands_sep"))
                    print_mon_thousands_sep (print_cat_names);
                else if (0 == std::strcmp (*argv, "mon_grouping"))
                    print_mon_grouping (print_cat_names);
                else if (0 == std::strcmp (*argv, "positive_sign"))
                    print_positive_sign (print_cat_names);
                else if (0 == std::strcmp (*argv, "negative_sign"))
                    print_negative_sign (print_cat_names);

                else if (0 == std::strcmp (*argv, "int_frac_digits"))
                    print_int_frac_digits (print_cat_names);
                else if (0 == std::strcmp (*argv, "frac_digits"))
                    print_frac_digits (print_cat_names);
                else if (0 == std::strcmp (*argv, "p_cs_precedes"))
                    print_p_cs_precedes (print_cat_names);
                else if (0 == std::strcmp (*argv, "p_sep_by_space"))
                    print_p_sep_by_space (print_cat_names);
                else if (0 == std::strcmp (*argv, "n_cs_precedes"))
                    print_n_cs_precedes (print_cat_names);
                else if (0 == std::strcmp (*argv, "n_sep_by_space"))
                    print_n_sep_by_space (print_cat_names);
                else if (0 == std::strcmp (*argv, "p_sign_posn"))
                    print_p_sign_posn (print_cat_names);
                else if (0 == std::strcmp (*argv, "n_sign_posn"))
                    print_n_sign_posn (print_cat_names);
                else if (0 == std::strcmp (*argv, "int_p_cs_precedes"))
                    print_int_p_cs_precedes (print_cat_names);
                else if (0 == std::strcmp (*argv, "int_p_sep_by_space"))
                    print_int_p_sep_by_space (print_cat_names);
                else if (0 == std::strcmp (*argv, "int_n_cs_precedes"))
                    print_int_n_cs_precedes (print_cat_names);
                else if (0 == std::strcmp (*argv, "int_n_sep_by_space"))
                    print_int_n_sep_by_space (print_cat_names);
                else if (0 == std::strcmp (*argv, "int_p_sign_posn"))
                    print_int_p_sign_posn (print_cat_names);
                else if (0 == std::strcmp (*argv, "int_n_sign_posn"))
                    print_int_n_sign_posn (print_cat_names);


                else if (0 == std::strcmp (*argv, "decimal_point"))
                    print_decimal_point (print_cat_names);
                else if (0 == std::strcmp (*argv, "thousands_sep"))
                    print_thousands_sep (print_cat_names);
                else if (0 == std::strcmp (*argv, "grouping"))
                    print_grouping (print_cat_names);

                else if (0 == std::strcmp (*argv, "upper"))
                    print_mask (std::ctype_base::upper, print_cat_names);

                else if (0 == std::strcmp (*argv, "lower"))
                    print_mask (std::ctype_base::lower, print_cat_names);
                else if (0 == std::strcmp (*argv, "space"))
                    print_mask (std::ctype_base::space, print_cat_names);
                else if (0 == std::strcmp (*argv, "print"))
                    print_mask (std::ctype_base::print, print_cat_names);
                else if (0 == std::strcmp (*argv, "cntrl"))
                    print_mask (std::ctype_base::cntrl, print_cat_names);
                else if (0 == std::strcmp (*argv, "alpha"))
                    print_mask (std::ctype_base::alpha, print_cat_names);
                else if (0 == std::strcmp (*argv, "digit"))
                    print_mask (std::ctype_base::digit, print_cat_names);
                else if (0 == std::strcmp (*argv, "punct"))
                    print_mask (std::ctype_base::punct, print_cat_names);
                else if (0 == std::strcmp (*argv, "graph"))
                    print_mask (std::ctype_base::graph, print_cat_names);
                else if (0 == std::strcmp (*argv, "xdigit"))
                    print_mask (std::ctype_base::xdigit, print_cat_names);
                else if (0 == std::strcmp (*argv, "tolower"))
                    print_tolower (print_cat_names);
                else if (0 == std::strcmp (*argv, "toupper"))
                    print_toupper (print_cat_names);

                else if (0 == std::strcmp (*argv, "charmap"))
                    print_charmap_name ();

                argv++;
            }
        }
    }
    catch (loc_exception&) {
        return 1;
    }
    catch (const std::exception& error) {
        std::cerr <<"ERROR: " << error.what() << '\n';
        return 1;
    }
    catch (...) {
        std::cerr << "unknown exception\n";
        return 1;
    }

#ifdef _RWSTD_NO_ISO_10646_WCHAR_T
        //      std::setlocale (LC_CTYPE, saved_loc_name);
#endif   // _RWSTD_NO_ISO_10646_WCHAR_T

    return 0;
}
