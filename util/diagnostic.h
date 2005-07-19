/***************************************************************************
 *
 * diagnostic.h
 *
 * $Id: //stdlib/dev/source/stdlib/util/diagnostic.h#12 $
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

#ifndef RWSTD_UTIL_DIAGNOSTIC_H_INCLUDED
#define RWSTD_UTIL_DIAGNOSTIC_H_INCLUDED

#include "scanner.h"  // for token_t

enum {
    E_FIRST   = 100,
    E_SYMUSED = 101,   // symbol used before defined
    E_OPENRD  = 109,   // opening a file for reading
    E_OPENWR  = 110,   // opening a file for writing
    E_PAIR    = 111,   // invalid pair of symbolic characters (<A>,<B>)
    E_COLNUM  = 114,   // bad number of collation orders
    E_CVT2EXT = 123,   // convert a character to external representation
    E_SYMEND  = 127,   // unterminated symbolic name (missing '>')
    E_CVT     = 136,   // convert a character
    E_IFMT    = 135,   // invalid integer format
    E_MBCHAR  = 308,   // illegal/incomplete multibyte character
    E_UCS     = 315,   // ill-formed or invalid UCS character
    E_MBTOWC  = 310,   // mbtowc() error
    E_RANGE   = 311,   // invalid range
    E_SYNTAX  = 312,   // bad syntax
    E_COLORD  = 133,   // bad collating order
    E_REORD   = 291,   // bad reorder-after
    E_INVAL   = 399,   // invalid value
    E_CMDARG  = 401,   // invalid command line argument
    E_XARG    = 402,   // extra command line argument
    E_NOARG   = 403,   // missing command line argument
    E_OPTARG  = 404,   // missing argument to a command line option
    E_CALL    = 405,   // system or libc call failed
    E_LAST    = 699,

    W_FIRST   = 700,
    W_DISABLE = 700,   // disable all warnings
    W_COMPAT  = 701,   // no compatible locale installed
    W_NOPCS   = 702,   // PCS character value not defined
    W_COLSYM  = 703,   // undefined collating symbol or collating element
    W_COLVAL  = 704,   // missing collation value for symbol
    W_REORD   = 705,   // bad reorder-after
    W_ICONV   = 706,   // iconv_open() or iconv() error
    W_SYM     = 707,   // unknown symbolic constant
    W_SYNTAX  = 708,   // recoverable syntax error
    W_RANGE   = 709,   // recoverable invalid range
    W_INVAL   = 710,   // recoverable invalid value
    W_CALL    = 711,   // system or libc call failed
    W_LAST    = 799,

    I_FIRST   = 800,
    I_ENABLE  = 800,    // enable all informational messages
    I_STAGE   = 801,    // information about stages of processing
    I_OPENRD  = 802,    // information about files being opened for reading
    I_OPENWR  = 803,    // information about files being opened for writing
    I_READ    = 804,    // information about data reads
    I_WRITE   = 805,    // information about data writes
    I_LAST    = 899
};


bool issue_diag (int id, bool is_error, 
                 const Scanner::token_t* token, 
                 const char *fmt, ...);

#endif   // RWSTD_UTIL_DIAGNOSTIC_H_INCLUDED
