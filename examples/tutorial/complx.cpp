/**************************************************************************
 *
 * complx.cpp - Complex Number example program.
 *
 * $Id$
 *
 ***************************************************************************
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <complex>
#include <utility>
#include <iostream>
    
#include <examples.h>

typedef std::complex<double> dcomplex;

// Return roots of a quadratic equation.
std::pair<dcomplex, dcomplex>
quadratic (dcomplex a, dcomplex b, dcomplex c) {

    dcomplex root = std::sqrt(b * b - 4.0 * a * c);
    a = a * 2.0;
    return std::make_pair ((-b + root) / a, (-b - root) / a);
}


int main () {
    
    dcomplex a(2, 3);
    dcomplex b(4, 5);
    dcomplex c(6, 7);
    
    std::pair<dcomplex, dcomplex> ans = quadratic(a, b, c);

    std::cout << "Roots are " << ans.first << " and "
              << ans.second   << std::endl;

    return 0;
}








