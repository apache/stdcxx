/***************************************************************************
 *
 * ctype.cpp
 *
 * $Id$
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

#include "def.h"             // for Def
#include "diagnostic.h"      // for issue_diag()
#include "loc_exception.h"   // for loc_exception
#include "path.h"            // for get_pathname()
#include "scanner.h"         // for scanner

#include <cassert>           // for assert()
#include <cctype>            // for toupper()
#include <cstdio>            // for sprintf()
#include <cstdlib>           // for strtol()
#include <cstring>           // for memset(), strchr()
#include <fstream>           // for ofstream
#include <locale>            // for ctype_base::mask


static wchar_t
convert_literal_to_ucs4 (Scanner::token_t& t)
{
    if ( t.name.size() < 4 || t.name [0] != '<' || t.name [1] != 'U') {
        issue_diag (E_CVT, true, &t,
                    "Symbol could not be converted to UCS-4 value"
                    "(literal form should have been <Uxxxxxxxx>)");
    } 

    long w = std::strtol (t.name.substr (2, t.name.size ()).c_str (),
                               0, 16);
    if (w > _RWSTD_WCHAR_T_MAX) {
        // issue_diag intercepted in process_transliteration_statement
        // but will render -w switch useless; just throw here
        throw loc_exception ("symbol could not be converted to UCS-4 "
                             "value (value outside wchar_t range)");
    }

    return wchar_t (w);
}


bool Def::get_n_val (const Scanner::token_t &tok, unsigned char &val)
{
    bool got_val = true;

    n_cmap_iter n_cmap_pos;

    switch (tok.token) {

    case Scanner::tok_sym_name:
        n_cmap_pos = charmap_.get_n_cmap ().find (tok.name);
        if (charmap_.get_n_cmap ().end () != n_cmap_pos)
            val = n_cmap_pos->second;
        else 
            got_val = false;
        break;

    case Scanner::tok_decimal_value:
    case Scanner::tok_hex_value:
    case Scanner::tok_octal_value:
        if (charmap_.mbcharlen (tok.name) == 1)
            val = scanner_.convert_escape (tok.name.c_str ());
        else 
            got_val = false;
        break;

    default:
        val = tok.name [0];
    }

    return got_val;
}


bool Def::get_w_val (const Scanner::token_t &tok, wchar_t &val)
{
    bool got_val = true;

    w_cmap_iter w_cmap_pos;

    switch (tok.token) {

    case Scanner::tok_sym_name:
        w_cmap_pos = charmap_.get_w_cmap ().find (tok.name);
        if (charmap_.get_w_cmap ().end () != w_cmap_pos)
            val = w_cmap_pos->second;
        else 
            got_val = false;
        break;

    case Scanner::tok_decimal_value:
    case Scanner::tok_hex_value:
    case Scanner::tok_octal_value:
        return charmap_.convert_to_wc ("", tok.name, val);

    default:
        val = wchar_t (tok.name [0]);
    }

    return got_val;
}


// process hexadecimal symbolic ellipsis, decimal symbolic ellipsis,
// and double increment hexadecimal symbolic ellipsis
void Def::process_sym_ellipsis (const std::string& start_sym,
                                const std::string& end_sym,
                                Scanner::token_id type,
                                std::ctype_base::mask m) {
    
    // first, get the alphabetic beginning of the sym name
    std::size_t idx = 0;
    std::string begin;            

    if (type == Scanner::tok_dellipsis 
        || type == Scanner::tok_doub_inc_ellipsis) {
        while (idx < start_sym.size () && 
               ((start_sym[idx] < '0' || start_sym[idx] > '9') &&
                (start_sym[idx] < 'a' || start_sym[idx] > 'f') && 
                (start_sym[idx] < 'A' || start_sym[idx] > 'F')))
            begin += start_sym[idx++];
    }
    else {
        while (idx < start_sym.size () && 
               ((start_sym[idx] < '0' || start_sym[idx] > '9')))
            begin += start_sym[idx++];
    }

    std::string num_str;  // the numeric portion of the sym name

    // get the numeric portion of the sym_name, this is the portion
    // that will be different for each sym_name within the ellipsis
    while (idx < start_sym.size() && start_sym[idx] != '>')
        num_str += start_sym[idx++];
            
    std::size_t num_len = num_str.size();
            
    // convert the numeric string to a long

    unsigned long num = 0;
    if (type == Scanner::tok_dellipsis 
        || type == Scanner::tok_doub_inc_ellipsis)
        num = std::strtoul (num_str.c_str(), (char**)0, 16);
    else
        num = std::strtoul (num_str.c_str(), (char**)0, 10);
    
    // now create the symbolic name
    char next_num [32];
    std::string sym_name;
    do {
        if (   type == Scanner::tok_dellipsis 
            || type == Scanner::tok_doub_inc_ellipsis) {
            std::sprintf (next_num, "%lx", num++);

            if (type == Scanner::tok_doub_inc_ellipsis)
                num++;

            // the numeric portion of the sym name must be uppercase
            for (std::size_t i = 0; next_num[i] != '\0'; i++)
                if (next_num[i] >= 'a' && next_num[i] <= 'f')
                    next_num[i] = (std::toupper) (next_num[i]);
        }
        else {
            std::sprintf (next_num, "%ld", num++);
        }

        sym_name = begin;
        for (std::size_t leading_zeros = num_len  - std::strlen (next_num);
             leading_zeros > 0; leading_zeros--)
            sym_name += '0';
        sym_name += next_num;
        sym_name += '>';

        next.name = sym_name;
        unsigned char n_val;
        // if the value is <= UCHARMAX then we will add mask to the
        // mask_tab table
        if (get_n_val (next, n_val)) {
            ctype_out_.mask_tab [n_val] |= m;
        }

        wchar_t w_val;
        // if the value is not in the charmap then we cannot continue
        if (!get_w_val (next, w_val)) {
            /*
            warnings_occurred_ = 
                issue_diag (W_SYM, false, 
                            &next, "symbolic name %s "
                            "was not found in the character map; "
                            "ignoring character\n", next.name.c_str()) 
                || warnings_occurred_;
            */
        }
        else {
            // add the mask to the mask map
            mask_iter mask_pos = mask_.find (w_val);
            if (mask_pos != mask_.end())
                mask_pos->second |= m;
            else {
                mask_.insert (std::make_pair (w_val, m));     
            }
        }                
    } while (sym_name != end_sym);

}


