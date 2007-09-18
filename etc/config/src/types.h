// definitions of the type_name() helper overloads

/***************************************************************************
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
 * Copyright 1999-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#define STRSTR(symbol)   #symbol
#define STR(symbol)      STRSTR (symbol)
#define DEFINE_TYPE_HELPER(T, symbol)                   \
    const char* type_name (T t) {                       \
        if (t) return symbol; else return STR (T);      \
    } typedef void unused_typedef


DEFINE_TYPE_HELPER (char, "CHAR");
DEFINE_TYPE_HELPER (signed char, "SCHAR");
DEFINE_TYPE_HELPER (unsigned char, "UCHAR");

DEFINE_TYPE_HELPER (short, "SHRT");
DEFINE_TYPE_HELPER (unsigned short, "USHRT");

DEFINE_TYPE_HELPER (int, "INT");
DEFINE_TYPE_HELPER (unsigned, "UINT");

DEFINE_TYPE_HELPER (long, "LONG");
DEFINE_TYPE_HELPER (unsigned long, "ULONG");

#ifdef LONG_LONG

DEFINE_TYPE_HELPER (LONG_LONG, "LLONG");
DEFINE_TYPE_HELPER (unsigned LONG_LONG, "ULLONG");

#endif   // LONG_LONG

// for any other type
const char* type_name (...) { return 0; }
