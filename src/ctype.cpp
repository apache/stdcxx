/***************************************************************************
 *
 * ctype.cpp - source for the C++ Standard Library ctype facets
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

#define _RWSTD_LIB_SRC

#include <rw/_defs.h>

#include <ctype.h>
#include <limits.h>
#include <locale.h>
#include <wctype.h>
#include <string.h>  // for memset()

#include <loc/_ctype.h>
#include <loc/_locale.h>
#include <loc/_localedef.h>

#include "access.h"
#include "setlocale.h"
#include "use_facet.h"


_RWSTD_NAMESPACE (__rw) {

// table of character classes
extern const _STD::ctype_base::mask
__rw_classic_tab [_STD::ctype<char>::table_size] = {

#undef  MASK
#define MASK(x)   _STD::ctype_base::mask (x)

#undef  LETTER
#define LETTER(m) MASK (__rw_print | __rw_alpha | __rw_graph | m)

#if 'A' == 0x41

    /* 0x00 */ __rw_cntrl,
    /* 0x01 */ __rw_cntrl,
    /* 0x02 */ __rw_cntrl,
    /* 0x03 */ __rw_cntrl,
    /* 0x04 */ __rw_cntrl,
    /* 0x05 */ __rw_cntrl,
    /* 0x06 */ __rw_cntrl,
    /* 0x07 */ __rw_cntrl,
    /* 0x08 */ __rw_cntrl,
    /* 0x09 */ MASK (__rw_space | __rw_cntrl),
    /* 0x0a */ MASK (__rw_space | __rw_cntrl),
    /* 0x0b */ MASK (__rw_space | __rw_cntrl),
    /* 0x0c */ MASK (__rw_space | __rw_cntrl),
    /* 0x0d */ MASK (__rw_space | __rw_cntrl),
    /* 0x0e */ __rw_cntrl,
    /* 0x0f */ __rw_cntrl,
    /* 0x10 */ __rw_cntrl,
    /* 0x11 */ __rw_cntrl,
    /* 0x12 */ __rw_cntrl,
    /* 0x13 */ __rw_cntrl,
    /* 0x14 */ __rw_cntrl,
    /* 0x15 */ __rw_cntrl,
    /* 0x16 */ __rw_cntrl,
    /* 0x17 */ __rw_cntrl,
    /* 0x18 */ __rw_cntrl,
    /* 0x19 */ __rw_cntrl,
    /* 0x1a */ __rw_cntrl,
    /* 0x1b */ __rw_cntrl,
    /* 0x1c */ __rw_cntrl,
    /* 0x1d */ __rw_cntrl,
    /* 0x1e */ __rw_cntrl,
    /* 0x1f */ __rw_cntrl,
    /* ' '  */ MASK (__rw_space | __rw_print),
    /* '!'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '"'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '#'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '$'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '%'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '&'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '\'' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '('  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* ')'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '*'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '+'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* ','  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '-'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '.'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '/'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '0'  */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '1'  */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '2'  */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '3'  */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '4'  */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '5'  */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '6'  */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '7'  */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '8'  */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '9'  */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* ':'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* ';'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '<'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '='  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '>'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '?'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '@'  */ MASK (__rw_print | __rw_punct | __rw_graph),

    /* 'A'  */ LETTER (__rw_upper | __rw_xdigit),
    /* 'B'  */ LETTER (__rw_upper | __rw_xdigit),
    /* 'C'  */ LETTER (__rw_upper | __rw_xdigit),
    /* 'D'  */ LETTER (__rw_upper | __rw_xdigit),
    /* 'E'  */ LETTER (__rw_upper | __rw_xdigit),
    /* 'F'  */ LETTER (__rw_upper | __rw_xdigit),
    /* 'G'  */ LETTER (__rw_upper),
    /* 'H'  */ LETTER (__rw_upper),
    /* 'I'  */ LETTER (__rw_upper),
    /* 'J'  */ LETTER (__rw_upper),
    /* 'K'  */ LETTER (__rw_upper),
    /* 'L'  */ LETTER (__rw_upper),
    /* 'M'  */ LETTER (__rw_upper),
    /* 'N'  */ LETTER (__rw_upper),
    /* 'O'  */ LETTER (__rw_upper),
    /* 'P'  */ LETTER (__rw_upper),
    /* 'Q'  */ LETTER (__rw_upper),
    /* 'R'  */ LETTER (__rw_upper),
    /* 'S'  */ LETTER (__rw_upper),
    /* 'T'  */ LETTER (__rw_upper),
    /* 'U'  */ LETTER (__rw_upper),
    /* 'V'  */ LETTER (__rw_upper),
    /* 'W'  */ LETTER (__rw_upper),
    /* 'X'  */ LETTER (__rw_upper),
    /* 'Y'  */ LETTER (__rw_upper),
    /* 'Z'  */ LETTER (__rw_upper),
    /* '['  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '\\' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* ']'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '^'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '_'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '`'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* 'a'  */ LETTER (__rw_lower | __rw_xdigit),
    /* 'b'  */ LETTER (__rw_lower | __rw_xdigit),
    /* 'c'  */ LETTER (__rw_lower | __rw_xdigit),
    /* 'd'  */ LETTER (__rw_lower | __rw_xdigit),
    /* 'e'  */ LETTER (__rw_lower | __rw_xdigit),
    /* 'f'  */ LETTER (__rw_lower | __rw_xdigit),
    /* 'g'  */ LETTER (__rw_lower),
    /* 'h'  */ LETTER (__rw_lower),
    /* 'i'  */ LETTER (__rw_lower),
    /* 'j'  */ LETTER (__rw_lower),
    /* 'k'  */ LETTER (__rw_lower),
    /* 'l'  */ LETTER (__rw_lower),
    /* 'm'  */ LETTER (__rw_lower),
    /* 'n'  */ LETTER (__rw_lower),
    /* 'o'  */ LETTER (__rw_lower),
    /* 'p'  */ LETTER (__rw_lower),
    /* 'q'  */ LETTER (__rw_lower),
    /* 'r'  */ LETTER (__rw_lower),
    /* 's'  */ LETTER (__rw_lower),
    /* 't'  */ LETTER (__rw_lower),
    /* 'u'  */ LETTER (__rw_lower),
    /* 'v'  */ LETTER (__rw_lower),
    /* 'w'  */ LETTER (__rw_lower),
    /* 'x'  */ LETTER (__rw_lower),
    /* 'y'  */ LETTER (__rw_lower),
    /* 'z'  */ LETTER (__rw_lower),
    /* '{'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '|'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '}'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '~'  */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* 0x7f */ __rw_cntrl

