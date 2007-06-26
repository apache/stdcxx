/***************************************************************************
 *
 * typeinfo.cpp - definitions of the Standard Library type_info
 *                and bad_cast class members
 *
 * $Id$
 *
 ***************************************************************************
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#define _RWSTD_LIB_SRC

#include <rw/_defs.h>

#include <typeinfo>
#include <string.h>


_RWSTD_NAMESPACE (__rw) {

inline int
__rw_ticmp (const _STD::type_info &lhs, const _STD::type_info &rhs)
{
    const char* const n1 = lhs.name ();
    const char* const n2 = rhs.name ();

    return strcmp (n1 ? n1 : "", n2 ? n2 : "");
}

}   // namespace __rw

/**************************************************************************/
// class type_info members

#if defined _RWSTD_TYPEINFO_INCLUDED

// define type_info members only if our own <typeinfo> header
// (as opposed to the compiler's native) has been included
// SunPro always #includes its own, regardless of options

#  if    !defined (_RWSTD_NO_STD_TYPE_INFO) \
      || !defined (_RWSTD_NO_GLOBAL_TYPE_INFO)

// at least one of the config tests above succeeded,
// rely on config tests for each individual member

#    ifndef _RWSTD_NO_STD_TYPE_INFO

// define type_info et al in namespace std iff they are found to be
// defined in namespace std in the compiler's language support library
// or if they are not found to be defined in either namespace std or
// in the global namespace

_RWSTD_NAMESPACE (std) {

#    endif   // _RWSTD_NO_STD_TYPE_INFO


#    ifdef _RWSTD_NO_TYPE_INFO_DTOR

type_info::~type_info () { }

#    endif   // _RWSTD_NO_TYPE_INFO_DTOR


#    ifdef _RWSTD_NO_TYPE_INFO_BEFORE

bool type_info::before (const type_info &rhs) const
{
    return 0 > _RW::__rw_ticmp (*this, rhs);
}

#    endif   // _RWSTD_NO_TYPE_INFO_BEFORE


#    ifdef _RWSTD_NO_TYPE_INFO_NAME

// used in type_info::name() when the class has no member called _C_name
static const char _C_name[] = "";

const char* type_info::name () const
{
    // silence warning: _C_name declared but never used
#ifdef _RWSTD_NO_STD_TYPE_INFO
    _RWSTD_UNUSED (::_C_name);
#else
    _RWSTD_UNUSED (std::_C_name);
#endif   // _RWSTD_NO_STD_TYPE_INFO

    return _C_name;
}

#    endif   // _RWSTD_NO_TYPE_INFO_NAME


#    ifdef _RWSTD_NO_TYPE_INFO_EQUALITY

bool type_info::operator== (const type_info &rhs) const
{
    return !before (rhs) && !rhs.before (*this);
}

#    endif   // _RWSTD_NO_TYPE_INFO_EQUALITY


#    ifdef _RWSTD_NO_TYPE_INFO_INEQUALITY

bool type_info::operator!= (const type_info &rhs) const
{
    return !(*this == rhs);
}

#    endif   // _RWSTD_NO_TYPE_INFO_INEQUALITY

#    ifndef _RWSTD_NO_STD_TYPE_INFO

}   // namespace std

#    endif   // _RWSTD_NO_STD_TYPE_INFO

#  else   // if _RWSTD_NO_STD_TYPEINFO && _RWSTD_NO_GLOBAL_TYPEINFO

// both config tests for class type_info failed so config test
// results for each individual member are likely to be unreliable
// rely on platform knowledge instead

#    ifndef _RWSTD_NO_RUNTIME_IN_STD

_RWSTD_NAMESPACE (std) {

#    endif   // _RWSTD_NO_RUNTIME_IN_STD

#    if defined (__HP_aCC)

const char* type_info::name () const
{
    return _C_name;
}

#      ifdef _HP_IA64ABI

bool type_info::before (const type_info &rhs) const
{
    return name () < rhs.name ();
}

bool type_info::operator== (const type_info &rhs) const
{
    return name () == rhs.name ();
}

bool type_info::operator!= (const type_info &rhs) const
{
    return !(*this == rhs);
}

#      endif   // _HP_IA64ABI

#    elif defined (__INTEL_COMPILER) && !defined (_MSC_VER)

const char* type_info::name () const
{
    return _C_name;
}

bool type_info::before (const type_info &rhs) const
{
    return name () < rhs.name ();
}

bool type_info::operator== (const type_info &rhs) const
{
    return name () == rhs.name ();
}

bool type_info::operator!= (const type_info &rhs) const
{
    return !(*this == rhs);
}

#    elif defined (__GNUG__) && !defined (_RWSTD_NO_HONOR_STD)

// define member functions inline in namespace std (they are defined
// as members of ::type_info (as opposed to std::type_info) in libstdc++
// unless lib is built with -fhonor-std

const char* type_info::name () const
{
    return _C_name;
}

bool type_info::operator== (const type_info &rhs) const
{
    return !this->before (rhs) && !rhs.before (*this);
}

bool type_info::operator!= (const type_info &rhs) const
{
    return !(*this == rhs);
}

#    elif defined (__IBMCPP__)

const char* type_info::name () const
{
    return _C_typeName;
}

bool type_info::operator== (const type_info &rhs) const
{
    return !this->before (rhs) && !rhs.before (*this);
}

bool type_info::operator!= (const type_info &rhs) const
{
    return !(*this == rhs);
}

#    endif   // __IBMCPP__


#    if defined (__GNUG__) && !defined (_RWSTD_NO_HONOR_STD)

// define member function in namespace std (it is defined as a member
// of ::type_info (as opposed to std::type_info) in libstdc++
// unless lib is built with -fhonor-std
bool type_info::before (const type_info &rhs) const
{
    return 0 > _RW::__rw_ticmp (*this, rhs);
}

#    elif defined (__IBMCPP__)

bool type_info::before (const type_info &rhs) const
{
    return 0 > _RW::__rw_ticmp (*this, rhs);
}

#    endif   //  __GNUG__ && !_RWSTD_NO_HONOR_STD


#    ifdef _MSC_VER

const char* type_info::name () const
{
    return _C_name;
}


// define in global namespace just like MSVC wants it
bool type_info::before (const type_info &rhs) const
{
    return 0 > _RW::__rw_ticmp (*this, rhs);
}


bool type_info::operator== (const type_info &rhs) const
{
    return 0 == _RW::__rw_ticmp (*this, rhs);
}


bool type_info::operator!= (const type_info &rhs) const
{
    return !(*this == rhs);
}

#    endif   // _MSC_VER

#    ifndef _RWSTD_NO_RUNTIME_IN_STD

}   // namespace std

