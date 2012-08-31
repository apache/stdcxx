/**************************************************************************
 *
 * 27.basic_ios.copyfmt.stdcxx-1058.cpp - regression test for STDCXX-1058
 *
 * http://issues.apache.org/jira/browse/STDCXX-1058
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
/*
 * This program should run to completion and exit with return status 0
 */

#include <ios>
#include <stdio.h>
#include <stdlib.h>

#define TEST_ASSERT(expr) \
    (expr) ? ret = 0 \
    : fprintf(stderr,"file %s line %i: Assertion failed: %s\n", \
    __FILE__, __LINE__, #expr); fflush(stderr); if (ret != 0) abort()

int x;

void testfun (std::ios_base::event ev, std::ios_base& iosobj, int index)
{
    x = index;

    switch (ev)
    {
        case std::ios_base::copyfmt_event:
            (void) fprintf(stderr, "copyfmt_event\n"); break;
        case std::ios_base::imbue_event:
            (void) fprintf(stderr, "imbue_event\n"); break;
        case std::ios_base::erase_event:
            (void) fprintf(stderr, "erase_event\n"); break;
        default:
            (void) fprintf(stderr, "unknown (this should never happen)\n"); break;
    }
}


int main ()
{
    struct A: std::streambuf { };

    struct B: std::ios {
        A sb;
        B () { init (&sb); }
    } f0, f1;

    int i = 101;
    int ret = 1;

    std::ios_base::event_callback e1 = &testfun;
    f0.register_callback (e1, i);

    x = 0;
    f0.imbue (f0.getloc());
    TEST_ASSERT (x == i);

    x = 0;
    ret = 1;
    f0.copyfmt (f1);
    TEST_ASSERT (x == i);

	B s0, s1;

	s0.register_callback (e1, i);

	x = 0;
	ret = 1;
	s0.imbue (f0.getloc ());
	TEST_ASSERT (x == i);

	x = 0;
	ret = 1;
	s0.copyfmt (s1);
	TEST_ASSERT (x == i);

    return ret;
}

