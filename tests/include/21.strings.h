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

struct _TEST_EXPORT StringMembers {

    // identifiers for the charT template argument
    enum charT  { UnknownChar = 0, Char, WChar, UChar };

    // identifiers for  the Traits template argument
    enum Traits { UnknownTraits = 0, DefaultTraits, UserTraits };

    // identifiers for  the Allocator template argument
    enum Allocator { UnknownAllocator = 0, DefaultAllocator, UserAllocator };

    enum SignatureId {
        // (void)
        sig_void = 1,
        // (const value_type*)
        sig_ptr,
        // (const basic_string&)
        sig_str,
        // (size_type)
        sig_size,
        // (const value_type*, size_type)
        sig_ptr_size,
        // (const basic_string&, size_type)
        sig_str_size,
        // (const value_type*, size_type, size_type)
        sig_ptr_size_size,
        // (const basic_string&, size_type, size_type)
        sig_str_size_size,
        // (size_type, const value_type*, size_type)
        sig_size_ptr_size,
        // (size_type, const basic_string&, size_type, size_type)
        sig_size_str_size_size,
        // (size_type, value_type)
        sig_size_val,
        // (size_type, const basic_string&)
        sig_size_str,
        // (size_type, size_type)
        sig_size_size,
        // (size_type, size_type, const value_type*)
        sig_size_size_ptr,
        // (size_type, size_type, const basic_string&)
        sig_size_size_str,
        // (size_type, size_type, value_type)
        sig_size_size_val,
        // (size_type, size_type, const value_type*, size_type)
        sig_size_size_ptr_size,
        // (size_type, size_type, const value_type*, size_type, size_type)
        sig_size_size_str_size_size,
        // (size_type, size_type, size_type, value_type)
        sig_size_size_size_val,
        // (value_type)
        sig_val,
        // (value_type, size_type)
        sig_val_size,
        // (InputIterator, InputIterator)
        sig_range,
        // (iterator)
        sig_iter,
        // (iterator, value_type)
        sig_iter_val,
        // (iterator, size_type, value_type)
        sig_iter_size_val,
        // (iterator, InputIterator, InputIterator)
        sig_iter_range,
        // (iterator, iterator)
        sig_iter_iter,
        // (iterator, iterator, const value_type*)
        sig_iter_iter_ptr,
        // (iterator, iterator, const basic_string&)
        sig_iter_iter_str,
        // (iterator, iterator, const value_type*, size_type)
        sig_iter_iter_ptr_size,
        // (iterator, iterator, size_type, value_type)
        sig_iter_iter_size_val,
        // (iterator, iterator, InputIterator, InputIterator)
        sig_iter_iter_range,

        //
        sig_last
    };

    enum MemberId {
        mem_append             = 1 << 6,
        mem_assign             = 1 << 7,
        mem_erase              = 1 << 8,
        mem_insert             = 1 << 9,
        mem_replace            = 1 << 10,
        mem_op_plus_eq         = 1 << 11,
        mem_find               = 1 << 12,
        mem_rfind              = 1 << 13,
        mem_find_first_of      = 1 << 14,
        mem_find_last_of       = 1 << 15,
        mem_find_first_not_of  = 1 << 16,
        mem_find_last_not_of   = 1 << 17,
        mem_compare            = 1 << 18,
        mem_mask       =
            mem_append | mem_assign | mem_erase | mem_insert | 
            mem_replace | mem_op_plus_eq | mem_find | mem_rfind |
            mem_find_first_of | mem_find_last_of | mem_find_first_not_of |
            mem_find_last_not_of | mem_compare
    };

    // unique identifiers for all overloads of each member function
    enum OverloadId {
        UnknownOverload = 0,
        //////////////////////////////////////////////////////////////
        // append (const value_type*)
        append_ptr = mem_append + sig_ptr,
        // append (const basic_string&)
        append_str = mem_append + sig_str,
        // append (const value_type*, size_type)
        append_ptr_size = mem_append + sig_ptr_size,
        // append (const basic_string&, size_type, size_type)
        append_str_size_size = mem_append + sig_str_size_size,
        // append (size_type, value_type)
        append_size_val = mem_append + sig_size_val,
        // append (InputIterator, InputIterator)
        append_range = mem_append + sig_range,

