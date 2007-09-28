// checking for runtime support in namespace std

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

#include "config.h"

#include <stdio.h>


#ifndef _RWSTD_NO_HONOR_STD
#  define STD_NAME(name)     name
#else   // if defined (_RWSTD_NO_HONOR_STD)
#  define STD_NAME(ignore)   ""
#endif   // _RWSTD_NO_HONOR_STD

const char* const std_defs[] = {

   // avoid false positives if namespace std is not honored

#ifdef _RWSTD_NO_STD_BAD_ALLOC
    STD_NAME ("BAD_ALLOC"),
#else
    "",
#endif   // _RWSTD_NO_STD_BAD_ALLOC

#ifdef _RWSTD_NO_STD_BAD_CAST
    STD_NAME ("BAD_CAST"),
#else
    "",
#endif   // _RWSTD_NO_STD_BAD_CAST

#ifdef _RWSTD_NO_STD_BAD_EXCEPTION
    STD_NAME ("BAD_EXCEPTION"),
#else
    "",
#endif   // _RWSTD_NO_STD_BAD_EXCEPTION

#ifdef _RWSTD_NO_STD_BAD_TYPEID
    STD_NAME ("BAD_TYPEID"),
#else
    "",
#endif   // _RWSTD_NO_STD_BAD_TYPEID

#ifdef _RWSTD_NO_STD_EXCEPTION
    STD_NAME ("EXCEPTION"),
#else
    "",
#endif   // _RWSTD_NO_STD_EXCEPTION

#ifdef _RWSTD_NO_STD_SET_NEW_HANDLER
    STD_NAME ("SET_NEW_HANDLER"),
#else
    "",
#endif   // _RWSTD_NO_STD_SET_NEW_HANDLER

#ifdef _RWSTD_NO_STD_SET_TERMINATE
    STD_NAME ("SET_TERMINATE"),
#else
    "",
#endif   // _RWSTD_NO_STD_SET_TERMINATE

#ifdef _RWSTD_NO_STD_SET_UNEXPECTED
    STD_NAME ("SET_UNEXPECTED"),
#else
    "",
#endif   // _RWSTD_NO_STD_SET_UNEXPECTED

#ifdef _RWSTD_NO_STD_TERMINATE
    STD_NAME ("TERMINATE"),
#else
    "",
#endif   // _RWSTD_NO_STD_TERMINATE

#ifdef _RWSTD_NO_STD_UNCAUGHT_EXCEPTION
    STD_NAME ("UNCAUGHT_EXCEPTION"),
#else
    "",
#endif   // _RWSTD_NO_STD_UNCAUGHT_EXCEPTION

#ifdef _RWSTD_NO_STD_UNEXPECTED
    STD_NAME ("UNEXPECTED"),
#else
    "",
#endif   // _RWSTD_NO_STD_UNEXPECTED

    0   // sentinel
};


const char* const global_defs[] = {

#ifdef _RWSTD_NO_GLOBAL_BAD_ALLOC
    "BAD_ALLOC",
#else
    "",
#endif   // _RWSTD_NO_GLOBAL_BAD_ALLOC

#ifdef _RWSTD_NO_GLOBAL_BAD_CAST
    "BAD_CAST",
#else
    "",
#endif   // _RWSTD_NO_GLOBAL_BAD_CAST

#ifdef _RWSTD_NO_GLOBAL_BAD_EXCEPTION
    "BAD_EXCEPTION",
#else
    "",
#endif   // _RWSTD_NO_GLOBAL_BAD_EXCEPTION

#ifdef _RWSTD_NO_GLOBAL_BAD_TYPEID
    "BAD_TYPEID",
#else
    "",
#endif   // _RWSTD_NO_GLOBAL_BAD_TYPEID

#ifdef _RWSTD_NO_GLOBAL_EXCEPTION
    "EXCEPTION",
#else
    "",
#endif   // _RWSTD_NO_GLOBAL_EXCEPTION

#ifdef _RWSTD_NO_GLOBAL_SET_NEW_HANDLER
    "SET_NEW_HANDLER",
#else
    "",
#endif   // _RWSTD_NO_GLOBAL_SET_NEW_HANDLER

#ifdef _RWSTD_NO_GLOBAL_SET_TERMINATE
    "SET_TERMINATE",
#else
    "",
#endif   // _RWSTD_NO_GLOBAL_SET_TERMINATE

#ifdef _RWSTD_NO_GLOBAL_SET_UNEXPECTED
    "SET_UNEXPECTED",
#else
    "",
#endif   // _RWSTD_NO_GLOBAL_SET_UNEXPECTED

#ifdef _RWSTD_NO_GLOBAL_TERMINATE
    "TERMINATE",
#else
    "",
#endif   // _RWSTD_NO_GLOBAL_TERMINATE

#ifdef _RWSTD_NO_GLOBAL_UNCAUGHT_EXCEPTION
    "UNCAUGHT_EXCEPTION",
#else
    "",
#endif   // _RWSTD_NO_GLOBAL_UNCAUGHT_EXCEPTION

#ifdef _RWSTD_NO_GLOBAL_UNEXPECTED
    "UNEXPECTED",
#else
    "",
#endif   // _RWSTD_NO_GLOBAL_UNEXPECTED

    0   // sentinel
};


