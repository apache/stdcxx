// checking function-try-block

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

int tried;
int caught;

template <class T>
T foo (T t)
    try {   // <-- testing
        ++tried;
        throw t;
    }
    catch (T) {
        ++caught;
        throw;
    }


struct S
{
    S (int i)
        try   // <-- testing
        : i_ (i) {
            ++tried;
            foo (i_);
        }
        catch (int) {
            ++caught;
            throw;
        }

    int i_;
};


int main ()
{
    int result = 0;

    try {
        S s (123);

        (void)&s;

        result |= 1;
    }
    catch (int i) {
        if (i != 123)
            result |= 2;

        if (tried != 2)
            result |= 4;

        if (caught != 2)
            result |= 8;
    }
    catch (...) {
        result |= 0x10;
    }

    return result;
}