#elif 'A' == 0xc1

    /* NUL */ __rw_cntrl,
    /* SOH */ __rw_cntrl,
    /* STX */ __rw_cntrl,
    /* ETX */ __rw_cntrl,
    /* PF  */ __rw_cntrl,
    /* HT  */ __rw_cntrl,
    /* LC  */ __rw_cntrl,
    /* DEL */ __rw_cntrl,
    /*     */ MASK (0),
    /*     */ MASK (0),
    /* SMM */ __rw_cntrl,
    /* VT  */ __rw_cntrl,
    /* FF  */ __rw_cntrl,
    /* CR  */ __rw_cntrl,
    /* SO  */ __rw_cntrl,
    /* SI  */ __rw_cntrl,
    /* DLE */ __rw_cntrl,
    /* DC1 */ __rw_cntrl,
    /* DC2 */ __rw_cntrl,
    /* TM  */ __rw_cntrl,
    /* RES */ __rw_cntrl,
    /* NL  */ __rw_cntrl,
    /* BS  */ __rw_cntrl,
    /* IL  */ __rw_cntrl,
    /* CAN */ __rw_cntrl,
    /* EM  */ __rw_cntrl,
    /* CC  */ __rw_cntrl,
    /* CU1 */ __rw_cntrl,
    /* IFS */ __rw_cntrl,
    /* IGS */ __rw_cntrl,
    /* IRS */ __rw_cntrl,
    /* IUS */ __rw_cntrl,
    /* DS  */ __rw_cntrl,
    /* SOS */ __rw_cntrl,
    /* FS  */ __rw_cntrl,
    /*     */ MASK (0),
    /* BYP */ __rw_cntrl,
    /* LF  */ __rw_cntrl,
    /* ETB */ __rw_cntrl,
    /* ESC */ __rw_cntrl,
    /*     */ MASK (0),
    /*     */ MASK (0),
    /* SM  */ __rw_cntrl,
    /* CU2 */ __rw_cntrl,
    /*     */ MASK (0),
    /* ENQ */ __rw_cntrl,
    /* ACK */ __rw_cntrl,
    /* BEL */ __rw_cntrl,
    /*     */ MASK (0),
    /*     */ MASK (0),
    /* SYN */ __rw_cntrl,
    /*     */ MASK (0),
    /* PN  */ __rw_cntrl,
    /* RS  */ __rw_cntrl,
    /* UC  */ __rw_cntrl,
    /* EOT */ __rw_cntrl,
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /* CU3 */ __rw_cntrl,
    /* DC4 */ __rw_cntrl,
    /* NAK */ __rw_cntrl,
    /*     */ MASK (0),
    /* SUB */ __rw_cntrl,
    /* SP  */ __rw_cntrl,
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /* ct. */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '.' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '<' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '(' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '+' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '|' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '&' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /* '!' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '$' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '*' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* ')' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* ';' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '~' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '-' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '/' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /* ',' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '%' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '_' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '>' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '?' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /* ':' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '#' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '@' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* ''' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '=' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /* '"' */ MASK (__rw_print | __rw_punct | __rw_graph),
    /*     */ MASK (0),
    /* 'a' */ LETTER (__rw_lower | __rw_xdigit),
    /* 'b' */ LETTER (__rw_lower | __rw_xdigit),
    /* 'c' */ LETTER (__rw_lower | __rw_xdigit),
    /* 'd' */ LETTER (__rw_lower | __rw_xdigit),
    /* 'e' */ LETTER (__rw_lower | __rw_xdigit),
    /* 'f' */ LETTER (__rw_lower | __rw_xdigit),
    /* 'g' */ LETTER (__rw_lower),
    /* 'h' */ LETTER (__rw_lower),
    /* 'i' */ LETTER (__rw_lower),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /* 'j' */ LETTER (__rw_lower),
    /* 'k' */ LETTER (__rw_lower),
    /* 'l' */ LETTER (__rw_lower),
    /* 'm' */ LETTER (__rw_lower),
    /* 'n' */ LETTER (__rw_lower),
    /* 'o' */ LETTER (__rw_lower),
    /* 'p' */ LETTER (__rw_lower),
    /* 'q' */ LETTER (__rw_lower),
    /* 'r' */ LETTER (__rw_lower),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /* 's' */ LETTER (__rw_lower),
    /* 't' */ LETTER (__rw_lower),
    /* 'u' */ LETTER (__rw_lower),
    /* 'v' */ LETTER (__rw_lower),
    /* 'w' */ LETTER (__rw_lower),
    /* 'x' */ LETTER (__rw_lower),
    /* 'y' */ LETTER (__rw_lower),
    /* 'z' */ LETTER (__rw_lower),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /* '`' */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /* 'A' */ LETTER (__rw_upper | __rw_xdigit),
    /* 'B' */ LETTER (__rw_upper | __rw_xdigit),
    /* 'C' */ LETTER (__rw_upper | __rw_xdigit),
    /* 'D' */ LETTER (__rw_upper | __rw_xdigit),
    /* 'E' */ LETTER (__rw_upper | __rw_xdigit),
    /* 'F' */ LETTER (__rw_upper | __rw_xdigit),
    /* 'G' */ LETTER (__rw_upper),
    /* 'H' */ LETTER (__rw_upper),
    /* 'I' */ LETTER (__rw_upper),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /* 'J' */ LETTER (__rw_upper),
    /* 'K' */ LETTER (__rw_upper),
    /* 'L' */ LETTER (__rw_upper),
    /* 'M' */ LETTER (__rw_upper),
    /* 'N' */ LETTER (__rw_upper),
    /* 'O' */ LETTER (__rw_upper),
    /* 'P' */ LETTER (__rw_upper),
    /* 'Q' */ LETTER (__rw_upper),
    /* 'R' */ LETTER (__rw_upper),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /* 'S' */ LETTER (__rw_upper),
    /* 'T' */ LETTER (__rw_upper),
    /* 'U' */ LETTER (__rw_upper),
    /* 'V' */ LETTER (__rw_upper),
    /* 'W' */ LETTER (__rw_upper),
    /* 'X' */ LETTER (__rw_upper),
    /* 'Y' */ LETTER (__rw_upper),
    /* 'Z' */ LETTER (__rw_upper),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /* '0' */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '1' */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '2' */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '3' */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '4' */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '5' */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '6' */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '7' */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '8' */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /* '9' */ MASK (__rw_print | __rw_digit | __rw_graph | __rw_xdigit),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0),
    /*     */ MASK (0)

