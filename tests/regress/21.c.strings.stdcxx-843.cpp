/************************************************************************
 *
 * 21.c.strings.stdcxx-843.cpp - regression test for STDCXX-843
 *
 * http://issues.apache.org/jira/browse/STDCXX-843
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
#include <cwchar>

#ifndef _RWSTD_MBSTATE_T_SIZE
   // explicitly include our own <rw/_defs.h> in case neither
   // of the two above is ours (i.e., we're using the compiler's
   // C++ C headers)
#  include <rw/_defs.h>
#endif   // _RWSTD_MBSTATE_T_SIZE


// known mbstate_t sizes on major platforms
#ifdef _RWSTD_OS_AIX
#  define KNOWN_SIZE   sizeof(long)
#elif defined _RWSTD_OS_HP_UX
#  define KNOWN_SIZE   8
#elif defined _RWSTD_OS_FREEBSD
#  define KNOWN_SIZE   128
#elif defined _RWSTD_OS_IRIX64
#  define KNOWN_SIZE   1
#elif defined _RWSTD_OS_LINUX
#  define KNOWN_SIZE   8
#elif defined _RWSTD_OS_OSF1
#  define KNOWN_SIZE   24
#elif defined _RWSTD_OS_SUNOS
#  define KNOWN_SIZE   (sizeof(long) == 8 ? 32 : 24)
#elif defined _RWSTD_OS_WINDOWS
#  define KNOWN_SIZE   4
#endif


int main ()
{
    // verify that the size is the same as what was detectected
    // during configuration
    assert (sizeof (std::mbstate_t) == _RWSTD_MBSTATE_T_SIZE);

#ifdef KNOWN_SIZE

    // on known platforms verify that the actual size matches
    // the size known on that platform
    assert (sizeof (std::mbstate_t) == KNOWN_SIZE);

#endif   // KNOWN_SIZE

    return 0;
}
