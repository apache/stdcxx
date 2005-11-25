/***************************************************************************
 *
 * alg_test.h - common definitions for algorithms tests
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

#ifndef _RWSTD_ALG_TEST_H_INCLUDED
#define _RWSTD_ALG_TEST_H_INCLUDED

#include <iterator>

#include <cassert>   // for assert()

#include <testdefs.h>
#include <rw/_defs.h>


// objects of class X maintain a count of their instances in existence,
// the number of defaut and copy ctor calls, assignment operators, and
// the number of calls to operator==() and operator<()
struct _TEST_EXPORT X
{
    const int id_;        // a unique non-zero id of the object
    int       origin_;    // id of the original object that this
                          // is a (perhaps indirect) copy of (id_
                          // when this is the original)
    int       src_id_;    // id of the object that this is a direct
                          // copy of (id_ when this the original)
    int       val_;       // object's value

    // number of times the object has been copied into another object,
    // regardless of whether the operation threw an exception or not
    _RWSTD_SIZE_T n_copy_ctor_;

    // number of times the object's assignment operator has been invoked,
    // regardless of whether the operation threw an exception or not
    _RWSTD_SIZE_T n_op_assign_;

    // number of times the object's operator== was invoked
    // regardless of whether the operation threw an exception
    _RWSTD_SIZE_T n_op_eq_;

    // number of times the object's operator< was invoked
    // regardless of whether the operation threw an exception
    _RWSTD_SIZE_T n_op_lt_;

    static _RWSTD_SIZE_T count_;   // number of objects in existence (>= 0)
    static int id_gen_;            // generates a unique non-zero id
    static int (*gen_)();          // extern "C++" int (*)()

    static _RWSTD_SIZE_T n_total_def_ctor_;    // number of default ctor calls
    static _RWSTD_SIZE_T n_total_copy_ctor_;   // ... copy ctors ...
    static _RWSTD_SIZE_T n_total_dtor_;        // ... dtors ...
    static _RWSTD_SIZE_T n_total_op_assign_;   // ... assignment operators ...
    static _RWSTD_SIZE_T n_total_op_eq_;       // ... equality operators ...
    static _RWSTD_SIZE_T n_total_op_lt_;       // ... operators <= ...

    // classes thrown from the respective functions
    struct Exception { int id_; };
    struct DefCtor: Exception { };
    struct CopyCtor: Exception { };
    struct Dtor: Exception { };
    struct OpAssign: Exception { };
    struct OpEq: Exception { };
    struct OpLt: Exception { };

    // throw object's `id' wrapped in the appropriate struct when the
    // corresponding n_total_xxx_ counter reaches the value pointed to
    // by the respective pointer below
    static _RWSTD_SIZE_T* def_ctor_throw_ptr_;
    static _RWSTD_SIZE_T* copy_ctor_throw_ptr_;
    static _RWSTD_SIZE_T* dtor_throw_ptr_;
    static _RWSTD_SIZE_T* op_assign_throw_ptr_;
    static _RWSTD_SIZE_T* op_eq_throw_ptr_;
    static _RWSTD_SIZE_T* op_lt_throw_ptr_;

    // objects to which the pointers above initally point
    static _RWSTD_SIZE_T def_ctor_throw_count_;
    static _RWSTD_SIZE_T copy_ctor_throw_count_;
    static _RWSTD_SIZE_T dtor_throw_count_;
    static _RWSTD_SIZE_T op_assign_throw_count_;
    static _RWSTD_SIZE_T op_eq_throw_count_;
    static _RWSTD_SIZE_T op_lt_throw_count_;

    X ();

    X (const X&);

    ~X ();

    X& operator= (const X&);

    bool operator== (const X&) const;
    bool operator< (const X&) const;

    // the following operators are not declared or defined in order
    // to detect any unwarranted assumptions made in algorithms
    //    bool operator!= (const X &rhs) const;
    //    bool operator> (const X &rhs) const;
    //    bool operator>= (const X &rhs) const;
    //    bool operator<= (const X &rhs) const;
    //    X operator- () const;
    //    X operator+ () const;
    
    bool
    is_count (_RWSTD_SIZE_T copy_ctor,
              _RWSTD_SIZE_T op_assign,
              _RWSTD_SIZE_T op_eq,
              _RWSTD_SIZE_T op_lt) const;

    static bool
    is_total (_RWSTD_SIZE_T count,
              _RWSTD_SIZE_T n_def_ctor,
              _RWSTD_SIZE_T n_copy_ctor,
              _RWSTD_SIZE_T n_op_assign,
              _RWSTD_SIZE_T n_op_eq,
              _RWSTD_SIZE_T n_op_lt);

    static void reset_totals ();

    // construct an array of objects of type X each initialized
    // from the corresponding element of the character array
    static X* from_char (const char*, _RWSTD_SIZE_T = ~0UL);

    // returns -1 when less, 0 when same, or +1 when the array
    // of X objects is greater than the character string
    static int compare (const X*, const char*, _RWSTD_SIZE_T = ~0UL);
    static int compare (const char*, const X*, _RWSTD_SIZE_T = ~0UL);

    // returns -1 when less, 0 when same, or +1 when the first
    // array of X objects is greater than the second array
    static int compare (const X*, const X*, _RWSTD_SIZE_T);
};


struct _TEST_EXPORT UnaryPredicate
{
    // total number of times operator() was invoked
    static _RWSTD_SIZE_T n_total_op_fcall_;

    UnaryPredicate ();

    UnaryPredicate (const UnaryPredicate&);

    UnaryPredicate& operator= (const UnaryPredicate&);

    virtual ~UnaryPredicate ();

    virtual bool operator()(const X&) const;
};


struct _TEST_EXPORT BinaryPredicate
{
    // total number of times operator() was invoked
    static _RWSTD_SIZE_T n_total_op_fcall_;

    bool ignore_case_;

    BinaryPredicate (bool = false);

    BinaryPredicate (const BinaryPredicate&);

    BinaryPredicate& operator= (const BinaryPredicate&);

    virtual ~BinaryPredicate ();

    virtual bool operator()(const X&, const X&) const;
};


class _TEST_EXPORT tempstr;

// converts a sequence of objects of type X to a tempstr object
// in the format "[%p0, %p1): { x0, >x1<, ..., xN - 1 } where N
// is defined as: N = (X*)p1 - (X*)p0
// the last argument, if non-negative, indicates the index of the
// element enclosed in between the '>' and '<' characters
_TEST_EXPORT void to_string (tempstr*, const X*, const X*, int = -1);


// generate a unique sequential number starting from 0
_TEST_EXPORT int gen_seq ();

// generate numbers in the sequence 0, 0, 1, 1, 2, 2, 3, 3, etc... 
_TEST_EXPORT int gen_seq_2lists ();

// generate a sequence of subsequences (i.e., 0, 1, 2, 3, 4, 0, 1, 2, etc...)
_TEST_EXPORT int gen_subseq ();

// wrapper around a (possibly) extern "C" int rand()
// extern "C++" 
_TEST_EXPORT  int gen_rnd ();


// computes an integral log2
inline unsigned ilog2 (unsigned long n)
{
    unsigned result = 0;
    while (n >>= 1)
        ++result;
    return result;
}


// computes an integral log10
inline unsigned ilog10 (unsigned long n)
{
    unsigned result = 0;
    while (n /= 10)
        ++result;
    return result;
}


// returns true iff a sequence of (not necessarily unique) values
// is sorted in an ascending order
template <class InputIterator>
inline bool is_sorted_lt (InputIterator first, InputIterator last)
{
    if (first == last)
        return true;

    for (InputIterator prev (first); ++first != last; prev = first) {
        if (*first < *prev)
            return false;
    }

    return true;
}


// returns true iff a sequence of (not necessarily unique) values
// is sorted in a descending order
template <class InputIterator>
inline bool is_sorted_gt (InputIterator first, InputIterator last)
{
    if (first == last)
        return true;

    for (InputIterator prev (first); ++first != last; prev = first) {
        if (*prev < *first)
            return false;
    }

    return true;
}


// type used to exercise that algorithms do not apply operators
// to function objects the latter are not required to define
struct conv_to_bool {

    static conv_to_bool make (bool val) {
        conv_to_bool tmp;
        tmp.val_ = val;
        return tmp;
    }

    operator bool () const {
        return val_;
    }

private:
    void operator!() const;                  // not defined

    bool val_;
};

// not defined
void operator&& (const conv_to_bool&, bool);
void operator&& (bool, const conv_to_bool&);
void operator|| (const conv_to_bool&, bool);
void operator|| (bool, const conv_to_bool&);

// element-type prototypes to exercise container requirements


// meets requirements listed at 25, p7
template <class T>
struct predicate {
    conv_to_bool operator() (const T &a) const {
        _RWSTD_UNUSED (a);
        return conv_to_bool::make (true);
    }
};


// meets requirements listed at 25, p8
template <class T>
struct binary_predicate {
    conv_to_bool operator() (const T &a, const T &b) const {
        _RWSTD_UNUSED (a);
        _RWSTD_UNUSED (b);
        return conv_to_bool::make (true);
    }
};


// meets requirements listed at 25.2.3, p2
template <class T>
struct func {
    typedef T              argument_type;
    typedef argument_type& reference;

    reference operator() (const argument_type&) const {
        return _RWSTD_REINTERPRET_CAST (reference,
                   _RWSTD_CONST_CAST (func*, this)->dummy);
    }

private:
    char dummy;
};


// meets requirements listed at 25.2.3, p2
template <class T>
struct binary_func {
    typedef T              argument_type;
    typedef argument_type& reference;

    reference operator() (const argument_type&, 
                          const argument_type&) const {
        return _RWSTD_REINTERPRET_CAST (reference,
                   _RWSTD_CONST_CAST (binary_func*, this)->dummy);
    }

private:
    char dummy;
};


// a base-class to extend the requirements classes from

enum { no_ctor = 0, def_ctor = 1, cpy_ctor = 2 };

template <int c = no_ctor>
struct base;


template<>
struct base<no_ctor>
{
private:
    // struct s added to prevent gcc warning: base<no_ctor> has a private
    // constructor and no friends
    struct s { };
    friend struct s;

    base ();
    base (const base&);
    void operator= (base&);
};


template<>
struct base<def_ctor>
{
    base () : unused (0) { }

private:

    void operator= (base&);
    base (const base&);

    // unused member prevents bogus HP aCC warnings (see Onyx #23561)
    int unused;
};


template<>
struct base<cpy_ctor>
{
    // explicitly specifying redundant template parameters to work
    // around a SunPro 5.2 bug (see Onyx #24260)
    base (const base<cpy_ctor> &rhs): unused (rhs.unused) { }

private:

    base ();
    void operator= (base&);

    // unused member prevents bogus HP aCC warnings (see Onyx #23561)
    int unused;
};


template<>
struct base<(def_ctor | cpy_ctor)>
{
    base (): unused (0) { }

    // explicitly specifying redundant template parameters to work
    // around a SunPro 5.2 bug (see Onyx #24260)
    base (const base<(def_ctor | cpy_ctor)> &rhs): unused (rhs.unused) { }

private:

    void operator= (base&);

    // unused member prevents bogus HP aCC warnings (see Onyx #23561)
    int unused;
};


template <class T>
struct eq_comp: T { };


template <class T>
inline bool operator== (const eq_comp<T>&, const eq_comp<T>&)
{
    return true;
}


template <class T>
struct lt_comp: T { };


template <class T>
inline bool operator< (const lt_comp<T>&, const lt_comp<T>&)
{
    return true;
}


// assignment

template <class T>
struct assign : T
{
    assign& operator= (const assign& rhs) {
        unused = rhs.unused;
        return *this;
    }
private:
    // unused member prevents bogus HP aCC warnings (see Onyx #23561)
    int unused;
};


// conversion structs

// struct split into 2 to eliminate the following g++ 2.95.2 warning:
// warning: choosing `convert<T>::operator U&()' over 
//                   `convert<T>::operator const U&() const'

template <class T, class U>
struct cvt : T 
{
    operator U& () {
        return _RWSTD_REINTERPRET_CAST (U&, *this);
    }
};


template <class T, class U>
struct const_cvt : T
{
    operator const U& () const {
        return _RWSTD_REINTERPRET_CAST (const U&, *this);
    }
};


#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC

struct DummyBase { };

#  define ITER_BASE(ign1, ign2, ign3, ign4, ign5) DummyBase
#else   // if defined (_RWSTD_NO_CLASS_PARTIAL_SPEC)
   // when partial specialization isn't supported 
#  define ITER_BASE(Cat, T, Dist, Ptr, Ref) \
          std::iterator<Cat, T, Dist, Ptr, Ref >
#endif   // _RWSTD_NO_CLASS_PARTIAL_SPEC


// satisfies the requirements in 24.1.1 [lib.input.iterators]
template <class T>
struct InputIter: ITER_BASE (std::input_iterator_tag, T, int, T*, T&)
{
    typedef T                       value_type;
    typedef value_type*             pointer;
    typedef value_type&             reference;
    typedef int                     difference_type;
    typedef std::input_iterator_tag iterator_category;

    // body shared by all copies of the same InputIter specialization
    // to detect algorithms that pass through the same interator more
    // than once (disallowed by 24.1.1, p3)
    struct Shared {
        const value_type *cur_;
        const value_type *beg_;
        const value_type *end_;
        int               ref_;

        Shared (const value_type *cur,
                const value_type *beg,
                const value_type *end)
            : cur_ (cur), beg_ (beg), end_ (end), ref_ (1) { }

        ~Shared () {
            cur_ = beg_ = end_ = 0;
            ref_ = -1;
        }

    private:
        Shared (const Shared&);           // not defined
        void operator= (const Shared&);   // not defined

    };

    // InputIterators are not default constructible
    InputIter (const value_type *cur,
               const value_type *beg,
               const value_type *end)
        : ptr_ (new Shared (cur, beg, end)), cur_ (cur) { }

    InputIter (const InputIter &rhs)
        : ptr_ (rhs.ptr_), cur_ (rhs.cur_) {
        assert (0 != ptr_);
        ++ptr_->ref_;
    }

    ~InputIter () {
        assert (0 != ptr_);

        if (0 == --ptr_->ref_)   // decrement the reference count
            delete ptr_;
        ptr_ = 0;
        cur_ = 0;
    }

    InputIter& operator= (const InputIter &rhs) {
        assert (rhs == rhs);   // assert `rhs' is valid

        assert (0 != ptr_);
        if (0 == --ptr_->ref_)
            delete ptr_;

        ptr_ = rhs.ptr_;

        assert (0 != ptr_);
        ++ptr_->ref_;

        cur_ = rhs.cur_;

        return *this;
    }

    bool operator== (const InputIter &rhs) const {
        // assert that both arguments are in the domain of operator==()
        // i.e., that no copy of *this or `rhs' has been incremented
        // and that no copy passed through this value of the iterator

        assert (0 != ptr_);
        assert (cur_ == ptr_->cur_);

        assert (0 != rhs.ptr_);
        assert (rhs.cur_ == rhs.ptr_->cur_);

        return cur_ == rhs.cur_;
    }

    bool operator!= (const InputIter &rhs) const {
        return !(*this == rhs);
    }

    // returning const-reference rather than a value in order
    // not to impose the CopyConstructible requirement on T
    // and to disallow constructs like *InputIter<T>() = T()
    const value_type& operator* () const {
        assert (*this == *this);      // assert *this is valid
        assert (cur_ < ptr_->end_);   // assert *this is dereferenceable
        return *cur_;
    }

    _RWSTD_OPERATOR_ARROW (const value_type* operator-> () const);

    InputIter& operator++ () {
        assert (*this == *this);      // assert *this is valid
        assert (cur_ < ptr_->end_);   // assert *this is not past the end

        ptr_->cur_ = ++cur_;

        return *this;
    }

    InputIter operator++ (int) {
        return ++*this;
    }

// private:
    Shared           *ptr_;
    const value_type *cur_;   // past-the-end
};


// satisfies the requirements in 24.1.2 [lib.output.iterators]
template <class T>
struct OutputIter: ITER_BASE (std::output_iterator_tag, T, int, T*, T&)
{
    typedef T                        value_type;
    typedef value_type*              pointer;
    typedef value_type&              reference;
    typedef int                      difference_type;
    typedef std::output_iterator_tag iterator_category;

    // body shared by all copies of the same OutputIter specialization
    // to detect algorithms that pass through the same interator more
    // than once (disallowed by 24.1.2, p2)
    struct Shared {
        pointer           cur_;
        pointer           assign_;
        const value_type *begin_;
        const value_type *end_;
        int               ref_;

        Shared (pointer cur, const value_type *end)
            : cur_ (cur), assign_ (cur), begin_ (cur), end_ (end), ref_ (1) { }

        ~Shared () {
            begin_ = end_ = cur_ = assign_ = 0;
            ref_ = -1;
        }

    private:
        Shared (const Shared&);           // not defined
        void operator= (const Shared&);   // not defined

    };

    // class whose objects are returned from OutputIter::operator*
    // to detect multiple assignments (disallowed by 24.1.2, p2)
    class Proxy {
        friend struct OutputIter;

        Shared* const ptr_;

        Proxy (Shared *ptr): ptr_ (ptr) { }

    public:
        void operator= (const value_type &rhs) {
            assert (0 != ptr_);

            // verify that the iterator is in the valid range
            assert (ptr_->cur_ >= ptr_->begin_ && ptr_->cur_ <= ptr_->end_);

            // verify that the assignment point is the same as the current
            // position `cur' within the sequence or immediately before it
            // (in order to allow the expression: *it++ = val)
            assert (   ptr_->assign_ == ptr_->cur_
                    || ptr_->assign_ + 1 == ptr_->cur_);

            // assign and increment the assignment point
            *ptr_->assign_++ = rhs;
        }
    };

    // OutputIterators are not default constructible
    OutputIter (pointer           cur,
                const value_type *,
                const value_type *end)
        : ptr_ (new Shared (cur, end)), cur_ (cur) { }

    OutputIter (const OutputIter &rhs)
        : ptr_ (rhs.ptr_), cur_ (rhs.cur_) {
        ++ptr_->ref_;   // increment the reference count
    }

    ~OutputIter () {
        if (0 == --ptr_->ref_)   // decrement the reference count
            delete ptr_;
        ptr_ = 0;
        cur_ = 0;
    }

    OutputIter& operator= (const OutputIter &rhs) {
        if (0 == --ptr_->ref_)
            delete ptr_;

        ptr_ = rhs.ptr_;
        ++ptr_->ref_;

        cur_ = rhs.cur_;

        return *this;
    }

    void operator= (const value_type &rhs) const {
        **this = rhs;
    }

    // return a proxy in order to detect multiple assignments
    // through the iterator (disallowed by 24.1.2, p2))
    Proxy operator* () const {
        assert (0 != ptr_);
        assert (ptr_->assign_ && ptr_->assign_ != ptr_->end_);

        return Proxy (ptr_);
    }

    _RWSTD_OPERATOR_ARROW (pointer operator-> () const);

    OutputIter& operator++ () {
        assert (cur_ == ptr_->cur_);
        assert (ptr_->cur_ >= ptr_->begin_ && ptr_->cur_ < ptr_->end_);
        cur_ = ++ptr_->cur_;
        return *this;
    }

    // returning a const value rather than a modifiable value
    // in order to verify the requirement in row 5 of Table 73
    const OutputIter operator++ (int) {
        OutputIter tmp (*this);
        return ++*this, tmp;
    }

// private:
    Shared  *ptr_;
    pointer  cur_;
};


// satisfies the requirements in 24.1.3 [lib.forward.iterators]
template <class T>
struct FwdIter: ITER_BASE (std::forward_iterator_tag, T, int, T*, T&)
{
    typedef T                         value_type;
    typedef value_type*               pointer;
    typedef value_type&               reference;
    typedef int                       difference_type;
    typedef std::forward_iterator_tag iterator_category;

    FwdIter (): cur_ (0), end_ (0) { }

    FwdIter (pointer           cur,
             const value_type *,
             const value_type *end)
        : cur_ (cur), end_ (end) { }

    FwdIter (const FwdIter &rhs)
        : cur_ (rhs.cur_), end_ (rhs.end_) { }

    ~FwdIter () {
        end_ = cur_ = 0;
    }

    FwdIter& operator= (const FwdIter &rhs) {
        cur_ = rhs.cur_;
        end_ = rhs.end_;
        return *this;
    }

    bool operator== (const FwdIter &rhs) const {
        assert (cur_ != 0);
        return cur_ == rhs.cur_;
    }

    bool operator!= (const FwdIter &rhs) const {
        return !(*this == rhs);
    }

    reference operator* () const {
        assert (cur_ != 0 && cur_ != end_);
        return *cur_;
    }

    _RWSTD_OPERATOR_ARROW (pointer operator-> () const);

    FwdIter& operator++ () {
        assert (cur_ != 0 && cur_ != end_);
        return ++cur_, *this;
    }

    FwdIter operator++ (int) {
        FwdIter tmp (*this);
        return ++*this, tmp;
    }

// private:
    pointer           cur_;   // pointer to current element
    const value_type *end_;   // past-the-end
};


template <class T>
struct ConstFwdIter: FwdIter<T>
{
    typedef T                   value_type;
    typedef FwdIter<value_type> Base;

    ConstFwdIter (): Base () { }

    ConstFwdIter (const value_type *cur,
                  const value_type *begin,
                  const value_type *end)
        : Base (_RWSTD_CONST_CAST (value_type*, cur), begin, end) { }

    const value_type& operator* () const {
        return Base::operator* ();
    }

    _RWSTD_OPERATOR_ARROW (const value_type* operator-> () const);
};


// satisfies the requirements in 24.1.4 [lib.bidirectional.iterators]
template <class T>
struct BidirIter: ITER_BASE (std::bidirectional_iterator_tag, T, int, T*, T&)
{
    typedef T                               value_type;
    typedef value_type*                     pointer;
    typedef value_type&                     reference;
    typedef int                             difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;

    BidirIter (): cur_ (0), begin_ (0), end_ (0) { }

    BidirIter (pointer           cur,
               const value_type *begin,
               const value_type *end)
        : cur_ (cur), begin_ (begin), end_ (end) { }

    BidirIter (const BidirIter &rhs)
        : cur_ (rhs.cur_), begin_ (rhs.begin_), end_ (rhs.end_) { }

    ~BidirIter () {
        begin_ = end_ = cur_ = 0;
    }

    BidirIter& operator= (const BidirIter &rhs) { 
        cur_ = rhs.cur_;
        end_ = rhs.end_;
        return *this; 
    }

    bool operator== (const BidirIter &rhs) const {
        assert (cur_ != 0 && rhs.cur_ != 0);
        return cur_ == rhs.cur_;
    }

    bool operator!= (const BidirIter &rhs) const {
        return !(*this == rhs);
    }

    reference operator* () const {
        assert (cur_ != 0 && cur_ != end_);
        return *cur_;
    }

    _RWSTD_OPERATOR_ARROW (pointer operator-> () const);

    BidirIter& operator++ () {
        assert (cur_ != 0 && cur_ != end_);
        return ++cur_, *this;
    }

    BidirIter operator++ (int) {
        BidirIter tmp (*this);
        return ++*this, tmp;
    }

    BidirIter& operator-- () {
        assert (cur_ != 0 && cur_ != begin_);
        return --cur_, *this;
    }

    BidirIter operator-- (int) {
        BidirIter tmp (*this);
        return --*this, tmp;
    }

// private:
    pointer           cur_;     // pointer to current element
    const value_type *begin_;   // first in range
    const value_type *end_;     // past-the-end
};


template <class T>
struct ConstBidirIter: BidirIter<T>
{
    typedef T                     value_type;
    typedef BidirIter<value_type> Base;

    ConstBidirIter (): Base () { }

    ConstBidirIter (const value_type *cur,
                    const value_type *begin,
                    const value_type *end)
        : Base (_RWSTD_CONST_CAST (value_type*, cur), begin, end) { }

    const value_type& operator* () const {
        return Base::operator* ();
    }

    _RWSTD_OPERATOR_ARROW (const value_type* operator-> () const);
};


// satisfies the requirements in 24.1.5 [lib.random.access.iterators]
template <class T>
struct RandomAccessIter
    : ITER_BASE (std::random_access_iterator_tag, T, int, T*, T&)
{
    typedef T                               value_type;
    typedef value_type*                     pointer;
    typedef value_type&                     reference;
    typedef int                             difference_type;
    typedef std::random_access_iterator_tag iterator_category;

    RandomAccessIter (): cur_ (0), begin_ (0), end_ (0) { }

    RandomAccessIter (pointer           cur,
                      const value_type *begin,
                      const value_type *end)
        : cur_ (cur), begin_ (begin), end_ (end) { }

    RandomAccessIter (const RandomAccessIter &rhs)
        : cur_ (rhs.cur_), begin_ (rhs.begin_), end_ (rhs.end_) { }

    ~RandomAccessIter () {
        begin_ = end_ = cur_ = 0;
    }

    RandomAccessIter& operator= (const RandomAccessIter &rhs) {
        cur_   = rhs.cur_;
        begin_ = rhs.begin_;
        end_   = rhs.end_;
        return *this; 
    }

    reference operator* () const {
        assert (cur_ != 0 && cur_ != end_);
        return *cur_;
    }

    _RWSTD_OPERATOR_ARROW (pointer operator-> () const);

    RandomAccessIter& operator++ () {
        assert (cur_ != 0 && cur_ != end_);
        return ++cur_, *this;
    }

    RandomAccessIter operator++ (int) {
        RandomAccessIter tmp (*this);
        return ++*this, tmp;
    }

    RandomAccessIter& operator-- () {
        assert (cur_ != 0 && cur_ != begin_);
        return --cur_, *this;
    }

    RandomAccessIter operator-- (int) {
        RandomAccessIter tmp (*this);
        return --*this, tmp;
    }

    RandomAccessIter& operator+= (difference_type n) {
        assert (   cur_ != 0
                && (!end_ || cur_ + n <= end_)
                && (!begin_ || cur_ + n >= begin_));
        return cur_ += n, *this;
    }
    RandomAccessIter& operator-= (difference_type n) {
        return *this += -n;
    }

    RandomAccessIter operator+ (difference_type n) const {
        return RandomAccessIter (*this) += n;
    }

    RandomAccessIter operator- (difference_type n) const {
        return RandomAccessIter (*this) -= n;
    }

    difference_type operator- (const RandomAccessIter &rhs) const { 
        assert (cur_ != 0 && rhs.cur_ != 0);
        return cur_ - rhs.cur_;
    }

    bool operator== (const RandomAccessIter &rhs) const {
        assert (cur_ != 0 && rhs.cur_ != 0);
        return cur_ == rhs.cur_;
    }

    bool operator!= (const RandomAccessIter &rhs) const {
        return !(*this == rhs);
    }

    bool operator< (const RandomAccessIter &rhs) const {
        assert (cur_ != 0 && rhs.cur_ != 0);
        return cur_ < rhs.cur_;
    };

    bool operator> (const RandomAccessIter &rhs) const {
        return rhs < *this;
    }

    bool operator<= (const RandomAccessIter &rhs) const {
        return !(rhs < *this);
    }

    bool operator>= (const RandomAccessIter &rhs) const {
        return !(*this < rhs);
    }

    reference operator[] (difference_type inx) const { 
        assert (   cur_ != 0
                && (!end_ || cur_ + inx < end_)
                && !(begin_ || cur_ + inx >= begin_));
        return cur_ [inx];
    }

// private:
    pointer           cur_;     // pointer to current element
    const value_type *begin_;   // first in range
    const value_type *end_;     // past-the-end
};


template <class T>
struct ConstRandomAccessIter: RandomAccessIter<T>
{
    typedef T                              value_type;
    typedef RandomAccessIter<value_type>   Base;
    typedef typename Base::difference_type difference_type;

    ConstRandomAccessIter (): Base () { }

    ConstRandomAccessIter (const value_type *cur,
                           const value_type *begin,
                           const value_type *end)
        : Base (_RWSTD_CONST_CAST (value_type*, cur), begin, end) { }

    const value_type& operator* () const {
        return Base::operator* ();
    }

    _RWSTD_OPERATOR_ARROW (const value_type* operator-> () const);

    const value_type& operator[] (difference_type inx) const {
        return Base::operator[] (inx);
    }
};


template <class T>
inline T*
make_iter (T *cur, const T*, const T*, T*)
{
    return cur;
}

template <class T>
inline T*
copy_iter (T *ptr, const T*)
{
    return ptr;
}

// dummy function argument provided to help broken compilers (PR #29835)

template <class T>
inline InputIter<T>
make_iter (const T *cur, const T *begin, const T *end, const InputIter<T>&)
{
    return InputIter<T>(cur, begin, end);
}

template <class T>
inline InputIter<T>
copy_iter (const InputIter<T> &it, const T*)
{
    return InputIter<T>(it.cur_, it.ptr_->beg_, it.ptr_->end_);
}

template <class T>
inline const char* type_name (InputIter<T>, const T*)
{ return "InputIterator"; }


template <class T>
inline OutputIter<T>
make_iter (T *cur, const T *begin, const T *end, const OutputIter<T>&)
{
    return OutputIter<T>(cur, begin, end);
}

template <class T>
inline OutputIter<T>
copy_iter (const OutputIter<T> &it, const T*)
{
    return OutputIter<T>(it.cur_, 0, it.ptr_->end);
}

template <class T>
inline const char* type_name (OutputIter<T>, const T*)
{ return "OutputIterator"; }


template <class T>
inline FwdIter<T>
make_iter (T *cur, const T *begin, const T *end, FwdIter<T>)
{
    return FwdIter<T>(cur, begin, end);
}

template <class T>
inline FwdIter<T>
copy_iter (const FwdIter<T> &it, const T*)
{
    return FwdIter<T>(it.cur_, 0, it.end_);
}

template <class T>
inline const char* type_name (FwdIter<T>, const T*)
{ return "ForwardIterator"; }


template <class T>
inline ConstFwdIter<T>
make_iter (T *cur, const T *begin, const T *end, ConstFwdIter<T>)
{
    return ConstFwdIter<T>(cur, begin, end);
}

template <class T>
inline ConstFwdIter<T>
copy_iter (const ConstFwdIter<T> &it, const T*)
{
    return ConstFwdIter<T>(it.cur_, 0, it.end_);
}

template <class T>
inline const char* type_name (ConstFwdIter<T>, const T*)
{ return "ConstForwardIterator"; }


template <class T>
inline BidirIter<T>
make_iter (T *cur, const T *begin, const T *end, BidirIter<T>)
{
    return BidirIter<T>(cur, begin, end);
}

template <class T>
inline BidirIter<T>
copy_iter (const BidirIter<T> &it, const T*)
{
    return BidirIter<T>(it.cur_, it.begin_, it.end_);
}

template <class T>
inline const char* type_name (BidirIter<T>, const T*)
{ return "BidirectionalIterator"; }


template <class T>
inline ConstBidirIter<T>
make_iter (T *cur, const T *begin, const T *end, ConstBidirIter<T>)
{
    return ConstBidirIter<T>(cur, begin, end);
}

template <class T>
inline ConstBidirIter<T>
copy_iter (const ConstBidirIter<T> &it, const T*)
{
    return ConstBidirIter<T>(it.cur_, it.begin_, it.end_);
}

template <class T>
inline const char* type_name (ConstBidirIter<T>, const T*)
{ return "ConstBidirectionalIterator"; }


template <class T>
inline RandomAccessIter<T>
make_iter (T *cur, const T *begin, const T *end, RandomAccessIter<T>)
{
    return RandomAccessIter<T>(cur, begin, end);
}

template <class T>
inline RandomAccessIter<T>
copy_iter (const RandomAccessIter<T> &it, const T*)
{
    return RandomAccessIter<T>(it.cur_, it.begin_, it.end_);
}

template <class T>
inline const char* type_name (RandomAccessIter<T>, const T*)
{ return "RandomAccessIterator"; }


template <class T>
inline ConstRandomAccessIter<T>
make_iter (T *cur, const T *begin, const T *end, ConstRandomAccessIter<T>)
{
    return ConstRandomAccessIter<T>(cur, begin, end);
}

template <class T>
inline ConstRandomAccessIter<T>
copy_iter (const ConstRandomAccessIter<T> &it, const T*)
{
    return ConstRandomAccessIter<T>(it.cur_, it.begin_, it.end_);
}

template <class T>
inline const char* type_name (ConstRandomAccessIter<T>, const T*)
{ return "ConstRandomAccessIterator"; }



#endif   // _RWSTD_ALG_TEST_H_INCLUDED
