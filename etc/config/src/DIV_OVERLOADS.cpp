// checking for overloads of div()

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

// determine whether the div() function (including
// any overloads) is defined in the usual libc headers

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

#ifndef _RWSTD_NO_LONG_LONG
#  if defined _MSC_VER
#    define LONG_LONG   __int64
#  else
#    define LONG_LONG   long long
#  endif
#endif   // _RWSTD_NO_LONG_LONG


int template_used;

template <class T>
ldiv_t div (T, T)
{
    ++template_used;

    return ldiv_t ();
}

template <class T>
void check_div (T t, const char *tname)
{
    template_used = 0;

    div (t, t);

    if (0 == template_used)
        printf ("%s", "// ");

    printf ("#define _RWSTD_NO_DIV_%s\n", tname);
}

int main ()
{
    check_div ((int)1, "INT");
    check_div ((long)1, "LONG");

#if defined (LONG_LONG)
    check_div ((LONG_LONG)1, "LLONG");
#endif   // LONG_LONG

    return 0;
}
