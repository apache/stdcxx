/***************************************************************************
 *
 * memchk.cpp - declarations of memory checking helper functions
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

#ifndef RWSTD_MEMCHK_H_INCLUDED
#define RWSTD_MEMCHK_H_INCLUDED

#include <cstddef>

// checks if the first argument points to a readable region of memory
// of the given size; returns the size on success, -1 otherwise
std::size_t memchk (const void*, std::size_t);

// checks of the argument points to a valid NUL-terminated character
// string; returns the length of the string on success, -1 otherwise
std::size_t strchk (const char*);

#endif   // RWSTD_MEMCHK_H_INCLUDED
