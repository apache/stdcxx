/***************************************************************************
 *
 * scanner.cpp
 *
 * $Id: //stdlib/dev/source/stdlib/util/scanner.cpp#59 $
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

#include "scanner.h"

#include "diagnostic.h"
#include "loc_exception.h"

#include <fstream>
#include <string>
#include <vector>

#include <cassert>   // for assert()
#include <climits>   // for UCHAR_MAX
#include <cstdlib>   // for strtol()
#include <cstring>   // for strcmp()


struct ScannerContext
{
    ScannerContext (const char*, char = '#', char = '\\');
    ScannerContext (const ScannerContext&);
 
    std::ifstream file;		// file stream object
    std::string   filename;	// filename

    // comment and escape for current file
    char comment_char;
    char escape_char;

    // current line and column for the scanner
    int line;

    // current line and position within it
    std::string line_;
    const char* pos_;

protected:
    void operator= (ScannerContext&);
};

/**************************************************************************/
// helpers

static void normal_path (std::string& s)
{
    std::string::iterator it(s.begin ());

    for (; it != s.end (); it++)
        if (*it == '/' || *it == '\\') {
#if defined (_MSC_VER)
            *it = '\\';
#else
            *it = '/';
#endif            
        }
}

/**************************************************************************/
// ScannerContext class definitions

ScannerContext::
ScannerContext (const char* name, char cc, char ec)
    : file (name),  filename (name),
      comment_char (cc), escape_char (ec),
    line (0) // , column (0)
{
    // update current position
    pos_ = line_.c_str ();

    if (!file.is_open ())
        issue_diag (500, true, 0, 
                    "%s could not be opened for reading\n", name);

    issue_diag (I_OPENRD, false, 0, "reading %s\n", name);
}


ScannerContext::
ScannerContext (const ScannerContext& scanner)
    : file (scanner.filename.c_str(), std::ios::binary), 
      filename (scanner.filename),
      comment_char (scanner.comment_char),
      escape_char (scanner.escape_char),
      line (scanner.line)
      // column (scanner.column)
{
}

/**************************************************************************/
// Scanner class definitions

Scanner::
Scanner ()
    : context_ (0), nlines_ (0), ntokens_ (0), escaped_newline_ (false)
{
}


Scanner::
~Scanner() 
{
    // empty the stack and destroy the current state
    delete context_;

    while (!context_stack_.empty ()) {
        delete context_stack_.top ();
        context_stack_.pop ();
    }
}


char Scanner::
escape_char () const
{
    return context_ ? context_->escape_char : 0;
}

void Scanner::
ignore_line ()
{
    while (next_token ().token != tok_nl);
}


void Scanner::
open (std::string name, char cc, char ec)
{
    normal_path (name);

    if (context_)
        context_stack_.push (context_);

    try {
        context_ = new ScannerContext (name.c_str (), cc, ec);
    }
    catch (loc_exception&) {
        context_ = 0;

        if (!context_stack_.empty ()) {
            context_ = context_stack_.top ();
            context_stack_.pop ();
        }

        throw;
    }

    nlines_ = 0;
    ntokens_ = 0;
}


void Scanner::
close ()
{
    assert (0 != context_);

    issue_diag (I_OPENRD, false, 0,
                "%s: %u tokens, %u lines\n",
                context_->filename.c_str (), ntokens_, nlines_);

    delete context_;

    if (context_stack_.empty ())
        context_ = 0;
    else {
        context_ = context_stack_.top ();
        context_stack_.pop ();
    }
}


