/***************************************************************************
 *
 * ti_wnum_put.cpp - explicit instantiation directives for num_put<wchar_t>
 *
 * $Id: //stdlib/dev/source/stdlib/ti_wnum_put.cpp#6 $
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

#define _RWSTD_LIB_SRC

#define _RWSTD_INSTANTIATE_TEMPLATES   1
#define _RWSTD_INSTANTIATE_NUM_PUT     1
#define _RWSTD_INSTANTIATE_WCHAR_T     1

#include <rw/_defs.h>

#undef _RWSTD_NO_TEMPLATE_DEFINITIONS

#include <loc/_locale.h>
#include <loc/_num_put.h>

#define _RWSTD_NO_BYNAME_FACET
#include "use_facet.h"


#ifndef _RWSTD_NO_WCHAR_T

#  define TARGS_W   <wchar_t, _RWSTD_OSTREAMBUF_ITER (wchar_t) >

_RWSTD_SPECIALIZE_FACET_ID (num_put, TARGS_W);
_RWSTD_DEFINE_FACET_FACTORY (static, num_put, TARGS_W, wnum_put);
_RWSTD_SPECIALIZE_USE_FACET (wnum_put);

#endif   // _RWSTD_NO_WCHAR_T
