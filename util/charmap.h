/***************************************************************************
 *
 * charmap.h
 *
 * $Id: //stdlib/dev/source/stdlib/util/charmap.h#41 $
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


#ifndef _RWSTD_LOC_CHARMAP_H_INCLUDED
#define _RWSTD_LOC_CHARMAP_H_INCLUDED

#include <string>
#include <map>
#include <set>

#ifndef _MSC_VER
#  include <iconv.h>
#endif  // _MSC_VER

#include "scanner.h"

class Charmap
{
public:
    Charmap(const char* /*corresponding C library locale*/,
            const char* /*filename*/, 
            bool /*is utf8 encoding?*/, 
            bool /*create_forward_charmaps*/,
            bool /*create_reverse_charmaps*/, 
            bool /*use UCS4 internally*/);    
  
    // get the narrow character map
    const std::map<std::string, unsigned char >& get_n_cmap() const {
        return n_cmap_;
    }
    
    // get the reverse narrow character map
    const std::map<unsigned char, std::string >& get_rn_cmap() const {
        return rn_cmap_;
    }

    // get the wide character map
    const std::map<std::string, wchar_t >& get_w_cmap() const {
        return w_cmap_;
    }

    // get the reverse wide character map
    const std::map<wchar_t, std::string >& get_rw_cmap() const {
        return rw_cmap_;
    }

    const std::map<std::string, wchar_t>& get_n_cmap2() const {
        return n_cmap2_;
    }

    const std::map<wchar_t, std::string>& get_rn_cmap2() const {
        return rn_cmap2_;
    }

    // get the string value map
    const std::map<std::string, std::string>& get_strval_map() const {
        return strval_map_;
    }

    const std::map <std::string, wchar_t>& get_ucs4_cmap () const {
        return ucs4_cmap_;
    }

    const std::map <wchar_t, std::string>& get_rucs4_cmap () const {
        return rucs4_cmap_;
    }

    const std::set<std::string>& get_valid_mb_set() const {
        return valid_mb_set_;
    }

    const std::set<std::string>& get_valid_wchar_set() const {
        return valid_wchar_set_;
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
    
    // increment the wide character value to the next encoded character in
    // this codeset
    wchar_t increment_val (const wchar_t) const;

private:
    
    // process the characters implicitly defined by using ellipsis between
    // two explicitly defined characters
    void process_ellipsis (const Scanner::token_t&, int);

    // process the charmap file making the necessary mappings in the cmaps
    void process_chars();

    // increment the multi-byte string by 1.
    const char* increment_strval (const char*);

    // make sure that all the characters in the portable character set are
    // defined in the character map
    void verify_portable_charset ();

#ifndef _MSC_VER
    // open the iconv descriptor to convert to utf8
    iconv_t open_iconv_to_utf8 () const;
#endif  // _MSC_VER

    std::string parse_ext_strval (const std::string&) const;

    // convert a multi-byte string to a utf8 multi-byte string
    char* convert_to_utf8 (const char *inbuf, std::size_t inbuf_s, 
                           char *outbuf, std::size_t outbuf_s) const;

#ifndef _MSC_VER
#  ifndef _RWSTD_NO_ISO_10646_WCHAR_T    
    // open the iconv descriptor to convert from utf8 to the external encoding
    iconv_t open_iconv_to_ext ();

#  endif   // _RWSTD_NO_ISO_10646_WCHAR_T
#endif  // _MSC_VER
    
    // add the sym_name and multi-byte character to the character maps
    void add_to_cmaps (const std::string&, const std::string&);
        
    // the scanner used to process the charmap file
    Scanner scanner_;
    
    // the name of the codeset
    std::string code_set_name_;

#if defined (_MSC_VER)
    int codepage_;
#endif // _MSC_VER

    // n_cmap maps the symbolic name to a narrow character value
    // rn_cmap does exactly the opposite
    std::map <std::string, unsigned char> n_cmap_;
    std::map <unsigned char, std::string> rn_cmap_;
    std::map <std::string, wchar_t> n_cmap2_;
    std::map <wchar_t, std::string> rn_cmap2_;
    typedef std::map <wchar_t, std::string>::const_iterator rn_cmap2_iter;
    typedef std::map <std::string, wchar_t>::const_iterator n_cmap2_iter;

    std::set<std::string> valid_mb_set_;
    std::set<std::string> valid_wchar_set_;

    // w_cmap maps the symbolic name to a wide character value
    // rw_cmap does exactly the opposite 
    std::map <std::string, wchar_t> w_cmap_;
    std::map <wchar_t, std::string> rw_cmap_;

    // ucs4_cmap maps the symbolic name to the UCS4 value for that name
    std::map <std::string, wchar_t> ucs4_cmap_;
    std::map <wchar_t, std::string> rucs4_cmap_;

    // the number of bytes in the largest multi-byte value
    int mb_cur_max_;

#ifndef _MSC_VER
    // the iconv file descriptor that converts to utf8
    iconv_t ic_to_utf8_;

    // the iconv file descriptor that converts from utf8 to external
    iconv_t ic_to_ext_;
#endif  // _MSC_VER

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

    // maps the string value to the symbolic name
    // this map is required for the UNDEFINED keyword
    // in localedef where the elements must be added in
    // increasing encoded order.
    std::map<std::string, std::string> strval_map_;

    Scanner::token_t next;
};


#endif   // _RWSTD_LOC_CHARMAP_H_INCLUDED

