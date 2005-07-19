// -*- C++ -*-
/***************************************************************************
 *
 * _cctype.h - C++ Standard library interface to the ANSI C header ctype.h
 *
 * $Id: //stdlib/dev/include/ansi/_cctype.h#7 $
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

#ifndef _RWSTD_CCTYPE_H_INCLUDED
#define _RWSTD_CCTYPE_H_INCLUDED

#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 

extern "C" {

int isalnum (int);
int isalpha (int);
int iscntrl (int);
int isdigit (int);
int isgraph (int);
int islower (int);
int isprint (int);
int ispunct (int);
int isspace (int);
int isupper (int);
int isxdigit (int);
int tolower (int);
int toupper (int);

}

}   // namespace std


#endif   // _RWSTD_CCTYPE_H_INCLUDED
