/************************************************************************
 *
 * value.cpp - defines a User Defined type
 *
 * $Id$
 *
 ************************************************************************
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
 * Copyright 1994-2005 Rogue Wave Software.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <ctype.h>      // for isdigit()
#include <stdarg.h>     // for va_arg, va_list, ...
#include <stdlib.h>     // for strtol()
#include <string.h>     // for size_t, strlen()


#include <rw_value.h>
#include <rw_printf.h>


/* static */ size_t UserClass::count_;
/* static */ int    UserClass::id_gen_;   // generates unique non-zero ids
/* static */ int  (*UserClass::gen_)();   // extern "C++" int (*)()

/* static */ size_t UserClass::n_total_def_ctor_;
/* static */ size_t UserClass::n_total_copy_ctor_;
/* static */ size_t UserClass::n_total_dtor_;
/* static */ size_t UserClass::n_total_op_assign_;
/* static */ size_t UserClass::n_total_op_plus_assign_;
/* static */ size_t UserClass::n_total_op_minus_assign_;
/* static */ size_t UserClass::n_total_op_times_assign_;
/* static */ size_t UserClass::n_total_op_div_assign_;
/* static */ size_t UserClass::n_total_op_eq_;
/* static */ size_t UserClass::n_total_op_lt_;

// default values of pointers
/* static */ size_t* UserClass::def_ctor_throw_ptr_ =
    &UserClass::def_ctor_throw_count_;
/* static */ size_t* UserClass::copy_ctor_throw_ptr_ =
    &UserClass::copy_ctor_throw_count_;
/* static */ size_t* UserClass::dtor_throw_ptr_ =
    &UserClass::dtor_throw_count_;
/* static */ size_t* UserClass::op_assign_throw_ptr_ =
    &UserClass::op_assign_throw_count_;
/* static */ size_t* UserClass::op_plus_assign_throw_ptr_ =
    &UserClass::op_plus_assign_throw_count_;
/* static */ size_t* UserClass::op_minus_assign_throw_ptr_ =
    &UserClass::op_minus_assign_throw_count_;
/* static */ size_t* UserClass::op_times_assign_throw_ptr_ =
    &UserClass::op_times_assign_throw_count_;
/* static */ size_t* UserClass::op_div_assign_throw_ptr_ =
    &UserClass::op_div_assign_throw_count_;
/* static */ size_t* UserClass::op_eq_throw_ptr_ =
    &UserClass::op_eq_throw_count_;
/* static */ size_t* UserClass::op_lt_throw_ptr_ =
    &UserClass::op_lt_throw_count_;

// exception throwing initially disabled
/* static */ size_t UserClass::def_ctor_throw_count_        = size_t (-1);
/* static */ size_t UserClass::copy_ctor_throw_count_       = size_t (-1);
/* static */ size_t UserClass::dtor_throw_count_            = size_t (-1);
/* static */ size_t UserClass::op_assign_throw_count_       = size_t (-1);
/* static */ size_t UserClass::op_plus_assign_throw_count_  = size_t (-1);
/* static */ size_t UserClass::op_minus_assign_throw_count_ = size_t (-1);
/* static */ size_t UserClass::op_times_assign_throw_count_ = size_t (-1);
/* static */ size_t UserClass::op_div_assign_throw_count_   = size_t (-1);
/* static */ size_t UserClass::op_eq_throw_count_           = size_t (-1);
/* static */ size_t UserClass::op_lt_throw_count_           = size_t (-1);


