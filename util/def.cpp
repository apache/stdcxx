/***************************************************************************
 *
 * def.cpp
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
 * Copyright 2001-2006 Rogue Wave Software.
 * 
 **************************************************************************/

// #ifndef _RWSTD_NO_PURE_C_HEADERS
// #  define _RWSTD_NO_PURE_C_HEADERS
// #endif   // _RWSTD_NO_PURE_C_HEADERS

// #ifndef _RWSTD_NO_DEPRECATED_C_HEADERS
// #  define _RWSTD_NO_DEPRECATED_C_HEADERS
// #endif   // _RWSTD_NO_DEPRECATED_C_HEADERS

#ifdef __DECCXX
#  undef __PURE_CNAME
#endif   // __DECCXX

#include <algorithm>
#include <fstream>
#include <iostream>
#include <locale>
#include <map>
#include <string>
#include <vector>

#include <cassert>
#include <cctype>
#include <cerrno>
#include <climits>
#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <cstring>   // for memset()

#include "aliases.h"
#include "def.h"
#include "diagnostic.h"
#include "loc_exception.h"
#include "path.h"


#define UTF8_MAX_SIZE 6


// convert_to_ext converts a wchar_t value with some encoding into 
// a narrow character string in the current locale's encoding
std::string Def::convert_to_ext (wchar_t val)
{
    rmb_cmap_iter it;
    if ((it = charmap_.get_rmb_cmap().find(val)) 
        != charmap_.get_rmb_cmap().end()){
      return it->second;
        
    }

    issue_diag (E_CVT2EXT, true, 0, 
                "unable to convert character %d to external "
                "representation\n", val);

    return std::string("");
}


// convert the wchar_t value into a utf8 string 
std::string Def::utf8_encode (wchar_t wc)
{
    unsigned int wc_int = _RWSTD_STATIC_CAST (unsigned int, wc);

    std::string ret;
    std::size_t size = 0;
    char buf[UTF8_MAX_SIZE + 1];
    char* bufp = buf;

    if (wc_int < 0x80)
    {
        size = 1;
        *bufp++ = wc_int;
    }
    else
    {
        int b;

        for (b = 2; b < UTF8_MAX_SIZE; b++)
            if ((wc_int & (~(wchar_t)0 << (5 * b + 1))) == 0)
                break;
        size = b;

        *bufp = (unsigned char) (~0xff >> b);
        --b;
        do
        {
            bufp[b] = 0x80 | (wc_int & 0x3f);
            wc_int >>= 6;
        }
        while (--b > 0);
        *bufp |= wc_int;
    }
    buf[size] = (char)0;
    for (unsigned int i = 0; i < size; i++)
        ret += buf[i];
    return ret;
    
}

void Def::copy_file (const std::string& name, const std::string& outname)
{
    assert (name.size() > 0);
    assert (outname.size() > 0);

    std::ifstream from (name.c_str(), std::ios::binary);
    if (!from) {
        issue_diag (E_OPENRD, true, 
                    &next, "unable to open locale database %s\n",
                    name.c_str());
    }
    from.exceptions (std::ios::badbit);

    std::ofstream to (outname.c_str(), std::ios::binary);
    if (!to) {
        issue_diag (E_OPENWR, true, 
                    &next, "unable to create locale database %s\n",
                    outname.c_str());
    }
    to.exceptions (std::ios::failbit | std::ios::badbit);

    // copy the file
    to << from.rdbuf ();
}


