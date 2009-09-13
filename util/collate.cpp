/***************************************************************************
 *
 * collate.cpp
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
 * Copyright 2001-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include "def.h"          // for Def
#include "diagnostic.h"   // for issue_diag()
#include "path.h"         // for get_pathname()
#include "scanner.h"      // for scanner

#include <fstream>        // for ofstream
#include <iostream>       // for cout
#include <string>         // for string
#include <vector>         // for vector

#include <cassert>        // for assert()
#include <cctype>         // for toupper()
#include <cstdio>         // for sprintf()
#include <cstdlib>        // for strtoul()
#include <cstring>        // for strchr()


void Def::
create_wchar_utf8_table ()
{
    if (!wchar_utf8_to_ext_.empty())
        return;

    typedef std::map<std::string, wchar_t>::const_iterator n_cmap_citer2;

    const n_cmap_citer2 n_cmap_end = charmap_.get_mb_cmap ().end ();

    for (n_cmap_citer2 n_cmap_it = charmap_.get_mb_cmap ().begin ();
         n_cmap_it != n_cmap_end; ++n_cmap_it) {

        const std::string wchar_utf8 = utf8_encode (n_cmap_it->second);

        wchar_utf8_to_ext_.insert (std::make_pair (wchar_utf8,
                                                   n_cmap_it->first));
    }
}


void Def::process_weights (collate_entry_t& entry)
{
    // iterator of weights tokens
    token_list_t::iterator w_it = entry.second.begin ();

    w_cmap_iter   wcmap_it;
    ce_map_iter   ce_map_it;
    coll_map_iter coll_map_it;

    Weights_t* weights = new Weights_t[collate_out_.num_weights];

    int i = 0;
    for (i = 0; i < collate_out_.num_weights && w_it != entry.second.end (); 
         ++i, ++w_it){
        get_weight (*w_it, weights, i);
    }

    const std::map<std::string, wchar_t>& w_cmap = charmap_.get_w_cmap ();

    // the remaining weights - see above - are given as weight the
    // collating value of the symbol at start of line
    while (i < collate_out_.num_weights) {
        weights[i].size = 1;
        wcmap_it = w_cmap.find (entry.first.name);
        if (wcmap_it != w_cmap.end()) {
            coll_map_it = coll_map_.find (wcmap_it->second);
            weights[i].weight[0] = coll_map_it->second.coll_val;
        }
        else if ((ce_map_it = ce_map_.find (entry.first.name)) != 
                 ce_map_.end()) {
           weights[i].weight[0] = ce_map_it->second.coll_val;
        }
        else {
            warnings_occurred_ = 
                issue_diag (W_COLSYM, false, &entry.first,
                            "symbolic name %s not defined as character or "
                            "collating-element, ignoring\n", 
                            entry.first.name.c_str()) || warnings_occurred_;
        }
        i++;
    }

    wcmap_it = w_cmap.find (entry.first.name);
    if (wcmap_it != w_cmap.end()) {
        coll_map_it = coll_map_.find (wcmap_it->second);
        coll_map_it->second.weights = weights;
    } else if ((ce_map_it = ce_map_.find (entry.first.name)) != ce_map_.end()) {
        ce_map_it->second.weights = weights;
    } else {
        warnings_occurred_ =  
            issue_diag (W_COLSYM, false, &entry.first,
                        "symbolic name %s not defined as character or "
                        "collating-element, ignoring\n", 
                        entry.first.name.c_str()) || warnings_occurred_;
    }
}


unsigned int Def::process_order_stmt (collate_section_t& section)
{
    // number of orders in the section
    collate_out_.num_weights = unsigned (section.order.size ());
    if (collate_out_.num_weights == 0) {
        collate_out_.num_weights = 1;
        collate_out_.weight_type[0] = 0;
    } else {
        collate_out_.num_weights = 0;
        token_list_t::iterator ord_it = section.order.begin ();
        for (; ord_it != section.order.end (); 
             ++ord_it, ++collate_out_.num_weights) {
            if (ord_it->token == Scanner::tok_forward) {
                collate_out_.weight_type[collate_out_.num_weights] = 0;
            } else if (ord_it->token == Scanner::tok_backward) {
                collate_out_.weight_type[collate_out_.num_weights] = 1;
            } else if (ord_it->token == Scanner::tok_position) {
                // this is equivalent to "forward,position"
                if (no_position_)
                    collate_out_.weight_type[collate_out_.num_weights] = 0;
                else
                    collate_out_.weight_type[collate_out_.num_weights] = 2;
            } else {
                std::string::size_type comma_pos = ord_it->name.find (',');
                if (comma_pos != std::string::npos) {
                    std::string first  = ord_it->name.substr (0, comma_pos);
                    std::string second = ord_it->name.substr (comma_pos + 1);

                    // verify a valid weight ordering
                    if (   (first == "forward" && second != "position")
                        || (first == "backward" && second != "position")) {
                        issue_diag (E_COLORD, true,
                                    &*ord_it, "unrecognized collating order\n");
                        break;
                    } else if (first == "forward") {
                        if (no_position_)
                            collate_out_.weight_type
                                [collate_out_.num_weights] = 0;
                        else
                            collate_out_.weight_type
                                [collate_out_.num_weights] = 2;
                    }
                    else
                        collate_out_.weight_type[collate_out_.num_weights] = 3;
                }
                else {
                    issue_diag (E_COLORD, true,
                                &*ord_it, "unrecognized collating order\n");
                }
            }
        }
    }

    // build the order value;
    unsigned int order = 0;
    for (int i = 0; i < collate_out_.num_weights; ++i) {
        order <<= 2;
        order |= collate_out_.weight_type[i];
    }

    return order;
}


// decimally increment the symbolic name, turning something like
// <U1234> into <U1245>, or <jis234> to <jis235>
static std::string
dec_increment (const std::string &sym)
{
    const char *pdig = sym.c_str ();

    for (; !(std::isdigit (*pdig)); ++pdig);

    char *end;

    const unsigned long sym_val = std::strtoul (pdig, &end, 10);

    char numstr [64];
    std::sprintf (numstr, "%lu", sym_val + 1);

    std::string next_sym = sym.substr (0, pdig - sym.c_str ());
    next_sym.append (numstr);
    next_sym.append (end);

    return next_sym;
}


// hexadecimally increment the symbolic name
static std::string
hex_increment (const std::string& sym)
{
    const char *pdig = sym.c_str ();

    for (; !(std::isxdigit (*pdig)); ++pdig);

    char *end;

    const unsigned long sym_val = std::strtoul (pdig, &end, 16);

    char numstr [64];
    std::sprintf (numstr, "%lx", sym_val + 1);

    std::string next_sym = sym.substr (0, pdig - sym.c_str ());
    next_sym.append (numstr);
    next_sym.append (end);

    return next_sym;
}


// scanning ahead and adding all symbols to the collating symbols map
void Def::preprocess_collation_definitions()
{
    // start with collation values from 2
    unsigned int coll_value = 2;

    // previous_elm and next_elm are used for processing ellipsis.
    std::string prev_elm;
    std::string next_elm;

    token_list_t::iterator cs_it = sym_list_.begin ();
    for (; cs_it != sym_list_.end (); ++cs_it) {
        cs_map_iter csm_it = cs_map_.find (cs_it->name);
        if (csm_it != cs_map_.end ())
            csm_it->second = coll_value++;
    }

    static token_list_t::size_type max_orders = 0;

    std::list<collate_section_t>::iterator s_it = section_list_.begin ();

    if (s_it != section_list_.end()) {
        // get maximum weights count
        max_orders = s_it->order.size ();
    }

    for (; s_it != section_list_.end (); ++s_it) {
        if (max_orders != s_it->order.size ())
            issue_diag (E_COLNUM, true, &*s_it->order.begin (),
                        "number of collation orders "
                        "different for this section\n");
    }

    const std::map<std::string, wchar_t>& w_cmap = charmap_.get_w_cmap ();

    // preprocess sections' collation definitions
    for (s_it = section_list_.begin (); 
         s_it != section_list_.end (); ++s_it) {

        // process the order statement and get the weight
        unsigned int order = process_order_stmt (*s_it);

        std::list<collate_entry_t>::iterator e_it = 
            s_it->entries.begin ();

        for (; e_it != s_it->entries.end (); ++e_it) {
            switch (e_it->first.token) {
            case Scanner::tok_sym_name:
                // store the previous name
                prev_elm = e_it->first.name;

                // process each of the collation definitions
                process_collation_definition (false, *e_it, 
                                              coll_value++, order);

                break;

            case Scanner::tok_abs_ellipsis:   // "<FOO>...<BAR>"
            case Scanner::tok_hex_ellipsis:   // "<FOO>..<BAR>"
            case Scanner::tok_dec_ellipsis:   // "<FOO>....<BAR>"
            {

                if (++e_it != s_it->entries.end ())
                    next_elm = e_it->first.name;
                else {
                    issue_diag (E_SYNTAX, true, &e_it->first,
                                "unexpected end of collation section while"
                                " processing ellipsis during scan_ahead\n");
                }

                if (e_it->first.token == Scanner::tok_abs_ellipsis) {
                    wchar_t w_cmap_cur_val 
                        = w_cmap.find(prev_elm)->second;
                    wchar_t w_cmap_end_val
                        = w_cmap.find(next_elm)->second;

                    // the first value has already been 
                    // added so don't add it again
                    w_cmap_cur_val = charmap_.increment_wchar (w_cmap_cur_val);
                    while (w_cmap_cur_val != w_cmap_end_val) {
                        // process iteration
                        collate_info_t ci = {UINT_MAX, UINT_MAX, 0, 0};
                        ci.coll_val = coll_value++;
                        ci.order = order;

                        coll_map_.insert (std::make_pair (w_cmap_cur_val, ci));
                        w_cmap_cur_val = 
                            charmap_.increment_wchar (w_cmap_cur_val);
                    }

                    // add last element "next_elm" to array
                    collate_info_t ci = {UINT_MAX, UINT_MAX, 0, 0};
                    ci.coll_val = coll_value++;
                    ci.order = order;
                    coll_map_.insert (std::make_pair (w_cmap_cur_val, ci));

                }
                else {
                    // we are incrementing the symbolic names
                    std::string next_name = prev_elm;
                    do {
                        if (e_it->first.token == Scanner::tok_hex_ellipsis) 
                            next_name = hex_increment (next_name);
                        else 
                            next_name = dec_increment (next_name);

                        w_cmap_iter it = w_cmap.find (next_name);
                        if (it != w_cmap.end()) {
                            // process iteration
                            collate_info_t ci = {UINT_MAX, UINT_MAX, 0, 0};
                            ci.coll_val = coll_value++;
                            ci.order = order;
                            coll_map_.insert (
                                std::make_pair (it->second, ci));
                        }
                    } while (next_name != next_elm);
                }

                prev_elm = next_elm;
                break;
            }

            case Scanner::tok_dbl_ellipsis:
                    issue_diag (W_NOTSUP, true, &e_it->first,
                                "ellipsis not supported"
                                " processing ellipsis during scan_ahead\n");
            default:
                break;
            }
        }
    }
}


void Def::process_collation_definition ( bool do_weights,
                                         collate_entry_t& entry,
                                         unsigned int coll_value,
                                         unsigned int order)
{
    const std::map<std::string, wchar_t>& w_cmap = charmap_.get_w_cmap ();

    // iterators
    w_cmap_iter w_cmap_pos = 
        w_cmap.find(entry.first.name);
    ce_map_iter ce_map_pos = 
        ce_map_.find(entry.first.name);

    // look up the symbolic name in the wide character map
    if (w_cmap_pos != w_cmap.end()) {

        wchar_t wval = w_cmap_pos->second;
        coll_map_iter coll_map_pos = coll_map_.find (wval);

        coll_map_pos->second.coll_val = coll_value;
        coll_map_pos->second.order = order;

        // process the weights
        if (do_weights) {
            unsigned int offset = next_offset_++;
            coll_map_pos->second.offset = offset;
            off_mapr_.insert(std::make_pair(offset, entry.first.name));

            process_weights (entry); 
        }

    }
    else if (ce_map_pos != ce_map_.end()) {

        ce_map_pos->second.coll_val = coll_value;
        ce_map_pos->second.order    = order;
                
        // process the weights
        if (do_weights) {
            unsigned int offset = next_offset_++;
            ce_map_pos->second.offset = offset;
            off_mapr_.insert(std::make_pair(offset, entry.first.name));     

            process_weights (entry);
        }

    }
    else if (cs_map_.find(entry.first.name) != cs_map_.end()) {

        cs_map_.find(entry.first.name)->second = coll_value;

    }
    else {
        // the symbol is missing from all three maps; 
        // add it in cs map as "sym <-> collating_value" pair
        cs_map_.insert (std::make_pair(entry.first.name, coll_value));

        warnings_occurred_ =  
            issue_diag (W_COLSYM, false, &entry.first, 
                        "unknown symbol name %s found in "
                        "LC_COLLATE definition\n", 
                        entry.first.name.c_str()) || warnings_occurred_;
    }
}

void Def::process_order(collate_section_t& section, 
                        unsigned int& coll_value)
{
    std::string sym;

    // get the orders
    unsigned int section_order = process_order_stmt (section);

    // current element
    std::string curr_elm;

    // previous_elm and next_elm are used for processing ellipsis.
    std::string prev_elm;
    std::string next_elm;

    const std::map<std::string, wchar_t>& w_cmap = charmap_.get_w_cmap ();

    // iterate thru section's entries
    std::list<collate_entry_t>::iterator e_it = section.entries.begin ();
    for (; e_it != section.entries.end (); ++e_it) {
        if (e_it->first.token == Scanner::tok_sym_name) {
            prev_elm = e_it->first.name;
            process_collation_definition (true, *e_it, 
                                          coll_value++, section_order);

        }
        else if (   e_it->first.token == Scanner::tok_abs_ellipsis
                 || e_it->first.token == Scanner::tok_dec_ellipsis
                 || e_it->first.token == Scanner::tok_hex_ellipsis) {

            // process line with ellipsis
            Weights_t* weights_template = 
                new Weights_t[collate_out_.num_weights];
            std::vector<bool> ordinal_weights;

            token_list_t::iterator w_it = e_it->second.begin ();
            for (int i = 0; i < collate_out_.num_weights 
                     && w_it != e_it->second.end (); ++i, ++w_it) {
                ordinal_weights.push_back (
                    get_weight (*w_it,weights_template, i));
            }
            while (ordinal_weights.size() < collate_out_.num_weights)
                ordinal_weights.push_back (true);

            // next line
            if (++e_it != section.entries.end ())
                next_elm = e_it->first.name;
            else {
                issue_diag (E_SYNTAX, true, &e_it->first,
                            "unexpected end of collation section while"
                            " processing ellipsis\n");
            }

            if (e_it->first.token == Scanner::tok_abs_ellipsis) {
                wchar_t w_cmap_cur_val 
                    = w_cmap.find(prev_elm)->second;
                wchar_t w_cmap_end_val
                    = w_cmap.find(next_elm)->second;
            
                // the first value has already been added so don't add it again
                w_cmap_cur_val = charmap_.increment_wchar (w_cmap_cur_val);
            
                while (w_cmap_cur_val != w_cmap_end_val) {

                    add_to_coll (w_cmap_cur_val, weights_template,
                                 coll_value++, ordinal_weights, false);
   
                    w_cmap_cur_val = charmap_.increment_wchar (w_cmap_cur_val);
                }

                // add the end element to the collation array.
                add_to_coll (w_cmap_end_val, weights_template, coll_value++,
                             ordinal_weights, false);

            } else { 

                // we are incrementing the symbolic names
                std::string next_name = prev_elm;
                do {
                    if (e_it->first.token == Scanner::tok_hex_ellipsis) 
                        next_name = hex_increment (next_name);
                    else 
                        next_name = dec_increment (next_name);

                    w_cmap_iter it = w_cmap.find (next_name);
                    if (it != w_cmap.end()) {
                        add_to_coll (it->second, weights_template, 
                                     coll_value++, ordinal_weights, false);
                    }
                } while (next_name != next_elm);
            }

            // store current left-hand symbol
            next_elm = prev_elm;
        
            delete[] weights_template;

        } else if (e_it->first.token == Scanner::tok_undefined) {

            // add all characters that were not explicitely given a collation
            // value in increasing order based on their encoded values
            undefined_keyword_found_ = true;
            collate_out_.undefined_optimization = true;
            undef_char_info_.order = section_order;
            Weights_t* weights_template 
                = new Weights_t[collate_out_.num_weights];
            std::vector<bool> ordinal_weights;

            token_list_t::iterator w_it = e_it->second.begin ();
            for (int i = 0; i < collate_out_.num_weights 
                     && w_it != e_it->second.end(); ++i, ++w_it) {

                if (w_it->token == Scanner::tok_abs_ellipsis) 
                    collate_out_.undefined_optimization = false;

                ordinal_weights.push_back (
                    !get_weight (*w_it, weights_template, i));
            }

            while (ordinal_weights.size() < collate_out_.num_weights)
                ordinal_weights.push_back (false);

            add_missing_values (ordinal_weights, weights_template,
                                coll_value, false);

            delete[] weights_template;
        } 
    }
}

void Def::add_missing_values (const std::vector<bool> &ordinal_weights,
                              const Weights_t* weights_template,
                              unsigned int &coll_value, bool give_warning)
{
    const std::map<std::string, wchar_t>& w_cmap = charmap_.get_w_cmap ();

    // we want to print a warning message once if there are characters that 
    // were not given collation values.
    bool warning_issued = false;

    symnames_list_iter symnames_it;
    for (symnames_it = charmap_.get_symnames_list ().begin ();
         symnames_it != charmap_.get_symnames_list ().end ();
         ++symnames_it) {

        std::map<std::string, wchar_t>::const_iterator w_cmap_it;
        if ((w_cmap_it = w_cmap.find(*symnames_it)) != w_cmap.end()) {
            wchar_t wchar_val = (*w_cmap_it).second;

            coll_map_iter coll_map_it;
            if ((coll_map_it = coll_map_.find(wchar_val)) != coll_map_.end()) {
                if (coll_map_it->second.offset == UINT_MAX) {
                    if (give_warning && !warning_issued) {
                        warning_issued = true;
                        warnings_occurred_ = 
                            issue_diag (W_MISSING, false,
                                        0, "some characters in the codeset "
                                        "were not explicitly given a "
                                        "collation value\n")
                            || warnings_occurred_;
                    }
                    if (!collate_out_.undefined_optimization) {
                        add_to_coll (wchar_val, weights_template, 
                                     coll_value++, ordinal_weights, true);
                    }
                }
            }
        }
    }
    if (collate_out_.undefined_optimization){
        collate_out_.undefined_weight_idx = next_offset_++;
        undef_char_info_.offset = collate_out_.undefined_weight_idx;
        undef_char_info_.coll_val = coll_value++;
        // initialize the weight template with the undefined collation info
        Weights_t* weights = new Weights_t[collate_out_.num_weights];

        if (0 != weights_template) {
            for (int k = 0; k < collate_out_.num_weights; ++k) {
                weights[k].size = weights_template[k].size; 
                for (int j = 0; j < 256; ++j)
                    weights[k].weight[j] = weights_template[k].weight[j];
            }
        }


        std::vector<bool>::const_iterator it;
                
        // now go through the vector that tells us if a particular 
        // weight is based on the ordinal position of the element
        // and place the correct ordinal value in the weight vector
        // if it is.
        int i = 0;
        for (it = ordinal_weights.begin();
             it != ordinal_weights.end(); ++it, ++i)
        {
            // FIXME: this may not be correct
            if (!*it){
                weights[i].size = 1;
                weights[i].weight[0] = undef_char_info_.coll_val;
            }
        }
        undef_char_info_.weights = weights;
        
        
    }
}

void Def::add_to_coll (const wchar_t            w_cmap_cur_val, 
                       const Weights_t*         weights_template,
                       const unsigned int       coll_value,
                       const std::vector<bool>& ordinal_weights,
                       bool                     undefined_value)
{
    Weights_t* weights = new Weights_t[collate_out_.num_weights];

    if (0 != weights_template) {
        for (int k = 0; k < collate_out_.num_weights; ++k) {
            weights[k].size = weights_template[k].size; 
            for (int j = 0; j < 256; ++j)
                weights[k].weight[j] = weights_template[k].weight[j];
        }
    }

    // get the symbolic name from the value
    std::string name 
        = charmap_.get_rw_cmap().find (w_cmap_cur_val)->second;
    coll_map_.find (w_cmap_cur_val)->second.coll_val = coll_value;

    unsigned int offset;
    if (undefined_value && collate_out_.undefined_optimization)
        offset = next_offset_;
    else
        offset = next_offset_++;

    const std::map<std::string, wchar_t>& w_cmap = charmap_.get_w_cmap ();

    w_cmap_iter w_cmap_pos = w_cmap.find(name);
    coll_map_iter coll_it = coll_map_.find (w_cmap_pos->second);
    coll_it->second.offset = offset;

    off_mapr_.insert(std::make_pair(offset, name));

    std::vector<bool>::const_iterator it;
                
    // now go through the vector that tells us if a particular 
    // weight is based on the ordinal position of the element
    // and place the correct ordinal value in the weight vector
    // if it is.
    int i = 0;
    for (it = ordinal_weights.begin();
         it != ordinal_weights.end(); ++it, ++i)
    {
        if (*it){
            weights[i].size = 1;
            weights[i].weight[0] = coll_value;
        }
    }
    coll_it->second.weights = weights;
}

// if undefined optimization is on then only those characters that have 
// been defined should go into the valid set, otherwise all characters go
// into the set.
void Def::gen_valid_coll_mb_set ()
{
    if (!valid_coll_mb_set_.empty())
        return;
    
    for (coll_map_iter coll_it = coll_map_.begin(); 
         coll_it != coll_map_.end(); ++coll_it) {
        if (coll_it->second.offset != UINT_MAX 
            || !collate_out_.undefined_optimization){
            rmb_cmap_iter rn_cmap_it 
                = charmap_.get_rmb_cmap().find (coll_it->first);
            std::string valid = rn_cmap_it->second.substr 
                (0, rn_cmap_it->second.size() - 1);
            
            while (valid.size() > 0){
                valid_coll_mb_set_.insert (valid);
                valid = valid.substr(0, valid.size() - 1); 
            }
        }
    }
}


// In processing the collate section of the locale definition file
// we will filter out the characters that are not present in the 
// charmap file; these characters when encountered on the left-hand side
// of a weight definition for a symbolic name will be left in place
// and used as a mere collating symbol for the rest of the parsing - 
// i.e. its weights ignored and assigned the weight corresponding to
// its position in the collate section.
void Def::process_collate()
{
    issue_diag (I_STAGE, false, 0, "processing LC_COLLATE section\n");

    // update flags
    collate_def_found_ = true;
    // initialization
    init_coll_map ();

    // first preprocess the collate section
    preprocess_collate ();

    const std::map<std::string, wchar_t>& w_cmap = charmap_.get_w_cmap ();

    // process the collating symbols list
    token_list_t::iterator cs_it = cs_list_.begin ();
    for (;cs_it != cs_list_.end (); ++cs_it) {
        if (w_cmap.find (cs_it->name) != 
            w_cmap.end())
            issue_diag (E_SYNTAX, true, &*cs_it,
                        "collating-symbol %s found in character map\n",
                        cs_it->name.c_str ());
        else if (ce_map_.find (cs_it->name) != ce_map_.end())
            issue_diag (E_SYNTAX, true, &*cs_it,
                        "collating-symbol %s has already been defined as "
                        "a collating-element\n", cs_it->name.c_str ());

        cs_map_.insert (std::make_pair (cs_it->name, 0));
    }

    // process the collating elements list
    std::list<collate_elem_t>::iterator ce_it = 
        ce_list_.begin ();
    for (; ce_it != ce_list_.end (); ++ce_it) {
        if (w_cmap.find (ce_it->first.name) != 
            w_cmap.end()) {
            issue_diag (E_SYNTAX, true, &ce_it->first,
                        "collating element %s found in charmap\n", 
                        ce_it->first.name.c_str());
        }

        // collating element info to be stored
        ce_info_t ce_tmp;
        ce_tmp.offset   = UINT_MAX;
        ce_tmp.coll_val = UINT_MAX;
        ce_tmp.order    = 0;
        ce_tmp.weights  = 0;

        // array of symbolic names
        std::vector<std::string> ce_sym_array;

        // get the definition of this collating element
        token_list_t& ce_def_list = ce_it->second;
        token_list_t::iterator ce_def_it = ce_def_list.begin ();
        bool invalid = false;

        if (ce_def_it->token == Scanner::tok_string) {
            if ((ce_tmp.ce_wstr = convert_wstring (*ce_def_it)).empty())
                invalid = true;
        } else if (ce_def_it->token == Scanner::tok_sym_name) {
            for (; ce_def_it != ce_def_list.end (); ++ce_def_it) 
                ce_sym_array.push_back (ce_def_it->name);
            if ((ce_tmp.ce_wstr = convert_wstring (ce_sym_array)).empty())
                invalid = true;
        } else {
            issue_diag (E_SYNTAX, true, &*ce_def_it, 
                        "illegal collating-element expression: %s\n",
                        ce_def_it->name.c_str ());
        }
        if (invalid)
            issue_diag (W_SYNTAX, false, &*ce_def_it,
                        "character in collating element definition "
                        "not found in character map\n");

        if (!invalid) {
            ce_map_.insert (std::make_pair(ce_it->first.name, ce_tmp));

            std::string ce_str;
            std::string ce_utf8_wstr;
            for (unsigned int i = 0; i < ce_tmp.ce_wstr.size(); ++i){
                ce_str += convert_to_ext(ce_tmp.ce_wstr[i]);
                ce_utf8_wstr += utf8_encode (ce_tmp.ce_wstr[i]); 
            }
            ce_sym_map_.insert (std::make_pair(ce_str, ce_it->first.name));
            ce_wsym_map_.insert (std::make_pair (ce_utf8_wstr, 
                                                 ce_it->first.name));
        }
    }

    // preprocess the collation definitions and make known all
    // the symbolic names available
    preprocess_collation_definitions();

    // collation values 0 and 1 are reserved
    unsigned int coll_value = 2;

    // FIXME: assign values to the collating-elements AGAIN
    cs_it = sym_list_.begin ();
    for (; cs_it != sym_list_.end (); ++cs_it) {
        cs_map_iter csm_it = cs_map_.find (cs_it->name);
        if (csm_it != cs_map_.end ())
            csm_it->second = coll_value++;
    }

    // process the sections
    std::list<collate_section_t>::iterator sect_it = 
        section_list_.begin ();
    for (; sect_it != section_list_.end (); ++sect_it)
        process_order (*sect_it, coll_value);

    // check to make sure that all the elements in the codeset were added
    if (!undefined_keyword_found_) {
        std::vector<bool> ordinal_weights;
        for (int i = 0; i < collate_out_.num_weights; ++i) 
            ordinal_weights.push_back (false);
        collate_out_.undefined_optimization = true;
        add_missing_values (ordinal_weights, 0, coll_value, true);
    }

}


void Def::gen_n_to_w_coll_tables (const std::string &charp, 
                                  unsigned int tab_num)
{
    gen_valid_coll_mb_set();
    offset_tab_t tab;
    for (unsigned int k = 0; k <= UCHAR_MAX; ++k) {
        tab.off[k] = UINT_MAX;
    }

    tab.first_offset = -1;
    mb_cmap_iter n_cmap_it;

    for (unsigned int i = 0; i <= UCHAR_MAX; ++i){
        unsigned char cur_char = (unsigned char)i;
        std::string mb_char = charp;
        mb_char += (char)cur_char;
        if ((n_cmap_it = charmap_.get_mb_cmap().find (mb_char)) 
            != charmap_.get_mb_cmap().end()) {
            coll_map_iter coll_map_it = coll_map_.find (n_cmap_it->second);
            if (coll_map_it->second.offset != UINT_MAX 
                || !collate_out_.undefined_optimization) {
                tab.off[cur_char] = coll_map_it->second.offset;
                if (tab.first_offset == -1)
                    tab.first_offset = cur_char;
            }
        }
        else {
            if (valid_coll_mb_set_.find (mb_char) != valid_coll_mb_set_.end()){
                ++next_tab_num_;
                tab.off[cur_char] = next_tab_num_ | 0x80000000;
                gen_n_to_w_coll_tables (mb_char, next_tab_num_);             
                if (tab.first_offset == -1)
                    tab.first_offset = cur_char;

            }
            else {
                tab.off[cur_char] = UINT_MAX;
            }
        }
    }
    char_offs_.insert (std::make_pair (tab_num, tab));
}


void Def::gen_w_to_n_coll_tables (const std::string &charp, 
                                  unsigned int tab_num)
{
    // initialize a table used in ctype and collate
    create_wchar_utf8_table();
    gen_valid_coll_wchar_set();

    offset_tab_t tab;
    for (unsigned int k = 0; k <= UCHAR_MAX; ++k) 
        tab.off[k] = UINT_MAX;

    tab.first_offset = -1;
    wchar_utf8_iter wu_it;
    mb_cmap_iter n_cmap_it;

    for (unsigned int i = 0; i <= UCHAR_MAX; ++i) {
        unsigned char cur_char = (unsigned char)i;
        std::string   mb_char = (charp);

        mb_char += (char)cur_char;

        wu_it = wchar_utf8_to_ext_.find (mb_char);
        if (wu_it != wchar_utf8_to_ext_.end()) {
            n_cmap_it = charmap_.get_mb_cmap().find (wu_it->second);
            if (n_cmap_it != charmap_.get_mb_cmap().end ()) {
                
                coll_map_iter coll_map_it = coll_map_.find (n_cmap_it->second);
                if (coll_map_it->second.offset != UINT_MAX || 
                    !collate_out_.undefined_optimization) {            
                    tab.off[cur_char] = coll_map_it->second.offset;
                    if (tab.first_offset == -1)
                        tab.first_offset = cur_char;
                }
            }
        }
        else 
            if (valid_coll_wchar_set_.find (mb_char) != 
                valid_coll_wchar_set_.end()) {

                ++next_wchar_coll_tab_num_;
                tab.off[cur_char] = next_wchar_coll_tab_num_ | 0x80000000;
                gen_w_to_n_coll_tables (mb_char, 
                                        next_wchar_coll_tab_num_);
                if (tab.first_offset == -1)
                    tab.first_offset = cur_char;
            } 
            else
                tab.off[cur_char] = UINT_MAX;
    }
    
    w_to_n_coll_.insert (std::make_pair (tab_num, tab));
}


void Def::gen_n_ce_tables (const std::set<std::string>ce_elms,
                           unsigned int idx, unsigned int tab_num)
{
    if (ce_elms.size() > 0) {
        ce_offset_tab_t tab;
        for (unsigned int k = 0; k <= UCHAR_MAX; ++k) {
            tab.off[k] = UINT_MAX;
        }
        tab.first_offset = -1;
        tab.last_offset = 0;
        ce_sym_map_iter ce_sym_map_it;
        ce_map_iter ce_map_it;

        std::set<std::string>::const_iterator ce_elms_it;
        std::set<std::string> next_elms;

        for (unsigned int i = 0; i <= UCHAR_MAX; ++i) {
            next_elms.clear();
            for (ce_elms_it = ce_elms.begin(); ce_elms_it != ce_elms.end(); 
                 ce_elms_it ++) {

                if ((unsigned char)i == (unsigned char)((*ce_elms_it)[idx])) {
                    if ((*ce_elms_it).size() == idx + 1) {
                        if ((ce_sym_map_it = ce_sym_map_.find (*ce_elms_it))
                            != ce_sym_map_.end()) {
                            ce_map_it = ce_map_.find(ce_sym_map_it->second);
                            tab.off[i] = ce_map_it->second.offset;
                            if (tab.first_offset == -1)
                                tab.first_offset = i;
                            if ((unsigned int)tab.last_offset < i)
                                tab.last_offset = i;
                        }
                    }
                    else {
                    
                        next_elms.insert (*ce_elms_it);
                        if (tab.off[i] == UINT_MAX) {
                            ++ next_n_ce_tab_num_;
                            tab.off[i] = next_n_ce_tab_num_ | 0x80000000;
                        }
                        if (tab.first_offset == -1)
                            tab.first_offset = i;
                        if ((unsigned int)tab.last_offset < i)
                            tab.last_offset = i;
                    }
                }
            }
            // now recursively call gen_n_ce_tables if any collating
            // elements with this character were found;
            if (next_elms.size() != 0)
                gen_n_ce_tables (next_elms, idx + 1, next_n_ce_tab_num_);
        }
        n_ce_offs_.insert(std::make_pair(tab_num, tab));

    }
}


void Def::gen_w_ce_tables (const std::set<std::string>ce_elms,
                           unsigned int idx, unsigned int tab_num)
{
    if (ce_elms.size() > 0) {
        ce_offset_tab_t tab;
        for (unsigned int k = 0; k <= UCHAR_MAX; ++k) {
            tab.off[k] = UINT_MAX;
        }
        tab.first_offset = -1;
        tab.last_offset = 0;
        ce_sym_map_iter ce_sym_map_it;
        ce_map_iter ce_map_it;

        std::set<std::string>::const_iterator ce_elms_it;
        std::set<std::string> next_elms;

        for (unsigned int i = 0; i <= UCHAR_MAX; ++i) {
            next_elms.clear();
            for (ce_elms_it = ce_elms.begin(); ce_elms_it != ce_elms.end(); 
                 ce_elms_it ++) {

                if ((unsigned char)i == (unsigned char)(*ce_elms_it)[idx]) {
                    if ((*ce_elms_it).size() == idx + 1) {
                        if ((ce_sym_map_it = ce_wsym_map_.find (*ce_elms_it))
                            != ce_wsym_map_.end()) {
                            ce_map_it = ce_map_.find(ce_sym_map_it->second);
                            tab.off[i] = ce_map_it->second.offset;
                            if (tab.first_offset == -1)
                                tab.first_offset = i;
                            if ((unsigned int)tab.last_offset < i)
                                tab.last_offset = i;
                        }
                    }
                    else {
                        next_elms.insert (*ce_elms_it);
                        if (tab.off[i] == UINT_MAX) {
                            ++ next_w_ce_tab_num_;
                            tab.off[i] = next_w_ce_tab_num_ | 0x80000000;
                        }
                        if (tab.first_offset == -1)
                            tab.first_offset = i;
                        if ((unsigned int)tab.last_offset < i)
                            tab.last_offset = i;
                    }
                }
            }
            // now recursively call gen_w_ce_tables if any collating
            // elements with this character were found;
            if (next_elms.size() != 0)
                gen_w_ce_tables (next_elms, idx + 1, next_w_ce_tab_num_);
        }
        w_ce_offs_.insert(std::make_pair(tab_num, tab));
    }
}


void Def::dump_collate ()
{
    std::cout << "LC_COLLATE\n";

    const std::map<std::string, wchar_t>& w_cmap = charmap_.get_w_cmap ();

    token_list_t::iterator sl_it = sym_list_.begin ();
    for (; sl_it != sym_list_.end (); ++sl_it) {
        std::cout << sl_it->name << " % "; 
        cs_map_iter cs_it = cs_map_.find (sl_it->name);
        if (cs_it != cs_map_.end ()) {
            std::cout << cs_it->second << '\n';
            continue;
        }

        ce_map_iter ce_it = ce_map_.find (sl_it->name);
        if (ce_it != ce_map_.end ()) {
            std::cout << ce_it->second.coll_val << '\n';
            continue;
        }
        
        std::map<std::string, wchar_t >::const_iterator cw_it = 
            w_cmap.find (sl_it->name);
        if (cw_it != w_cmap.end ()) {
            coll_map_iter cm_it = 
                coll_map_.find (cw_it->second);
            if (cm_it != coll_map_.end ()) {
                std::cout << cm_it->second.coll_val << '\n';
                continue;
            }
        }
        std::cout << '\n';
    }

    std::cout << "\n\n";

    std::list<collate_section_t>::iterator sect_it = 
        section_list_.begin ();
    while (sect_it != section_list_.end ()) {

        // dump the orders
        std::cout << "order_start " << sect_it->name;
        token_list_t::iterator o_it = sect_it->order.begin ();
        for (; o_it != sect_it->order.end (); ++o_it)
            std::cout << ";" << o_it->name;
        std::cout << '\n';

        // for each entry in the entries list
        collate_entry_list_t::iterator e_it = sect_it->entries.begin ();
        for (; e_it != sect_it->entries.end (); ++e_it) {

            // dump the collation definition (weights included)
            std::cout << e_it->first.name << " ";
            if (!e_it->second.empty ()) {
                token_list_t::iterator w_it = e_it->second.begin ();
                std::cout << (w_it++)->name;
                for (; w_it != e_it->second.end (); ++w_it) 
                    std::cout << ";" << w_it->name;
            }

            // lookup the value associated with this collation symbol
            cs_map_iter cs_it = cs_map_.find (e_it->first.name);
            if (cs_it != cs_map_.end ()) {
                std::cout << " % " << cs_it->second << '\n';
                continue;
            }

            ce_map_iter ce_it = ce_map_.find (e_it->first.name);
            if (ce_it != ce_map_.end ()) {
                std::cout << " % " << ce_it->second.coll_val << '\n';
                continue;
            }
        
            std::map<std::string, wchar_t >::const_iterator cw_it = 
                w_cmap.find (e_it->first.name);
            if (cw_it != w_cmap.end ()) {
                coll_map_iter cm_it = 
                    coll_map_.find (cw_it->second);
                if (cm_it != coll_map_.end ()) {
                    std::cout << " % " << cm_it->second.coll_val << '\n';
                    continue;
                }

            }
            std::cout << '\n';
        }

        std::cout << "order_end\n";
        ++sect_it;
    }
    std::cout << "END LC_COLLATE\n";
}


void Def::write_collate (std::string dir_name)
{
    assert (!dir_name.empty());

    static const char lc_name[] = "LC_COLLATE";

    if (collate_written_)
        return;

    if (!collate_def_found_) {
        issue_diag (I_SKIP, false, 0,
                    "%s section not found, skipping\n", lc_name);
        return;
    }

    next_tab_num_ = 0;
    next_wchar_coll_tab_num_ = 0;
    std::set<std::string> ce_strs;
    ce_sym_map_iter it;
    ce_map_iter ce_mit;
    for (it = ce_sym_map_.begin (); it != ce_sym_map_.end (); ++it) {
        if ((ce_mit = ce_map_.find (it->second))->second.coll_val 
            != UINT_MAX) {
            // check to see of the largest_ce needs to be changed
            if (ce_mit->second.ce_wstr.size() + 1 
                > collate_out_.largest_ce)
                collate_out_.largest_ce =
                    unsigned (ce_mit->second.ce_wstr.size());
            
            ce_strs.insert (it->first);
        }
    }
    next_n_ce_tab_num_ = 0;
    gen_n_ce_tables(ce_strs, 0, 0);
    ce_strs.clear();
    for (it = ce_wsym_map_.begin (); it != ce_wsym_map_.end (); ++it) {
        if (ce_map_.find (it->second)->second.coll_val != UINT_MAX)
            ce_strs.insert (it->first);
    }
    next_w_ce_tab_num_ = 0;
    gen_w_ce_tables (ce_strs, 0, 0);

    gen_n_to_w_coll_tables ("", next_tab_num_);
    gen_w_to_n_coll_tables ("", next_wchar_coll_tab_num_);

    (dir_name += _RWSTD_PATH_SEP) += lc_name;

    issue_diag (I_OPENWR, false, 0, "writing %s\n", dir_name.c_str ());

    std::ofstream out (dir_name.c_str(), std::ios::binary);
    out.exceptions (std::ios::failbit | std::ios::badbit);

    // calculate the size of an individual weight element
    collate_out_.elm_size = collate_out_.num_weights 
        * collate_out_.longest_weight * unsigned (sizeof (unsigned int)) + 
        unsigned (sizeof (unsigned int));

    // the first section of the collate database is the collating
    // element information
    collate_out_.n_ce_tab_off = 0;
    collate_out_.w_ce_tab_off = collate_out_.n_ce_tab_off;
    n_ce_offs_iter n_ce_offs_it;
    for (n_ce_offs_it = n_ce_offs_.begin(); 
         n_ce_offs_it != n_ce_offs_.end (); ++n_ce_offs_it) {
        collate_out_.w_ce_tab_off += (n_ce_offs_it->second.last_offset 
                                      - n_ce_offs_it->second.first_offset + 1)
                                     * unsigned (sizeof (int));
    }

    // next comes the weight information
    collate_out_.weight_tab_off = collate_out_.w_ce_tab_off;
    w_ce_offs_iter w_ce_offs_it;
    for (w_ce_offs_it = w_ce_offs_.begin(); 
         w_ce_offs_it != w_ce_offs_.end(); ++w_ce_offs_it) {
        collate_out_.weight_tab_off += (w_ce_offs_it->second.last_offset 
                                        - w_ce_offs_it->second.first_offset + 1)
                                       * unsigned (sizeof (int));
    }

    coll_map_iter coll_map_pos;

    collate_out_.num_elms = unsigned (off_mapr_.size());
    if (collate_out_.undefined_optimization)
        ++collate_out_.num_elms;

    // now calculate the offset for the first narrow character table
    collate_out_.n_char_tab_off = collate_out_.weight_tab_off
        + collate_out_.num_elms * collate_out_.elm_size;

    // now calculate the offset fo the fist wide character table
    // but first we need the size of the narrow tables
    char_offs_iter char_offs_it;
    unsigned int char_offs_size = 0;
    for (char_offs_it = char_offs_.begin(); 
         char_offs_it != char_offs_.end(); ++char_offs_it) {
        char_offs_size += (UCHAR_MAX + 1 
                           - char_offs_it->second.first_offset)
            * unsigned (sizeof (unsigned int));
    }
        
    collate_out_.w_char_tab_off = collate_out_.n_char_tab_off
        + char_offs_size;

    // now calculate the offset for the narrow character offset table
    // but first we need the size of the wide tables
    w_to_n_coll_iter w_to_n_coll_it;
    unsigned int w_to_n_size = 0;
    for (w_to_n_coll_it = w_to_n_coll_.begin();
         w_to_n_coll_it != w_to_n_coll_.end(); ++w_to_n_coll_it) {
        w_to_n_size += (UCHAR_MAX + 1 
                        - w_to_n_coll_it->second.first_offset)
            * unsigned (sizeof (unsigned int));
    }

    collate_out_.n_char_off_tab_off = collate_out_.w_char_tab_off
        + w_to_n_size;

    // now calculate the offset for the wide character offset table
    collate_out_.w_char_off_tab_off = collate_out_.n_char_off_tab_off
        + unsigned (char_offs_.size() * sizeof (unsigned int));

    // calculate the offset for the narrow collating element offset table
    collate_out_.n_ce_off_tab_off = collate_out_.w_char_off_tab_off
        + unsigned (w_to_n_coll_.size() * sizeof (unsigned int));

    // calculate the offset for the wide collating element offset table
    collate_out_.w_ce_off_tab_off = collate_out_.n_ce_off_tab_off
        + unsigned (n_ce_offs_.size() * sizeof (unsigned int));

    // now calculate the offset of the first character information
    collate_out_.n_char_first_char_off = collate_out_.w_ce_off_tab_off
        + unsigned (w_ce_offs_.size() * sizeof (unsigned int));
        
    // now calculate the offset of the wide table first char info
    collate_out_.w_char_first_char_off = collate_out_.n_char_first_char_off
        + unsigned (char_offs_.size() * sizeof (unsigned char));
        
    // now calculate the offset of the narrow ce first character info
    collate_out_.n_ce_first_char_off = collate_out_.w_char_first_char_off
        + unsigned (w_to_n_coll_.size() * sizeof (unsigned char));

    // now calculate the offset of the wide ce first character info
    collate_out_.w_ce_first_char_off = collate_out_.n_ce_first_char_off
        + unsigned (n_ce_offs_.size() * sizeof (unsigned char));

    // now calculate the offset of the narrow ce last character info
    collate_out_.n_ce_last_char_off = collate_out_.w_ce_first_char_off
        + unsigned (w_ce_offs_.size() * sizeof (unsigned char));

    // now calculate the offset of the wide ce last character info
    collate_out_.w_ce_last_char_off = collate_out_.n_ce_last_char_off
        + unsigned (n_ce_offs_.size() * sizeof (unsigned char));

    // now calculate the offset of the codeset name
    collate_out_.codeset_off = collate_out_.w_ce_last_char_off
        + unsigned (w_ce_offs_.size() * sizeof (unsigned char));

    // finally calculate the offset of the charmap name
    collate_out_.charmap_off = collate_out_.codeset_off 
        + unsigned (charmap_.get_code_set_name().size()) + 1;
        


    // print out the collate struct
    out.write ((char*)&collate_out_, sizeof(collate_out_));

    unsigned int i;
    for (n_ce_offs_it = n_ce_offs_.begin(); 
         n_ce_offs_it != n_ce_offs_.end(); ++n_ce_offs_it) {
        for (i = (unsigned int)n_ce_offs_it->second.first_offset; 
             i <= (unsigned int)n_ce_offs_it->second.last_offset; ++i)
            out.write ((char*)&n_ce_offs_it->second.off[i], 
                       sizeof (n_ce_offs_it->second.off[i]));
    }

    for (w_ce_offs_it = w_ce_offs_.begin(); 
         w_ce_offs_it != w_ce_offs_.end(); ++w_ce_offs_it) {
        for (i = (unsigned int)w_ce_offs_it->second.first_offset; 
             i <= (unsigned int)w_ce_offs_it->second.last_offset; ++i)
            out.write ((char*)&w_ce_offs_it->second.off[i], 
                       sizeof (w_ce_offs_it->second.off[i]));
    }

        
    // now print out the weight array
    unsigned int maxw = UINT_MAX;
    Weights_t* weights;
    bool undefined_written = false;

    const std::map<std::string, wchar_t>& w_cmap = charmap_.get_w_cmap ();

    std::size_t off_idx = 0;
    off_mapr_iter current_off = off_mapr_.begin();
    for (; current_off != off_mapr_.end(); ++off_idx) {

        // check the current offset
        if (current_off->first != off_idx 
            && off_idx != undef_char_info_.offset)
            issue_diag (E_RANGE, true, 0, 
                        "current_off : %d, index : %d, giving up\n",
                        current_off->first, off_idx);

        if (collate_out_.undefined_optimization 
            && off_idx == undef_char_info_.offset) {
            undefined_written = true;
            weights = undef_char_info_.weights;
            out.write ((char*)&undef_char_info_.order, 
                       sizeof (undef_char_info_.order));
            for (int j = 0; j < collate_out_.num_weights; ++j) {
                for (int k = 0; k < weights[j].size; ++k) {
                    out.write ((char*)&weights[j].weight[k],
                               sizeof (weights[j].weight[k]));
                }
                for (int c = weights[j].size; 
                     c < collate_out_.longest_weight; ++c)
                    out.write ((char*)&maxw, sizeof (maxw)); 
            }
            continue;
        }

        w_cmap_iter w_cmap_pos = w_cmap.find 
            (current_off->second);
        if (w_cmap_pos != w_cmap.end()) {
            coll_map_pos = coll_map_.find (w_cmap_pos->second);
            out.write ((char*)&coll_map_pos->second.order, 
                       sizeof (coll_map_pos->second.order));
            weights = coll_map_pos->second.weights;
            for (int j = 0; j < collate_out_.num_weights; ++j) {
                for (int k = 0; k < weights[j].size; ++k) {
                    out.write ((char*)&weights[j].weight[k],
                               sizeof (weights[j].weight[k]));
                }
                for (int c = weights[j].size; 
                     c < collate_out_.longest_weight; ++c)
                    out.write ((char*)&maxw, sizeof (maxw)); 
            }
            ++current_off;
        }

        else {
            ce_map_iter ce_map_it = ce_map_.find (current_off->second);
            if (ce_map_it != ce_map_.end() 
                && ce_map_it->second.coll_val != UINT_MAX) {
                out.write ((char*)&ce_map_it->second.order, 
                           sizeof (ce_map_it->second.order));
                weights = ce_map_it->second.weights;
                for (int j = 0; j < collate_out_.num_weights; ++j) {
                    for (int k = 0; k < weights[j].size; ++k) {
                        out.write ((char*)&weights[j].weight[k],
                                   sizeof (weights[j].weight[k]));
                    }
                    for (int c = weights[j].size; 
                         c < collate_out_.longest_weight; ++c)
                        out.write ((char*)&maxw, sizeof (maxw)); 
                }

            }
            ++current_off;
        }
    }            

    // maske sure that we got to the undefined value
    if (!undefined_written && collate_out_.undefined_optimization) {
        if (off_idx != undef_char_info_.offset)
            issue_diag (E_RANGE, true, 0, 
                        "current_off : %d, index : %d, giving up\n",
                        undef_char_info_.offset, off_idx);

        weights = undef_char_info_.weights;
        out.write ((char*)&undef_char_info_.order, 
                   sizeof (undef_char_info_.order));
        for (int j = 0; j < collate_out_.num_weights; ++j) {
            for (int k = 0; k < weights[j].size; ++k) {
                out.write ((char*)&weights[j].weight[k],
                           sizeof (weights[j].weight[k]));
            }
            for (int c = weights[j].size; 
                 c < collate_out_.longest_weight; ++c)
                out.write ((char*)&maxw, sizeof (maxw)); 
        }
    }

    // now print out the narrow character tables
    for (char_offs_it = char_offs_.begin(); 
         char_offs_it != char_offs_.end(); ++char_offs_it) {
        for (unsigned int c = char_offs_it->second.first_offset; 
             c <= UCHAR_MAX; ++c) {
            out.write ((const char*)&char_offs_it->second.off[c],
                       sizeof (char_offs_it->second.off[c]));
        }
    }

    // now print out the wide character tables
    for (w_to_n_coll_it = w_to_n_coll_.begin(); 
         w_to_n_coll_it != w_to_n_coll_.end(); ++w_to_n_coll_it) {
        for (unsigned int c = w_to_n_coll_it->second.first_offset; 
             c <= UCHAR_MAX; ++c) {
            out.write ((const char*)&w_to_n_coll_it->second.off[c],
                       sizeof (w_to_n_coll_it->second.off[c]));
        }
    }
        
    // now print the narrow character table offsets
    unsigned int current_offset = 0;
    for (char_offs_it = char_offs_.begin();
         char_offs_it != char_offs_.end(); ++char_offs_it) {
        out.write ((const char*)&current_offset, sizeof (current_offset));
        current_offset += (UCHAR_MAX + 1 
                           - char_offs_it->second.first_offset);
    }

    // now print the wide character table offsets
    current_offset = 0;
    for (w_to_n_coll_it = w_to_n_coll_.begin();
         w_to_n_coll_it != w_to_n_coll_.end(); ++w_to_n_coll_it) {
        out.write ((const char*)&current_offset, sizeof (current_offset));
        current_offset += (UCHAR_MAX + 1 
                           - w_to_n_coll_it->second.first_offset);
    }

    // now print the narrow ce table offsets
    current_offset = 0;
    for (n_ce_offs_it = n_ce_offs_.begin();
         n_ce_offs_it != n_ce_offs_.end(); ++n_ce_offs_it) {
        out.write ((const char*)&current_offset, sizeof (current_offset));
        current_offset += (n_ce_offs_it->second.last_offset 
                           - n_ce_offs_it->second.first_offset + 1);
    }

    // now print the wide ce table offsets
    current_offset = 0;
    for (w_ce_offs_it = w_ce_offs_.begin();
         w_ce_offs_it != w_ce_offs_.end(); ++w_ce_offs_it) {
        out.write ((const char*)&current_offset, sizeof (current_offset));
        current_offset += (w_ce_offs_it->second.last_offset 
                           - w_ce_offs_it->second.first_offset + 1);
    }


    // now print out the narrow character tables starting character
    for (char_offs_it = char_offs_.begin();
         char_offs_it != char_offs_.end(); ++char_offs_it) {
        const char off = char ((char_offs_it->second).first_offset);
        out << off;
    }        
        
    // now print out the wide character tables starting character
    for (w_to_n_coll_it = w_to_n_coll_.begin();
         w_to_n_coll_it != w_to_n_coll_.end(); ++w_to_n_coll_it) {
        const char off = char ((w_to_n_coll_it->second).first_offset);
        out << off;
    }        
        
    // now print out the narrow ce tables starting character
    for (n_ce_offs_it = n_ce_offs_.begin();
         n_ce_offs_it != n_ce_offs_.end(); ++n_ce_offs_it) {
        const char off = char ((n_ce_offs_it->second).first_offset);
        out << off;
    }        

    // now print out the wide ce tables starting character
    for (w_ce_offs_it = w_ce_offs_.begin();
         w_ce_offs_it != w_ce_offs_.end(); ++w_ce_offs_it) {
        const char off = char ((w_ce_offs_it->second).first_offset);
        out << off;
    }        

    // now print out the narrow ce tables ending character
    for (n_ce_offs_it = n_ce_offs_.begin();
         n_ce_offs_it != n_ce_offs_.end(); ++n_ce_offs_it) {
        const char off = char ((n_ce_offs_it->second).last_offset);
        out << off;
    }        

    // now print out the wide ce tables ending character
    for (w_ce_offs_it = w_ce_offs_.begin();
         w_ce_offs_it != w_ce_offs_.end(); ++w_ce_offs_it) {
        const char off = char ((w_ce_offs_it->second).last_offset);
        out << off;
    }        
        
    // finally write the codeset and charmap names
    out << charmap_.get_code_set_name() << std::ends
        << charmap_.get_charmap_name() << std::ends;
}


void Def::init_coll_map() {

    rw_cmap_iter rw_cmap_pos;
    collate_info_t tmp = {UINT_MAX, UINT_MAX, 0, 0};

    for (rw_cmap_pos = charmap_.get_rw_cmap().begin(); 
         rw_cmap_pos != charmap_.get_rw_cmap().end();
         ++rw_cmap_pos) {
        coll_map_.insert (std::make_pair (rw_cmap_pos->first, tmp));
    }
}


// In the preprocess_collate member function the collate section is 
// preprocessed and the result of the preprocessing is stored in-memory
// as a number of lists; the content of these lists is then preprocessed 
// acoording to the reorder statements; the result is then passed
// on to process_collate.
void Def::preprocess_collate ()
{
    int nesting_level = 0;

    do {
        // fetch next token
        next = scanner_.next_token();
        
        switch (next.token) {
        case Scanner::tok_end:
            next = scanner_.next_token();
            if (next.token == Scanner::tok_collate) {
                // end of collation block
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
            ++nesting_level;

            issue_diag (I_STAGE, false, 0, "processing copy directive\n");

            // open the file
            scanner_.open (get_pathname (strip_quotes (next.name), next.file));

            // get comment char and escape char; 
            // these informations are stored by the scanner
            while ((next = scanner_.next_token ()).token 
                   != Scanner::tok_collate ){
                // the LC_IDENTIFICATION section may also have a 
                // LC_COLLATE token that will mess up the parsing
                if (next.token == Scanner::tok_ident) {
                    while ((next = scanner_.next_token()).token
                           != Scanner::tok_end );
                    next = scanner_.next_token();
                }
            }
            
            break;
        }

        // a collating element definition defines a symbolic name that
        // represents symbolically the congregation of two other symbolic
        // names which have to be present in the character map;
        // the form of the phrase is:
        // collating-element sym from ("string" | (sym sym+))
        case Scanner::tok_coll_elem: {
            next = scanner_.next_token();
            // we expect a symbolic name
            if (next.token != Scanner::tok_sym_name) 
                issue_diag (E_SYNTAX, true, &next,
                            "symbolic name expected following "
                            "collating-element\n");
            // symbolic name
            token_t sym (next);

            next = scanner_.next_token ();
            if (next.token != Scanner::tok_from)
                issue_diag (E_SYNTAX, true, &next,
                            "illegal collating-element expression\n");

            token_list_t ce_def_list;
            next = scanner_.next_token ();
            if (next.token == Scanner::tok_string) {
                ce_def_list.push_back (next);
                ce_list_.push_back (std::make_pair(sym,ce_def_list));
            } else if (next.token == Scanner::tok_sym_name) {
                do {
                    ce_def_list.push_back (next);
                    next = scanner_.next_token();
                } while (next.token != Scanner::tok_nl);
                ce_list_.push_back (std::make_pair(sym,ce_def_list));
            } else 
                issue_diag (E_SYNTAX, true, &next,
                            "illegal collating-element expression\n");
            break;
        }
        case Scanner::tok_coll_sym:
            next = scanner_.next_token();
            if (next.token != Scanner::tok_sym_name)
                issue_diag (E_SYNTAX, true, &next,
                            "symbolic name expected following "
                            "collating-symbol\n");
            cs_list_.push_back (next);
            break;
        case Scanner::tok_script:
            next = scanner_.next_token ();
            if (next.token != Scanner::tok_sym_name)
                issue_diag (W_SYNTAX, false, &next,
                            "expecting script name, got %s\n", 
                            next.name.c_str ());
            script_list_.push_back(next);
            break;
        case Scanner::tok_sym_name:
            sym_list_.push_back (next);
            break;
        case Scanner::tok_order_start:
            preprocess_order();
            break;

        case Scanner::tok_reorder:
            preprocess_reorder ();
            break;

        case Scanner::tok_reorder_section:
            preprocess_reorder_section ();
            break;

        default:
            break;
        }
    }
    while (Scanner::tok_end_tokens != next.token);
}


// The task of preprocess_order is to parse and model the content of the 
// order sections in the input files
void Def::preprocess_order ()
{
    // one order-start keyword has been encountered; 
    // push a collate_section down the list and use it
    while (next.token != Scanner::tok_order_end) {
        section_list_.push_back (collate_section_t());
        collate_section_t& section = section_list_.back ();

        next = scanner_.next_token ();
        
        // store the orders
        while (next.token != Scanner::tok_nl) {
            if (next.token == Scanner::tok_sym_name) {
                // symbolic name, has to be a section name
                section.name = next.name;
            } else {
                // might be a combination of order and position
                // do the same until a better way is found
                section.order.push_back (next);
            }

            // fetch next token
            next = scanner_.next_token ();
        }

        // unnamed sections
        if (section.name == "")
            section.name = "unnamed";

        issue_diag (I_STAGE, false, 0,
                    "processing %s order\n", section.name.c_str ());

        // store the collation statements
        while (!(next.token == Scanner::tok_order_end || 
                 next.token == Scanner::tok_order_start )) {

            next = scanner_.next_token ();
            switch (next.token) {
            case Scanner::tok_end:
                issue_diag (E_SYNTAX, true, &next,
                            "unexpected END directive while "
                            "parsing collate section\n");
            case  Scanner::tok_nl:
                continue;
            case Scanner::tok_comment:
                scanner_.ignore_line ();
                break;
            case Scanner::tok_undefined:
            case Scanner::tok_sym_name: 
            {
                token_t sym (next);
                section.entries.push_back (
                    std::make_pair(sym,token_list_t()));
                collate_entry_t& entry = section.entries.back ();
                
                next = scanner_.next_token ();
                while (next.token != Scanner::tok_nl) {
                    entry.second.push_back (next);
                    next = scanner_.next_token ();
                }
                break;
            }
                        default:
                                break;
            }
        }
    }
}


// Handles the reorder statements in the form:
//    reorder-after <sym>
//    <sym> <sym><sym>...
// OR
//    reorder-after <sym>
//    ..  <sym><sym>.....
// followed by reorder-end statement or another reorder-after
void Def::preprocess_reorder ()
{
    while (true) {
        // process one or more statements grouped 
        // under a reorder section
        
        // retrieve the symbol
        next = scanner_.next_token ();
        if (next.token != Scanner::tok_sym_name)
            issue_diag (E_SYNTAX, true, &next,
                       "expecting symbolic name, got %s\n",
                       next.name.c_str ());
            
        // store the symbolic name
        token_t sym (next);
        collate_entry_list_t entries_list;
        while (true) {

            // process the statements
            next = scanner_.next_token ();

            if (next.token == Scanner::tok_nl )
                continue;
            else if (   next.token == Scanner::tok_sym_name
                     || next.token == Scanner::tok_hex_ellipsis) {
                // the line will contain one single symbolic name 
                // or a complete collation statement
                collate_entry_t entry;
                entry.first = next;

                next = scanner_.next_token ();
                while (next.token != Scanner::tok_nl) { 
                    entry.second.push_back (next);
                    next = scanner_.next_token ();
                }

                // add the entry to the list
                entries_list.push_back (entry);
            } else if (next.token == Scanner::tok_reorder) {
                // call insert_entry
                if (!entries_list.empty ())
                    insert_entries (sym, entries_list);
                entries_list.clear ();
                break;
            } else if (next.token == Scanner::tok_reorder_end ) {
                // call insert_entry
                // call insert_entry
                if (!entries_list.empty ())
                    insert_entries (sym, entries_list);
                return;
            } else {
                issue_diag (E_SYNTAX, true, &next,
                            "unexpected token : %s in a reorder block\n",
                            next.name.c_str ());
            }
        }
    }
}


void Def::preprocess_reorder_section ()
{
    while (next.token != Scanner::tok_reorder_section_end) {
        // process one or more statements grouped 
        // under a reorder section
        
        // retrieve the symbol
        next = scanner_.next_token ();
        token_t sym (next);

        // process the statements
        while (!(next.token == Scanner::tok_reorder_section || 
                 next.token == Scanner::tok_reorder_section_end)) {

            next = scanner_.next_token ();
            switch (next.token) {
            case Scanner::tok_nl:
                continue;
            case Scanner::tok_sym_name: {
                // the line will contain one single symbolic name 
                // which is the name of a section
                next = scanner_.next_token ();
                if (next.token != Scanner::tok_sym_name) {
                    issue_diag (E_SYNTAX, true, &next,
                                "expecting section name, got %s\n",
                                next.name.c_str ());
                }

                std::list<collate_section_t>::iterator beg = 
                    section_list_.begin ();
                std::list<collate_section_t>::iterator end = 
                    section_list_.end ();
                std::list<collate_section_t>::iterator ref_it, mov_it;
                for (ref_it = beg; 
                     ref_it->name != sym.name && ref_it != end; 
                     ++ref_it);
                for (mov_it = beg; 
                     mov_it->name != next.name && mov_it != end; 
                     ++mov_it);

                if (ref_it == end || mov_it == end)
                    issue_diag (E_SYNTAX, true, &next,
                                "missing sections %s %s requested\n",
                                sym.name.c_str (), next.name.c_str ());

                // replace it
                section_list_.splice (++ref_it, section_list_,
                                      mov_it, mov_it);

                // replace the insertion point
                sym = next;
                break;
            }
            default:
                issue_diag (E_SYNTAX, true, &next,
                            "unexpected token : %s in a "
                            "reorder section block\n",
                            next.name.c_str ());
                break;
            }

        }
    }
}


void Def::list_collate ()
{
    // print the script list
    token_list_t::iterator script_it = script_list_.begin ();
    while (script_it != script_list_.end ()) {
        std::cout << "script  " << (script_it++)->name << '\n';
    }
    std::cout << '\n';

    // print the collating symbols' list
    token_list_t::iterator cs_it = cs_list_.begin ();
    while (cs_it != cs_list_.end ()) {
        std::cout << "collating-symbol     " << (cs_it++)->name << '\n';
    }
    std::cout << '\n';

    // print the collating elements list
    std::list<collate_elem_t>::iterator ce_it = ce_list_.begin ();
    while (ce_it != ce_list_.end ()) {
        std::cout << "collating-element " << ce_it->first.name
                  << " from ";
        token_list_t::iterator ce_def_it = ce_it->second.begin ();
        for (;ce_def_it != ce_it->second.end (); ce_def_it++) {
            std::cout << ce_def_it->name << ";";
        }
        std::cout << '\n';
    }    
    std::cout << '\n';

    // print out general/global symbols' list
    token_list_t::iterator sym_it = sym_list_.begin ();
    while (sym_it != sym_list_.end ()) {
        std::cout << (sym_it++)->name << '\n';
    }
    std::cout << '\n';

    // print out sections
    std::list<collate_section_t>::iterator sc_it = 
        section_list_.begin ();
    while (sc_it != section_list_.end ()) {
        // print prolog
        std::cout << "order_start ";
        if (sc_it->name != "unnamed")
            std::cout << sc_it->name << ";";
        
        token_list_t::iterator it = sc_it->order.begin ();
        while (it != sc_it->order.end ())
            std::cout << (it++)->name << ";";
        std::cout << '\n';

        std::list<collate_entry_t>::iterator e_it = 
            sc_it->entries.begin ();
        while (e_it != sc_it->entries.end ()) {
            std::cout << e_it->first.name << "   ";
            token_list_t::iterator w_it =  e_it->second.begin ();
            while (w_it != e_it->second.end ())
                std::cout << (w_it++)->name << ";";
            std::cout << '\n';
            ++e_it;
        }

        ++sc_it;
    }
    std::cout << '\n' << "order_end\n";
}


// FIXME - modify the algorithm to get log(N) complexity
// Inserts an entry inside the section information;
// searching for the appropriate entry in the list is
// done in linear time 
bool Def::insert_entries (token_t& s, collate_entry_list_t& e)
{
    // first remove these entries if found
    collate_entry_list_t::iterator r_it = e.begin ();
    for (; r_it != e.end (); ++r_it) 
        remove_entry (*r_it);

    // first search through the symbols list; if found check
    // the collate_entry object; it should not have weights
    token_list_t::iterator it = sym_list_.begin ();
    for (; it != sym_list_.end (); ++it) {
        if (it->name != s.name) 
            continue;

        // FIXME - all statements have to have no weights if they are
        // to be inserted after collation symbols outside section boundaries;
        // found it;  check one collate_entry_t object
        if (!e.begin ()->second.empty ()) {
            issue_diag (E_REORD, true, &s,
                        "requested reorder-after: reference "
                        "symbol %s was found outside section boundaries "
                        "and the objects to be reordered are collation "
                        "definitions\n", s.name.c_str ());
        }

        // insert the symbolic name there
        collate_entry_list_t::iterator e_it = e.begin ();
        for (; e_it != e.end (); ++e_it) 
            it = sym_list_.insert (++it, e_it->first);

        return true;
    }

    // if not successful then search in each section
    std::list<collate_section_t>::iterator sect_it = section_list_.begin ();
    while (sect_it != section_list_.end ()) {
        collate_entry_list_t::iterator e_it = sect_it->entries.begin ();
        for (; e_it != sect_it->entries.end (); ++e_it) {
            if (e_it->first.name != s.name)
                continue;

            // found it; insert entry
            sect_it->entries.insert (++e_it, e.begin (), e.end ());
            return true;
        }
        ++sect_it;
    }

    issue_diag (W_REORD, false, &s,
                "requested reorder-after: reference "
                "symbol %s was not found \n", s.name.c_str ());

    return false;
}


// FIXME - modify the algorithm to get log(N) complexity
void Def::remove_entry (collate_entry_t& e)
{
    // search in the sym_list_ and in the sections
    if (e.second.empty ()) {
        token_list_t::iterator it = sym_list_.begin ();
        for (; it != sym_list_.end (); ++it) {
            if (it->name != e.first.name) 
                continue;

            sym_list_.erase (it);
            return;
        }
    }

    // search in the sections
    std::list<collate_section_t>::iterator sect_it = section_list_.begin ();
    while (sect_it != section_list_.end ()) {
        std::list<collate_entry_t>::iterator e_it = sect_it->entries.begin ();
        for (; e_it != sect_it->entries.end (); ++e_it) {
            if (e_it->first.name != e.first.name)
                continue;

            sect_it->entries.erase (e_it);
            return;
        }
        ++sect_it;
    }
    return ;
}



bool Def::get_weight ( token_t&     w,
                       Weights_t*   weights, 
                       int          weight_num)
{
    const std::map<std::string, wchar_t>& w_cmap = charmap_.get_w_cmap ();

    bool ret = false;
    weights[weight_num].size = 1;
    ce_map_iter ce_map_it;
    if(w.token == Scanner::tok_sym_name) {
        w_cmap_iter w_cmap_pos = w_cmap.find (w.name);
        if (w_cmap_pos != w_cmap.end()){
            coll_map_iter coll_map_pos = coll_map_.find(w_cmap_pos->second);

            if (coll_map_pos->second.coll_val == UINT_MAX) 
                issue_diag (E_SYMUSED, true, 
                            &w, "symbolic name %s "
                            "used as weight before being assigned a "
                            "collation value\n", w.name.c_str());

            // store weight
            weights[weight_num].weight[0] = coll_map_pos->second.coll_val;
        }
        else if ((ce_map_it = ce_map_.find (w.name)) 
                 != ce_map_.end())
            weights[weight_num].weight[0] = ce_map_it->second.coll_val;
        else {
            cs_map_iter cs_it = cs_map_.find (w.name);
            if (cs_it == cs_map_.end ()) 
                issue_diag (E_SYNTAX, true, 
                            &w, "symbolic name %s not found\n", 
                            w.name.c_str());
              
            weights[weight_num].weight[0] = cs_it->second;
        }
    }
    else if (w.token == Scanner::tok_char_value) {
        // the weight is given in numerical form
        const char* next_val =
            std::strchr (w.name.c_str (), scanner_.escape_char ());

        assert (0 != next_val);

        const char* next_wt = std::strchr (w.name.c_str (), ';');

        while (weight_num < collate_out_.num_weights) {

            unsigned c;

            for (c = 0; *next_val && (!next_wt || next_val < next_wt); ++c) {

                const char* end = 0;

                weights [weight_num].weight [c] =
                    unsigned (scanner_.convert_escape (next_val, &end, true));

                assert (0 != end);

                next_val = end;
            }

            weights [weight_num++].size = c;

            if (next_wt)
                next_wt = std::strchr (next_val, ';');
                
        }

    }
    else if(w.token == Scanner::tok_ignore) {
        // use the special weight 0 for IGNORE weights
        weights[weight_num].size = 1;
        weights[weight_num].weight[0] = 0;
    }
    else if (w.token == Scanner::tok_string) {
        // the weights are given either in symbolic name form (e.g.,
        // "<symbolic-name>" or in the form of a quoted multibyte
        // character string (e.g., "\001\d010\x16\")
        const std::string tmp (w.name.substr (1, w.name.size() - 2));

        // keeps track of the length of the weight
        unsigned char k = 0;

        // iterate thru the string content and retrieve the symbols
        std::string::const_iterator it = tmp.begin ();

        const char escape = scanner_.escape_char ();

        while (it != tmp.end ()) {
            // weight in string form
            std::string wsym;

            // next comes a symbolic name
            if (*it == '<') {
                while (*it != '>') {
                    if (*it == escape) 
                        ++it;

                    wsym += *it++;
                }
                wsym += *it++;

                // wsym has the symbolic name, lookup for it in 
                // the character map, collating-symbol map, 
                // and collating-element map
                w_cmap_iter w_cm_pos = w_cmap.find(wsym);
                cs_map_iter cs_it = cs_map_.find (wsym);
                ce_map_iter ce_it = ce_map_.find (wsym);

                if (w_cm_pos != w_cmap.end()) {
                    // is in the character map, check its associated
                    // collation value
                    coll_map_iter coll_it = coll_map_.find(w_cm_pos->second);
                    if (coll_it == coll_map_.end())
                        issue_diag (E_SYMUSED, true, &w, 
                                    "weight %s not defined\n", 
                                    wsym.c_str());
                    if (coll_it->second.coll_val == UINT_MAX) 
                        issue_diag (E_SYMUSED, true, 
                                    &w, "symbolic name %s "
                                    "used as weight before being assigned a "
                                    "collation value\n", wsym.c_str());

                    weights[weight_num].weight[k++] = coll_it->second.coll_val;
                }
                else if (ce_it != ce_map_.end()) {
                    // it is in the collating-element map
                    weights[weight_num].weight[k++] = ce_it->second.coll_val;
                }
                else if (cs_it != cs_map_.end()) {
                    // it is in the collating-symbol map
                    weights[weight_num].weight[k++] = cs_it->second;
                }
                else {
                    // it is not in any of the maps, that's an error
                    issue_diag (E_SYNTAX, true, &w, 
                                "symbolic name %s not found\n", 
                                wsym.c_str());
                }
            }
            else if (*it == escape) {

                // weight is given in a quoted escape form
                const char* const beg = tmp.c_str () + (it - tmp.begin ());
                const char*       end = 0;

                weights [weight_num].weight [k++] =
                    unsigned (scanner_.convert_escape (beg, &end, true));

                assert (0 != end);

                it += end - beg;

                break;
            }
            else 
                issue_diag (E_SYNTAX, true, &w,
                            "illegal string content as a weight");
        }

        weights[weight_num].size = k;
        if (k > collate_out_.longest_weight)
            collate_out_.longest_weight = k;

    }
    else if (w.token == Scanner::tok_abs_ellipsis)
        // return true if ellipsis are embedded in the weight
        ret = true;
    else
        warnings_occurred_ = 
            issue_diag (W_SYNTAX, false, &w,
                        "illegal token %s found in collation definition\n",
                        w.name.c_str()) || warnings_occurred_;

    return ret;
}


// if undefined optimization is on then only those characters that have 
// been defined should go into the valid set, otherwise all characters go
// into the set.
void Def::gen_valid_coll_wchar_set () {

    if (!valid_coll_wchar_set_.empty())
        return;

    create_wchar_utf8_table();

    for (coll_map_iter coll_it = coll_map_.begin(); 
         coll_it != coll_map_.end(); ++coll_it) {
        if (coll_it->second.offset != UINT_MAX 
            || !collate_out_.undefined_optimization){

            std::string valid = utf8_encode (coll_it->first);
            valid = valid.substr (0, valid.size() - 1);
            
            while (valid.size() > 0){
                valid_coll_wchar_set_.insert (valid);
                valid = valid.substr(0, valid.size() - 1); 
            }
        }
    }
}
