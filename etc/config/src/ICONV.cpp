// checking for iconv() in <iconv.h>

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

#include <iconv.h>
#include <stddef.h>

extern "C" {

typedef size_t (f_i_cppc)(iconv_t, const char**, size_t*, char**, size_t*);
typedef size_t (f_i_cpp) (iconv_t,       char**, size_t*, char**, size_t*);

}


const char* foo (f_i_cppc*) { return "const char**"; }
const char* foo (f_i_cpp*)  { return "char**"; }


const char* bar ()
{
    return foo (&iconv);
}


// include last so as not to pollute the global namespace
// and skew the result of the test
#include <stdio.h>


int main ()
{
    const char *argtype = bar ();

    if ('o' == argtype [1]) {
        // iconv() second argument type is const char**
        printf ("// ");
    }

    // may be commented out in the printf statement above
    printf ("#define _RWSTD_NO_ICONV_CONST_CHAR\n");

    printf ("#define _RWSTD_ICONV_ARG2_T %s\n", argtype);

    return 0;
}