#else   // 'A' != 0x41 && 'A' != 0xc1
#  error unknown character set (neither ASCII nor EBCDIC)
#endif   // ASCII or EBCDIC

};


// table of lowercase to uppercase character mappings
extern const unsigned char
__rw_upper_tab [_STD::ctype<char>::table_size + 1] = {

#if 'A' == 0x41

    // basic ASCII:
    //          .0  .1  .2  .3  .4  .5  .6  .7  .8  .9  .a  .b  .c  .d  .e  .f
    //         --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
    //         NUL SOH STX ETX EOT ENQ ACK BEL BS  TAB  LF  VT  FF  CR  SO  SI
    /* 0. */ "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
    //         DLE DC1 DC2 DC3 DC4 NAK SYN ETB CAN  EM SUB ESC  FS  GS  RS  US
    /* 1. */ "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
    //         SPC   !   "   #   $   %   '   '   (   )   *   +   ,   -   .   /
    /* 2. */ "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
    //           0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
    /* 3. */ "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
    //               A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
    /* 4. */ "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
    //           P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
    /* 5. */ "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
    //           `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
    /* 6. */ "\x60\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
    //           p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~ DEL
    /* 7. */ "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x7b\x7c\x7d\x7e\x7f"

    // extended ASCII:
    /* 8. */ "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
    /* 9. */ "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
    /* a. */ "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
    /* b. */ "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
    /* c. */ "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
    /* d. */ "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
    /* e. */ "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
    /* f. */ "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"

#elif 'A' == 0xc1

    // EBCDIC:
    //          .0  .1  .2  .3  .4  .5  .6  .7  .8  .9  .a  .b  .c  .d  .e  .f
    //         --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
    //         NUL SOH STX ETX  PF  HT  LC DEL         SMM  VT  FF  CR  SO  SI
    /* 0. */ "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
    //         DLE DC1 DC2  TM RES  NL  BS  IL CAN  EM  CC CU1 IFS IGS IRS IUS
    /* 1. */ "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
    //          DS SOS  FS     BYP  LF ETB ESC          SM CU2     ENQ ACK BEL
    /* 2. */ "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
    //                 SYN      PN  RS  UC EOT             CU3 DC4 NAK     SUB
    /* 3. */ "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
    //          SP                                     ct.  .   <   (   +   |
    /* 4. */ "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
    //          &                                       !   $   *   )   ;   ~
    /* 5. */ "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
    //          -   /                                       ,   %   _   >   ?
    /* 6. */ "\x60\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
    //                                                  :   #   @   '   =   "
    /* 7. */ "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"
    //              a   b   c   d   e   f   g   h   i
    /* 8. */ "\x80\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\x8a\x8b\x8c\x8d\x8e\x8f"
    //              j   k   l   m   n   o   p   q   r
    /* 9. */ "\x90\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\x9a\x9b\x9c\x9d\x9e\x9f"
    //                  s   t   u   v   w   x   y   z
    /* a. */ "\xa0\xa1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xaa\xab\xac\xad\xae\xaf"
    //                                            `
    /* b. */ "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
    //              A   B   C   D   E   F   G   H   I
    /* c. */ "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
    //              J   K   L   M   N   O   P   Q   R
    /* d. */ "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
    //                  S   T   U   V   W   X   Y   Z
    /* e. */ "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
    //          0   1   2   3   4   5   6   7   8   9
    /* f. */ "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"

#else   // 'A' != 0x41 && 'A' != 0xc1
#  error unknown character set (neither ASCII nor EBCDIC)
#endif   // ASCII or EBCDIC

};


