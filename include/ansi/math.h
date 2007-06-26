/***************************************************************************
 *
 * math.h - C++ Standard library wrapper for the ANSI C library header
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

#include <rw/_defs.h>

#ifndef _RWSTD_NO_DEPRECATED_C_HEADERS
#  ifndef _RWSTD_MATH_H_INCLUDED
#    define _RWSTD_MATH_H_INCLUDED

#    include <cmath>

#    ifndef _RWSTD_NO_NAMESPACE

using std::acos;
using std::asin;
using std::atan2;
using std::atan;
using std::ceil;
using std::cos;
using std::cosh;
using std::exp;
using std::fabs;
using std::floor;
using std::fmod;
using std::frexp;
using std::ldexp;
using std::log10;
using std::log;
using std::modf;
using std::pow;
using std::sin;
using std::sinh;
using std::sqrt;
using std::tan;
using std::tanh;

#    endif   // _RWSTD_NO_NAMESPACE
#  endif   // _RWSTD_MATH_H_INCLUDED
#else
#  include _RWSTD_ANSI_C_MATH_H

#undef exception
#undef abs
#undef acos
#undef asin
#undef atan
#undef atan2
#undef cos
#undef exp
#undef fabs
#undef log
#undef log10
#undef sin
#undef sqrt
#undef tan

#endif   // _RWSTD_NO_DEPRECATED_C_HEADERS
