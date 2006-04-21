/************************************************************************
 *
 * 21_strings.h - definitions of helpers used in clause 21 tests
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
    enum charT  { Char, WChar, UChar };

    // identifiers for  the Traits template argument
    enum Traits { DefaultTraits, UserTraits };

    // identifiers for  the Allocator template argument
    enum Allocator { DefaultAllocator, UserAllocator };

    // identifiers for all overloads of each member function
    enum MemberFunction {

        //////////////////////////////////////////////////////////////
        append,
        append_first = append,
        // append (const value_type*)
        append_ptr = append_first,
        // append (const basic_string&)
        append_str,
        // append (const value_type*, size_type)
        append_ptr_size,
        // append (const basic_string&, size_type, size_type)
        append_str_off_size,
        // append (size_type, value_type)
        append_size_val,
        // append (InputIterator, InputIterator)
        append_range,
        append_last = append_range,

        // number of member function overloads
        append_overloads = append_last - append_first,

        //////////////////////////////////////////////////////////////
        assign,
        assign_first = assign,
        // assign (const value_type*)
        assign_ptr = assign_first,
        // assign (const basic_string&)
        assign_str,
        // assign (const value_type*, size_type)
        assign_ptr_size,
        // assign (const basic_string&, size_type, size_type)
        assign_str_off_size,
        // assign (size_type, value_type)
        assign_size_val,
        // assign (InputIterator, InputIterator)
        assign_range,
        assign_last = assign_range,

        // number of member function overloads
        assign_overloads = assign_last - assign_first,

        //////////////////////////////////////////////////////////////
        insert,
        insert_first = insert,
        // insert (size_type, const value_type*)
        insert_off_ptr = insert_first,
        // insert (size_type, const basic_string&)
        insert_off_str,
        // insert (size_type, const value_type*, size_type)
        insert_off_ptr_size,
        // insert (size_type, basic_string&, size_type, size_type)
        insert_off_str_off_size,
        // insert (size_type, size_type, value_type)
        insert_off_size_val,
        // insert (iterator, value_type)
        insert_val,
        // insert (iterator, size_type, value_type)
        insert_size_val,
        // insert (iterator p, InputIterator, InputIterator)
        insert_range,
        insert_last = insert_range,

        // number of member function overloads
        insert_overloads = insert_last - insert_first,

        //////////////////////////////////////////////////////////////
        replace,
        replace_first = replace,
        // (size_type, size_type, const value_type*)
        replace_off_size_ptr = replace_first,
        // (size_type, size_type, basic_string&)
        replace_off_size_str,
        // (size_type1, size_type, const value_type*, size_type)
        replace_off_size_ptr_size,
        // (size_type, size_type, const basic_string&, size_type, size_type)
        replace_off_size_str_off_size,
        // (size_type, size_type, size_type, value_type)
        replace_off_size_size_val,
        // (iterator, iterator, const value_type*)
        replace_ptr,
        // (iterator, iterator, const basic_string&)
        replace_str,
        // (iterator, iterator, const value_type*, size_type)
        replace_ptr_size,
        // (iterator, iterator, size_type, value_type)
        replace_size_val,
        // (iterator, iterator, InputIterator, InputIterator)
        replace_range,
        replace_last = replace_range,

        // number of member function overloads
        replace_overloads = replace_last - replace_first,

        //////////////////////////////////////////////////////////////
        // must be last: total number of member functions
        member_functions
    };

    // describes a single test case for any overload
    // of any member function
    struct TestCase {
        MemberFunction which;     // member function to call
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
        const TestCase *cases;        // test cases to exercise
        _RWSTD_SIZE_T   case_count;   // number of test cases
        const char     *funsig;       // function signature
    };

    // dynamically allocates and formats a string describing
    // the call to the member function, including the values
    // of its arguments, specfified by its arguments
    static char*
    format (charT, Traits, Allocator, const TestCase&);

    // array of integers to use for command line option
    // processing (to disable individual overloads of all
    // member functions)
    static int
    opt_memfun_disabled [member_functions];

    static int opt_no_user_char;          // for --no-user_char
    static int opt_no_char_traits;        // for --no-char_traits
    static int opt_no_user_traits;        // for --no-user_traits

    static int opt_no_exceptions;         // for --no-exceptions
    static int opt_no_exception_safety;   // for --no-exception-safety
};


#endif   // RW_21_STRINGS_H_INCLUDED
