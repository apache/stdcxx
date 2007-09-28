// checking for C library in namespace std

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

#include "config.h"

#ifndef _RWSTD_NO_NEW_HEADER

#include <cctype>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#else   // _RWSTD_NO_NEW_HEADER

#include <ctype.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#endif   // _RWSTD_NO_NEW_HEADER


int main ()
{
    (void)&std::isalpha;     // <ctype>
    (void)&std::setlocale;   // <clocale>
    std::pow (0.0, 0.0);     // <cmath>
    (void)&std::fopen;       // <cstdio>
    (void)&std::malloc;      // <cstdlib>
    (void)&std::strcpy;      // <cstring>
    (void)&std::clock;       // <ctime>
    return std::size_t (0);  // <cstddef>, <cstring>, ...
}