// process_mask is called from process_ctype when process_ctype reaches
// a mask defintion (ie. upper, lower, digit).  It processes each token
// until a new line is reached (which designates the end of the mask
// definition).  If the token is a symbolic name then it looks up the name
// in the cmap map to find the value of the character, otherwise it uses
// the value of the character and adds the character to the mask map (if
// the character is not alreay there) with the current mask.
void Def::
process_mask (std::ctype_base::mask m) 
{
    next = scanner_.next_token();
    Scanner::token_t nextnext = scanner_.next_token();
    while (next.token != Scanner::tok_nl){                
        if (Scanner::tok_ellipsis == nextnext.token) {
            // if there are ellipsis then include all characters in between
            // the values that surround the ellipsis
            nextnext = scanner_.next_token();
            
            // first we need to handle narrow chars if the range is a range 
            // of narrow characters
            unsigned char n_start_val;
            unsigned char n_end_val;
            // check to see if the start value is in the narrow map
            // if it is then we have to add some values to the narrow mask_tab
            if (get_n_val (next, n_start_val) 
                && get_n_val(nextnext, n_end_val)) {
                // both the start value and end value are in the mask table
                // so add the mask to the narrow table from start value
                // to end_value.  Make sure that start < end
                if (n_start_val > n_end_val)
                    issue_diag (E_RANGE, true, 
                                &next, "illegal range found in "
                                "ctype definition\n");
                for(unsigned char n_current_val = n_start_val;
                    n_current_val <= n_end_val; n_current_val++) {
                    ctype_out_.mask_tab [n_current_val] |= m;
                }
            }

            wchar_t w_start_val;
            wchar_t w_end_val;
            if (!get_w_val (next, w_start_val) 
                || !get_w_val (nextnext, w_end_val)) {
                warnings_occurred_ = 
                    issue_diag (W_RANGE, false, 
                                &next, "beginning or endpoint of range "
                                "was not found in the character map; "
                                "ignoring range\n") || warnings_occurred_;
            }
            else {
                wchar_t w_current_val = w_start_val;
                while (w_current_val != w_end_val) {
                    mask_iter mask_pos = mask_.find (w_current_val);
                    if(mask_pos != mask_.end())
                        mask_pos->second |= m;
                    else {
                        mask_.insert(std::make_pair(w_current_val, m));
                    }
                    
                    w_current_val = charmap_.increment_val (w_current_val);
                }
                // now add the end_value
                mask_iter mask_pos = mask_.find (w_end_val);
                if(mask_pos != mask_.end())
                    mask_pos->second |= m;
                else {
                    mask_.insert(std::make_pair(w_end_val, m));
                }

            }

            next = scanner_.next_token();

        }
        else if (Scanner::tok_dellipsis == nextnext.token 
                 || Scanner::tok_qellipsis == nextnext.token
                 || Scanner::tok_doub_inc_ellipsis == nextnext.token) {

            Scanner::token_id tok = nextnext.token;
            // the next token will be the end of the range
            nextnext = scanner_.next_token ();
            process_sym_ellipsis (next.name, nextnext.name, tok, m);
            next = scanner_.next_token();
        }
        else if (Scanner::tok_sym_name == next.token) {

            unsigned char n_val;
            // if the value is <= UCHARMAX then we will add this mask to the
            // mask_tab table
            if (get_n_val (next, n_val)) {
                ctype_out_.mask_tab [n_val] |= m;
            }

            wchar_t w_val;
            // if the value is not in the charmap then we cannot continue
            if (!get_w_val (next, w_val)) {
                /*
                warnings_occurred_ = 
                    issue_diag (W_SYM, false, 
                                &next, "symbolic name %s "
                                "was not found in the character map; "
                                "ignoring character\n", next.name.c_str()) 
                    || warnings_occurred_;
                */
            }
            else {
                // add the mask to the mask map
                mask_iter mask_pos = mask_.find (w_val);
                if (mask_pos != mask_.end())
                    mask_pos->second |= m;
                else {
                    mask_.insert (std::make_pair (w_val, m));     
                }
            }
            next = nextnext;
        }

        else if (   Scanner::tok_decimal_value == next.token 
                 || Scanner::tok_hex_value == next.token
                 || Scanner::tok_octal_value == next.token) {

            unsigned char n_val;
            if (get_n_val (next, n_val))
                ctype_out_.mask_tab[n_val] |= m;
            
            wchar_t w_val;
            if (get_w_val (next, w_val)) {
                mask_iter mask_pos = mask_.find (w_val);
                if (mask_pos != mask_.end())
                    mask_pos->second |= m;
                else {
                    mask_.insert (std::make_pair (w_val, m));
                }
                
            }
                
            next = nextnext;

        }

        else {
            // the ctype category definition contains non-symbolic characters
            // the actual value of the characters will be used.  This is
            // unportable
            warnings_occurred_ = 
                issue_diag (W_SYM, false, &next,
                            "non-symbolic character %s found in ctype "
                            "definition.\n", next.name.c_str()) 
                || warnings_occurred_;
            if (next.name.size () != 1)
                warnings_occurred_ = 
                    issue_diag (W_SYM, false, &next, 
                                "non-symbolic character %s in ctype "
                                "definition is longer than one char in "
                                "length. Ignoring character\n", 
                                next.name.c_str()) || warnings_occurred_;
            else {
                ctype_out_.mask_tab [(unsigned char) next.name[0]] |= m;
                wchar_t mb_val = wchar_t ((unsigned char)next.name[0]);
                mask_iter mask_pos = mask_.find (mb_val);
                if (mask_pos != mask_.end())
                    mask_pos->second |= m;
                else
                    mask_.insert (std::make_pair (mb_val, m));
            }
            next = nextnext;
        }
        // if we are not at the newline get the next token
        if (Scanner::tok_nl != next.token)
            nextnext = scanner_.next_token();

    }
}


