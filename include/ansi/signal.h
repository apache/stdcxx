/***************************************************************************
 *
 * signal.h
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
#  ifndef _RWSTD_SIGNAL_H_INCLUDED
#    define _RWSTD_SIGNAL_H_INCLUDED

#    include <csignal>

#    ifndef _RWSTD_NO_NAMESPACE

using std::sig_atomic_t;

using std::raise;
using std::signal;

#      ifdef __linux__

         // hackery needed by <pthread.h>

#        ifndef _SIGSET_H_types
#          define _SIGSET_H_types   1

typedef int __sig_atomic_t;

#          define _SIGSET_NWORDS (1024 / (8 * sizeof (unsigned long int)))

typedef struct {
    unsigned long int __val[_SIGSET_NWORDS];
} __sigset_t;

#        endif   // _SIGSET_H_types
#      endif   // __linux__


#    endif   // _RWSTD_NO_NAMESPACE
#  endif   // _RWSTD_SIGNAL_H_INCLUDED
#else
#  include _RWSTD_ANSI_C_SIGNAL_H
#endif   // _RWSTD_NO_DEPRECATED_C_HEADERS
