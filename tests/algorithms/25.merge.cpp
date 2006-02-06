/***************************************************************************
 *
 * 25.merge.cpp - test exercising 25.3.4 [lib.alg.merge]
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

#include <algorithm>    // for merge, 
#include <cstring>      // for strlen, size_t

#include <alg_test.h>
#include <driver.h>     // for rw_test()

/**************************************************************************/

_RWSTD_NAMESPACE (std) { 

// disable explicit instantiation for compilers (like MSVC)
// that can't handle it
#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
OutputIter<lt_comp<assign<base<cpy_ctor> > > >
merge (InputIter<lt_comp<assign<base<cpy_ctor> > > >, 
       InputIter<lt_comp<assign<base<cpy_ctor> > > >,
       InputIter<lt_comp<assign<base<cpy_ctor> > > >,
       InputIter<lt_comp<assign<base<cpy_ctor> > > >,
       OutputIter<lt_comp<assign<base<cpy_ctor> > > >);

template
OutputIter<lt_comp<assign<base<cpy_ctor> > > >
merge (InputIter<lt_comp<assign<base<cpy_ctor> > > >, 
       InputIter<lt_comp<assign<base<cpy_ctor> > > >,
       InputIter<lt_comp<assign<base<cpy_ctor> > > >,
       InputIter<lt_comp<assign<base<cpy_ctor> > > >,
       OutputIter<lt_comp<assign<base<cpy_ctor> > > >,
       binary_predicate<lt_comp<assign<base<cpy_ctor> > > >);

template
void
inplace_merge (BidirIter<lt_comp<assign<base<cpy_ctor> > > >, 
               BidirIter<lt_comp<assign<base<cpy_ctor> > > >,
               BidirIter<lt_comp<assign<base<cpy_ctor> > > >);

template
void
inplace_merge (BidirIter<lt_comp<assign<base<cpy_ctor> > > >, 
               BidirIter<lt_comp<assign<base<cpy_ctor> > > >,
               BidirIter<lt_comp<assign<base<cpy_ctor> > > >,
               binary_predicate<lt_comp<assign<base<cpy_ctor> > > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std

/**************************************************************************/

template <class T>
struct Less
{
    static std::size_t funcalls_;

    // dummy arguments provided to prevent the class from being
    // default constructible and implicit conversion from int
    Less (int /* dummy */, int /* dummy */) {
        funcalls_ = 0;
    }

    // return a type other than bool but one that is implicitly
    // convertible to bool to detect incorrect assumptions
    conv_to_bool operator() (const T &x, const T &y) /* non-const */ {
        ++funcalls_;
        return conv_to_bool::make (x.val_ < y.val_);
    }

    static const char* name () { return "Less"; }

private:
    void operator= (StrictWeakLess&);   // not assignable
};

template<class T> std::size_t Less<T>::funcalls_;

/**************************************************************************/

template <class T, class InputIter1, class InputIter2, 
          class OutputIterator, class BidirectIterator, class Predicate>
void test_merge (int                     line,
                 const char             *src1,
                 const char             *src2,
                 const std::size_t       midinx,
                 const InputIter1       &it1,
                 const InputIter2       &it2,
                 const OutputIterator   &out,
                 const BidirectIterator &bit,
                 const T*,
                 const Predicate        *ppred,
                 bool                    inplace)
{
    const char* const it1name =
        inplace ? type_name (bit, (T*)0) : type_name (it1, (T*)0);
    const char* const it2name = type_name (it2, (T*)0);
    const char* const outname = type_name (out, (T*)0);
    const char* const fname   = inplace ? "inplace_merge" : "merge";
    const char* const funname = Predicate::name();

    const std::size_t nsrc1 = std::strlen (src1);
    const std::size_t nsrc2 = std::strlen (src2);

    T* const xsrc1 = T::from_char (src1, nsrc1);
    T* const xsrc2 = T::from_char (src2, nsrc2);

    const std::size_t ndst = nsrc1 + nsrc2;
    T* const xdst = inplace ? xsrc1 : new T [ndst];

    T* const xsrc1_end = xsrc1 + nsrc1;
    T* const xsrc2_end = xsrc2 + nsrc2;
    T* const xsrc_mid  = xsrc1 + midinx;
    T* const xdst_end  = xdst + ndst;

    const InputIter1 first1 = make_iter (xsrc1,     xsrc1, xsrc1_end, it1);
    const InputIter1 last1  = make_iter (xsrc1_end, xsrc1, xsrc1_end, it1);
    const InputIter2 first2 = make_iter (xsrc2,     xsrc2, xsrc2_end, it2);
    const InputIter2 last2  = make_iter (xsrc2_end, xsrc2, xsrc2_end, it2);

    const OutputIterator res_first = make_iter (xdst, xdst, xdst_end, out);

    const BidirectIterator first = make_iter (xsrc1,     xsrc1, xsrc1_end, bit);
    const BidirectIterator mid   = make_iter (xsrc_mid,  xsrc1, xsrc1_end, bit);
    const BidirectIterator last  = make_iter (xsrc1_end, xsrc1, xsrc1_end, bit);

    const Predicate pred (0, 0);

    const std::size_t last_n_op_lt = T::n_total_op_lt_;
    OutputIterator result (0, 0, 0);

    if (inplace) {  // inplace_merge
        if (ppred)
            std::inplace_merge (first, mid, last, pred);
        else
            std::inplace_merge (first, mid, last);
    }
    else {          // merge
        result = ppred ?
              std::merge (first1, last1, first2, last2, res_first, pred)
            : std::merge (first1, last1, first2, last2, res_first);
    }

    const std::size_t n_ops_lt = ppred ? 
        Predicate::funcalls_ : T::n_total_op_lt_ - last_n_op_lt;

    bool success = true;

    // check output iterator for merge
    if (!inplace) {
        success = result.cur_ == xdst_end;
        rw_assert (success, 0, line,
                   "line %d: %s<%s, %s, %s%{?}, %s%{;}> (\"%s\", \"%s\", ...)"
                   " return : expected first + %zu, got first + %td",
                   __LINE__, fname, it1name, it2name, outname, ppred, funname,
                   src1, src2, ndst, result.cur_ - res_first.cur_);
    }

    // check that the sequence is sorted
    success = is_sorted_lt (xdst, xdst_end);
    rw_assert (success, 0, line,
               "line %d: %s<%s%{?}, %s, %s%{;}%{?}, %s%{;}> "
               "(\"%s\", %{?}\"%s\"%{;}%{?}%zu%{;}, ...) ==> \"%{X=*.*}\" "
               "not sorted",
               __LINE__, fname, it1name, !inplace, it2name, outname, 
               ppred, funname, src1, !inplace, src2, inplace, midinx,
               int (ndst), -1, xdst);

    // check that the algorithm is stable
    std::size_t i = 1;
    for ( ; i < ndst; i++) {
        if (xdst [i - 1].val_ == xdst [i].val_) {
            success = xdst [i - 1].origin_ < xdst [i].origin_;
            if (!success)
                break;
        }
    }

    // to avoid errors in --trace mode
    i = i < ndst ? i : ndst - 1;

    rw_assert (success, 0, line,
               "line %d: %s<%s%{?}, %s, %s%{;}%{?}, %s%{;}> "
               "(\"%s\", %{?}\"%s\"%{;}%{?}%zu%{;}, ...) ==> \"%{X=*.*}\" "
               "not stable at %zu: got ids %d !< %d for values %#c == %#c",
               __LINE__, fname, it1name, !inplace, it2name, outname, 
               ppred, funname, src1, !inplace, src2, inplace, midinx,
               int (ndst), i, xdst, i, xdst [i - 1].origin_, 
               xdst [i].origin_, xdst [i - 1].val_, xdst [i].val_);

    // check the complexity
    success = n_ops_lt <= ndst - 1;
    rw_assert (success, 0, line,
               "line %d: %s<%s%{?}, %s, %s%{;}%{?}, %s%{;}> "
               "(\"%s\", %{?}\"%s\"%{;}%{?}%zu%{;}, ...) ==> \"%{X=*.*}\" "
               "complexity: got %zu, expected no more than %zu",
               __LINE__, fname, it1name, !inplace, it2name, outname, 
               ppred, funname, src1, !inplace, src2, inplace, midinx,
               int (ndst), -1, xdst, n_ops_lt, ndst - 1);

    delete[] xsrc1;
    delete[] xsrc2;

    if (!inplace)
        delete[] xdst;
}

/**************************************************************************/

template <class T, class InputIter1, class InputIter2, 
          class OutputIterator, class Predicate>
void test_merge (const InputIter1     &it1,
                 const InputIter2     &it2,
                 const OutputIterator &out,
                 const T*,
                 const Predicate      *ppred)
{
    const char* const it1name = type_name (it1, (T*)0);
    const char* const it2name = type_name (it2, (T*)0);
    const char* const outname = type_name (out, (T*)0);
    const char* const fname   = "merge";
    const char* const funname = Predicate::name();

    static const BidirIter<T> bidir_iter (0, 0, 0);

    rw_info (0, 0, 0,
             "%s std::%s(%s, %3$s, %s, %4$s, %1$s%{?}, %s%{;})",
             outname, fname, it1name, it2name, ppred, funname);

#define TEST(src1, src2)                                            \
    test_merge (__LINE__, src1, src2, 0, it1, it2, out,             \
                bidir_iter, (T*)0, ppred, false)     

    TEST ("ab", "");
    TEST ("", "ab");

    TEST ("a", "b");
    TEST ("b", "a");
    TEST ("aa", "bb");
    TEST ("bb", "aa");

    TEST ("acegi", "bdfhj");
    TEST ("aaegi", "bdfjj");
    TEST ("accgi", "bdhhj");
    TEST ("aceei", "bffhj");
    TEST ("acegg", "ddfhj");
    TEST ("aceii", "bbfhj");

    TEST ("bdfhj", "acegi");
    TEST ("bdfjj", "aaegi");
    TEST ("bdhhj", "accgi");
    TEST ("bffhj", "aceei");
    TEST ("ddfhj", "acegg");
    TEST ("bbfhj", "aceii");

    TEST ("abcde", "abcde");
    TEST ("aacde", "aacde");
    TEST ("abbde", "abbde");
    TEST ("abcce", "abcce");
    TEST ("abcdd", "abcdd");
    TEST ("abcee", "abcee");

    TEST ("aaegi", "bdfjj");
    TEST ("accgi", "bdhhj");
    TEST ("aceei", "bffhj");
    TEST ("acegg", "ddfhj");
    TEST ("aceii", "bbfhj");

    TEST ("a","abcdefghij");
    TEST ("ac","abcdefghi");
    TEST ("ace","abcdefgh");
    TEST ("aceg","abcdefg");

    TEST ("j","abcdefghjj");
    TEST ("gj","abcdeggjj");
    TEST ("egj","abeeggjj");

    TEST ("abcdefghij","a");
    TEST ("abcdefghi","ac");
    TEST ("abcdefgh","ace");
    TEST ("abcdefg","aceg");

    TEST ("abcdefghjj","j");
    TEST ("abcdeggjj","gj");
    TEST ("abeeggjj","egj");

    TEST ("aaaaa", "aaaaa");
    TEST ("bbbbb", "bbbbb");
}

/**************************************************************************/

template <class T, class BidirectIterator, class Predicate>
void test_inplace_merge (const BidirectIterator &it,
                         const T*, 
                         const Predicate        *ppred)
{
    const char* const itname  = type_name (it, (T*)0);
    const char* const fname   = "inplace_merge";
    const char* const funname = Predicate::name();

    rw_info (0, 0, 0,
             "std::%s(%s, %2$s, %2$s%{?}, %s%{;})",
             fname, itname, ppred, funname);

#undef TEST
#define TEST(src, mid)                                              \
    test_merge (__LINE__, src, "", mid, it, it, it,                 \
                it, (T*)0, ppred, true)     

    TEST ("a",  0);
    TEST ("aa", 0);
    TEST ("a",  1);
    TEST ("aa", 2);

    TEST ("aa", 1);
    TEST ("ab", 1);
    TEST ("ba", 1);

    TEST ("abcdefghij", 5);
    TEST ("aacdeffhij", 5);
    TEST ("abbdefggij", 5);
    TEST ("abccefghhj", 5);
    TEST ("abcddfghii", 5);
    TEST ("abceefghjj", 5);

    TEST ("abcdeabcde", 5);
    TEST ("aacdeaacde", 5);
    TEST ("abbdeabbde", 5);
    TEST ("abcceabcce", 5);
    TEST ("abcddabcdd", 5);
    TEST ("abceeabcee", 5);

    TEST ("aabcdefghi", 1);
    TEST ("aaacdefghi", 1);
    TEST ("babcdefghi", 1);
    TEST ("babbdefghi", 1);

    TEST ("abcdefghia", 9);
    TEST ("aacdefghia", 9);
    TEST ("abcdefghib", 9);
    TEST ("abbdefghib", 9);
}

/**************************************************************************/

/* extern */ int rw_opt_no_merge;               // --no-merge
/* extern */ int rw_opt_no_inplace_merge;       // --no-inplace_merge
/* extern */ int rw_opt_no_predicate;           // --no-predicate
/* extern */ int rw_opt_no_input_iter;          // --no-InputIterator
/* extern */ int rw_opt_no_output_iter;         // --no-OutputIterator
/* extern */ int rw_opt_no_fwd_iter;            // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;          // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;            // --no-RandomAccessIterator

/**************************************************************************/

template <class T, class InputIterator1, class InputIterator2, 
          class OutputIterator, class Predicate>
void test_merge (const InputIterator1   &it1,
                 const InputIterator2   &it2,
                 const OutputIterator   &out,
                 int                     tag1,
                 int                     tag2,
                 int                     tag3,
                 const T*,
                 const Predicate        *ppred)
{
    static const InputIter<T>        input_iter (0, 0, 0);
    static const OutputIter<T>       output_iter (0, 0, 0);
    static const FwdIter<T>          fwd_iter (0, 0, 0);
    static const BidirIter<T>        bidir_iter (0, 0, 0);
    static const RandomAccessIter<T> rand_iter (0, 0, 0);

    if (tag1 && tag2 && tag3) {
        rw_info (0, 0, 0,
                 "template <class %s, class %s, class %s%{?}, class %s%{;}> "
                 "%3$s std::merge(%1$s, %1$s, %2$s, %2$s, %3$s%{?}, %s%{;})",
                 "InputIterator1", "InputIterator2", "OutputIterator",
                 ppred, "Compare", ppred, "Compare");
    }

    // tag1, tag2 and tag3 indicates that an iterator needs to be generated
    // at the corresponding position by a recursive call to test_merge
    // for all tags:
    // 0 means that no iterator is needed here
    //         (maybe it was already generated or just not needed)
    // 1 means that the iterator is needed at this position
    //         (first for tag1, second for tag2, etc)

    ////////////////////////////////////////////////////////////////////////
    if (rw_opt_no_input_iter) {
        if (tag1 || tag2) 
            rw_note (0, __FILE__, __LINE__, "InputIterator test disabled");
    }
    else {
        if (tag1) { 
            test_merge (input_iter, it2, out, 0, tag2, tag3, (T*)0, ppred);
        }
        else if (tag1 == 0 && tag2) {
            test_merge (it1, input_iter, out, tag1, 0, tag3, (T*)0, ppred);
        }
    }

    ////////////////////////////////////////////////////////////////////////
    if (rw_opt_no_fwd_iter) {
        if (tag1 || tag2 || tag3)
            rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        if (tag1) { 
            test_merge (fwd_iter, it2, out, 0, tag2, tag3, (T*)0, ppred);
        }
        else if (tag1 == 0 && tag2) {
            test_merge (it1, fwd_iter, out, tag1, 0, tag3, (T*)0, ppred);
        }
        else if (tag1 == 0 && tag2 == 0 && tag3) {
            test_merge (it1, it2, fwd_iter, tag1, tag2, 0, (T*)0, ppred);
        }
    }

    ////////////////////////////////////////////////////////////////////////
    if (rw_opt_no_bidir_iter) {
        if (tag1 || tag2 || tag3)
            rw_note (0, __FILE__, __LINE__, 
                     "BidirectionalIterator test disabled");
    }
    else {
        if (tag1) { 
            test_merge (bidir_iter, it2, out, 0, tag2, tag3, (T*)0, ppred);
        }
        else if (tag1 == 0 && tag2) {
            test_merge (it1, bidir_iter, out, tag1, 0, tag3, (T*)0, ppred);
        }
        else if (tag1 == 0 && tag2 == 0 && tag3) {
            test_merge (it1, it2, bidir_iter, tag1, tag2, 0, (T*)0, ppred);
        }
    }

    ////////////////////////////////////////////////////////////////////////
    if (rw_opt_no_rnd_iter) {
        if (tag1 || tag2 || tag3)
            rw_note (0, __FILE__, __LINE__, 
                     "RandomAccessIterator test disabled");
    }
    else {
        if (tag1) { 
            test_merge (rand_iter, it2, out, 0, tag2, tag3, (T*)0, ppred);
        }
        else if (tag1 == 0 && tag2) {
            test_merge (it1, rand_iter, out, tag1, 0, tag3, (T*)0, ppred);
        }
        else if (tag1 == 0 && tag2 == 0 && tag3) {
            test_merge (it1, it2, rand_iter, tag1, tag2, 0, (T*)0, ppred);
        }
    }

    ////////////////////////////////////////////////////////////////////////
    if (rw_opt_no_output_iter) {
        if (!tag1 && !tag2 && tag3)
            rw_note (0, __FILE__, __LINE__, 
                     "OutputIterator test disabled");
    }
    else {
        if (tag1 == 0 && tag2 == 0 && tag3) {
            test_merge (it1, it2, output_iter, tag1, tag2, 0, (T*)0, ppred);
        }
    }

    if (0 == tag1 && 0 == tag2 && 0 == tag3)
        test_merge (it1, it2, out, (T*)0, ppred);
}

/**************************************************************************/

template <class T, class Predicate>
void test_inplace_merge (const T*, 
                         const Predicate *ppred)
{
    static const BidirIter<T>        bidir_iter (0, 0, 0);
    static const RandomAccessIter<T> rand_iter (0, 0, 0);

    rw_info (0, 0, 0,
             "template <class %s%{?}, class %s%{;}> "
             "std::inplace_merge (%1$s, %1$s, %1$s%{?}, %s%{;})",
             "BidirectionalIterator", ppred, "Compare", ppred, "Compare");

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_inplace_merge (bidir_iter, (T*)0, ppred);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_inplace_merge (rand_iter, (T*)0, ppred);
    }
}

/**************************************************************************/

static int run_test (int, char*[])
{
    const Less<X> less_pred(0, 0);

    if (rw_opt_no_merge) {
        rw_note (0, __FILE__, __LINE__, 
                 "std::merge test disabled");
    }
    else {
        static const FwdIter<X> fwd_iter (0, 0, 0);

        test_merge (fwd_iter, fwd_iter, fwd_iter, 1, 1, 1, 
                   (X*)0, (Less<X>*)0);

        if (rw_opt_no_predicate) {
          rw_note (0, __FILE__, __LINE__, 
                   "std::merge predicate test disabled");
        }
        else {
            test_merge (fwd_iter, fwd_iter, fwd_iter, 1, 1, 1, 
                       (X*)0, &less_pred);
        }
    }

    if (rw_opt_no_inplace_merge) {
        rw_note (0, __FILE__, __LINE__, 
                 "std::inplace_merge test disabled");
    }
    else {
        test_inplace_merge ((X*)0, (Less<X>*)0);

        if (rw_opt_no_predicate) {
          rw_note (0, __FILE__, __LINE__, 
                   "std::inplace_merge predicate test disabled");
        }
        else {
            test_inplace_merge ((X*)0, &less_pred);
        }
    }

    return 0;
}


/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.merge",
                    0 /* no comment */, run_test,
                    "|-no-merge# "
                    "|-no-inplace_merge# "
                    "|-no-predicate#"
                    "|-no-InputIterator# "
                    "|-no-OutputIterator# "
                    "|-no-ForwardIterator# "
                    "|-no-BidirectionalIterator# "
                    "|-no-RandomAccessIterator#",
                    &rw_opt_no_merge,
                    &rw_opt_no_inplace_merge,
                    &rw_opt_no_predicate,
                    &rw_opt_no_input_iter,
                    &rw_opt_no_output_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter);
}
