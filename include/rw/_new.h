// -*- C++ -*-
/***************************************************************************
 *
 * _new.h - definitions of placement operator new and delete
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

#ifndef _RWSTD_RW_NEW_H_INCLUDED
#define _RWSTD_RW_NEW_H_INCLUDED

#ifndef _RWSTD_RW_DEFS_H_INCLUDED
#  include <rw/_defs.h>
#endif   // _RWSTD_RW_DEFS_H_INCLUDED


#if defined (__SUNPRO_CC)
#  include <new>
#else   // if !defined (__SUNPRO_CC)

void* operator new (_RWSTD_SIZE_T, void*) _NEW_THROWS (());

#  ifdef _RWSTD_NO_OPERATOR_NEW_PLACEMENT

#    if !defined (_MSC_VER) || !defined (__PLACEMENT_NEW_INLINE)

       // remember to disable definitions of placement new and
       // placement delete in MSVC's <new.h> later in this file
#      define _RWSTD_DEFINE_PLACEMENT_NEW_INLINE

// 18.4.1.3, p2 - not replaceable, no-op
inline void*
operator new (_RWSTD_SIZE_T, void *__ptr) _NEW_THROWS (())
{
    return __ptr;
}

#    endif   // !MSVC || !__PLACEMENT_NEW_INLINE
#  endif   // _RWSTD_NO_OPERATOR_NEW_PLACEMENT


#  ifndef _RWSTD_NO_PLACEMENT_DELETE

#    if !defined (_MSC_VER) || !defined (__PLACEMENT_NEW_INLINE)

void operator delete (void*, void*) _NEW_THROWS (());

#      ifdef _RWSTD_NO_OPERATOR_DELETE_PLACEMENT

       // remember to disable definitions of placement new and
       // placement delete in MSVC's <new.h> later in this file
#      define _RWSTD_DEFINE_PLACEMENT_NEW_INLINE

// not replaceable, no-op
inline void
operator delete (void*, void*) _NEW_THROWS (())
{
    // no-op
}

#      endif   // _RWSTD_NO_OPERATOR_DELETE_PLACEMENT
#    endif   // !MSVC || !__PLACEMENT_NEW_INLINE
#  endif   // _RWSTD_NO_PLACEMENT_DELETE

#  ifdef _RWSTD_DEFINE_PLACEMENT_NEW_INLINE
#    define __PLACEMENT_NEW_INLINE
#    undef _RWSTD_DEFINE_PLACEMENT_NEW_INLINE
#  endif   // _RWSTD_DEFINE_PLACEMENT_NEW_INLINE

#endif   // __SUNPRO_CC


#endif   // _RWSTD_RW_NEW_H_INCLUDED
