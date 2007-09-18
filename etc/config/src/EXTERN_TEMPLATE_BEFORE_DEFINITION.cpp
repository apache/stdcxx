// checking for extern template before definition

/***************************************************************************
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
 * Copyright 1999-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include "config.h"

#ifndef _RWSTD_NO_extern_template_before_definition_imp
  // establish a dependency on the source file to get it
  // compiled first and  link with the object file
  // LDOPTS = extern_template_before_definition_imp.o
#endif

#ifdef _RWSTD_NO_IMPLICIT_INCLUSION
   // tell "instantiation_before_definition.h" to #include its
   // implementation file, instantiation_before_definition.cc
#  define INCLUDE_CC_FILE
#endif   // _RWSTD_NO_IMPLICIT_INCLUSION

#ifdef _RWSTD_NO_EXTERN_FUNCTION_TEMPLATE
   // tell "instantiation_before_definition.h" to use explicit
   // function template specialization instead of the extern
   // template extension
#  define SPECIALIZE_FUNCTION_TEMPLATE
#endif   // _RWSTD_NO_EXTERN_FUNCTION_TEMPLATE

#define EXTERN_TEMPLATE
#include "instantiation_before_definition.h"

// test exercises the ability to declare an explicit instantiation
// of a template using the "extern template" syntax and lexically
// before the definition of the template (exercises, for example,
// gcc 4.0.2 bug #24511:
// http://gcc.gnu.org/bugzilla/show_bug.cgi?id=24511)

int main ()
{
    InstantiatedBeforeDefined<int> ibd;

    return   ibd.defined_first (0)
           + ibd.instantiated_first (0)
           + instantiated_before_defined (0);
}
