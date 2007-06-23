/***************************************************************************
 *
 * access.h
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
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
 
#ifndef _RWSTD_ACCESS_H_INCLUDED
#define _RWSTD_ACCESS_H_INCLUDED

#include <loc/_facet.h>
#include <loc/_locale.h>
#include <rw/_defs.h>


_RWSTD_NAMESPACE (__rw) {

    
struct __rw_access {
    
    static _RWSTD_SIZE_T*&
    _C_get_pid (__rw_facet &__f) {
        return __f._C_pid;
    }    
    
    static _RWSTD_SIZE_T&
    _C_get_ref (__rw_facet &__f) {
        return __f._C_ref;
    }    

    static _RWSTD_SIZE_T&
    _C_get_id (const __rw_facet_id& __id) {
        return __id._C_id;
    }
    
    static __rw_facet::_C_facet_type&
    _C_get_type (__rw_facet &__f) {
        return __f._C_type;
    }
    
    static const __rw_facet::_C_facet_type&
    _C_get_type (const __rw_facet &__f) {
        return __f._C_type;
    }
       
    static const char*
    _C_get_name (const __rw_facet &__f) {
        return __f._C_name;
    } 
    
    static const void*&
    _C_get_impdata (__rw_facet &__f) {
        return __f._C_impdata;
    }
    
    static _RWSTD_SIZE_T&
    _C_get_impsize (__rw_facet &__f) {
        return __f._C_impsize;
    }
};

}  // namepace __rw

#endif   // _RWSTD_ACCESS_H_INCLUDED