Scanner::token_id Scanner::
process_token (const char* name)
{
    assert (0 != name);

    if (*name == context_->escape_char) {

        switch (name [1]) {

        case 'd':
            return tok_decimal_value;
        case 'x':
            return tok_hex_value;
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
            return tok_octal_value;
        default:
            break;
        }

        return tok_ndef;
    }

    // look for a predefined token

    static const struct {
        const char*       name;
        Scanner::token_id token;
    } tok_map [] = {
        { "CHARMAP", Scanner::tok_charmap },
        { "END", Scanner::tok_end },
        { "IGNORE", Scanner::tok_ignore },
        { "LC_COLLATE", Scanner::tok_collate },
        { "LC_CTYPE", Scanner::tok_ctype },
        { "LC_IDENTIFICATION", Scanner::tok_identification },
        { "LC_MESSAGES", Scanner::tok_messages },
        { "LC_MONETARY", Scanner::tok_monetary },
        { "LC_NUMERIC", Scanner::tok_numeric },
        { "LC_TIME", Scanner::tok_time },
        { "UNDEFINED", Scanner::tok_undefined },
        { "WIDTH", Scanner::tok_width },
        { "abday", Scanner::tok_abday },
        { "abmon", Scanner::tok_abmon },
        { "alpha", Scanner::tok_alpha },
        { "alt_digits", Scanner::tok_alt_digits },
        { "am_pm", Scanner::tok_am_pm },
        { "backward", Scanner::tok_backward },
        { "blank", Scanner::tok_blank },
        { "cntrl", Scanner::tok_cntrl },
        { "collating-element", Scanner::tok_collating_element },
        { "collating-symbol", Scanner::tok_collating_symbol },
        { "comment_char", Scanner::tok_comment_char },
        { "copy", Scanner::tok_copy },
        { "currency_symbol", Scanner::tok_currency_symbol },
        { "d_fmt", Scanner::tok_d_fmt },
        { "d_t_fmt", Scanner::tok_d_t_fmt },
        { "day", Scanner::tok_day },
        { "decimal_point", Scanner::tok_decimal_point },
        { "digit", Scanner::tok_digit },
        { "era", Scanner::tok_era },
        { "era_d_fmt", Scanner::tok_era_d_fmt },
        { "era_d_t_fmt", Scanner::tok_era_d_t_fmt },
        { "era_t_fmt", Scanner::tok_era_t_fmt },
        { "escape_char", Scanner::tok_escape_char },
        { "falsename", Scanner::tok_falsename },
        { "forward", Scanner::tok_forward },
        { "frac_digits", Scanner::tok_frac_digits },
        { "from", Scanner::tok_from },
        { "graph", Scanner::tok_graph },
        { "grouping", Scanner::tok_grouping },
        { "include", Scanner::tok_include },
        { "int_curr_symbol", Scanner::tok_int_curr_symbol },
        { "int_frac_digits", Scanner::tok_int_frac_digits },
        { "int_n_cs_precedes", Scanner::tok_int_n_cs_precedes },
        { "int_n_sep_by_space", Scanner::tok_int_n_sep_by_space },
        { "int_n_sign_posn", Scanner::tok_int_n_sign_posn },
        { "int_p_cs_precedes", Scanner::tok_int_p_cs_precedes },
        { "int_p_sep_by_space", Scanner::tok_int_p_sep_by_space },
        { "int_p_sign_posn", Scanner::tok_int_p_sign_posn },
        { "lower", Scanner::tok_lower },
        { "mon", Scanner::tok_mon },
        { "mon_decimal_point", Scanner::tok_mon_decimal_point },
        { "mon_grouping", Scanner::tok_mon_grouping },
        { "mon_thousands_sep", Scanner::tok_mon_thousands_sep },
        { "noexpr", Scanner::tok_noexpr },
        { "n_cs_precedes", Scanner::tok_n_cs_precedes },
        { "n_sep_by_space", Scanner::tok_n_sep_by_space },
        { "n_sign_posn", Scanner::tok_n_sign_posn },
        { "negative_sign", Scanner::tok_negative_sign },
        { "order_end", Scanner::tok_order_end },
        { "order_start", Scanner::tok_order_start },
        { "p_cs_precedes", Scanner::tok_p_cs_precedes },
        { "p_sep_by_space", Scanner::tok_p_sep_by_space },
        { "p_sign_posn", Scanner::tok_p_sign_posn },
        { "position", Scanner::tok_position },
        { "positive_sign", Scanner::tok_positive_sign },
        { "print", Scanner::tok_print },
        { "punct", Scanner::tok_punct },
        { "reorder-after", Scanner::tok_reorder },
        { "reorder-end", Scanner::tok_reorder_end },
        { "reorder-section-after", Scanner::tok_reorder_section },
        { "reorder-section-end", Scanner::tok_reorder_section_end },
        { "space", Scanner::tok_space },
        { "script", Scanner::tok_script },
        { "t_fmt", Scanner::tok_t_fmt },
        { "t_fmt_ampm", Scanner::tok_t_fmt_ampm },
        { "thousands_sep", Scanner::tok_thousands_sep },
        { "tolower", Scanner::tok_tolower },
        { "toupper", Scanner::tok_toupper },
        { "translit_end", Scanner::tok_translit_end },
        { "translit_start", Scanner::tok_translit_start },
        { "truename", Scanner::tok_truename },
        { "upper", Scanner::tok_upper },
        { "xdigit", Scanner::tok_xdigit },
        { "yesexpr", Scanner::tok_yesexpr },
        { "eof", Scanner::tok_eof }
    };

    int low  = 0;
    int high = sizeof tok_map / sizeof *tok_map - 1;

    // this loop implements a binary search to find 'name' in the   
    // tok_map list and when found returns the token value.
    while (low <= high) {

        const int cur = (low + high) / 2;

        const int cmp = std::strcmp (name, tok_map [cur].name);
        if (0 == cmp)
            return tok_map [cur].token;

        if (cmp < 0)
            high = cur - 1;
        else
            low = cur + 1;
    }

    return Scanner::tok_ndef;
}


