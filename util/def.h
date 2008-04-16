/***************************************************************************
 *
 * def.h
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

#ifndef RWSTD_UTIL_DEF_H_INCLUDED
#define RWSTD_UTIL_DEF_H_INCLUDED

#include <list>
#include <locale>              // for ctype_base
#include <map>
#include <string>
#include <vector>

#include <cassert>             // for assert()
#include <climits>             // for UCHAR_MAX
#include <cstddef>             // for size_t

#include <loc/_localedef.h>

#include "scanner.h"
#include "charmap.h"


class Def
{
public:

    // the constructor takes in a pointer to the character map, the name
    // of the file that hold the locale definiton, the name of the locale
    // being created, and the value of mb_cur_max specified in the 
    // charmap file
    Def(const char* filename,
        const char* out_name, 
        Charmap& charmap, bool no_position);
    
    // free up all the dynamically allocated memory
    ~Def ();

    // start point for processing the input files
    void process_input ();

    // write the LC_CTYPE file to the specified directory
    void write_ctype(std::string dir_name);

    void write_codecvt(std::string dir_name);

    // write the LC_NUMERIC file to the specified directory
    void write_numeric(std::string dir_name);

    // write the LC_MONETARY file to the specified directory
    void write_monetary(std::string dir_name);

    // write the LC_TIME file to the specified directory
    void write_time(std::string dir_name);

    // write the LC_MESSAGES file to the specified directory
    void write_messages(std::string dir_name);

    // write the LC_COLLATE file to the specified directory
    void write_collate(std::string dir_name);
 
    // dump the collate information
    void dump_collate ();

    // have warnings occurred
    bool warnings_occurred_;

    // was the content of the locale definition file scanned ahead
    bool scan_ahead_;

    typedef Scanner::token_t                token_t;
    typedef std::pair<token_t,token_t>      token_pair_t;
    typedef std::list<token_t>              token_list_t;
    typedef std::pair<token_t,token_list_t> collate_entry_t;
    typedef std::pair<token_t,token_list_t> collate_elem_t;
    typedef std::list<collate_entry_t>      collate_entry_list_t;
    struct collate_section_t;

    struct collate_section_t {
        std::string          name;
        token_list_t         order;
        collate_entry_list_t entries;
    };

private:
    struct ce_info_t;
    struct collate_info_t;
    friend struct ce_info_t;
    friend struct collate_info_t;

    // a struct used to represent the weights for each collating element
    struct Weights_t {
        unsigned char size;
        unsigned int weight[256];
    };
    
    /////////////////////////////////////////////////////////////////////
    // collate preprocessing information

    token_list_t  script_list_;
    token_list_t  cs_list_;
    token_list_t  sym_list_;
    std::list<collate_elem_t>     ce_list_;
    std::list<collate_section_t>  section_list_;

    // preprocessing for collate section
    void preprocess_collate ();
    void preprocess_order   ();
    void preprocess_reorder ();
    void preprocess_reorder_section ();
    void preprocess_collation_definitions();

    void process_collation_definition ( bool, collate_entry_t&,
                                        unsigned int, unsigned int);
    unsigned int process_order_stmt (collate_section_t&);

    bool insert_entries (token_t&, collate_entry_list_t&);
    void remove_entry   (collate_entry_t&);
    void list_collate   ();

    // automatically fill any ctype categories that depend upon characters
    // being defined in other categories
    void auto_fill ();

    // copy a category from one locale into the current locale
    void copy_category(int cat, std::string name);

    // copy a file
    void copy_file(const std::string &name, const std::string &outname);
    
    // process absolute ellipsis
    std::size_t process_abs_ellipsis (const Scanner::token_t&,
                                      std::ctype_base::mask);

    // process hexadecimal symbolic ellipsis, decimal symbolic ellipsis,
    // and double increment hexadecimal symbolic ellipsis
    std::size_t process_sym_ellipsis (const std::string&,
                                      const std::string&,
                                      Scanner::token_id,
                                      std::ctype_base::mask);

    // parse the era string
    void parse_era (const token_t&);

    // process the ctype category specified by m with the exception of
    // (e.g. std::ctype_base::upper)
    void process_mask (std::ctype_base::mask, const char*);

    // process the ctype toupper and tolower definitions
    void process_upper_lower(Scanner::token_id tok);

    // process the ctype section of the locale definition file
    void process_ctype();

    // process transliteration information
    void process_xlit ();

    void process_xlit_statement (std::size_t&);

    // process the collate section of the locale definition file
    void process_collate ();

    // processing of collating definition statements
    void process_collate_definition (bool, collate_entry_t&,
                                     unsigned int&, unsigned int);

    // helper function for process_collate() that processes the collition
    // order of the collating elements
    void process_order      (collate_section_t&, unsigned int&);

    // helper function for process_order() that processes the sequence
    // of weights for each collating element
    void process_weights(collate_entry_t&);

    // get the next weight
    bool get_weight (token_t&, Weights_t*, int);

    // add a symbolic name to the collition array
    void add_to_coll (const wchar_t val, 
                      const Weights_t* weight_template, 
                      const unsigned int coll_value, 
                      const std::vector<bool>& ordinal_weights,
                      bool undefined_value);

    // add missing values when the UNDEFINED keyword is found or at the
    // end of the collition array if UNDEFINED is not found
    void add_missing_values (const std::vector<bool> &ordinal_weights,
                             const Weights_t* weights_template,
                             unsigned int &coll_value, bool give_warning);

    // process the monetary section of the locale definition file
    void process_monetary();

    // create the monetary formats
    void create_format (char [4], char, char, char, bool);

    // process the numeric section of the locale definition file
    void process_numeric();

    // extracts and converts an array of strings such as those
    // representing the names of weekdays in the LC_TIME section
    Scanner::token_t
    extract_string_array (std::string*, std::wstring*, std::size_t);


    // process the time section of the locale definition file
    void process_time();

    // process the messages section of the locale definition file
    void process_messages();

    std::string convert_string   (const std::string&);
    std::wstring convert_wstring (const token_t&);
    std::wstring convert_wstring (const std::vector<std::string>&);

    void strip_pair(const std::string&, std::string&, std::string&);

    // encode a wchar_t into utf8 encoding
    std::string utf8_encode (wchar_t ch);

    // convert a utf8 encoded string to the encoding for this locale
    std::string convert_to_ext (wchar_t val);

    bool get_n_val (const Scanner::token_t&, unsigned char &val);
    bool get_w_val (const Scanner::token_t&, wchar_t &val);

    // initialize the coll_map with all the characters in the codeset
    void init_coll_map();

    void gen_n_to_w_coll_tables (const std::string &charp, 
                                unsigned int tab_num);

    void gen_w_to_n_coll_tables (const std::string &charp, 
                                 unsigned int tab_num);

    // the next useable offset for collating elements greater then UCHAR_MAX
    unsigned int next_offset_;

    Scanner::token_t next;
    
    // the name of the locale we are creating
    std::string output_name_;
        
    // the charmap used to process the character map definition file
    Charmap& charmap_;

    // the scanner used to process the locale definition file
    Scanner scanner_;

    bool ctype_symlink_;
    std::string ctype_filename_;
    

    // maps characters to a mask value
    std::map<wchar_t, unsigned int> mask_;
    
    // maps characters to their lower case representation
    std::map<wchar_t, wchar_t> lower_;

    // maps characters to their upper case representation
    std::map<wchar_t, wchar_t> upper_;

    typedef std::map<std::string, unsigned int>::iterator mb_char_off_map_iter;

    struct codecvt_offset_tab_t {
        unsigned int off [UCHAR_MAX + 1];
    };

    void create_wchar_utf8_table ();
    std::map<std::string, std::string> wchar_utf8_to_ext_;
    typedef std::map<std::string, std::string>::iterator wchar_utf8_iter;

    void gen_valid_coll_wchar_set ();

    std::set<std::string> valid_coll_wchar_set_;
    typedef std::set<std::string>::iterator valid_coll_wchar_set_iter;
    std::set<std::string> valid_codecvt_wchar_set_;
    typedef std::set<std::string>::iterator valid_codecvt_wchar_set_iter;

    typedef std::map<unsigned, const codecvt_offset_tab_t*>
    codecvt_offsets_map_t;

    // generates conversion tables of all valid multibyte characters
    // from a multibyte character map populated from the character
    // set description file
    std::size_t
    gen_mbchar_tables (codecvt_offsets_map_t&,
                       std::map<std::string, unsigned>&,
                       const std::string& = "",
                       unsigned = 0);

    std::size_t
    gen_wchar_tables (codecvt_offsets_map_t&,
                      const std::string& = "",
                      unsigned = 0);

    std::size_t
    gen_utf8_tables (codecvt_offsets_map_t&,
                     std::map<std::string, unsigned>&,
                     const std::string& = "",
                     unsigned = 0);

    std::set<std::string> valid_coll_mb_set_;

    void gen_valid_coll_mb_set();

    // generation of transliteration tables
    void gen_xlit_data ();

    // specifies if the locale file has already been written such as when
    // the "copy" directive is used in a locale definition file
    bool ctype_written_, codecvt_written_, collate_written_, time_written_,
        num_written_, mon_written_, messages_written_;
    bool ctype_def_found_, collate_def_found_, 
        time_def_found_, num_def_found_, mon_def_found_, messages_def_found_;

    // specifies if the keyword UNDEFINED is used in the LC_COLLATE definition
    bool undefined_keyword_found_;

    // no_position_ is set by the "--no_position" command line option
    // when true forward,postion orders will be treated like forward orders
    bool no_position_;

    // collate maps

    struct offset_tab_t {
        int first_offset;
        unsigned int off[UCHAR_MAX + 1];
    };

    std::map<unsigned int, offset_tab_t> char_offs_;
    typedef std::map<unsigned int, offset_tab_t>::iterator char_offs_iter;

    std::map<unsigned int, offset_tab_t> w_to_n_coll_;
    typedef std::map<unsigned int, offset_tab_t>::iterator w_to_n_coll_iter;

    unsigned int next_tab_num_;
    unsigned int next_wchar_coll_tab_num_;


    struct ce_offset_tab_t {
        int first_offset;
        int last_offset;
        unsigned int off[UCHAR_MAX + 1];
    };
    std::map<unsigned int, ce_offset_tab_t> n_ce_offs_;
    typedef std::map<unsigned int, ce_offset_tab_t>::iterator n_ce_offs_iter;

    std::map<unsigned int, ce_offset_tab_t> w_ce_offs_;
    typedef std::map<unsigned int, ce_offset_tab_t>::iterator w_ce_offs_iter;
    
    std::set<std::string> valid_n_ce_set;
    typedef std::set<std::string>::iterator valid_n_ce_set_iter;
    
    void gen_n_ce_tables (const std::set<std::string>,
                          unsigned int, unsigned int);
    unsigned int next_n_ce_tab_num_;
    void gen_w_ce_tables (const std::set<std::string>,
                          unsigned int, unsigned int);
    unsigned int next_w_ce_tab_num_;

    std::map<std::string, std::string>ce_sym_map_;
    std::map<std::string, std::string>ce_wsym_map_;
    typedef std::map<std::string, std::string>::iterator ce_sym_map_iter;
    // off_mapr maps an offset value to the symbol name or collating element
    std::map<unsigned int, std::string> off_mapr_;

    // cs_map_ maps a collating symbol name to a collation value
    std::map<std::string, unsigned int> cs_map_;
    typedef std::map<std::string, unsigned int>::iterator cs_map_iter;

    // transliteration information
    struct xlit_offset_table {
        unsigned int offset_table [UCHAR_MAX + 1];
    };
    typedef struct xlit_offset_table  xlit_offset_table_t;
    typedef std::map<wchar_t,std::list<std::string> >  xlit_map_t;
    typedef std::map<wchar_t, unsigned int>            xlit_data_offset_map_t;
    typedef std::map<unsigned int,xlit_offset_table_t> xlit_table_map_t;
    xlit_map_t                    xlit_map_;
    xlit_data_offset_map_t        xlit_data_offset_map_;
    xlit_table_map_t              xlit_table_map_;

    // the collate_info_t struct contains information concerning the collation
    // of each character
    struct collate_info_t{
        unsigned int offset;
        unsigned int coll_val;
        unsigned int order;
        Weights_t *weights;
    };

    // we need one collate_info_t to hold information about the undefined
    // characters.  All the other characters have collate_info_ts that are
    // located in the coll_map.
    collate_info_t undef_char_info_;

    // the ce_info_t strurct contains information concerning the collation
    // of a collating element.

    struct ce_info_t {
        unsigned int offset;
        unsigned int coll_val;
        unsigned int order;
        Weights_t *weights;
        std::wstring ce_wstr;
    };

    // The coll_map_ contains a mapping from the wide char value to the
    // collition information about that value.
    std::map<wchar_t, collate_info_t> coll_map_;
    typedef std::map<wchar_t, collate_info_t>::iterator coll_map_iter;

    // the ce_map_ contains a mapping from the symbolic collating element
    // name to the collition information about that element
    std::map <std::string, ce_info_t> ce_map_;
    typedef std::map <std::string, ce_info_t>::iterator ce_map_iter;
    
    // iterator type definitions for the maps
    typedef std::map<wchar_t, unsigned int>::iterator mask_iter;
    typedef std::map<wchar_t, wchar_t>::iterator upper_iter;
    typedef std::map<wchar_t, wchar_t>::iterator lower_iter;
    typedef std::map< std::string, unsigned char >::const_iterator n_cmap_iter;
    typedef std::map<std::string, wchar_t>::const_iterator mb_cmap_iter;
    typedef std::map<wchar_t, std::string>::const_iterator rmb_cmap_iter;
    typedef std::map<std::string, wchar_t >::const_iterator w_cmap_iter;
    typedef std::map<wchar_t, std::string >::const_iterator rw_cmap_iter;
    typedef std::map<unsigned int, std::string>::iterator off_mapr_iter;
    typedef std::map<std::string, wchar_t>::const_iterator ucs4_cmap_iter;

    typedef std::list<std::string>::const_iterator symnames_list_iter;

    // the structures used to hold the offsets for each locale category
    // and any non-pointer locale information
    _RW::__rw_punct_t num_punct_out_;
    _RW::__rw_ctype_t ctype_out_;
    _RW::__rw_time_t time_out_;
    _RW::__rw_collate_t collate_out_;
    _RW::__rw_mon_t mon_out_;
    _RW::__rw_num_t num_out_;
    _RW::__rw_messages_t messages_out_;

    // structures used for internally holding locale information
    // LC_CTYPE structures


    struct ctype_t {
        struct mask_elm {
            wchar_t ch;             // the wide character value
            unsigned int mask;      // the mask for that character
        };
        struct upper_elm {
            wchar_t lower;    // the lower case wide character
            wchar_t upper;    // the upper case wide character 
        };
        struct lower_elm {
            wchar_t upper;    // the upper case wide character
            wchar_t lower;    // the lower case wide character 
        };

        char max_mb_s;                  // the max number of bytes in a char
        upper_elm* wtoupper_tab;        // the wide char to_upper table
        lower_elm* wtolower_tab;        // the wide char to_lower table
        mask_elm* wmask_tab;            // the wide char mask_table

    };
    

    struct era_st {
        std::string name;     
        std::string fmt;
        std::wstring wname;
        std::wstring wfmt;
        _RW::__rw_time_t::era_t era_out;
    };

    std::list<era_st> era_list_;
    typedef std::list<era_st>::iterator era_list_iter;

    // LC_COLLATE structure
    struct collate_t {
    } ;



    // LC_MONETARY structure
    struct mon_t {
        std::string int_curr_symbol;    // narrow char* int_curr_symbol
        std::string currency_symbol;    // narrow char* currency_symbol
        std::string mon_decimal_point;  // narrow char* mon_decimal_point
        std::string mon_thousands_sep;  // narrow char* mon_thoucands_sep
        std::string mon_grouping;       // narrow char* mon_grouping
        std::string positive_sign;      // narrow char* positive_sign
        std::string negative_sign;      // narrow char* negative_sign
        std::wstring wint_curr_symbol;   // wide wchar_t* int_curr_symbol
        std::wstring wcurrency_symbol;   // wide wchar_t* currency_symbol
        std::wstring wmon_decimal_point; // wide wchar_t* mon_decimal_point
        std::wstring wmon_thousands_sep; // wide wchar_t* mon_thousands_sep
        std::wstring wpositive_sign;     // wide wchar_t* positive_sign
        std::wstring wnegative_sign;     // wide wchar_t* negative_sign
    };



    // LC_NUMERIC structure
    struct num_t {
        std::string decimal_point;  // narrow char* decimal_point
        std::string thousands_sep;  // narrow char* thousands_sep
        std::string grouping;       // narrow char* grouping
        std::string truename;       // narrow char* truename
        std::string falsename;      // narrow char* falsename

        std::wstring wdecimal_point;   // wide wchar_t* decimal_point
        std::wstring wthousands_sep;   // wide wchar_t* thousands_sep     
        std::wstring wtruename;        // wide wchar_t* truename
        std::wstring wfalsename;       // wide wchar_t* falsename
    };

    // list to hold the alternate digits
    struct alt_digit_t {
        std::string n_alt_digit;
        std::wstring w_alt_digit;
        unsigned int n_offset;
        unsigned int w_offset;
    };
    std::list<alt_digit_t> alt_digits_;
    typedef std::list<alt_digit_t>::iterator alt_digits_iter;

    // LC_TIME structure
    struct time_t {
        std::string abday[7];      // narrow array of abbreviated days
        std::string day[7];        // narrow array of days
        std::string abmon[12];     // narrow array of abbreviated months
        std::string mon[12];       // narrow array of months
        std::string am_pm[2];      // narrow array of am/pm specifiers
        std::string d_t_fmt;       // narrow date and time format string
        std::string d_fmt;         // narrow date format string
        std::string t_fmt;         // narrow time format string
        std::string t_fmt_ampm;    // narrow time format string with am/pm
        std::string era_d_t_fmt;   // narrow era date and time format string
        std::string era_d_fmt;     // narrow era date format string
        std::string era_t_fmt;     // narrow era time format string



        std::wstring wabday[7];      // wide array of abbreviated days
        std::wstring wday[7];        // wide array of days
        std::wstring wabmon[12];     // wide array of abbreviated months
        std::wstring wmon[12];       // wide array of months
        std::wstring wam_pm[2];      // wide array of am/pm specifiers
        std::wstring wd_t_fmt;       // wide date and time format string
        std::wstring wd_fmt;         // wide date format string
        std::wstring wt_fmt;         // wide time format string
        std::wstring wt_fmt_ampm;    // wide time format string with am/pm
        std::wstring wera_d_t_fmt;   // wide era date and time format string
        std::wstring wera_d_fmt;     // wide era date format string
        std::wstring wera_t_fmt;     // wide era time format string
    } ;

    // LC_MESSAGES structure
    struct messages_t {
        std::string yesexpr;
        std::string noexpr;

        std::wstring wyesexpr;
        std::wstring wnoexpr;
    };

    messages_t messages_st_;
    time_t time_st_;
    ctype_t ctype_st_;
    mon_t mon_st_;
    num_t num_st_;
    collate_t collate_st_;


};


inline std::string strip_quotes (const std::string& str)
{
    assert (0 != str.size ());
    assert (str [0] == '\"');

    // return a string from str[1] to the position of the end-quote
    return std::string (str, 1, str.rfind ('\"') - 1);

}


#endif   // RWSTD_UTIL_DEF_H_INCLUDED
