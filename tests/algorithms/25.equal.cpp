/***************************************************************************
 *
 * 25.equal.cpp - test exercising 25.1.8 [lib.alg.equal]
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

#include <algorithm>    // for equal
#include <functional>   // for equal_to
#include <cstddef>      // for size_t

#include <alg_test.h>
#include <driver.h>     // for rw_test()...

/**************************************************************************/

_RWSTD_NAMESPACE (std) { 

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
bool
equal (InputIter<eq_comp<base<> > >, 
       InputIter<eq_comp<base<> > >,
       InputIter<eq_comp<base<> > >);

template
bool
equal (InputIter<eq_comp<base<> > >, 
       InputIter<eq_comp<base<> > >, 
       InputIter<eq_comp<base<> > >, 
       binary_predicate<eq_comp<base<> > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std


// exercises std::equal()
template <class InputIterator1, class InputIterator2, class T>
void test_equal (std::size_t           N,
                 const InputIterator1 &it1_dummy,
                 const InputIterator2 &it2_dummy,
                 T*,
                 const char           *predicate)
{
    static const char* const it1name = type_name (it1_dummy, (T*)0);
    static const char* const it2name = type_name (it2_dummy, (T*)0);

    rw_info (0, 0, 0, "std::equal (%s, %1$s, %s%{?}, %s%{;})",
             it1name, it2name, 0 != predicate, predicate);

    // generate sequential values for each default constructed X
    // for both lists
    X::gen_ = gen_seq_2lists;

    // use ::operator new() to prevent default initialization
    X *buf1 = _RWSTD_STATIC_CAST (X*, ::operator new (N * sizeof (X)));
    X *buf2 = _RWSTD_STATIC_CAST (X*, ::operator new (N * sizeof (X)));

    const std::size_t mid_inx = N / 2;

    for (std::size_t i = 0; i != N; ++i) {

        // default-construct a new X at the end of the array
        new (buf1 + i) X ();

        // build a nearly identical array only missing the N/2-th element
        if (i != mid_inx)
            new (buf2 + i) X ();
    }

    new (buf2 + mid_inx) X ();

    for (std::size_t i = 0; i != N; ++i) {
    
        // exercise 25.1.8 - std::equal()
        std::size_t last_n_op_eq  = X::n_total_op_eq_;

        X* const buf1_end = buf1 + i + 1;
        X* const buf2_end = buf2 + i + 1;

        const InputIterator1 first1 =
            make_iter (buf1, buf1, buf1_end, it1_dummy);

        const InputIterator1 last1 =
            make_iter (buf1_end, buf1_end, buf1_end, it1_dummy);

        const InputIterator2 first2 =
            make_iter (buf2, buf2, buf2_end, it2_dummy);

        bool res;

        if (predicate) {
            res = std::equal (first1, last1, first2, std::equal_to<X>());
        }
        else {
            res = std::equal (first1, last1, first2);
        }

        // verify 25.1.8, p1
        int success = res && i < mid_inx || !res && mid_inx <= i;

        const bool equal_expected = i < mid_inx;

        rw_assert (success, 0, __LINE__,
                   "%zu. equal(%s, %2$s, %s%{?}, %s%{;}) == %d, got %d",
                   i + 1, it1name, it2name,
                   0 != predicate, predicate,
                   equal_expected, res);

        // verify 25.1.8, p2
        success = X::n_total_op_eq_ - last_n_op_eq <= (N + 1);
        rw_assert (success, 0, __LINE__, 
                   "%zu. equal(%s, %2$s, %s%s%{?}, %s%{;}) complexity: "
                   "%zu <= %zu",
                   i + 1, it1name, it2name,
                   0 != predicate, predicate,
                   X::n_total_op_eq_, i + 1);

        if (!success)
            break;
    }

    ::operator delete (buf1);
    ::operator delete (buf2);
}

/**************************************************************************/

/* extern */ int rw_opt_no_input_iter;   // --no-InputIterator
/* extern */ int rw_opt_no_fwd_iter;     // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;   // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;     // --no-RandomAccessIterator
/* extern */ int rw_opt_no_predicate;    // --no-Predicate


template <class InputIterator, class T>
void test_equal (const InputIterator &dummy, T*, const char *predicate)
{
    static const std::size_t N = 1024;

    if (rw_opt_no_input_iter) {
        rw_note (0, __FILE__, __LINE__, "InputIterator test disabled");
    }
    else {
        test_equal (N, dummy, InputIter<X>((X*)0, (X*)0, (X*)0), (X*)0,
                    predicate);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_equal (N, dummy, FwdIter<X>(), (X*)0, predicate);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_equal (N, dummy, BidirIter<X>(), (X*)0, predicate);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_equal (N, dummy, RandomAccessIter<X>(), (X*)0, predicate);
    }
}


/**************************************************************************/

static void
test_equal (const char *predicate)
{
    rw_info (0, 0, 0,
             "template <class %s, class %s> "
             "bool std::equal (%1$s, %1$s, %2$s%{?}, %s%{;})",
             "InputIterator1", "InputIterator2",
             0 != predicate, predicate);

    if (rw_opt_no_input_iter) {
        rw_note (0, __FILE__, __LINE__, "InputIterator test disabled");
    }
    else {
        test_equal (InputIter<X>((X*)0, (X*)0, (X*)0), (X*)0, predicate);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_equal (FwdIter<X>(), (X*)0, predicate);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_equal (BidirIter<X>(), (X*)0, predicate);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_equal (RandomAccessIter<X>(), (X*)0, predicate);
    }

}

/**************************************************************************/

static int
run_test (int, char*[])
{
    test_equal (0);

    if (rw_opt_no_predicate) {
        rw_note (0, __FILE__, __LINE__, "Predicate test disabled");
    }
    else {
        test_equal ("std::equal_to<X>");
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.equal",
                    0 /* no comment */, run_test,
                    "|-no-ForwardIterator#"
                    "|-no-BidirectionalIterator#"
                    "|-no-RandomAccessIterator#"
                    "|-no-Predicate",
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter,
                    &rw_opt_no_predicate);
}