// table of uppercase to lowercase character mappings
extern const unsigned char
__rw_lower_tab [_STD::ctype<char>::table_size + 1] = {

#if 'A' == 0x41

    // basic ASCII:
    //          .0  .1  .2  .3  .4  .5  .6  .7  .8  .9  .a  .b  .c  .d  .e  .f
    //         --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
    //         NUL SOH STX ETX EOT ENQ ACK BEL BS  TAB  LF  VT  FF  CR  SO  SI
    /* 0. */ "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
    //         DLE DC1 DC2 DC3 DC4 NAK SYN ETB CAN  EM SUB ESC  FS  GS  RS  US
    /* 1. */ "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
    //         SPC   !   "   #   $   %   '   '   (   )   *   +   ,   -   .   /
    /* 2. */ "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
    //           0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
    /* 3. */ "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
    //               A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
    /* 4. */ "\x40\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"
    //           P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
    /* 5. */ "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x5b\x5c\x5d\x5e\x5f"
    //           `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
    /* 6. */ "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"
    //           p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~ DEL
    /* 7. */ "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"

    // extended ASCII:
    /* 8. */ "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
    /* 9. */ "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
    /* a. */ "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
    /* b. */ "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
    /* c. */ "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
    /* d. */ "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
    /* e. */ "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
    /* f. */ "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"

#elif 'A' == 0xc1

    // EBCDIC:
    //          .0  .1  .2  .3  .4  .5  .6  .7  .8  .9  .a  .b  .c  .d  .e  .f
    //         --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
    //         NUL SOH STX ETX  PF  HT  LC DEL         SMM  VT  FF  CR  SO  SI
    /* 0. */ "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
    //         DLE DC1 DC2  TM RES  NL  BS  IL CAN  EM  CC CU1 IFS IGS IRS IUS
    /* 1. */ "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
    //          DS SOS  FS     BYP  LF ETB ESC          SM CU2     ENQ ACK BEL
    /* 2. */ "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
    //                 SYN      PN  RS  UC EOT             CU3 DC4 NAK     SUB
    /* 3. */ "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
    //          SP                                     ct.  .   <   (   +   |
    /* 4. */ "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
    //          &                                       !   $   *   )   ;   ~
    /* 5. */ "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
    //          -   /                                       ,   %   _   >   ?
    /* 6. */ "\x60\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
    //                                                  :   #   @   '   =   "
    /* 7. */ "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"
    //              a   b   c   d   e   f   g   h   i
    /* 8. */ "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
    //              j   k   l   m   n   o   p   q   r
    /* 9. */ "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
    //                  s   t   u   v   w   x   y   z
    /* a. */ "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
    //                                            `
    /* b. */ "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
    //              A   B   C   D   E   F   G   H   I
    /* c. */ "\xc0\x81\x82\x83\x84\x85\x86\x87\x88\x89\xca\xcb\xcc\xcd\xce\xcf"
    //              J   K   L   M   N   O   P   Q   R
    /* d. */ "\xd0\x91\x92\x93\x94\x95\x96\x97\x98\x99\xda\xdb\xdc\xdd\xde\xdf"
    //                  S   T   U   V   W   X   Y   Z
    /* e. */ "\xe0\xe1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xea\xeb\xec\xed\xee\xef"
    //          0   1   2   3   4   5   6   7   8   9
    /* f. */ "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"

#else   // 'A' != 0x41 && 'A' != 0xc1
#  error unknown character set (neither ASCII nor EBCDIC)
#endif   // ASCII or EBCDIC

};


