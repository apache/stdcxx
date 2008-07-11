/************************************************************************
 *
 * 18.support.runtime.stdcxx-989.cpp - regression test for STDCXX-989
 *
 * http://issues.apache.org/jira/browse/STDCXX-989
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
#include <csetjmp>

int main ()
{

#ifdef __linux__
#  ifdef __i386
     // Linux on x86 (including x64 and derivatives)
#    define expected_jmp_buf_size   (4 == sizeof (long) ? 156 : 200)
#  endif
#elif defined __sun
#  ifdef __sparc
     // Sun Solaris on SPARC
#    define expected_jmp_buf_size   (sizeof (long) * 12)
#  endif
#endif

#ifdef expected_jmp_buf_size

    using namespace std;

    assert (expected_jmp_buf_size == sizeof (jmp_buf));

#endif   // expected_jmp_buf_size

    return 0;

}
