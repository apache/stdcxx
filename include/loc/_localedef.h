/***************************************************************************
 *
 * _localedef.h
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

#ifndef _RWSTD_LOC_LOCALEDEF_H_INCLUDED
#define _RWSTD_LOC_LOCALEDEF_H_INCLUDED


#include <rw/_defs.h>


#if 6 <= _RWSTD_HP_aCC_MAJOR
   // suppress HP aCC 6 remarks
   // 4298: addition result truncated before cast to bigger sized type
   // 4299: multiply result truncated before cast to bigger sized type
#  pragma diag_suppress 4298, 4299
#endif   // aCC >= 6.0


_RWSTD_NAMESPACE (__rw) { 

// LC_CTYPE structures
struct __rw_mask_elm
{
    wchar_t         ch;        // the wide character value
    _RWSTD_UINT32_T mask;      // the mask for that character
};

struct __rw_upper_elm
{
    wchar_t lower;    // the lower case wide character
    wchar_t upper;    // the upper case wide character that the lower maps to
};

struct __rw_lower_elm
{
    wchar_t upper;    // the upper case wide character
    wchar_t lower;    // the lower case wide character that the upper maps to
};


// returns a pointer to a character at the given byte offset
// from the end of the structure
#define _RWSTD_CHAR_ARRAY(off)   \
    (_RWSTD_REINTERPRET_CAST (const char*, this + 1) + (off))

// returns a pointer to a wide character at the given byte offset
// from the end of the structure
#define _RWSTD_WCHAR_T_ARRAY(off)   \
    _RWSTD_REINTERPRET_CAST (const wchar_t*, _RWSTD_CHAR_ARRAY (off))

#define _RWSTD_UINT_ARRAY(off)    \
    _RWSTD_REINTERPRET_CAST (const _RWSTD_UINT32_T*, _RWSTD_CHAR_ARRAY (off))

// returns a pointer to the i-th element of an array
// at the given byte offset from the end of the structure
#define _RWSTD_ARRAY_ELM(T, off, i)                        \
    _RWSTD_REINTERPRET_CAST (const T*,                     \
        (  _RWSTD_REINTERPRET_CAST (const char*, this + 1) \
         + (off) + (i) * sizeof (T)))


struct __rw_codecvt_t
{
    // offsets from the end of *this
    _RWSTD_UINT32_T codeset_off;
    _RWSTD_UINT32_T charmap_off;
    _RWSTD_UINT32_T n_to_w_tab_off;
    _RWSTD_UINT32_T w_to_n_tab_off;
    _RWSTD_UINT32_T utf8_to_ext_tab_off;
    _RWSTD_UINT32_T wchar_off;
    _RWSTD_UINT32_T xliteration_off;
    _RWSTD_UINT32_T codecvt_ext_off;

    // the size of the longest multibyte character
    _RWSTD_UINT8_T mb_cur_max;

    // the name of the codeset and its database file
    const char* codeset_name () const {
        return _RWSTD_CHAR_ARRAY (codeset_off);
    }

    // the name of the character map description file
    const char* charmap_name () const {
        return _RWSTD_CHAR_ARRAY (charmap_off);
    }

    // pointer to the first table for wide to narrow conversions
    const _RWSTD_UINT32_T* w_to_n_tab () const {
        return _RWSTD_UINT_ARRAY (w_to_n_tab_off);
    }

    // pointer to the first table for narrow to wide conversions
    const _RWSTD_UINT32_T* n_to_w_tab () const {
        return _RWSTD_UINT_ARRAY (n_to_w_tab_off);
    }
    
    // pointer to the first table for utf8 to external conversions
    const _RWSTD_UINT32_T* utf8_to_ext_tab () const {
        return _RWSTD_UINT_ARRAY (utf8_to_ext_tab_off);
    }

    // returns the value of the wide character at a given offset
    // obtained from one of the n_to_w_tab() tables
    wchar_t get_internal_at_offset (_RWSTD_UINT32_T off) const {
        return *(_RWSTD_ARRAY_ELM (wchar_t, wchar_off, off * 2));

    }

    // returns the value of the UCS character at a given offset
    wchar_t get_ucs4_at_offset (_RWSTD_UINT32_T off) const {
        return *(_RWSTD_ARRAY_ELM (wchar_t, wchar_off, off * 2 + 1));
    }

    const void* get_ext () const {
        return _RWSTD_CHAR_ARRAY (codecvt_ext_off);
    }

    const _RWSTD_UINT32_T* get_xliteration_tab () const {
        return _RWSTD_UINT_ARRAY (xliteration_off);
    }
};


// LC_CTYPE structure
struct __rw_ctype_t
{
    _RWSTD_UINT32_T codeset_off;        // byte offset of the codeset_name
    _RWSTD_UINT32_T charmap_off;        // byte offset of the charmap name
    _RWSTD_UINT32_T wmask_s;
    _RWSTD_UINT32_T wtoupper_off;       // byte offset of the wtoupper_tab
    _RWSTD_UINT32_T wtolower_off;       // byte offset of the wtolower_tab
    _RWSTD_UINT32_T wmask_off;          // byte offset of the wmask_tab_tab

    // this is added to allow for future extensions
    _RWSTD_UINT32_T ctype_ext_off;      // extended ctype data offset

    _RWSTD_UINT8_T  toupper_tab[256];   // the narrow char to_upper table
    _RWSTD_UINT8_T  tolower_tab[256];   // the narrow char to_lower table
    _RWSTD_UINT32_T mask_tab[256];      // the narrow char mask table
    _RWSTD_UINT8_T  mb_cur_max;         // max number of bytes per MB character

    _RWSTD_SIZE_T wtoupper_s () const {
        return (wtolower_off - wtoupper_off) / sizeof (__rw_upper_elm);
    }

    _RWSTD_SIZE_T wtolower_s () const {
        return (wmask_off - wtolower_off) / sizeof (__rw_lower_elm);
    }

    _RWSTD_SIZE_T codeset_s () const {
        return charmap_off - codeset_off;
    }

    _RWSTD_SIZE_T charmap_s () const {
        return wtoupper_off - charmap_off;
    }

    const char* codeset_name () const {
        return _RWSTD_CHAR_ARRAY (codeset_off);
    }

    const char* charmap_name () const {
        return _RWSTD_CHAR_ARRAY (charmap_off);
    }

    __rw_upper_elm wtoupper_tab (_RWSTD_SIZE_T idx) const {
        return *_RWSTD_ARRAY_ELM (__rw_upper_elm, wtoupper_off, idx);
    }

    __rw_lower_elm wtolower_tab (_RWSTD_SIZE_T idx) const {
        return *_RWSTD_ARRAY_ELM (__rw_lower_elm, wtolower_off, idx);
    }

    __rw_mask_elm wmask_tab (_RWSTD_SIZE_T idx) const {
        return *_RWSTD_ARRAY_ELM (__rw_mask_elm, wmask_off, idx);
    }

    const void* get_ext () const {
        return _RWSTD_CHAR_ARRAY (ctype_ext_off);
    }

};
    

// LC_COLLATE structure
struct __rw_collate_t
{
    _RWSTD_UINT32_T codeset_off;    // byte offset of the codeset_name
    _RWSTD_UINT32_T charmap_off;    // byte offset to the charmap name

    _RWSTD_UINT32_T n_ce_tab_off;
    _RWSTD_UINT32_T w_ce_tab_off;

    _RWSTD_UINT32_T weight_tab_off;  // offset to the weight information

    _RWSTD_UINT32_T n_char_tab_off; // offset to the first nchar table
    _RWSTD_UINT32_T n_char_off_tab_off;  // offset to offset table for nchars
    _RWSTD_UINT32_T n_char_first_char_off; // offset to first character info

    _RWSTD_UINT32_T w_char_tab_off; // offset to the first wide character table
    _RWSTD_UINT32_T w_char_off_tab_off;  // offset to offset table for wchars
    _RWSTD_UINT32_T w_char_first_char_off; // offset to first character info

    _RWSTD_UINT32_T n_ce_off_tab_off;
    _RWSTD_UINT32_T w_ce_off_tab_off;
    _RWSTD_UINT32_T n_ce_first_char_off;
    _RWSTD_UINT32_T w_ce_first_char_off;
    _RWSTD_UINT32_T n_ce_last_char_off;
    _RWSTD_UINT32_T w_ce_last_char_off;

    _RWSTD_UINT32_T undefined_weight_idx;

    _RWSTD_UINT32_T elm_size;       // number of bytes in each coll array elem
    _RWSTD_UINT32_T num_elms;       // numer of elements in coll array

    _RWSTD_UINT32_T num_wchars;     // number of wide chars in wchar mapping

    // this is added to allow for future extensions
    _RWSTD_UINT32_T collate_ext_off;      // extended collate data offset

    _RWSTD_UINT32_T undefined_optimization;

    _RWSTD_UINT8_T longest_weight;    // the longest weight value
    _RWSTD_UINT8_T num_weights;       // number of weights
    _RWSTD_UINT8_T largest_ce;

    _RWSTD_UINT8_T weight_type[256];  // weight types (ex. forward)

    // get the offset of a table number `tabno'
    _RWSTD_UINT32_T get_n_tab_off (_RWSTD_UINT32_T tabno) const {
        return (_RWSTD_UINT32_T)
            (sizeof (_RWSTD_UINT32_T) * (*(const _RWSTD_UINT32_T*)(
             (_RWSTD_SIZE_T)((const char*)this + sizeof *this 
                            + n_char_off_tab_off 
                            + (tabno * sizeof (_RWSTD_UINT32_T))))));
    }
    _RWSTD_UINT32_T get_n_ce_tab_off (_RWSTD_UINT32_T tabno) const {
        return (_RWSTD_UINT32_T)
            (sizeof (_RWSTD_UINT32_T) * (*(const _RWSTD_UINT32_T*)(
             (_RWSTD_SIZE_T)((const char*)this + sizeof *this 
                            + n_ce_off_tab_off 
                            + (tabno * sizeof (_RWSTD_UINT32_T))))));
    }

    _RWSTD_UINT32_T get_w_ce_tab_off (_RWSTD_UINT32_T tabno) const {
        return (_RWSTD_UINT32_T)
            (sizeof (_RWSTD_UINT32_T) * (*(const _RWSTD_UINT32_T*)(
             (_RWSTD_SIZE_T)((const char*)this + sizeof *this 
                            + w_ce_off_tab_off 
                            + (tabno * sizeof (_RWSTD_UINT32_T))))));
    }

    _RWSTD_UINT8_T get_first_char_in_n_tab (_RWSTD_UINT32_T tabno) const {
        return *((const _RWSTD_UINT8_T*)this + sizeof *this 
                 + n_char_first_char_off + tabno);
        
    }

    _RWSTD_UINT8_T get_first_char_in_n_ce_tab (_RWSTD_UINT32_T tabno) const {
        return *((const _RWSTD_UINT8_T*)this + sizeof *this 
                 + n_ce_first_char_off + tabno);
    }

    _RWSTD_UINT8_T get_first_char_in_w_ce_tab (_RWSTD_UINT32_T tabno) const {
        return *((const _RWSTD_UINT8_T*)this + sizeof *this 
                 + w_ce_first_char_off + tabno);
    }

    _RWSTD_UINT8_T get_last_char_in_n_ce_tab (_RWSTD_UINT32_T tabno) const {
        return *((const _RWSTD_UINT8_T*)this + sizeof *this 
                 + n_ce_last_char_off  + tabno);
    }

    _RWSTD_UINT8_T get_last_char_in_w_ce_tab (_RWSTD_UINT32_T tabno) const {
        return *((const _RWSTD_UINT8_T*)this + sizeof *this 
                 + w_ce_last_char_off + tabno);
    }

    const _RWSTD_UINT32_T* get_n_tab (_RWSTD_UINT32_T tabno) const {
        return (const _RWSTD_UINT32_T*)
            ((_RWSTD_SIZE_T)((const char*)this + sizeof *this
                               + n_char_tab_off + get_n_tab_off (tabno)));

    }
    const _RWSTD_UINT32_T* get_n_ce_tab (_RWSTD_UINT32_T tabno) const {
        return (const _RWSTD_UINT32_T*)
            ((_RWSTD_SIZE_T)((const char*)this + sizeof *this
                             + n_ce_tab_off + get_n_ce_tab_off (tabno)));
    }

    const _RWSTD_UINT32_T* get_w_ce_tab (_RWSTD_UINT32_T tabno) const {
        return (const _RWSTD_UINT32_T*)
            ((_RWSTD_SIZE_T)((const char*)this + sizeof *this
                             + w_ce_tab_off + get_w_ce_tab_off (tabno)));
    }

    _RWSTD_UINT32_T get_w_tab_off (_RWSTD_UINT32_T tabno) const {
        return (_RWSTD_UINT32_T)
            (sizeof (_RWSTD_UINT32_T) * (*(const _RWSTD_UINT32_T*)(
             (_RWSTD_SIZE_T)((const char*)this + sizeof *this
                            + w_char_off_tab_off 
                            + (tabno * sizeof (_RWSTD_UINT32_T))))));
    }

    _RWSTD_UINT8_T get_first_char_in_w_tab (_RWSTD_UINT32_T tabno) const {
        return *((const _RWSTD_UINT8_T*)this + sizeof *this 
                 + w_char_first_char_off + tabno);
        
    }

    const _RWSTD_UINT32_T* get_w_tab (_RWSTD_UINT32_T tabno) const {
        return (const _RWSTD_UINT32_T*)
            ((_RWSTD_SIZE_T)((const char*)this + sizeof *this
                             + w_char_tab_off + get_w_tab_off (tabno)));

    }

    const _RWSTD_UINT32_T* coll () const {
        return _RWSTD_UINT_ARRAY (weight_tab_off); 
    }

    const char* codeset_name () const {
        return _RWSTD_CHAR_ARRAY (codeset_off);
    }

    const char* charmap_name () const {
        return _RWSTD_CHAR_ARRAY (charmap_off);
    }

    const _RWSTD_UINT32_T* get_weight (_RWSTD_UINT32_T idx) const {
        return (const _RWSTD_UINT32_T*)((_RWSTD_SIZE_T)((const char*)coll ()
                                                        + elm_size * idx));
    }

    const void* get_ext () const {
        return _RWSTD_CHAR_ARRAY (collate_ext_off);
    }
};


// punct structure for monetary and numeric facets
struct __rw_punct_t
{
    _RWSTD_UINT32_T decimal_point_off [2];   // narrow and wide decimal_point
    _RWSTD_UINT32_T thousands_sep_off [2];   // narrow and wide thousands_sep
    _RWSTD_UINT32_T grouping_off;            // grouping string
    _RWSTD_UINT32_T punct_ext_off;           // offset of extended data

    // decimal_point (0) points to a char*, decimal_point (1) to wchar_t*
    const void* decimal_point (bool wide) const {
        return _RWSTD_CHAR_ARRAY (decimal_point_off [wide]);
    }

    // thousands_sep (0) points to a char*, thousands_sep (1) to wchar_t*
    const void* thousands_sep (bool wide) const {
        return _RWSTD_CHAR_ARRAY (thousands_sep_off [wide]);
    }

    const void* grouping () const {
        return _RWSTD_CHAR_ARRAY (grouping_off);
    }

    const void* get_ext () const {
        return _RWSTD_CHAR_ARRAY (punct_ext_off);
    }
};


// LC_MONETARY structure
struct __rw_mon_t
{
    _RWSTD_UINT32_T codeset_off;           // byte offset of the codeset_name
    _RWSTD_UINT32_T charmap_off;           // byte offset to the charmap name

    _RWSTD_UINT32_T curr_symbol_off [2][2];    // byte offset of curr_symbol
    _RWSTD_UINT32_T positive_sign_off [2];     // byte offset of positive_sign
    _RWSTD_UINT32_T negative_sign_off [2];     // byte offset of negative_sign

    // this is added to allow for future extensions
    _RWSTD_UINT32_T monetary_ext_off;      // extended monetary data offset

    // monetary format pattern for positive values
    char pos_format[2][4];    

    // monetary format pattern for negative values
    char neg_format[2][4];    

    // num of frac digits to write using currency_symbol 
    char frac_digits[2];      

    // 0 = currency symbol then pos value
    // 1 = pos value then currency symbol 
    char p_cs_precedes[2];    
                                     

    // separation of curr symbol and value
    // 0 - no separation
    // 1 = if the symbol and sign are adjacent, a space separates them from 
    //     the value; otherwise, a space separates the symbol from the value 
    // 2 = if the symbol and sign are adjacent, a space separates them; 
    //     otherwise, a space separates  the sign from the value
    char p_sep_by_space[2];   

    // 0 = currency symbol succeeds the neg value
    // 1 = currency symbol precedes the neg value
    char n_cs_precedes[2]; 

    // 0 = no space seperates curr sym from neg value
    // 1 = space seperates symbol from neg value
    // 2 = space seperates symbol and sign string
    char n_sep_by_space[2];

    // 0 = Parentheses enclose the quantity of the curr sym
    // 1 = Sign string precedes the quantity and curr sym
    // 2 = sign string succeeds the quantity and curr sym
    // 3 = sign string precedes the curr symbol
    // 4 = sign string succeeds the curr symbol
    char p_sign_posn[2];

    // 0 = Parentheses enclose the quantity of the curr sym
    // 1 = Sign string precedes the quantity and curr sym
    // 2 = sign string succeeds the quantity and curr sym
    // 3 = sign string precedes the curr symbol
    // 4 = sign string succeeds the curr symbol
    char n_sign_posn[2];

    const void* curr_symbol (bool intl, bool wide) const {
        return _RWSTD_CHAR_ARRAY (curr_symbol_off [intl][wide]);
    }

    const void* positive_sign (bool wide) const {
        return _RWSTD_CHAR_ARRAY (positive_sign_off [wide]);
    }

    const void* negative_sign (bool wide) const {
        return _RWSTD_CHAR_ARRAY (negative_sign_off [wide]);
    }

    const char* codeset_name () const {
        return _RWSTD_CHAR_ARRAY (codeset_off);
    }

    const char* charmap_name () const {
        return _RWSTD_CHAR_ARRAY (charmap_off);
    }

    const void* get_ext () const {
        return _RWSTD_CHAR_ARRAY (monetary_ext_off);
    }
};


// LC_NUMERIC structure
struct __rw_num_t
{
    _RWSTD_UINT32_T codeset_off;       // byte offset of the codeset_name
    _RWSTD_UINT32_T charmap_off;       // byte offset to the charmap name

    _RWSTD_UINT32_T truename_off [2];  // offset of narrow and wide truename
    _RWSTD_UINT32_T falsename_off [2]; // offset of narrow and wide falsename

    _RWSTD_UINT32_T numeric_ext_off;     // extended numeric data offset

    const void* truename (bool wide) const {
        return _RWSTD_CHAR_ARRAY (truename_off [wide]);
    }

    const void* falsename (bool wide) const {
        return _RWSTD_CHAR_ARRAY (falsename_off [wide]);
    }

    const char* codeset_name () const {
        return _RWSTD_CHAR_ARRAY (codeset_off);
    }

    const char* charmap_name () const {
        return _RWSTD_CHAR_ARRAY (charmap_off);
    }

    const void* get_ext () const {
        return _RWSTD_CHAR_ARRAY (numeric_ext_off);
    }
};


// LC_MESSAGES structure
struct __rw_messages_t
{
    _RWSTD_UINT32_T codeset_off;
    _RWSTD_UINT32_T charmap_off;
    
    _RWSTD_UINT32_T yesexpr_off[2];
    _RWSTD_UINT32_T noexpr_off[2];

    _RWSTD_UINT32_T messages_ext_off;

    const void* yesexpr (bool wide) const {
        return _RWSTD_CHAR_ARRAY (yesexpr_off[wide]);
    }

    const void* noexpr (bool wide) const {
        return _RWSTD_CHAR_ARRAY (noexpr_off[wide]);
    }

    const char* codeset_name () const {
        return _RWSTD_CHAR_ARRAY (codeset_off);
    }

    const char* charmap_name () const {
        return _RWSTD_CHAR_ARRAY (charmap_off);
    }

    const void* get_ext () const {
        return _RWSTD_CHAR_ARRAY (messages_ext_off);
    }

};


// LC_TIME structure
struct __rw_time_t
{
    _RWSTD_UINT32_T codeset_off;     // byte offset of the codeset_name
    _RWSTD_UINT32_T charmap_off;     // byte offset to the charmap name
    _RWSTD_UINT32_T num_eras;        // the number of eras
    _RWSTD_UINT32_T num_alt_digits;  // the number of alt_digits

    _RWSTD_UINT32_T era_off;            // bute offset of the first era
    _RWSTD_UINT32_T alt_digits_off;      // byte offset to offset array 
    _RWSTD_UINT32_T abday_off[2][7];    // byte offset of each abday
    _RWSTD_UINT32_T day_off[2][7];      // byte offset of each day
    _RWSTD_UINT32_T abmon_off[2][12];   // byte offset of each abmon
    _RWSTD_UINT32_T mon_off[2][12];     // byte offset of each mon
    _RWSTD_UINT32_T am_pm_off[2][2];    // byte offset of am and pm
    _RWSTD_UINT32_T d_t_fmt_off[2];     // byte offset of d_t_fmt
    _RWSTD_UINT32_T d_fmt_off[2];       // byte offset of d_fmt
    _RWSTD_UINT32_T t_fmt_off[2];       // byte offset of t_fmt
    _RWSTD_UINT32_T t_fmt_ampm_off[2];  // byte offset of t_fmt_ampm
    _RWSTD_UINT32_T era_d_t_fmt_off[2]; // byte offset of era_d_t_fmt
    _RWSTD_UINT32_T era_d_fmt_off[2];   // byte offset of era_d_fmt
    _RWSTD_UINT32_T era_t_fmt_off[2];   // byte offset of era_t_fmt

    // this is added to allow for future extensions
    _RWSTD_UINT32_T time_ext_off;      // extended time data offset

    // %a: abbreviated weekday name [tm_wday]
    const void* abday (_RWSTD_SIZE_T idx, bool wide) const {
        return _RWSTD_CHAR_ARRAY (abday_off [wide][idx]);
    }

    // %A: full weekday name [tm_wday]
    const void* day (_RWSTD_SIZE_T idx, bool wide) const {
        return _RWSTD_CHAR_ARRAY (day_off [wide][idx]);
    }

    // %b: abbreviated month name [tm_mon]
    const void* abmon (_RWSTD_SIZE_T idx, bool wide) const {
        return _RWSTD_CHAR_ARRAY (abmon_off [wide][idx]);
    }

    // %B: full month name [tm_mon]
    const void* mon (_RWSTD_SIZE_T idx, bool wide) const {
        return _RWSTD_CHAR_ARRAY (mon_off [wide][idx]);
    }

    // %p: the locale's equivalent of the AM/PM designations
    //     associated with a 12-hour clock [tm_hour]
    const void* am_pm (_RWSTD_SIZE_T idx, bool wide) const {
        return _RWSTD_CHAR_ARRAY (am_pm_off [wide][idx]);
    }

    // %c: the appropriate date and time representation
    const void* d_t_fmt (bool wide) const {
        return _RWSTD_CHAR_ARRAY (d_t_fmt_off[wide]);
    }

    // %x: the appropriate date representation
    const void* d_fmt (bool wide) const {
        return _RWSTD_CHAR_ARRAY (d_fmt_off[wide]);
    }

    // %X: the appropriate time representation
    const void* t_fmt (bool wide) const {
        return _RWSTD_CHAR_ARRAY (t_fmt_off[wide]);
    }

    const void* t_fmt_ampm (bool wide) const {
        return _RWSTD_CHAR_ARRAY (t_fmt_ampm_off[wide]);
    }

    struct era_t {
        _RWSTD_UINT32_T name_off [2];   // narrow and wide era names
        _RWSTD_UINT32_T fmt_off [2];    // narrow and wide format of the year

        // negative offset represents direction of '-', otherwise '+'
        _RWSTD_INT32_T  offset;         // year closest to start_date

        // the beginning of time is represented by INT_MIN stored in
        // year [1], and CHAR_MIN in month [1] and day [1]
        // the end of time is represented by INT_MAX stored in
        // year [1], and CHAR_MAX in month [1] and day [1]
        _RWSTD_INT32_T  year [2];       // start and end year
        char            month [2];      // start and end month [0..11]
        char            day [2];        // start and end day [1..31]
    };

    // era description segment
    const era_t* era (_RWSTD_SIZE_T idx) const {
        return _RWSTD_ARRAY_ELM (era_t, era_off, idx);
    }

    // the number of era description segments
    _RWSTD_SIZE_T era_count () const {
        return num_eras;
    }

    const void* era_name (_RWSTD_SIZE_T idx, bool wide) const {
        return _RWSTD_CHAR_ARRAY (era (idx)->name_off[wide]);
    }

    const void* era_fmt (_RWSTD_SIZE_T idx, bool wide) const {
        return _RWSTD_CHAR_ARRAY (era (idx)->fmt_off[wide]);
    }

    // %Ec: the locale's alternate date and time representation
    const void* era_d_t_fmt (bool wide) const {
        return _RWSTD_CHAR_ARRAY (era_d_t_fmt_off[wide]);
    }

    // %Ex: the locale's alternative date representation
    const void* era_d_fmt (bool wide) const {
        return _RWSTD_CHAR_ARRAY (era_d_fmt_off[wide]);
    }

    // %EX: the locale's alternative time representation
    const void* era_t_fmt (bool wide) const {
        return _RWSTD_CHAR_ARRAY (era_t_fmt_off[wide]);
    }

    // alternative symbols for digits, corresponding to the %O modifier
    const void* alt_digits (_RWSTD_SIZE_T idx, bool wide) const {
        return _RWSTD_CHAR_ARRAY (*_RWSTD_ARRAY_ELM 
                                  (_RWSTD_UINT32_T, alt_digits_off, 
                                   idx * 2 + wide));
    }

    // number of alternative digits
    _RWSTD_UINT32_T alt_digits_count () const {
        return num_alt_digits;
    }

    const char* codeset_name () const {
        return _RWSTD_CHAR_ARRAY (codeset_off);
    }

    const char* charmap_name () const {
        return _RWSTD_CHAR_ARRAY (charmap_off);
    }

    const void* get_ext () const {
        return _RWSTD_CHAR_ARRAY (time_ext_off);
    }
};


#if 6 <= _RWSTD_aCC_MAJOR
   // restore HP aCC 6 remarks suppressed above to their default state
#  pragma diag_default 4298, 4299
#endif   // aCC >= 6


static inline _RWSTD_SIZE_T
__rw_itoutf8 (_RWSTD_UINT32_T wchar, char *to)
{
    typedef _RWSTD_UINT8_T _UChar;

    if (wchar < 0x80U) {
        to [0] = _UChar (wchar);
        return 1U;
    }
    if (wchar < 0x800U) {
        to [0] = _UChar (0xc0U | (wchar >> 6));
        to [1] = _UChar (0x80U | (wchar & 0x3fU));
        return 2U;
    }
    if (wchar < 0x10000U) {
        to [0] = _UChar (0xe0U | (wchar >> 12));
        to [1] = _UChar (0x80U | (wchar >>  6 & 0x3fU));
        to [2] = _UChar (0x80U | (wchar       & 0x3fU));
        return 3U;
    }
    if (wchar < 0x200000U) {
        to [0] = _UChar (0xf0U | (wchar >> 18));
        to [1] = _UChar (0x80U | (wchar >> 12 & 0x3fU));
        to [2] = _UChar (0x80U | (wchar >>  6 & 0x3fU));
        to [3] = _UChar (0x80U | (wchar       & 0x3fU));
        return 4U;
    }
    if (wchar < 0x4000000U) {
        to [0] = _UChar (0xf8U | (wchar >> 24));
        to [1] = _UChar (0x80U | (wchar >> 18 & 0x3fU));
        to [2] = _UChar (0x80U | (wchar >> 12 & 0x3fU));
        to [3] = _UChar (0x80U | (wchar >>  6 & 0x3fU));
        to [4] = _UChar (0x80U | (wchar       & 0x3fU));
        return 5U;
    }
    to [0] = _UChar (0xfcU | (wchar >> 30));
    to [1] = _UChar (0x80U | (wchar >> 24 & 0x3fU));
    to [2] = _UChar (0x80U | (wchar >> 18 & 0x3fU));
    to [3] = _UChar (0x80U | (wchar >> 12 & 0x3fU));
    to [4] = _UChar (0x80U | (wchar >>  6 & 0x3fU));
    to [5] = _UChar (0x80U | (wchar       & 0x3fU));
    return 6U;
}


// converts a single UTF-8 character starting at `from' to its UCS-4
// representation and, if successful, stores the result in `*ret'
// returns a pointer to the beginning of the next UTF-8 character
// or 0 on error
// returns `from' when the sequence in the range [`from', `from_end')
// forms an incomplete UTF-8 character
static inline const char*
__rw_utf8toucs4 (_RWSTD_INT32_T *ret, const char *from, const char *from_end)
{
    _RWSTD_ASSERT (0 != ret);
    _RWSTD_ASSERT (0 != from);
    _RWSTD_ASSERT (from <= from_end);

    typedef _RWSTD_INT32_T _Int32;

    const _RWSTD_UINT8_T* const byte =
        _RWSTD_REINTERPRET_CAST (const _RWSTD_UINT8_T*, from);

    if (byte [0] < 0x80U) {
        *ret = _Int32 (byte [0]);
        return from + 1;
    }

    if (byte [0] < 0xc2U) {
        // sequences beginning with a byte in the range [0x80, 0xc2)
        // do not form a valid UTF-8 character
        return 0;
    }

    const _RWSTD_PTRDIFF_T len = from_end - from;

    if (byte [0] < 0xe0U) {
        if (len < 2)
            return from;

        *ret = _Int32 ((byte [0] & 0x1fU) << 6 | byte [1] & 0x3fU);
        return from + 2;
    }

    if (byte [0] < 0xf0U) {
        if (len < 3)
            return from;

        *ret = _Int32 (  (byte [0] & 0x0fU) << 12
                       | (byte [1] & 0x3fU) <<  6
                       | (byte [2] & 0x3fU));
        return from + 3;
    }

    if (byte [0] < 0xf8U) {
        if (len < 4)
            return from;

        *ret = _Int32 (  (byte [0] & 0x07U) << 18
                       | (byte [1] & 0x3fU) << 12 
                       | (byte [2] & 0x3fU) <<  6
                       | (byte [3] & 0x3fU));
        return from + 4;
    }

    if (byte [0] < 0xfcU) {
        if (len < 5)
            return from;

        *ret = _Int32 (  (byte [0] & 0x03U) << 24
                       | (byte [1] & 0x3fU) << 18
                       | (byte [2] & 0x3fU) << 12
                       | (byte [3] & 0x3fU) <<  6 
                       | (byte [4] & 0x3fU));
        return from + 5;
    }

    if (byte [0] < 0xfeU) {
        if (len < 6)
            return from;

        *ret = _Int32 (  (byte [0] & 0x01U) << 30
                       | (byte [1] & 0x3fU) << 24 
                       | (byte [2] & 0x3fU) << 18 
                       | (byte [3] & 0x3fU) << 12 
                       | (byte [4] & 0x3fU) <<  6 
                       | (byte [5] & 0x3fU));
        return from + 6;
    }

    return 0;
}


// looks up the offset of the wide character corresponing to the multibyte
// character starting at `from'; returns the offset on success, UINT_MAX if
// no such wide character exists (i.e., on conversion error), or any value
// with bit 31 set if the the range [`from', `from_end') forms an incomplete
// multibyte character (i.e., if it's an initial subsequence of one)
static inline 
unsigned __rw_mbtowco (const unsigned *cvtbl,
                       const char     *&from,
                       const char     *from_end)
{
    typedef _RWSTD_UINT8_T _UChar;

    // `bit31' has the most significant bit set and all others clear
    const unsigned bit31 = 0x80000000U;

    const char *next = from;

    unsigned wc;

    for (const unsigned *ptbl = cvtbl; (wc = ptbl [_UChar (*next)]) & bit31; ) {

        // check if it is valid and if so, whether we have reached the
        // end of the source sequence and found an incomplete character
        // note, however, that an incomplete character does not mean 
        // partial conversion
        if (wc == _RWSTD_UINT_MAX || ++next == from_end)
            return wc;

        // use the `wc' value to compute the address of the next table
        ptbl = cvtbl + 256 * (wc & ~bit31);
    }

    from = next + 1;

    return wc;
}


}   // namespace __rw


#endif    // _RWSTD_LOC_LOCALEDEF_H_INCLUDED
