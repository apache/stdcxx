// -*- C++ -*-
/***************************************************************************
 *
 * _cassert.h - C++ Standard library interface to the ANSI C header assert.h
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

#include <rw/_defs.h>

#undef assert

#if !defined (NDEBUG)
#  if defined (__GNUG__) || defined (__HP_aCC)
#    define assert(expr)                                                   \
     ((expr) ? (void)0 : _RW::__rw_assert_fail (#expr, __FILE__, __LINE__, \
                                                __PRETTY_FUNCTION__))
#  elif !defined (_RWSTD_NO_FUNC)
#    define assert(expr)                                                   \
     ((expr) ? (void)0 : _RW::__rw_assert_fail (#expr, __FILE__, __LINE__, \
                                           __func__))
#  else   // if !__GNUG__ && _RWSTD_NO_FUNC
#    define assert(expr)   \
     ((expr) ? (void)0 : _RW::__rw_assert_fail (#expr, __FILE__, __LINE__, 0))
#  endif
#else   //  if !defined (NDEBUG)
#  define assert(ignore)   ((void)0)
#endif   // NDEBUG
