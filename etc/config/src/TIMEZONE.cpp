// checking for int timezone in <time.h>

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

#include <time.h>

int get_timezone_value ()
{
    // POSIX requires a timezone variable:
    // http://www.opengroup.org/onlinepubs/009695399/functions/timezone.html

    // BSD-based systems such as Darwin define a char* timezone(int, int)
    // function that has nothing to do with the variable:
    // http://developer.apple.com/documentation/Darwin/Reference/ManPages/ \
    // man3/timezone.3.html

    // check to see if a timezone variable exists and is convertible to int
    return timezone / 2;
}
