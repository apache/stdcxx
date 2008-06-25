/***************************************************************************
 *
 * codecvt.cpp
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

#include "diagnostic.h"   // for issue_diag()
#include "def.h"          // for Def

#include "path.h"         // for get_pathname()
#include "scanner.h"      // for scanner

#include <cassert>        // for assert()
#include <climits>        // for UCHAR_MAX
#include <cstring>        // for memset()
#include <fstream>        // for ifstream, ofstream


typedef std::map<std::string, wchar_t>::const_iterator n_cmap_citer2;


std::size_t Def::
gen_mbchar_tables (codecvt_offsets_map_t           &tab,
                   std::map<std::string, unsigned> &off_map,
                   const std::string               &charp /* = "" */,
                   unsigned                         tabno /* = 0 */)
{
    // upon the first call (but not during subsequent recursive calls)
    // generate a set of multibyte prefixes from the set of all known
    // multibyte characters
    static unsigned               ntabs   = 0;
    static std::set<std::string>* pfx_set = 0;

    const n_cmap_citer2 mb_map_end = charmap_.get_mb_cmap ().end ();

    if (0 == pfx_set) {
        pfx_set = new std::set<std::string>;

        // iterate over the range of valid multibyte characters
        // obtained from the charmap and generate a complete
        // subset of non-empty multibyte prefixes from each
        unsigned off = 0;

        const n_cmap_citer2 mb_map_begin = charmap_.get_mb_cmap ().begin ();

        for (n_cmap_citer2 it = mb_map_begin; it != mb_map_end; ++it, ++off) {

            // insert the ordinal number of each multibyte character
            // into a map for fast lookup later
            off_map.insert (std::make_pair (it->first, off));

            // generate non-empty prefixes up to one byte less
            // in length than the complete multibyte character
            for (std::string prefix = it->first; 1 < prefix.size (); ) {
                prefix = prefix.substr (0, prefix.size () - 1);
                pfx_set->insert (prefix);
            }
        }
    }

    // number of valid characters inserted into the tables
    std::size_t nchars = 0;

    // an array of offsets to the multibyte character or to the next
    // array containing such offsets (defined recursively for up to
    // MB_CUR_MAX levels of nesting)
    codecvt_offset_tab_t* const offsets = new codecvt_offset_tab_t;

    std::string mb_char (charp + '\0');

    for (unsigned i = 0; i <= UCHAR_MAX; ++i) {

        unsigned char cur_char = (unsigned char)i;

        mb_char [mb_char.size () - 1] = char (cur_char);

        if (mb_map_end == charmap_.get_mb_cmap ().find (mb_char)) {
            // mb_char is not a complete, valid multibyte character
            // check to see if it's a prefix of one

            if (pfx_set->find (mb_char) == pfx_set->end ()) {
                // mb_char is not a prefix of a valid multibyte
                // character, mark it invalide
                offsets->off [cur_char] = UINT_MAX;
            }
            else {
                // mb_char is a prefix of a valid multibyte character,
                // set the MSB to denote that it "continues" in the
                // table at the next higher offset
                offsets->off [cur_char] = ++ntabs | 0x80000000;

                // generate that table
                nchars += gen_mbchar_tables (tab, off_map, mb_char, ntabs);
            }
        }
        else {
            // mb_char is a complete, valid miltibyte character
            // insert its ordinal number (offset) into the array
            offsets->off [cur_char] = off_map.find (mb_char)->second;
            ++nchars;
        }
    }

    // insert the completely populated table into the map
    tab.insert (std::make_pair (tabno, offsets));

    if (0 == ntabs) {
        // clean up on return from the topmost (non-recursive) call
        delete pfx_set;
        pfx_set = 0;
    }

    return nchars;
}


