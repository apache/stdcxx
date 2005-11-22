/***************************************************************************
 *
 * 25.for_each.cpp - test exercising 25.1.1 [lib.alg.foreach]
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

#include <algorithm>     // for for_each
#include <cstddef>       // for size_t

#include <alg_test.h>
#include <driver.h>      // for rw_test(), ...

/**************************************************************************/

struct FunctionBase
{
    static std::size_t  funcalls_;
    static int          sum_;
    bool                is_const_;

    FunctionBase (bool is_const)
        : is_const_ (is_const) {
        funcalls_ = 0;
        sum_      = 0;
    }

private:
    void operator= (FunctionBase&);   // not assignable
};

std::size_t FunctionBase::funcalls_;
int         FunctionBase::sum_;


struct ConstFunction: FunctionBase
{
    // dummy arguments provided to prevent the class
    // from being default constructible
    ConstFunction (int, int): FunctionBase (true) { }

    void operator() (X val)  /* not const */ {
        ++funcalls_;
        sum_ += val.val_;
    }

    static const char* name () { return "ConstFunction"; }
};


struct MutableFunction: FunctionBase
{
    // dummy arguments provided to prevent the class
    // from being default constructible
    MutableFunction (int, int): FunctionBase (false) { }

    void operator() (X &val) /* not const */ {
        ++funcalls_;
        val.val_ = -val.val_;
    }

    static const char* name () { return "MutableFunction"; }
};


// exercises std::for_each()
template <class InputIterator, class T, class Function>
void test_for_each (std::size_t N, InputIterator dummy, T*, Function*)
{
    static const char* const itname = type_name (dummy, (T*)0);
    static const char* const fnname = Function::name ();

    rw_info (0, 0, 0, "std::for_each (%s, %1$s, %s)", itname, fnname);

    // generate sequential values for each default constructed X
    X::gen_ = gen_seq;

    X *buf = new X [N];

    const int first_val = buf [0].val_;

    for (std::size_t i = 0; i != N; ++i) {

        X* const buf_end = buf + i;

        const InputIterator first =
            make_iter (buf, buf, buf_end, dummy);

        const InputIterator last =
            make_iter (buf_end, buf_end, buf_end, dummy);

        // create a const function object and zero out all its counters
        const Function fun (0, 0);

        std::for_each (first, last, fun);

        // compute the sum of elements in the sequence and check each
        // element's value against the expected one
        int sum = 0;

        for (std::size_t j = 0; j != i; ++j) {

            int expect;

            if (fun.is_const_) {
                // const function object doesn't modify the subject
                // sequence; the expected value of the element is
                // the same as the original value
                expect = first_val + int (j);
            }
            else {
                // non-const function object negates each argument
                expect = -(first_val + int (j));
            }

            // compute the sum (computed by the const function object)
            sum += buf [j].val_;

            // assert the element value as the same as the expected value
            rw_assert (expect == buf [j].val_, 0, __LINE__,
                       "for_each (%s, %1$s, %s); element [%zu] == %d, got %d",
                       itname, fnname, j, expect, buf [j].val_);

            if (expect != buf [j].val_) {
                // break out of both loops on failure
                i = N;
                break;
            }

            // restore the original value of the element
            buf [j].val_ = first_val + int (j);
        }

        // assert that for_each invoked the function object's operator()
        // exactly as many times as necessary and required
        rw_assert (i == fun.funcalls_, 0, __LINE__,
                   "for_each (%s, %1$s, %s); expected %zu invocations of "
                   "Function::operator(), got %zu", itname, fnname,
                   i, fun.funcalls_);

        if (fun.is_const_) {
            rw_assert (sum == fun.sum_, 0, __LINE__,
                       "for_each (%s, %1$s, %s); sum of %zu elements == %d, "
                       "got %d", itname, fnname, sum, fun.sum_);
        }
    }

    delete[] (buf);
}

/**************************************************************************/

static int rw_opt_no_input_iter;   // --no-InputIterator
static int rw_opt_no_fwd_iter;     // --no-ForwardIterator
static int rw_opt_no_bidir_iter;   // --no-BidirectionalIterator
static int rw_opt_no_rnd_iter;     // --no-RandomAccessIterator


static int
run_test (int, char*[])
{
    static const std::size_t N = 32;

    rw_info (0, 0, 0,
             "template <class %s, class %s> "
             "%2$s std::for_each (%1$s, %1$s, %2$s)",
             "InputIterator", "Function");

    if (rw_opt_no_input_iter) {
        rw_note (0, __FILE__, __LINE__, "InputIterator test disabled");
    }
    else {
        test_for_each (N, InputIter<X>(0, 0, 0), (X*)0, (ConstFunction*)0);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_for_each (N, ConstFwdIter<X>(), (X*)0, (ConstFunction*)0);
        test_for_each (N, FwdIter<X>(), (X*)0, (ConstFunction*)0);
        test_for_each (N, FwdIter<X>(), (X*)0, (MutableFunction*)0);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_for_each (N, ConstBidirIter<X>(), (X*)0, (ConstFunction*)0);
        test_for_each (N, BidirIter<X>(), (X*)0, (ConstFunction*)0);
        test_for_each (N, BidirIter<X>(), (X*)0, (MutableFunction*)0);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_for_each (N, ConstRandomAccessIter<X>(), (X*)0, (ConstFunction*)0);
        test_for_each (N, RandomAccessIter<X>(), (X*)0, (ConstFunction*)0);
        test_for_each (N, RandomAccessIter<X>(), (X*)0, (MutableFunction*)0);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.foreach",
                    0 /* no comment */, run_test,
                    "|-no-InputIterator#"
                    "|-no-ForwardIterator#"
                    "|-no-BidirectionalIterator#"
                    "|-no-RandomAccessIterator#",
                    &rw_opt_no_input_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