// process_upper_lower processes the toupper and tolower ctype categories
// These categories consist of pairs of characters in the format '(<a>,<b>)'
void Def::
process_upper_lower (Scanner::token_id tok)
{
    assert (Scanner::tok_toupper == tok || Scanner::tok_tolower == tok);

    // process the toupper and tolower ctype categories

    next = scanner_.next_token();
    while (next.token != Scanner::tok_nl) {
        std::string sym, sym2;
        
        // seperate the symbolic names in the toupper or tolower pair
        // and place the result in sym and sym2
        strip_pair(next.name, sym, sym2);

        // first process toupper or tolower for the narrow characters
        n_cmap_iter sym1_pos = charmap_.get_n_cmap().find (sym);
        n_cmap_iter sym2_pos = charmap_.get_n_cmap().find (sym2);
        if (sym1_pos != charmap_.get_n_cmap().end() 
            && sym2_pos != charmap_.get_n_cmap().end()) {
            if (tok == Scanner::tok_toupper)
                ctype_out_.toupper_tab [sym1_pos->second] = sym2_pos->second;
            else
                ctype_out_.tolower_tab [sym1_pos->second] = sym2_pos->second;
        }

        // now process toupper or tolower fot the wide characters
        w_cmap_iter wsym1_pos = charmap_.get_w_cmap().find (sym);
        w_cmap_iter wsym2_pos = charmap_.get_w_cmap().find (sym2);
        if (wsym1_pos == charmap_.get_w_cmap().end())
            warnings_occurred_ = 
                issue_diag (W_SYM, false, &next, 
                            "unknown symbol name %s found in "
                            "LC_CTYPE definition\n", 
                            sym.c_str()) || warnings_occurred_;
        else if (wsym2_pos == charmap_.get_w_cmap().end())
            warnings_occurred_ = 
                issue_diag (W_SYM, false, &next, 
                            "unknown symbol name %s found in "
                            "LC_CTYPE definition\n", 
                            sym2.c_str()) || warnings_occurred_;
        else {
            if (tok == Scanner::tok_toupper)
                upper_.insert (std::make_pair (wsym1_pos->second, 
                                              wsym2_pos->second));
            else
                lower_.insert (std::make_pair (wsym1_pos->second, 
                                              wsym2_pos->second));
        }
        next = scanner_.next_token();
    }
}


