/************************************************************************
 *
 * 21.strings.h - definitions of helpers used in clause 21 tests
 *
 * $Id$
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

#ifndef RW_21_STRINGS_H_INCLUDED
#define RW_21_STRINGS_H_INCLUDED

#include <testdefs.h>

/**************************************************************************/

struct StringIds {

    // identifiers for the charT template argument
    enum CharId { Char, WChar, UChar };

    // identifiers for the Traits template argument
    enum TraitsId { DefaultTraits, UserTraits };

    // identifiers for the Allocator template argument
    enum AllocId { DefaultAlloc, UserAlloc };

    // identifies a set of overloaded member or non-member
    // string functions
    enum FuncId {
        // 6 bits, 64 functions max
        /*  0 */ fid_append,
        /*  1 */ fid_assign,
        /*  2 */ fid_erase,
        /*  3 */ fid_insert,
        /*  4 */ fid_replace,
        /*  5 */ fid_op_plus_eq,
        /*  6 */ fid_find,
        /*  7 */ fid_rfind,
        /*  8 */ fid_find_first_of,
        /*  9 */ fid_find_last_of,
        /* 10 */ fid_find_first_not_of,
        /* 11 */ fid_find_last_not_of,
        /* 12 */ fid_compare,
        /* 13 */ fid_substr,
        /* 14 */ fid_op_index,
        /*    */ fid_op_index_const = fid_op_index,
        /* 15 */ fid_at,
        /*    */ fid_at_const = fid_at,
        /* 16 */ fid_copy,
        /* 17 */ fid_ctor,
        /* 18 */ fid_op_set,
        /* 19 */ fid_swap,
        /* 20 */ fid_push_back,
        /* 21 */ fid_op_plus,
        /* 22 */ fid_op_equal,
        /* 23 */ fid_op_not_equal,
        /* 24 */ fid_op_less,
        /* 25 */ fid_op_less_equal,
        /* 26 */ fid_op_greater,
        /* 27 */ fid_op_greater_equal,
        /* -- */ fid_bits = 6,
        /* -- */ fid_mask = 63
    };

    // identifies the type of a function argument, including
    // the implicit this
    enum ArgId {
        // 4 bits, 16 types max
        /*  0 */ arg_void,    // void
        /*  1 */ arg_size,    // size_type
        /*  2 */ arg_val,     // value_type
        /*  3 */ arg_ptr,     // pointer
        /*  4 */ arg_cptr,    // const_pointer
        /*  5 */ arg_ref,     // reference
        /*  6 */ arg_cref,    // const_reference
        /*  7 */ arg_iter,    // iterator
        /*  8 */ arg_citer,   // const_iterator
        /*  9 */ arg_range,   // Iterator, Iterator
        /* 10 */ arg_str,     // string& (or this for member functions)
        /* 11 */ arg_cstr,    // const string& (or const this for members)
        /* 12 */ arg_alloc,   // const allocator&
        /* -- */ arg_bits = 4,
        /* -- */ arg_mask = 15
    };

