/***************************************************************************
 *
 * rw_iso2022.cpp
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#include <rw/_defs.h>

#include <string.h>     // for strxxx()
#include <iosfwd>       // for mbstate_t

#include <loc/_codecvt.h>
#include <loc/_localedef.h>

#include "iso2022.h"
#include "locale_body.h"


#define CHSET_ANSI_X_3_4_1968             6
#define CHSET_JIS_X_0201_KANA            13
#define CHSET_JIS_X_0201_ROMAN           14
#define CHSET_JIS_X_0208_1978            42
#define CHSET_JIS_X_0208_1983            87
#define CHSET_JIS_X_0212_1990           159
#define CHSET_GB2312_1980                58
#define CHSET_KSC_5601_1987             149
#define CHSET_ISO_8859_1                100
#define CHSET_ISO_8859_7                126

#define DBIDX_ANSI_X_3_4_1968             1
#define DBIDX_ISO_8859_1                  2
#define DBIDX_ISO_8859_2                  3
#define DBIDX_ISO_8859_3                  4
#define DBIDX_ISO_8859_4                  5
#define DBIDX_ISO_8859_5                  6
#define DBIDX_ISO_8859_6                  7
#define DBIDX_ISO_8859_7                  8
#define DBIDX_ISO_8859_8                  9
#define DBIDX_EUC_JP                     10
#define DBIDX_EUC_KR                     11
#define DBIDX_GB2312                     12

#define ISO_2022_JP_SET_ASCII              "\x1B(B"
#define ISO_2022_JP_SET_JIS201_ROMAN       "\x1B(J"
#define ISO_2022_JP_SET_JIS201_KANA        "\x1B(I"
#define ISO_2022_JP_SET_JIS208_78          "\x1B$@"
#define ISO_2022_JP_SET_JIS208_83          "\x1B$B"
#define ISO_2022_JP_SET_GB2312_80          "\x1B$A"
#define ISO_2022_JP_SET_KSC5601_87         "\x1B$(C"
#define ISO_2022_JP_SET_JIS212_90          "\x1B$(D"
#define ISO_2022_JP_SET_ISO_8859_1         "\x1B.A"
#define ISO_2022_JP_SET_ISO_8859_7         "\x1B.F"
#define ISO_2022_JP_SET_SS2                "\x1BN"


#define ASCII_CHAR_LEN                          1

#define EUCJP_ASCII_CHAR_LEN                    1
#define EUCJP_JISX0201_ROMAN_CHAR_LEN           1
#define EUCJP_JISX0201_KANA_CHAR_LEN            2
#define EUCJP_JISX0208_CHAR_LEN                 2
#define EUCJP_JISX0212_CHAR_LEN                 3

#define GB2312_CHAR_LEN                         2
#define KSC5601_CHAR_LEN                        2

#define ISO_8859_1_CHAR_LEN                     1
#define ISO_8859_7_CHAR_LEN                     1


#define ISO_2022_SET_SB94_G0                 0x28
#define ISO_2022_SET_SB94_G1                 0x29
#define ISO_2022_SET_SB94_G2                 0x2A
#define ISO_2022_SET_SB94_G3                 0x2B

#define ISO_2022_SET_SB96_G1                 0x2D
#define ISO_2022_SET_SB96_G2                 0x2E
#define ISO_2022_SET_SB96_G3                 0x2F

#define ISO_2022_SET_MB                      0x24

#define ISO_2022_SET_MB94_G0                 0x28
#define ISO_2022_SET_MB94_G1                 0x29
#define ISO_2022_SET_MB94_G2                 0x2A
#define ISO_2022_SET_MB94_G3                 0x2B

#define ISO_2022_SET_MB96_G1                 0x2D
#define ISO_2022_SET_MB96_G2                 0x2E
#define ISO_2022_SET_MB96_G3                 0x2F

#define ISO_2022_SET_SS2                     0x4E

#define ESCAPE_CHAR                          0x1B

#if !defined (_UTF8_MB_CUR_MAX)
#  define _UTF8_MB_CUR_MAX                      6
#endif


#define CODECVT_ERROR   _STD::codecvt_base::error
#define CODECVT_OK      _STD::codecvt_base::ok
#define CODECVT_PARTIAL _STD::codecvt_base::partial


_RWSTD_NAMESPACE (__rw) {


enum __rw_codeset_type_t {
    sb94 = 0, sb96, mb94, mb96
};

// State of the ISO-2022 conversions
struct __rw_iso2022_state_t {
    // 0 - unused, 1 - used, default 0
    unsigned used:1;

    // invoked code elements: valid values 1-4, invalid 0, default 0
    unsigned gl_map:3;
    unsigned gr_map:3;

    // shift state: 0 - one character, 1 - two characters, default 0
    unsigned shift_state:1;

    // single-shift 2 and 3 functions
    unsigned sshift2:1;
    unsigned sshift3:1;

    // equivalent of G0-G3 code elements
    unsigned char g_map [4];    // default 0
};

// Mapping between the code of the character set and the name
struct __rw_chset_map_t
{
    const char*   name;         // codeset canonical name
    unsigned char index;        // database index
    unsigned char width;        // encoding width
    const char*   seq;          // designating sequence
    unsigned int  seqlen;       // designating sequence length
};

// Database file mappings
struct __rw_db_map_t
{
    const void*   pv;           // pointer to mapping
    _RWSTD_SIZE_T size;         // size of mapping
    const char*   name;         // canonical name
};


// make sure there is just one copy of the empty string used
// in the table below (space optimization in case the compiler
// doesn't collapse them automatically
static const char __rw_empty[] = "";


// Array of mappings: code registration number to name
static const __rw_chset_map_t __rw_chset_map [256] = {

#define EMPTY   __rw_empty, 0, 0, 0, 0

    {/* 0x00 */ EMPTY }, {/* 0x01 */ EMPTY }, {/* 0x02 */ EMPTY },
    {/* 0x03 */ EMPTY }, {/* 0x04 */ EMPTY }, {/* 0x05 */ EMPTY },

    {
        /* 0x06 */ "ANSI_X3.4-1968", DBIDX_ANSI_X_3_4_1968, 0,
        ISO_2022_JP_SET_ASCII,
        sizeof (ISO_2022_JP_SET_ASCII) - 1U
    },

    {/* 0x07 */ EMPTY }, {/* 0x08 */ EMPTY }, {/* 0x09 */ EMPTY },
    {/* 0x0a */ EMPTY }, {/* 0x0b */ EMPTY }, {/* 0x0c */ EMPTY },

    {
        /* 0x0d */ "EUC-JP", DBIDX_EUC_JP, 0,
        ISO_2022_JP_SET_JIS201_KANA,
        sizeof (ISO_2022_JP_SET_JIS201_KANA) - 1U
    },

    {
        /* 0x0e */ "EUC-JP", DBIDX_EUC_JP, 0,
        ISO_2022_JP_SET_JIS201_ROMAN,
        sizeof (ISO_2022_JP_SET_JIS201_ROMAN) - 1U
    },

    {/* 0x0f */ EMPTY }, {/* 0x10 */ EMPTY }, {/* 0x11 */ EMPTY },
    {/* 0x12 */ EMPTY }, {/* 0x13 */ EMPTY }, {/* 0x14 */ EMPTY },
    {/* 0x15 */ EMPTY }, {/* 0x16 */ EMPTY }, {/* 0x17 */ EMPTY },
    {/* 0x18 */ EMPTY }, {/* 0x19 */ EMPTY }, {/* 0x1a */ EMPTY },
    {/* 0x1b */ EMPTY }, {/* 0x1c */ EMPTY }, {/* 0x1d */ EMPTY },
    {/* 0x1e */ EMPTY }, {/* 0x1f */ EMPTY }, {/* 0x20 */ EMPTY },
    {/* 0x21 */ EMPTY }, {/* 0x22 */ EMPTY }, {/* 0x23 */ EMPTY },
    {/* 0x24 */ EMPTY }, {/* 0x25 */ EMPTY }, {/* 0x26 */ EMPTY },
    {/* 0x27 */ EMPTY }, {/* 0x28 */ EMPTY }, {/* 0x29 */ EMPTY },

    {
        /* 0x2a */ "EUC-JP", DBIDX_EUC_JP, 1,
        ISO_2022_JP_SET_JIS208_78,
        sizeof (ISO_2022_JP_SET_JIS208_78) - 1U
    },

    {/* 0x2b */ EMPTY }, {/* 0x2c */ EMPTY }, {/* 0x2d */ EMPTY },
    {/* 0x2e */ EMPTY }, {/* 0x2f */ EMPTY }, {/* 0x30 */ EMPTY },
    {/* 0x31 */ EMPTY }, {/* 0x32 */ EMPTY }, {/* 0x33 */ EMPTY },
    {/* 0x34 */ EMPTY }, {/* 0x35 */ EMPTY }, {/* 0x36 */ EMPTY },
    {/* 0x37 */ EMPTY }, {/* 0x38 */ EMPTY }, {/* 0x39 */ EMPTY },

    {
        /* 0x3a */ "GB2312", DBIDX_GB2312, 1,
        ISO_2022_JP_SET_GB2312_80,
        sizeof (ISO_2022_JP_SET_GB2312_80) - 1U
    },

    {/* 0x3b */ EMPTY },
    {/* 0x3c */ EMPTY }, {/* 0x3d */ EMPTY }, {/* 0x3e */ EMPTY },
    {/* 0x3f */ EMPTY }, {/* 0x40 */ EMPTY }, {/* 0x41 */ EMPTY },
    {/* 0x42 */ EMPTY }, {/* 0x43 */ EMPTY }, {/* 0x44 */ EMPTY },
    {/* 0x45 */ EMPTY }, {/* 0x46 */ EMPTY }, {/* 0x47 */ EMPTY },
    {/* 0x48 */ EMPTY }, {/* 0x49 */ EMPTY }, {/* 0x4a */ EMPTY },
    {/* 0x4b */ EMPTY }, {/* 0x4c */ EMPTY }, {/* 0x4d */ EMPTY },
    {/* 0x4e */ EMPTY }, {/* 0x4f */ EMPTY }, {/* 0x50 */ EMPTY },
    {/* 0x51 */ EMPTY }, {/* 0x52 */ EMPTY }, {/* 0x53 */ EMPTY },
    {/* 0x54 */ EMPTY }, {/* 0x55 */ EMPTY }, {/* 0x56 */ EMPTY },

    {
        /* 0x57 */ "EUC-JP", DBIDX_EUC_JP, 1,
        ISO_2022_JP_SET_JIS208_83,
        sizeof (ISO_2022_JP_SET_JIS208_83) - 1U
    },

    {/* 0x58 */ EMPTY }, {/* 0x59 */ EMPTY }, {/* 0x5a */ EMPTY },
    {/* 0x5b */ EMPTY }, {/* 0x5c */ EMPTY }, {/* 0x5d */ EMPTY },
    {/* 0x5e */ EMPTY }, {/* 0x5f */ EMPTY }, {/* 0x60 */ EMPTY },
    {/* 0x61 */ EMPTY }, {/* 0x62 */ EMPTY }, {/* 0x63 */ EMPTY },

    {
        /* 0x64 */ "ISO-8859-1", DBIDX_ISO_8859_1, 0,
        ISO_2022_JP_SET_ISO_8859_1,
        sizeof (ISO_2022_JP_SET_ISO_8859_1) - 1U
    },

    {/* 0x65 */ EMPTY }, {/* 0x66 */ EMPTY }, {/* 0x67 */ EMPTY },
    {/* 0x68 */ EMPTY }, {/* 0x69 */ EMPTY }, {/* 0x6a */ EMPTY },
    {/* 0x6b */ EMPTY }, {/* 0x6c */ EMPTY }, {/* 0x6d */ EMPTY },
    {/* 0x6e */ EMPTY }, {/* 0x6f */ EMPTY }, {/* 0x70 */ EMPTY },
    {/* 0x71 */ EMPTY }, {/* 0x72 */ EMPTY }, {/* 0x73 */ EMPTY },
    {/* 0x74 */ EMPTY }, {/* 0x75 */ EMPTY }, {/* 0x76 */ EMPTY },
    {/* 0x77 */ EMPTY }, {/* 0x78 */ EMPTY }, {/* 0x79 */ EMPTY },
    {/* 0x7a */ EMPTY }, {/* 0x7b */ EMPTY }, {/* 0x7c */ EMPTY },
    {/* 0x7d */ EMPTY },

    {
        /* 0x7e */ "ISO-8859-7", DBIDX_ISO_8859_7, 0,
        ISO_2022_JP_SET_ISO_8859_7,
        sizeof (ISO_2022_JP_SET_ISO_8859_7) - 1U
    },

    {/* 0x7f */ EMPTY }, {/* 0x80 */ EMPTY }, {/* 0x81 */ EMPTY },
    {/* 0x82 */ EMPTY }, {/* 0x83 */ EMPTY }, {/* 0x84 */ EMPTY },
    {/* 0x85 */ EMPTY }, {/* 0x86 */ EMPTY }, {/* 0x87 */ EMPTY },
    {/* 0x88 */ EMPTY }, {/* 0x89 */ EMPTY }, {/* 0x8a */ EMPTY },
    {/* 0x8b */ EMPTY }, {/* 0x8c */ EMPTY }, {/* 0x8d */ EMPTY },
    {/* 0x8e */ EMPTY }, {/* 0x8f */ EMPTY }, {/* 0x90 */ EMPTY },
    {/* 0x91 */ EMPTY }, {/* 0x92 */ EMPTY }, {/* 0x93 */ EMPTY },
    {/* 0x94 */ EMPTY },

    {
        /* 0x95 */ "EUC-KR", DBIDX_EUC_KR, 1,
        ISO_2022_JP_SET_KSC5601_87,
        sizeof (ISO_2022_JP_SET_KSC5601_87) - 1U
    },

    {/* 0x96 */ EMPTY }, {/* 0x97 */ EMPTY }, {/* 0x98 */ EMPTY },
    {/* 0x99 */ EMPTY }, {/* 0x9a */ EMPTY }, {/* 0x9b */ EMPTY },
    {/* 0x9c */ EMPTY }, {/* 0x9d */ EMPTY }, {/* 0x9e */ EMPTY },

    {
        /* 0x9f */ "EUC-JP", DBIDX_EUC_JP, 1,
        ISO_2022_JP_SET_JIS212_90,
        sizeof (ISO_2022_JP_SET_JIS212_90) - 1U
    },

    {/* 0xa0 */ EMPTY }, {/* 0xa1 */ EMPTY }, {/* 0xa2 */ EMPTY },
    {/* 0xa3 */ EMPTY }, {/* 0xa4 */ EMPTY }, {/* 0xa5 */ EMPTY },
    {/* 0xa6 */ EMPTY }, {/* 0xa7 */ EMPTY }, {/* 0xa8 */ EMPTY },
    {/* 0xa9 */ EMPTY }, {/* 0xaa */ EMPTY }, {/* 0xab */ EMPTY },
    {/* 0xac */ EMPTY }, {/* 0xad */ EMPTY }, {/* 0xae */ EMPTY },
    {/* 0xaf */ EMPTY }, {/* 0xb0 */ EMPTY }, {/* 0xb1 */ EMPTY },
    {/* 0xb2 */ EMPTY }, {/* 0xb3 */ EMPTY }, {/* 0xb4 */ EMPTY },
    {/* 0xb5 */ EMPTY }, {/* 0xb6 */ EMPTY }, {/* 0xb7 */ EMPTY },
    {/* 0xb8 */ EMPTY }, {/* 0xb9 */ EMPTY }, {/* 0xba */ EMPTY },
    {/* 0xbb */ EMPTY }, {/* 0xbc */ EMPTY }, {/* 0xbd */ EMPTY },
    {/* 0xbe */ EMPTY }, {/* 0xbf */ EMPTY }, {/* 0xc0 */ EMPTY },
    {/* 0xc1 */ EMPTY }, {/* 0xc2 */ EMPTY }, {/* 0xc3 */ EMPTY },
    {/* 0xc4 */ EMPTY }, {/* 0xc5 */ EMPTY }, {/* 0xc6 */ EMPTY },
    {/* 0xc7 */ EMPTY }, {/* 0xc8 */ EMPTY }, {/* 0xc9 */ EMPTY },
    {/* 0xca */ EMPTY }, {/* 0xcb */ EMPTY }, {/* 0xcc */ EMPTY },
    {/* 0xcd */ EMPTY }, {/* 0xce */ EMPTY }, {/* 0xcf */ EMPTY },
    {/* 0xd0 */ EMPTY }, {/* 0xd1 */ EMPTY }, {/* 0xd2 */ EMPTY },
    {/* 0xd3 */ EMPTY }, {/* 0xd4 */ EMPTY }, {/* 0xd5 */ EMPTY },
    {/* 0xd6 */ EMPTY }, {/* 0xd7 */ EMPTY }, {/* 0xd8 */ EMPTY },
    {/* 0xd9 */ EMPTY }, {/* 0xda */ EMPTY }, {/* 0xdb */ EMPTY },
    {/* 0xdc */ EMPTY }, {/* 0xdd */ EMPTY }, {/* 0xde */ EMPTY },
    {/* 0xdf */ EMPTY }, {/* 0xe0 */ EMPTY }, {/* 0xe1 */ EMPTY },
    {/* 0xe2 */ EMPTY }, {/* 0xe3 */ EMPTY }, {/* 0xe4 */ EMPTY },
    {/* 0xe5 */ EMPTY }, {/* 0xe6 */ EMPTY }, {/* 0xe7 */ EMPTY },
    {/* 0xe8 */ EMPTY }, {/* 0xe9 */ EMPTY }, {/* 0xea */ EMPTY },
    {/* 0xeb */ EMPTY }, {/* 0xec */ EMPTY }, {/* 0xed */ EMPTY },
    {/* 0xee */ EMPTY }, {/* 0xef */ EMPTY }, {/* 0xf0 */ EMPTY },
    {/* 0xf1 */ EMPTY }, {/* 0xf2 */ EMPTY }, {/* 0xf3 */ EMPTY },
    {/* 0xf4 */ EMPTY }, {/* 0xf5 */ EMPTY }, {/* 0xf6 */ EMPTY },
    {/* 0xf7 */ EMPTY }, {/* 0xf8 */ EMPTY }, {/* 0xf9 */ EMPTY },
    {/* 0xfa */ EMPTY }, {/* 0xfb */ EMPTY }, {/* 0xfc */ EMPTY },
    {/* 0xfd */ EMPTY }, {/* 0xfe */ EMPTY }, {/* 0xff */ EMPTY }
};


