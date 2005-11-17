/***************************************************************************
 *
 * for_each.cpp - test exercising 25.1.1 [lib.alg.foreach]
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
#include <functional>    // for negate, unary_function
#include <cstddef>       // for ptrdiff_t

#include <alg_test.h>
#include <driver.h>      // for rw_test(), ...

/**************************************************************************/

_RWSTD_NAMESPACE (std) { 

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
func<base<> >
for_each (InputIter<base<> >, InputIter<base<> >, func<base<> >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

// working around aCC error 708: A definition of a namespace declaration
// must be written in directly in the namespace (unqualified) or in an
// enclosing namespace (qualified).
_RWSTD_NAMESPACE (std) { 

_RWSTD_SPECIALIZED_CLASS
struct negate<X>: unary_function<X, X>
{
    void operator() (argument_type &x) {
        x.val_ = -x.val_;
    }
};

}   // namespace std

// exercises std::for_each()
template <class ForwardIterator, class T>
void test_for_each (std::size_t N, ForwardIterator, T*)
{
    static const char* const itname = type_name (ForwardIterator (), (T*)0);

    rw_info (0, 0, 0, "std::for_each (%s, %s, Function)", itname, itname);

    // generate sequential values for each default constructed X
    X::gen_ = gen_seq;

    // use ::operator new() to prevent default initialization
    X* const buf = _RWSTD_STATIC_CAST (X*, ::operator new (N * sizeof (X)));

    std::size_t i;

    for (i = 0; i != N; ++i) {

        X* const buf_end = buf + i + 1;

        // default-construct a new X at the end of the list
        X* new_x = new (buf + i) X ();

        // exercise 25.1.1 - std::for_each()
        std::size_t last_n_op_assign = X::n_total_op_assign_;

        const ForwardIterator first =
            make_iter (buf, buf, buf_end, ForwardIterator ());

        const ForwardIterator last =
            make_iter (buf_end, buf_end, buf_end, ForwardIterator ());

        std::for_each (first, last, std::negate<X>());

        int success = 0;

        // verify 25.1.1, p2
        std::ptrdiff_t j;

        for (j = 0; j != std::ptrdiff_t (i); ++j ) {
            switch (j % 2) {
            case 0:
                success = new_x->val_ != -j;
                rw_assert (success, 0, __LINE__,
                           "%d. std::for_each () correctness: %td != %td",
                           j + 1, new_x->val_, -j);
                break;
            case 1:
                success = new_x->val_ != j;
                rw_assert (success, 0, __LINE__,
                           "%d. std::for_each () correctness: %td != %td",
                           j + 1, new_x->val_, j);
                break;
            }
        }
        
        // verify 25.1.1, p3
        success = X::n_total_op_assign_ - last_n_op_assign <= i + 1;

        rw_assert (success, 0, __LINE__,
                   "%d. std::for_each () complexity: %zu <= %zu",
                   X::n_total_op_assign_, i + 1);

        // break out of the loop if assertion fails
        if (!success)
            break;
    }

    rw_assert (N == i, 0, __LINE__, "passed %zu of %zu iterations", i, N);

    ::operator delete (buf);
}

/**************************************************************************/

static int rw_opt_no_fwd_iter;     // --no-ForwardIterator
static int rw_opt_no_bidir_iter;   // --no-BidirectionalIterator
static int rw_opt_no_rnd_iter;     // --no-RandomAccessIterator


static int
run_test (int, char*[])
{
    static const std::size_t N = 1024;

    rw_info (0, 0, 0,
             "template <class ForwardIterator, class Function> "
             "Function "
             "std::for_each (ForwardIterator, ForwardIterator, Function)");

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_for_each (N, FwdIter<X>(), (X*)0);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_for_each (N, BidirIter<X>(), (X*)0);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_for_each (N, RandomAccessIter<X>(), (X*)0);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.foreach",
                    0 /* no comment */, run_test,
                    "|-no-ForwardIterator#"
                    "|-no-BidirectionalIterator#"
                    "|-no-RandomAccessIterator#",
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
