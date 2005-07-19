/***************************************************************************
 *
 * setjmp.h
 *
 * $Id: //stdlib/dev/include/ansi/setjmp.h#12 $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#ifndef _RWSTD_SETJMP_H_INCLUDED
#define _RWSTD_SETJMP_H_INCLUDED


#include <rw/_defs.h>


#ifndef _RWSTD_NO_DEPRECATED_C_HEADERS

#  include <csetjmp>

#  ifndef _RWSTD_NO_NAMESPACE

using std::jmp_buf;

using std::longjmp;

#  endif   // _RWSTD_NO_NAMESPACE
#else
#  include _RWSTD_ANSI_C_SETJMP_H
#endif   // _RWSTD_NO_DEPRECATED_C_HEADERS

#endif   // _RWSTD_SETJMP_H_INCLUDED