// Mapping pointer - size of mapping - canonical name
static __rw_db_map_t __rw_db_map [32] = {
    { 0, 0, __rw_empty   }, { 0, 0, "ANSI_X3.4-1968" },
    { 0, 0, "ISO-8859-1" }, { 0, 0, "ISO-8859-2" },
    { 0, 0, "ISO-8859-3" }, { 0, 0, "ISO-8859-4" },
    { 0, 0, "ISO-8859-5" }, { 0, 0, "ISO-8859-6" },
    { 0, 0, "ISO-8859-7" }, { 0, 0, "ISO-8859-8" },
    { 0, 0, "EUC-JP" },     { 0, 0, "EUC-KR" },
    { 0, 0, "GB2312" },     { 0, 0, __rw_empty },
    { 0, 0, __rw_empty },   { 0, 0, __rw_empty },
    { 0, 0, __rw_empty },   { 0, 0, __rw_empty },
    { 0, 0, __rw_empty },   { 0, 0, __rw_empty },
    { 0, 0, __rw_empty },   { 0, 0, __rw_empty },
    { 0, 0, __rw_empty },   { 0, 0, __rw_empty },
    { 0, 0, __rw_empty },   { 0, 0, __rw_empty },
    { 0, 0, __rw_empty },   { 0, 0, __rw_empty },
    { 0, 0, __rw_empty },   { 0, 0, __rw_empty },
    { 0, 0, __rw_empty },   { 0, 0, __rw_empty }
};


