// -*- C++ -*-
/***************************************************************************
 *
 * _cstdarg.h - C++ Standard library interface to the ANSI C header stdarg.h
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

#ifndef _RWSTD_CSTDARG_H_INCLUDED
#define _RWSTD_CSTDARG_H_INCLUDED


#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 

extern "C" {

#ifdef _RWSTD_VA_LIST
typedef _RWSTD_VA_LIST va_list;
#else
typedef void* va_list;
#endif   // _RWSTD_VA_LIST


#define va_arg(va, T) \
    ((T*)(va = (va_list)((char*)va + sizeof (T))))[-1]

#define va_start(va, name) \
    (void)(va = (va_list)((char*)&name \
           + ((sizeof (name) + (sizeof (int) - 1)) & ~(sizeof (int) - 1))))

#define va_end(ignore)   (void)0

}   // extern "C"

}   // namespace std


#endif   // _RWSTD_CSTDARG_H_INCLUDED
