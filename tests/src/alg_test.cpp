/************************************************************************
 *
 * alg_test.cpp - definitions of class X members
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

#include <stddef.h>     // for size_t
#include <stdlib.h>     // for rand()
#include <string.h>     // for strlen()
#include <ctype.h>      // for toupper()

#include <alg_test.h>


#ifdef _RWSTDDEBUG
   // use own assertion mechanism for better diagnostic output
#  undef assert
#  define assert(expr) _RWSTD_ASSERT(expr)
#else
#  include <assert.h>     // for assert()
#endif   // _RWSTDDEBUG


/* static */ size_t X::count_;
/* static */ int    X::id_gen_;   // generates unique non-zero ids
/* static */ int  (*X::gen_)();   // extern "C++" int (*)()

/* static */ size_t X::n_total_def_ctor_;
/* static */ size_t X::n_total_copy_ctor_;
/* static */ size_t X::n_total_dtor_;
/* static */ size_t X::n_total_op_assign_;
/* static */ size_t X::n_total_op_eq_;
/* static */ size_t X::n_total_op_lt_;

// default values of pointers
/* static */ size_t* X::def_ctor_throw_ptr_  = &X::def_ctor_throw_count_;
/* static */ size_t* X::copy_ctor_throw_ptr_ = &X::copy_ctor_throw_count_;
/* static */ size_t* X::dtor_throw_ptr_      = &X::dtor_throw_count_;
/* static */ size_t* X::op_assign_throw_ptr_ = &X::op_assign_throw_count_;
/* static */ size_t* X::op_eq_throw_ptr_     = &X::op_eq_throw_count_;
/* static */ size_t* X::op_lt_throw_ptr_     = &X::op_lt_throw_count_;

// exception throwing initially disabled
/* static */ size_t X::def_ctor_throw_count_  = size_t (-1);
/* static */ size_t X::copy_ctor_throw_count_ = size_t (-1);
/* static */ size_t X::dtor_throw_count_      = size_t (-1);
/* static */ size_t X::op_assign_throw_count_ = size_t (-1);
/* static */ size_t X::op_eq_throw_count_     = size_t (-1);
/* static */ size_t X::op_lt_throw_count_     = size_t (-1);


X::X ()
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


