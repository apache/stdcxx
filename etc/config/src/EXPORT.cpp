// checking for exported templates

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

#ifndef _RWSTD_NO_EXPORT_KEYWORD

// establish a dependency on EXPORT_KEYWORD.cpp which must be
// compiled first since it defines the exported function template
// LDOPTS = EXPORT_KEYWORD.o 
#endif   // _RWSTD_NO_EXPORT_KEYWORD


export template <class T>
T exported_function_template (T);

export template <class T>
struct ExportedClass
{
    T exported_class_member (T);
};

template <class T>
T ExportedClass<T>::
exported_class_member (T t) {
    return exported_function_template (t);
}


struct MyClass {
    int i_;
    MyClass (): i_ (0) { }
};


int main ()
{
    MyClass x;

    return ExportedClass<MyClass>().exported_class_member (x).i_;
}
