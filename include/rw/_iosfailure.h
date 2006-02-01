/***************************************************************************
 *
 * _iosfailure.h - definition of the ios_base::failure class
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#ifndef _RWSTD_RW_IOSFAILURE_H_INCLUDED
#define _RWSTD_RW_IOSFAILURE_H_INCLUDED

#ifndef _RWSTD_RW_EXCEPTION_H_INCLUDED
#  include <rw/_exception.h>
#endif   // _RWSTD_RW_EXCEPTION_H_INCLUDED

#ifndef _RWSTD_RW_IOSFWD_H_INCLUDED
#  include <rw/_iosfwd.h>
#endif   // _RWSTD_RW_IOSFWD_H_INCLUDED


_RWSTD_NAMESPACE (__rw) {

struct _RWSTD_EXPORT __rw_failure: __rw_exception
{
    _EXPLICIT __rw_failure (const _STD::string &__what)
        : __rw_exception (__what) { }

    // extension
    _EXPLICIT __rw_failure (const char *__what = 0)
        : __rw_exception (__what) { }

    virtual ~__rw_failure () _THROWS (());
};


struct _RWSTD_EXPORT __rw_badbit_set: __rw_failure { };

struct _RWSTD_EXPORT __rw_eofbit_set: __rw_failure { };

struct _RWSTD_EXPORT __rw_failbit_set: __rw_failure { };

}   // namespace __rw


#endif   // _RWSTD_RW_IOSFAILURE_H_INCLUDED
