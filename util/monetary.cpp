/***************************************************************************
 *
 * monetary.cpp
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
#include <locale>         // for money_base


static const char lc_name[] = "LC_MONETARY";


void Def::process_monetary()
{
    issue_diag (I_STAGE, false, 0, "processing %s section\n", lc_name);

    // nexting level
    int nesting_level = 0;

    mon_def_found_ = true;
    
    while ((next = scanner_.next_token ()).token != Scanner::tok_monetary) {
        
        switch (next.token) {

        case Scanner::tok_end:
            next = scanner_.next_token();
            if (next.token == Scanner::tok_monetary) {
                // end of monetary block
                if (nesting_level == 0) 
                    return;

                nesting_level--;
                scanner_.close ();
            }
            else
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
                   != Scanner::tok_monetary ){
                // the LC_IDENTIFICATION section may also have a 
                // LC_MONETARY token that will mess up the parsing
                if (next.token == Scanner::tok_ident) {
                    while ((next = scanner_.next_token()).token
                           != Scanner::tok_end );
                    next = scanner_.next_token();
                }
            }
            break;
        }

        case Scanner::tok_int_curr_symbol:            
            next = scanner_.next_token();
            mon_st_.int_curr_symbol = convert_string (next.name);
            mon_st_.wint_curr_symbol = convert_wstring (next);
            break;

        case Scanner::tok_currency_symbol:
            next = scanner_.next_token();
            mon_st_.currency_symbol = convert_string (next.name);
            mon_st_.wcurrency_symbol = convert_wstring (next);
            break;

        case Scanner::tok_mon_decimal_point:
            next = scanner_.next_token();
            mon_st_.mon_decimal_point = convert_string (next.name);
            mon_st_.wmon_decimal_point = convert_wstring (next);
            break;

        case Scanner::tok_mon_thousands_sep:
            next = scanner_.next_token();
            mon_st_.mon_thousands_sep = convert_string (next.name);
            mon_st_.wmon_thousands_sep = convert_wstring (next);
            break;

        case Scanner::tok_mon_grouping:
            mon_st_.mon_grouping.clear();
            while ((next = scanner_.next_token()).token 
                   != Scanner::tok_nl)
                mon_st_.mon_grouping += std::atoi(next.name.c_str());
            break;

        case Scanner::tok_positive_sign:
            next = scanner_.next_token();
            mon_st_.positive_sign = convert_string (next.name);
            mon_st_.wpositive_sign = convert_wstring (next);
            break;

        case Scanner::tok_negative_sign:
            next = scanner_.next_token();
            mon_st_.negative_sign = convert_string (next.name);
            mon_st_.wnegative_sign = convert_wstring (next);
            break;

        case Scanner::tok_int_frac_digits:
             mon_out_.frac_digits[1] =
                 std::atoi(scanner_.next_token().name.c_str());
                 //strtol(scanner_.next_token().name.c_str(), 0, 10);
            break;

        case Scanner::tok_frac_digits:
             mon_out_.frac_digits[0] =
                 std::atoi(scanner_.next_token().name.c_str());
            break;

        case Scanner::tok_p_cs_precedes:
            next = scanner_.next_token();
            if (next.name.size() == 1 && (next.name[0] == '0' 
                                          || next.name[0] == '1')) {
                mon_out_.p_cs_precedes[0] = std::atoi(next.name.c_str());
            }
            else
                 issue_diag (E_INVAL, true, &next, 
                             "%s is an invalid p_cs_precedes value\n",  
                             next.name.c_str());
            break;

        case Scanner::tok_p_sep_by_space:
            next = scanner_.next_token();
            if (next.name.size() == 1 && (next.name[0] >= '0' 
                                          && next.name[0] <= '2')) {
                mon_out_.p_sep_by_space[0] = std::atoi(next.name.c_str());
            }
            else
                 issue_diag (E_INVAL, true, &next, 
                             "%s is an invalid p_sep_by_space value\n",  
                             next.name.c_str());
            break;

        case Scanner::tok_n_cs_precedes:
            next = scanner_.next_token();
            if (next.name.size() == 1 && (next.name[0] == '0' 
                                          || next.name[0] == '1')) {
                mon_out_.n_cs_precedes[0] = std::atoi(next.name.c_str());
            }
            else
                 issue_diag (E_INVAL, true, &next, 
                             "%s is an invalid n_cs_precedes value\n",  
                             next.name.c_str());
            break;

        case Scanner::tok_n_sep_by_space:
            next = scanner_.next_token();
            if (next.name.size() == 1 && (next.name[0] >= '0' 
                                          && next.name[0] <= '2')) {
                mon_out_.n_sep_by_space[0] = std::atoi(next.name.c_str());
            }
            else
                 issue_diag (E_INVAL, true, &next, 
                             "%s is an invalid n_sep_by_space value\n",  
                             next.name.c_str());
            break;

        case Scanner::tok_p_sign_posn:
            next = scanner_.next_token();
            if (next.name.size() == 1 && (next.name[0] >= '0' 
                                          && next.name[0] <= '4')) {
                mon_out_.p_sign_posn[0] = std::atoi(next.name.c_str());
            }
            else
                 issue_diag (120, true, &next, 
                             "%s is an invalid p_sign_posn value\n",  
                             next.name.c_str());
            break;

        case Scanner::tok_n_sign_posn:
            next = scanner_.next_token();
            if (next.name.size() == 1 && (next.name[0] >= '0' 
                                          && next.name[0] <= '4')) {
                mon_out_.n_sign_posn[0] = std::atoi(next.name.c_str());
            }
            else
                 issue_diag (120, true, &next, 
                             "%s is an invalid n_sign_posn value\n",  
                             next.name.c_str());
            break;

        case Scanner::tok_int_p_cs_precedes:
            next = scanner_.next_token();
            if (next.name.size() == 1 && (next.name[0] == '0' 
                                          || next.name[0] == '1')) {
                mon_out_.p_cs_precedes[1] = std::atoi(next.name.c_str());
            }
            else {
                if (!(next.name.size() == 2 && (next.name[0] == '-'
                                                && next.name[1] == '1')))
                    issue_diag (E_INVAL, true, &next, 
                                "%s is an invalid int_p_cs_precedes value\n",  
                                next.name.c_str());
            }
            break;

        case Scanner::tok_int_p_sep_by_space:
            next = scanner_.next_token();
            if (next.name.size() == 1 && (next.name[0] >= '0' 
                                          && next.name[0] <= '2')) {
                mon_out_.p_sep_by_space[1] = std::atoi(next.name.c_str());
            }
            else {
                if (!(next.name.size() == 2 && (next.name[0] == '-'
                                                && next.name[1] == '1')))
                    issue_diag (E_INVAL, true, &next, 
                                "%s is a invalid int_p_sep_by_space value\n",  
                                next.name.c_str());
            }
            break;

        case Scanner::tok_int_n_cs_precedes:
            next = scanner_.next_token();
            if (next.name.size() == 1 && (next.name[0] == '0' 
                                          || next.name[0] == '1')) {
                mon_out_.n_cs_precedes[1] = std::atoi(next.name.c_str());
            }
            else {
                if (!(next.name.size() == 2 && (next.name[0] == '-'
                                                && next.name[1] == '1')))
                    issue_diag (E_INVAL, true, &next, 
                                "%s is an invalid int_n_cs_precedes value\n",  
                                next.name.c_str());
            }
            break;

        case Scanner::tok_int_n_sep_by_space:
            next = scanner_.next_token();
            if (next.name.size() == 1 && (next.name[0] >= '0' 
                                          && next.name[0] <= '2')) {
                mon_out_.n_sep_by_space[1] = std::atoi(next.name.c_str());
            }
            else {
                if (!(next.name.size() == 2 && (next.name[0] == '-'
                                                && next.name[1] == '1')))
                    issue_diag (124, true, &next, 
                                "%s is a invalid int_n_sep_by_space value\n",  
                                next.name.c_str());
            }
            break;

        case Scanner::tok_int_p_sign_posn:
            next = scanner_.next_token();
            if (next.name.size() == 1 && (next.name[0] >= '0' 
                                          && next.name[0] <= '4')) {
                mon_out_.p_sign_posn[1] = std::atoi(next.name.c_str());
            }
            else {
                if (!(next.name.size() == 2 && (next.name[0] == '-'
                                                && next.name[1] == '1')))
                    issue_diag (E_INVAL, true, &next, 
                                "%s is an invalid int_p_sign_posn value\n",  
                                next.name.c_str());
            }
            break;

        case Scanner::tok_int_n_sign_posn:
            next = scanner_.next_token();
            if (next.name.size() == 1 && (next.name[0] >= '0' 
                                          && next.name[0] <= '4')) {
                mon_out_.n_sign_posn[1] = std::atoi(next.name.c_str());
            }
            else {
                if (!(next.name.size() == 2 && (next.name[0] == '-'
                                                && next.name[1] == '1')))
                    issue_diag (E_INVAL, true, &next, 
                                "%s is an invalid int_n_sign_posn value\n",  
                                next.name.c_str());
            }
            break;

        default:
            break;

        }
    }
}


void Def::create_format (char format[], const char sign_posn, 
                         const char cs_precedes, const char sep_by_space,
                         bool is_positive) 
{
    switch (sign_posn) {
        // the international extension is not defined for this locale
    case -1:
        format[0] = -1;
        format[1] = -1;
        format[2] = -1;
        format[3] = -1;
        return;
    case 0:
        // if sign_posn is 0 then we change the sign to "()", if sign is 
        // not the empty string then issue a warning
        if (is_positive) {
            if (!mon_st_.positive_sign.empty())
                warnings_occurred_ = 
                    issue_diag (W_INVAL, false, 0,
                                "invalid combination of positive_sign "
                                "and p_sign_posn.  Ignoring positive_sign")
                    || warnings_occurred_;
            mon_st_.positive_sign = "()";
        }
        else {
            if (!mon_st_.negative_sign.empty())
                warnings_occurred_ = 
                    issue_diag (W_INVAL, false, 0,
                                "invalid combination of negative_sign "
                                "and n_sign_posn.  Ignoring negative_sign")
                    || warnings_occurred_;
            mon_st_.negative_sign = "()";        
        }

        // now construct the format
        format[0] = std::money_base::sign;
        if (cs_precedes == 0) {
            format[1] = std::money_base::value;
            format[2] = std::money_base::symbol;
        }
        else {
            format[1] = std::money_base::symbol;
            format[2] = std::money_base::value;
        }

        break;
    case 1:
        format[0] = std::money_base::sign;
        if (cs_precedes == 0) {
            format[1] = std::money_base::value;
            format[2] = std::money_base::symbol;
        }
        else {
            format[1] = std::money_base::symbol;
            format[2] = std::money_base::value;
        }
        break;
    case 2:
        if (cs_precedes == 0) {
            format[0] = std::money_base::value;
            format[1] = std::money_base::symbol;
        }
        else {
            format[0] = std::money_base::symbol;
            format[1] = std::money_base::value;
        }
        format[2] = std::money_base::sign;
        break;
    case 3:
        if (cs_precedes == 0) {
            format[0] = std::money_base::value;
            format[1] = std::money_base::sign;
            format[2] = std::money_base::symbol;
        }
        else {
            format[0] = std::money_base::sign;
            format[1] = std::money_base::symbol;
            format[2] = std::money_base::value;
        }
        break;
    case 4:
        if (cs_precedes == 0) {
            format[0] = std::money_base::value;
            format[1] = std::money_base::symbol;
            format[2] = std::money_base::sign;
        }
        else {
            format[0] = std::money_base::symbol;
            format[1] = std::money_base::sign;
            format[2] = std::money_base::value;
        }
        break;
    default:
        break;
    }

    // now put the space in the appropriate position
    if (sep_by_space == 0)
        format[3] = std::money_base::none;
    else if (sep_by_space == 1) {
        if (format[0] == std::money_base::value) {
            format[3] = format[2];
            format[2] = format[1];
            format[1] = std::money_base::space;
        }
        else if (format[2] == std::money_base::value) {
            format[3] = format[2];
            format[2] = std::money_base::space;
        }
        else if (format[0] == std::money_base::symbol) {
            format[3] = format[2];
            format[2] = format[1];
            format[1] = std::money_base::space;            
        }
        else {
            format[3] = format[2];
            format[2] = std::money_base::space;            
        }
    }
    else {
        if (format[0] == std::money_base::value) {
            format[3] = format[2];
            format[2] = std::money_base::space;
        }
        else if (format[2] == std::money_base::value) {
            format[3] = format[2];
            format[2] = format[1];
            format[1] = std::money_base::space;        
        }
        else if (format[0] == std::money_base::symbol) {
            format[3] = format[2];
            format[2] = std::money_base::space;            
        }
        else {
            format[3] = format[2];
            format[2] = format[1];
            format[1] = std::money_base::space; 
        }
    }
} 


void Def::write_monetary (std::string dir_name)
{
    assert (!dir_name.empty());

    if (mon_written_)
        return;

    if (!mon_def_found_) {
        issue_diag (I_SKIP, false, 0,
                    "%s section not found, skipping\n", lc_name);
        return;
    }

    // write out all the information in the LC_MONETARY category
    (dir_name += _RWSTD_PATH_SEP) += lc_name;

    issue_diag (I_OPENWR, false, 0, "writing %s\n", dir_name.c_str ());

    std::ofstream out (dir_name.c_str(), std::ios::binary);
    out.exceptions (std::ios::failbit | std::ios::badbit);

    // calculate the offsets for the mon_punct structure
    mon_punct_out_.decimal_point_off [1] = 0;

    mon_punct_out_.thousands_sep_off [1] = 
          mon_punct_out_.decimal_point_off [1] 
        + (mon_st_.wmon_decimal_point.size () + 1) * sizeof (wchar_t);

    mon_punct_out_.decimal_point_off [0] = 
          mon_punct_out_.thousands_sep_off [1]
        + (mon_st_.wmon_thousands_sep.size () + 1) * sizeof (wchar_t);

    mon_punct_out_.thousands_sep_off [0] = 
          mon_punct_out_.decimal_point_off [0]
        + mon_st_.mon_decimal_point.size () + 1;

    mon_punct_out_.grouping_off =
          mon_punct_out_.thousands_sep_off [0]
        + mon_st_.mon_thousands_sep.size () + 1;

    mon_punct_out_.punct_ext_off =
          mon_punct_out_.grouping_off
        + mon_st_.mon_grouping.size () + 1;

    // compute the alignment requirement of any offset member
    const std::size_t align = sizeof mon_punct_out_.punct_ext_off;

    // align the offset of the extension struct on the required boundary
    const std::size_t misalign = mon_punct_out_.punct_ext_off % align;

    // compute the amount of padding between the two structs
    const std::size_t pad = misalign ? align - misalign : 0;

    mon_punct_out_.punct_ext_off += pad;

    mon_out_.curr_symbol_off [1][1] = 0;

    mon_out_.curr_symbol_off [0][1] =
          mon_out_.curr_symbol_off [1][1]
        + (mon_st_.wint_curr_symbol.size () + 1) * sizeof (wchar_t);

    mon_out_.positive_sign_off [1] =
          mon_out_.curr_symbol_off [0][1]
        + (mon_st_.wcurrency_symbol.size () + 1) * sizeof (wchar_t);

    mon_out_.negative_sign_off [1] =
          mon_out_.positive_sign_off [1]
        + (mon_st_.wpositive_sign.size () + 1) * sizeof (wchar_t);


    // calculate all the narrow character string offsets
    mon_out_.curr_symbol_off [1][0] =
          mon_out_.negative_sign_off [1]
        + (mon_st_.wnegative_sign.size () + 1) * sizeof (wchar_t);

    mon_out_.curr_symbol_off [0][0] =
          mon_out_.curr_symbol_off [1][0]
        + mon_st_.int_curr_symbol.size () + 1;

    mon_out_.positive_sign_off [0] =
          mon_out_.curr_symbol_off [0][0]
        + mon_st_.currency_symbol.size() + 1;

    mon_out_.negative_sign_off [0] =
          mon_out_.positive_sign_off [0]
        + mon_st_.positive_sign.size () + 1;

    mon_out_.codeset_off =
          mon_out_.negative_sign_off [0]
        + mon_st_.negative_sign.size () + 1;

    mon_out_.charmap_off =
          mon_out_.codeset_off
        + charmap_.get_code_set_name ().size () + 1;

    issue_diag (I_WRITE, false, 0,
                "%s layout:\n"
                "__rw_punct_t {\n"
                "    decimal_point_off[] = { %u, %u }\n"
                "    thousand_sep_off[] = { %u, %u }\n"
                "    grouping_off = %u\n"
                "    ext_off = %u\n"
                "}\n__rw_moneypunct_t {\n"
                "    curr_symbol_off[][] = { { %u, %u }, { %u, %u } }\n"
                "    positive_sign_off[] = { %u, %u }\n"
                "    negative_sign_off[] = { %u, %u }\n"
                "    codeset_off = %u\n"
                "    charmap_off = %u\n"
                "}\n",
                lc_name,
                mon_punct_out_.decimal_point_off [0],
                mon_punct_out_.decimal_point_off [1],
                mon_punct_out_.thousands_sep_off [0],
                mon_punct_out_.thousands_sep_off [1],
                mon_punct_out_.grouping_off,
                mon_punct_out_.punct_ext_off,
                mon_out_.curr_symbol_off [0][0],
                mon_out_.curr_symbol_off [0][1],
                mon_out_.curr_symbol_off [1][0],
                mon_out_.curr_symbol_off [1][1],
                mon_out_.positive_sign_off [0],
                mon_out_.negative_sign_off [1],
                mon_out_.codeset_off,
                mon_out_.charmap_off);

    // construct the pos_format and neg_format
    create_format (mon_out_.pos_format[0], mon_out_.p_sign_posn[0],
                   mon_out_.p_cs_precedes[0], mon_out_.p_sep_by_space[0],
                   true);

    create_format (mon_out_.neg_format[0], mon_out_.n_sign_posn[0],
                   mon_out_.n_cs_precedes[0], mon_out_.n_sep_by_space[0],
                   false);

    create_format (mon_out_.pos_format[1], mon_out_.p_sign_posn[1],
                   mon_out_.p_cs_precedes[1], mon_out_.p_sep_by_space[1],
                   true);

    create_format (mon_out_.neg_format[1], mon_out_.n_sign_posn[1],
                   mon_out_.n_cs_precedes[1], mon_out_.n_sep_by_space[1],
                   false);
        
    // write out the mon_punct structure
    out.write ((char*)&mon_punct_out_, sizeof (mon_punct_out_));
        
    // write out the strings in the mon_punct_out_ structure
    out.write ((const char*)mon_st_.wmon_decimal_point.c_str(),
               (mon_st_.wmon_decimal_point.size() + 1) * sizeof(wchar_t));
    out.write ((const char*)mon_st_.wmon_thousands_sep.c_str(),
               (mon_st_.wmon_thousands_sep.size() + 1) * sizeof(wchar_t));
        
    out << mon_st_.mon_decimal_point << std::ends
        << mon_st_.mon_thousands_sep << std::ends
        << mon_st_.mon_grouping << std::ends;

    // pad the structure up to the next alignment boundary
    out.write ("\0\0\0\0\0\0\0\0", pad);

    // now write out the monetary offset extension struct
    out.write ((char*)&mon_out_, sizeof (mon_out_));

    // write the wide character strings
    out.write ((const char*)mon_st_.wint_curr_symbol.c_str(),
               (mon_st_.wint_curr_symbol.size() + 1) * sizeof(wchar_t));
    out.write ((const char*)mon_st_.wcurrency_symbol.c_str(),
               (mon_st_.wcurrency_symbol.size() + 1) * sizeof(wchar_t));
    out.write ((const char*)mon_st_.wpositive_sign.c_str(),
               (mon_st_.wpositive_sign.size() + 1) * sizeof(wchar_t));
    out.write ((const char*)mon_st_.wnegative_sign.c_str(),
               (mon_st_.wnegative_sign.size() + 1) * sizeof(wchar_t));

    // write the narrow character strings
    out << mon_st_.int_curr_symbol << std::ends
        << mon_st_.currency_symbol << std::ends
        << mon_st_.positive_sign << std::ends
        << mon_st_.negative_sign << std::ends
        << charmap_.get_code_set_name() << std::ends
        << charmap_.get_charmap_name() << std::ends;
}
