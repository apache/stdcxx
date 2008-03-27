/***************************************************************************
 *
 * scanner.h
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

#ifndef RWSTD_UTIL_SCANNER_H_INCLUDED
#define RWSTD_UTIL_SCANNER_H_INCLUDED

#include <string>
#include <stack>
#include <climits>   // for ULONG_MAX


struct ScannerContext;


class Scanner
{
public:

    // enumeration of all tokens in the character map 
    // and locale definition file
    enum token_id {
        tok_code_set_name,   // <code_set_name>
        tok_mb_cur_max,      // <mb_cur_max>
        tok_mb_cur_min,      // <mb_cur_min>
        // sections
        tok_charmap,         // beginning of CHARMAP section
        tok_collate,         // beginning of LC_COLLATE section
        tok_ctype,           // beginning of LC_CTYPE section
        tok_messages,        // beginning of LC_MESSAGES section
        tok_monetary,        // beginning of LC_MONETARY section
        tok_numeric,         // beginning of LC_NUMERIC section
        tok_time,            // beginning of LC_TIME section
        // ISO/IEC TR 14652 extensions:
        tok_addr,            // beginning of LC_ADDRESS section
        tok_ident,           // beginning of LC_IDENTIFICATION section
        tok_measure,         // beginning of LC_MEASUREMENT section
        tok_name,            // beginning of LC_NAME section
        tok_paper,           // beginning of LC_PAPER section
        tok_phone,           // beginning of LC_TELEPHONE section
        //
        tok_end,             // END of a section
        // LC_CTYPE-specific tokens
        tok_upper,           // upper section of LC_CTYPE
        tok_lower,           // lower section of LC_CTYPE
        tok_digit,           // digit section of LC_CTYPE
        tok_space,           // space section of LC_CTYPE
        tok_alpha,           // alpha section of LC_CTYPE
        tok_graph,           // graph section of LC_CTYPE
        tok_print,           // print section of LC_CTYPE
        tok_cntrl,           // cntrl section of LC_CTYPE
        tok_punct,           // punct section of LC_CTYPE
        tok_xdigit,          // xdigit section of LC_CTYPE
        tok_blank,           // blank section of LC_CTYPE
        tok_tolower,         // tolower section of LC_CTYPE
        tok_toupper,         // toupper section of LC_CTYPE
        // LC_COLLATE-specific tokens
        tok_script,
        tok_coll_elem,       // collating-element
        tok_coll_sym,        // collating symbol
        tok_from,
        tok_xlit_start,      // translit_start
        tok_xlit_end,        // translit_end
        tok_reorder,
        tok_reorder_end,
        tok_reorder_section,
        tok_reorder_section_end,
        tok_order_start,
        tok_order_end,
        tok_forward,
        tok_backward,
        tok_position,
        tok_undefined,
        // 
        tok_string,
        tok_ignore,
        // absolute, hexadecimal, decimal, and double-increment
        // ellipses (see ISO/IEC TR 14652)
        tok_abs_ellipsis,    // "..."
        tok_hex_ellipsis,    // ".."
        tok_dec_ellipsis,    // "...."
        tok_dbl_ellipsis,    // "..(N).."
        tok_width,
        // LC_MONETARY-specific tokens
        tok_int_curr_symbol,
        tok_currency_symbol, 
        tok_mon_decimal_point,
        tok_mon_thousands_sep, 
        tok_mon_grouping,
        tok_positive_sign,
        tok_negative_sign,
        tok_int_frac_digits,
        tok_frac_digits,
        tok_p_cs_precedes,
        tok_p_sep_by_space,
        tok_n_cs_precedes,
        tok_n_sep_by_space,
        tok_p_sign_posn,
        tok_n_sign_posn,
        tok_int_p_cs_precedes,
        tok_int_n_cs_precedes,
        tok_int_p_sep_by_space,
        tok_int_n_sep_by_space,
        tok_int_p_sign_posn,
        tok_int_n_sign_posn,
        // LC_NUMERIC-specific tokens
        tok_decimal_point,   // decimal point
        tok_thousands_sep,   // thousands_sep
        tok_grouping,        // grouping
        tok_truename,        // truename (C++ extension)
        tok_falsename,       // falsename (C++ extension)
        // LC_TIME-specific tokens
        tok_abday,
        tok_day,
        tok_abmon,
        tok_mon,
        tok_d_t_fmt,
        tok_d_fmt,
        tok_t_fmt,
        tok_am_pm,
        tok_t_fmt_ampm,
        tok_era,
        tok_era_d_fmt,
        tok_era_t_fmt,
        tok_era_d_t_fmt, 
        tok_alt_digits,
        // LC_MESSAGES-specific tokens
        tok_yesexpr,
        tok_noexpr,
        // LC_ADDRESS-specific tokens
        // LC_IDENTIFICATION-specific tokens
        // LC_MEASUREMENT-specific tokens
        // LC_NAME-specific tokens
        // LC_PAPER-specific tokens
        // LC_TELEPHONE-specific tokens
        // other:
        tok_sym_name,        // symbolic character name
        tok_char_value,      // character value (octal, decimal, or hex)
        tok_comment,         // comment
        tok_comment_char,    // <comment_char>
        tok_escape_char,     // <escape_char>
        tok_copy,            // copy directive
        tok_include,         // include directive
        tok_nl,              // newline
        tok_ndef,            // unknown/undefined token
        tok_end_tokens       // end of input
    };

    // scanner states
    // enum {valid, invalid};

    // a structure that represents a token
    struct token_t {
        std::string name;
        token_id    token;

        // file position 
        int  line;
        int  column;

        // file name pointer
        const char* file;
    };

    // realization
    Scanner ();
    virtual ~Scanner();

    // public interface
    token_t next_token ();

    void open (std::string, char = '#', char = '\\');

    void close ();

    char escape_char () const;

    void ignore_line ();

    // converts an octal, decimal, or hexadecimal escape sequence
    // (or a multibyte sequence of such things) to a numeric value
    unsigned long
    convert_escape (const char*, const char** = 0, bool = false) const;

private:

    Scanner (const Scanner&);          // not defined
    void operator= (const Scanner&);   // not defined

    // helper function that identifies a token from a string and 
    // returns a new token_t object
    token_id process_token (const char* name);

    // read a line from stream
    void read_line ();

    // current file context and stack of context objects
    ScannerContext*             context_;
    std::stack<ScannerContext*> context_stack_;

    unsigned nlines_;    // number of lines read
    unsigned ntokens_;   // number of tokens read

    // was the last token an escaped newline
    bool escaped_newline_;
};


#endif   // RWSTD_UTIL_SCANNER_H_INCLUDED