    enum {
        // bit designating a member function
        bit_member = 1 << fid_bits + 6 * arg_bits
    };

// FCAT() concatenates prefix, underscrore, and suffix
#define _FCAT(a, b)             a ## b
#define FCAT(a, b)              _FCAT (a ## _, b)

// FID_N() constructs the name for an overload of a string function
#define FID_0(f)                FCAT (f, void)
#define FID_1(f, a)             FCAT (f, a)
#define FID_2(f, a, b)          FID_1 (FCAT (f, a), b)
#define FID_3(f, a, b, c)       FID_2 (FCAT (f, a), b, c)
#define FID_4(f, a, b, c, d)    FID_3 (FCAT (f, a), b, c, d)
#define FID_5(f, a, b, c, d, e) FID_4 (FCAT (f, a), b, c, d, e)

// ARG() creates a bitmap of an argument type at the given position
#define ARG(a, N)   ((arg_ ## a << (N * arg_bits)) << fid_bits)

// SIG_N() creates an argument bitmap for the given function signature
#define SIG_0(f)                   fid_ ## f
#define SIG_1(f, a)                SIG_0 (f) | ARG (a, 0)
#define SIG_2(f, a, b)             SIG_1 (f, a) | ARG (b, 1)
#define SIG_3(f, a, b, c)          SIG_2 (f, a, b) | ARG (c, 2)
#define SIG_4(f, a, b, c, d)       SIG_3 (f, a, b, c) | ARG (d, 3)
#define SIG_5(f, a, b, c, d, e)    SIG_4 (f, a, b, c, d) | ARG (e, 4)
#define SIG_6(f, a, b, c, d, e, g) SIG_5 (f, a, b, c, d, e) | ARG (g, 5)

// convenience macro to define member function overload id's
// where the first argument encodes the constness of the member
// function (or the lack thereof)
#define MEMBER_0(f, self) \
        FID_0 (f) = SIG_1 (f, self) | bit_member
#define MEMBER_1(f, self, a) \
        FID_1 (f, a) = SIG_2 (f, self, a) | bit_member
#define MEMBER_2(f, self, a, b) \
        FID_2 (f, a, b) = SIG_3 (f, self, a, b) | bit_member
#define MEMBER_3(f, self, a, b, c) \
        FID_3 (f, a, b, c) = SIG_4 (f, self, a, b, c) | bit_member
#define MEMBER_4(f, self, a, b, c, d) \
        FID_4 (f, a, b, c, d) = SIG_5 (f, self, a, b, c, d) | bit_member
#define MEMBER_5(f, self, a, b, c, d, e) \
        FID_5 (f, a, b, c, d, e) = SIG_6 (f, self, a, b, c, d, e) | bit_member

// convenience macro to define non-member function overload id's
#define NON_MEMBER_0(f) \
        FID_0 (f) = SIG_0 (f)
#define NON_MEMBER_1(f, a) \
        FID_1 (f, a) = SIG_1 (f, a)
#define NON_MEMBER_2(f, a, b) \
        FID_2 (f, a, b) = SIG_2 (f, a, b)
#define NON_MEMBER_3(f, a, b, c) \
        FID_3 (f, a, b, c) = SIG_3 (f, a, b, c)
#define NON_MEMBER_4(f, a, b, c, d) \
        FID_4 (f, a, b, c, d) = SIG_4 (f, a, b, c, d)
#define NON_MEMBER_5(f, a, b, c, d, e) \
        FID_5 (f, a, b, c, d, e) = SIG_5 (f, a, b, c, d, e)

    // unique identifiers for all overloads of each member function
    //     6 bits for FuncId
    // 6 * 4 bits for ArgId (at most 6 arguments including this)
    //     1 bit for membership
    enum OverloadId {
        //////////////////////////////////////////////////////////////
        // append (const_pointer)
        MEMBER_1 (append, str, cptr),
        // append (const basic_string&)
        MEMBER_1 (append, str, cstr),
        // append (const_pointer, size_type)
        MEMBER_2 (append, str, cptr, size),
        // append (const basic_string&, size_type, size_type)
        MEMBER_3 (append, str, cstr, size, size),
        // append (size_type, value_type)
        MEMBER_2 (append, str, size, val),
        // append (InputIterator, InputIterator)
        MEMBER_1 (append, str, range),

        //////////////////////////////////////////////////////////////
        // assign (const_pointer)
        MEMBER_1 (assign, str, cptr),
        // assign (const basic_string&)
        MEMBER_1 (assign, str, cstr),
        // assign (const_pointer, size_type)
        MEMBER_2 (assign, str, cptr, size),
        // assign (const basic_string&, size_type, size_type)
        MEMBER_3 (assign, str, cstr, size, size),
        // assign (size_type, value_type)
        MEMBER_2 (assign, str, size, val),
        // assign (InputIterator, InputIterator)
        MEMBER_1 (assign, str, range),

        //////////////////////////////////////////////////////////////
        // erase ()
        MEMBER_0 (erase, str),
        // erase (size_type)
        MEMBER_1 (erase, str, size),
        // erase (size_type, size_type)
        MEMBER_2 (erase, str, size, size),
        // erase (iterator)
        MEMBER_1 (erase, str, iter),
        // erase (iterator, iterator)
        MEMBER_2 (erase, str, iter, iter),