void Def::copy_category(int category, std::string name)
{
    assert (name.size() > 0);

    // create the name of the file to copy to and call copy_file
    std::string outname (output_name_);

    makedir (outname.c_str ());

    switch (category) {
        // append the category name to both 'name' and 'outname'
        // and call the copy_file routine
        // the xxx_written variable is set to true so that write_xxx
        // does not overwrite the file that is written here
    case LC_CTYPE:
        (name += _RWSTD_PATH_SEP) += "LC_CTYPE";
        (outname += _RWSTD_PATH_SEP) += "LC_CTYPE";
        copy_file (name, outname);
        ctype_written_ = true;

        break;
    case LC_COLLATE:
        (name += _RWSTD_PATH_SEP) += "LC_COLLATE";
        (outname += _RWSTD_PATH_SEP) += "LC_COLLATE";
        copy_file(name, outname);
        collate_written_ = true;

        break;
    case LC_MONETARY:
        (name += _RWSTD_PATH_SEP) += "LC_MONETARY";
        (outname += _RWSTD_PATH_SEP) += "LC_MONETARY";
        copy_file(name, outname);
        mon_written_ = true;

        break;

    case LC_NUMERIC:
        (name += _RWSTD_PATH_SEP) += "LC_NUMERIC";
        (outname += _RWSTD_PATH_SEP) += "LC_NUMERIC";
        copy_file(name, outname);
        num_written_ = true;

        break;
    case LC_TIME:
        (name += _RWSTD_PATH_SEP) += "LC_TIME";
        (outname += _RWSTD_PATH_SEP) += "LC_TIME";
        copy_file(name, outname);
        time_written_ = true;

        break;

#ifdef LC_MESSAGES
    case LC_MESSAGES:
        (name += _RWSTD_PATH_SEP) += "LC_MESSAGES";
        (outname += _RWSTD_PATH_SEP) += "LC_MESSAGES";
        copy_file(name, outname);
        messages_written_ = true;
        break;
#endif   // LC_MESSAGES

    default:
        break;
    }
}


// strip a pair, which should be in the form '(<sym>,<sym2>)'
void Def::strip_pair (const std::string &tok, std::string &sym,
                      std::string &sym2)
{
    std::size_t i = 0;

    if(tok[i] == '(') {
        if(tok[++i] == '<')
            while (tok[i] != '>'){
                if (tok[i] == scanner_.escape_char ())
                    i++;
                sym.push_back(tok[i++]);
            }
        // this push_back is safe because the while loop above ends when 
        // tok[i] == '>'
        sym.push_back(tok[i++]);
        if (tok[i++] != ',')
            issue_diag (E_PAIR, true, &next, 
                        "invalid pair %s\n", tok.c_str());
        if (tok[i] == '<')
            while (tok[i] != '>'){
                if (tok[i] == scanner_.escape_char ())
                    sym2.push_back(tok[i++]);
                if ('\0' != tok[i])
                    sym2.push_back(tok[i++]);
                else 
                    issue_diag (E_PAIR, true, &next, 
                                "invalid pair %s\n",  tok.c_str());
            }
        
        // this push_back is safe because the while loop above ends when 
        // tok[i] == '>'
        sym2.push_back(tok[i++]);

    }
}

// converts str, which is a string in the following format
// "[<sym_name>][char]" including the quotes to a string of characters
// str is not a const reference because if the string spans multiple lines
// str is modified
std::string Def::convert_string (const std::string &str1)
{
    assert (str1[0] == '\"');

    std::string ret;
    
    std::string sym;
    // the index starts at 1 so that we ignore the initial '"'
    int idx = 1;

    const char* str = str1.c_str();
    while (str[idx] != '\"') {
        sym.clear();
        // if we reach the null-terminator before we see an end-quote
        // then we must have a multi-line string, so get the next token
        if (str[idx] == '\0') {
            if((next = scanner_.next_token()).token == Scanner::tok_string)
                break;
            str = next.name.c_str();
            idx = 0;
        }

        // '<' marks the beginning of a symbolic name
        // construct the name and look up its value in the cmap
        if (str[idx] == '<') {
            while (str [idx] && str [idx] != '>') {
                if (str[idx] == scanner_.escape_char ())
                    idx++;
                sym += str[idx++];
            }

            // this is safe because the while loop ended with *str == '>'
            if (str [idx])
                sym += str [idx++];

            w_cmap_iter w_pos = charmap_.get_w_cmap().find (sym);
            if (w_pos != charmap_.get_w_cmap().end()) {
                ret += convert_to_ext(w_pos->second);
            }
            else {
                return std::string();
            }
        }

        // the definition file contains a sting with non-symbol names.
        // process each character as it's actual character value.
        // Locale definitions that use this may not be portable.
        else {
            ret += (char)str[idx++];
            
        }
    }

    return ret;
   

}

#ifndef _RWSTD_NO_WCHAR_T
// converts a collating element definition to an array of wide characters
// (the wide characters the collating element is composed of).

// this overload deals with collating elements defined through
// a sequence of symbolic names, NOT enclosed within quotes.
std::wstring 
Def::convert_wstring (const StringVector& sym_array)
{
    std::wstring ret;
    StringVector::const_iterator it = sym_array.begin ();
    while (it != sym_array.end ()) {
        // lookup the symbol we just constructed
        w_cmap_iter w_pos = charmap_.get_w_cmap().find (*it);
        if (w_pos != charmap_.get_w_cmap().end()) {
            ret += w_pos->second;
            it++;
        }
        else {
            // we return an empty string if we couldn't find any character 
            // in the character map
            ret.clear();
            return ret;
        }
    }

    return ret;
}

