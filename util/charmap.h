/***************************************************************************
 *
 * charmap.h
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
 * Copyright 2001-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/


#ifndef _RWSTD_CHARMAP_H_INCLUDED
#define _RWSTD_CHARMAP_H_INCLUDED

#include <string>
#include <list>
#include <map>
#include <set>

#ifndef _RWSTD_NO_ICONV
#  include <iconv.h>
#endif   // _RWSTD_NO_ICONV


#include "scanner.h"

class Charmap
{
public:
    static const char* const portable_charset [];

    Charmap(const char* /*corresponding C library locale*/,
            const char* /*filename*/, 
            bool /*is utf8 encoding?*/, 
            bool /*create_forward_charmaps*/,
            bool /*create_reverse_charmaps*/, 
            bool /*use UCS4 internally*/);    
  
    // returns the narrow character map which maps a symbolic character
    // name to its narrow character value
    const std::map<std::string, unsigned char>& get_n_cmap() const {
        return n_cmap_;
    }
    
    // returns the reverse narrow character map which maps a narrow
    // character value to its symbolic name
    const std::map<unsigned char, std::string>& get_rn_cmap() const {
        return rn_cmap_;
    }

    // returns the wide character map which maps a symbolic character
    // name to its wide character value
    const std::map<std::string, wchar_t>& get_w_cmap() const {
        return w_cmap_;
    }

    // returns the reverse wide character map which maps a wide
    // character value to its symbolic name
    const std::map<wchar_t, std::string>& get_rw_cmap() const {
        return rw_cmap_;
    }

    // returns the multibyte character map which maps a multibyte
    // character to its corresponding wide character value
    const std::map<std::string, wchar_t>& get_mb_cmap() const {
        return mb_cmap_;
    }

    // returns the reverse multibyte character map which maps a wide
    // character value to its corresponding multibyte character
    const std::map<wchar_t, std::string>& get_rmb_cmap() const {
        return rmb_cmap_;
    }

    // get the string value map
    const std::list<std::string>& get_symnames_list() const {
        return symnames_list_;
    }

    const std::map <std::string, wchar_t>& get_ucs4_cmap () const {
        return ucs4_cmap_;
    }

    const std::map <wchar_t, std::string>& get_rucs4_cmap () const {
        return rucs4_cmap_;
    }

    // return the value of mb_cur_max
    int get_mb_cur_max() const {
        return mb_cur_max_;
    }

    // return the name of the codeset
    const std::string& get_code_set_name () const {
        return code_set_name_;
    }

    // return the name of the character map
    std::string get_charmap_name () const;

    // return the full path to the charmap
    std::string get_full_charmap_name () const {
        return charmap_name_;
    }

    // convert the externally encoded string to the internal encoding
    bool convert_to_wc (const std::string&, const std::string&, wchar_t&);

    // convert the externally encoded string to UCS
    bool convert_to_ucs (const std::string&, const std::string&, wchar_t&);

    // convert the externally encoded string to UCS
    wchar_t convert_sym_to_ucs (const std::string&) const;


    // get the number of bytes in a single multi-byte character
    std::size_t mbcharlen (const std::string&) const;

    // convert the first byte in the multi-byte character to an unsigned char
    unsigned char convert_char (const char*, const char** = 0) const;

    unsigned char get_largest_nchar () const;
    
    // increments the wide character value to the next encoded character
    // in the current codeset; returns the incremented value or -1 on
    // error
    wchar_t increment_wchar (wchar_t) const;

private:
    
    // processes characters implicitly defined by an ellipsis denoted
    // by two explicitly defined characters; returns the number of
    // characters in the range, -1 on error
    std::size_t process_ellipsis (const Scanner::token_t&, int);

    // process the charmap file making the necessary mappings in the cmaps
    void process_chars();

    // increment the encoded multi byte character argument
    bool increment_encoding (std::string&);

    // verify that all the characters in the portable character set
    // are defined in the character map
    void verify_portable_charset () const;

#ifndef _RWSTD_NO_ICONV
    // open the iconv descriptor to convert to utf8
    iconv_t open_iconv_to_utf8 () const;
#endif   // _RWSTD_NO_ICONV

    // convert a human-readable encoding of a character
    // to its raw multibyte character representation
    std::string encoding_to_mbchar (const std::string&) const;

    // convert a multi-byte string to a utf8 multi-byte string
    char* convert_to_utf8 (const char *inbuf, std::size_t inbuf_s, 
                           char *outbuf, std::size_t outbuf_s) const;

#ifndef _RWSTD_NO_ICONV
#  ifndef _RWSTD_NO_ISO_10646_WCHAR_T    

    // open the iconv descriptor to convert from utf8 to the external encoding
    iconv_t open_iconv_to_ext ();

#  endif   // _RWSTD_NO_ISO_10646_WCHAR_T
#endif   // _RWSTD_NO_ICONV
    
    // add the symbolic name of a character and the raw multibyte
    // character corresponding to it to the character maps
    void add_to_cmaps (const std::string&,
                       const std::string&,
                       bool = false);
        
    // the scanner used to process the charmap file
    Scanner scanner_;
    
    // the name of the codeset
    std::string code_set_name_;

#if defined (_WIN32)
    int codepage_;
#endif // _WIN32

    // n_cmap maps the symbolic name to a narrow character value
    // rn_cmap does the opposite
    std::map <std::string, unsigned char> n_cmap_;
    std::map <unsigned char, std::string> rn_cmap_;

    // mb_cmap maps a multibyte character representation to its
    // corresponding wide character value
    // rmb_cmap does the opposite
    std::map <std::string, wchar_t> mb_cmap_;
    std::map <wchar_t, std::string> rmb_cmap_;

    typedef std::map <wchar_t, std::string>::const_iterator rmb_cmap_iter;
    typedef std::map <std::string, wchar_t>::const_iterator mb_cmap_iter;

    // w_cmap maps the symbolic name to a wide character value
    // rw_cmap does exactly the opposite 
    std::map <std::string, wchar_t> w_cmap_;
    std::map <wchar_t, std::string> rw_cmap_;

    // ucs4_cmap maps the symbolic name to the UCS4 value for that name
    std::map <std::string, wchar_t> ucs4_cmap_;
    std::map <wchar_t, std::string> rucs4_cmap_;

    // the number of bytes in the largest multi-byte value
    int mb_cur_max_;

#ifndef _RWSTD_NO_ICONV
    // the iconv file descriptor that converts to utf8
    iconv_t ic_to_utf8_;

    // the iconv file descriptor that converts from utf8 to external
    iconv_t ic_to_ext_;
#endif   // _RWSTD_NO_ICONV

    // the name of the character map file
    std::string charmap_name_;

    // the name of the C library locale with same encoding
    std::string Clocale_;

    unsigned char largest_nchar_;

    // are we in the utf8 encoding?
    bool in_utf8_;

    // should we create the forward character maps
    bool forward_maps;

    // should we create the reverse character maps
    bool reverse_maps;

    // should we use UCS4 as the internal representation
    bool UCS4_internal_;

    // list of all known symbolic character names
    std::list<std::string> symnames_list_;

    Scanner::token_t next;
};


#endif   // _RWSTD_CHARMAP_H_INCLUDED