        //////////////////////////////////////////////////////////////
        // insert (size_type, const_pointer)
        MEMBER_2 (insert, str, size, cptr),
        // insert (size_type, const basic_string&)
        MEMBER_2 (insert, str, size, cstr),
        // insert (size_type, const_pointer, size_type)
        MEMBER_3 (insert, str, size, cptr, size),
        // insert (size_type, const basic_string&, size_type, size_type)
        MEMBER_4 (insert, str, size, cstr, size, size),
        // insert (size_type, size_type, value_type)
        MEMBER_3 (insert, str, size, size, val),
        // insert (iterator, value_type)
        MEMBER_2 (insert, str, iter, val),
        // insert (iterator, size_type, value_type)
        MEMBER_3 (insert, str, iter, size, val),
        // insert (iterator, InputIterator, InputIterator)
        MEMBER_2 (insert, str, iter, range),

        //////////////////////////////////////////////////////////////
        // (size_type, size_type, const_pointer)
        MEMBER_3 (replace, str, size, size, cptr),
        // (size_type, size_type, const basic_string&)
        MEMBER_3 (replace, str, size, size, cstr),
        // (size_type, size_type, const_pointer, size_type)
        MEMBER_4 (replace, str, size, size, cptr, size),
        // (size_type, size_type, const basic_string&, size_type, size_type)
        MEMBER_5 (replace, str, size, size, cstr, size, size),
        // (size_type, size_type, size_type, value_type)
        MEMBER_4 (replace, str, size, size, size, val),
        // (iterator, iterator, const_pointer)
        MEMBER_3 (replace, str, iter, iter, cptr),
        // (iterator, iterator, const basic_string&)
        MEMBER_3 (replace, str, iter, iter, cstr),
        // (iterator, iterator, const_pointer, size_type)
        MEMBER_4 (replace, str, iter, iter, cptr, size),
        // (iterator, iterator, size_type, value_type)
        MEMBER_4 (replace, str, iter, iter, size, val),
        // (iterator, iterator, InputIterator, InputIterator)
        MEMBER_3 (replace, str, iter, iter, range),

        //////////////////////////////////////////////////////////////
        // operator+= (const_pointer)
        MEMBER_1 (op_plus_eq, str, cptr), 
        // operator+= (const basic_string&)
        MEMBER_1 (op_plus_eq, str, cstr),
        // operator+= (value_type)
        MEMBER_1 (op_plus_eq, str, val),

        //////////////////////////////////////////////////////////////
        // overloads of find, rfind, find_first_of, find_last_of, 
        // find_first_not_of, find_last_not_of

        // find (const_pointer) const
        MEMBER_1 (find, cstr, cptr),
        // find (const basic_string&) const
        MEMBER_1 (find, cstr, cstr),
        // find (const_pointer, size_type) const
        MEMBER_2 (find, cstr, cptr, size),
        // find (const_pointer, size_type, size_type) const
        MEMBER_3 (find, cstr, cptr, size, size),
        // find (const basic_string&, size_type) const
        MEMBER_2 (find, cstr, cstr, size),
        // find (value_type) const
        MEMBER_1 (find, cstr, val),
        // find (value_type, size_type) const
        MEMBER_2 (find, cstr, val, size),

        //////////////////////////////////////////////////////////////
        // rfind (const_pointer) const
        MEMBER_1 (rfind, cstr, cptr),
        // rfind (const basic_string&) const
        MEMBER_1 (rfind, cstr, cstr),
        // rfind (const_pointer, size_type) const
        MEMBER_2 (rfind, cstr, cptr, size),
        // rfind (const_pointer, size_type, size_type) const
        MEMBER_3 (rfind, cstr, cptr, size, size),
        // rfind (const basic_string&, size_type) const
        MEMBER_2 (rfind, cstr, cstr, size),
        // rfind (value_type) const
        MEMBER_1 (rfind, cstr, val),
        // rfind (value_type, size_type) const
        MEMBER_2 (rfind, cstr, val, size),

