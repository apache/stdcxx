/***************************************************************************
 *
 * locale_stub.cpp
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

#include <cstdlib>   // for size_t, system()
#include <cstring>   // for strcat(), strlen()


int main (int argc, char *argv[])
{
    // compute the total length of the command line arguments
    std::size_t arglen = 0;
    for (int i = 0; i != argc; ++i)
        arglen += std::strlen (argv [i]);

    // add the length of space separating adjacent pairs
    arglen += argc;

    // add the length of the option to invoke localeded in locale mode
    const char argv_1[] = "--locale-mode";
    arglen += sizeof argv_1;

    const char localedef[] = "localedef";
    arglen += sizeof localedef;

    // allocate space for the new command line
    char* const cmdline = new char [arglen];

    // replace the basename of argv[0] with "localedef" keeping the
    // directory portion, if it's there, followed by the new option
    // separated by a space
    std::strcpy (cmdline, argv [0]);
    char* slash = std::strrchr (cmdline, '/');

    if (0 == slash) {
        // look for backslash for Windoze
        slash = std::strrchr (cmdline, '\\');
    }

    if (slash)
        std::strcpy (slash + 1, localedef);
    else
        std::strcpy (cmdline, localedef);

    std::strcat (cmdline, " ");
    std::strcat (cmdline, argv_1);

    // copy arguments to the newly allocated buffer
    for (int i = 1; i != argc; ++i) {
        std::strcat (cmdline, " ");
        std::strcat (cmdline, argv [i]);
    }

    // invoke the localedef utility instructing it to switch
    // to locale mode
    const int status = std::system (cmdline);

    // clean up
    delete[] cmdline;

    return status;
}
