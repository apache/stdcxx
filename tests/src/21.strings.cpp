/************************************************************************
 *
 * 21.strings.cpp - definitions of helpers used in clause 21 tests
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

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <21.strings.h>

#include <cmdopt.h>       // for rw_enabled()
#include <driver.h>       // for rw_info()
#include <environ.h>      // for rw_putenv()
#include <rw_printf.h>    // for rw_asnprintf()

#include <stdarg.h>       // for va_arg, ...
#include <stddef.h>       // for size_t
#include <stdlib.h>       // for free()

/**************************************************************************/

char StringMembers::
long_string [StringMembers::long_string_len];

int StringMembers::
opt_memfun_disabled [StringMembers::sig_last];

int StringMembers::
opt_no_user_char;

int StringMembers::
opt_no_char_traits;

int StringMembers::
opt_no_user_traits;

int StringMembers::
opt_no_exceptions;

int StringMembers::
opt_no_exception_safety;


void StringMembers::
setvars (const Function &fun, const TestCase *pcase /* = 0 */)
{
    static const char* const char_names[] = {
        "char", "wchar_t", "UserChar"
    };


    static const char* const traits_names[] = {
        "char_traits", "UserTraits"
    };


    static const char* const allocator_names[] = {
        "allocator", "UserAllocator"
    };

    char*  buf     = 0;
    size_t bufsize = 0;

    if (0 == pcase) {
        // set the {CLASS}, {FUNC}, and {FUNCSIG} environment variables
        // to the name of the specialization of the template, the name
        // of the member function, and the name of the overload of the
        // member function, respectively, when no test case is given

        if (   DefaultTraits == fun.traits_id_
            && (Char == fun.char_id_ || WChar == fun.char_id_)) {
            // format std::string and std::wstring
            rw_asnprintf (&buf, &bufsize,
                          "std::%{?}w%{;}string", WChar == fun.char_id_);
        }
        else {
            // format std::basic_string specializations other than
            // std::string and std::wstring, leaving out the name
            // of the default allocator for brevity
            rw_asnprintf (&buf, &bufsize,
                          "std::basic_string<%s, %s<%1$s>%{?}, %s<%1$s>%{;}>",
                          char_names [fun.char_id_ - 1],
                          traits_names [fun.traits_id_ - 1],
                          DefaultAllocator != fun.alloc_id_,
                          allocator_names [fun.alloc_id_ - 1]);
        }

        // set the {CLASS} variable to the name of the specialization
        // of basic_string
        rw_putenv ("CLASS=");
        rw_fprintf (0, "%{$CLASS:=*}", buf);

        // determine the member function name
        const char* fname = 0;

        if (fun.which_ & mem_append)
            fname = "append";
        else if (fun.which_ & mem_assign)
            fname = "assign";
        else if (fun.which_ & mem_erase)
            fname ="erase";
        else if (fun.which_ & mem_insert)
            fname ="insert";
        else if (fun.which_ & mem_replace)
            fname ="replace";
        else if (fun.which_ & mem_op_plus_eq)
            fname ="operator+=";

        free (buf);
        buf     = 0;
        bufsize = 0;

        // set the {FUNC} variable to the unqualified name
        // of the member function
        rw_asnprintf (&buf, &bufsize, "%s", fname);

        rw_putenv ("FUNC=");
        rw_fprintf (0, "%{$FUNC:=*}", buf);

        static const char* const signatures[] = {
            "void",
            "const value_type*",
            "const basic_string&",
            "size_type",
            "const value_type*, size_type",
            "const basic_string&, size_type, size_type",
            "size_type, const value_type*, size_type",
            "size_type, const basic_string&, size_type, size_type",
            "size_type, value_type",
            "size_type, const basic_string&",
            "size_type, size_type",
            "size_type, size_type, const value_type*",
            "size_type, size_type, const basic_string&",
            "size_type, size_type, value_type",
            "size_type, size_type, const value_type*, size_type",
            "size_type, size_type, const value_type*, size_type, size_type",
            "size_type, size_type, size_type, value_type",
            "value_type",
            "InputIterator, InputIterator",
            "iterator, value_type",
            "iterator, size_type, value_type",
            "iterator, InputIterator, InputIterator",
            "iterator, iterator, const value_type*",
            "iterator, iterator, const basic_string&",
            "iterator, iterator, const value_type*, size_type",
            "iterator, iterator, size_type, value_type",
            "iterator, iterator, InputIterator, InputIterator",
        };

        const size_t siginx = size_t (fun.which_ & ~mem_mask) - 1U;

        RW_ASSERT (siginx < sizeof signatures / sizeof *signatures);

        // append the function signature
        rw_asnprintf (&buf, &bufsize, "%{+} (%s)", signatures [siginx]);

        rw_putenv ("FUNCSIG=");
        rw_fprintf (0, "%{$FUNCSIG:=*}", buf);
        free (buf);

        return;
    }

    // do the function call arguments reference *this?
    const bool self = 0 == pcase->arg;

    // append the ctor argument(s) and the member function name
    rw_asnprintf (&buf, &bufsize,
                  "%{$CLASS} (%{?}%{#*s}%{;}).%{$FUNC} ",
                  pcase->str != 0, int (pcase->str_len), pcase->str);

    // format and append member function arguments
    switch (fun.which_) {
    case append_ptr:
    case assign_ptr:
    case op_plus_eq_ptr:
        rw_asnprintf (&buf, &bufsize,
                      "%{+}(%{?}%{#*s}%{;}%{?}this->c_str ()%{;})",
                      !self, int (pcase->arg_len), pcase->arg, self);
        break;

    case append_str:
    case assign_str:
    case op_plus_eq_str:
        rw_asnprintf (&buf, &bufsize,
                      "%{+}(%{?}string (%{#*s})%{;}%{?}*this%{;})",
                      !self, int (pcase->arg_len), pcase->arg, self);
        break;

    case append_ptr_size:
    case assign_ptr_size:
        rw_asnprintf (&buf, &bufsize, "%{+}("
                      "%{?}%{#*s}%{;}%{?}this->c_str ()%{;}, %zu)",
                      !self, int (pcase->arg_len), pcase->arg,
                      self, pcase->size);
        break;

    case append_str_size_size:
    case assign_str_size_size:
        rw_asnprintf (&buf, &bufsize, "%{+}("
                      "%{?}string (%{#*s})%{;}%{?}*this%{;}, %zu, %zu)",
                      !self, int (pcase->arg_len), pcase->arg, self,
                      pcase->off, pcase->size);
        break;

    case append_size_val:
    case assign_size_val:
        rw_asnprintf (&buf, &bufsize,
                      "%{+} (%zu, %#c)", pcase->size, pcase->val);
        break;

    case append_range:
    case assign_range:
        rw_asnprintf (&buf, &bufsize, "%{+}("
                      "%{?}%{#*s}%{;}%{?}this->%{;}.begin() + %zu, "
                      "%{?}%{#*s}%{;}%{?}this->%{;}.begin() + %zu)",
                      !self, int (pcase->arg_len), pcase->arg,
                      self, pcase->off, !self, int (pcase->arg_len),
                      pcase->arg,
                      self, pcase->off + pcase->size);
        break;

    case insert_size_ptr:
        rw_asnprintf (&buf, &bufsize, 
                      "%{+} (%zu, %{?}%{#*s}%{;}%{?}this->c_str ()%{;})",
                      pcase->off, !self, int (pcase->arg_len), 
                      pcase->arg, self);
        break;

    case insert_size_str:
        rw_asnprintf (&buf, &bufsize,  
                      "%{+} (%zu, %{?}string (%{#*s})%{;}%{?}*this%{;})",
                      pcase->off, !self, int (pcase->arg_len), 
                      pcase->arg, self);
        break;

    case insert_size_ptr_size:
        rw_asnprintf (&buf, &bufsize, "%{+} ("
                      "%zu, %{?}%{#*s}%{;}%{?}this->c_str ()%{;}, %zu)", 
                      pcase->off, !self, int (pcase->arg_len),
                      pcase->arg, self, pcase->size2);
        break;

    case insert_size_str_size_size:
        rw_asnprintf (&buf, &bufsize, "%{+} ("
                      "%zu, %{?}string (%{#*s})%{;}%{?}*this%{;}, %zu, %zu)",
                      pcase->off, !self, int (pcase->arg_len), pcase->arg,
                      self, pcase->off2, pcase->size2);
        break;

    case insert_size_size_val:
        rw_asnprintf (&buf, &bufsize,
                      "%{+} (%zu, %zu, %#c)",
                      pcase->off, pcase->size2, pcase->val);
        break;

    case insert_size_val:
        rw_asnprintf (&buf, &bufsize, 
                      "%{+} (begin + %zu, %zu, %#c)",
                      pcase->off, pcase->size2, pcase->val);
        break;

    case insert_val:
        rw_asnprintf (&buf, &bufsize,
                      "%{+} (begin + %zu, %#c)",
                      pcase->off, pcase->val);
        break;

    case insert_range:
        rw_asnprintf (&buf, &bufsize, "%{+} (begin + %zu, "
                      "%{?}%{#*s}%{;}%{?}*this%{;}.begin + %zu, "
                      "%{?}%{#*s}%{;}%{?}*this%{;}.begin + %zu)", 
                      pcase->off, !self, int (pcase->arg_len), pcase->arg,
                      self, pcase->off2, !self, int (pcase->arg_len),
                      pcase->arg, self, pcase->off2 + pcase->size2);
        break;

    case replace_size_size_ptr:
        rw_asnprintf (&buf, &bufsize, "%{+}("
                      "%zu, %zu, %{?}%{#*s}%{;}%{?}this->c_str ()%{;})",
                      pcase->off, pcase->size, !self, 
                      int (pcase->arg_len), pcase->arg, self);
        break;

    case replace_size_size_str:
        rw_asnprintf (&buf, &bufsize, "%{+}("
                      "%zu, %zu, %{?}string (%{#*s})%{;}%{?}*this%{;})",
                      pcase->off, pcase->size, !self, 
                      int (pcase->arg_len), pcase->arg, self);
        break;

    case replace_size_size_ptr_size:
        rw_asnprintf (&buf, &bufsize, "%{+}("
                      "%zu, %zu, %{?}%{#*s}%{;}%{?}this->c_str ()%{;}, %zu)", 
                      pcase->off, pcase->size, !self, 
                      int (pcase->arg_len), pcase->arg, self, pcase->size2);
        break;

    case replace_size_size_str_size_size:
        rw_asnprintf (&buf, &bufsize, "%{+}(%zu, %zu, "
                      "%{?}string (%{#*s})%{;}%{?}*this%{;}, %zu, %zu)",
                      pcase->off, pcase->size, !self, 
                      int (pcase->arg_len), pcase->arg, self, 
                      pcase->off2, pcase->size2);
        break;

    case replace_size_size_size_val:
        rw_asnprintf (&buf, &bufsize, 
                      "%{+}(%zu, %zu, %zu, %#c)",
                      pcase->off, pcase->size, pcase->size2, pcase->val);
        break;

    case replace_iter_iter_ptr:
        rw_asnprintf (&buf, &bufsize, "%{+}(begin() + %zu, begin() + %zu, "
                      "%{?}%{#*s}%{;}%{?}this->c_str ()%{;})",
                      pcase->off, pcase->off + pcase->size, 
                      !self, int (pcase->arg_len), pcase->arg, self);
        break;

    case replace_iter_iter_str:
        rw_asnprintf (&buf, &bufsize, "%{+}(begin() + %zu, begin() + %zu, " 
                      "%{?}string (%{#*s})%{;}%{?}*this%{;})",
                      pcase->off, pcase->off + pcase->size, 
                      !self, int (pcase->arg_len), pcase->arg, self);
        break;

    case replace_iter_iter_ptr_size:
        rw_asnprintf (&buf, &bufsize, "%{+}(begin() + %zu, begin() + %zu, " 
                      "%{?}%{#*s}%{;}%{?}this->c_str ()%{;}, %zu)", 
                      pcase->off, pcase->off + pcase->size, !self, 
                      int (pcase->arg_len), pcase->arg, self, pcase->size2);
        break;

    case replace_iter_iter_size_val:
        rw_asnprintf (&buf, &bufsize, 
                      "%{+}(begin() + %zu, begin() + %zu, %zu, %#c)",
                      pcase->off, pcase->off + pcase->size, 
                      pcase->size2, pcase->val);
        break;

    case replace_iter_iter_range:
        rw_asnprintf (&buf, &bufsize, "%{+}(begin() + %zu, begin() + %zu, "
                      "%{?}%{#*s}%{;}%{?}this->%{;}begin() + %zu, "
                      "%{?}%{#*s}%{;}%{?}this->%{;}begin() + %zu)", 
                      pcase->off, pcase->off + pcase->size, !self, 
                      int (pcase->arg_len), pcase->arg, self, pcase->off2,
                      !self, int (pcase->arg_len), pcase->arg, self, 
                      pcase->off2 + pcase->size2);
        break;

    case op_plus_eq_val:
        rw_asnprintf (&buf, &bufsize,
                      "%{+} (%#c)", pcase->val);
        break;

    default:
        RW_ASSERT (!"test logic error: unknown overload");
    }

    rw_putenv ("FUNCALL=");
    rw_fprintf (0, "%{$FUNCALL:=*}", buf);
    free (buf);
}