        //////////////////////////////////////////////////////////////
        // find_first_of (const_pointer) const
        MEMBER_1 (find_first_of, cstr, cptr),
        // find_first_of (const basic_string&) const
        MEMBER_1 (find_first_of, cstr, cstr),
        // find_first_of (const_pointer, size_type) const
        MEMBER_2 (find_first_of, cstr, cptr, size),
        // find_first_of (const_pointer, size_type, size_type) const
        MEMBER_3 (find_first_of, cstr, cptr, size, size),
        // find_first_of (const basic_string&, size_type) const
        MEMBER_2 (find_first_of, cstr, cstr, size),
        // find_first_of (value_type) const
        MEMBER_1 (find_first_of, cstr, val),
        // find_first_of (value_type, size_type) const
        MEMBER_2 (find_first_of, cstr, val, size),

        //////////////////////////////////////////////////////////////
        // find_last_of (const_pointer) const
        MEMBER_1 (find_last_of, cstr, cptr),
        // find_last_of (const basic_string&) const
        MEMBER_1 (find_last_of, cstr, cstr),
        // find_last_of (const_pointer, size_type) const
        MEMBER_2 (find_last_of, cstr, cptr, size),
        // find_last_of (const_pointer, size_type, size_type) const
        MEMBER_3 (find_last_of, cstr, cptr, size, size),
        // find_last_of (const basic_string&, size_type) const
        MEMBER_2 (find_last_of, cstr, cstr, size),
        // find_last_of (value_type) const
        MEMBER_1 (find_last_of, cstr, val),
        // find_last_of (value_type, size_type) const
        MEMBER_2 (find_last_of, cstr, val, size),

        //////////////////////////////////////////////////////////////
        // find_first_not_of (const_pointer) const
        MEMBER_1 (find_first_not_of, cstr, cptr),
        // find_first_not_of (const basic_string&) const
        MEMBER_1 (find_first_not_of, cstr, cstr),
        // find_first_not_of (const_pointer, size_type) const
        MEMBER_2 (find_first_not_of, cstr, cptr, size),
        // find_first_not_of (const_pointer, size_type, size_type) const
        MEMBER_3 (find_first_not_of, cstr, cptr, size, size),
        // find_first_not_of (const basic_string&, size_type) const
        MEMBER_2 (find_first_not_of, cstr, cstr, size),
        // find_first_not_of (value_type) const
        MEMBER_1 (find_first_not_of, cstr, val),
        // find_first_not_of (value_type, size_type) const
        MEMBER_2 (find_first_not_of, cstr, val, size),

        //////////////////////////////////////////////////////////////
        // find_last_not_of (const_pointer) const
        MEMBER_1 (find_last_not_of, cstr, cptr),
        // find_last_not_of (const basic_string&) const
        MEMBER_1 (find_last_not_of, cstr, cstr),
        // find_last_not_of (const_pointer, size_type) const
        MEMBER_2 (find_last_not_of, cstr, cptr, size),
        // find_last_not_of (const_pointer, size_type, size_type) const
        MEMBER_3 (find_last_not_of, cstr, cptr, size, size),
        // find_last_not_of (const basic_string&, size_type) const
        MEMBER_2 (find_last_not_of, cstr, cstr, size),
        // find_last_not_of (value_type) const
        MEMBER_1 (find_last_not_of, cstr, val),
        // find_last_not_of (value_type, size_type) const
        MEMBER_2 (find_last_not_of, cstr, val, size),

        //////////////////////////////////////////////////////////////
        // compare (const_pointer) const
        MEMBER_1 (compare, cstr, cptr),
        // compare (const basic_string&) const
        MEMBER_1 (compare, cstr, cstr),
        // compare (size_type, size_type, const_pointer) const
        MEMBER_3 (compare, cstr, size, size, cptr),
        // compare (size_type, size_type, const basic_string&) const
        MEMBER_3 (compare, cstr, size, size, cstr),
        // compare (size_type, size_type, const_pointer, size_type) const
        MEMBER_4 (compare, cstr, size, size, cptr, size),
        // compare (size_type, size_type, const basic_string&,
        //          size_type, size_type) const
        MEMBER_5 (compare, cstr, size, size, cstr, size, size),

        //////////////////////////////////////////////////////////////
        // substr (void) const
        MEMBER_1 (substr, cstr, void),
        // substr (size_type) const
        MEMBER_1 (substr, cstr, size),
        // substr (size_type, size_type) const
        MEMBER_2 (substr, cstr, size, size),