_RWSTD_EXPORT __rw_facet::_C_ctor_t __rw_ct_ctype;

_RWSTD_EXPORT __rw_facet* __rw_ct_ctype (_RWSTD_SIZE_T ref, const char *name)
{
    __rw_facet* pfacet;

    if (name) {
        pfacet = new _STD::ctype_byname<char>(name, ref);
    }
    else {
        static union {
            void *align_;
            char  data_ [sizeof (_STD::ctype<char>)];
        } f;
        static __rw_facet* const pf =
            new (&f) _STD::ctype<char>(__rw_classic_tab, false, ref);
        pfacet = pf;
    }

    // set the pointer to the facet id
    __rw_access::_C_get_pid (*pfacet) =
        &(__rw_access::_C_get_id (_STD::ctype<char>::id));

    return pfacet;
}


}   // namespace __rw


typedef _STD::ctype<char> __rw_ctype_facet;

_RWSTD_SPECIALIZE_USE_FACET (ctype);


_RWSTD_NAMESPACE (std) {


_RW::__rw_facet_id ctype<char>::id;


ctype<char>::ctype (const mask *tab, bool del, _RWSTD_SIZE_T refs)
    : _RW::__rw_facet (refs),
    _C_mask_tab (tab),
    _C_delete_it (del)
{
    // Save the passed table of mask values.
    if (!_C_mask_tab) {
        _C_mask_tab  = _RW::__rw_classic_tab;
        // prevent deletion if `tab' was false and `del' was true
        _C_delete_it = false;
    }

    _C_upper_tab = _RWSTD_CONST_CAST (_UChar*, _RW::__rw_upper_tab);
    _C_lower_tab = _RWSTD_CONST_CAST (_UChar*, _RW::__rw_lower_tab);

    memset (_C_narrow_tab, 0, sizeof _C_narrow_tab);
    memset (_C_wide_tab, 0, sizeof _C_wide_tab);
}


ctype<char>::~ctype ()
{
    if (_C_delete_it) {
        delete[] _RWSTD_CONST_CAST (mask*, _C_mask_tab);

        if (_C_upper_tab != _RW::__rw_upper_tab) {
            // lower_tab is allocated in the same block of memory
            // as upper_tab and must not be deleted
            delete[] _C_upper_tab;
        }
    }
}


const ctype<char>::mask* ctype<char>::classic_table () _THROWS (())
{
    return _RW::__rw_classic_tab;
}


const ctype<char>::char_type*
ctype<char>::
is (const char_type *lo, const char_type *hi, mask *vec) const
{
    _RWSTD_ASSERT (lo <= hi);
    _RWSTD_ASSERT (!hi || (lo && vec));
    _RWSTD_ASSERT (lo == hi || table ());

    while (lo < hi) {
        // follows lwg 28
        *vec++ = table ()[_UChar (*lo++)];
    }
    return hi;
}


const ctype<char>::char_type*
ctype<char>::
scan_is (mask m, const char_type *lo, const char_type *hi) const
{
    _RWSTD_ASSERT (lo <= hi);
    _RWSTD_ASSERT (!hi || lo);
    _RWSTD_ASSERT (lo == hi || table ());

    while (lo != hi && !(table ()[_UChar (*lo)] & m))
        ++lo;

    return lo;
}


const ctype<char>::char_type*
ctype<char>::
scan_not (mask m, const char_type *lo, const char_type *hi) const
{
    _RWSTD_ASSERT (lo <= hi);
    _RWSTD_ASSERT (!hi || lo);
    _RWSTD_ASSERT (lo == hi || table ());

    while (lo != hi && (table ()[_UChar (*lo)] & m))
        ++lo;

    return lo;
}


/* virtual */ ctype<char>::char_type
ctype<char>::
do_toupper (char_type ch) const
{
#if _RWSTD_UCHAR_MAX <= 255
    return _C_upper_tab [_UChar (ch)]; 
#else
    return ch < table_size ? _C_upper_tab [_UChar (ch)] : ch;
#endif
}


/* virtual */ const ctype<char>::char_type*
ctype<char>::
do_toupper (char_type *lo, const char_type *hi) const
{
    _RWSTD_ASSERT (lo <= hi);
    _RWSTD_ASSERT (!hi || lo);
    _RWSTD_ASSERT (lo == hi || _C_upper_tab);

    for ( ; lo < hi; ++lo)
        *lo = _C_upper_tab [_UChar (*lo)];

    return lo;
}


/* virtual */ ctype<char>::char_type
ctype<char>::
do_tolower (char_type ch) const
{
#if _RWSTD_UCHAR_MAX <= 255
    return _C_lower_tab [_UChar (ch)];
#else
    return ch < table_size ? _C_lower_tab [_UChar (ch)] : ch;
#endif
}


/* virtual */ const ctype<char>::char_type*
ctype<char>::
do_tolower (char_type *lo, const char_type *hi) const
{
    _RWSTD_ASSERT (lo <= hi);
    _RWSTD_ASSERT (!hi || lo);
    _RWSTD_ASSERT (lo == hi || _C_lower_tab);

    for ( ; lo < hi; ++lo)
        *lo = _C_lower_tab [_UChar (*lo)];

    return lo;
}


/* virtual */ ctype<char>::char_type
ctype<char>::
do_widen (char ch) const
{
#if _RWSTD_UCHAR_MAX <= 255

    if (_C_wide_tab [_UChar (ch)])
        return _C_wide_tab [_UChar (ch)];

    return _RWSTD_CONST_CAST (ctype<char>*, this)->
        _C_wide_tab [_UChar (ch)] = char_type (ch);
#else
    return char_type (ch);
#endif
}


/* virtual */ const ctype<char>::char_type*
ctype<char>::
do_widen (const char_type* lo, const char_type* hi, char_type* dst) const
{
    _RWSTD_ASSERT (lo <= hi);
    _RWSTD_ASSERT (!hi || (lo && dst));

    ::memcpy (dst, lo, hi - lo);

    return hi;
}


/* virtual */ ctype<char>::char_type
ctype<char>::
do_narrow (char_type ch, char_type) const
{
    return ch;
}


/* virtual */ const ctype<char>::char_type*
ctype<char>::
do_narrow (const char_type* lo, const char_type* hi, char_type,
           char_type* dst) const
{
    _RWSTD_ASSERT (lo <= hi);
    _RWSTD_ASSERT (!hi || (lo && dst));

    ::memcpy (dst, lo, hi - lo);

    return hi;
}


ctype_byname<char>::ctype_byname (const char *name, _RWSTD_SIZE_T refs)
    : ctype<char>(0, false, refs)
{
    _RWSTD_ASSERT (0 != name);

    // the name of the locale must be initialized before we attempt
    // to initialize the tables
    this->_C_set_name (name, _C_namebuf, sizeof _C_namebuf);

    if ('C' == name [0] && '\0' == name [1])
        return;

    // delayed initialization not used for efficiency
    _RW::__rw_ctype_t* impl =
        _RWSTD_CONST_CAST (_RW::__rw_ctype_t*,
            _RWSTD_STATIC_CAST (const _RW::__rw_ctype_t*,
                _RW::__rw_get_facet_data (LC_CTYPE, _C_impsize, _C_name)));

    _C_impdata = impl;

    if (impl == 0 || (this->_C_opts & this->_C_use_libc)) {

        // try to set locale first for exception safety
        const _RW::__rw_setlocale clocale (name, LC_CTYPE);

        // allocate memory for all three tables
        _C_mask_tab = new mask [table_size];

        _TRY {
            // allocate for both the upper and lower table to avoid
            // having to allocate twice
            _C_upper_tab = new unsigned char [table_size * 2];
            _C_lower_tab = _C_upper_tab + table_size;

            // set flag last, after allocation succeeds
            _C_delete_it = true;
        }
        _CATCH (...) {
            delete[] _RWSTD_CONST_CAST (mask*, _C_mask_tab);
            _RETHROW;
        }

        int i;

        for (i = 0; i != _RWSTD_UCHAR_MAX + 1; ++i) {

            int m = 0;

            if ((::isspace)(i))
                m |= _RW::__rw_space;
            if ((::isprint)(i))
                m |= _RW::__rw_print;
            if ((::iscntrl)(i))
                m |= _RW::__rw_cntrl;
            if ((::isupper)(i))
                m |= _RW::__rw_upper;
            if ((::islower)(i))
                m |= _RW::__rw_lower;
            if ((::isalpha)(i))
                m |= _RW::__rw_alpha;
            if ((::isdigit)(i))
                m |= _RW::__rw_digit;
            if ((::ispunct)(i))
                m |= _RW::__rw_punct;
            if ((::isxdigit)(i))
                m |= _RW::__rw_xdigit;
            if ((::isgraph)(i))
                m |= _RW::__rw_graph;

            _RWSTD_CONST_CAST (mask*, _C_mask_tab)[i] = mask (m);
        }

        typedef unsigned char UChar;

        unsigned char *p;
        for (p = _C_upper_tab, i = 0; i != _RWSTD_UCHAR_MAX + 1; ++i, ++p) {
            const int upr = (::toupper)(i);
            *p = UChar (_RWSTD_EOF == upr ? 0 : upr);
        }

        for (p = _C_lower_tab, i = 0; i != _RWSTD_UCHAR_MAX + 1; ++i, ++p) {
            const int lwr = (::tolower)(i);
            *p = UChar (_RWSTD_EOF == lwr ? 0 : lwr);
        }
    }
    else {
        _C_delete_it = false;

        _C_upper_tab = impl->toupper_tab;
        _C_lower_tab = impl->tolower_tab;

        const mask *mask_tab =
            _RWSTD_REINTERPRET_CAST (const mask*, impl->mask_tab);

        // verify that mask is the same size
        _RWSTD_ASSERT (sizeof *mask_tab == sizeof *impl->mask_tab);

        _C_mask_tab = _RWSTD_CONST_CAST (mask*, mask_tab);
    }
}


}   // namespace std
