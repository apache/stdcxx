// -*- C++ -*-
/***************************************************************************
 *
 * _stringio.h - declarations of the string extractors
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
 * Copyright 2003-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#if __GNUG__ >= 3
#  pragma GCC system_header
#endif   // gcc >= 3

#ifndef _RWSTD_RW_STRINGIO_H_INCLUDED
#define _RWSTD_RW_STRINGIO_H_INCLUDED


_RWSTD_NAMESPACE (std) {


_EXPORT
template<class _CharT, class _Traits, class _Allocator>
basic_istream<_CharT, _Traits>&
operator>> (basic_istream<_CharT, _Traits>&,
            basic_string<_CharT, _Traits, _Allocator>&);


_EXPORT
template<class _CharT, class _Traits, class _Allocator>
basic_istream<_CharT, _Traits>&
getline (basic_istream<_CharT, _Traits>&,
         basic_string<_CharT, _Traits, _Allocator>&,
         _CharT);


template<class _CharT, class _Traits, class _Allocator>
inline basic_istream<_CharT, _Traits>& 
getline (basic_istream<_CharT, _Traits>&            __is,
         basic_string<_CharT, _Traits, _Allocator>& __str);


template<class _CharT, class _Traits, class _Allocator>
inline basic_ostream<_CharT, _Traits>&
operator<< (basic_ostream<_CharT, _Traits>&,
            const basic_string<_CharT, _Traits, _Allocator>&);


}   // namespace std


#endif   // _RWSTD_RW_STRINGIO_H_INCLUDED


#if   !defined (_RWSTD_STRING_INSERTER_INCLUDED) \
    && defined (_RWSTD_INCLUDE_STRING_INSERTER)
#  define _RWSTD_STRING_INSERTER_INCLUDED

_RWSTD_NAMESPACE (std) {

template<class _CharT, class _Traits, class _Allocator>
inline basic_ostream<_CharT, _Traits>&
operator<< (basic_ostream<_CharT, _Traits>&                  __os,
            const basic_string<_CharT, _Traits, _Allocator>& __str)
{
    return _RW::__rw_insert (__os, __str.data (), __str.length (),
                             __os.width ());
}

}   // namespace std

#endif   // !_RWSTD_STRING_INSERTER_INCLUDED


#if   !defined (_RWSTD_STRING_EXTRACTORS_INCLUDED) \
    && defined (_RWSTD_INCLUDE_STRING_EXTRACTORS)
#  define _RWSTD_STRING_EXTRACTORS_INCLUDED


_RWSTD_NAMESPACE (std) {

template<class _CharT, class _Traits, class _Allocator>
inline basic_istream<_CharT, _Traits>& 
getline (basic_istream<_CharT, _Traits>&            __is,
         basic_string<_CharT, _Traits, _Allocator>& __str)
{
    return getline (__is, __str, __is.widen ('\n'));
}

}   // namespace std


#  if _RWSTD_DEFINE_TEMPLATE_FIRST (_BASIC_ISTREAM)
#    include <rw/_stringio.cc>
#  endif   // _RWSTD_DEFINE_TEMPLATE_FIRST (_BASIC_ISTREAM)


#ifdef _RWSTD_MSVC
#  pragma warning (push)
#  pragma warning (disable: 4231)
#endif   // _RWSTD_MSVC


_RWSTD_NAMESPACE (std) {

#if _RWSTD_INSTANTIATE (_BASIC_ISTREAM, _CHAR)

_RWSTD_INSTANTIATE_FUN_1 (_RWSTD_TI_EXPORT istream&
                          operator>> (istream&, string&));

_RWSTD_INSTANTIATE_FUN_1 (_RWSTD_TI_EXPORT istream&
                          getline (istream&, string&, char));

#endif   // _RWSTD_INSTANTIATE (_BASIC_ISTREAM, _CHAR)

#if _RWSTD_INSTANTIATE (_BASIC_ISTREAM, _WCHAR_T)

_RWSTD_INSTANTIATE_FUN_1 (_RWSTD_TI_EXPORT wistream&
                          operator>> (wistream&, wstring&));

_RWSTD_INSTANTIATE_FUN_1 (_RWSTD_TI_EXPORT wistream&
                          getline (wistream&, wstring&, wchar_t));
#endif   // _RWSTD_INSTANTIATE (_BASIC_ISTREAM, _WCHAR_T)

}   // namespace std


#ifdef _RWSTD_MSVC
#  pragma warning (pop)
#endif   // _RWSTD_MSVC


#  if _RWSTD_DEFINE_TEMPLATE_LAST (_BASIC_ISTREAM)
#    include <rw/_stringio.cc>
#  endif   // _RWSTD_DEFINE_TEMPLATE_LAST (_BASIC_ISTREAM)


#endif   // !_RWSTD_STRING_EXTRACTORS_INCLUDED