#    endif   // _RWSTD_NO_RUNTIME_IN_STD

#  endif   // !_RWSTD_NO_STD_TYPEINFO || !_RWSTD_NO_GLOBAL_TYPEINFO


//**************************************************************************
// class bad_cast members

#  ifndef _RWSTD_NO_STD_BAD_CAST

_RWSTD_NAMESPACE (std) {

#  endif   // _RWSTD_NO_STD_BAD_CAST

#  ifdef _RWSTD_NO_BAD_CAST_DEFAULT_CTOR

bad_cast::bad_cast () _THROWS (())
{
    // empty
}

#  endif   // _RWSTD_NO_BAD_CAST_DEFAULT_CTOR

#  ifdef _RWSTD_NO_BAD_CAST_COPY_CTOR

bad_cast::bad_cast (const bad_cast &rhs) _THROWS (())
    : exception (rhs)
{
    // empty
}

#  endif   // _RWSTD_NO_BAD_CAST_COPY_CTOR

#  ifdef _RWSTD_NO_BAD_CAST_DTOR

bad_cast::~bad_cast () _THROWS (())
{
    // empty
}

#  endif   // _RWSTD_NO_BAD_CAST_DTOR

#  ifdef _RWSTD_NO_BAD_CAST_ASSIGNMENT

bad_cast&
bad_cast::operator= (const bad_cast &rhs) _THROWS (())
{
    exception::operator= (rhs);

    return *this;
}

#  endif   // _RWSTD_NO_BAD_CAST_ASSIGNMENT

#  ifdef _RWSTD_NO_BAD_CAST_WHAT

const char* bad_cast::what () const _THROWS (())
{
    return "bad_cast";
}

#  endif   // _RWSTD_NO_BAD_CAST_WHAT

#  ifndef _RWSTD_NO_STD_BAD_CAST

}   // namespace std

#  endif   // _RWSTD_NO_STD_BAD_CAST


//**************************************************************************
// class bad_typeid members

#  ifndef _RWSTD_NO_STD_BAD_TYPEID

_RWSTD_NAMESPACE (std) {

#  endif   // _RWSTD_NO_STD_BAD_TYPEID

#  ifdef _RWSTD_NO_BAD_TYPEID_DEFAULT_CTOR

bad_typeid::bad_typeid () _THROWS (())
{
    // empty
}

#  endif   // _RWSTD_NO_BAD_TYPEID_DEFAULT_CTOR

#  ifdef _RWSTD_NO_BAD_TYPEID_COPY_CTOR

bad_typeid::bad_typeid (const bad_typeid &rhs) _THROWS (())
    : exception (rhs)
{
    // empty
}

#  endif   // _RWSTD_NO_BAD_TYPEID_COPY_CTOR

#  ifdef _RWSTD_NO_BAD_TYPEID_DTOR

bad_typeid::~bad_typeid () _THROWS (())
{
    // empty
}

#  endif   // _RWSTD_NO_BAD_TYPEID_DTOR

#  ifdef _RWSTD_NO_BAD_TYPEID_ASSIGNMENT

bad_typeid&
bad_typeid::operator= (const bad_typeid &rhs) _THROWS (())
{
    exception::operator= (rhs);

    return *this;
}

#  endif   // _RWSTD_NO_BAD_TYPEID_ASSIGNMENT

#  ifdef _RWSTD_NO_BAD_TYPEID_WHAT

const char* bad_typeid::what () const _THROWS (())
{
    return "bad_typeid";
}

#  endif   // _RWSTD_NO_BAD_TYPEID_WHAT

#  ifndef _RWSTD_NO_STD_BAD_TYPEID

}   // namespace std

#  endif   // _RWSTD_NO_STD_BAD_TYPEID

#endif   // _RWSTD_TYPEINFO_INCLUDED
