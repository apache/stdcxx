/************************************************************************
 *
 * util.h - Utility macros / prototypes for the runall utility
 *
 * $Id$
 *
 ************************************************************************
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
 **************************************************************************/

#ifndef RW_UTIL_H
#define RW_UTIL_H

void terminate ( const int state, const char* const format, ... );

/* Note: RW_MALLOC should be used rather than malloc within the runall 
   utility.  This macro calls the guarded_malloc function which performs 
   validation on inputs and outputs.  guarded_malloc shouldn't be called 
   directly, as it needs file and line information, provided by RW_MALLOC.
*/

#define RW_MALLOC(size)                             \
    guarded_malloc(size, __FILE__, __LINE__)
void* guarded_malloc (const size_t size, const char* const file, 
                      const unsigned line);

#endif   // RW_UTIL_H
