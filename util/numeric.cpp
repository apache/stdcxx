/***************************************************************************
 *
 * numeric.cpp
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

#include "def.h"          // for Def
#include "diagnostic.h"   // for issue_diag()
#include "path.h"         // for get_pathname()
#include "scanner.h"      // for scanner

#include <cassert>        // for assert()
#include <cstdlib>        // for atoi()
#include <fstream>        // for ofstream


static const char lc_name[] = "LC_NUMERIC";


void Def::process_numeric ()
{
    issue_diag (I_STAGE, false, 0, "processing %s section\n", lc_name);

    // nesting level
    int nesting_level = 0;

    num_def_found_ = true;

    while ((next = scanner_.next_token()).token != Scanner::tok_numeric) {
        
        switch (next.token) {

        case Scanner::tok_end:
            next = scanner_.next_token();
            if (next.token == Scanner::tok_numeric) {
                // end of numeric block
                if (nesting_level == 0) 
                    return;

                nesting_level--;
                scanner_.close ();
            } else
                issue_diag (E_SYNTAX, true, &next,
                            "wrong section name in END directive\n");

            break;

        case Scanner::tok_copy: {
            next = scanner_.next_token();
            if (next.token != Scanner::tok_string)
                issue_diag (E_SYNTAX, true, &next,
                            "expected string following \"copy\" directive\n");

            // bump up the nesting level
            nesting_level++;

            issue_diag (I_STAGE, false, 0, "processing copy directive\n");

            // open the file
            scanner_.open (get_pathname (strip_quotes (next.name), next.file));

            // get comment char and escape char; 
            // these informations are stored by the scanner
            while ((next = scanner_.next_token ()).token 
                   != Scanner::tok_numeric ){
                // the LC_IDENTIFICATION section may also have a 
                // LC_NUMERIC token that will mess up the parsing
                if (next.token == Scanner::tok_ident) {
                    while ((next = scanner_.next_token()).token
                           != Scanner::tok_end );
                    next = scanner_.next_token();
                }
            }

            break;
        }

        case Scanner::tok_decimal_point:
            next = scanner_.next_token();
            num_st_.decimal_point = convert_string (next.name);
            num_st_.wdecimal_point = convert_wstring (next);
            break;

        case Scanner::tok_thousands_sep:
            next = scanner_.next_token();
            num_st_.thousands_sep = convert_string (next.name);
            num_st_.wthousands_sep = convert_wstring (next);
            break;

        case Scanner::tok_grouping:
            // convert the grouping digits from ascii to integers before
            // adding them to the grouping string
            num_st_.grouping.clear();
            while ((next = scanner_.next_token()).token 
                   != Scanner::tok_nl)
                num_st_.grouping += std::atoi(next.name.c_str());
            break;

        case Scanner::tok_truename:
            next = scanner_.next_token();
            num_st_.truename = convert_string (next.name);
            num_st_.wtruename = convert_wstring (next);
            break;

        case Scanner::tok_falsename:
            next = scanner_.next_token();
            num_st_.falsename = convert_string (next.name);
            num_st_.wfalsename = convert_wstring (next);
            break;

        default:
            break;
        }
    }
}


void Def::write_numeric (std::string dir_name)
{
    assert (!dir_name.empty ());

    if (num_written_)
        return;

    if (!num_def_found_) {
        issue_diag (I_SKIP, false, 0,
                    "%s section not found, skipping\n", lc_name);
        return;
    }

    (dir_name += _RWSTD_PATH_SEP) += lc_name;

    issue_diag (I_OPENWR, false, 0, "writing %s\n", dir_name.c_str ());

    std::ofstream out (dir_name.c_str (), std::ios::binary);
    out.exceptions (std::ios::failbit | std::ios::badbit);

    // calculate the offsets of members of the num_punct struct

    // start with wide strings to simplify dealing with their alignment
    num_punct_out_.decimal_point_off [1] = 0;

    num_punct_out_.thousands_sep_off [1] = 
        unsigned (num_punct_out_.decimal_point_off [1] 
        + (num_st_.wdecimal_point.size () + 1) * sizeof (wchar_t));

    num_punct_out_.decimal_point_off [0] = 
        unsigned (num_punct_out_.thousands_sep_off [1]
        + (num_st_.wthousands_sep.size() + 1) * sizeof (wchar_t));

    num_punct_out_.thousands_sep_off [0] = 
        unsigned (num_punct_out_.decimal_point_off [0] 
        + num_st_.decimal_point.size () + 1);

    num_punct_out_.grouping_off =
        unsigned (num_punct_out_.thousands_sep_off [0]
        + num_st_.thousands_sep.size () + 1);

    num_punct_out_.punct_ext_off =
        unsigned (num_punct_out_.grouping_off
        + num_st_.grouping.size () + 1);

    // compute the alignment requirement of any offset member
    const std::size_t align = sizeof num_punct_out_.punct_ext_off;

    // align the offset of the extension struct on the required boundary
    const std::size_t misalign = num_punct_out_.punct_ext_off % align;

    // compute the amount of padding between the two structs
    const std::size_t pad = misalign ? align - misalign : 0;

    num_punct_out_.punct_ext_off += unsigned (pad);

    // calculate the offsets of members of the numeric extension struct

    num_out_.truename_off [1] = 0;

    num_out_.falsename_off [1] =
        unsigned (num_out_.truename_off [1]
        + (num_st_.wtruename.size() + 1) * sizeof (wchar_t));

    num_out_.truename_off [0] =
        unsigned (num_out_.falsename_off [1]
        + (num_st_.wfalsename.size() + 1) * sizeof (wchar_t));

    num_out_.falsename_off [0] =
        unsigned (num_out_.truename_off [0]
        + num_st_.truename.size() + 1);

    num_out_.codeset_off =
        unsigned (num_out_.falsename_off [0]
        + num_st_.falsename.size() + 1);

    num_out_.charmap_off =
        unsigned (num_out_.codeset_off
        + charmap_.get_code_set_name().size() + 1);

    issue_diag (I_WRITE, false, 0,
                "%s layout:\n"
                "__rw_punct_t {\n"
                "    decimal_point_off[] = { %u, %u }\n"
                "    thousand_sep_off[] = { %u, %u }\n"
                "    grouping_off = %u\n"
                "    ext_off = %u\n"
                "}\n__rw_numpunct_t {\n"
                "    falsename_off[] = { %u, %u }\n"
                "    truename_off[] = { %u, %u }\n"
                "    codeset_off = %u\n"
                "    charmap_off = %u\n"
                "}\n",
                lc_name,
                num_punct_out_.decimal_point_off [0],
                num_punct_out_.decimal_point_off [1],
                num_punct_out_.thousands_sep_off [0],
                num_punct_out_.thousands_sep_off [1],
                num_punct_out_.grouping_off,
                num_punct_out_.punct_ext_off,
                num_out_.falsename_off [0],
                num_out_.falsename_off [1],
                num_out_.truename_off [0],
                num_out_.truename_off [1],
                num_out_.codeset_off,
                num_out_.charmap_off);
            
    // write the num_punct struct
    out.write ((char*)&num_punct_out_, sizeof (num_punct_out_));
        
    // write out the strings in the num_punct struct
    out.write ((const char*)num_st_.wdecimal_point.c_str (),
               (num_st_.wdecimal_point.size() + 1) * sizeof (wchar_t));
    out.write ((const char*)num_st_.wthousands_sep.c_str(),
               (num_st_.wthousands_sep.size() + 1) * sizeof(wchar_t));
        
    out << num_st_.decimal_point << std::ends
        << num_st_.thousands_sep << std::ends
        << num_st_.grouping << std::ends;

    // pad the structure up to the next alignment boundary
    out.write ("\0\0\0\0\0\0\0\0", pad);

    // write the numeric offset extension struct
    out.write ((char*)&num_out_, sizeof (num_out_));

    // write all the wide character strings
    out.write ((const char*)num_st_.wtruename.c_str(),
               (num_st_.wtruename.size() + 1) * sizeof (wchar_t));
    out.write ((const char*)num_st_.wfalsename.c_str(),
               (num_st_.wfalsename.size() + 1) * sizeof (wchar_t));
        

    // write the narrow strings and codeset name
    out << num_st_.truename << std::ends
        << num_st_.falsename << std::ends
        << charmap_.get_code_set_name() << std::ends
        << charmap_.get_charmap_name() << std::ends;

    out.close();
}
