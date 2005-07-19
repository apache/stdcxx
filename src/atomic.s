/***************************************************************************
 *
 * atomic.s
 *
 * $Id: //stdlib/dev/source/stdlib/atomic.s#11 $
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


#if defined (__i386__)
#  include "atomic-i86.s"
#endif

#if defined (__ia64)
#  include "atomic-ia64.s"
#endif

#if defined (__parisc)
#  if defined (__LP64__)
#    include "atomic-pa2-64.s"
#  else
#    include "atomic-pa2.s"
#  endif
#endif 

#if defined (__sparc)
#  if defined (__sparcv9)
#    include "atomic-sparc64.s"
#  else
#    include "atomic-sparc.s"
#  endif
#endif 
