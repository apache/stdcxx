/***************************************************************************
 *
 * 26.partial.sum.cpp - test exercising 26.4.3 [lib.partial.sum]
 *
 * $Id: 
 *
 ***************************************************************************
 *
 * Copyright 2006 The Apache Software Foundation or its licensors,
 * as applicable.
 *
 * Copyright 2006 Rogue Wave Software.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 **************************************************************************/

#include <numeric>      // for partial_sum 
#include <cstddef>      // for size_t

#include <alg_test.h>
#include <driver.h>     // for rw_test()

/**************************************************************************/

// plus
template <class T>
struct plus: T
{
private:
    // unused member prevents bogus HP aCC warnings (see Onyx #23561)
    int unused;
};


template <class T>
const plus<T>& operator+ (const plus<T>& lhs, const plus<T>& rhs)
{
    _RWSTD_UNUSED (rhs);
    return lhs;
}

_RWSTD_NAMESPACE (std) { 

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template 
OutputIter<plus<assign<base<cpy_ctor> > > >
partial_sum (InputIter<plus<assign<base<cpy_ctor> > > >,
             InputIter<plus<assign<base<cpy_ctor> > > >,
             OutputIter<plus<assign<base<cpy_ctor> > > >);

template 
OutputIter<assign<base<cpy_ctor> > > 
partial_sum (InputIter<assign<base<cpy_ctor> > >,
             InputIter<assign<base<cpy_ctor> > >,
             OutputIter<assign<base<cpy_ctor> > >,
             binary_func<assign<base<cpy_ctor> > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

struct Y: public X
{
    // number of times the object's + operator has been invoked,
    // regardless of whether the operation threw an exception or not
    std::size_t n_op_plus_;

    static std::size_t n_total_op_plus_;       // ... + operators ...

    // class thrown from the respective functions
    struct OpPlus: Exception { };

    // throw object's `id' wrapped in the appropriate struct when the
    // corresponding n_total_xxx_ counter reaches the value pointed to
    // by the respective pointer below
    static std::size_t* op_plus_throw_ptr_;

    // objects to which the pointers above initally point
    static std::size_t op_plus_throw_count_;
};

/* static */ std::size_t  Y::n_total_op_plus_;
/* static */ std::size_t* Y::op_plus_throw_ptr_ =
    &Y::op_plus_throw_count_;
/* static */ std::size_t  Y::op_plus_throw_count_ =
    std::size_t (-1);

/**************************************************************************/

Y operator+ (const Y& lhs, const Y& rhs) {

    // increment the number of times each distinct object
    // has been used as the argument to operator+
    // (do so even if the function throws an exception below)
    ++_RWSTD_CONST_CAST (Y*, &lhs)->n_op_plus_;

    if (&lhs != &rhs)
        ++_RWSTD_CONST_CAST (Y*, &rhs)->n_op_plus_;

    // increment the total number of invocations of the operator
    // (do so even if the function throws an exception below)
    ++Y::n_total_op_plus_;

#ifndef _RWSTD_NO_EXCEPTIONS

    // throw an exception if the number of calls
    // to operator== reaches the given value

    if (   lhs.op_plus_throw_ptr_
        && Y::n_total_op_plus_ == *lhs.op_plus_throw_ptr_) {
        Y::OpPlus ex;
        ex.id_ = lhs.id_;
        throw ex;
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    Y res (lhs);
    res.val_ += rhs.val_;
    return res;
}


/**************************************************************************/

template <class T>
struct conv_to_T
{
    static conv_to_T make (T val) {
        return conv_to_T (val);
    }

    // strictly convertible to a T value 
    operator T () const {
        return val_;
    }

private:
    // not (publicly) Default-Constructible
    conv_to_T (T val): val_ (val) { }

    void operator= (conv_to_T);   // not Assignable
    void operator!() const;       // not defined

    T val_;
};

/**************************************************************************/

struct Accumulator
{
    static std::size_t funcalls_;

    // dummy arguments provided to prevent the class from being
    // default constructible and implicit conversion from int
    Accumulator (int /* dummy */, int /* dummy */) {
        funcalls_ = 0;
    }

    // return a type convertible to Y
    conv_to_T<Y> operator() (const Y &x, const Y &y) /* non-const */ {
        ++funcalls_;
        Y res (x);
        res.val_ += y.val_;
        return conv_to_T<Y>::make (res);
    }

private:
    void operator= (Accumulator&);   // not assignable
};

std::size_t Accumulator::funcalls_;

/**************************************************************************/

struct PartialSumBase
{
    virtual ~PartialSumBase() { /* no-op */ }

    const char* iter_names [2];

    // pure virtual
    virtual Y*
    partial_sum (const Y *xsrc, const Y *xsrc_end,
                 Y *xdst, const Y *xdst_end,
                 const Accumulator *op) const = 0;
};


template <class InputIterator, class OutputIterator>
struct PartialSum: PartialSumBase
{
    PartialSum () {
        iter_names [0] = type_name (InputIterator (0, 0, 0), (Y*)0);
        iter_names [1] = type_name (OutputIterator (0, 0, 0), (Y*)0);
    }

    virtual Y*
    partial_sum (const Y *xsrc, const Y *xsrc_end,
                 Y *xdst, const Y *xdst_end,
                 const Accumulator *op) const {

        const InputIterator  first (xsrc,     xsrc, xsrc_end);
        const InputIterator  last  (xsrc_end, xsrc, xsrc_end);
        const OutputIterator result (xdst,    xdst, xdst_end);

        const OutputIterator res = op ?
              std::partial_sum (first, last, result, *op)
            : std::partial_sum (first, last, result);

        // silence EDG eccp 3.7 and prior remark #550-D:
        //   variable was set but never used
        _RWSTD_UNUSED (res);

        return res.cur_;
    }
};

/**************************************************************************/

// exercises partial_sum (26.4.3)
void test_partial_sum (const std::size_t       N,
                       const PartialSumBase   &alg,
                       bool                    binop,
                       bool                    same_seq)
{
    const char* const itname =  alg.iter_names [0];
    const char* const outname = alg.iter_names [1];
    const char* const opname = "Plus";

    rw_info (0, 0, 0, 
             "std::partial_sum (%s, %1$s, %s%{?}, %s%{;})%{?}, %s%{;}",
             itname, outname, binop, opname, same_seq, "first == result");

    Y::gen_ = gen_seq;

    Y* const src = new Y [N];
    Y* dst = same_seq ? src : new Y [N];

    for (std::size_t i = 0; i != N; ++i) {

        Y* const src_end = src + i;
        Y* const dst_end = dst + i;

        std::size_t last_n_op_plus  = Y::n_total_op_plus_;

        const Accumulator acc  (0, 0);
        const Accumulator* const pbinop = binop ? &acc : 0;

        std::size_t k = 0;
        int* const tmp_val = new int [i];
        for (; k < i; ++k)
            tmp_val [k] = src [k].val_;

        const Y* const res =
            alg.partial_sum (src, src_end, dst, dst_end, pbinop);

        const std::size_t plus_ops = binop ? Accumulator::funcalls_ : 
            Y::n_total_op_plus_ - last_n_op_plus;

        // verify the returned iterator 26.4.3, p2
        bool success = res == dst_end;
        rw_assert (success, 0, __LINE__,
                   "step %zu: partial_sum<%s, %s%{?}, %s%{;}> = %p"
                   " expected %p, difference %td",
                   i + 1, itname, outname, binop, opname, res, dst_end,
                   res - dst_end);

        int sum = 0;
        for (k = 0; k < i; ++k) {
            sum += tmp_val [k];
            success = dst [k].val_ == sum;
            if (!success)
                break;
        }

        // verify the result 26.4.3, p1
        if (i > 0) {
            // to avoid errors in --trace mode
            k = k < i ? k : i - 1;

            rw_assert (success, 0, __LINE__,
                       "step %zu: partial_sum<%s, %s%{?}, %s%{;}>: "
                       "got %u at %zu, expected %u here",
                       i + 1, itname, outname, binop, opname, dst[k].val_, 
                       k + 1, sum);
        }

        delete[] tmp_val;

        if (!success)
            break;

        // verify the complexity, 26.4.3, p3
        const std::size_t exp_plus_ops = i > 0 ? i - 1 : 0;
        success = plus_ops == exp_plus_ops;
        rw_assert (success, 0, __LINE__,
                   "step %zu: partial_sum <%s, %s%{?}, %s%{;}> "
                   "complexity: got %zu invocations of %s, expected %zu",
                   i + 1, itname, outname, binop, opname, plus_ops, 
                   binop ? "BinaryPlus" : "operator+", exp_plus_ops);

        if (!success)
            break;
    }

    delete[] src;

    if (!same_seq)
        delete[] dst;
}

/**************************************************************************/

/* extern */ int rw_opt_nloops = 64;            // --nloops
/* extern */ int rw_opt_no_binary_op;           // --no-binary_op
/* extern */ int rw_opt_no_input_iter;          // --no-InputIterator
/* extern */ int rw_opt_no_output_iter;         // --no-OutputIterator
/* extern */ int rw_opt_no_fwd_iter;            // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;          // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;            // --no-RandomAccessIterator

/**************************************************************************/

template <class InputIterator, class OutputIterator>
void gen_partial_sum_test (const std::size_t N,
                           const InputIterator&,
                           const OutputIterator&,
                           bool              binop)
{
    const PartialSum<InputIterator, OutputIterator> alg;

    // test the algorithm than input and output arrays differ
    test_partial_sum (N, alg, binop, false);

    // test the algorithm than input and output arrays are the same
    test_partial_sum (N, alg, binop, true);
}


template <class InputIterator>
void gen_partial_sum_test (const std::size_t     N,
                           const InputIterator  &it, 
                           bool                  binop)
{
    if (0 == rw_opt_no_output_iter)
        gen_partial_sum_test (
            N, it, OutputIter<Y>(0, 0, 0), binop);
    if (0 == rw_opt_no_fwd_iter)
        gen_partial_sum_test (
            N, it, FwdIter<Y>(0, 0, 0), binop);
    if (0 == rw_opt_no_bidir_iter)
        gen_partial_sum_test (
            N, it, BidirIter<Y>(0, 0, 0), binop);
    if (0 == rw_opt_no_rnd_iter)
        gen_partial_sum_test (
            N, it, RandomAccessIter<Y>(0, 0, 0), binop);
}

// generates a specialization of the partial_sum test for each of the required
// iterator categopries
void gen_partial_sum_test (const std::size_t N,
                           bool              binop)
{
    rw_info (0, 0, 0,
             "template <class %s, class %s%{?}, class %s%{;}> "
             "%2$s partial_sum (%1$s, %1$s, %2$s%{?}, %s%{;})", 
             "InputIterator", "OutputIterator", binop, "BinaryOperation", 
             binop, "BinaryOperation");

    if (rw_opt_no_output_iter)
        rw_note (0, 0, 0, "OutputIterator test disabled");

    if (rw_opt_no_input_iter)
        rw_note (0, 0, 0, "InputIterator test disabled");
    else
        gen_partial_sum_test (N, InputIter<Y>(0, 0, 0), binop);

    if (rw_opt_no_fwd_iter)
        rw_note (0, 0, 0, "ForwardIterator test disabled");
    else
        gen_partial_sum_test (N, ConstFwdIter<Y>(0, 0, 0), binop);

    if (rw_opt_no_bidir_iter)
        rw_note (0, 0, 0, "BidirectionalIterator test disabled");
    else
        gen_partial_sum_test (N, ConstBidirIter<Y>(0, 0, 0), binop);

    if (rw_opt_no_rnd_iter)
        rw_note (0, 0, 0, "RandomAccessIterator test disabled");
    else
        gen_partial_sum_test (N, ConstRandomAccessIter<Y>(0, 0, 0), binop);
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    const std::size_t N = std::size_t (rw_opt_nloops);

    gen_partial_sum_test (N, false);

    if (rw_opt_no_binary_op)
        rw_note (0, 0, 0, 
                 "partial_sum with binary operation test disabled");
    else
        gen_partial_sum_test (N, true);

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.partial.sum",
                    0 /* no comment */,
                    run_test,
                    "|-nloops#0 "   // must be non-negative
                    "|-no-binary_op#"
                    "|-no-InputIterator# "
                    "|-no-OutputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator#",
                    &rw_opt_nloops,
                    &rw_opt_no_binary_op,
                    &rw_opt_no_input_iter,
                    &rw_opt_no_output_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