        //////////////////////////////////////////////////////////////
        // assign (const value_type*)
        assign_ptr = mem_assign + sig_ptr,
        // assign (const basic_string&)
        assign_str = mem_assign + sig_str,
        // assign (const value_type*, size_type)
        assign_ptr_size = mem_assign + sig_ptr_size,
        // assign (const basic_string&, size_type, size_type)
        assign_str_size_size = mem_assign + sig_str_size_size,
        // assign (size_type, value_type)
        assign_size_val = mem_assign + sig_size_val,
        // assign (InputIterator, InputIterator)
        assign_range = mem_assign + sig_range,

        //////////////////////////////////////////////////////////////
        // erase ()
        erase_void = mem_erase + sig_void,
        // erase (size_type)
        erase_size = mem_erase + sig_size,
        // erase (size_type, size_type)
        erase_size_size = mem_erase + sig_size_size,
        // erase (iterator)
        erase_iter = mem_erase + sig_iter,
        // erase (iterator, iterator)
        erase_iter_iter = mem_erase + sig_iter_iter,

        //////////////////////////////////////////////////////////////
        // insert (size_type, const value_type*)
        insert_size_ptr = mem_insert + sig_size_val,
        // insert (size_type, const basic_string&)
        insert_size_str = mem_insert + sig_size_str,
        // insert (size_type, const value_type*, size_type)
        insert_size_ptr_size = mem_insert + sig_size_ptr_size,
        // insert (size_type, basic_string&, size_type, size_type)
        insert_size_str_size_size = mem_insert + sig_size_str_size_size,
        // insert (size_type, size_type, value_type)
        insert_size_size_val = mem_insert + sig_size_size_val,
        // insert (iterator, value_type)
        insert_val = mem_insert + sig_iter_val,
        // insert (iterator, size_type, value_type)
        insert_size_val = mem_insert + sig_iter_size_val,
        // insert (iterator p, InputIterator, InputIterator)
        insert_range = mem_insert + sig_iter_range,

        //////////////////////////////////////////////////////////////
        // (size_type, size_type, const value_type*)
        replace_size_size_ptr = mem_replace + sig_size_size_ptr,
        // (size_type, size_type, basic_string&)
        replace_size_size_str = mem_replace + sig_size_size_str,
        // (size_type, size_type, const value_type*, size_type)
        replace_size_size_ptr_size = mem_replace + sig_size_size_ptr_size,
        // (size_type, size_type, const basic_string&, size_type, size_type)
        replace_size_size_str_size_size =
            mem_replace + sig_size_size_str_size_size,
        // (size_type, size_type, size_type, value_type)
        replace_size_size_size_val = mem_replace + sig_size_size_size_val,
        // (iterator, iterator, const value_type*)
        replace_iter_iter_ptr = mem_replace + sig_iter_iter_ptr,
        // (iterator, iterator, const basic_string&)
        replace_iter_iter_str = mem_replace + sig_iter_iter_str,
        // (iterator, iterator, const value_type*, size_type)
        replace_iter_iter_ptr_size = mem_replace + sig_iter_iter_ptr_size,
        // (iterator, iterator, size_type, value_type)
        replace_iter_iter_size_val = mem_replace + sig_iter_iter_size_val,
        // (iterator, iterator, InputIterator, InputIterator)
        replace_iter_iter_range = mem_replace + sig_iter_iter_range,

        // operator += (const value_type*)
        op_plus_eq_ptr = mem_op_plus_eq + sig_ptr,   
        // operator += (const basic_string&)
        op_plus_eq_str = mem_op_plus_eq + sig_str,
        // operator += (value_type)
        op_plus_eq_val = mem_op_plus_eq + sig_val,

        //////////////////////////////////////////////////////////////
        // overloads of find, rfind, find_first_of, find_last_of, 
        // find_first_not_of, find_last_not_of

        // find (const value_type*)
        find_ptr = mem_find + sig_ptr,
        // find (basic_string&)
        find_str = mem_find + sig_str,
        // find (const value_type*, size_type)
        find_ptr_size = mem_find + sig_ptr_size,
        // find (const value_type*, size_type, size_type)
        find_ptr_size_size = mem_find + sig_ptr_size_size,
        // find (basic_string&, size_type)
        find_str_size = mem_find + sig_str_size,
        // find (value_type)
        find_val = mem_find + sig_val,
        // find (value_type, size_type)
        find_val_size = mem_find + sig_val_size,

