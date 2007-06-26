/***************************************************************************
 *
 * _basic_ios.cc - definitions of basic_ios members
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


_RWSTD_NAMESPACE (std) {


#ifdef _RWSTD_NO_UNDEFINED_TEMPLATES

template <class _CharT, class _Traits>
basic_ios<_CharT, _Traits>::
basic_ios (const basic_ios<_CharT, _Traits>&)
{
    _RWSTD_ASSERT (!"not callable");
}


template <class _CharT, class _Traits>
basic_ios<_CharT, _Traits>&
basic_ios<_CharT, _Traits>::
operator= (const basic_ios<_CharT, _Traits>&)
{
    _RWSTD_ASSERT (!"not callable");

    return *this;
}

#endif   // _RWSTD_NO_UNDEFINED_TEMPLATES


}   // namespace std
