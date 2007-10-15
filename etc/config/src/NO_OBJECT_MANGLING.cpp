// checking for object name mangling

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

#ifdef _RWSTD_object_mangling_imp
  // link with the object file below
  // LDOPTS = object_mangling_imp.o
#endif   // _RWSTD_object_mangling_imp

// declared but not defined here (defined in object_mangling_imp.cpp)
extern int declared_int_defined_char_array;

int main ()
{
    // return 0 to indicate that the object has non-zero value
    return !declared_int_defined_char_array;
}