        //////////////////////////////////////////////////////////////
        // operator[] (size_type)
        MEMBER_1 (op_index, str, size),
        // operator[] (size_type) const
        MEMBER_1 (op_index_const, cstr, size),
        // at (size_type)
        MEMBER_1 (at, str, size),
        // at (size_type) const
        MEMBER_1 (at_const, cstr, size),

        //////////////////////////////////////////////////////////////
        // copy (pointer, size_type) const
        MEMBER_2 (copy, cstr, ptr, size),
        // copy (pointer, size_type, size_type) const
        MEMBER_3 (copy, cstr, ptr, size, size),

        //////////////////////////////////////////////////////////////
        // basic_string (void)
        MEMBER_0 (ctor, str),
        // basic_string (const allocator_type&)
        MEMBER_1 (ctor, str, alloc),
        // basic_string (const_pointer)
        MEMBER_1 (ctor, str, cptr),
        // basic_string (const_pointer, const allocator_type&)
        MEMBER_2 (ctor, str, cptr, alloc),
        // basic_string (const basic_string&)
        MEMBER_1 (ctor, str, cstr),
        // basic_string (const basic_string&, const allocator_type&)
        MEMBER_2 (ctor, str, cstr, alloc),
        // basic_string (const_pointer, size_type)
        MEMBER_2 (ctor, str, cptr, size),
        // basic_string (const_pointer, size_type, const allocator_type&)
        MEMBER_3 (ctor, str, cptr, size, alloc),
        // basic_string (const basic_string&, size_type)
        MEMBER_2 (ctor, str, cstr, size),
        // basic_string (const basic_string&, size_type, const allocator&)
        MEMBER_3 (ctor, str, cstr, size, alloc),
        // basic_string (const basic_string&, size_type, size_type)
        MEMBER_3 (ctor, str, cstr, size, size),
        // basic_string (const basic_string&, size_type, size_type, allocator&)
        MEMBER_4 (ctor, str, cstr, size, size, alloc),
        // basic_string (size_type, value_type)
        MEMBER_2 (ctor, str, size, val),
        // basic_string (size_type, value_type, const allocator_type&)
        MEMBER_3 (ctor, str, size, val, alloc),
        // basic_string (InputIterator, InputIterator)
        MEMBER_1 (ctor, str, range),
        // basic_string (InputIterator, InputIterator, const allocator&)
        MEMBER_2 (ctor, str, range, alloc),

        //////////////////////////////////////////////////////////////
        // operator= (const_pointer)
        MEMBER_1 (op_set, str, cptr),
        // operator= (const basic_string&)
        MEMBER_1 (op_set, str, cstr),
        // operator= (value_type)
        MEMBER_1 (op_set, str, val),

        //////////////////////////////////////////////////////////////
        // swap (basic_string&)
        MEMBER_1 (swap, str, str),

        //////////////////////////////////////////////////////////////
        // push_back (value_type)
        MEMBER_1 (push_back, str, val),

        //////////////////////////////////////////////////////////////
        // operator+ (const_pointer, const basic_string&)
        NON_MEMBER_2 (op_plus, cptr, cstr),
        // operator+ (const basic_string&, const basic_string&)
        NON_MEMBER_2 (op_plus, cstr, cstr),
        // operator+ (const basic_string&, const_pointer)
        NON_MEMBER_2 (op_plus, cstr, cptr),
        // operator+ (const basic_string&, value_type)
        NON_MEMBER_2 (op_plus, cstr, val),
        // operator+ (value_type, const basic_string&)
        NON_MEMBER_2 (op_plus, val, cstr),

        //////////////////////////////////////////////////////////////
        // operator== (const_pointer, const basic_string&)
        NON_MEMBER_2 (op_equal, cptr, cstr),
        // operator== (const basic_string&, const basic_string&)
        NON_MEMBER_2 (op_equal, cstr, cstr),
        // operator== (const basic_string&, const_pointer)
        NON_MEMBER_2 (op_equal, cstr, cptr),