// Retrieves the type of the encoding as indicated by the name
int __rw_encoding_from_name (const char* name)
{
    _RWSTD_ASSERT (0 != name);

    static const char pfx [] = "ISO-2022-";

    if (0 != strncmp (pfx, name, sizeof pfx - 1U))
        return stateless;

    name += sizeof pfx - 1U;

    if (0 == strcmp (name, "JP-2")) {
        return iso2022_jp2;
    }
    if (0 == strcmp (name, "JP")) {
        return iso2022_jp;
    }
    if (0 == strcmp (name, "KR")) {
        return iso2022_kr;
    }
    if (0 == strcmp (name, "CN")) {
        return iso2022_cn;
    }
    return stateless;
}


//  Array of ISO-2022 encoding states; the array has a default size of 256
//  entries; the declaration of the array is guarded for size
//
//  The usage of the structs in the array:
//  - the struct "allocated" are marked as  used;
//  - the index of the struct in the array is stored in the std::mbstate_t
//  struct  used in  codecvt facet;
//  - the  std::mbstate_t  values are shifted from  the real iso2022_state
//  struct index  by one  so  that a  value of  zero in  an std::mbstate_t
//  variable indicates a new conversion for which a state has to be
//  allocated
#if _RWSTD_MBSTATE_T_SIZE == 1
#  define ISO_2022_ARRAY_SIZE     255
#  define ISO_2022_STATE_INDEX_T  unsigned char
#else
#  define ISO_2022_ARRAY_SIZE     511
#  define ISO_2022_STATE_INDEX_T  unsigned short
#endif


// array of available states (limits the number of distinct mbstate_t
// objects with unique value that may be simultaneously valid)
static __rw_iso2022_state_t __rw_iso2022_states [ISO_2022_ARRAY_SIZE];


// returns an index in the array of state structures or -1 if none
// is available; ISO-2022-JP and ISO-2022-JP-2 assume different
// initializations
static inline
int __rw_allocate_state ()
{
    _RWSTD_MT_CLASS_GUARD (__rw_iso2022_state_t);

    for (int i = 0; i < ISO_2022_ARRAY_SIZE; i++)
        if (__rw_iso2022_states [i].used == 0) {
            __rw_iso2022_states [i].used = 1;
            return i;
        }

    return -1;
}


// deallocates state and makes it available for future conversions
// if `initial_only' is non-zero suceeds only if the `iso_state'
// argument represents an initial shift state
static inline
void __rw_deallocate_state (__rw_iso2022_state_t &iso_state,
                            _RWSTD_MBSTATE_T     &state,
                            bool                  initial_only)
{
    _RWSTD_MT_CLASS_GUARD (__rw_iso2022_state_t);

    if (initial_only) {
        // test the state object for return to initial shift state
        const int i = iso_state.gl_map - 1;
        _RWSTD_ASSERT (i >= 0 && i < 4);

        if (   iso_state.g_map [i] != CHSET_ANSI_X_3_4_1968
            || iso_state.sshift2
            || iso_state.sshift3)
            return;
        
        // proceed only if `iostate' is in an initial shift state
    }

    // zero out both structures
    memset (&state, 0, sizeof state);
    memset (&iso_state, 0, sizeof iso_state);
}


static __rw_iso2022_state_t*
__rw_get_iso2022_state (_RWSTD_MBSTATE_T& state, int enc)
{
    __rw_iso2022_state_t* pstate = 0;

    // retrieve the array index
    ISO_2022_STATE_INDEX_T* pc =
        _RWSTD_REINTERPRET_CAST (ISO_2022_STATE_INDEX_T*, &state);

    int n = *pc;

    _RWSTD_ASSERT(n >= 0 && n <= ISO_2022_ARRAY_SIZE);

    // a value of zero in std::mbstate_t indicates conversion anew,
    // calling for a state allocation
    if (n == 0) {
        n = __rw_allocate_state ();

        // the case when the allocation was not possible
        if (n == -1)
            return pstate;

        // store the state index
        *pc = _RWSTD_STATIC_CAST (ISO_2022_STATE_INDEX_T, n + 1);
    }
    else
        n--;

    // store the pointer to the state object
    pstate = &__rw_iso2022_states [n];

    // initialize the state according to the encoding type
    switch (enc) {
    case iso2022_jp:
    case iso2022_jp2:
        pstate->g_map [0] = CHSET_ANSI_X_3_4_1968;
        pstate->gl_map = 1;
        break;
    default:
        break;
    }

    return pstate;
}


// Retrieval of the character set's registration number; the code
// designator values overlap (they are, however, distinct in each category)
static inline
unsigned char __rw_sb94_encoding_reg (unsigned char c)
{
    unsigned char reg = 0;

    switch (c) {
    case 0x42:
        reg = CHSET_ANSI_X_3_4_1968;    /* ANSI X-3.4/1968     */
        break;
    case 0x49:
        reg = CHSET_JIS_X_0201_KANA;    /* JIS X 0201 Katakana */
        break;
    case 0x4a:
        reg = CHSET_JIS_X_0201_ROMAN;   /* JIS X 0201 Roman    */
        break;
    }

    return reg;
}


static inline
unsigned char __rw_sb96_encoding_reg (unsigned char c)
{
    unsigned char reg = 0;

    switch (c) {
    case 0x41:
        reg = CHSET_ISO_8859_1;         /* ISO-8859-1 */
        break;
    case 0x46:
        reg = CHSET_ISO_8859_7;         /* ISO-8859-7 */
        break;
    }

    return reg;
}


static inline
unsigned char __rw_mb94_encoding_reg (unsigned char c)
{
    unsigned char reg = 0;

    switch (c) {
    case 0x40:
        reg = CHSET_JIS_X_0208_1978;    /* JIS X 0208-1978 */
        break;
    case 0x41:
        reg = CHSET_GB2312_1980;        /* GB 2312-80      */
        break;
    case 0x42:
        reg = CHSET_JIS_X_0208_1983;    /* JIS X 0208-1983 */
        break;
    case 0x43:
        reg = CHSET_KSC_5601_1987;      /* KSC 5601 Korean */
        break;
    case 0x44:
        reg = CHSET_JIS_X_0212_1990;    /* JIS X 0212-1990 */
        break;
    }

    return reg;
}


static inline
unsigned char __rw_mb96_encoding_reg (unsigned char)
{
    return 0;
}


// Retrieves the code registration associated with a particular encoding class
// and a particular character set designator in that class
static inline
unsigned char __rw_get_encoding_reg (__rw_codeset_type_t type,
                                     unsigned char code)
{
    switch (type) {
    case sb94:
        return __rw_sb94_encoding_reg (code);
    case sb96:
        return __rw_sb96_encoding_reg (code);
    case mb94:
        return __rw_mb94_encoding_reg (code);
    case mb96:
        return __rw_mb96_encoding_reg (code);
    }

    return 0;
}


#if 0   // unused

// Retrieval of the character set's registration name
static inline
const char* __rw_get_encoding_name (unsigned char n)
{
    return (__rw_chset_map [n].name [0] == 0)?0:__rw_chset_map [n].name;
}


