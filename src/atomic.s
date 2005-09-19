/***************************************************************************
 *
 * atomic.s
 *
 * $Id$
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
#  include "i86/atomic.s"
#endif

#if defined (__ia64)
#  include "ia64/atomic.s"
#endif

#if defined (__parisc)
#  if defined (__LP64__)
#    include "parisc/atomic-64.s"
#  else
#    include "parisc/atomic.s"
#  endif
#endif 

#if defined (__sparc)
#  if defined (__sparcv9)
#    include "sparc/atomic-64.s"
#  else
#    include "sparc/atomic.s"
#  endif
#endif 