void Scanner::
read_line ()
{
    context_->line_.clear ();

    std::getline (context_->file, context_->line_);

    context_->line_ += '\n';
      
    context_->pos_ = context_->line_.c_str ();

    ++context_->line;
    // context_->column = 0;

    ++nlines_;
    
    assert (context_->line_.size ());
}


Scanner::token_t Scanner::
next_token ()
{
    assert (0 != context_);
    assert (context_->file.is_open ());

    // token
    token_t next_tok;

    next_tok.name   = "";
    next_tok.token  = tok_ndef;
    next_tok.line   = 0;
    next_tok.column = 0;
    next_tok.file   = 0;

    while (true) {

        // store the *current* file name
        next_tok.file = context_->filename.c_str ();

        // the assert above for eof checks if the caller has lost it;
        if (context_->file.eof ()) {
            next_tok.token = tok_end_tokens;
            return next_tok;
        }

        // if we exhausted the current line, advance
        if (   context_->line_.size ()
            <= std::size_t (context_->pos_ - context_->line_.c_str ())) {
            read_line ();
        }

        // line and column for the token start; they are set at each
        // iteration; the finding of a token breaks and next_tok leaves
        // this loop having the line/col info
        next_tok.line   = context_->line;
        next_tok.column = context_->pos_ - context_->line_.c_str ();

        // plug in the pointer to current position
        const char*& next = context_->pos_;

        if (*next != context_->comment_char)
            escaped_newline_ = false;

        if (*next == '<') {
            // beginning of a symbolic name or keyword
            const char* tok_begin = next++;

            for (; '>' != *next; ++next) {

                // if has an escaped close angular, pass
                if (*next == context_->escape_char) {

                    // append symbol name up to but not including the escape
                    next_tok.name.append (tok_begin, next - tok_begin);

                    // advance the next pointer to skip the escape
                    tok_begin = ++next;
                }
                else if ('\n' == *next) {
                    // past the end of the line
                    issue_diag (E_SYNTAX, true, &next_tok, 
                                " unterminated symbolic name\n");
                    break;
                }
            }

            next_tok.name.append (tok_begin, ++next - tok_begin);

            // check the name fetched so far
            if (next_tok.name == "<code_set_name>") {
                next_tok.token = tok_code_set_name;
            }
            else if (   next_tok.name == "<escape_char>"
                     || next_tok.name == "<comment_char>") {

                // eat away spaces
                while (' ' == *next || '\t' == *next) {
                    ++next;
                }

                // test for end of line
                if (*next == '\n')
                    issue_diag (E_SYNTAX, true, &next_tok, 
                                "missing value for %s\n",
                                next_tok.name.c_str ());

                // store character
                if (next_tok.name == "<escape_char>")
                    context_->escape_char = *next;
                else
                    context_->comment_char = *next;

                // adjust positions;
                context_->pos_ =
                    context_->line_.c_str () + context_->line_.size ();

                // set token to a newline
                next_tok.name = "";
                next_tok.token = tok_nl;
            }
            else if (next_tok.name == "<mb_cur_max>") {
                next_tok.token = tok_mb_cur_max;
            }
            else if (next_tok.name == "<mb_cur_min>") {
                next_tok.token = tok_mb_cur_min;
            }
            else {
                next_tok.token = tok_sym_name;
            }
            break;
        }
        else if (*next == ' ' || *next == '\t' || *next == ';') {
            // ignore whitespace and separators
            while (*next == ' ' || *next == '\t' || *next == ';') {
                ++next;
            }
        }
        else if (*next == '\n') {
            ++next;
            next_tok.token = tok_nl;
            break;
        }
        else if (*next == context_->comment_char) {
            // start of a comment - check as early as necessary
            // adjust to end of line
            context_->pos_ = context_->line_.c_str () + context_->line_.size ();

            if (escaped_newline_)
                continue;

            next_tok.token = tok_nl;
            next_tok.name = "\n";
            break;
        }
        else if (*next == '(') {
            // push open parenthesis
            next_tok.name.push_back (*next++);

            // start of a grouping
            while (*next != ')') {
                // contains a symbolic name
                if (*next == '<') {
                    // push open angular parenthesis
                    next_tok.name.push_back (*next++);

                    while (*next != '\n') {
                        // if has an escaped close angular, pass
                        if (next [0] == context_->escape_char) {
                            next_tok.name.push_back (*next++);
                            next_tok.name.push_back (*next++);
                            continue;
                        }

                        // if we have reached the end of the sym name
                        if (*next == '>') {
                            next_tok.name.push_back (*next);
                            break;
                        }

                        // still inside the sym name/keyword
                        next_tok.name.push_back (*next++);
                    }

                    // check if we have gone past the end of the line
                    if (*next == '\n')
                        issue_diag (E_SYNTAX, true, &next_tok, 
                                    " unterminated symbolic name");

                    // increment the pointer
                    ++next;
                }
                else {
                    // fetch the character
                    next_tok.name.push_back (*next++);
                }

                if (*next == '\n')
                    issue_diag (E_SYNTAX, true, &next_tok, 
                                " unterminated grouping ");
            }

            next_tok.name.push_back (*next++);
            next_tok.token = tok_grouping;
            break;
        } 
        else if (*next == '.') {
            int ellipsis_count = 0;
            // start of an interval
            while (*next == '.') {
                next_tok.name.push_back (*next++);
                ++ellipsis_count;
            }
            switch (ellipsis_count) {
            case 2 : 
            {
                const char* tmp = next;
                if (*tmp++ == '(' && *tmp++ == '2' && *tmp++ == ')'
                    && *tmp++ == '.' && *tmp++ == '.') {
                    next_tok.token = tok_doub_inc_ellipsis;
                    next = tmp;
                }
                else
                    next_tok.token = tok_dellipsis;
                break;
            }
            case 3:
                next_tok.token = tok_ellipsis;
                break;
            case 4:
                next_tok.token = tok_qellipsis;
                break;
            default:
                issue_diag (E_SYNTAX, true, &next_tok, "illegal ellipsis\n");
            }
            break;

        } 
        else if (*next == '\"') {

            // start of a string
            next_tok.name.push_back (*next++);
            const char ec = context_->escape_char;

            while (next[0] != '\n') {

                // escaped newline; continue
                if (next [0] == ec && next [1] == '\n') {
                    read_line ();
                    continue;
                }

                // escaped quote
                if (next[0] == ec) {
                    next_tok.name.push_back (*next++);
                    next_tok.name.push_back (*next++);
                    continue;
                }

                if (next [0] == '\"') {
                    next_tok.name.push_back (*next);
                    break;
                }

                // still inside the string
                next_tok.name.push_back (*next++);
            }

            // test for closure
            if (*next == '\n')
                issue_diag (E_SYNTAX, true, &next_tok, "unterminated string");
            
            ++next;
            next_tok.token = tok_string;
            break;

        }
        else if (*next == context_->escape_char) {
            // start of an escape sequence
            // escaped new line
            if (next [1] == '\n') {
                // adjust to end of line
                context_->pos_ =
                    context_->line_.c_str () + context_->line_.size ();

                escaped_newline_ = true;
                continue;
            }

            // or
            while (   *next != ' ' && *next != '\t'
                   && *next != ';' && *next != '\n') {
                next_tok.name.push_back (*next++);
            }

            // retrieve token based on value
            next_tok.token = process_token (next_tok.name.c_str ());
            break;
        }
        else {
            // the rest of it
            for (const char ec = context_->escape_char; ; ) {

                // stop at esc-newline or at first "separator"
                if (   (next [0] == ec && next [1] == '\n')
                    || next [0] == ' '
                    || next [0] == '\t' 
                    || next [0] == '\n'
                    || next [0] == ';') {
                    // continuation of a line, separators
                    break;
                } 
                
                // fetch characters
                next_tok.name.push_back (*next++);
            }

            // assert length of input
            assert (next_tok.name.size ());

            // it wasn't a locale definition keyword so call process_token
            // and add the result to the list
            next_tok.token = process_token (next_tok.name.c_str ());
            

            // an extra bit of processing since we keep comment and escape
            // characters in the scanner for a faster processing
            if (   next_tok.token == tok_escape_char
                || next_tok.token == tok_comment_char) {

                // eat away spaces
                while (' ' == *next || '\t' == *next) {
                    ++next;
                }

                // test for end of line
                if (*next == '\n')
                    issue_diag (E_SYNTAX, true, &next_tok,
                                "unterminated statement");

                // store character
                if (next_tok.token == tok_escape_char)
                    context_->escape_char = next [0];
                else
                    context_->comment_char = next [0];

                // adjust positions;
                context_->pos_ =
                    context_->line_.c_str () + context_->line_.size ();

                // return the token
                next_tok.name = "";
                next_tok.token = tok_nl;
            }

            break;
        }
    }

    ++ntokens_;
    
    return next_tok;
}


