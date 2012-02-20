/************************************************************************
*
* 21.string.append.stdcxx-1060.cpp - test case from STDCXX-1060 issue
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

#include <cassert>
#include <cstddef>
#include <string>
#include <stdexcept>

template <class ch>
struct my_traits : std::char_traits<ch>
{
    using std::char_traits<ch>::char_type;
    using std::char_traits<ch>::int_type;
    using std::char_traits<ch>::off_type; 
    using std::char_traits<ch>::state_type;
    using std::char_traits<ch>::pos_type;

    using std::char_traits<ch>::eof;
    using std::char_traits<ch>::eq;
    using std::char_traits<ch>::lt;
    using std::char_traits<ch>::compare;
    using std::char_traits<ch>::find;
    using std::char_traits<ch>::length;
    using std::char_traits<ch>::move;
    using std::char_traits<ch>::eq_int_type;
    using std::char_traits<ch>::not_eof;
    using std::char_traits<ch>::to_char_type;
    using std::char_traits<ch>::to_int_type;

    static void assign (ch& c1, const ch& c2) {
        std::char_traits<ch>::assign (c1, c2);
    }

    static ch* copy (ch* dst, const ch* src, std::size_t n);
    static ch* assign (ch* s, std::size_t n, ch c);
};

typedef std::basic_string<char, my_traits<char> > my_string;

std::size_t throw_size;

template <class ch>
ch* my_traits<ch>::copy (ch* dst, const ch* src, std::size_t n)
{
    if (throw_size == n)
        throw n;
    return std::char_traits<ch>::copy(dst, src, n);
}

template <class ch>
ch* my_traits<ch>::assign (ch* s, std::size_t n, ch c)
{
    if (throw_size == n)
        throw n;
    return std::char_traits<ch>::assign (s, n, c);
}

void test (std::size_t size)
{
    my_string s (5, 'a');
    throw_size = size;

    try {
        s.append ("a", size);
        assert (my_string::npos == size || !"Expect length error, got nothing");
    }
    catch (std::length_error&) {
    }
    catch (...) {
        assert (!"Expect length error, got other exception");
    }

    try {
        s.append (size, 'a');
        assert (!"Expect length error, got nothing");
    }
    catch (std::length_error&) {
    }
    catch (...) {
        assert (!"Expect length error, got other exception");
    }
}

int main ()
{
    test (my_string().max_size() + 1);
    test (my_string::npos - 1);
    test (my_string::npos);

    return 0;
}
