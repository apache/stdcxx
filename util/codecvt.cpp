/***************************************************************************
 *
 * codecvt.cpp
 *
 * $Id: //stdlib/dev/source/stdlib/util/codecvt.cpp#4 $
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

#include "diagnostic.h"   // for issue_diag()
#include "def.h"          // for Def

#include "path.h"         // for get_pathname()
#include "scanner.h"      // for scanner

#include <cassert>        // for assert()
#include <fstream>        // for ifstream, ofstream


// all characters should go into the codecvt_mb_set
void Def::
gen_valid_codecvt_mb_set () 
{
    if (!valid_codecvt_mb_set_.empty())
        return;
    
    for (n_cmap_iter2 n_cmap_it = charmap_.get_n_cmap2().begin();
         n_cmap_it != charmap_.get_n_cmap2().end(); n_cmap_it++) {
        std::string valid = n_cmap_it->first.substr 
            (0, n_cmap_it->first.size() - 1);
        while (valid.size() > 0){
            valid_codecvt_mb_set_.insert (valid);
            valid = valid.substr(0, valid.size() - 1); 
        }
    }
}


void Def::
create_wchar_utf8_table ()
{
    if (!wchar_utf8_to_ext_.empty())
        return;

    n_cmap_iter2 n_cmap_it;
    for (n_cmap_it = charmap_.get_n_cmap2().begin();
         n_cmap_it != charmap_.get_n_cmap2().end();
         n_cmap_it ++) {
        std::string wchar_utf8 = utf8_encode (n_cmap_it->second);
        wchar_utf8_to_ext_.insert (std::make_pair (wchar_utf8, 
                                                   n_cmap_it->first));
    }
}


void Def::
gen_valid_codecvt_wchar_set () {

    if (!valid_codecvt_wchar_set_.empty())
        return;

    create_wchar_utf8_table();

    for (wchar_utf8_iter it = wchar_utf8_to_ext_.begin();
         it != wchar_utf8_to_ext_.end(); it++) {
        std::string str = it->first.substr (0, it->first.size () - 1);
        while (str.size() > 0) {
            valid_codecvt_wchar_set_.insert (str);
            str = str.substr (0, str.size() - 1); 
        }
    }
}


void Def::
gen_valid_codecvt_utf8_set () {

    if (!valid_codecvt_utf8_set_.empty())
        return;

    for (ucs4_cmap_iter it = charmap_.get_ucs4_cmap().begin();
         it != charmap_.get_ucs4_cmap().end(); it++) {
        std::string str = utf8_encode(it->second);
        str = str.substr (0, str.size () - 1);
        while (str.size() > 0) {
            valid_codecvt_utf8_set_.insert (str);
            str = str.substr (0, str.size() - 1); 
        }
    }
}


void Def::
generate_codecvt_table (const std::string &charp, 
                        unsigned int tab_num)
{
    gen_valid_codecvt_mb_set();
    ctype_offset_tab_t tab;
    n_cmap_iter2 n_cmap_it;

    for (unsigned int i = 0; i <= UCHAR_MAX; i++) {

        unsigned char cur_char = (unsigned char)i;

        std::string mb_char = charp;
        mb_char += char (cur_char);

        n_cmap_it = charmap_.get_n_cmap2 ().find (mb_char);

        if (n_cmap_it == charmap_.get_n_cmap2 ().end ()) {

            if (   valid_codecvt_mb_set_.find (mb_char) 
                != valid_codecvt_mb_set_.end ()) {

                ++next_codecvt_tab_num_;
                tab.off [cur_char] = next_codecvt_tab_num_ | 0x80000000;
                generate_codecvt_table (mb_char, next_codecvt_tab_num_);
            }
            else {
                tab.off [cur_char] = UINT_MAX;
            }
        }
        else {
            // get the offset for this character and put it in the table
            tab.off[cur_char] = wchar_off_map_.find (mb_char)->second;
        }
    }

    mb_char_offs_.insert (std::make_pair (tab_num, tab));
}


void Def::
generate_wchar_codecvt_table (const std::string &charp, 
                              unsigned int tab_num)
{
    gen_valid_codecvt_wchar_set();

    ctype_offset_tab_t tab;
    wchar_utf8_iter wu_it;

    for (unsigned int i = 0; i <= UCHAR_MAX; i++){
        unsigned char cur_char = (unsigned char)i;
        std::string mb_char (charp);
        mb_char += (char)cur_char;

        wu_it = wchar_utf8_to_ext_.find (mb_char);
        if (wu_it != wchar_utf8_to_ext_.end()) {
            tab.off[cur_char] = (mb_char_off_map_.find 
                                 (wu_it->second))->second;
        }
        else {
            valid_codecvt_wchar_set_iter wit = valid_codecvt_wchar_set_.find (mb_char);
            if (wit != valid_codecvt_wchar_set_.end()) {
                ++next_wchar_codecvt_tab_num_;
                tab.off[cur_char] = next_wchar_codecvt_tab_num_ | 0x80000000;
                generate_wchar_codecvt_table (mb_char, 
                                              next_wchar_codecvt_tab_num_);
            }
            else {
                tab.off[cur_char] = UINT_MAX;
            }
        }
    }

    wchar_offs_.insert (std::make_pair (tab_num, tab));
}


void Def::
gen_utf8_map()
{
    if (!utf8_map_.empty())
        return;
    
    for (ucs4_cmap_iter it = charmap_.get_ucs4_cmap().begin();
         it != charmap_.get_ucs4_cmap().end(); it++) {
        utf8_map_.insert (std::make_pair(utf8_encode (it->second),
                                         it->second));
    }
}


void Def::
generate_utf8_codecvt_table (const std::string &charp, 
                             unsigned int tab_num)
{
    gen_valid_codecvt_utf8_set();
    gen_utf8_map();

    ctype_offset_tab_t tab;
    utf8_map_iter utf8_it;

    for (unsigned int i = 0; i <= UCHAR_MAX; i++){
        unsigned char cur_char = (unsigned char)i;
        std::string mb_char = charp;
        mb_char += (char)cur_char;
        if ((utf8_it = utf8_map_.find (mb_char)) 
            != utf8_map_.end()) {
            // first get the symbolic name
            std::string str
                = charmap_.get_rucs4_cmap().find(utf8_it->second)->second;
            // then get the internal encoding of the character
            wchar_t int_enc = charmap_.get_w_cmap().find (str)->second;
            // then get the external encoding to use in a lookup in 
            // mb_char_off_map
            str = charmap_.get_rn_cmap2().find (int_enc)->second;
            tab.off[cur_char] = (mb_char_off_map_.find 
                                 (str))->second;
        }
        else {
            if (valid_codecvt_utf8_set_.find (mb_char) 
                != valid_codecvt_utf8_set_.end()) {
                ++next_utf8_codecvt_tab_num_;
                tab.off[cur_char] = next_utf8_codecvt_tab_num_ | 0x80000000;
                generate_utf8_codecvt_table (mb_char, 
                                             next_utf8_codecvt_tab_num_);
            }
            else {
                tab.off[cur_char] = UINT_MAX;
            }
        }
    }
    utf8_offs_.insert (std::make_pair (tab_num, tab));
}


void Def::
generate_xliteration_data ()
{
    // data offset points to the beginning of the data containing
    // the narrow strings character encodings
    unsigned int data_offset = 0;

    // traverse the map and construct the map of offsets
    xlit_map_t::iterator it = xlit_map_.begin ();
    for (; it != xlit_map_.end (); it++) {
        // insert pair(wchar_t value, offset of first string in data block)
        xlit_data_offset_map_.insert (
            std::make_pair (it->first,data_offset));

        // advance the data_offset value to the next "first" string
        std::list<std::string>::iterator sit = 
            it->second.begin ();
        for (; sit != it->second.end (); sit++) {
            data_offset += sit->size () + 1;
        }
        data_offset++;
    }

    // create a new table (first), populate it with default values
    // and insert it in the tables map
    xlit_offset_table_t table0;
    unsigned int k;
    for (k = 0; k < _RWSTD_UCHAR_MAX + 1; k++)
        table0.offset_table [k] = _RWSTD_UINT_MAX;

    // insert it into the map
    xlit_table_map_.insert (std::make_pair(0, table0));

    // traverse the map again and build the tables
    for (it = xlit_map_.begin (); it != xlit_map_.end (); it++) {
        // encode the wchar_t value to UTF-8
        std::string utf8_rep (utf8_encode (it->first));
        data_offset = xlit_data_offset_map_.find (it->first)->second;

        // traverse the utf8 representation string and create the 
        // necessary tables and populate the indexes
        unsigned int table_idx = 0;
        std::string::iterator string_it = utf8_rep.begin ();
        for (; string_it != utf8_rep.end (); string_it++) {
            // get the table corresponding to the current index and locate
            // the value at that index
            xlit_table_map_t::iterator res = xlit_table_map_.find (table_idx);
            assert (res != xlit_table_map_.end ());

            // offset in table
            unsigned char off_idx = (unsigned char)*string_it;

            // res is the iterator pointing to the correct table in the map
            // check the index and if not populated, create a new table
            if (res->second.offset_table [off_idx] == _RWSTD_UINT_MAX) {

                // if this is the last position in the string, then
                // fill the table position with the offset of the string data
                if ((string_it + 1) == utf8_rep.end ()) {
                    xlit_data_offset_map_t::iterator data_it = 
                        xlit_data_offset_map_.find (it->first);
                    assert (data_it != xlit_data_offset_map_.end ());

                    // fill the table position with the found offset
                    res->second.offset_table [off_idx] = data_it->second;
                    continue;
                }

                // create a new table and append it to the map
                xlit_offset_table_t table;
                for (unsigned int i = 0; i < _RWSTD_UCHAR_MAX + 1; i++)
                    table.offset_table [i] = _RWSTD_UINT_MAX;

                // insert it into the map
                unsigned int tmp = xlit_table_map_.size ();
                xlit_table_map_.insert (std::make_pair(tmp, table));
                
                // store its index at correct position in current table
                res->second.offset_table [off_idx] = tmp | 0x80000000;
                table_idx = tmp;
            } else {
                table_idx = 
                    res->second.offset_table [off_idx] & 0x7FFFFFFF;
            }
        }
    }
}


void Def::
write_codecvt (std::string dir_name)
{
    next_wchar_codecvt_tab_num_ = 0;
    next_utf8_codecvt_tab_num_ = 0;

    // if it has been already written
    if (codecvt_written_)
        return;

    // compose the directory name 
    ((dir_name += _RWSTD_PATH_SEP) += "..") += _RWSTD_PATH_SEP;
    dir_name += charmap_.get_code_set_name();

    // check for its existence
    std::ifstream in (dir_name.c_str(), std::ios::in);

    if (in) {
        issue_diag (I_OPENWR, false, 0,
                    "%s exists, skipping\n", dir_name.c_str ());
        return;
    }

    issue_diag (I_OPENWR, false, 0, "writing %s\n", dir_name.c_str ());

    // create the stream with exceptions enabled
    std::ofstream out (dir_name.c_str(), std::ios::binary);
    out.exceptions (std::ios::failbit | std::ios::badbit);
            
    std::size_t temp_off = 0;
    std::size_t count_off = 0;
    n_cmap_iter2 iter;
    for (iter = charmap_.get_n_cmap2().begin();
         iter != charmap_.get_n_cmap2().end(); iter++, count_off ++) {
        mb_char_off_map_.insert (std::make_pair (iter->first, 
                                                 temp_off));
        wchar_off_map_.insert (std::make_pair (iter->first,
                                               count_off));
        temp_off += iter->first.size() + 1;
              
    }

    next_codecvt_tab_num_ = 0;
    next_wchar_codecvt_tab_num_ = 0;
                
    generate_codecvt_table ("", 0);
    generate_wchar_codecvt_table ("", 0);
    generate_utf8_codecvt_table ("", 0);
    
    // generate the transliteration tables and the transliteration data
    generate_xliteration_data ();

    // calculate all offsets
    codecvt_out_.n_to_w_tab_off = 0;
    codecvt_out_.w_to_n_tab_off = codecvt_out_.n_to_w_tab_off 
        + mb_char_offs_.size() * (UCHAR_MAX + 1) 
        * sizeof (unsigned int);

    codecvt_out_.utf8_to_ext_tab_off = codecvt_out_.w_to_n_tab_off
        + wchar_offs_.size() * (UCHAR_MAX + 1)
        * sizeof (unsigned int);

    // insert the transliteration tables here
    codecvt_out_.xliteration_off = codecvt_out_.utf8_to_ext_tab_off
        + utf8_offs_.size() * (UCHAR_MAX + 1)
        * sizeof (unsigned int);

    codecvt_out_.wchar_off = codecvt_out_.xliteration_off + 
        xlit_table_map_.size () * (_RWSTD_UCHAR_MAX + 1) * 
        sizeof (unsigned int);

    codecvt_out_.codeset_off = codecvt_out_.wchar_off 
        + charmap_.get_n_cmap2().size() * 2 * sizeof (wchar_t);

    codecvt_out_.charmap_off = codecvt_out_.codeset_off 
        + charmap_.get_code_set_name().size() + 1;
            
    std::size_t mb_offset = codecvt_out_.charmap_off
        + charmap_.get_charmap_name().size() + 1;

    // compute the size of narrow strings map which added to 
    // mb_offset will give the start of the transliteration data
    std::size_t xlit_data_offset = mb_offset;
    for (iter = charmap_.get_n_cmap2().begin();
         iter != charmap_.get_n_cmap2().end(); iter++) {
        xlit_data_offset += iter->first.size() + 1;
    }

    // now traverse again the utf8 tables for transliteration data
    // and recompute the offsets:
    xlit_table_map_t::iterator xit = xlit_table_map_.begin ();
    for (; xit != xlit_table_map_.end (); xit++) {
        for (unsigned int i = 0; i < _RWSTD_UCHAR_MAX + 1; i++) {
            if (xit->second.offset_table [i] & 0x80000000)
                continue;
            // add the offset for xliteration data 
            xit->second.offset_table [i] += xlit_data_offset;
        }
    }

    wchar_offs_iter wchar_offs_it;
    for (wchar_offs_it = wchar_offs_.begin(); 
         wchar_offs_it != wchar_offs_.end(); wchar_offs_it ++) {
        for (unsigned int i = 0; i <= UCHAR_MAX; i++) {
            if (!((wchar_offs_it->second).off[i] & 0x80000000))
                (wchar_offs_it->second).off[i] += mb_offset;
        }
    }

    utf8_offs_iter utf8_offs_it;
    for (utf8_offs_it = utf8_offs_.begin(); 
         utf8_offs_it != utf8_offs_.end(); utf8_offs_it ++) {
        for (unsigned int i = 0; i <= UCHAR_MAX; i++) {
            if (!((utf8_offs_it->second).off[i] & 0x80000000))
                (utf8_offs_it->second).off[i] += mb_offset;
        }
    }
    codecvt_out_.mb_cur_max = charmap_.get_mb_cur_max();


    // write the codecvt_out structure
    out.write ((char*)&codecvt_out_, sizeof(codecvt_out_));

    issue_diag (I_WRITE, false, 0, "writing char to wchar_t table\n");

    // write the narrow_to_wide tables
    mb_char_offs_iter mb_char_offs_it;
    for (mb_char_offs_it = mb_char_offs_.begin(); 
         mb_char_offs_it != mb_char_offs_.end(); mb_char_offs_it++) {
        for (unsigned int c = 0; c <= UCHAR_MAX; c++) {
            out.write ((const char*)&mb_char_offs_it->second.off[c],
                       sizeof (mb_char_offs_it->second.off[c]));
        }
    }

    issue_diag (I_WRITE, false, 0, "writing wchar_t to char table\n");

    // now write the wide_to_narrow tables
    for (wchar_offs_it = wchar_offs_.begin(); 
         wchar_offs_it != wchar_offs_.end(); wchar_offs_it++) {
        for (unsigned int c = 0; c <= UCHAR_MAX; c++) {
            out.write ((const char*)&wchar_offs_it->second.off[c],
                       sizeof (wchar_offs_it->second.off[c]));
        }
    }
            
    issue_diag (I_WRITE, false, 0, "writing UTF-8 to char table\n");

    // write the utf8_to_external tables
    for (utf8_offs_it = utf8_offs_it = utf8_offs_.begin(); 
         utf8_offs_it != utf8_offs_.end(); utf8_offs_it++) {
        for (unsigned int c = 0; c <= UCHAR_MAX; c++) {
            out.write ((const char*)&utf8_offs_it->second.off[c],
                       sizeof (utf8_offs_it->second.off[c]));
        }
    }

    issue_diag (I_WRITE, false, 0, "writing transliteration lookup table\n");

    // write the transliteration UTF-8 lookup tables
    xit = xlit_table_map_.begin ();
    for (; xit != xlit_table_map_.end (); xit++) {
        unsigned int* ptable = &xit->second.offset_table [0];
        for (unsigned int i = 0; i < _RWSTD_UCHAR_MAX + 1; i++, ptable++) 
            out.write ((const char*)ptable, sizeof (unsigned int));
    }

    issue_diag (I_WRITE, false, 0, "writing UCS to wchar_t table\n");

    // write the locale-encoded wchar_t and the UCS4 wchar_t
    for (iter = charmap_.get_n_cmap2().begin();
         iter != charmap_.get_n_cmap2().end(); iter++) {
        out.write ((const char*)&iter->second, sizeof (iter->second));
        out.write ((const char*)& (charmap_.get_ucs4_cmap().find 
                                   (charmap_.get_rw_cmap().find 
                                    (iter->second)->second))->second,
                   sizeof (wchar_t));
    }

    // write the code_set_name string and charmap string
    out << charmap_.get_code_set_name() << std::ends
        << charmap_.get_charmap_name() << std::ends;


    // write out the narrow character strings
    for (iter = charmap_.get_n_cmap2().begin();
         iter != charmap_.get_n_cmap2().end(); iter++) {
        out.write (iter->first.c_str(), iter->first.size() + 1);
    }

    issue_diag (I_WRITE, false, 0, "writing transliteration table\n");

    // write out the transliteration data
    xlit_map_t::iterator xlit_data_it = xlit_map_.begin ();
    for (; xlit_data_it != xlit_map_.end (); xlit_data_it++) {
        std::list<std::string>::iterator sit = 
            xlit_data_it->second.begin ();
        for (; sit != xlit_data_it->second.end (); sit++) {
            out.write (sit->c_str (), sit->size () + 1);
        }
        out.write ("\0", 1);
    }
}
