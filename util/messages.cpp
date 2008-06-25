/***************************************************************************
 *
 * messages.cpp
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
#include <fstream>        // for ofstream


static const char lc_name[] = "LC_MESSAGES";


void Def::process_messages()
{
    issue_diag (I_STAGE, false, 0, "processing %s section\n", lc_name);

    // nesting level
    int nesting_level = 0;

    messages_def_found_ = true;

    std::string name;

    while ((next = scanner_.next_token()).token != Scanner::tok_messages) {

        switch(next.token) {

        case Scanner::tok_end:
            next = scanner_.next_token();
            if (next.token == Scanner::tok_messages) {
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
                   != Scanner::tok_messages ){
                // the LC_IDENTIFICATION section may also have a 
                // LC_MESSAGES token that will mess up the parsing
                if (next.token == Scanner::tok_ident) {
                    while ((next = scanner_.next_token()).token
                           != Scanner::tok_end );
                    next = scanner_.next_token();
                }
            }
            break;
        }

        case Scanner::tok_yesexpr: {
            next = scanner_.next_token();
            messages_st_.yesexpr = convert_string (next.name);
            messages_st_.wyesexpr = convert_wstring (next);
            break;
        }

        case Scanner::tok_noexpr: {
            next = scanner_.next_token();
            messages_st_.noexpr = convert_string (next.name);
            messages_st_.wnoexpr = convert_wstring (next);
            break;
        }

        default:
            break;
        }
    }
}


void Def::write_messages(std::string dir_name)
{
    assert (!dir_name.empty());
    
    if (messages_written_)
        return;

    if (!messages_def_found_) {
        issue_diag (I_SKIP, false, 0,
                    "%s section not found, skipping\n", lc_name);
        return;
    }

    (dir_name += _RWSTD_PATH_SEP) += lc_name;

    issue_diag (I_OPENWR, false, 0, "writing %s\n", dir_name.c_str ());

    std::ofstream out (dir_name.c_str(), std::ios::binary);
    out.exceptions (std::ios::failbit | std::ios::badbit);

    // now calculate the offsets for the wide string representations
    messages_out_.yesexpr_off[1] = 0;
    messages_out_.noexpr_off[1] = unsigned (messages_out_.yesexpr_off[1] 
        + (messages_st_.wyesexpr.size() + 1) * sizeof (wchar_t));
        
    // now calculate the offsets for the narrow string representations
    messages_out_.yesexpr_off[0] = unsigned (messages_out_.noexpr_off[1] 
        + (messages_st_.wnoexpr.size() + 1) * sizeof (wchar_t));
    messages_out_.noexpr_off[0] = unsigned (messages_out_.yesexpr_off[0] 
        + (messages_st_.yesexpr.size() + 1) * sizeof (char));

    messages_out_.codeset_off = unsigned (messages_out_.noexpr_off[0] 
        + (messages_st_.noexpr.size() + 1) * sizeof (char));
    messages_out_.charmap_off = unsigned (messages_out_.codeset_off 
        + (charmap_.get_code_set_name().size() + 1) * sizeof (char));

    // first write out the messages structure
    out.write ((char*)&messages_out_, sizeof(messages_out_));

    // now write out all the strings
    out.write ((const char*)messages_st_.wyesexpr.c_str(), 
               (messages_st_.wyesexpr.size() + 1) * sizeof (wchar_t));
    out.write ((const char*)messages_st_.wnoexpr.c_str(), 
               (messages_st_.wnoexpr.size() + 1) * sizeof (wchar_t));

    out << messages_st_.yesexpr << std::ends;
    out << messages_st_.noexpr << std::ends;
    out << charmap_.get_code_set_name() << std::ends;
    out << charmap_.get_charmap_name() << std::ends;
    out.close();
}
