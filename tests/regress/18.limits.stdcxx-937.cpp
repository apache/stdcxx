/************************************************************************
 *
 * 18.limits.stdcxx-937.cpp - test case for STDCXX-937
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
#include <limits>

int main()
{
    // work extra hard to prevent aggressive optimizers from
    // optimizing the code away on the assumption that no
    // object has a null address
    const void* const fmemaddr[] = {
        &std::numeric_limits<float>::is_specialized,
        &std::numeric_limits<float>::digits,
        &std::numeric_limits<float>::digits10,

        &std::numeric_limits<float>::is_signed,
        &std::numeric_limits<float>::is_integer,
        &std::numeric_limits<float>::is_exact,
        &std::numeric_limits<float>:: radix,

        &std::numeric_limits<float>::min_exponent,
        &std::numeric_limits<float>::min_exponent10,
        &std::numeric_limits<float>::max_exponent,
        &std::numeric_limits<float>::max_exponent10,

        &std::numeric_limits<float>::has_infinity,
        &std::numeric_limits<float>::has_quiet_NaN,
        &std::numeric_limits<float>::has_signaling_NaN,
        &std::numeric_limits<float>::has_denorm,
        &std::numeric_limits<float>::has_denorm_loss,

        &std::numeric_limits<float>::is_iec559,
        &std::numeric_limits<float>::is_bounded,
        &std::numeric_limits<float>::is_modulo,

        &std::numeric_limits<float>::traps,
        &std::numeric_limits<float>::tinyness_before,

        &std::numeric_limits<float>::round_style
    };

    const void* const memaddr[] = {
        &std::numeric_limits<double>::is_specialized,
        &std::numeric_limits<double>::digits,
        &std::numeric_limits<double>::digits10,

        &std::numeric_limits<double>::is_signed,
        &std::numeric_limits<double>::is_integer,
        &std::numeric_limits<double>::is_exact,
        &std::numeric_limits<double>:: radix,

        &std::numeric_limits<double>::min_exponent,
        &std::numeric_limits<double>::min_exponent10,
        &std::numeric_limits<double>::max_exponent,
        &std::numeric_limits<double>::max_exponent10,

        &std::numeric_limits<double>::has_infinity,
        &std::numeric_limits<double>::has_quiet_NaN,
        &std::numeric_limits<double>::has_signaling_NaN,
        &std::numeric_limits<double>::has_denorm,
        &std::numeric_limits<double>::has_denorm_loss,

        &std::numeric_limits<double>::is_iec559,
        &std::numeric_limits<double>::is_bounded,
        &std::numeric_limits<double>::is_modulo,

        &std::numeric_limits<double>::traps,
        &std::numeric_limits<double>::tinyness_before,

        &std::numeric_limits<double>::round_style
    };

    const void* const ldmemaddr[] = {
        &std::numeric_limits<long double>::is_specialized,
        &std::numeric_limits<long double>::digits,
        &std::numeric_limits<long double>::digits10,

        &std::numeric_limits<long double>::is_signed,
        &std::numeric_limits<long double>::is_integer,
        &std::numeric_limits<long double>::is_exact,
        &std::numeric_limits<long double>:: radix,

        &std::numeric_limits<long double>::min_exponent,
        &std::numeric_limits<long double>::min_exponent10,
        &std::numeric_limits<long double>::max_exponent,
        &std::numeric_limits<long double>::max_exponent10,

        &std::numeric_limits<long double>::has_infinity,
        &std::numeric_limits<long double>::has_quiet_NaN,
        &std::numeric_limits<long double>::has_signaling_NaN,
        &std::numeric_limits<long double>::has_denorm,
        &std::numeric_limits<long double>::has_denorm_loss,

        &std::numeric_limits<long double>::is_iec559,
        &std::numeric_limits<long double>::is_bounded,
        &std::numeric_limits<long double>::is_modulo,

        &std::numeric_limits<long double>::traps,
        &std::numeric_limits<long double>::tinyness_before,

        &std::numeric_limits<long double>::round_style
    };

    for (unsigned i = 0; i != sizeof memaddr / sizeof *memaddr; ++i) {
        assert (fmemaddr [i] != 0);
        assert (memaddr [i] != 0);
        assert (ldmemaddr [i] != 0);
    }

    return 0;
}