/**************************************************************************/

void StringMembers::
run_test (TestFun *test_callback, const Test *tests, size_t test_count)
{
    if ('\0' == long_string [0]) {
        // initialize long_string
        for (size_t i = 0; i != sizeof long_string - 1; ++i)
            long_string [i] = 'x';
    }

    static const charT char_types[] = {
        Char, WChar, UChar,
        UnknownChar
    };

    static const Traits traits_types[] = {
        DefaultTraits, UserTraits,
        UnknownTraits,
    };

    static const Allocator alloc_types[] = {
        DefaultAllocator,
        UnknownAllocator
    };

    for (size_t i = 0; char_types [i]; ++i) {

        for (size_t j = 0; traits_types [j]; ++j) {

            for (size_t k = 0; alloc_types [k]; ++k) {

                for (size_t m = 0; m != test_count; ++m) {

                    const Function memfun = {
                        char_types [i],
                        traits_types [j],
                        alloc_types [k],
                        tests [m].which
                    };

                    // set the {CLASS}, {FUNC}, and {FUNCSIG} environment
                    // variable to the name of the basic_string specializaton
                    // and its member function being exercised
                    setvars (memfun);

                    rw_info (0, 0, 0, "%{$CLASS}::%{$FUNCSIG}");

                    for (size_t n = 0; n != tests [m].case_count; ++n)
                        if (rw_enabled (tests [m].cases [n].line)) {

                            // set the {FUNCALL} environment variable
                            setvars (memfun, tests [m].cases + n);

                            test_callback (memfun, tests [m].cases [n]);
                        }
                        else
                            rw_note (0, 0, 0,
                                     "test on line %d disabled",
                                     tests [m].cases [n].line);
                }
            }
        }
    }
}