        //////////////////////////////////////////////////////////////
        // rfind (const value_type*)
        rfind_ptr = mem_rfind + sig_ptr,
        // rfind (basic_string&)
        rfind_str = mem_rfind + sig_str,
        // rfind (const value_type*, size_type)
        rfind_ptr_size = mem_rfind + sig_ptr_size,
        // rfind (const value_type*, size_type, size_type)
        rfind_ptr_size_size = mem_rfind + sig_ptr_size_size,
        // rfind (basic_string&, size_type)
        rfind_str_size = mem_rfind + sig_str_size,
        // rfind (value_type)
        rfind_val = mem_rfind + sig_val,
        // rfind (value_type, size_type)
        rfind_val_size = mem_rfind + sig_val_size,

        //////////////////////////////////////////////////////////////
        // find_first_of (const value_type*)
        find_first_of_ptr = mem_find_first_of + sig_ptr,
        // find_first_of (basic_string&)
        find_first_of_str = mem_find_first_of + sig_str,
        // find_first_of (const value_type*, size_type)
        find_first_of_ptr_size = mem_find_first_of + sig_ptr_size,
        // find_first_of (const value_type*, size_type, size_type)
        find_first_of_ptr_size_size = mem_find_first_of + sig_ptr_size_size,
        // find_first_of (basic_string&, size_type)
        find_first_of_str_size = mem_find_first_of + sig_str_size,
        // find_first_of (value_type)
        find_first_of_val = mem_find_first_of + sig_val,
        // find_first_of (value_type, size_type)
        find_first_of_val_size = mem_find_first_of + sig_val_size,

        //////////////////////////////////////////////////////////////
        // find_last_of (const value_type*)
        find_last_of_ptr = mem_find_last_of + sig_ptr,
        // find_last_of (basic_string&)
        find_last_of_str = mem_find_last_of + sig_str,
        // find_last_of (const value_type*, size_type)
        find_last_of_ptr_size = mem_find_last_of + sig_ptr_size,
        // find_last_of (const value_type*, size_type, size_type)
        find_last_of_ptr_size_size = mem_find_last_of + sig_ptr_size_size,
        // find_last_of (basic_string&, size_type)
        find_last_of_str_size = mem_find_last_of + sig_str_size,
        // find_last_of (value_type)
        find_last_of_val = mem_find_last_of + sig_val,
        // find_last_of (value_type, size_type)
        find_last_of_val_size = mem_find_last_of + sig_val_size,

        //////////////////////////////////////////////////////////////
        // find_first_not_of (const value_type*)
        find_first_not_of_ptr = mem_find_first_not_of + sig_ptr,
        // find_first_not_of (basic_string&)
        find_first_not_of_str = mem_find_first_not_of + sig_str,
        // find_first_not_of (const value_type*, size_type)
        find_first_not_of_ptr_size = mem_find_first_not_of + sig_ptr_size,
        // find_first_not_of (const value_type*, size_type, size_type)
        find_first_not_of_ptr_size_size = 
            mem_find_first_not_of + sig_ptr_size_size,
        // find_first_not_of (basic_string&, size_type)
        find_first_not_of_str_size = mem_find_first_not_of + sig_str_size,
        // find_first_not_of (value_type)
        find_first_not_of_val = mem_find_first_not_of + sig_val,
        // find_first_not_of (value_type, size_type)
        find_first_not_of_val_size = mem_find_first_not_of + sig_val_size,

        //////////////////////////////////////////////////////////////
        // find_last_not_of (const value_type*)
        find_last_not_of_ptr = mem_find_last_not_of + sig_ptr,
        // find_last_not_of (basic_string&)
        find_last_not_of_str = mem_find_last_not_of + sig_str,
        // find_last_not_of (const value_type*, size_type)
        find_last_not_of_ptr_size = mem_find_last_not_of + sig_ptr_size,
        // find_last_not_of (const value_type*, size_type, size_type)
        find_last_not_of_ptr_size_size = 
            mem_find_last_not_of + sig_ptr_size_size,
        // find_last_not_of (basic_string&, size_type)
        find_last_not_of_str_size = mem_find_last_not_of + sig_str_size,
        // find_last_not_of (value_type)
        find_last_not_of_val = mem_find_last_not_of + sig_val,
        // find_last_not_of (value_type, size_type)
        find_last_not_of_val_size = mem_find_last_not_of + sig_val_size,

