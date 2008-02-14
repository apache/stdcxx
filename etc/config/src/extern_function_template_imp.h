
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

// avoid including <config.h> here to avoid inadvertently
// trying to introduce dependencies on other config tests
// that the config infrastructure won't know about (since
// it only searches .cpp files for them)
// #include <config.h>

#if __GNUG__ >= 3
   // disable gcc 3.x (and beyond) error: ISO C++ forbids the use
   // of `extern' on explicit instantiations
#  pragma GCC system_header
#endif   // gcc >= 3


template <class T>
struct S
{
    T t;
};

template <class T>
S<T> foobar (S<T> s)
{
#if defined (INSTANTIATE)
    s.t = 1;
#else   // if !defined (INSTANTIATE)
    s.t = -1;
#endif   // INSTANTIATE

    return s;
}

#if defined (INSTANTIATE)

template S<int> foobar (S<int>);

#else   // if !defined (INSTANTIATE)

extern template S<int> foobar (S<int>);

#endif   // INSTANTIATE
