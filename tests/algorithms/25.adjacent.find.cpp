/***************************************************************************
 *
 * adjacent_find.cpp - test exercising 25.1.5 [lib.alg.adjacent.find]
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

#include <algorithm>   // for adjacent_find
#include <cstddef>     // for size_t

#include <alg_test.h>
#include <driver.h>

/**************************************************************************/

_RWSTD_NAMESPACE (std) {

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
FwdIter<eq_comp<base<> > >
adjacent_find (FwdIter<eq_comp<base<> > >, FwdIter<eq_comp<base<> > >);

template
FwdIter<eq_comp<base<> > >
adjacent_find (FwdIter<eq_comp<base<> > >, FwdIter<eq_comp<base<> > >, 
               binary_predicate<eq_comp<base<> > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std


/**************************************************************************/

typedef unsigned char UChar;

// used to initialize an array of objects of type X
static const char *xinit_begin;

int xinit ()
{
    static const char *cur = 0;

    if (!cur || !*cur)
        cur = xinit_begin;

    return UChar (*cur++);
}


// exercises std::adjacent_find()
template <class Iterator, class T>
void do_test (int line, const char *src, 
              std::size_t resoff,
              Iterator, const T*)
{
    static const char* const itname = type_name (Iterator (), (T*)0);

    const std::size_t nsrc = std::strlen (src);

    if (std::size_t (-1) == resoff)
        resoff = nsrc;

    xinit_begin = src;
    X::gen_ = xinit;

    X* const xsrc = new X [nsrc];

    const Iterator first =
        make_iter (xsrc, xsrc, xsrc + nsrc, Iterator ());

    const Iterator last =
        make_iter (xsrc + nsrc, xsrc, xsrc + nsrc, Iterator ());

    // compute the number of invocations of the predicate
    std::size_t n_total_pred = X::n_total_op_eq_;

    const Iterator res = std::adjacent_find (first, last);

    // silence a bogus EDG eccp remark #550-D:
    // variable "res" was set but never used
    _RWSTD_UNUSED (res);

    n_total_pred = X::n_total_op_eq_ - n_total_pred;

    // verify that the returned iterator is set as expected
    int success = res.cur_ == first.cur_ + resoff;
    rw_assert (success, 0, line, 
               "line %d: adjacent_find<%s>(it = \"%s\", ...)"
               " == (it + %zu), got (it + %td)",
               __LINE__, itname, src,
               resoff, res.cur_ - first.cur_);

    // verify the number of applications of the predicate (lwg issue 240):
    // Complexity: For a nonempty range, exactly
    //             min((i - first) + 1, (last - first) - 1)
    // applications of the corresponding predicate, where i is
    // adjacent_find's return value.

    // compute the expected number of invocations of the predicate
    std::size_t n_expect_pred = 0;

    if (nsrc) {
        // test iterators are guaranteed to be in range
        _RWSTD_ASSERT (first.cur_ <= res.cur_ && res.cur_ <= last.cur_);

        n_expect_pred = std::size_t (res.cur_ - first.cur_) + 1;
        const std::size_t tmp = std::size_t (last.cur_ - first.cur_) - 1;
        if (tmp < n_expect_pred)
            n_expect_pred = tmp;
    }

    success = std::size_t (n_expect_pred) == n_total_pred;
    rw_assert (success, 0, line, 
               "line %d: adjacent_find<%s>(\"%s\", ...) "
               "invoked operator==() %zu times, expected %td",
               __LINE__, itname, src,
               n_total_pred, n_expect_pred);
}

/**************************************************************************/

template <class Iterator, class T>
void run_tests (Iterator it, const T*)
{
    static const char* const itname = type_name (Iterator (), (T*)0);

    rw_info (0, 0, 0, "std::adjacent_find (%s, %s)", itname, itname);
    
#define TEST(src, off) \
    do_test (__LINE__, src, std::size_t (off), it, (X*)0)

    TEST ("",            -1);
    TEST ("a",           -1);
    TEST ("ab",          -1);
    TEST ("abc",         -1);
    TEST ("abcd",        -1);
    TEST ("abcde",       -1);
    TEST ("abcdef",      -1);
    TEST ("abcdefg",     -1);
    TEST ("abcdefgh",    -1);
    TEST ("abcdefghi",   -1);
    TEST ("abcdefghij",  -1);
    TEST ("abcdefghijk", -1);

    TEST ("aabcdefghij",  0);
    TEST ("abbcdefghij",  1);
    TEST ("abccdefghij",  2);
    TEST ("abcddefghij",  3);
    TEST ("abcdeefghij",  4);
    TEST ("abcdeffghij",  5);
    TEST ("abcdefgghij",  6);
    TEST ("abcdefghhij",  7);
    TEST ("abcdefghiij",  8);
    TEST ("abcdefghijj",  9);

    TEST ("aaabcdefghi",  0);
    TEST ("abbbcdefghi",  1);
    TEST ("abcccdefghi",  2);
    TEST ("abcdddefghi",  3);
    TEST ("abcdeeefghi",  4);
    TEST ("abcdefffghi",  5);
    TEST ("abcdefggghi",  6);
    TEST ("abcdefghhhi",  7);
    TEST ("abcdefghiii",  8);
    TEST ("abcdefghijjj", 9);

}

/**************************************************************************/

static int rw_opt_no_fwd_iter;     // --no-ForwardIterator
static int rw_opt_no_bidir_iter;   // --no-BidirectionalIterator
static int rw_opt_no_rnd_iter;     // --no-RandomAccessIterator

static int
run_test (int, char*[])
{
    rw_info (0, 0, 0, 
             "template <class ForwardIterator> "
             "std::adjacent_find (ForwardIterator, ForwardIterator)");

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        run_tests (FwdIter<X>(), (X*)0);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        run_tests (BidirIter<X>(), (X*)0);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        run_tests (RandomAccessIter<X>(), (X*)0);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.adjacent.find",
                    0 /* no comment */, run_test,
                    "|-no-ForwardIterator#"
                    "|-no-BidirectionalIterator#"
                    "|-no-RandomAccessIterator#",
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