// this overload deals with collating elements defined through
// a sequence of characters or symbolic names, enclosed within quotes.
std::wstring 
Def::convert_wstring (const token_t& t)
{
    std::wstring ret;
    std::string  sym;

    std::string str1 (t.name);

    int         idx = 0;
    char        term = 0;
    const char* str = str1.c_str();

    // skip first character if quote
    if (str[idx] == '\"') {
        term = '\"', idx++;
    }

    while (str[idx] != term) {
        sym.clear();

        // '<' marks the beginning of a symbolic name
        // construct the name and look up its value in the cmap
        if (str[idx] == '<') {
            while (str[idx] != '>') {
                if (str[idx] == scanner_.escape_char ()) {
                    // sym += str[idx++];
                    idx++;
                }

                if ('\0' != str[idx])
                    sym += str[idx++];
                else 
                    issue_diag (E_SYMEND, true, &t, 
                                "end of symbolic name not found\n");
            }

            // this is safe because the while loop ended with *str == '>'
            sym += str[idx++];

            // lookup the symbol we just constructed
            w_cmap_iter w_pos = charmap_.get_w_cmap().find (sym);
            if (w_pos != charmap_.get_w_cmap().end()) {
                ret += w_pos->second;
            }
            else {
                // if we can't find a symbol then return an empty string,
                // most likely this will happen if inside a collating-element
                // the user uses a character that is not in the current
                // codeset, in this case the collating element will be ignored
                ret.clear();
                return ret;
            }
        }
        // the definition file contains a string with non-symbol names.
        // process each character as it's actual character value.
        // Locale definitions that use this may not be portable.
        else 
            ret += (wchar_t)str[idx++];
    }

    return ret;
   
}

#endif  // _RWSTD_NO_WCHAR_T


// automatically fill any categories that depend on other categories
void Def::auto_fill ()
{

    mask_iter mask_pos;

    for (std::size_t i = 0; i <= UCHAR_MAX; i++) {
        if (   ctype_out_.mask_tab[i] & std::ctype_base::upper 
            || ctype_out_.mask_tab[i] & std::ctype_base::lower
            || ctype_out_.mask_tab[i] & std::ctype_base::alpha
            || ctype_out_.mask_tab[i] & std::ctype_base::digit 
            || ctype_out_.mask_tab[i] & std::ctype_base::xdigit 
            || ctype_out_.mask_tab[i] & std::ctype_base::punct)
            
            ctype_out_.mask_tab[i] |= std::ctype_base::print;

        if (   ctype_out_.mask_tab[i] & std::ctype_base::upper 
            || ctype_out_.mask_tab[i] & std::ctype_base::lower)

            ctype_out_.mask_tab[i] |= std::ctype_base::alpha;

        if (   ctype_out_.mask_tab[i] & std::ctype_base::upper 
            || ctype_out_.mask_tab[i] & std::ctype_base::lower
            || ctype_out_.mask_tab[i] & std::ctype_base::alpha
            || ctype_out_.mask_tab[i] & std::ctype_base::digit 
            || ctype_out_.mask_tab[i] & std::ctype_base::xdigit 
            || ctype_out_.mask_tab[i] & std::ctype_base::punct)
            
            ctype_out_.mask_tab[i] |= std::ctype_base::graph;
    }

    for (mask_pos = mask_.begin(); mask_pos != mask_.end(); mask_pos++) {
        // all lower, alpha, digit, xdigit, and punct, and space 
        // characters are automatically print
        
        if (   mask_pos->second & std::ctype_base::upper 
            || mask_pos->second & std::ctype_base::lower
            || mask_pos->second & std::ctype_base::alpha
            || mask_pos->second & std::ctype_base::digit 
            || mask_pos->second & std::ctype_base::xdigit 
            || mask_pos->second & std::ctype_base::punct)
            //     || mask_pos->second & std::ctype_base::space)

            mask_pos->second |= std::ctype_base::print;

        // all upper and lower characters are alpha
        if (   mask_pos->second & std::ctype_base::upper 
            || mask_pos->second & std::ctype_base::lower)

            mask_pos->second |= std::ctype_base::alpha;

        // all upper, lower, alpha, digit, xdigit, and punct characters
        // are graph characters
        if (   mask_pos->second & std::ctype_base::upper 
            || mask_pos->second & std::ctype_base::lower
            || mask_pos->second & std::ctype_base::alpha
            || mask_pos->second & std::ctype_base::digit 
            || mask_pos->second & std::ctype_base::xdigit 
            || mask_pos->second & std::ctype_base::punct)
            
            mask_pos->second |= std::ctype_base::graph;
            

    }
}