void Def::
process_transliteration_statement ()
{
    // convert the name we have for a symbolic name
    std::string sym_s (next.name);
    wchar_t sym_w;
    try {
        sym_w = convert_literal_to_ucs4 (next);
    }
    catch (loc_exception&) {
        scanner_.ignore_line ();
        return;
    }
    catch (...) {
        throw;
    }
    
    // add a new element to the transliteration map
    std::pair<xlit_map_t::iterator, bool> res = 
        xlit_map_.insert (std::make_pair(sym_w, std::list<std::string>()));
    if (res.second == false) {
        scanner_.ignore_line ();
        return;
    }

    xlit_map_t::iterator& it = res.first;
    next = scanner_.next_token ();
    while (next.token != Scanner::tok_nl) {

        switch (next.token) {
        case Scanner::tok_sym_name: {
            // convert this symbol to a string with the external encoding 
            w_cmap_iter w_pos = charmap_.get_w_cmap().find (next.name);
            if (w_pos != charmap_.get_w_cmap().end()) {
                it->second.push_back(convert_to_ext(w_pos->second));
            }
            break;
        }
        case Scanner::tok_string: {
            // for empty names there is no processing
            if (next.name.size () <= 2)
                break;

            // convert this symbol or string of symbols to a string
            // with the external encoding 
            
            std::string enc = convert_string (next.name);
            if (enc.empty())
                break;
            it->second.push_back (enc);

            break;
        }
        default:
            issue_diag (W_SYNTAX, false, &next,
                        "unexpected token while processing "
                        "a transliteration statement\n");
            break;
        }

        next = scanner_.next_token ();
    }

    // if the transliteration statement contained only symbols undefined in
    // the character map, dump this balast
    if (it->second.empty ())
        xlit_map_.erase (it);
}


