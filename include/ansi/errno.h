/***************************************************************************
 *
 * errno.h
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

#  ifndef _RWSTD_ERRNO_H_INCLUDED
#    define _RWSTD_ERRNO_H_INCLUDED

#    include <cerrno>

#    ifndef _RWSTD_NO_NAMESPACE

#      ifndef errno

// be prepared to handle 17.4.1.2, p5 and lwg issue 310
// which say that errno may and must be a macro, respectively

using std::errno;

#      endif   // errno

#    endif   // _RWSTD_NO_NAMESPACE

#  endif   // _RWSTD_ERRNO_H_INCLUDED

#else

#  if    !defined (errno) \
      || !defined (EDOM) || !defined (ERANGE) || !defined (EAGAIN)
#    include _RWSTD_ANSI_C_ERRNO_H
#  endif   // errno ...

#endif   // _RWSTD_NO_DEPRECATED_C_HEADERS
