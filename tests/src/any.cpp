/************************************************************************
 *
 * any.cpp - definitions of the rw_any_t class members
 *
 * $Id$
 *
 ************************************************************************
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

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <any.h>

#include <printf.h>   // for rw_sprintfa()
#include <stdlib.h>   // for free()
#include <string.h>   // for memset()


#ifndef _RWSTD_NO_BOOL

rw_any_t::rw_any_t (bool value)
    : str_ (0), tid_ (t_bool)
{
    // using memset instead of default-initialization in the ctor
    // initializer list here and in all other ctors to work around
    // a SunPro -xarch=v9 bug (PR #28328) that causes the following:
    // Warning (Anachronism): Assigning int to __BIT_BLAST_16_16.
    // Error: Cannot cast from int to __BIT_BLAST_16_16
    
    memset (&val_, 0, sizeof val_);
    val_.bool_ = value;
}

#endif   // _RWSTD_NO_BOOL

rw_any_t::rw_any_t (char value)
    : str_ (0), tid_ (t_char)
{
    memset (&val_, 0, sizeof val_);
    val_.char_ = value;
}


rw_any_t::rw_any_t (signed char value)
    : str_ (0), tid_ (t_schar)
{
    memset (&val_, 0, sizeof val_);
    val_.schar_ = value;
}


rw_any_t::rw_any_t (unsigned char value)
    : str_ (0), tid_ (t_uchar)
{
    memset (&val_, 0, sizeof val_);
    val_.uchar_ = value;
}


rw_any_t::rw_any_t (signed short value)
    : str_ (0), tid_ (t_sshrt)
{
    memset (&val_, 0, sizeof val_);
    val_.sshrt_ = value;
}


rw_any_t::rw_any_t (unsigned short value)
    : str_ (0), tid_ (t_ushrt)
{
    memset (&val_, 0, sizeof val_);
    val_.ushrt_ = value;
}


rw_any_t::rw_any_t (signed int value)
    : str_ (0), tid_ (t_sint)
{
    memset (&val_, 0, sizeof val_);
    val_.sint_ = value;
}


rw_any_t::rw_any_t (unsigned int value)
    : str_ (0), tid_ (t_uint)
{
    memset (&val_, 0, sizeof val_);
    val_.uint_ = value;
}


rw_any_t::rw_any_t (signed long value)
    : str_ (0), tid_ (t_slong)
{
    memset (&val_, 0, sizeof val_);
    val_.slong_ = value;
}


rw_any_t::rw_any_t (unsigned long value)
    : str_ (0), tid_ (t_ulong)
{
    memset (&val_, 0, sizeof val_);
    val_.ulong_ = value;
}


#ifdef _RWSTD_LONG_LONG

rw_any_t::rw_any_t (signed _RWSTD_LONG_LONG value)
    : str_ (0), tid_ (t_sllong)
{
    memset (&val_, 0, sizeof val_);
    val_.sllong_ = value;
}


rw_any_t::rw_any_t (unsigned _RWSTD_LONG_LONG value)
    : str_ (0), tid_ (t_ullong)
{
    memset (&val_, 0, sizeof val_);
    val_.ullong_ = value;
}

#endif   // _RWSTD_LONG_LONG


rw_any_t::rw_any_t (float value)
    : str_ (0), tid_ (t_flt)
{
    memset (&val_, 0, sizeof val_);
    val_.flt_ = value;
}


rw_any_t::rw_any_t (double value)
    : str_ (0), tid_ (t_dbl)
{
    memset (&val_, 0, sizeof val_);
    val_.dbl_ = value;
}


#ifndef _RWSTD_NO_LONG_DOUBLE

rw_any_t::rw_any_t (long double value)
    : str_ (0), tid_ (t_ldbl)
{
    memset (&val_, 0, sizeof val_);
    val_.ldbl_ = value;
}

#endif   // _RWSTD_NO_LONG_DOUBLE


rw_any_t::rw_any_t (const void* value)
    : str_ (0), tid_ (t_pvoid)
{
    memset (&val_, 0, sizeof val_);
    val_.pvoid_ = value;
}


rw_any_t::rw_any_t (const char* value)
    : str_ (0), tid_ (t_str)
{
    memset (&val_, 0, sizeof val_);
    val_.pvoid_ = value;
}


#ifndef _RWSTD_NO_NATIVE_WCHAR_T

rw_any_t::rw_any_t (wchar_t value)
    : str_ (0), tid_ (t_wchar)
{
    memset (&val_, 0, sizeof val_);
    val_.wchar_ = value;
}

#endif   // _RWSTD_NO_NATIVE_WCHAR_T


#ifndef _RWSTD_NO_WCHAR_T

rw_any_t::rw_any_t (const wchar_t* value)
    : str_ (0), tid_ (t_wstr)
{
    memset (&val_, 0, sizeof val_);
    val_.pvoid_ = value;
}

#endif   // _RWSTD_NO_WCHAR_T


rw_any_t::rw_any_t (const rw_any_t &rhs)
    : val_ (rhs.val_), str_ (0), tid_ (rhs.tid_)
{
}


rw_any_t& rw_any_t::operator= (const rw_any_t &rhs)
{
    // free string allocated by tostr() (via a call to sprintfa())
    free (str_);

    val_ = rhs.val_;
    str_ = 0;
    tid_ = rhs.tid_;

    return *this;
}


rw_any_t::~rw_any_t ()
{
    // free string allocated by tostr() (via a call to sprintfa())
    free (str_);

    memset (&val_, 0, sizeof val_);

    str_ = 0;
    tid_ = type_id_t ();
}


const char*
rw_any_t::type_name () const
{
    static const char* const names[] = {
        "<no type>",
        "bool", "signed char", "unsigned char", "char",
        "short", "unsigned short", "int", "unsigned int",
        "long", "unsigned long",
        "long long", "unsigned long long",
        "float", "double", "long double",
        "wchar_t", "void*",
        "const char*", "const wchar_t*"
    };

    // the liftime of the returned string must extend
    // to the end of the program
    return names [tid_];
}


const char*
rw_any_t::tostr (const char *fmt /* = 0 */)
{
    // free the previously allocated string
    free (str_);
    str_ = 0;

    switch (tid_) {
    case t_none:
        return 0;

    case t_bool:
        return val_.bool_ ? "true" : "false";

    case t_char:
        if (0 == fmt)
            fmt = "%c";
        str_ = rw_sprintfa (fmt, val_.char_);
        break;

    case t_schar:
        if (0 == fmt)
            fmt = "%hhi";
        str_ = rw_sprintfa (fmt, val_.schar_);
        break;

    case t_uchar:
        if (0 == fmt)
            fmt = "%hhu";
        str_ = rw_sprintfa (fmt, val_.uchar_);
        break;

    case t_sshrt:
        if (0 == fmt)
            fmt = "%hi";
        str_ = rw_sprintfa (fmt, val_.sshrt_);
        break;

    case t_ushrt:
        if (0 == fmt)
            fmt = "%hu";
        str_ = rw_sprintfa (fmt, val_.ushrt_);
        break;

    case t_sint:
        if (0 == fmt)
            fmt = "%i";
        str_ = rw_sprintfa (fmt, val_.sint_);
        break;

    case t_uint:
        if (0 == fmt)
            fmt = "%u";
        str_ = rw_sprintfa (fmt, val_.uint_);
        break;

    case t_slong:
        if (0 == fmt)
            fmt = "%li";
        str_ = rw_sprintfa (fmt, val_.slong_);
        break;

    case t_ulong:
        if (0 == fmt)
            fmt = "%lu";
        str_ = rw_sprintfa (fmt, val_.ulong_);
        break;

#ifdef _RWSTD_LONG_LONG

    case t_sllong:
        if (0 == fmt)
            fmt = "%lli";
        str_ = rw_sprintfa (fmt, val_.sllong_);
        break;

    case t_ullong:
        if (0 == fmt)
            fmt = "%llu";
        str_ = rw_sprintfa (fmt, val_.ullong_);
        break;

#endif   // _RWSTD_LONG_LONG

    case t_flt:
        if (0 == fmt)
            fmt = "%g";
        str_ = rw_sprintfa (fmt, val_.flt_);
        break;

    case t_dbl:
        if (0 == fmt)
            fmt = "%g";
        str_ = rw_sprintfa (fmt, val_.dbl_);
        break;

#ifndef _RWSTD_NO_LONG_DOUBLE

    case t_ldbl:
        if (0 == fmt)
            fmt = "%Lg";
        str_ = rw_sprintfa (fmt, val_.ldbl_);
        break;

#endif   // _RWSTD_NO_LONG_DOUBLE

    case t_pvoid:
        if (0 == fmt)
            fmt = "%p";
        str_ = rw_sprintfa (fmt, val_.pvoid_);
        break;

#ifndef _RWSTD_NO_NATIVE_WCHAR_T

    case t_wchar:
        if (0 == fmt)
            fmt = "%lc";
        str_ = rw_sprintfa (fmt, val_.wchar_);
        break;

#endif   // _RWSTD_NO_NATIVE_WCHAR_T

    case t_str:
        if (0 == fmt)
            fmt = "%s";
        str_ = rw_sprintfa (fmt, val_.pvoid_);
        break;

#ifndef _RWSTD_NO_WCHAR_T

    case t_wstr:
        if (0 == fmt)
            fmt = "%ls";
        str_ = rw_sprintfa (fmt, val_.pvoid_);
        break;

#endif   // _RWSTD_NO_WCHAR_T

    }

    return str_;
}