void Def::
process_transliteration ()
{
    // used in processing  the include directive
    int nesting_level = 0;
    std::list<std::string> file_list;

    while (true) {
        next = scanner_.next_token ();

        switch (next.token) {
        case Scanner::tok_include: {

            // extract all file names from the list
            std::list<std::string> tmp_list;
            while (next.token != Scanner::tok_nl) {
                next = scanner_.next_token ();
                if (next.token == Scanner::tok_string &&
                    next.name.size () > 2)
                    tmp_list.push_back (next.name);
            }

            // insert this list into the main list - at beginning
            file_list.insert (file_list.begin (), 
                              tmp_list.begin (), tmp_list.end ());

            // get the top of the list
            std::string fname (file_list.front ());
            file_list.pop_front ();

            // bump up the nesting level
            nesting_level++;

            // get the full path for the included file and open it
            scanner_.open (get_pathname (strip_quotes (fname), next.file));

            // get comment char and escape char; 
            // these informations are stored by the scanner
            while ((next = scanner_.next_token ()).token 
                   != Scanner::tok_translit_start );
            
            break;
        }
        case Scanner::tok_sym_name: {
            process_transliteration_statement ();
            break;
        }
        case Scanner::tok_translit_end: {
            if (nesting_level == 0)
                return;

            // decrement nesting level, close opened file
            nesting_level--;
            scanner_.close ();

            // check if the list of files is empty or not
            if (file_list.empty ())
                break;

            // if not take the following file and open it
            std::string fname (file_list.front ());
            file_list.pop_front ();

            // bump up the nesting level
            nesting_level++;

            // get the full path for the included file and open it
            scanner_.open (get_pathname (strip_quotes (fname), next.file));

            // get comment char and escape char; 
            // these informations are stored by the scanner
            while ((next = scanner_.next_token ()).token 
                   != Scanner::tok_translit_start );
            
        }
        default:
            break;
        }
    }
}


void Def::
process_ctype ()
{
    ctype_def_found_ = true;

    // used in processing  the copy/include directive
    int nesting_level = 0;

    while ((next = scanner_.next_token()).token != Scanner::tok_ctype) {

        switch(next.token) {

        case Scanner::tok_copy: {
            // when we see the copy directive in the ctype definition we 
            // are going to either create the shared database and create a 
            // symbolic link to it, or we are going to create a symbolic link
            // to the already existing shared ctype database.

            next = scanner_.next_token();
            if (next.token != Scanner::tok_string)
                issue_diag (E_SYNTAX, true, &next,
                            "expected string following \"copy\" directive\n"); 
#ifndef _MSC_VER           

            ctype_symlink_ = true;

            // first lets make sure that the ctype database for this
            // locale hasn't already been generated
            ctype_filename_ = output_name_;
            // strip off the last directory
            ctype_filename_ = ctype_filename_.substr 
                (0, ctype_filename_.rfind
                 (_RWSTD_PATH_SEP, ctype_filename_.length() - 1) + 1);
            ctype_filename_ += strip_quotes(next.name);
            ctype_filename_ += ".ctype.";
            ctype_filename_ += charmap_.get_charmap_name();
            std::ifstream f (ctype_filename_.c_str(), std::ios::binary);
            if (f) {
                // the database exists so simply create a sym link to it
                ctype_written_ = true;
                f.close();
                continue;
            }

#endif  // _MSC_VER            

            // bump up the nesting level
            nesting_level++;

            issue_diag (I_STAGE, false, 0, "processing copy directive\n");

            // open the file
            scanner_.open (get_pathname (strip_quotes (next.name), next.file));

            // get comment char and escape char; 
            // these informations are stored by the scanner
            while ((next = scanner_.next_token ()).token 
                   != Scanner::tok_ctype ){
                // the LC_IDENTIFICATION section may also have a 
                // LC_CTYPE token that will mess up the parsing
                if (next.token == Scanner::tok_identification) {
                    while ((next = scanner_.next_token()).token
                           != Scanner::tok_end );
                    next = scanner_.next_token();
                }
            }

            break;
        }
        case Scanner::tok_nl:
            break;

        case Scanner::tok_upper:
            process_mask(std::ctype_base::upper);
            break;

        case Scanner::tok_lower:
            process_mask(std::ctype_base::lower);
            break;

        case Scanner::tok_alpha:
            process_mask(std::ctype_base::alpha);
            break;

        case Scanner::tok_digit:
            process_mask(std::ctype_base::digit);
            break;

        case Scanner::tok_space:
            process_mask(std::ctype_base::space);
            break;

        case Scanner::tok_cntrl:
            process_mask(std::ctype_base::cntrl);
            break;

        case Scanner::tok_punct:
            process_mask(std::ctype_base::punct);
            break;

        case Scanner::tok_graph:
            process_mask(std::ctype_base::graph);
            break;

        case Scanner::tok_print:
            process_mask(std::ctype_base::print);
            break;

        case Scanner::tok_xdigit:
            process_mask(std::ctype_base::xdigit);
            break;

        case Scanner::tok_toupper:
            process_upper_lower (Scanner::tok_toupper);
            break;

        case Scanner::tok_tolower:
            process_upper_lower (Scanner::tok_tolower);
            break;

        case Scanner::tok_blank:
            scanner_.ignore_line();
            break;

        case Scanner::tok_translit_start:
            process_transliteration ();
            break;

        case Scanner::tok_end:
            next = scanner_.next_token();
            if (next.token == Scanner::tok_ctype) {
                // end of ctype block
                if (nesting_level == 0) 
                    return;

                nesting_level--;
                scanner_.close ();
            } else
                issue_diag (E_SYNTAX, true, &next,
                            "wrong section name in END directive\n");

            break;

        default:
            // ignore locale specific character classes because the c++
            // library does not make use of them
            scanner_.ignore_line();
            break;

        }
    }
}


