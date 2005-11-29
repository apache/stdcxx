/***************************************************************************
 *
 * 25.fill.cpp - test exercising 25.2.5 [lib.alg.fill]
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

#include <algorithm>    // for fill
#include <cstddef>      // for size_t

#include <alg_test.h>   
#include <driver.h>     // for rw_test()

/**************************************************************************/

// exercises std::fill()
template <class ForwardIterator, class T>
void test_fill (std::size_t            N,
                const ForwardIterator& fill_iter,
                const T*)
{
    static const char* const itname = type_name (fill_iter, (T*) 0);
    static const char* const tname  = "X";

    rw_info (0, 0, 0, "void std::fill (%s, %1$s, const %s&)", itname, tname);

    // generate sequential values for each default constructed T
    T::gen_ = gen_seq;

    // use ::operator new() to prevent default initialization
    T *buf = _RWSTD_STATIC_CAST (T*, ::operator new (N * sizeof (T)));

    // default-construct the first T at buf[0]
    new (buf) T ();

    for (std::size_t i = 0; i < N; ++i) {
        // default-construct a new X at the end of buf
        T* const new_t = new (buf + i) T ();

        // exercise 25.2.5 - std::fill<> ()
        std::size_t last_n_op_assign = T::n_total_op_assign_;

        T* const buf_end = buf + i + 1;

        const ForwardIterator begin =
            make_iter (buf, buf, buf_end, fill_iter);

        const ForwardIterator end =
            make_iter (buf_end, buf_end, buf_end, fill_iter);

        std::fill (begin, end, *new_t);

        // verify 25.2.5, p2
        bool success;
        std::size_t j = 0;
        for ( ; j != i + 1; ++j) {
            success = buf[j].val_ == new_t->val_;
            if (!success)
                break;
        }

        rw_assert (success, 0, __LINE__, 
                   "%zu. fill (%s, %1$s, const %s&): buf[%zu]: %d != %d",
                   i + 1, itname, tname, j, buf[j].val_, new_t->val_);

        if (!success)
            break;

        // verify 25.2.5, p3
        success = T::n_total_op_assign_ - last_n_op_assign == i + 1;
        rw_assert (success, 0, __LINE__,
                   "%zu. fill (%s, %1$s, const %s&) complexity: "
                   "%zu != %zu", i + 1, itname, tname,
                   T::n_total_op_assign_ - last_n_op_assign, i + 1);

        if (!success)
            break;
    }

    ::operator delete (buf);
}

/**************************************************************************/

// exercises std::fill_n()
template <class OutputIterator, class T>
void test_fill_n (std::size_t         N,
                  const OutputIterator& fill_iter,
                  const T*)
{
    static const char* const itname = type_name (fill_iter, (T*) 0);
    static const char* const szname = "std::size_t";
    static const char* const tname  = "X";

    rw_info (0, 0, 0, "void std::fill_n (%s, %s, const %s&)",
             itname, szname, tname);

    // generate sequential values for each default constructed T
    T::gen_ = gen_seq;

    // use ::operator new() to prevent default initialization
    T *buf = _RWSTD_STATIC_CAST (T*, ::operator new (N * sizeof (T)));

    // default-construct the first T at buf[0]
    new (buf) T ();

    for (std::size_t i = 0; i < N; ++i) {
        // default-construct a new X at the end of buf
        T* const new_t = new (buf + i) T ();

        // exercise 25.2.5 - std::fill<> ()
        std::size_t last_n_op_assign = T::n_total_op_assign_;

        T* const buf_end = buf + i + 1;

        const OutputIterator begin =
            make_iter (buf, buf, buf_end, fill_iter);

        std::fill_n (begin, i, *new_t);

        bool success = true;

        // verify 25.2.5, p2
        std::size_t j = 0;
        for ( ; j != i; ++j) {
            success = buf[j].val_ == new_t->val_;
            if (!success)
                break;
        }

        rw_assert (success, 0, __LINE__, 
                   "%zu. fill_n (%s, %s, const %s&): buf[%zu]: %d != %d",
                   i + 1, itname, szname, tname, j, buf[j].val_, new_t->val_);

        if (!success)
            break;

        success = T::n_total_op_assign_ - last_n_op_assign == i;
        rw_assert (success, 0, __LINE__,
                   "%zu. fill_n (%s, %s, const %s&) complexity: "
                   "%zu != %zu", i + 1, itname, szname, tname,
                   T::n_total_op_assign_ - last_n_op_assign, i);

        if (!success)
            break;
    }

    ::operator delete (buf);
}


/**************************************************************************/

/* extern */ int rw_opt_nloops = 32;     // --nloops
/* extern */ int rw_opt_no_output_iter;  // --no-OutputIterator
/* extern */ int rw_opt_no_fwd_iter;     // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;   // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;     // --no-RandomAccessIterator

static void
test_fill (const std::size_t N)
{
    rw_info (0, 0, 0,
            "template <class %s, class %s> "
            "void std::fill (%1$s, %1$s, const %2$s&)",
            "ForwardIterator", "T");

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_fill (N, FwdIter<X>(), (X*)0);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_fill (N, BidirIter<X>(), (X*)0);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__,  "RandomAccessIterator test disabled");
    }
    else {
        test_fill (N, RandomAccessIter<X>(), (X*)0);
    }
}

/**************************************************************************/

static void
test_fill_n (const std::size_t N)
{
    rw_info (0, 0, 0,
            "template <class %s, class %s, class %s> "
            "void std::fill_n (%1$s, %2$s, const %3$s&)",
            "OutputIterator", "Size", "T");

    if (rw_opt_no_output_iter) {
        rw_note (0, __FILE__, __LINE__, "OutputIterator test disabled");
    }
    else {
        test_fill_n (N, OutputIter<X>(0, 0, 0), (X*)0);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_fill_n (N, FwdIter<X>(), (X*)0);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_fill_n (N, BidirIter<X>(), (X*)0);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_fill_n (N, RandomAccessIter<X>(), (X*)0);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    // check that the number of loops is non-negative
    rw_fatal (-1 < rw_opt_nloops, 0, 0,
              "number of loops must be non-negative, got %d",
              rw_opt_nloops);

    const std::size_t N = std::size_t (rw_opt_nloops);

    test_fill (N);

    test_fill_n (N);

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.fill",
                    0 /* no comment */, run_test,
                    "|-nloops# "
                    "|-no-OutputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator#",
                    &rw_opt_nloops,
                    &rw_opt_no_output_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
