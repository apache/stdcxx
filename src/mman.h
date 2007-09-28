/***************************************************************************
 *
 * mman.h
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
 * Copyright 2001-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _RWSTD_MMAN_H_INCLUDED
#define _RWSTD_MMAN_H_INCLUDED


#include <rw/_defs.h>


_RWSTD_NAMESPACE (__rw) {


// maps a named file into memory as shared, read-only, returns
// the beginning address on success and fills 'size' with the
// size of the file; returns 0 on failure
void* __rw_mmap (const char*, _RWSTD_SIZE_T*);

// unmaps a memory region
void __rw_munmap (const void*, _RWSTD_SIZE_T);


}   // namespace __rw


#endif   // _RWSTD_MMAN_H_INCLUDED
