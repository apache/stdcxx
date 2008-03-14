/***************************************************************************
 *
 * export.cpp - exported templates
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
 * Copyright 2004-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <rw/_defs.h>

#ifndef _RWSTD_NO_EXPORT

// include every library header that declares any exported templates
// and the corresponding implementation file where the template is
// defined

#include <algorithm>
#include <algorithm.cc>

#include <bitset>
#include <bitset.cc>

#include <complex>
#include <complex.cc>

#include <deque>
#include <deque.cc>

#include <fstream>
#include <fstream.cc>

#include <ios>

#include <istream>
#include <istream.cc>

#include <limits>
#include <limits.cc>

#include <list>
#include <list.cc>

#include <ostream>
#include <ostream.cc>

#include <sstream>
#include <sstream.cc>

#include <string>
#include <string.cc>

#include <valarray>
#include <valarray.cc>

#include <vector>
#include <vector.cc>

#include <streambuf>
#include <streambuf.cc>

#include <locale>

#include <loc/_codecvt.h>
#include <loc/_collate.h>
#include <loc/_ctype.h>
#include <loc/_messages.h>
#include <loc/_money_get.h>
#include <loc/_money_put.h>
#include <loc/_moneypunct.h>
#include <loc/_num_get.h>
#include <loc/_num_put.h>
#include <loc/_numpunct.h>
#include <loc/_time_get.h>
#include <loc/_time_put.h>
#include <loc/_punct.h>

#include <loc/_codecvt.cc>
#include <loc/_collate.cc>
#include <loc/_ctype.cc>
#include <loc/_messages.cc>
#include <loc/_money_get.cc>
#include <loc/_money_put.cc>
#include <loc/_moneypunct.cc>
#include <loc/_num_get.cc>
#include <loc/_num_put.cc>
#include <loc/_numpunct.cc>
#include <loc/_time_get.cc>
#include <loc/_time_put.cc>
#include <loc/_punct.cc>

#include <rw/_algobase.h>
#include <rw/_algobase.cc>

#include <rw/_basic_ios.h>
#include <rw/_basic_ios.cc>

#include <rw/_stringio.h>
#include <rw/_stringio.cc>

#include <rw/_heap.h>
#include <rw/_heap.cc>

#include <rw/_ioinsert.h>
#include <rw/_ioinsert.cc>

#include <rw/_tree.h>
#include <rw/_tree.cc>

#include "podarray.h"
#include "podarray.cc"

#endif   // _RWSTD_NO_EXPORT

// export __rw_atomic_xxx() functions, defined in atomic.asm
#if defined (_WIN32) && defined (_DLL)

#  if defined (_M_IX86)

#    pragma comment(linker, "/EXPORT:___rw_atomic_add8")
#    pragma comment(linker, "/EXPORT:___rw_atomic_add16")
#    pragma comment(linker, "/EXPORT:___rw_atomic_add32")
#    pragma comment(linker, "/EXPORT:___rw_atomic_xchg8")
#    pragma comment(linker, "/EXPORT:___rw_atomic_xchg16")
#    pragma comment(linker, "/EXPORT:___rw_atomic_xchg32")

#  elif defined (_M_X64)

#    pragma comment(linker, "/EXPORT:__rw_atomic_add8")
#    pragma comment(linker, "/EXPORT:__rw_atomic_add16")
#    pragma comment(linker, "/EXPORT:__rw_atomic_add32")
#    pragma comment(linker, "/EXPORT:__rw_atomic_xchg8")
#    pragma comment(linker, "/EXPORT:__rw_atomic_xchg16")
#    pragma comment(linker, "/EXPORT:__rw_atomic_xchg32")

#    ifdef _RWSTD_INT64_T
#      pragma comment(linker, "/EXPORT:__rw_atomic_add64")
#      pragma comment(linker, "/EXPORT:__rw_atomic_xchg64")
#    endif   // _RWSTD_INT64_T
#  endif   // _M_IX86

#endif   // _WIN32 && _DLL
