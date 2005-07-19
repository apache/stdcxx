/***************************************************************************
 *
 * loc_exception.h - exception class used in locale and localedef
 *
 * $Id: //stdlib/dev/source/stdlib/util/loc_exception.h#8 $
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

#include <exception>

class loc_exception : public std::exception {
public:
    loc_exception (){}
        
    loc_exception (const std::string &str) :
        what_str(str) {
    }
    ~loc_exception () throw () {}
    virtual const char* what() const throw() {
        return what_str.c_str();
    }
private:
    const std::string what_str;
};
