/************************************************************************
 *
 * 22.locale.stdcxx-554.cpp - the regression test for STDCXX-554 issue
 *
 * $Id$
 *
 ***************************************************************************
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

#include <cstring>   // for memset()

#include <locale>    // for std::moneypunct, std::messages

#include <driver.h>  // for rw_test()

template <class charT>
void test_moneypunct (charT, const char* name)
{
    typedef std::moneypunct <charT> PunctT;

    const char fill = '\xdc';

    char buf [sizeof (PunctT) + 1];
    std::memset (buf, fill, sizeof (buf));

    PunctT* p = new (buf) PunctT ();

    rw_assert (fill == buf [sizeof (PunctT)], __FILE__, __LINE__,
               "buf [sizeof (std::moneypunct <%s>)] expected %d, got %d\n",
               name, int (fill), int (buf [sizeof (PunctT)]));

    p->~PunctT ();
}

template <class charT>
void test_messages (charT, const char* name)
{
    typedef std::messages <charT> MessagesT;

    const char fill = '\xdc';

    char buf [sizeof (MessagesT) + 1];
    std::memset (buf, fill, sizeof (buf));

    MessagesT* p = new (buf) MessagesT ();

    rw_assert (fill == buf [sizeof (MessagesT)], __FILE__, __LINE__,
               "buf [sizeof (std::messages <%s>)] expected %d, got %d\n",
               name, int (fill), int (buf [sizeof (MessagesT)]));

    p->~MessagesT ();
}

static int run_test (int, char**)
{
    test_moneypunct (char (), "char");
    test_messages (char (), "char");

#ifndef _RWSTD_NO_WCHAR_T
    test_moneypunct (wchar_t (), "wchar_t");
    test_messages (wchar_t (), "wchar_t");
#endif

    return 0;
}

int main (int argc, char* argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.locale",
                    "STDCXX-544",
                    run_test,
                    "",
                    (void*)0);
}