//  A way of  mapping a combination type-code to an index  in the array of
//  codecvt databases  mappings. The  databases mappings are  installed in
//  the array of mappings like this:
//    1  - ANSI.X-3.4-1968
//  2-9  - ISO-8859-1, 2, 3, 4, 5, 6, 7, 8
//   10  - EUC-JP
//   11  - EUC-KR
//   12  - GB2312-1980
static inline
unsigned char __rw_get_encoding_dbindex (__rw_codeset_type_t t,
                                         unsigned char code)
{
    unsigned char reg = __rw_get_encoding_reg (t, code);
    if (reg == 0)
        return 0;

    return __rw_chset_map [reg].index;
}

#endif   // 0/1


static inline
const void* __rw_get_encoding_database (unsigned char n)
{
    // if already mapped return it
    if (__rw_db_map [n].pv)
        return __rw_db_map [n].pv;

    // a wrong index would point to a struct bearing no name
    if (__rw_db_map [n].name [0] == 0)
        return 0;

    // if not, obtain it from __rw_get_facet_data
    int cat = __rw_get_cat ((__rw_facet::_C_wcodecvt_byname + 1) / 2);
    __rw_db_map [n].pv =
        __rw_get_facet_data (cat, __rw_db_map[n].size, 0, 
                             __rw_db_map [n].name);

    return __rw_db_map [n].pv;
}


// Insert designating sequence in the destination buffer; the encoding is
// given by the last parameter and is currently one of the following:
//   - ISO_2022-JP
//   - ISO_2022-JP2
static _STD::codecvt_base::result
__rw_iso2022jp_designate (__rw_iso2022_state_t& state,
                          char*&                to,
                          char*                 to_end,
                          unsigned char         reg,
                          int                   enc)
{
    // length of a designating sequence in ISO-2022-JP and
    // ISO-2022-JP-2 is 3 or 4 elements
    _RWSTD_SIZE_T len   = sizeof (ISO_2022_JP_SET_ASCII) - 1U;
    _RWSTD_SIZE_T sslen = 0;

    // register designation
    bool g2 = false;

    const char* esc = 0;
    const char* ss  = 0;

    switch (reg) {
    case CHSET_ANSI_X_3_4_1968:
        esc = ISO_2022_JP_SET_ASCII;
        break;
    case CHSET_JIS_X_0201_ROMAN:
        esc = ISO_2022_JP_SET_JIS201_ROMAN;
        break;
    case CHSET_JIS_X_0208_1978:
        esc = ISO_2022_JP_SET_JIS208_78;
        break;
    case CHSET_JIS_X_0208_1983:
        esc = ISO_2022_JP_SET_JIS208_83;
        break;
    case CHSET_GB2312_1980:
        esc = (enc == iso2022_jp)?0:ISO_2022_JP_SET_GB2312_80;
        break;
    case CHSET_ISO_8859_1:
        esc   = (enc == iso2022_jp)?0:ISO_2022_JP_SET_ISO_8859_1;
        ss    = ISO_2022_JP_SET_SS2;
        sslen = sizeof (ISO_2022_JP_SET_SS2) - 1U;
        g2    = true;
        break;
    case CHSET_ISO_8859_7:
        esc   = (enc == iso2022_jp)?0:ISO_2022_JP_SET_ISO_8859_7;
        ss    = ISO_2022_JP_SET_SS2;
        sslen = sizeof (ISO_2022_JP_SET_SS2) - 1U;
        g2    = true;
        break;

    // only JIS X 0212-1990 and KSC5601-1987 are 4 elements long
    case CHSET_JIS_X_0212_1990:
        if (enc != iso2022_jp) {
            esc = ISO_2022_JP_SET_JIS212_90;
            len = sizeof (ISO_2022_JP_SET_JIS212_90) - 1U;
        }
        break;

    case CHSET_KSC_5601_1987:
        if (enc != iso2022_jp) {
            esc = ISO_2022_JP_SET_KSC5601_87;
            len = sizeof (ISO_2022_JP_SET_KSC5601_87) - 1U;
        }
        break;

    default:
        return CODECVT_ERROR;
    }

    if (g2) {
        if (state.sshift2 == 1) {
            // there is already a correct designation and a single
            // shift function inserted in the destination buffer
            return CODECVT_OK;
        }

        // insert the designation
        if (reg != state.g_map [2]) {
            // FIXME - check correctness
            if (_RWSTD_STATIC_CAST (_RWSTD_SIZE_T, to_end - to) < len)
                return CODECVT_PARTIAL;

            if (len) {
                _RWSTD_ASSERT (to && esc);
                
                memcpy (to, esc, len);
                to += len;
            }

            // adjust the state
            state.g_map [2] = reg;
        }

        // insert the single shift function
        // FIXME - check correctness
        if (_RWSTD_STATIC_CAST (_RWSTD_SIZE_T, to_end - to) < len)
            return CODECVT_PARTIAL;

        if (sslen) {
            _RWSTD_ASSERT (to && ss);

            memcpy (to, ss, sslen);
            to += sslen;
        }

        // adjust the single shift indicator
        state.sshift2 = 1;
    } else {

        // FIXME - check correctness
        if (_RWSTD_STATIC_CAST (_RWSTD_SIZE_T, to_end - to) < len)
            return CODECVT_PARTIAL;

        if (len) {
            _RWSTD_ASSERT (to && esc);

            memcpy (to, esc, len);
            to += len;
        }

        // adjust the state
        state.g_map [0]   = reg;
        state.gl_map      = 1;
        state.shift_state = __rw_chset_map [reg].width;
    }
    return CODECVT_OK;
}

// Process one escape sequence in an ISO-2022 stream;
//
// Upon entry:
//   - from  - points to the first byte after the ESCAPE_CHAR indicator;
//   - enc   - encoding type.
static _STD::codecvt_base::result
__rw_iso2022_escape (__rw_iso2022_state_t& state,
                     const char*&          from,
                     const char*           from_end,
                     int                   enc)
{
    // copy of source pointer
    const char* ps = from + 1;

    // test for end of sequence
    if (from_end == ps)
        return CODECVT_OK;


    __rw_codeset_type_t type = sb94;
    switch (*ps) {
    case ISO_2022_SET_SS2:
        // Single-Shift 2 invokes G2 in GL for one character;
        // adjust the source pointer...
        from = ++ps;

        // ...and set the single-shift 2 flag in the state structure
        state.sshift2 = 1;
        return CODECVT_OK;

    case ISO_2022_SET_SB94_G0:
        type = sb94;
        break;

    case ISO_2022_SET_SB96_G2:
        type = sb96;
        break;

    case ISO_2022_SET_MB:
        type = mb94;

        // test for end of sequence
        if (from_end == ps + 1)
            return CODECVT_OK;

        // advance one more; this designation is 4 bytes long
        if (*(ps + 1) == ISO_2022_SET_MB94_G0)
            ps++;
        break;

    default:
        return CODECVT_ERROR;
    }

    if (from_end == ++ps)
        return CODECVT_OK;

    // retrieve the registration code and store the state
    const unsigned char reg = __rw_get_encoding_reg (type, *ps++);
    if (!reg)
        return CODECVT_ERROR;

    // check the registration code returned by the function above
    if (enc == iso2022_jp &&
        (reg == CHSET_JIS_X_0212_1990 || reg == CHSET_GB2312_1980 ||
         reg == CHSET_KSC_5601_1987   || reg == CHSET_ISO_8859_1  ||
         reg == CHSET_ISO_8859_7)) {
        return CODECVT_ERROR;
    }

    // designation is always in G0 unless the type is sb96
    state.g_map [type == sb96 ? 2 : 0] = reg;

    // however, the invocation is always in GL for ISO-2022-xxx!!
    state.gl_map = 1;
    state.shift_state = __rw_chset_map [reg].width;

    // adjust the source pointer
    from = ps;
    return CODECVT_OK;
}

/****************************************************************************/

// ISO-2022-JP conversion from ANSI_X3.4-1968
// Convert one character.
// Returns:
//    std::codecvt_base::ok      in case the conversion succeeded
//    std::codecvt_base::partial for partial conversions
//    std::codecvt_base::error   erroneous sequence
static _STD::codecvt_base::result
__rw_ascii_to_iso2022 (__rw_iso2022_state_t& state,
                       const char*&          from,
                       const char*           from_end,
                       char*&                to,
                       char*                 to_end,
                       int                   enc)
{
    // the registration
    unsigned char reg   = CHSET_ANSI_X_3_4_1968;
    unsigned int  width = __rw_chset_map [reg].width + 1;

    _RWSTD_ASSERT (from_end-from >= int(width));
    _RWSTD_UNUSED (from_end);

    _STD::codecvt_base::result ret;

    // check the designation sequence
    if (state.g_map [0] != reg) {
        char* pd = to;

        // insert a designation sequence in this place
        ret = __rw_iso2022jp_designate (state, pd, to_end, reg, enc);
        if (ret != CODECVT_OK)
            return ret;

        state.gl_map = 1;
        state.g_map [0] = reg;
        state.shift_state = __rw_chset_map [reg].width;

        // adjust destination pointer
        to = pd;

        if (to == to_end)
            return ret;
    }

    // FIXME - check the validity of this test
    if (to_end - to < int (width))
        return CODECVT_PARTIAL;

    *to++ = char (*from++ & 0x7f);
    if (width > 1)
        *to++ = char (*from++ & 0x7f);

    return CODECVT_OK;
}


