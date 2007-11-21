// checking for munmap() in <sys/mman.h>

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

// include <sys/types.h> first in case mman.h depends
// on it (such as the one on Darwin -- see STDCXX-107)
#include <sys/types.h>   // for size_t
#include <sys/mman.h>    // for munmap()

extern "C" {

typedef int (f_i_vpui)(void*, size_t);
typedef int (f_i_cpui)(char*, size_t);

}


const char* munmap_arg2_type_imp (f_i_vpui*) { return "void*"; }
const char* munmap_arg2_type_imp (f_i_cpui*) { return "char*"; }

// determine whether the type of the second argument
// to munmap() is void* or char*
const char* get_munmap_arg2_type ()
{
    return munmap_arg2_type_imp (&munmap);
}


// include last so as not to pollute the global namespace
// and skew the result of the test
#include <stdio.h>


int main ()
{
    const char *argtype = get_munmap_arg2_type ();

    printf ("#define _RWSTD_MUNMAP_ARG1_T %s\n", argtype);

    return 0;
}