        //////////////////////////////////////////////////////////////
        // operator!= (const_pointer, const basic_string&)
        NON_MEMBER_2 (op_not_equal, cptr, cstr),
        // operator!= (const basic_string&, const basic_string&)
        NON_MEMBER_2 (op_not_equal, cstr, cstr),
        // operator!= (const basic_string&, const_pointer)
        NON_MEMBER_2 (op_not_equal, cstr, cptr),

        //////////////////////////////////////////////////////////////
        // operator< (const_pointer, const basic_string&)
        NON_MEMBER_2 (op_less, cptr, cstr),
        // operator< (const basic_string&, const basic_string&)
        NON_MEMBER_2 (op_less, cstr, cstr),
        // operator< (const basic_string&, const_pointer)
        NON_MEMBER_2 (op_less, cstr, cptr),

        //////////////////////////////////////////////////////////////
        // operator<= (const_pointer, const basic_string&)
        NON_MEMBER_2 (op_less_equal, cptr, cstr),
        // operator<= (const basic_string&, const basic_string&)
        NON_MEMBER_2 (op_less_equal, cstr, cstr),
        // operator<= (const basic_string&, const_pointer)
        NON_MEMBER_2 (op_less_equal, cstr, cptr),

        //////////////////////////////////////////////////////////////
        // operator> (const_pointer, const basic_string&)
        NON_MEMBER_2 (op_greater, cptr, cstr),
        // operator> (const basic_string&, const basic_string&)
        NON_MEMBER_2 (op_greater, cstr, cstr),
        // operator> (const basic_string&, const_pointer)
        NON_MEMBER_2 (op_greater, cstr, cptr),

        //////////////////////////////////////////////////////////////
        // operator>= (const_pointer, const basic_string&)
        NON_MEMBER_2 (op_greater_equal, cptr, cstr),
        // operator>= (const basic_string&, const basic_string&)
        NON_MEMBER_2 (op_greater_equal, cstr, cstr),
        // operator>= (const basic_string&, const_pointer)
        NON_MEMBER_2 (op_greater_equal, cstr, cptr)
    };

// clean up helper macros used above
#undef _FCAT
#undef FCAT
#undef FID_0
#undef FID_1
#undef FID_2
#undef FID_3
#undef FID_4
#undef FID_5

#undef ARG

#undef SIG_0
#undef SIG_1
#undef SIG_2
#undef SIG_3
#undef SIG_4
#undef SIG_5

#undef MEMBER_0
#undef MEMBER_1
#undef MEMBER_2
#undef MEMBER_3
#undef MEMBER_4
#undef MEMBER_5

    static ArgId arg_type (OverloadId id, int argno) {
        return ArgId (((id >> fid_bits) >> argno * arg_bits) & arg_mask);
    }
};


static const _RWSTD_SIZE_T
NPOS = _RWSTD_SIZE_MAX;


struct StringFunc
{
    StringIds::CharId     char_id_;
    StringIds::TraitsId   traits_id_;
    StringIds::AllocId    alloc_id_;
    StringIds::OverloadId which_;
};


// describes a single test case for any overload of any string
// function (the same test case can be used to exercise more
// than one overload of the same function)
struct StringTestCase
{
    int            line;      // test case line number

    int            off;       // offset (position argument)
    int            size;      // size (count argument)

    int            off2;      // offset 2 (position argument)
    int            size2;     // size 2 (count argument)

    int            val;       // value (single character to append)

    const char*    str;       // controlled sequence
    _RWSTD_SIZE_T  str_len;   // length of sequence

    const char*    arg;       // sequence to insert
    _RWSTD_SIZE_T  arg_len;   // length of sequence

    const char*    res;       // resulting sequence
    _RWSTD_SIZE_T  nres;      // length of sequence or expected result
                              // value for find, rfind, compare, etc

    int            bthrow;    // exception expected
};


// describes a set of test cases for a single overload of a function
struct StringTest
{
    // string function overload to exercise
    StringIds::OverloadId which;

    // test cases to exercise overload withh
    const StringTestCase *cases;

    // number of test cases
    _RWSTD_SIZE_T case_count;
};


typedef void StringTestFunc (const StringFunc&, const StringTestCase&);