// ISO-2022-JP conversion from KSC5601-1987
// Convert one character.
// Returns:
//    std::codecvt_base::ok      in case the conversion succeeded
//    std::codecvt_base::partial for partial conversions
//    std::codecvt_base::error   erroneous sequence
static _STD::codecvt_base::result
__rw_ksc5601_to_iso2022 (__rw_iso2022_state_t& state,
                         const char*&        from,
                         const char*         from_end,
                         char*&              to,
                         char*               to_end,
                         int                 enc)
{
    // the registration
    unsigned char reg   = CHSET_KSC_5601_1987;
    unsigned int  width = __rw_chset_map [reg].width + 1;

    _RWSTD_ASSERT(from_end-from >= int(width));
    _RWSTD_UNUSED(from_end);

    _STD::codecvt_base::result ret;

    // check the designation sequence
    if (state.g_map [0] != reg) {
        char* pd = to;

        // insert a designation sequence in this place
        ret = __rw_iso2022jp_designate (state, pd, to_end, reg, enc);
        if (ret != CODECVT_OK)
            return ret;

        state.gl_map = 1;
        state.g_map [0] = reg;
        state.shift_state = __rw_chset_map [reg].width;

        // adjust destination pointer
        to = pd;

        if (to == to_end)
            return ret;
    }

    // FIXME - check the validity of this test
    if (to_end - to < int (width))
        return CODECVT_PARTIAL;

    *to++ = char (*from++ & 0x7f);
    if (width > 1)
        *to++ = char (*from++ & 0x7f);

    return CODECVT_OK;
}


// ISO-2022-JP conversion from GB2312-1980
// Convert one character.
// Returns:
//    std::codecvt_base::ok      in case the conversion succeeded
//    std::codecvt_base::partial for partial conversions
//    std::codecvt_base::error   erroneous sequence
static _STD::codecvt_base::result
__rw_gb2312_to_iso2022 (__rw_iso2022_state_t& state,
                        const char*&          from,
                        const char*           from_end,
                        char*&                to,
                        char*                 to_end,
                        int                   enc)
{
    // the registration
    unsigned char reg   = CHSET_GB2312_1980;
    unsigned int  width = __rw_chset_map [reg].width + 1;

    _RWSTD_ASSERT(from_end-from >= int(width));
    _RWSTD_UNUSED(from_end);

    _STD::codecvt_base::result ret;

    // check the designation sequence
    if (state.g_map [0] != reg) {
        char* pd = to;

        // insert a designation sequence in this place
        ret = __rw_iso2022jp_designate (state, pd, to_end, reg, enc);
        if (ret != CODECVT_OK)
            return ret;

        state.gl_map = 1;
        state.g_map [0] = reg;
        state.shift_state = __rw_chset_map [reg].width;

        // adjust destination pointer
        to = pd;

        if (to == to_end)
            return ret;
    }

    // FIXME - check the validity of this test
    if (to_end - to < int(width))
        return CODECVT_PARTIAL;

    *to++ = char (*from++ & 0x7f);
    if (width > 1)
        *to++ = char (*from++ & 0x7f);

    return CODECVT_OK;
}


// ISO-2022-JP conversion from ISO-8859-7
// Convert one character.
// Returns:
//    std::codecvt_base::ok      in case the conversion succeeded
//    std::codecvt_base::partial for partial conversions
//    std::codecvt_base::error   erroneous sequence
static _STD::codecvt_base::result
__rw_iso88597_to_iso2022 (__rw_iso2022_state_t& state,
                          const char*&          from,
                          const char*           from_end,
                          char*&                to,
                          char*                 to_end,
                          int                   enc)
{
    // the registration
    unsigned char reg2   = CHSET_ISO_8859_7;
    unsigned int  width2 = __rw_chset_map [reg2].width + 1;

    _RWSTD_ASSERT(from_end-from >= int(width2));
    _RWSTD_UNUSED(from_end);

    _STD::codecvt_base::result ret;

    // always call designate for this one; designation will at least insert a
    // single shift function in the destination buffer
    ret = __rw_iso2022jp_designate (state, to, to_end, reg2, enc);
    if (ret != CODECVT_OK)
        return ret;

    // adjust the G2 designation
    state.g_map [2]   = reg2;

    // FIXME - check the validity of this test
    if (to_end - to < int (width2))
        return CODECVT_PARTIAL;

    *to++ = char (*from++ & 0x7f);
    if (width2 > 1)
        *to++ = char (*from++ & 0x7f);

    // clear the single shift functions
    state.sshift2 = state.sshift3 = 0;

    return CODECVT_OK;
}


// ISO-2022-JP conversion from ISO-8859-1
// Convert one character.
// Returns:
//    std::codecvt_base::ok      in case the conversion succeeded
//    std::codecvt_base::partial for partial conversions
//    std::codecvt_base::error   erroneous sequence
static _STD::codecvt_base::result
__rw_iso88591_to_iso2022 (__rw_iso2022_state_t& state,
                          const char*&          from,
                          const char*           from_end,
                          char*&                to,
                          char*                 to_end,
                          int                   enc)
{
    // the registration
    unsigned char reg2   = CHSET_ISO_8859_1;
    unsigned int  width2 = __rw_chset_map [reg2].width + 1;

    _RWSTD_ASSERT(from_end-from >= int(width2));
    _RWSTD_UNUSED(from_end);

    _STD::codecvt_base::result ret;

    // always call designate for this one; designation will at least insert a
    // single shift function in the destination buffer
    ret = __rw_iso2022jp_designate (state, to, to_end, reg2, enc);
    if (ret != CODECVT_OK)
        return ret;

    // adjust the G2 designation
    state.g_map [2]   = reg2;

    // FIXME - check the validity of this test
    if (to_end - to < int (width2))
        return CODECVT_PARTIAL;

    *to++ = char (*from++ & 0x7f);
    if (width2 > 1)
        *to++ = char (*from++ & 0x7f);

    // clear the single shift functions
    state.sshift2 = state.sshift3 = 0;

    return CODECVT_OK;
}


// ISO-2022-JP conversion from packed EUC-JP
// Convert one character.
// Returns:
//    std::codecvt_base::ok      in case the conversion succeeded
//    std::codecvt_base::partial for partial conversions
//    std::codecvt_base::error   erroneous sequence
static _STD::codecvt_base::result
__rw_eucjp_to_iso2022 (__rw_iso2022_state_t& state,
                       const char*&          from,
                       const char*           from_end,
                       char*&                to,
                       char*                 to_end,
                       int                   enc)
{
    int width = 0;
    unsigned char reg = 0;

    _STD::codecvt_base::result ret;

    // convert and store the encoding
    unsigned char c = *_RWSTD_REINTERPRET_CAST(const unsigned char*, from);
    switch (c) {
    case 0x8e:
        // JIS X 0201 Katakana
        reg   = CHSET_JIS_X_0201_KANA;
        from++;
        break;

    case 0x8f:
        // JIS X 0212 1990
        reg   = CHSET_JIS_X_0212_1990;
        from++;
        break;

    default:
        if (c > 0x80) {
            // JIS X 0208-1978
            reg   = CHSET_JIS_X_0208_1978;
        } else {
            // JIS X 0201 Roman/ANSI_X3.4-1968
            reg   = CHSET_ANSI_X_3_4_1968;
        }
        break;
    }

    // width in elements of the ISO-2022 output
    width = __rw_chset_map [reg].width + 1;

    // check the designation sequence
    if (state.g_map [0] != reg) {
        char* pd = to;

        // insert a designation sequence in this place
        ret = __rw_iso2022jp_designate (state, pd, to_end, reg, enc);
        if (ret != CODECVT_OK)
            return ret;

        // adjust destination pointer
        to = pd;

        state.gl_map = 1;
        state.g_map [0] = reg;
        state.shift_state = __rw_chset_map [reg].width;

        if (to == to_end)
            return ret;
    }

    // insert the character in the destination buffer
    if (width > from_end - from)
        return CODECVT_OK;

    // FIXME - check the validity of this test
    if (to_end - to < __rw_chset_map[reg].width)
        return CODECVT_PARTIAL;

    *to++ = char (*from++ & 0x7f);
    if (width > 1)
        *to++ = char (*from++ & 0x7f);

    return CODECVT_OK;
}


// ISO-2022-JP conversion to GB2312-1980
// Converts one character.
// Returns:
//    std::codecvt_base::ok      in case the conversion succeeded
//    std::codecvt_base::partial for partial conversions
//    std::codecvt_base::error   erroneous sequence
static inline
_STD::codecvt_base::result
__rw_iso2022_to_gb2312 (const char*& from,
                        const char*  from_end,
                        char*&       to,
                        int          /* reg */,
                        int          /* enc */)
{
    _STD::codecvt_base::result res = CODECVT_OK;

    if (from_end - from < GB2312_CHAR_LEN)
        return res;

    // store the destination bytes after setting the highest bit
    *to++ = char (*from++ | 0x80);
    *to++ = char (*from++ | 0x80);

    return res;
}


