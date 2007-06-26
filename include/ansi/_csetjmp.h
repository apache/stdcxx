// -*- C++ -*-
/***************************************************************************
 *
 * csetjmp - C++ Standard library interface to the ANSI C header setjmp.h
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

#ifndef _RWSTD_CSETJMP_H_INCLUDED
#define _RWSTD_CSETJMP_H_INCLUDED

#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 


extern "C" {

typedef char jmp_buf [8];

int setjmp (jmp_buf);
void longjmp (jmp_buf, int);

}   // extern "C"

}   // namespace std


// 18.7, p1, Table 20
#define setjmp   _RWSTD_C::setjmp


#endif   // _RWSTD_CSETJMP_H_INCLUDED