std::size_t Def::
gen_wchar_tables (codecvt_offsets_map_t &tab,
                  const std::string     &charp /* = "" */, 
                  unsigned int           tabno /* = 0 */)
{
    // upon the first call (but not during subsequent recursive calls)
    // generate a set of multibyte prefixes from the set of all known
    // multibyte characters
    static unsigned                            ntabs   = 0;
    static std::set<std::string>              *pfx_set = 0;
    static std::map<std::string, unsigned>    *off_map = 0;
    static std::map<std::string, std::string> *utf_map = 0;

    if (0 == utf_map) {
        pfx_set = new std::set<std::string>;
        off_map = new std::map<std::string, unsigned>;
        utf_map = new std::map<std::string, std::string>;

        const n_cmap_citer2 first = charmap_.get_mb_cmap ().begin ();
        const n_cmap_citer2 last  = charmap_.get_mb_cmap ().end ();

        unsigned off = 0;

        for (n_cmap_citer2 it = first; it != last; ++it) {

            off_map->insert (std::make_pair (it->first, off));

            off += unsigned (it->first.size ()) + 1;

            std::string utf = utf8_encode (it->second);

            utf_map->insert (std::make_pair (utf, it->first));

            while (1 < utf.size ()) {
                utf = utf.substr (0, utf.size () - 1);
                pfx_set->insert (utf);
            }
        }
    }

    codecvt_offset_tab_t* const offsets = new codecvt_offset_tab_t;

    // number of valid characters inserted into the tables
    std::size_t nchars = 0;

    std::string mb_char (charp + '\0');

    for (unsigned i = 0; i <= UCHAR_MAX; ++i) {

        unsigned char cur_char = (unsigned char)i;

        mb_char [mb_char.size () - 1] = char (cur_char);

        const wchar_utf8_iter it = utf_map->find (mb_char);
        if (it == utf_map->end ()) {
            if (pfx_set->find (mb_char) == pfx_set->end ()) {
                offsets->off [cur_char] = UINT_MAX;
            }
            else {
                offsets->off [cur_char] = ++ntabs | 0x80000000;

                nchars += gen_wchar_tables (tab, mb_char, ntabs);
            }
        }
        else {
            offsets->off [cur_char] = off_map->find (it->second)->second;

            ++nchars;
        }
    }

    tab.insert (std::make_pair (tabno, offsets));

    if (0 == ntabs) {
        // clean up
        delete pfx_set;
        delete utf_map;

        pfx_set = 0;
        utf_map = 0;
    }

    return nchars;
}


std::size_t Def::
gen_utf8_tables (codecvt_offsets_map_t           &tab,
                 std::map<std::string, unsigned> &off_map,
                 const std::string               &charp /* = "" */,
                 unsigned                         tabno /* = 0 */)
{
    static unsigned                        ntabs   = 0;
    static std::set<std::string>          *pfx_set = 0;
    static std::map<std::string, wchar_t> *utf_map = 0;

    if (0 == pfx_set) {
        pfx_set = new std::set<std::string>;

        const ucs4_cmap_iter first = charmap_.get_ucs4_cmap ().begin ();
        const ucs4_cmap_iter last  = charmap_.get_ucs4_cmap ().end ();

        for (ucs4_cmap_iter it = first; it != last; ++it) {

            for (std::string prefix = utf8_encode (it->second);
                 1 < prefix.size (); ) {
                prefix = prefix.substr (0, prefix.size () - 1);
                pfx_set->insert (prefix);
            }
        }
    }

    // the set of complete utf8 strings in the current character map
    typedef std::map<std::string, wchar_t>::iterator utf8_map_iter;

    if (0 == utf_map) {
        utf_map = new std::map<std::string, wchar_t>;
    
        const ucs4_cmap_iter first = charmap_.get_ucs4_cmap ().begin ();
        const ucs4_cmap_iter last  = charmap_.get_ucs4_cmap ().end ();

        for (ucs4_cmap_iter it = first; it != last; ++it) {
            const std::string utf = utf8_encode (it->second);
            utf_map->insert (std::make_pair (utf, it->second));
        }
    }

    codecvt_offset_tab_t* const offsets = new codecvt_offset_tab_t;

    // number of valid characters inserted into the tables
    std::size_t nchars = 0;

    std::string mb_char = charp + '\0';

    for (unsigned int i = 0; i <= UCHAR_MAX; ++i) {

        unsigned char cur_char = (unsigned char)i;

        mb_char [mb_char.size () - 1] = char (cur_char);

        const utf8_map_iter where = utf_map->find (mb_char);

        if (where == utf_map->end ()) {
            if (pfx_set->find (mb_char) == pfx_set->end ()) {
                offsets->off [cur_char] = UINT_MAX;
            }
            else {
                offsets->off [cur_char] = ++ntabs | 0x80000000;
                nchars += gen_utf8_tables (tab, off_map, mb_char, ntabs);
            }
        }
        else {
            // first get the symbolic name
            std::string str
                = charmap_.get_rucs4_cmap ().find (where->second)->second;

            // then get the internal encoding of the character
            const wchar_t int_enc = charmap_.get_w_cmap().find (str)->second;

            // then get the external encoding to use in a lookup in 
            // mb_char_off_map
            str = charmap_.get_rmb_cmap ().find (int_enc)->second;

            offsets->off [cur_char] = off_map.find (str)->second;

            ++nchars;
        }
    }

    tab.insert (std::make_pair (tabno, offsets));

    if (0 == ntabs) {
        // clean up
        delete pfx_set;
        delete utf_map;

        pfx_set = 0;
        utf_map = 0;
    }
    return nchars;
}


