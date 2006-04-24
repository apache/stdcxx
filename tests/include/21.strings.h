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
        // (InputIterator, InputIterator)
        sig_range,
        // (iterator, value_type)
        sig_iter_val,
        // (iterator, size_type, value_type)
        sig_iter_size_val,
        // (iterator, InputIterator, InputIterator)
        sig_iter_range,
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
        mem_append  = 1 << 5,
        mem_assign  = 1 << 6,
        mem_erase   = 1 << 7,
        mem_insert  = 1 << 8,
        mem_replace = 1 << 9,
        mem_mask    =
            mem_append | mem_assign | mem_erase | mem_insert | mem_replace
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
        replace_iter_iter_range = mem_replace + sig_iter_iter_range
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
        _RWSTD_SIZE_T  res_len;   // length of sequence

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

    static void
    run_test (TestFun*, const Test*, _RWSTD_SIZE_T);

    // array of integers to use for command line option
    // processing (to disable individual overloads of all
    // member functions)
    static int
    opt_memfun_disabled [sig_last];

    static int opt_no_user_char;          // for --no-user_char
    static int opt_no_char_traits;        // for --no-char_traits
    static int opt_no_user_traits;        // for --no-user_traits

    static int opt_no_exceptions;         // for --no-exceptions
    static int opt_no_exception_safety;   // for --no-exception-safety

private:

    // sets the {CLASS}, {FUNC}, {FUNCSIG}, and optionally {FUNCALL}
    // environment variables as follows:
    // CLASS:   the name of basic_string specialization
    // FUNC:    the name of the basic_string member function
    // FUNCSIG: the name and signature of a specific overload
    //          of the basic_string member function
    // FUNCALL: a string describing the call to the basic_string member
    //          function with function with function arguments expanded
    //          (as specified by the TestCase argument)
    static void
    setvars (const Function &fun, const TestCase* = 0);

};

#define Disabled(which)   \
    StringMembers::opt_memfun_disabled [which & ~StringMembers::mem_mask]

#endif   // RW_21_STRINGS_H_INCLUDED