int main ()
{
    // since some (possibly many) individual config tests fail
    // to detect either an STD or a GLOBAL version of the feature,
    // this test overrides (#undef-ines) one of the matching pair
    // of macros based on the overall findings to avoid complex
    // #ifdef logic in the library headers

    unsigned n_std_defs      = 0;   // number of macros defined for std names
    unsigned n_global_defs   = 0;   // number of macros defined for global names
    unsigned n_both_defs     = 0;   // number of macros defined for both
    unsigned n_neither_defs  = 0;   // number of macros defined for neither
    unsigned n_count         = 0;   // total number of symbols

    // numbers of symbols found to be defined only in namespace std
    // and not in the global namespace, and vice versa
    unsigned n_std_only_syms    = 0;
    unsigned n_global_only_syms = 0;

    unsigned i;

    // count the number of macro definitions for the symbols
    // found to be missing in namespace std and in the global
    // namespace, the number of symbols found to be defined
    // in both namespaces, and the total number of symbols
    for (i = 0; std_defs [i]; ++i, ++n_count) {

        // number of macros defined
        int cnt = 0;

        if (*std_defs [i]) {
            ++n_std_defs;
            ++cnt;
        }

        if (*global_defs [i]) {

            if (0 == cnt) {
                // increment the number of symbols that are found to be
                // defined in namespace std and not in the global namespace
                ++n_std_only_syms;
            }

            ++n_global_defs;
            ++cnt;
        }
        else {
            if (1 == cnt) {
                // increment the number of symbols that are found to be
                // defined in the global namespace and not also in std
                ++n_global_only_syms;
            }
        }

        if (2 == cnt)
            ++n_both_defs;
        else if (0 == cnt)
            ++n_neither_defs;
    }

    // #undef macros that are (assumed to be) #defined due to
    // false negative test results
    for (i = 0; std_defs [i]; ++i) {
        if (*std_defs [i] && *global_defs [i]) {

            // both the NO_STD and NO_GLOBAL forms of the same macro
            // are #defined (i.e., the symbol wasn't found in either
            // namespace, presumably because of a false negative
            // result of one or both tests for the symbol)

            const char *symbol_scope;

            if (n_std_only_syms && n_global_only_syms <= n_std_only_syms) {
                // there are at least as many names defined only in
                // namespace std and not also in the global namespace
                // than there are those defined in the global namespace
                // that are not also defined in namespace std (and this
                // number is non-zero)
                symbol_scope = "STD";
            }
            else if (n_global_defs < n_std_defs) {
                // there are more symbols found to be defined in the global
                // namespace than there are those defined in namespace std
                symbol_scope = "GLOBAL";
            }
            else
                symbol_scope = "STD";

            // undefine the macro
            printf ("#undef _RWSTD_NO_%s_%s\n", symbol_scope, std_defs [i]);
        }
    }

    // in a comment print out the number of symbols found to be defined
    // in namespace std, in the global namespace, in both namespaces,
    // and in neither
    printf ("// number of runtime library symbols examined: %2u\n"
            "// symbols found only in namespace std:        %2u\n"
            "// symbols found only in the global namespace: %2u\n"
            "// symbols found in both namespaces:           %2u\n"
            "// symbols found in neither namespace:         %2u\n",
            n_count,
            n_std_only_syms,
            n_global_only_syms,
            n_neither_defs,
            n_both_defs);

    if (n_std_only_syms) {
        // comment out the NO_RUNTIME_IN_STD macro when at least one symbol
        // is defined in namespace std and not also defined in the global
        // namespace
        printf ("%s ", "//");
    }

    printf ("#define %s\n", "_RWSTD_NO_RUNTIME_IN_STD\n");

    return 0;
}