void Def::
gen_xlit_data ()
{
    // data offset points to the beginning of the data containing
    // the narrow strings character encodings
    unsigned int data_offset = 0;

    // traverse the map and construct the map of offsets
    xlit_map_t::const_iterator it = xlit_map_.begin ();
    for (; it != xlit_map_.end (); ++it) {
        // insert pair(wchar_t value, offset of first string in data block)
        xlit_data_offset_map_.insert (
            std::make_pair (it->first,data_offset));

        // advance the data_offset value to the next "first" string
        std::list<std::string>::const_iterator sit = 
            it->second.begin ();
        for (; sit != it->second.end (); ++sit) {
            data_offset += unsigned (sit->size ()) + 1;
        }
        ++data_offset;
    }

    // create a new table (first), populate it with default values
    // and insert it in the tables map
    xlit_offset_table_t table0;
    unsigned int k;
    for (k = 0; k < UCHAR_MAX + 1; ++k)
        table0.offset_table [k] = UINT_MAX;

    // insert it into the map
    xlit_table_map_.insert (std::make_pair(0, table0));

    const xlit_map_t::const_iterator xlit_map_end = xlit_map_.end ();

    // traverse the map again and build the tables
    for (it = xlit_map_.begin (); it != xlit_map_end; ++it) {

        // encode the wchar_t value to UTF-8
        const std::string utf8_rep (utf8_encode (it->first));
        data_offset = xlit_data_offset_map_.find (it->first)->second;

        // traverse the utf8 representation string and create the 
        // necessary tables and populate the indexes
        unsigned int table_idx = 0;

        const std::string::const_iterator utf8_rep_end = utf8_rep.end ();
        std::string::const_iterator       string_it    = utf8_rep.begin ();

        for (; string_it != utf8_rep_end; ++string_it) {
            // get the table corresponding to the current index and locate
            // the value at that index
            const xlit_table_map_t::iterator res =
                xlit_table_map_.find (table_idx);

            assert (res != xlit_table_map_.end ());

            // offset in table
            unsigned char off_idx = (unsigned char)*string_it;

            // res is the iterator pointing to the correct table in the map
            // check the index and if not populated, create a new table
            if (res->second.offset_table [off_idx] == UINT_MAX) {

                // if this is the last position in the string, then
                // fill the table position with the offset of the string data
                if ((string_it + 1) == utf8_rep.end ()) {
                    xlit_data_offset_map_t::const_iterator data_it = 
                        xlit_data_offset_map_.find (it->first);
                    assert (data_it != xlit_data_offset_map_.end ());

                    // fill the table position with the found offset
                    res->second.offset_table [off_idx] = data_it->second;
                    continue;
                }

                // create a new table and append it to the map
                xlit_offset_table_t table;
                for (unsigned int i = 0; i < UCHAR_MAX + 1; ++i)
                    table.offset_table [i] = UINT_MAX;

                // insert it into the map
                unsigned int tmp = unsigned (xlit_table_map_.size ());
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
    // if it has been already written
    if (codecvt_written_)
        return;

    // compose the directory name 
    ((dir_name += _RWSTD_PATH_SEP) += "..") += _RWSTD_PATH_SEP;
    dir_name += charmap_.get_code_set_name ();

    // check to see if the codecvt database already exists and
    // avoid recreating it if it does (as an optimization)
    if (std::ifstream (dir_name.c_str ())) {
        issue_diag (I_OPENWR, false, 0,
                    "%s exists, skipping\n", dir_name.c_str ());
        return;
    }

    //////////////////////////////////////////////////////////////////
    // generate multibyte conversion tables
    issue_diag (I_STAGE, false, 0, "generating multibyte tables\n");

    codecvt_offsets_map_t           mbchar_offs;
    std::map<std::string, unsigned> off_map;
    const std::size_t n_mbchars = gen_mbchar_tables (mbchar_offs, off_map);

    // generate wchar_t conversion tables
    issue_diag (I_STAGE, false, 0, "generating wchar_t tables\n");

    codecvt_offsets_map_t wchar_offs;
    const std::size_t n_wchars = gen_wchar_tables (wchar_offs);

    // generate UTF-8 conversion conversion tables
    issue_diag (I_STAGE, false, 0, "generating UTF-8 tables\n");

    codecvt_offsets_map_t uchar_offs;
    const std::size_t n_uchars = gen_utf8_tables (uchar_offs, off_map);

    // not needed beyond this point, clear it out
    off_map.clear ();
    
    // generate the transliteration tables and the transliteration data
    issue_diag (I_STAGE, false, 0, "generating transliteration tables\n");
    gen_xlit_data ();

    //////////////////////////////////////////////////////////////////
    // populate the codecvt structure before writing it out
    // in binary form to the file (the codecvt database)
    _RW::__rw_codecvt_t codecvt_out;
    std::memset (&codecvt_out, 0, sizeof codecvt_out);

#define UINT(x) _RWSTD_STATIC_CAST(unsigned, x)

    // calculate byte offsets within the structure
    codecvt_out.n_to_w_tab_off = 0;
    codecvt_out.w_to_n_tab_off = UINT (codecvt_out.n_to_w_tab_off
        + mbchar_offs.size () * (UCHAR_MAX + 1) * sizeof (unsigned));

    codecvt_out.utf8_to_ext_tab_off = UINT (codecvt_out.w_to_n_tab_off
        + wchar_offs.size () * (UCHAR_MAX + 1) * sizeof (unsigned));

    // insert the transliteration tables here
    codecvt_out.xliteration_off = UINT (codecvt_out.utf8_to_ext_tab_off
        + uchar_offs.size () * (UCHAR_MAX + 1) * sizeof (unsigned));

    codecvt_out.wchar_off = UINT (codecvt_out.xliteration_off
        + xlit_table_map_.size () * (UCHAR_MAX + 1) * sizeof (unsigned));

    codecvt_out.codeset_off = UINT (codecvt_out.wchar_off
        + charmap_.get_mb_cmap ().size () * 2 * sizeof (wchar_t));

    codecvt_out.charmap_off = UINT (codecvt_out.codeset_off
        + charmap_.get_code_set_name ().size () + 1 /* NUL */);
            
    const std::size_t mb_offset = UINT (codecvt_out.charmap_off
        + charmap_.get_charmap_name ().size () + 1 /* NUL */);

    // compute the size of narrow strings map which added to 
    // mb_offset will give the start of the transliteration data
    std::size_t xlit_data_offset = mb_offset;

    mb_cmap_iter iter;

    for (iter = charmap_.get_mb_cmap ().begin();
         iter != charmap_.get_mb_cmap().end(); ++iter) {
        xlit_data_offset += iter->first.size() + 1;
    }

    // now traverse again the utf8 tables for transliteration data
    // and recompute the offsets:
    const xlit_table_map_t::const_iterator xlit_table_map_end =
        xlit_table_map_.end ();

    xlit_table_map_t::iterator xit = xlit_table_map_.begin ();
    for (; xit != xlit_table_map_end; ++xit) {
        for (unsigned int i = 0; i < UCHAR_MAX + 1; ++i) {
            if (xit->second.offset_table [i] & 0x80000000)
                continue;
            // add the offset for xliteration data 
            xit->second.offset_table [i] += UINT (xlit_data_offset);
        }
    }

    codecvt_out.mb_cur_max = charmap_.get_mb_cur_max();

    issue_diag (I_OPENWR, false, 0, "writing %s\n", dir_name.c_str ());

    // create the stream with exceptions enabled
    std::ofstream out (dir_name.c_str(), std::ios::binary);
    out.exceptions (std::ios::failbit | std::ios::badbit);
            
    // write the codecvt_out structure
    out.write ((char*)&codecvt_out, sizeof codecvt_out);

    typedef codecvt_offsets_map_t::iterator off_iter_t;

    //////////////////////////////////////////////////////////////////
    // write out the multibyte to wchar_t tables
    issue_diag (I_WRITE, false, 0,
                "writing %lu multibyte tables (%lu characters)\n",
                mbchar_offs.size (), n_mbchars);

    for (off_iter_t it = mbchar_offs.begin (); it != mbchar_offs.end (); ++it) {
        for (unsigned i = 0; i <= UCHAR_MAX; ++i) {

            const unsigned off = it->second->off [i];

            out.write ((const char*)&off, sizeof off);
        }

        delete it->second;
    }

    // not needed beyond this point, clear it out
    mbchar_offs.clear ();

    //////////////////////////////////////////////////////////////////
    // write out the wchar_t to multibyte conversion tables
    issue_diag (I_WRITE, false, 0,
                "writing %lu wchar_t tables (%lu characters)\n",
                wchar_offs.size (), n_wchars);

    for (off_iter_t it = wchar_offs.begin (); it != wchar_offs.end (); ++it) {
        for (unsigned i = 0; i <= UCHAR_MAX; ++i) {

            // adjust offsets to multibyte characters (but not those
            // to other tables or invalid encodings)
            unsigned off = it->second->off [i];

            if (!(off & 0x80000000))
                off += UINT (mb_offset);

            out.write ((const char*)&off, sizeof off);
        }

        delete it->second;
    }
            
    // not needed beyond this point, clear it out
    wchar_offs.clear ();

    //////////////////////////////////////////////////////////////////
    // write out the UTF-8 to (libc) multibyte tables
    issue_diag (I_WRITE, false, 0,
                "writing %lu UTF-8 tables (%lu characters)\n",
                uchar_offs.size (), n_uchars);

    for (off_iter_t it = uchar_offs.begin (); it != uchar_offs.end (); ++it) {
        for (unsigned i = 0; i <= UCHAR_MAX; ++i) {

            // adjust offsets to multibyte characters (but not those
            // to other tables or invalid encodings)
            unsigned off = it->second->off [i];

            if (!(off & 0x80000000))
                off += UINT (mb_offset);

            out.write ((const char*)&off, sizeof off);
        }

        delete it->second;
    }

    // not needed beyond this point, clear it out
    uchar_offs.clear ();

    //////////////////////////////////////////////////////////////////
    // write out the transliteration UTF-8 lookup tables
    issue_diag (I_WRITE, false, 0,
                "writing transliteration table (size %lu)\n",
                xlit_table_map_.size ());

    xit = xlit_table_map_.begin ();
    for (; xit != xlit_table_map_end; ++xit) {
        const unsigned int* ptable = &xit->second.offset_table [0];
        for (unsigned int i = 0; i < UCHAR_MAX + 1; ++i, ++ptable) 
            out.write ((const char*)ptable, sizeof (unsigned int));
    }

    issue_diag (I_WRITE, false, 0,
                "writing the UCS table (%lu characters)\n",
                charmap_.get_mb_cmap ().size ());

    const mb_cmap_iter n_cmap2_end = charmap_.get_mb_cmap ().end ();

    // write the locale-encoded wchar_t and the UCS4 wchar_t
    for (iter = charmap_.get_mb_cmap ().begin();
         iter != n_cmap2_end; ++iter) {
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
    for (iter = charmap_.get_mb_cmap().begin();
         iter != n_cmap2_end; ++iter) {
        out.write (iter->first.c_str(), iter->first.size() + 1);
    }

    issue_diag (I_WRITE, false, 0,
                "writing transliteration data (size %lu)\n",
                xlit_map_.size ());

    // write out the transliteration data
    xlit_map_t::const_iterator xlit_data_it = xlit_map_.begin ();
    for (; xlit_data_it != xlit_map_.end (); ++xlit_data_it) {
        std::list<std::string>::const_iterator sit = 
            xlit_data_it->second.begin ();
        for (; sit != xlit_data_it->second.end (); ++sit) {
            out.write (sit->c_str (), sit->size () + 1);
        }
        out.write ("\0", 1);
    }
}