void Def::process_input ()
{
    while ((next = scanner_.next_token ()).token != Scanner::tok_end_tokens) {

        switch (next.token) {

        case Scanner::tok_comment:
            scanner_.ignore_line ();
            break;

        case Scanner::tok_ctype:
            process_ctype ();
            break;

        case Scanner::tok_collate:
            process_collate ();
            break;

        case Scanner::tok_monetary:
            process_monetary ();
            break;

        case Scanner::tok_numeric:
            process_numeric ();
            break;

        case Scanner::tok_time:
            process_time ();
            break;

        case Scanner::tok_messages:
            process_messages ();
            break;

        case Scanner::tok_nl:
            break;

        default:
            scanner_.ignore_line ();
            break;
        }
    }

    auto_fill ();
}


Def::Def (const char* filename, const char* out_name, Charmap& char_map,
          bool no_position)
    : warnings_occurred_ (false),
      scan_ahead_ (false),
      next_offset_ (0),
      output_name_ (out_name),
      charmap_ (char_map), 
      ctype_written_ (false),
      codecvt_written_ (false),
      collate_written_ (false),
      time_written_ (false), 
      num_written_ (false),
      mon_written_ (false),
      messages_written_ (false),
      ctype_def_found_ (false),
      collate_def_found_ (false),
      time_def_found_ (false),
      num_def_found_ (false), 
      mon_def_found_ (false),
      messages_def_found_ (false), 
      undefined_keyword_found_ (false),
      no_position_ (no_position)

{
    // make sure ctype_out object is cleared
    std::memset (&ctype_out_, 0, sizeof (ctype_out_));
    std::memset (&time_out_, 0, sizeof (time_out_));

    // invalidate format characters by setting each to CHAR_MAX
    // as specified by the C function localeconv()
    mon_out_.frac_digits    [0] = CHAR_MAX;
    mon_out_.frac_digits    [1] = CHAR_MAX;
    mon_out_.p_cs_precedes  [0] = CHAR_MAX;
    mon_out_.p_sep_by_space [0] = CHAR_MAX;
    mon_out_.n_cs_precedes  [0] = CHAR_MAX;
    mon_out_.n_sep_by_space [0] = CHAR_MAX;
    mon_out_.p_sign_posn    [0] = CHAR_MAX;
    mon_out_.n_sign_posn    [0] = CHAR_MAX;

    mon_st_.mon_grouping += CHAR_MAX;

    // invalidate int'l formats
    mon_out_.p_cs_precedes  [1] = CHAR_MAX;
    mon_out_.p_sep_by_space [1] = CHAR_MAX;
    mon_out_.n_cs_precedes  [1] = CHAR_MAX;
    mon_out_.n_sep_by_space [1] = CHAR_MAX;
    mon_out_.p_sign_posn    [1] = CHAR_MAX;
    mon_out_.n_sign_posn    [1] = CHAR_MAX;
    
    num_st_.grouping += CHAR_MAX;

    collate_out_.largest_ce     = 1;
    collate_out_.longest_weight = 1;
    collate_out_.num_wchars     = 0;
    std::memset (collate_out_.weight_type, 0,
                 sizeof (collate_out_.weight_type));

    // initialize all extensions to 0
    ctype_out_.ctype_ext_off     = 0;
    num_out_.numeric_ext_off     = 0;
    collate_out_.collate_ext_off = 0;
    mon_out_.monetary_ext_off    = 0;
    time_out_.time_ext_off       = 0;

    // actual processing
    scanner_.open (filename);
}


Def::~Def ()
{
    // free up the memory that was allocated

    coll_map_iter coll_map_pos;
    for (coll_map_pos = coll_map_.begin();
         coll_map_pos != coll_map_.end(); coll_map_pos ++) {
        delete[] (coll_map_pos->second.weights);
    }

}
