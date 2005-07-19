/**************************************************************************
 *
 * messages.cpp - Example program for the messages facet. 
 *
 * $Id: //stdlib/dev/examples/stdlib/manual/messages.cpp#10 $
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

#include <string>     // for string
#include <locale>     // for locale, use_facet, messages
#include <iostream>   // for cerr, cout, endl

#include <rwstdmessages.h>
#include <examples.h>

#ifndef _WIN32
#  define CAT_NAME   "./rwstdmessages.cat"
#else
#  define CAT_NAME   "rwstdmessages.dll"
#endif


int main ()
{
    std::locale loc;

    // Get a reference to the messages<char> facet
    const std::messages<char>& msgs =
        std::use_facet <std::messages<char> >(loc);

    // Open a catalog and try to grab
    // both some valid messages, and an invalid message
    const std::string def  = "Message Not Found";

    std::messages<char>::catalog cat = msgs.open (CAT_NAME, loc);

    if (cat != std::messages<char>::catalog (-1)) {
        std::cout << msgs.get (cat, 1, _RW_MSG_HELLO, def)   << '\n'
                  << msgs.get (cat, 1, _RW_MSG_GOODBYE, def) << '\n'
                  << msgs.get (cat, 1, _RW_MSG_NOGOOD, def)  << '\n'
                  << msgs.get (cat, 2, _RW_MSG_TREES, def)   << '\n';

        msgs.close (cat);
    }
    else
        std::cerr << "Unable to open message catalog" << std::endl;

    return 0;
}
