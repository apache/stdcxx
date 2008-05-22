/**************************************************************************
 *
 * 27.stream.inserters.stdcxx-51.cpp - regression test for STDCXX-51
 *
 * http://issues.apache.org/jira/browse/STDCXX-51
 *
 * $Id$
 *
 **************************************************************************
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
#include <cstdio>
#include <limits>
#include <sstream>


template <class FloatT>
bool test (const char *tname)
{
    // zero and one used to verify sanity
    const FloatT zero =  FloatT ();
    const FloatT pone = +FloatT (1);
    const FloatT none = -FloatT (1);

    // positive and negative Infinity
    const FloatT pinf = +std::numeric_limits<FloatT>::infinity ();
    const FloatT ninf = -std::numeric_limits<FloatT>::infinity ();

    // positive and negative Quiet NaN
    const FloatT pqnan = +std::numeric_limits<FloatT>::quiet_NaN ();
    const FloatT nqnan = -std::numeric_limits<FloatT>::quiet_NaN ();

    // positive and negative Signaling NaN
    const FloatT psnan = +std::numeric_limits<FloatT>::signaling_NaN ();
    const FloatT nsnan = -std::numeric_limits<FloatT>::signaling_NaN ();

    std::ostringstream out;

    out << "     0.0:  " << zero  << '\n'
        << "  +/-1.0:  " << pone  << ' ' << none  << '\n'
        << "  +/-INF:  " << pinf  << ' ' << ninf  << '\n'
        << "  +/-QNAN: " << pqnan << ' ' << nqnan << '\n'
        << "  +/-SNAN: " << psnan << ' ' << nsnan << '\n';

    // expected output
    const char expect[] = {
        "     0.0:  0\n"
        "  +/-1.0:  1 -1\n"
        "  +/-INF:  inf -inf\n"
        "  +/-QNAN: qnan -qnan\n"
        "  +/-SNAN: snan -snan\n"
    };

    bool pass = true;

    if (expect != out.str ()) {
        std::fprintf (stderr, "Expected (%s):\n%s\nGot:\n%s\n",
                      tname, expect, out.str ().c_str ());
        pass = false;
    }

    // clear string and set the uppercase bit in flags
    out.str ("");
    out.setf (out.uppercase);

    out << "     0.0:  " << zero  << '\n'
        << "  +/-1.0:  " << pone  << ' ' << none  << '\n'
        << "  +/-INF:  " << pinf  << ' ' << ninf  << '\n'
        << "  +/-QNAN: " << pqnan << ' ' << nqnan << '\n'
        << "  +/-SNAN: " << psnan << ' ' << nsnan << '\n';

    // expected output
    const char EXPECT[] = {
        "     0.0:  0\n"
        "  +/-1.0:  1 -1\n"
        "  +/-INF:  INF -INF\n"
        "  +/-QNAN: QNAN -QNAN\n"
        "  +/-SNAN: SNAN -SNAN\n"
    };

    if (EXPECT != out.str ()) {
        std::fprintf (stderr, "Expected (%s):\n%s\nGot:\n%s\n",
                      tname, EXPECT, out.str ().c_str ());
        pass = false;
    }

    return pass;
}


int main ()
{
    bool pass;

    // exercise the formatting of Infinities and NaNs
    pass = test<float>("float");
    pass = test<double>("double") && pass;

#ifndef _RWSTD_NO_LONG_DOUBLE

    pass = test<long double>("long double") && pass;

#endif   // _RWSTD_NO_LONG_DOUBLE

    assert (pass);

    return 0;
}
