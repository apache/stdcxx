/***************************************************************************
 *
 * ti_numpunct.cpp - explicit instantiation directives for numpunct<char>
 *
 * $Id: //stdlib/dev/source/stdlib/ti_numpunct.cpp#10 $
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

#define _RWSTD_INSTANTIATE_TEMPLATES  1
#define _RWSTD_INSTANTIATE_NUMPUNCT   1
#define _RWSTD_INSTANTIATE_CHAR       1

#include <rw/_defs.h>

#undef _RWSTD_NO_TEMPLATE_DEFINITIONS

#include <streambuf>

#include <loc/_locale.h>
#include <loc/_numpunct.h>
#include <loc/_punct.h>

#include "use_facet.h"


_RWSTD_SPECIALIZE_FACET_ID (numpunct, <char>);
_RWSTD_DEFINE_FACET_FACTORY (_RWSTD_EXPORT, numpunct, <char>, numpunct);
_RWSTD_SPECIALIZE_USE_FACET (numpunct);
