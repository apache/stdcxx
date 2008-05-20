/***************************************************************************
 *
 * ti_num_get.cpp - explicit instantiation directives for num_get<char>
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#define _RWSTD_INSTANTIATE_TEMPLATES   1
#define _RWSTD_INSTANTIATE_PUNCT       1
#define _RWSTD_INSTANTIATE_NUM_GET     1
#define _RWSTD_INSTANTIATE_CHAR        1

#include <rw/_defs.h>

#undef _RWSTD_NO_TEMPLATE_DEFINITIONS

#include <loc/_locale.h>
#include <loc/_num_get.h>
#include <loc/_punct.h>

#define _RWSTD_NO_BYNAME_FACET
#include "use_facet.h"


#define TARGS_C   <char, _RWSTD_ISTREAMBUF_ITER (char) >


#if 6 == _RWSTD_HP_aCC_MAJOR
   // silence the useless HP aCC 6 remark 4244: extern storage class
   // used with a function definition on the "extern _RWSTD_EXPORT"
   // macro argument below where the redundant "extern" is there to
   // silence HP aCC 3 Warning (suggestion) 933: Null macro argument #1
#  pragma diag_suppress 4244
#endif   // aCC 6


_RWSTD_SPECIALIZE_FACET_ID (num_get, TARGS_C);
_RWSTD_DEFINE_FACET_FACTORY (extern _RWSTD_EXPORT, num_get, TARGS_C, num_get);
_RWSTD_SPECIALIZE_USE_FACET (num_get);