// ISO-2022-JP conversion to KSC2312
// Converts one character.
// Returns:
//    std::codecvt_base::ok      in case the conversion succeeded
//    std::codecvt_base::partial for partial conversions
//    std::codecvt_base::error   erroneous sequence
static inline
_STD::codecvt_base::result
__rw_iso2022_to_ksc5601 (const char*& from,
                         const char*  from_end,
                         char*&       to,
                         int          /* reg */,
                         int          /* enc */)
{
    _STD::codecvt_base::result res = CODECVT_OK;

    if (from_end - from < KSC5601_CHAR_LEN)
        return res;

    // store the destination bytes after setting the highest bit
    *to++ = char (*from++ | 0x80);
    *to++ = char (*from++ | 0x80);

    return res;
}


// ISO-2022-JP conversion to ISO-8859-1
// Converts one character.
// Returns:
//    std::codecvt_base::ok      in case the conversion succeeded
//    std::codecvt_base::partial for partial conversions
//    std::codecvt_base::error   erroneous sequence
static inline
_STD::codecvt_base::result
__rw_iso2022_to_iso88591 (const char*& from,
                          const char*  from_end,
                          char*&       to,
                          int          /* reg */,
                          int          /* enc */)
{
    _STD::codecvt_base::result res = CODECVT_OK;

    if (from_end - from < ISO_8859_1_CHAR_LEN)
        return res;

    // store the destination bytes after setting the highest bit
    *to++ = char (*from++ | 0x80);

    return res;
}


// ISO-2022-JP conversion to ISO-8859-7
// Converts one character.
// Returns:
//    std::codecvt_base::ok      in case the conversion succeeded
//    std::codecvt_base::partial for partial conversions
//    std::codecvt_base::error   erroneous sequence
static inline
_STD::codecvt_base::result
__rw_iso2022_to_iso88597 (const char*& from,
                          const char*  from_end,
                          char*&       to,
                          int          /* reg */,
                          int          /* enc */)
{
    _STD::codecvt_base::result res = CODECVT_OK;

    if (from_end - from < ISO_8859_7_CHAR_LEN)
        return res;

    // store the destination bytes after setting the highest bit
    *to++ = char (*from++ | 0x80);

    return res;
}


// ISO-2022-JP conversion to packed EUC-JP
// Converts one character.
// Returns:
//    std::codecvt_base::ok      in case the conversion succeeded
//    std::codecvt_base::partial for partial conversions
//    std::codecvt_base::error   erroneous sequence
static _STD::codecvt_base::result
__rw_iso2022_to_eucjp (const char*&          from,
                       const char*           from_end,
                       char*&                to,
                       int                   reg,
                       int                   enc)
{
    // encoding registration number
    int width = __rw_chset_map [reg].width + 1;

    // test the length of the input
    if (from_end - from < width)
        return CODECVT_OK;

    _STD::codecvt_base::result res = CODECVT_OK;

    // from either the ANSI_X3.4-1968 or JIS X 0201 Roman or JIS X 0201 Kana
    switch (reg) {
    case CHSET_ANSI_X_3_4_1968:
    case CHSET_JIS_X_0201_ROMAN:
        // these character sets encode on one byte in EUC-JP
        *to++ = *from++;

        break;

    case CHSET_JIS_X_0208_1978:
    case CHSET_JIS_X_0208_1983:
        *to++ = char (*from++ | 0x80);
        *to++ = char (*from++ | 0x80);

        break;

    // The following belong to ISO-2022-JP-2
    case CHSET_JIS_X_0201_KANA:
        if (enc == iso2022_jp) {
            res = CODECVT_ERROR;
            break;
        }

        // this character set encode on two bytes in EUC-JP
        *to++ = '\x8e';
        *to++ = char (*from++ | 0x80);

        break;

    case CHSET_JIS_X_0212_1990:
        if (enc == iso2022_jp) {
            res = CODECVT_ERROR;
            break;
        }

        *to++ = '\x8f';
        *to++ = char (*from++ | 0x80);
        *to++ = char (*from++ | 0x80);

        break;

    default:
        res = CODECVT_ERROR;
        break;
    }

    return res;
}


//  Converts  one  character  from  the  external  representation  to  the
//  intermediary encoding  that is later  used in retrieving  the internal
//  representation of that character
static _STD::codecvt_base::result
__rw_iso2022_to_interm (__rw_iso2022_state_t& state,
                        const char*&          from,
                        const char*           from_end,
                        char*&                to,
                        unsigned char&        reg,
                        int                   enc)
{
    _STD::codecvt_base::result res = CODECVT_OK;

    bool first = true;
    for (; *from == ESCAPE_CHAR; first = false) {
        // clear up the single shift functions set before
        if (first == false)
            state.sshift2 = state.sshift3 = 0;

        const char* tmp = from;
        res = __rw_iso2022_escape (state, from, from_end, enc);

        // the detected escape sequence has not been consumed because it
        // was incomplete or an error has been returned
        if (res != CODECVT_OK || tmp == from)
            return res;
    }

    if (*from <= 0x20) {
        ;// FIXME - process control characters
    }

    // if any single shift sequence has been selected, then clear up the
    // flag and adjust the registration code accordingly
    if (state.sshift2 == 1) {
        state.sshift2 = 0;
        // registration code switches to whatever code has been selected
        // in G2 (one of ISO-8859-1 and ISO-8859-7)
        reg = state.g_map [2];
    } else {
        // refresh the reg value
        reg = state.g_map [0];
    }

    // ISO-2022-JP and ISO-2022-JP-2 use G0 exclusively
    switch (reg) {
    case CHSET_ANSI_X_3_4_1968:
    case CHSET_JIS_X_0201_KANA:
    case CHSET_JIS_X_0201_ROMAN:
    case CHSET_JIS_X_0208_1978:
    case CHSET_JIS_X_0208_1983:
    case CHSET_JIS_X_0212_1990:
        res = __rw_iso2022_to_eucjp (from, from_end, to, reg, enc);
        break;

    case CHSET_GB2312_1980:
        res = __rw_iso2022_to_gb2312 (from, from_end, to, reg, enc);
        break;

    case CHSET_KSC_5601_1987:
        res = __rw_iso2022_to_ksc5601 (from, from_end, to, reg, enc);
        break;

    case CHSET_ISO_8859_1:
        res = __rw_iso2022_to_iso88591 (from, from_end, to, reg, enc);
        break;

    case CHSET_ISO_8859_7:
        res = __rw_iso2022_to_iso88597 (from, from_end, to, reg, enc);
        break;
    }

    // clear any single shift functions, if any
    state.sshift2 = state.sshift3 = 0;

    return CODECVT_OK;
}


static _STD::codecvt_base::result
__rw_ucs4_to_eucjp (const wchar_t*& from,
                    const wchar_t*  from_end,
                    char*&          to,
                    char*           to_end,
                    const _RW::__rw_codecvt_t* impl)
{
    typedef unsigned char UChar;

    // "impl" as raw pointer to unsigned ints
    const char* impl_raw =
        _RWSTD_REINTERPRET_CAST(const char*,impl);

    // utf8 temporary buffer
    char tmp [_UTF8_MB_CUR_MAX];
    char* ptmp = tmp;

    // tbls will point to the first lookup table
    const unsigned int* tbls = impl->utf8_to_ext_tab ();;
    const unsigned int* tbl  = tbls;
    _RWSTD_ASSERT(tbls);

    if (from == from_end || to == to_end)
        return CODECVT_OK;

    _RWSTD_SIZE_T ret = __rw_itoutf8 (*from, ptmp);

    unsigned int wc = tbl [UChar (*ptmp)];
    while (wc & 0x80000000) {
        if (wc == _RWSTD_UINT_MAX)
            // FIXME - this is an error condition
            return CODECVT_ERROR;

        if (ret-- == 0)
            // FIXME - this is an error condition
            return CODECVT_ERROR;

        // get next table
        wc &= 0x7fffffff;
        tbl = tbls + 256 * wc;
        wc = tbl [UChar (*++ptmp)];
    }

    // store the encoding sequence at destination
    _RWSTD_SIZE_T offset = wc + sizeof (_RW::__rw_codecvt_t);
    while (impl_raw [offset]) {
        if (to == to_end)
            return CODECVT_PARTIAL;
        else
            *to++ = impl_raw [offset++];
    }

    from++;
    return CODECVT_OK;
}


