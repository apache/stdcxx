// checking for ctype constants

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

#include <ctype.h>
#include <stdio.h>


int main ()
{
    struct {
        const char* name;
        int         val;
    } bits[] = {

        {
            "UPPER",
#if defined (_ISUPPER)
            _ISUPPER
#elif defined (_ISupper)
            _ISupper
#elif defined (_U)
            _U
#elif defined (__GLIBC__)
            _ISupper,
#else
            -1
#endif
        },

        {
            "LOWER",
#if defined (_ISLOWER)
            _ISLOWER
#elif defined (_ISlower)
            _ISlower
#elif defined (_L)
            _L
#elif defined (__GLIBC__)
            _ISlower,
#else
            -1
#endif
        },

        {
            "DIGIT",
#if defined (_ISDIGIT)
            _ISDIGIT
#elif defined (_ISdigit)
            _ISdigit
#elif defined (_D)
            _D
#elif defined (__GLIBC__)
            _ISdigit,
#else
            -1
#endif
        },

        {
            "SPACE",
#if defined (_ISSPACE)
            _ISSPACE
#elif defined (_ISspace)
            _ISspace
#elif defined (_S)
            _S
#elif defined (__GLIBC__)
            _ISspace,
#else
            -1
#endif
        },

        {
            "PUNCT",
#if defined (_ISPUNCT)
            _ISPUNCT
#elif defined (_ISpunct)
            _ISpunct
#elif defined (_P)
            _P
#elif defined (__GLIBC__)
            _ISpunct,
#else
            -1
#endif
        },

        {
            "CNTRL",
#if defined (_ISCNTRL)
            _ISCNTRL
#elif defined (_IScntrl)
            _IScntrl
#elif defined (_C)
            _C
#elif defined (__GLIBC__)
            _IScntrl,
#else
            -1
#endif
        },

        {
            "BLANK",
#if defined (_ISBLANK)
            _ISBLANK
#elif defined (_ISblank)
            _ISblank
#elif defined (_B)
            _B
#elif defined (__GLIBC__)
            _ISblank,
#else
            -1
#endif
        },

        {
            "XDIGIT",
#if defined (_ISXDIGIT)
            _ISXDIGIT
#elif defined (_ISxdigit)
            _ISxdigit
#elif defined (_X)
            _X
#elif defined (__GLIBC__)
            _ISxdigit,
#else
            -1
#endif
        },

        {
            "GRAPH",
#if defined (_ISGRAPH)
            _ISGRAPH
#elif defined (_ISgraph)
            _ISgraph
#elif defined (__GLIBC__)
            _ISgraph,
#else
            -1
#endif
        },

        {
            "ALPHA",
#if defined (_ISALPHA)
            _ISALPHA
#elif defined (_ISalpha)
            _ISalpha
#elif defined (__GLIBC__)
            _ISalpha,
#else
            -1
#endif
        },

        {
            "PRINT",
#if defined (_ISPRINT)
            _ISPRINT
#elif defined (_ISprint)
            _ISprint
#elif defined (__GLIBC__)
            _ISprint,
#else
            -1
#endif
        },

        {
            "ALNUM",
#if defined (_ISALNUM)
            _ISALNUM
#elif defined (_ISalnum)
            _ISalnum
#elif defined (__GLIBC__)
            _ISalnum,
#else
            -1
#endif
        }
    };

    int used = 0;

    for (unsigned i = 0; i != sizeof bits / sizeof *bits; ++i) {

        if (-1 != bits [i].val) {
            used |= bits [i].val;

            printf ("#define _RWSTD_CTYPE_%s %#010x"
                    "   /* libc value */\n",
                    bits [i].name, bits [i].val);
        }
    }

    for (unsigned j = 0; j != sizeof bits / sizeof *bits; ++j) {
        if (-1 == bits [j].val) {
            for (unsigned i = 0; i != sizeof used * 8; ++i) {
                if (!(used & (1 << i))) {

                    used |= 1 << i;

                    printf ("#define _RWSTD_CTYPE_%s %#010x"
                            "   /* computed */\n",
                            bits [j].name, 1 << i);

                    break;
                }
            }
        }
    }

    return 0;
}
