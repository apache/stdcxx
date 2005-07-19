/***************************************************************************
 *
 * scanner.h
 *
 * $Id: //stdlib/dev/source/stdlib/util/scanner.h#39 $
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

#ifndef RWSTD_UTIL_SCANNER_H_INCLUDED
#define RWSTD_UTIL_SCANNER_H_INCLUDED

#include <string>
#include <stack>


struct ScannerContext;


class Scanner
{
public:

    // enumeration of all tokens in the character map 
    // and locale definition file
    enum token_id {
        tok_charmap, tok_code_set_name, tok_mb_cur_max,
        tok_mb_cur_min, tok_ctype, tok_upper, tok_lower, 
        tok_digit, tok_space, tok_alpha, tok_graph, tok_print,
        tok_cntrl, tok_punct, tok_xdigit, tok_blank, tok_tolower,
        tok_toupper, tok_end, tok_collate, tok_monetary,
        tok_numeric, tok_time, tok_messages, tok_sym_name, 
        tok_script,
        tok_collating_element, tok_from, tok_collating_symbol, 
        tok_translit_start, tok_translit_end,
        tok_reorder, tok_reorder_end,
        tok_reorder_section, tok_reorder_section_end,
        tok_order_start, tok_order_end, tok_forward, tok_backward,
        tok_position, tok_undefined, tok_string, tok_ignore,
        tok_int_curr_symbol, tok_currency_symbol, 
        tok_mon_decimal_point, tok_mon_thousands_sep, 
        tok_mon_grouping, tok_positive_sign, tok_negative_sign,
        tok_int_frac_digits, tok_frac_digits, tok_p_cs_precedes,
        tok_p_sep_by_space, tok_n_cs_precedes, tok_n_sep_by_space,
        tok_p_sign_posn, tok_n_sign_posn,
        tok_int_p_cs_precedes, tok_int_n_cs_precedes,
        tok_int_p_sep_by_space, tok_int_n_sep_by_space,
        tok_int_p_sign_posn, tok_int_n_sign_posn,
        tok_decimal_point, tok_thousands_sep, tok_grouping,
        tok_abday, tok_day, tok_abmon, tok_mon, tok_d_t_fmt,
        tok_d_fmt, tok_t_fmt, tok_am_pm, tok_t_fmt_ampm, tok_era,
        tok_era_d_fmt, tok_era_t_fmt, tok_era_d_t_fmt, 
        tok_alt_digits, tok_ellipsis, tok_dellipsis, tok_qellipsis,
        tok_truename, tok_falsename,
        tok_yesexpr, tok_noexpr, tok_identification,
        tok_decimal_value, tok_hex_value, tok_octal_value,
        tok_comment, tok_comment_char, tok_escape_char, tok_pair,
        tok_copy, tok_include, tok_cont_line, tok_nl, tok_ndef, 
        tok_eof, tok_end_tokens, tok_doub_inc_ellipsis, tok_width
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
    // to a numeric value in the range [0, UCHAR_MAX]
    unsigned char convert_escape (const char*, const char** = 0) const;

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