static unsigned char /* registration code */
__rw_ucs4_to_interm (  const wchar_t*& from,
                       char*&          to)
{
    typedef unsigned char UChar;

    static const struct {
        UChar db;
        UChar reg;
    } db_array [] = {
        { DBIDX_ANSI_X_3_4_1968, CHSET_ANSI_X_3_4_1968 },
        { DBIDX_EUC_JP         , CHSET_JIS_X_0212_1990 },
        { DBIDX_EUC_KR         , CHSET_KSC_5601_1987   },
        { DBIDX_GB2312         , CHSET_GB2312_1980     },
        { DBIDX_ISO_8859_7     , CHSET_ISO_8859_7      },
        { DBIDX_ISO_8859_1     , CHSET_ISO_8859_1      }
    };

    // utf8 temporary buffer
    char tmp [_UTF8_MB_CUR_MAX];

    for (size_t i = 0; i < sizeof db_array / sizeof *db_array; ++i) {
        char* ptmp = tmp;

        // obtain the database mapping
        const void* pdb = __rw_get_encoding_database (db_array [i].db);
        if (pdb == 0)
            continue;

        // cast the database map pointer to the correct type
        const _RW::__rw_codecvt_t* impl =
            _RWSTD_STATIC_CAST(const _RW::__rw_codecvt_t*, pdb);

        // the "raw access" pointer used in finding the external repr.
        const char* impl_raw = _RWSTD_REINTERPRET_CAST(const char*,impl);

        // tbls will point to the first lookup table
        const unsigned int* tbls = impl->utf8_to_ext_tab ();
        const unsigned int* tbl  = tbls;
        _RWSTD_ASSERT(tbls);

        _RWSTD_SIZE_T ret = __rw_itoutf8 (*from, ptmp);

        bool success = true;
        unsigned int wc = tbl [UChar (*ptmp)];
        while (wc & 0x80000000) {
            if (wc == _RWSTD_UINT_MAX || ret-- == 0) {
                success = false;
                break;
            }

            // get next table
            wc &= 0x7fffffff;
            tbl = tbls + 256 * wc;
            wc = tbl [UChar (*++ptmp)];
        }

        // continue on the next lookup
        if (!success)
            continue;

        // store the encoding sequence at destination
        _RWSTD_SIZE_T offset = wc + sizeof (_RW::__rw_codecvt_t);
        while (impl_raw [offset])
            *to++ = impl_raw [offset++];

        // advance source pointer to next position
        from++;

        // Find the registration code of the encoding and return it
        return db_array [i].reg;
    }

    // invalid registration code
    return 0;
}


// does the conversion of one character to internal representation
static _STD::codecvt_base::result
__rw_iso2022_to_ucs4 (_RWSTD_MBSTATE_T&     state,
                      __rw_iso2022_state_t* iso_state,
                      const char*&          from,
                      const char*           from_end,
                      wchar_t*&             to,
                      wchar_t*              /* to_end */,
                      int                   enc)
{
    _STD::codecvt_base::result res;

    // the registration code of the character set
    unsigned char reg   = iso_state->g_map [0];
    int           width = iso_state->shift_state + 1;

    // test input width
    if (from_end - from < width) {
        // test and deallocate state
        if (iso_state->g_map [0] == CHSET_ANSI_X_3_4_1968)
            __rw_deallocate_state (*iso_state, state, false);

        return CODECVT_OK;
    }

    // temporary buffer to accomodate the intermediary encoding
    char tmp_buffer [8];
    char* tmp_to = tmp_buffer;
    char* tmp_to_end;

    const char* from_next  = from;

    // convert one character at a time and note that the registration
    // code is actually modified by the following call in the case
    // of single shift functions
    res = __rw_iso2022_to_interm (*iso_state, from_next, from_end,
                                  tmp_to, reg, enc);
    if (res != CODECVT_OK ||
        (from_next == from && tmp_to == tmp_buffer)) {

        __rw_deallocate_state (*iso_state, state, CODECVT_ERROR != res);

        return res;
    }

    // the input sequence started with an escape sequence
    if (tmp_to == tmp_buffer) {
        from = from_next;
        return res;
    }

    tmp_to_end = tmp_to;
    tmp_to = tmp_buffer;

    // get the proper database for decoding input
    const void* pdb = __rw_get_encoding_database (__rw_chset_map [reg].index);
    if (pdb == 0) {
        __rw_deallocate_state (*iso_state, state, false);
        return CODECVT_ERROR;
    }
    const _RW::__rw_codecvt_t* pimpl =
        _RWSTD_STATIC_CAST(const _RW::__rw_codecvt_t*, pdb);

    // retrieve the lookup table start
    const unsigned int* tbls = pimpl->n_to_w_tab ();
    const unsigned int* tbl  = tbls;
    _RWSTD_ASSERT(tbls);

    // wider character representations will be looked upon in the
    // database tables
    unsigned int wc = tbl [*(unsigned char*)tmp_to];
    while (wc & 0x80000000) {
        if (wc == _RWSTD_UINT_MAX) {
            __rw_deallocate_state (*iso_state, state, false);
            return CODECVT_ERROR;
        }

        // wc is table index
        tbl = tbls + 256*(wc & 0x7fffffff);
        tmp_to++;

        // we have reached end of EUC-JP temporary buffer and we found
        // here an incomplete sequence; the incomplete sequence here
        // is an error
        if (tmp_to == tmp_to_end) {
            __rw_deallocate_state (*iso_state, state, false);
            return CODECVT_ERROR;
        }

        wc = tbl [*(unsigned char*)tmp_to];
    }

    // update source pointer and store the value at destination
    from = from_next;
    *to++ = pimpl->get_ucs4_at_offset(wc);

    return CODECVT_OK;
}

/****************************************************************************/

// Conversion from ISO-2022-JP to UCS-4
_STD::codecvt_base::result
__rw_iso2022jp_do_in (_RWSTD_MBSTATE_T& state,
                      const char*&      from,
                      const char*       from_end,
                      wchar_t*&         to,
                      wchar_t*          to_end)
{
    // the iso2022 state
    __rw_iso2022_state_t* iso_state =
        __rw_get_iso2022_state (state, iso2022_jp);

    if (iso_state == 0)
        return CODECVT_ERROR;

    // Loop until the source buffer is consumed, an error occurs, or
    // the destination buffer reaches capacity
    const char* from_next = from;
    wchar_t*    to_next   = to;

    while (from_end - from && to_end - to) {
        // operation result
        _STD::codecvt_base::result res;
        res = __rw_iso2022_to_ucs4 (state, iso_state, from_next,
                                    from_end, to_next, to_end, iso2022_jp);

        if (res != CODECVT_OK)
            return res;

        // an ok result with no conversion
        if (res == CODECVT_OK &&
            from_next == from && to_next == to)
            return res;

        from = from_next;
        to   = to_next;
    }

    // deallocate state if the conversion has gone back to initial state
    __rw_deallocate_state (*iso_state, state, true);

    return CODECVT_OK;
}


// Conversion from UCS-4 to ISO-2022-JP
_STD::codecvt_base::result
__rw_iso2022jp_do_out (_RWSTD_MBSTATE_T& state,
                       const wchar_t*&   from,
                       const wchar_t*    from_end,
                       char*&            to,
                       char*             to_end)
{
    // the iso2022 state
    __rw_iso2022_state_t* iso_state =
        __rw_get_iso2022_state (state, iso2022_jp);

    if (iso_state == 0)
        return CODECVT_ERROR;

    // destination buffer to accomodate the EUC-JP encoding
    char euc_buffer  [8];

    // use the EUC-JP database for a full lookup
    const void* pdb = __rw_get_encoding_database (DBIDX_EUC_JP);
    if (pdb == 0){
        __rw_deallocate_state (*iso_state, state, false);
        return CODECVT_ERROR;
    }

    const _RW::__rw_codecvt_t* pimpl =
        _RWSTD_STATIC_CAST(const _RW::__rw_codecvt_t*, pdb);
    _RWSTD_ASSERT(pimpl);

    // Loop until the source buffer is consumed, an error occurs, or
    // the destination buffer reaches capacity
    while (from_end - from && to_end - to) {
        _STD::codecvt_base::result res;

        // convert the UCS-4 value to EUC-JP
        const wchar_t* ps   = from;
        const wchar_t* pse  = from_end;

        char* euc   = euc_buffer;
        char* euce  = euc_buffer + 8;

        res = __rw_ucs4_to_eucjp (ps, pse, euc, euce, pimpl);
        if (res != CODECVT_OK ||
            (ps == from && euc == euc_buffer)) {

            __rw_deallocate_state (*iso_state, state, CODECVT_ERROR != res);

            return res;
        }

        char* pd = to;

        // convert the EUC-JP value to ISO-2022
        euce = euc;
        const char* ceuc = euc_buffer;
        res  = __rw_eucjp_to_iso2022 (*iso_state, ceuc,
                                      euce, pd, to_end, iso2022_jp);
        if (res != CODECVT_OK ||
            (ceuc == euc_buffer && pd == to)) {

            __rw_deallocate_state (*iso_state, state, CODECVT_ERROR != res);

            return res;
        }

        //adjust source pointer
        from = ps;
        to   = pd;
    }

    // deallocate state if the conversion has gone back to
    // initial state
    __rw_deallocate_state (*iso_state, state, true);

    return CODECVT_OK;
}


