// checking for class std::type_info

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


namespace std {

#if defined (__EDG__) || defined (__sgi) && !defined (__GNUG__)
   // let edg (or its derivative) know about this fake type_info
#  pragma define_type_info
#endif   // __EDG__


class type_info { };

}   // namespace std


struct B { virtual ~B () { } };
struct D: B { };
struct E: B { };


int main (int argc, char *argv[])
{
    const std::type_info* ti1 = 0;
    const std::type_info* ti2 = 0;

    if (argc > 3) {
        ti1 = &typeid (D);
        ti2 = &typeid (E);
    }
    else if (argc > 2) {
        ti1 = &typeid (D);
        ti2 = &typeid (D);
    }

    if (argc > 1)
        return ti1 == ti2;

    // link only test 
    return 0;
}
