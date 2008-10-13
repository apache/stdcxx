/***************************************************************************
 *
 * atomic.s
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
 * Copyright 2003-2006 Rogue Wave Software.
 * 
 **************************************************************************/


#if defined (__i386__)
#  include "x86/atomic.s"
#elif defined (__x86_64) || defined (__x86_64__)
#  include "x86_64/atomic.s"
#elif defined (__ia64) || defined (__ia64__)
#  if defined (_LP64) || defined (__LP64__)
#    include "ia64/atomic-64.s"
#  else
#    include "ia64/atomic.s"
#  endif
#elif defined (__parisc) || defined (__parisc__)
#  if defined (__LP64__)
#    include "parisc/atomic-64.s"
#  else
#    include "parisc/atomic.s"
#  endif
#elif defined (__sparc) || defined (__sparc__)
#  if defined (__sparcv9) || defined (__sparcv9__)
#    include "sparc/atomic-64.s"
#  else
#    include "sparc/atomic.s"
#  endif
#endif 
