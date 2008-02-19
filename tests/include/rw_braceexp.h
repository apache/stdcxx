/************************************************************************
 *
 * rw_braceexp.h - declarations of testsuite helpers
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

#ifndef RW_BRACEEXP_H_INCLUDED
#define RW_BRACEEXP_H_INCLUDED


#include <testdefs.h>


// rw_brace_expand() performs brace expansion according to chapter 3.5.1 of
// the Bash Reference Manual with the exception of special handling for the
// string `${'. This function is, for the most part, consistent with brace
// expansion used by bash 3.0.

//
// this function will attempt to expand the brace expression `brace_expr'
// into `n' bytes of the output buffer `s', seperating each expansion with
// a single seperator character `sep'. if the output buffer `s' is is null,
// or the number of bytes `n' is insufficient to contain all expansions of
// `brace_expr', an appropriately sized buffer will be allocated with
// malloc(). a pointer to the output buffer that is used will be returned.
// if the pointer returned is not equal to the user supplied input buffer
// `s', then the caller is expected to free() the returned pointer at some
// point.
//
// this function can return null if the brace expansion could not be
// processed.
//
_TEST_EXPORT char*
rw_brace_expand (const char* brace_expr, char* s, _RWSTD_SIZE_T n, char sep);


#endif   // RW_BRACEEXP_H_INCLUDED