X::X (const X &rhs)
    : id_ (++id_gen_), origin_ (rhs.origin_), src_id_ (rhs.id_),
      val_ (rhs.val_),
      n_copy_ctor_ (0), n_op_assign_ (0), n_op_eq_ (0), n_op_lt_ (0)
{
    // verify id validity
    assert (rhs.id_ && rhs.id_ < id_gen_);

    // increment the number of times `rhs' has been copied
    // (do so even if the function throws an exception below)
    ++_RWSTD_CONST_CAST (X*, &rhs)->n_copy_ctor_;

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


X::~X ()
{
    // verify id validity
    assert (id_ && id_ <= id_gen_);

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


X&
X::operator= (const X &rhs)
{
    // verify id validity and uniqueness
    assert (id_ && id_ <= id_gen_);
    assert (rhs.id_ && rhs.id_ <= id_gen_);
    assert (this == &rhs || id_ != rhs.id_);

    // increment the total number of invocations of the operator
    // (do so even if the function throws an exception below)
    ++n_total_op_assign_;

    // increment the number of times the object has been assigned to
    // (do so even if the function throws an exception below)
    ++n_op_assign_;

#ifndef _RWSTD_NO_EXCEPTIONS

    // throw an exception if the number of calls to
    // the assignment operator reaches the given value

    if (op_assign_throw_ptr_ && n_total_op_assign_ == *op_assign_throw_ptr_) {
        OpAssign ex;
        ex.id_ = id_;
        throw ex;
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    // overwrite value and source id only when the operation
    // is successful (i.e., only when it doesn't throw)

    origin_ = rhs.origin_;
    src_id_ = rhs.id_;
    val_    = rhs.val_;

    return *this;
}


bool
X::operator== (const X &rhs) const
{
    // verify id validity and uniqueness
    assert (id_ && id_ <= id_gen_);
    assert (rhs.id_ && rhs.id_ <= id_gen_);
    assert (this == &rhs || id_ != rhs.id_);

    // increment the number of times each distinct object
    // has been used as the argument to operator==
    // (do so even if the function throws an exception below)
    ++_RWSTD_CONST_CAST (X*, this)->n_op_eq_;

    if (this != &rhs)
        ++_RWSTD_CONST_CAST (X*, &rhs)->n_op_eq_;

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
X::operator< (const X &rhs) const
{
    // verify id validity and uniqueness
    assert (id_ && id_ <= id_gen_);
    assert (rhs.id_ && rhs.id_ <= id_gen_);
    assert (this == &rhs || id_ != rhs.id_);

    // increment the number of times each distinct object
    // has been used as the argument to operator<
    // (do so even if the function throws an exception below)
    ++_RWSTD_CONST_CAST (X*, this)->n_op_lt_;

    if (this != &rhs)
        ++_RWSTD_CONST_CAST (X*, &rhs)->n_op_lt_;

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


bool X::
is_count (size_t n_copy_ctor,
          size_t n_op_assign,
          size_t n_op_eq,
          size_t n_op_lt) const
{
    // verify id validity
    assert (id_ && id_ <= id_gen_);

    return    (size_t (-1) == n_copy_ctor || n_copy_ctor_ == n_copy_ctor)
           && (size_t (-1) == n_op_assign || n_op_assign_ == n_op_assign)
           && (size_t (-1) == n_op_eq     || n_op_eq_     == n_op_eq)
           && (size_t (-1) == n_op_lt     || n_op_lt_     == n_op_lt);
}


/* static */ bool X::
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


/* static */ void
X::reset_totals ()
{
    n_total_def_ctor_  =
    n_total_copy_ctor_ =
    n_total_dtor_      =
    n_total_op_assign_ =
    n_total_op_eq_     =
    n_total_op_lt_     = 0;
}


typedef unsigned char UChar;

// used to initialize an array of objects of type X
static const char *xinit_begin;

static int xinit ()
{
    if (xinit_begin)
        return UChar (*xinit_begin++);

    return 0;
}


/* static */ X*
X::from_char (const char *str, size_t len /* = -1 */)
{
    // handle null pointers
    if (!str)
        return 0;

    // compute the length of the character array if not specified
    if (size_t (-1) == len)
        len = strlen (str);

    // set the global pointer to point to the beginning of `str'
    xinit_begin = str;

    // save the previous pointer to the initializer function
    int (*gen_save)() = X::gen_;

    // set the generating function
    X::gen_ = xinit;

    X *array = 0;

    _TRY {
        // allocate and construct `len' elements, initializing
        // each from the character array `str' (via `xinit')
        array = new X [len];
    }
    _CATCH (...) {

        // restore the original initializer function and rethrow
        X::gen_ = gen_save;

        _RETHROW;
    }

    // restore the original initializer function
    X::gen_ = gen_save;

    return array;
}


/* static */ int
X::compare (const X *x, const char *str, size_t len /* = -1 */)
{
    if (!str)
        return x == 0;

    if (size_t (-1) == len)
        len = strlen (str);

    for (size_t i = 0; i != len; ++i) {

        const int val = UChar (str [i]);

        if (val != x [i].val_)
            return x [i].val_ - val;
    }

    return 0;
}


/* static */ int
X::compare (const char *str, const X *x, size_t len /* = -1 */)
{
    return -X::compare (x, str, len);
}


/* static */ int
X::compare (const X *x, const X *y, size_t count)
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


/* virtual */ bool UnaryPredicate::
operator()(const X&) const
{
    ++n_total_op_fcall_;

    return true;
}


/* static */ size_t BinaryPredicate::n_total_op_fcall_;


BinaryPredicate::
BinaryPredicate (bool ignore_case)
    : ignore_case_ (ignore_case)
{
    // no-op
}


BinaryPredicate::
BinaryPredicate (const BinaryPredicate &rhs)
    : ignore_case_ (rhs.ignore_case_)
{
    // no-op
}


BinaryPredicate& BinaryPredicate::
operator= (const BinaryPredicate &rhs)
{
    ignore_case_ = rhs.ignore_case_;
    return *this;
}


/* virtual */ BinaryPredicate::~BinaryPredicate ()
{
    // no-op
}


/* virtual */ bool BinaryPredicate::
operator()(const X &lhs, const X &rhs) const
{
    ++n_total_op_fcall_;

    if (lhs == rhs)
        return true;

    if (ignore_case_) {

        const int lval = lhs.val_;
        const int rval = rhs.val_;

        if (   lval < 0 || lval > int (_RWSTD_UCHAR_MAX)
            || rval < 0 || rval > int (_RWSTD_UCHAR_MAX))
            return false;

        const int lup = toupper (lval);
        const int rup = toupper (rval);

        return _RWSTD_EOF != lup && lup == rup;
    }

    return false;
}


// generate a unique sequential number starting from 0
_TEST_EXPORT int gen_seq ()
{
    static int val;

    return ++val;
}


// generate numbers in the sequence 0, 0, 1, 1, 2, 2, 3, 3, etc... 
_TEST_EXPORT int gen_seq_2lists ()
{
    static int vals [2];

    return vals [0] += ++vals [1] % 2;
}


// generate a sequence of subsequences (i.e., 0, 1, 2, 3, 4, 0, 1, 2, etc...)
_TEST_EXPORT int gen_subseq ()
{
    static int val;

    return val++ % 5;
}


// wrapper around a (possibly) extern "C" int rand()
// extern "C++" 
_TEST_EXPORT int gen_rnd ()
{
    return rand ();
}
