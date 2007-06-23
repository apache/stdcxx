/***************************************************************************
 *
 * _math.h - Standard Library vs math.h exception conflict hack.
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
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

#ifndef _RWSTD_RW_MATH_H_INCLUDED
#define _RWSTD_RW_MATH_H_INCLUDED

#ifndef _RWSTD_RW_DEFS_H_INCLUDED
#  include <rw/_defs.h>
#endif   // _RWSTD_RW_DEFS_H_INCLUDED

// avoid a conflicting exception structure on platforms where
// struct exception is defined unguarded in <math.h>
#ifndef _RWSTD_NO_MATH_EXCEPTION
#  undef exception
#  define exception math_exception
#endif   // _RWSTD_NO_MATH_EXCEPTION

#include _RWSTD_CMATH

#undef exception

// MSVC provides its own complex macro
#ifdef _MSC_VER
#  ifdef complex
#    undef complex
#  endif
#endif


#endif   // _RWSTD_RW_MATH_H_INCLUDED
