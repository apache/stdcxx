/***************************************************************************
 *
 * podarray.cc
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

_RWSTD_NAMESPACE (__rw) { 

template <class _TypeT, _SizeT _Size>
__rw_pod_array<_TypeT, _Size>&
__rw_pod_array<_TypeT, _Size>::append (const _TypeT *__a, _SizeT __len)
{
    const _SizeT __newlen = _C_len + __len;

    if (_C_len + __len >= _Size) {
        _TypeT* const __tmp = new _TypeT [__newlen + 1];

        memcpy (__tmp, _C_pbuf, _C_len * sizeof *__tmp);

        if (_C_pbuf != _C_buffer)
            delete[] _C_pbuf;

        _C_pbuf = __tmp;
    }

    memcpy (_C_pbuf + _C_len, __a, __len * sizeof *_C_pbuf);

    _C_len = __newlen;

    _C_pbuf [_C_len] = _TypeT ();

    return *this;
}

}   // namespace __rw
