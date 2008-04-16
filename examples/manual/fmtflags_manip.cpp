/**************************************************************************
 *
 * fmtflags_manip.cpp - Example program demonstrating the implementation
 * of a user-defined manipulator for convenient and exception-safe setting
 * and restoring of stream formatting flags.
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

#include <ios>        // for hex
#include <iomanip>    // for __rw_smanip
#include <iostream>   // for cout

#include <examples.h>


// implementation class of the fmtflags manipulator that saves
// a stream object's fmtflags state before optionally setting
// their new value and restores the saved state when destroyed
// uses ios::basefield as an invalid flags value (it's a valid
// mask but not a valid flags value)
class fmtflags_manip
{
    std::ios_base           *strm_;
    std::ios_base::fmtflags  saved_;

public:
    fmtflags_manip (): strm_ (), saved_ (std::ios_base::basefield) { }

    void operator() (std::ios_base           &strm,
                     std::ios_base::fmtflags  flags) const {

        const_cast<fmtflags_manip*>(this)->strm_  = &strm;
        const_cast<fmtflags_manip*>(this)->saved_ = strm.flags ();

        if (flags != std::ios_base::basefield)
            strm.flags (flags);
    }

    ~fmtflags_manip () {
        if (strm_)
            strm_->flags (saved_);
    }
};


// saveflags manipulator to temporarily set formatting flags and
// restore their initial value at the end of an insertion statement
inline std::__rw_smanip<fmtflags_manip, std::ios_base::fmtflags>
saveflags (std::ios_base::fmtflags flags = std::ios_base::basefield)
{
    typedef std::__rw_smanip<fmtflags_manip, std::ios_base::fmtflags> Manip;

    return Manip (flags);
}


// dummy class whose insertion operator always throws
struct BadClass { };


inline std::ostream&
operator<< (std::ostream &strm, BadClass&)
{
    // inserter that always throws an exception
    return throw "exception", strm;

}


int main ()
{
    try {
        BadClass throw_exception;

        // use the saveflags manipulator to set the formatting flags
        // for the rest of this insertion statement only and have it
        // restore the  previous flags at the end of the statement
        // or on exception
        std::cout << saveflags (std::cout.hex)
                  << std::showbase
                  << "hex 1234: " << 0x1234 << '\n'
                  << "hex 2345: " << 0x2345 << '\n'
                  << throw_exception    // inserting will throw
                  << "never output!";   // this string is never output
    }
    catch (...) {
        // show that default formatting flags (decimal output and no
        // showbase) have been restored during stack unwinding above
        std::cout << "dec 3456:   " << 3456 << '\n'
                  << std::oct
                  << "oct 4567:   " << 04567 << '\n';
    }

    // use ordinary manipulators to set hex output with base prefix
    std::cout << std::hex << std::showbase
              << "hex 5678: " << 0x5678 << '\n'
              << "hex 6789: " << 0x6789 << '\n';

    // the same flags set above are still in effect
    std::cout << "hex 789a: " << 0x789a << '\n';

    return 0;
}