        //////////////////////////////////////////////////////////////
        // compare (const value_type*)
        compare_ptr = mem_compare + sig_ptr,
        // compare (const basic_string&)
        compare_str = mem_compare + sig_str,
        // compare (size_type, size_type, const value_type*)
        compare_size_size_ptr = mem_compare + sig_size_size_ptr,
        // compare (size_type, size_type, const basic_string&)
        compare_size_size_str = mem_compare + sig_size_size_str,
        // compare (size_type, size_type, const value_type*, size_type)
        compare_size_size_ptr_size = mem_compare + sig_size_size_ptr_size,
        // (size_type, size_type, const basic_string&, size_type, size_type)
        compare_size_size_str_size_size = 
            mem_compare + sig_size_size_str_size_size
    };

    struct Function {
        charT      char_id_;
        Traits     traits_id_;
        Allocator  alloc_id_;
        OverloadId which_;
    };

    // describes a single test case for any overload
    // of any member function (the same test case can
    // be used to exercise more than one overload of
    // the same member function)
    struct TestCase {
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

    // describes a set of test cases for a single overload
    // of a member function
    struct Test {
        OverloadId      which;        // member function overload to exercise
        const TestCase *cases;        // test cases to exercise overload withh
        _RWSTD_SIZE_T   case_count;   // number of test cases
    };

    typedef void TestFun (const Function&, const TestCase&);

    static int
    run_test (int, char**, const char*, const char*,
              TestFun*, const Test*, _RWSTD_SIZE_T);

    enum { long_string_len = 4096 };

    static char
    long_string [long_string_len];
};

#define Disabled(which)   \
    StringMembers::opt_memfun_disabled [which & ~StringMembers::mem_mask]

#ifndef _RWSTD_NO_WCHAR_T
#  define TEST_DISPATCH(fname, memfun, tcase)                   \
    if (StringMembers::DefaultTraits == memfun.traits_id_) {    \
        if (StringMembers::Char == memfun.char_id_)             \
            fname (char (), (std::char_traits<char>*)0,         \
                   memfun.which_, tcase);                       \
        else if (StringMembers::WChar == memfun.char_id_)       \
            fname (wchar_t (), (std::char_traits<wchar_t>*)0,   \
                   memfun.which_, tcase);                       \
        else                                                    \
            rw_note (0, 0, 0,                                   \
                     "%{$CLASS} tests not implemented");        \
    }                                                           \
    else {                                                      \
       if (StringMembers::Char == memfun.char_id_)              \
           fname (char (), (UserTraits<char>*)0,                \
                  memfun.which_, tcase);                        \
       else if (StringMembers::WChar == memfun.char_id_)        \
           fname (wchar_t (), (UserTraits<wchar_t>*)0,          \
                  memfun.which_, tcase);                        \
       else                                                     \
           fname (UserChar (), (UserTraits<UserChar>*)0,        \
                  memfun.which_, tcase);                        \
    }                                                           \
    (void)0

#else   // if defined (_RWSTD_NO_WCHAR_T)
#  define TEST_DISPATCH(fname, memfun, tcase)                   \
    if (StringMembers::DefaultTraits == memfun.traits_id_) {    \
        if (StringMembers::Char == memfun.char_id_)             \
            fname (char (), (std::char_traits<char>*)0,         \
                   memfun.which_, tcase);                       \
        else if (StringMembers::WChar == memfun.char_id_)       \
            RW_ASSERT (!"logic error: wchar_t disabled");       \
        else                                                    \
            rw_note (0, 0, 0,                                   \
                     "%{$CLASS} tests not implemented");        \
        }                                                       \
    }                                                           \
    else {                                                      \
        if (StringMembers::Char == memfun.char_id_)             \
            fname (char (), (UserTraits<char>*)0,               \
                   memfun.which_, tcase);                       \
        else if (StringMembers::WChar == memfun.char_id_)       \
             RW_ASSERT (!"logic error: wchar_t disabled");      \
        else if (StringMembers::UChar == memfun.char_id_)       \
            fname (UserChar (), (UserTraits<UserChar>*)0,       \
                   memfun.which_, tcase);                       \
    }                                                           \
    (void)0

#endif   // _RWSTD_NO_WCHAR_T


#define DEFINE_TEST_DISPATCH(fname)                             \
    static void                                                 \
    fname (const MemFun &memfun, const TestCase &tcase) {       \
        TEST_DISPATCH (fname, memfun, tcase);                   \
    } typedef void rw_unused_typedef
    

#endif   // RW_21_STRINGS_H_INCLUDED
