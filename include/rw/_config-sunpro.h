/***************************************************************************
 *
 * _config-xlc.h - IBM VisualAge/XLC++ configuration definitions
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

// _REENTRANT defined by the -mt compiler option

#if __SUNPRO_CC <= 0x540
   // works around a partial specialization bug (PR #28119)
#  ifndef _RWSTD_NO_EXT_CONST_ALLOCATOR
#    define _RWSTD_NO_EXT_CONST_ALLOCATOR
#  endif
#endif   // SunPro < 5.4

#ifndef _RWSTD_NO_EXPORT
#  define _RWSTD_NO_EXPORT
#endif   // _RWSTD_NO_EXPORT

// prevent warnings about SunPro being unable to find
// source for (intentionally undefined) member functions
// of class templates
#define _RWSTD_NO_UNDEFINED_TEMPLATES