_TEST_EXPORT int
rw_run_string_test (int, char**, const char*, const char*,
                    StringTestFunc*, const StringTest*, _RWSTD_SIZE_T);


// encapsulates the state of a string object without regard to type
// used in exception safety tests to determine changes to the state
// after a modifying operation throws an exception
struct StringState
{
    const void*   data_;
    _RWSTD_SIZE_T size_;
    _RWSTD_SIZE_T capacity_;

    // invokes rw_assert() to verify that two states are the same
    void assert_equal (const StringState&, int, int, const char*) const;
};


// creates a StringState object from a basic_string
template <class String>
inline StringState
rw_get_string_state (const String &str)
{
    const StringState state = {
        str.data (), str.size (), str.capacity ()
    };

    return state;
}


#define Disabled(which)   \
    StringIds::opt_memfun_disabled [which & ~StringIds::fid_mask]


#ifndef _RWSTD_NO_WCHAR_T
#  define TEST_DISPATCH(Alloc, fname, func, tcase)              \
    if (StringIds::DefaultTraits == func.traits_id_) {          \
        if (StringIds::Char == func.char_id_)                   \
            fname (char (), (std::char_traits<char>*)0,         \
                   (Alloc<char>*)0, func, tcase);               \
        else if (StringIds::WChar == func.char_id_)             \
            fname (wchar_t (), (std::char_traits<wchar_t>*)0,   \
                   (Alloc<wchar_t>*)0, func, tcase);            \
        else                                                    \
            rw_note (0, 0, 0,                                   \
                     "%{$CLASS} tests not implemented");        \
    }                                                           \
    else {                                                      \
        if (StringIds::Char == func.char_id_)                   \
            fname (char (), (UserTraits<char>*)0,               \
                   (Alloc<char>*)0, func, tcase);               \
       else if (StringIds::WChar == func.char_id_)              \
            fname (wchar_t (), (UserTraits<wchar_t>*)0,         \
                   (Alloc<wchar_t>*)0, func, tcase);            \
       else                                                     \
           fname (UserChar (), (UserTraits<UserChar>*)0,        \
                  (Alloc<UserChar>*)0, func, tcase);            \
    }                                                           \
    (void)0

#else   // if defined (_RWSTD_NO_WCHAR_T)
#  define TEST_DISPATCH(Alloc, fname, func, tcase)              \
    if (StringIds::DefaultTraits == func.traits_id_) {          \
        if (StringIds::Char == func.char_id_)                   \
            fname (char (), (std::char_traits<char>*)0,         \
                   (Alloc<char>*)0, func, tcase);               \
        else if (StringIds::WChar == func.char_id_)             \
            RW_ASSERT (!"logic error: wchar_t disabled");       \
        else                                                    \
            rw_note (0, 0, 0,                                   \
                     "%{$CLASS} tests not implemented");        \
        }                                                       \
    }                                                           \
    else {                                                      \
        if (StringIds::Char == func.char_id_)                   \
            fname (char (), (UserTraits<char>*)0,               \
                   (Alloc<char>*)0, func, tcase);               \
        else if (StringIds::WChar == func.char_id_)             \
             RW_ASSERT (!"logic error: wchar_t disabled");      \
        else if (StringIds::UChar == func.char_id_)             \
            fname (UserChar (), (UserTraits<UserChar>*)0,       \
                   (Alloc<UserChar>*)0, func, tcase);           \
    }                                                           \
    (void)0

#endif   // _RWSTD_NO_WCHAR_T


#define DEFINE_STRING_TEST_DISPATCH(fname)                      \
    static void                                                 \
    fname (const StringFunc     &fun,                           \
           const StringTestCase &tcase) {                       \
        if (StringIds::DefaultAlloc == fun.alloc_id_) {         \
            TEST_DISPATCH (std::allocator, fname, fun, tcase);  \
        }                                                       \
        else if (StringIds::UserAlloc == fun.alloc_id_) {       \
            TEST_DISPATCH (UserAlloc, fname, fun, tcase);       \
        }                                                       \
        else                                                    \
            RW_ASSERT (!"logic error: bad allocator");          \
    } typedef void rw_unused_typedef
    

#endif   // RW_21_STRINGS_H_INCLUDED