_STD::codecvt_base::result
__rw_iso2022jp_do_unshift (_RWSTD_MBSTATE_T& state,
                           char*& to, char* to_end)
{
    _STD::codecvt_base::result res =
        CODECVT_ERROR;

    // the iso2022 state
    __rw_iso2022_state_t* iso_state =
        __rw_get_iso2022_state (state, iso2022_jp);

    if (iso_state == 0)
        return res;

    if (iso_state->g_map [iso_state->gl_map - 1] != CHSET_ANSI_X_3_4_1968)
        res = __rw_iso2022jp_designate (*iso_state, to, to_end,
                                        CHSET_ANSI_X_3_4_1968, iso2022_jp);
    else
        res = CODECVT_OK;

    // deallocate a state if the conversion has gone back to
    // initial shift state and operation succeeded, OR, if
    // the result is error
    __rw_deallocate_state (*iso_state, state, CODECVT_ERROR != res);

    return res;
}


_RWSTD_SIZE_T
__rw_iso2022jp_do_length (_RWSTD_MBSTATE_T& state,
                          const char* from, const char* from_end,
                          _RWSTD_SIZE_T max)
{
    _RWSTD_ASSERT(from <= from_end);

    // the iso2022 state
    __rw_iso2022_state_t* iso_state =
        __rw_get_iso2022_state (state, iso2022_jp);

    if (iso_state == 0)
        return CODECVT_ERROR;

    int ret = 0;
    _STD::codecvt_base::result res = CODECVT_OK;
    while (max && from_end - from) {
        while (*from == ESCAPE_CHAR)


        if (*from == ESCAPE_CHAR) {
            const char* tmp = from;
            res = __rw_iso2022_escape (*iso_state, from, from_end, iso2022_jp);

            if (res != CODECVT_OK || from == tmp)
                break;

            continue;
        }

        if (from_end - from < int(iso_state->shift_state) + 1)
            break;

        ret++, max--;
        from += iso_state->shift_state + 1;
    }

    // deallocate a state if the conversion has gone back to initial shift
    // state
    __rw_deallocate_state (*iso_state, state, CODECVT_ERROR != res);

    return ret;
}


_RWSTD_SIZE_T
__rw_iso2022jp_do_max_length ()
{
    return 2;
}


int __rw_iso2022jp_do_encoding ()
{
    return -1;
}

bool __rw_iso2022jp_do_always_noconv ()
{
    return false;
}

/****************************************************************************/

// Conversion from ISO-2022-JP to UCS-4
_STD::codecvt_base::result
__rw_iso2022jp2_do_in (_RWSTD_MBSTATE_T& state,
                       const char*&      from,
                       const char*       from_end,
                       wchar_t*&         to,
                       wchar_t*          to_end)
{
    // the iso2022 state
    __rw_iso2022_state_t* iso_state =
        __rw_get_iso2022_state (state, iso2022_jp);

    if (iso_state == 0)
        return CODECVT_ERROR;

    // Loop until the source buffer is consumed, an error occurs, or
    // the destination buffer reaches capacity
    while (from_end - from && to_end - to) {

        const char* from_next = from;
        wchar_t*    to_next   = to;

        // operation result
        _STD::codecvt_base::result res;
        res = __rw_iso2022_to_ucs4 (state, iso_state, from_next,
                                    from_end, to_next, to_end, iso2022_jp2);

        if (res != CODECVT_OK)
            return res;

        // an ok result with no conversion
        if (res == CODECVT_OK &&
            from_next == from && to_next == to)
            return res;

        from = from_next;
        to   = to_next;
    }

    // deallocate state if the conversion has gone back to initial state
    __rw_deallocate_state (*iso_state, state, true);

    return CODECVT_OK;
}


// Conversion from UCS-4 to ISO-2022-JP-2
_STD::codecvt_base::result
__rw_iso2022jp2_do_out (_RWSTD_MBSTATE_T& state,
                       const wchar_t*&    from,
                       const wchar_t*     from_end,
                       char*&             to,
                       char*              to_end)
{
    // the iso2022 state
    __rw_iso2022_state_t* iso_state =
        __rw_get_iso2022_state (state, iso2022_jp);

    if (iso_state == 0)
        return CODECVT_ERROR;

    // destination buffer to accomodate the intermediate encoding
    char tmp  [8];

    // Loop until the source buffer is consumed, an error occurs, or
    // the destination buffer reaches capacity
    while (from_end - from && to_end - to) {
        _STD::codecvt_base::result res;

        // convert the UCS-4 value to intermediary encoding
        const wchar_t* ps   = from;

        char* tmps  = tmp;
        char* tmpe  = tmp + 8;

        unsigned char reg =
            __rw_ucs4_to_interm (ps, tmps);
        if (reg == 0) {
            // deallocate state
            __rw_deallocate_state (*iso_state, state, false);

            // error condition - none of the databases contained a mapping
            // of the character
            return CODECVT_ERROR;
        }

        char* pd = to;

        // convert the intermediary value to ISO-2022
        tmpe = tmps;
        const char* ctmps = tmp;

        // the registration of the encoding dictates how the conversion is
        // to be performed further
        switch (reg) {
        case CHSET_ANSI_X_3_4_1968:
            res = __rw_ascii_to_iso2022     (*iso_state, ctmps, tmpe,
                                             pd, to_end, iso2022_jp2);
            break;
        case CHSET_JIS_X_0212_1990:
            res = __rw_eucjp_to_iso2022     (*iso_state, ctmps, tmpe,
                                             pd, to_end, iso2022_jp2);
            break;
        case CHSET_KSC_5601_1987:
            res = __rw_ksc5601_to_iso2022   (*iso_state, ctmps, tmpe,
                                             pd, to_end, iso2022_jp2);
            break;
        case CHSET_GB2312_1980:
            res = __rw_gb2312_to_iso2022    (*iso_state, ctmps, tmpe,
                                             pd, to_end, iso2022_jp2);
            break;
        case CHSET_ISO_8859_7:
            res = __rw_iso88597_to_iso2022  (*iso_state, ctmps, tmpe,
                                             pd, to_end, iso2022_jp2);
            break;
        case CHSET_ISO_8859_1:
            res = __rw_iso88591_to_iso2022  (*iso_state, ctmps, tmpe,
                                             pd, to_end, iso2022_jp2);
            break;
        default:
            return CODECVT_ERROR;
        }

        if (res != CODECVT_OK || (ctmps==tmp && pd==to)) {

            __rw_deallocate_state (*iso_state, state, CODECVT_ERROR != res);

            return res;
        }

        //adjust source pointer
        from = ps;
        to   = pd;

        // after a successful insertion the single shift functions
        // must be cleared
        iso_state->sshift2 = iso_state->sshift3 = 0;

    }

    // deallocate state if the conversion has gone back to
    // initial state
    __rw_deallocate_state (*iso_state, state, true);

    return CODECVT_OK;
}


_STD::codecvt_base::result
__rw_iso2022jp2_do_unshift (_RWSTD_MBSTATE_T& state,
                            char*& to, char* to_end)
{
    return __rw_iso2022jp_do_unshift (state, to, to_end);
}


_RWSTD_SIZE_T
__rw_iso2022jp2_do_length (_RWSTD_MBSTATE_T& state,
                           const char* from, const char* from_end,
                           _RWSTD_SIZE_T max)
{
    _RWSTD_ASSERT(from <= from_end);

    // the iso2022 state
    __rw_iso2022_state_t* iso_state =
        __rw_get_iso2022_state (state, iso2022_jp);

    if (iso_state == 0)
        return CODECVT_ERROR;

    int ret = 0;
    _STD::codecvt_base::result res = CODECVT_OK;

    while (max && from_end - from) {

        bool first = true;
        for (; *from == ESCAPE_CHAR; first = false) {
            // clear up the single shift functions set before
            if (first == false)
                iso_state->sshift2 = iso_state->sshift3 = 0;

            const char* tmp = from;
            res = __rw_iso2022_escape (*iso_state, from, from_end,
                                       iso2022_jp2);

            // the detected escape sequence has not been consumed because it
            // was incomplete
            if (res != CODECVT_OK || tmp == from)
                return ret;
        }

        if (*from <= 0x20) {
            ;// FIXME - process control characters
        }

        // the registration for all character sets other than the ISO's
        // is stored in the first register, G0.
        unsigned char reg = iso_state->g_map [0];
        unsigned int  width;
        if (iso_state->sshift2 == 1) {
            width = 1;
        } else {
            width = __rw_chset_map [reg].width + 1;
        }

        if (from_end - from < int (width))
            break;

        ret++, max--;
        from += width;

        // clean up the single shift function
        iso_state->sshift2 = iso_state->sshift3 = 0;

    }

    // deallocate a state if the conversion has gone back to initial shift
    // state
    __rw_deallocate_state (*iso_state, state, CODECVT_ERROR != res);

    return ret;
}


_RWSTD_SIZE_T
__rw_iso2022jp2_do_max_length ()
{
    return 2;
}


int __rw_iso2022jp2_do_encoding ()
{
    return -1;
}


bool __rw_iso2022jp2_do_always_noconv ()
{
    return false;
}

}   // namespace __rw