unsigned char Scanner::
convert_escape (const char* esc, const char **pend /* = 0 */) const
{
    assert (0 != esc);

    const char* s = esc;

    if (escape_char () != *s)
        issue_diag (E_SYNTAX, true, 0,
                    "expected the escape character ('%c'): %s\n",
                    escape_char (), esc);

    int base = 16;
    const char *basename = "hexadecimal";

    switch (*++s) {
    case 'o': base = 8;  basename = "octal"; break;
    case 'd': base = 10; basename = "decimal"; break;
    case 'x': break;
    default:
        issue_diag (E_SYNTAX, true, 0,
                    "one of { 'o', 'd', 'x' } expected following "
                    "the escape character: %s\n", esc);
    }    

    ++s;

    const char *end = 0;

    if (!pend)
        pend = &end;

    const long val = std::strtol (s, (char**)pend, base);

    if (pend == &end && *pend && **pend)
        issue_diag (E_SYNTAX, true, 0,
                    "%s constant expected: %s\n", basename, esc);

    if (val < 0 || val > long (UCHAR_MAX))
        issue_diag (E_INVAL, true, 0,
                    "%s value in the range [0, %lu) expected: %s",
                    basename, esc, long (UCHAR_MAX));

    typedef unsigned char UChar;

    return UChar (val);
}
