/***************************************************************************
 *
 * iosdata.h
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
 * Copyright 1994-2006 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifndef _RWSTD_IOSDATA_H_INCLUDED
#define _RWSTD_IOSDATA_H_INCLUDED


#include <rw/_iosbase.h>
#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) {

// POD-type
struct ios_base::_C_usr_data
{
    struct _C_event_cb {
        int                      _C_index;
        ios_base::event_callback _C_fn;
    };

    typedef void (ios_base::*_C_fire_fun)(ios_base::event, bool);

    // allocates a 0-initialized chunk of storage
    static _C_usr_data* _C_alloc (_C_fire_fun);
    static void _C_dealloc (_C_usr_data*);

    void         *_C_tie;       // tied stream object
    long         *_C_iarray;    // user long data
    void*        *_C_parray;    // user void* data
    _C_event_cb  *_C_cbarray;   // array of user-registered callbacks
    _RWSTD_SIZE_T _C_isize;     // size of _C_iarray in elements
    _RWSTD_SIZE_T _C_psize;     // size of _C_parray in elements
    _RWSTD_SIZE_T _C_cbsize;    // size of _C_cbarray in elements

    // set to point to ios_base::_C_fire_event, used to eliminate
    // the dependency of the translation unit that defines common
    // ios_base members on the one that defines rarely used ones
    _C_fire_fun _C_fire;

    static _C_usr_data _C_std_usr_data [2];
};

}   // namespace std

#endif   // _RWSTD_IOSDATA_H_INCLUDED
