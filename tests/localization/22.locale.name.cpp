/***************************************************************************
 *
 * 22.locale.name.cpp - test exercising std::locale::name()
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
 **************************************************************************/

#include <cstdio>     // for printf()
#include <cstring>    // for strcmp()
#include <locale>     // for locale

#include <rw_driver.h>


void test_combined_name ()
{
    const char name[] = {
#ifdef _AIX
        "LC_TIME=EN_US.UTF-8;"
        "LC_NUMERIC=C;"
        "LC_MONETARY=en_US;"
        "LC_MESSAGES=EN_US.UTF-8;"
        "LC_CTYPE=C;"
        "LC_COLLATE=en_US.ISO8859-1;"
#elif defined __hpux
        "LC_TIME=en_US.utf8;"
        "LC_NUMERIC=C;"
        "LC_MONETARY=en_US.roman8;"
        "LC_MESSAGES=en_US.utf8;"
        "LC_CTYPE=C;"
        "LC_COLLATE=en_US.iso88591;"
#elif defined __linux
        "LC_TIME=en_US.utf8;"
        "LC_NUMERIC=C;"
        "LC_MONETARY=en_US;"
        "LC_MESSAGES=en_US.utf8;"
        "LC_CTYPE=C;"
        "LC_COLLATE=en_US.iso88591;"
#elif defined __osf__
        "C"
#elif defined __sun
        "LC_TIME=en_US.UTF-8;"
        "LC_NUMERIC=C;"
        "LC_MONETARY=en_US;"
        "LC_MESSAGES=en_US.UTF-8;"
        "LC_CTYPE=C;"
        "LC_COLLATE=en_US.ISO8859-1;"
#elif defined _WIN32
        "C"
#else
        "C"
#endif
    };

   const char expected_name[] = {
#ifdef _AIX
        "en_US.ISO8859-1 "   // LC_COLLATE
        "C "                 // LC_CTYPE
        "en_US "             // LC_MONETARY
        "C "                 // LC_NUMERIC
        "EN_US.UTF-8 "       // LC_TIME
        "EN_US.UTF-8"        // LC_MESSAGES
#elif defined __hpux
        "en_US.iso88591 "    // LC_COLLATE
        "C "                 // LC_CTYPE
        "en_US.roman8 "      // LC_MONETARY
        "C "                 // LC_NUMERIC
        "en_US.utf8 "        // LC_TIME
        "en_US.utf8"         // LC_MESSAGES
#elif defined __linux
        "LC_CTYPE=C;"
        "LC_NUMERIC=C;"
        "LC_TIME=en_US.utf8;"
        "LC_COLLATE=en_US.iso88591;"
        "LC_MONETARY=en_US;"
        "LC_MESSAGES=en_US.utf8"
#elif defined __osf__
        ""
#elif defined __sun
        "/C"                 // LC_COLLATE
        "/C"                 // LC_CTYPE
        "/en_US.UTF-8"       // LC_MONETARY
        "/en_US.ISO8859-1"   // LC_NUMERIC
        "/en_US"             // LC_TIME
        "/en_US.UTF-8"       // LC_MESSAGES
#elif defined _WIN32
        "C" 
#else
        "C"
#endif
    };

    std::locale loc;

    try {
        // try to create the combined locale
        loc = std::locale (name);
    }
    catch (...) {
        rw_error (0, 0, __LINE__,
                  "std::locale(%#s) threw an exception\n", name);
        return;
    }

    // verify that the actual locale name matches the expected name
    const int cmp = std::strcmp (loc.name ().c_str (), expected_name);

    rw_assert (0 == cmp, 0, __LINE__,
               "std::locale(%#s).name() == %#s, got %#s",
               name, expected_name, loc.name ().c_str ());
}


static int
run_test (int, char**)
{
    test_combined_name ();

    return 0;
}


int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "[lib.locale]",
                    "name()",   // comment
                    run_test, "", 0);
}

