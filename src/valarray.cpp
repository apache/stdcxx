/***************************************************************************
 *
 * valaray - Declarations for the Standard Library valarray
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#include <valarray>
#include <string.h>   // for memset()

#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 


_RWSTD_SIZE_T gslice::next_ind ()
{
    _RWSTD_SIZE_T __n = _C_length.size ();

    for (/**/; __n; --__n)
    {
        if (   _C_length [__n - 1]
            && _C_r_length [__n - 1] != _C_length [__n - 1] - 1)
            break;
    }

    if (0 == __n) {
        _C_reset    = true;
        _C_r_length = 0;
        return _C_start;
    }
     
    if (_C_reset) {
        _C_reset = false;
        return _C_start;
    }

    _RWSTD_ASSERT (0 < __n);

    ++_C_r_length [__n - 1];

    if (__n < _C_r_length.size ())
        ::memset (&_C_r_length [__n], 0,
                  (_C_r_length.size () - __n) * sizeof _C_r_length [__n]);
    else
        __n = _C_r_length.size ();

    _RWSTD_SIZE_T __inx = _C_start;
    for (_RWSTD_SIZE_T __i = 0; __i != __n; ++__i)
        __inx += _C_r_length [__i] * _C_stride [__i];

    return __inx;
}

}   // namespace std