UserClass::UserClass ()
    : id_ (++id_gen_), origin_ (id_), src_id_ (id_), val_ (0),
      n_copy_ctor_ (0), n_op_assign_ (0), n_op_eq_ (0), n_op_lt_ (0)
{
    // increment the total number of invocations of the default ctor
    // (do so even if the function throws an exception below)
    ++n_total_def_ctor_;

#ifndef _RWSTD_NO_EXCEPTIONS

    if (def_ctor_throw_ptr_ && n_total_def_ctor_ == *def_ctor_throw_ptr_) {
        DefCtor ex;
        ex.id_ = id_;
        throw ex;
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    // initialize the object's value
    if (gen_)
        val_ = gen_ ();

    // increment the number of successfully constructed objects
    ++count_;
}


UserClass::UserClass (const UserClass &rhs)
    : id_ (++id_gen_), origin_ (rhs.origin_), src_id_ (rhs.id_),
      val_ (rhs.val_),
      n_copy_ctor_ (0), n_op_assign_ (0), n_op_eq_ (0), n_op_lt_ (0)
{
    // verify id validity
    RW_ASSERT (rhs.id_ && rhs.id_ < id_gen_);

    // increment the number of times `rhs' has been copied
    // (do so even if the function throws an exception below)
    ++_RWSTD_CONST_CAST (UserClass*, &rhs)->n_copy_ctor_;

    // increment the total number of invocations of the copy ctor
    // (do so even if the function throws an exception below)
    ++n_total_copy_ctor_;

#ifndef _RWSTD_NO_EXCEPTIONS

    // throw an exception if the number of calls
    // to the copy ctor reaches the given value
    if (copy_ctor_throw_ptr_ && n_total_copy_ctor_ == *copy_ctor_throw_ptr_) {
        CopyCtor ex;
        ex.id_ = id_;
        throw ex;
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    // increment the number of successfully constructed objects
    ++count_;
}


UserClass::~UserClass ()
{
    // verify id validity
    RW_ASSERT (id_ && id_ <= id_gen_);

    // increment the total number of invocations of the dtor
    // (do so even if the function throws an exception below)
    ++n_total_dtor_;

#ifndef _RWSTD_NO_EXCEPTIONS

    // throw an exception if the number of calls
    // to the class dtor reaches the given value
    if (dtor_throw_ptr_ && n_total_dtor_ == *dtor_throw_ptr_) {
        Dtor ex;
        ex.id_ = id_;
        throw ex;
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    // decrement the number of objects in existence
    --count_;

    // invalidate id
    _RWSTD_CONST_CAST (int&, this->id_) = 0;
}


void UserClass::
assign (assign_op which, const UserClass &rhs)
{
    // verify id validity and uniqueness:
    // a valid id is non-zero (dtor resets)
    RW_ASSERT (id_ && id_ <= id_gen_);
    RW_ASSERT (rhs.id_ && rhs.id_ <= id_gen_);

    // no two id's have the same value
    RW_ASSERT (this == &rhs || id_ != rhs.id_);

    size_t *p_total_op = 0;
    size_t *p_op       = 0;
    size_t *p_throw    = 0;

    Exception *pex = 0; 

    OpAssign      ex_assign;
    OpPlusAssign  ex_plus_assign;
    OpMinusAssign ex_minus_assign;
    OpTimesAssign ex_times_assign;
    OpDivAssign   ex_div_assign;

    int new_val;

    switch (which) {
    case op_assign:
        p_total_op = &n_total_op_assign_;
        p_op       = &n_op_assign_;
        p_throw    = op_assign_throw_ptr_;
        pex        = &ex_assign;
        new_val    = rhs.val_;
        break;

    case op_plus_assign:
        p_total_op = &n_total_op_plus_assign_;
        p_op       = &n_op_plus_assign_;
        p_throw    = op_plus_assign_throw_ptr_;
        pex        = &ex_plus_assign;
        new_val    = val_ + rhs.val_;
        break;

    case op_minus_assign:
        p_total_op = &n_total_op_minus_assign_;
        p_op       = &n_op_minus_assign_;
        p_throw    = op_minus_assign_throw_ptr_;
        pex        = &ex_minus_assign;
        new_val    = val_ - rhs.val_;
        break;

    case op_times_assign:
        p_total_op = &n_total_op_times_assign_;
        p_op       = &n_op_times_assign_;
        p_throw    = op_times_assign_throw_ptr_;
        pex        = &ex_times_assign;
        new_val    = val_ * rhs.val_;
        break;

    case op_div_assign:
        p_total_op = &n_total_op_div_assign_;
        p_op       = &n_op_div_assign_;
        p_throw    = op_div_assign_throw_ptr_;
        pex        = &ex_div_assign;
        new_val    = val_ / rhs.val_;
        break;
    }

    // increment the number of invocations of the operator
    // (do so even if the function throws an exception below)

    ++*p_total_op;
    ++*p_op;

#ifndef _RWSTD_NO_EXCEPTIONS

    // throw an exception if the number of calls to
    // the assignment operator reaches the given value

    if (p_throw && *p_throw == *p_total_op) {
        pex->id_ = id_;
        throw *pex;
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    // overwrite value and source id only when the operation
    // is successful (i.e., only when it doesn't throw)

    origin_ = rhs.origin_;
    src_id_ = rhs.id_;
    val_    = new_val;
}


UserClass& UserClass::
operator= (const UserClass &rhs)
{
    assign (op_assign, rhs);

    return *this;
}


UserClass& UserClass::
operator+= (const UserClass &rhs)
{
    assign (op_plus_assign, rhs);

    return *this;
}


UserClass& UserClass::
operator-= (const UserClass &rhs)
{
    assign (op_minus_assign, rhs);

    return *this;
}


UserClass& UserClass::
operator*= (const UserClass &rhs)
{
    assign (op_times_assign, rhs);

    return *this;
}


UserClass& UserClass::
operator/= (const UserClass &rhs)
{
    assign (op_div_assign, rhs);

    return *this;
}


bool
UserClass::operator== (const UserClass &rhs) const
{
    // verify id validity and uniqueness
    RW_ASSERT (id_ && id_ <= id_gen_);
    RW_ASSERT (rhs.id_ && rhs.id_ <= id_gen_);
    RW_ASSERT (this == &rhs || id_ != rhs.id_);

    // increment the number of times each distinct object
    // has been used as the argument to operator==
    // (do so even if the function throws an exception below)
    ++_RWSTD_CONST_CAST (UserClass*, this)->n_op_eq_;

    if (this != &rhs)
        ++_RWSTD_CONST_CAST (UserClass*, &rhs)->n_op_eq_;

    // increment the total number of invocations of the operator
    // (do so even if the function throws an exception below)
    ++n_total_op_eq_;

#ifndef _RWSTD_NO_EXCEPTIONS

    // throw an exception if the number of calls
    // to operator== reaches the given value

    if (op_eq_throw_ptr_ && n_total_op_eq_ == *op_eq_throw_ptr_) {
        OpEq ex;
        ex.id_ = id_;
        throw ex;
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    return val_ == rhs.val_;
}


bool
UserClass::operator< (const UserClass &rhs) const
{
    // verify id validity and uniqueness
    RW_ASSERT (id_ && id_ <= id_gen_);
    RW_ASSERT (rhs.id_ && rhs.id_ <= id_gen_);
    RW_ASSERT (this == &rhs || id_ != rhs.id_);

    // increment the number of times each distinct object
    // has been used as the argument to operator<
    // (do so even if the function throws an exception below)
    ++_RWSTD_CONST_CAST (UserClass*, this)->n_op_lt_;

    if (this != &rhs)
        ++_RWSTD_CONST_CAST (UserClass*, &rhs)->n_op_lt_;

    // increment the total number of invocations of the operator
    // (do so even if the function throws an exception below)
    ++n_total_op_lt_;

#ifndef _RWSTD_NO_EXCEPTIONS

    // throw an exception if the number of calls
    // to operator== reaches the given value

    if (op_lt_throw_ptr_ && n_total_op_lt_ == *op_lt_throw_ptr_) {
        OpLt ex;
        ex.id_ = id_;
        throw ex;
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    return val_ < rhs.val_;
}


bool UserClass::
is_count (size_t n_copy_ctor,
          size_t n_op_assign,
          size_t n_op_eq,
          size_t n_op_lt) const
{
    // verify id validity
    RW_ASSERT (id_ && id_ <= id_gen_);

    return    (size_t (-1) == n_copy_ctor || n_copy_ctor_ == n_copy_ctor)
           && (size_t (-1) == n_op_assign || n_op_assign_ == n_op_assign)
           && (size_t (-1) == n_op_eq     || n_op_eq_     == n_op_eq)
           && (size_t (-1) == n_op_lt     || n_op_lt_     == n_op_lt);
}


/* static */ bool UserClass::
is_total (size_t cnt,
          size_t n_def_ctor,
          size_t n_copy_ctor,
          size_t n_op_assign,
          size_t n_op_eq,
          size_t n_op_lt)
{
    return    (size_t (-1) == cnt         || count_             == cnt)
           && (size_t (-1) == n_def_ctor  || n_total_def_ctor_  == n_def_ctor)
           && (size_t (-1) == n_copy_ctor || n_total_copy_ctor_ == n_copy_ctor)
           && (size_t (-1) == n_op_assign || n_total_op_assign_ == n_op_assign)
           && (size_t (-1) == n_op_eq     || n_total_op_eq_     == n_op_eq)
           && (size_t (-1) == n_op_lt     || n_total_op_lt_     == n_op_lt);
}


/* static */ const UserClass*
UserClass::first_less (const UserClass *xarray, size_t nelems)
{
    size_t inx = nelems;

    if (1 < nelems) {
        for (inx = 1; inx != nelems; ++inx) {
            if (xarray [inx] < xarray [inx - 1])
                break;
        }
    }
    
    return inx < nelems ? xarray + inx : 0;
}


/* static */ void
UserClass::reset_totals ()
{
    n_total_def_ctor_  =
    n_total_copy_ctor_ =
    n_total_dtor_      =
    n_total_op_assign_ =
    n_total_op_eq_     =
    n_total_op_lt_     = 0;
}


typedef unsigned char UChar;

// used to initialize an array of objects of type UserClass
static const char *xinit_begin;

static int xinit ()
{
    if (xinit_begin)
        return UChar (*xinit_begin++);

    return 0;
}


/* static */ UserClass*
UserClass::from_char (const char *str, size_t len /* = -1 */,
                      bool sorted /* = false */)
{
    // handle null pointers
    if (!str)
        return 0;

    // compute the length of the character array if not specified
    if (size_t (-1) == len)
        len = strlen (str);

    if (sorted) {
        // verify that the sequence is sorted
        for (size_t i = 1; i < len; ++i) {
            if (str [i] < str [i - 1]) {
                return 0;
            }
        }
    }

    // set the global pointer to point to the beginning of `str'
    xinit_begin = str;

    // save the previous pointer to the initializer function
    int (*gen_save)() = UserClass::gen_;

    // set the generating function
    UserClass::gen_ = xinit;

    UserClass *array = 0;

    _TRY {
        // allocate and construct `len' elements, initializing
        // each from the character array `str' (via `xinit')
        array = new UserClass [len];
    }
    _CATCH (...) {

        // restore the original initializer function and rethrow
        UserClass::gen_ = gen_save;

        _RETHROW;
    }

    // restore the original initializer function
    UserClass::gen_ = gen_save;

    return array;
}


/* static */ const UserClass*
UserClass::mismatch (const UserClass *xarray, const char *str,
                     size_t len /* = -1 */)
{
    if (!str)
        return xarray;

    if (size_t (-1) == len)
        len = strlen (str);

    for (size_t i = 0; i != len; ++i) {

        const int val = UChar (str [i]);

        if (val != xarray [i].val_)
            return xarray + i;
    }

    return 0;
}


/* static */ int
UserClass::compare (const UserClass *xarray, const char *str,
                    size_t len /* = -1 */)
{
    const UserClass* const px = mismatch (xarray, str, len);

    if (px) {
        RW_ASSERT (size_t (px - xarray) < len);

        return px->val_ - int (UChar (str [px - xarray]));
    }

    return 0;
}


/* static */ int
UserClass::compare (const char *str, const UserClass *xarray,
                    size_t len /* = -1 */)
{
    return -UserClass::compare (xarray, str, len);
}


/* static */ int
UserClass::compare (const UserClass *x, const UserClass *y, size_t count)
{
    for (size_t i = 0; i != count; ++i) {
        if (x [i].val_ != y [i].val_)
            return x [i].val_ - y [i].val_;
    }

    return 0;
}


/* static */ size_t UnaryPredicate::n_total_op_fcall_;


UnaryPredicate::
UnaryPredicate ()
{
    // no-op
}


UnaryPredicate::
UnaryPredicate (const UnaryPredicate&)
{
    // no-op
}


UnaryPredicate& UnaryPredicate::
operator= (const UnaryPredicate&)
{
    return *this;
}


/* virtual */ UnaryPredicate::~UnaryPredicate ()
{
    // no-op
}


/* virtual */ conv_to_bool UnaryPredicate::
operator()(const UserClass&) const
{
    ++n_total_op_fcall_;

    return conv_to_bool::make (true);
}


/* static */ size_t BinaryPredicate::n_total_op_fcall_;


BinaryPredicate::
BinaryPredicate (binary_op op): op_ (op)
{
    // no-op
}


/* virtual */ BinaryPredicate::~BinaryPredicate ()
{
    // no-op
}


/* virtual */ conv_to_bool BinaryPredicate::
operator()(const UserClass &lhs, const UserClass &rhs) /* non-const */
{
    ++n_total_op_fcall_;

    bool result;

    switch (op_) {
    case op_equals:        result = lhs.val_ == rhs.val_; break;
    case op_not_equals:    result = !(lhs.val_ == rhs.val_); break;
    case op_less:          result = lhs.val_ < rhs.val_; break;
    case op_less_equal:    result = !(rhs.val_ < lhs.val_); break;
    case op_greater:       result = rhs.val_ < lhs.val_; break;
    case op_greater_equal: result = !(rhs.val_ < lhs.val_); break;
    }

    return conv_to_bool::make (result);
}


static int
_rw_fmtxarrayv (char **pbuf, size_t *pbufsize, const char *fmt, va_list va)
{
    RW_ASSERT (0 != pbuf);
    RW_ASSERT (0 != pbufsize);
    RW_ASSERT (0 != fmt);

    va_list* pva      =  0;
    bool     fl_plus  =  false;
    bool     fl_pound =  false;
    int      nelems   = -1;
    int      paramno  = -1;
    int      cursor   = -1;

    const UserClass* pelem    = 0;

    // directive syntax:
    // "X=" [ '#' ] [ '+' ] [ '*' | <n> ] [ '.' [ '*' | '@' | <n> ] ]
    // where
    // '#' causes UserClass::id_ to be included in output
    // '+' forces UserClass::val_ to be formatted as an integer (otherwise
    //     it is formatted as an (optionally escaped) character
    // '*' or <n> is the number of elements in the sequence (the
    //     first occurrence)
    // '*', <n> is the offset of the cursor within the sequence
    //          (where the cursor is a pair of pointy brackets
    //          surrounding the element, e.g., >123<)
    // '@' is the pointer to the element to be surrended by the
    //     pair of pointy brackets

    if ('X' != fmt [0] || '=' != fmt [1])
        return _RWSTD_INT_MIN;

    fmt += 2;

    if ('+' == *fmt) {
        // use numerical formatting for UserClass::val_
        fl_plus = true;
        ++fmt;
    }

    if ('#' == *fmt) {
        // include UserClass::id_ in output
        fl_pound = true;
        ++fmt;
    }

    if ('*' == *fmt) {
        // process width
        pva = va_arg (va, va_list*);

        RW_ASSERT (0 != pva);

        // extract the width from rw_snprintfa's variable argument
        // list pass through to us by the caller
        nelems = va_arg (*pva, int);
        ++fmt;
    }
    else if (isdigit (*fmt)) {
        // process positional parameter or width
        char* end = 0;
        const int arg = strtol (fmt, &end, 10);
        if ('$' == *end)
            paramno = arg;
        else
            nelems = arg;

        fmt = end;
    }

    if ('.' == *fmt) {
        // process precision (cursor)
        if ('*' == *++fmt) {
            if (0 == pva)
                pva = va_arg (va, va_list*);

            RW_ASSERT (0 != pva);

            // extract the width from rw_snprintfa's variable argument
            // list passed through to us by the caller
            cursor = va_arg (*pva, int);
            ++fmt;
        }
        else if ('@' == *fmt) {
            if (0 == pva)
                pva = va_arg (va, va_list*);

            RW_ASSERT (0 != pva);

            // extract the pointer from rw_snprintfa's variable argument
            // list passed through to us by the caller
            pelem = va_arg (*pva, UserClass*);

            ++fmt;
        }
        else if (isdigit (*fmt)) {
            char* end = 0;
            cursor = strtol (fmt, &end, 10);

            fmt = end;
        }
    }

    RW_ASSERT ('\0' == *fmt);

    // extract the address of the caller's variable argument list
    if (0 == pva)
        pva = va_arg (va, va_list*);

    RW_ASSERT (0 != pva);

    // extract a pointer to UserClass from rw_snprintfa's variable argument
    // list pass through to us by the caller 
    const UserClass* const xbeg = va_arg (*pva, UserClass*);

    if (-1 != cursor) {
        RW_ASSERT (-1 < cursor);
        RW_ASSERT (0 == pelem);

        pelem = xbeg + cursor;
    }

    // extract the address where to store the extracted argument
    // for use by any subsequent positional paramaters
    const UserClass** const pparam = va_arg (va, const UserClass**);

    RW_ASSERT (0 != pparam);

    // store the extracted argument
    *pparam = xbeg;

    // compute the length of the buffer formatted so far
    const size_t buflen_0 = *pbuf ? strlen (*pbuf) : 0;

    int nbytes = 0;

    //////////////////////////////////////////////////////////////////
    // invoke rw_asnprintf() recursively to format our arguments
    // and append the result to the end of the buffer; pass the
    // value returned from rw_asnprintf() (i.e., the number of
    // bytes appended) back to the caller

    for (const UserClass *px = xbeg; px != xbeg + nelems; ++px) {
        const int n =
            rw_asnprintf (pbuf, pbufsize,
                          "%{+}%{?}>%{;}"
                          "%{?}%d:%{;}"
                          "%{?}%d%{?},%{;}%{:}%{lc}%{;}"
                          "%{?}<%{;}",
                          px == pelem,                    // '>'
                          fl_pound, px->id_,              // "<id>:"
                          fl_plus, px->val_,              // <val>
                          px + 1 < xbeg + nelems,         // ','
                          px->val_,                       // <val>
                          px == pelem);                   // '<'
        if (n < 0)
            return n;

        nbytes += n;
    }

    //////////////////////////////////////////////////////////////////

    // compute the new length of the buffer
    const size_t buflen_1 = *pbuf ? strlen (*pbuf) : 0;

    // assert that the function really appended as many characters
    // as it said it did (assumes no NULs embedded in the output)
    // and that it didn't write past the end of the buffer
    RW_ASSERT (buflen_1 == buflen_0 + nbytes);
    RW_ASSERT (buflen_1 < *pbufsize);

    return nbytes;
}


static int
_rw_fmtxarray (char **pbuf, size_t *pbufsize, const char *fmt, ...)
{
    va_list va;
    va_start (va, fmt);

    const int nbytes = _rw_fmtxarrayv (pbuf, pbufsize, fmt, va);

    va_end (va);

    return nbytes;
}


UserClassFmatInit::
UserClassFmatInit ()
{
    // push a new formatter function on top of the stack
    // of user-defined formatting callbacks invoked by
    // rw_printf() at al to process extended directives
    static int format_init = rw_printf ("%{+!}", _rw_fmtxarray);

    _RWSTD_UNUSED (format_init);
}
