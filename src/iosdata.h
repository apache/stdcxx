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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _RWSTD_IOSDATA_H_INCLUDED
#define _RWSTD_IOSDATA_H_INCLUDED


#include <string.h>

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


inline /* static */ ios_base::_C_usr_data*
ios_base::_C_usr_data::_C_alloc (_C_fire_fun pfire)
{
    _TRY {
        // rely on 0-initialization of PODs
        _C_usr_data* const pdata = new _C_usr_data ();

#ifdef _RWSTD_NO_NEW_THROWS

        if (!pdata)
            return 0;

#endif   // _RWSTD_NO_NEW_THROWS

        _RWSTD_ASSERT (0 != pdata);

#ifndef _RWSTD_NO_POD_ZERO_INIT

        // assert that the POD ctor above zeroed out all members
        _RWSTD_ASSERT (!pdata->_C_tie);
        _RWSTD_ASSERT (!pdata->_C_iarray);
        _RWSTD_ASSERT (!pdata->_C_parray);
        _RWSTD_ASSERT (!pdata->_C_cbarray);
        _RWSTD_ASSERT (!pdata->_C_isize);
        _RWSTD_ASSERT (!pdata->_C_psize);
        _RWSTD_ASSERT (!pdata->_C_cbsize);

#else   // if defined (_RWSTD_NO_POD_ZERO_INIT)

        memset (pdata, 0, sizeof *pdata);

#endif   // _RWSTD_NO_POD_ZERO_INIT

        pdata->_C_fire = pfire;

        return pdata;
    }
    _CATCH (...) {
        return 0;
    }
}


inline /* static */ void
ios_base::_C_usr_data::_C_dealloc (_C_usr_data *ptr)
{
    if (ptr) {
        operator delete (ptr->_C_iarray);
        operator delete (ptr->_C_parray);
        operator delete (ptr->_C_cbarray);

        if (   ptr != _C_usr_data::_C_std_usr_data
            && ptr != _C_usr_data::_C_std_usr_data + 1)
            delete ptr;
    }
}


}   // namespace std


#endif   // _RWSTD_IOSDATA_H_INCLUDED
