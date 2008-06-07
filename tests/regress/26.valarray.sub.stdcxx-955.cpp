/************************************************************************
 *
 * 26.valarray.sub.stdcxx-955.cpp - regression test for STDCXX-955
 *
 *   http://issues.apache.org/jira/browse/STDCXX-955
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
 * Copyright 2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <cstdlib>
#include <valarray>
#include <cassert>

void test_0 ()
{
    const std::size_t p_leng [] = { 0 };
    const std::size_t p_strd [] = { 3 };

    const std::valarray<std::size_t>
        va_leng (p_leng, sizeof (p_leng) / sizeof (*p_leng));

    const std::valarray<std::size_t>
        va_strd (p_strd, sizeof (p_strd) / sizeof (*p_strd));

    const std::gslice gs_slice (1, va_leng, va_strd);

    std::valarray<char> va_lhs ('!', 4);

    va_lhs [gs_slice] = '?';

    assert (va_lhs [0] == '!');
    assert (va_lhs [1] == '!');
    assert (va_lhs [2] == '!');
    assert (va_lhs [3] == '!');
}

void test_1 ()
{
    const std::size_t p_leng [] = { 2, 0 };
    const std::size_t p_strd [] = { 1, 3 };

    const std::valarray<std::size_t>
        va_leng (p_leng, sizeof (p_leng) / sizeof (*p_leng));

    const std::valarray<std::size_t>
        va_strd (p_strd, sizeof (p_strd) / sizeof (*p_strd));

    const std::gslice gs_slice (1, va_leng, va_strd);

    std::valarray<char> va_lhs ('!', 4);

    va_lhs [gs_slice] = '?';

    assert (va_lhs [0] == '!');
    assert (va_lhs [1] == '?');
    assert (va_lhs [2] == '?');
    assert (va_lhs [3] == '!');
}

int main ()
{
    test_0 ();
    test_1 ();

    return 0;
}
