// checking for madvise() in <sys/mman.h>

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

#include <sys/mman.h>    // for madvise(), MADV_WILLNEED
#include <sys/types.h>   // for caddr_t


int main (int argc, char *argv[])
{
    const caddr_t addr = (caddr_t)argv;

    const int result = madvise (addr, argc, MADV_WILLNEED);

    if (1 < argc) {
        // invoke with any command line arguments to test
        return result;
    }

    // always succeed when invoked without arguments
    return 0;
}
