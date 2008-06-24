// checking if integer math traps

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

#include "nodbg.h"


#if defined (_MSC_VER)
#  define TRY             __try
#  define EXCEPT(expr)    __except (expr)
#else   // not MSVC
#  define TRY               if (1)
#  define EXCEPT(ignore)    else if (0)
#endif   // _MSC_VER


int get_int (int);


volatile int int_zero;
volatile int int_one;


int main (int argc, char*[])
{
    nodbg ();

    // test expects to be called with no command line arguments
    // i.e., (argc < 2) is expected to hold

    // argc used to try to foil optimizers
    int_zero = get_int (argc);
    int_one  = get_int (argc);

    int result [2];

    TRY {
        // use both division and modulo to try to foil optimizers
        result [0] = int_one / int_zero;
        result [1] = int_one % int_zero;
    }
    EXCEPT (1) {
        return 1;
    }

    // NEGATIVE test: successful exit status indicates a failure
    return argc < 2 ? result [0] : result [1];
}

// use volatile to try to foil optimizers
volatile int int_value;

// use recursion to try to foil optimizers
int get_int (int arg)
{
    if (1 < arg)
        return get_int (arg - 1) + get_int (arg - 2);

    const int value = int_value;

    ++int_value;

    return value;
}
