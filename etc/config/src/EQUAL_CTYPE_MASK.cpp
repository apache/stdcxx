// checking that ctype mask's for char and wchar_t are equal

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

#include <locale.h>
#include <ctype.h>

#ifndef _RWSTD_NO_WCTYPE_H
#  include <wctype.h>
#endif

int main ()
{
    setlocale (LC_ALL, "C");

    for (int i = 0; i < 256; ++i) {

#ifndef _RWSTD_NO_ISWSPACE
        if (!!isspace (i)  != !!iswspace (i))
            return 1;
#endif

#ifndef _RWSTD_NO_ISWPRINT
        if (!!isprint (i)  != !!iswprint (i))
            return 1;
#endif

#ifndef _RWSTD_NO_ISWCNTRL
        if (!!iscntrl (i)  != !!iswcntrl (i))
            return 1;
#endif

#ifndef _RWSTD_NO_ISWUPPER
        if (!!isupper (i)  != !!iswupper (i))
            return 1;
#endif

#ifndef _RWSTD_NO_ISWLOWER
        if (!!islower (i)  != !!iswlower (i))
            return 1;
#endif

#ifndef _RWSTD_NO_ISWALPHA
        if (!!isalpha (i)  != !!iswalpha (i))
            return 1;
#endif

#ifndef _RWSTD_NO_ISWDIGIT
        if (!!isdigit (i)  != !!iswdigit (i))
            return 1;
#endif

#ifndef _RWSTD_NO_ISWPUNCT
        if (!!ispunct (i)  != !!iswpunct (i))
            return 1;
#endif

#ifndef _RWSTD_NO_ISWXDIGIT
        if (!!isxdigit (i) != !!iswxdigit (i))
            return 1;
#endif

#ifndef _RWSTD_NO_ISWALNUM
        if (!!isalnum (i)  != !!iswalnum (i))
            return 1;
#endif

#ifndef _RWSTD_NO_ISWGRAPH
        if (!!isgraph (i)  != !!iswgraph (i))
            return 1;
#endif
    }

    return 0;
}
