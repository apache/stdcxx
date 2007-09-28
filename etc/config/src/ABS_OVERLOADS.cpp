// checking for overloads of abs()

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

// determine whether the abs() function (including
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
T abs (T t)
{
    ++template_used;

    return t;
}

template <class T>
void check_abs (T t, const char *tname)
{
    template_used = 0;

    abs (t);

    if (0 == template_used)
        printf ("%s", "// ");

    printf ("#define _RWSTD_NO_ABS_%s\n", tname);
}

int main ()
{
    check_abs ((bool)0, "BOOL");
    check_abs ((char)0, "CHAR");
    check_abs ((signed char)0, "SCHAR");
    check_abs ((unsigned char)0, "UCHAR");
    check_abs ((short)0, "SHRT");
    check_abs ((unsigned short)0, "USHRT");
    check_abs ((int)0, "INT");
    check_abs ((unsigned)0, "UINT");
    check_abs ((long)0, "LONG");
    check_abs ((unsigned long)0, "ULONG");

#if defined (LONG_LONG)
    check_abs ((LONG_LONG)0, "LLONG");
    check_abs ((unsigned LONG_LONG)0, "ULLONG");
#endif   // LONG_LONG

    return 0;
}