void Def::
write_ctype (std::string dir_name)
{
    // dir_name cannot be empty
    assert (!dir_name.empty());

    if (ctype_filename_.empty ()) {
        ctype_filename_ = dir_name + _RWSTD_PATH_SEP + "LC_CTYPE";
        ctype_symlink_ = false;
    }

    // if a CTYPE section was not found or ctype info has been already written
    // in the database
    if (ctype_def_found_ && !ctype_written_) {      

        issue_diag (I_OPENWR, false, 0,
                    "writing %s\n", ctype_filename_.c_str ());

        std::ofstream out (ctype_filename_.c_str(), std::ios::binary);
        out.exceptions (std::ios::failbit | std::ios::badbit);
        
        // calculate the offsets for the wchar_t arrays
        ctype_out_.wtoupper_off = 0;
        ctype_out_.wtolower_off = ctype_out_.wtoupper_off 
            + upper_.size() * sizeof (_RW::__rw_upper_elm);
        ctype_out_.wmask_off = ctype_out_.wtolower_off
            + lower_.size() * sizeof (_RW::__rw_lower_elm);
        ctype_out_.wmask_s = mask_.size();

        // calculate the offsets for the codeset name string and character
        // map name string
        ctype_out_.codeset_off = ctype_out_.wmask_off
            + mask_.size() * sizeof (_RW::__rw_mask_elm);
        ctype_out_.charmap_off = ctype_out_.codeset_off 
            + charmap_.get_code_set_name().size() + 1;
       
        ctype_out_.mb_cur_max = charmap_.get_mb_cur_max();
        std::size_t i;

        for (i = 0; i <= UCHAR_MAX; i++) {
            if(0 == ctype_out_.toupper_tab[i])
                ctype_out_.toupper_tab[i] = (char)i;
            if(0 == ctype_out_.tolower_tab[i])
                ctype_out_.tolower_tab[i] = (char)i;
        }
        
        // write the ctype_out structure
        out.write ((char*)&ctype_out_, sizeof(ctype_out_));

        // print out the wide character arrays
        for(upper_iter u_pos = upper_.begin(); u_pos != upper_.end(); u_pos++){
            _RW::__rw_upper_elm elm = {u_pos->first, u_pos->second};
            out.write((char*)&elm, sizeof(elm));
        }
        for(lower_iter l_pos = lower_.begin(); l_pos != lower_.end(); l_pos++){
            _RW::__rw_lower_elm elm = {l_pos->first, l_pos->second};
            out.write((char*)&elm, sizeof(elm));
        }
        for(mask_iter m_pos = mask_.begin(); m_pos != mask_.end(); m_pos++){
            _RW::__rw_mask_elm elm = {m_pos->first, m_pos->second};
            out.write((char*)&elm, sizeof(elm));
        }

        // write the code_set_name string and charmap string
        out << charmap_.get_code_set_name() << std::ends
            << charmap_.get_charmap_name() << std::ends;
    
    }

#ifndef _MSC_VER
    
    if (ctype_symlink_) {

        std::string xname (ctype_filename_);
        if (xname [0] != _RWSTD_PATH_SEP) {
            xname = std::string (".."); 
            xname += _RWSTD_PATH_SEP;
            xname += ctype_filename_.substr (
                ctype_filename_.rfind (_RWSTD_PATH_SEP) + 1,
                ctype_filename_.size ());
        }

        std::string sname ("LC_CTYPE");
        create_symlink (output_name_, xname, sname);
        return;
    }

#endif  // _MSC_VER

}
