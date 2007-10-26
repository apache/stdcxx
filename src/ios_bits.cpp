/**************************************************************************
 *
 * ios_bits.cpp - definitions of ios_base static const data members
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


#include <rw/_iosbase.h>
#include <rw/_defs.h>


_RWSTD_NAMESPACE (std) { 

// required to be defined by 9.4.2, p4, even if initialized inside class
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::boolalpha);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::dec);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::fixed);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::hex);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::internal);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::left);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::oct);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::right);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::scientific);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::showbase);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::showpoint);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::showpos);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::skipws);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::unitbuf);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::uppercase);

#ifndef _RWSTD_NO_EXT_BIN_IO

_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::bin);

#endif   // _RWSTD_NO_EXT_BIN_IO

_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::adjustfield);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::basefield);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::floatfield);

#ifndef _RWSTD_NO_EXT_REENTRANT_IO

_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::nolock);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::fmtflags ios_base::nolockbuf);

#endif   // _RWSTD_NO_EXT_REENTRANT_IO

_RWSTD_DEFINE_STATIC_CONST (const ios_base::iostate ios_base::goodbit);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::iostate ios_base::badbit);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::iostate ios_base::eofbit);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::iostate ios_base::failbit);

_RWSTD_DEFINE_STATIC_CONST (const ios_base::openmode ios_base::app);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::openmode ios_base::binary);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::openmode ios_base::in);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::openmode ios_base::out);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::openmode ios_base::trunc);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::openmode ios_base::ate);

_RWSTD_DEFINE_STATIC_CONST (const ios_base::seekdir ios_base::beg);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::seekdir ios_base::cur);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::seekdir ios_base::end);

#ifndef _RWSTD_STRICT_ANSI

_RWSTD_DEFINE_STATIC_CONST (const ios_base::openmode ios_base::nocreate);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::openmode ios_base::noreplace);

#endif   // _RWSTD_STRICT_ANSI

#ifndef _RWSTD_NO_EXT_STDIO

_RWSTD_DEFINE_STATIC_CONST (const ios_base::openmode ios_base::stdio);
_RWSTD_DEFINE_STATIC_CONST (const ios_base::openmode ios_base::native);

#endif   // _RWSTD_NO_EXT_STDIO


}   // namespace std
