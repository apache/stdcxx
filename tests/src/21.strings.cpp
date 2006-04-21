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
#include <rw_printf.h>    // for rw_asnprintf()

#include <stddef.h>       // for size_t

/**************************************************************************/

static const char* const char_names[] = {
    "char", "wchar_t", "UserChar"
};

static const char* const traits_names[] = {
    "char_traits", "UserTraits"
};

static const char* const allocator_names[] = {
    "allocator", "UserAllocator"
};

static const char* const function_names[] = {
    "append", "assign", "erase", "insert", "replace"
};


int StringMembers::
opt_memfun_disabled [StringMembers::member_functions];

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


char* StringMembers::
format (charT cid, Traits tid, Allocator aid, const TestCase &tcase)
{
    char*  buf     = 0;
    size_t bufsize = 0;

    const bool self = 0 == tcase.arg;

    if (DefaultTraits == tid && (Char == cid || WChar == cid)) {
        // format std::string and std::wstring
        rw_asnprintf (&buf, &bufsize,
                      "std::%{?}w%{;}string (%{?}%{#*s}%{;}).",
                      WChar == cid,
                      tcase.str != 0, int (tcase.str_len), tcase.str);
    }
    else {
        // format std::basic_string specializations other than
        // std::string and std::wstring, leaving out the name
        // of the default allocator for brevity
        rw_asnprintf (&buf, &bufsize,
                      "std::basic_string<%s, %s<%1$s>%{?}, %s<%1$s>%{;}>"
                      "(%{?}%{#*s}%{;}).",
                      char_names [cid],
                      traits_names [tid],
                      DefaultAllocator != aid, allocator_names [aid],
                      tcase.str != 0, int (tcase.str_len), tcase.str);
    }

    const char* fname = 0;

    if (append_first <= tcase.which && tcase.which < append_last)
        fname = "append";
    else if (assign_first <= tcase.which && tcase.which < assign_last)
        fname = "assign";
    else if (insert_first <= tcase.which && tcase.which < insert_last)
        fname ="insert";
    else if (replace_first <= tcase.which && tcase.which < replace_last)
        fname ="replace";

    switch (tcase.which) {
    case append_ptr:
    case assign_ptr:
        rw_asnprintf (&buf, &bufsize,
                      "%{+}%s (%{?}%{#*s}%{;}%{?}this->c_str ()%{;})",
                      fname, !self, int (tcase.arg_len), tcase.arg, self);
        break;

    case append_str:
    case assign_str:
        rw_asnprintf (&buf, &bufsize,
                      "%{+}%s (%{?}string (%{#*s})%{;}%{?}*this%{;})",
                      fname, !self, int (tcase.arg_len), tcase.arg, self);
        break;

    case append_ptr_size:
    case assign_ptr_size:
        rw_asnprintf (&buf, &bufsize, "%{+}%s ("
                      "%{?}%{#*s}%{;}%{?}this->c_str ()%{;}, %zu)",
                      fname, !self, int (tcase.arg_len), tcase.arg,
                      self, tcase.size);
        break;

    case append_str_off_size:
    case assign_str_off_size:
        rw_asnprintf (&buf, &bufsize, "%{+}%s ("
                      "%{?}string (%{#*s})%{;}%{?}*this%{;}, %zu, %zu)",
                      fname, !self, int (tcase.arg_len), tcase.arg, self,
                      tcase.off, tcase.size);
        break;

    case append_size_val:
    case assign_size_val:
        rw_asnprintf (&buf, &bufsize,
                      "%{+} %s (%zu, %#c)", tcase.size, tcase.val);
        break;

    case append_range:
    case assign_range:
        rw_asnprintf (&buf, &bufsize, "%{+}%s ("
                      "%{?}%{#*s}%{;}%{?}this->%{;}begin() + %zu, "
                      "%{?}%{#*s}%{;}%{?}this->%{;}.begin() + %zu)",
                      fname, !self, int (tcase.arg_len), tcase.arg,
                      self, tcase.off, !self, int (tcase.arg_len), tcase.arg,
                      self, tcase.off + tcase.size);
        break;

    case replace_off_size_ptr:
        rw_asnprintf (&buf, &bufsize, "%{+}%s ("
                      "%zu, %zu, %{?}%{#*s}%{;}%{?}this->c_str ()%{;})",
                      fname, tcase.off, tcase.size, !self, 
                      int (tcase.arg_len), tcase.arg, self);
        break;

    case replace_off_size_str:
        rw_asnprintf (&buf, &bufsize, "%{+}%s ("
                      "%zu, %zu, %{?}string (%{#*s})%{;}%{?}*this%{;})",
                      fname, tcase.off, tcase.size, !self, 
                      int (tcase.arg_len), tcase.arg, self);
        break;

    case replace_off_size_ptr_size:
        rw_asnprintf (&buf, &bufsize, "%{+}%s ("
                      "%zu, %zu, %{?}%{#*s}%{;}%{?}this->c_str ()%{;}, %zu)", 
                      fname, tcase.off, tcase.size, !self, 
                      int (tcase.arg_len), tcase.arg, self, tcase.size2);
        break;

    case replace_off_size_str_off_size:
        rw_asnprintf (&buf, &bufsize, "%{+}%s (%zu, %zu, "
                      "%{?}string (%{#*s})%{;}%{?}*this%{;}, %zu, %zu)",
                      fname, tcase.off, tcase.size, !self, 
                      int (tcase.arg_len), tcase.arg, self, 
                      tcase.off2, tcase.size2);
        break;

    case replace_off_size_size_val:
        rw_asnprintf (&buf, &bufsize, 
                      "%{+}%s (%zu, %zu, %zu, %#c)",
                      fname, tcase.off, tcase.size, tcase.size2, tcase.val);
        break;

    case replace_ptr:
        rw_asnprintf (&buf, &bufsize, "%{+}%s (begin() + %zu, begin() + %zu, "
                      "%{?}%{#*s}%{;}%{?}this->c_str ()%{;})",
                      fname, tcase.off, tcase.off + tcase.size, 
                      !self, int (tcase.arg_len), tcase.arg, self);
        break;

    case replace_str:
        rw_asnprintf (&buf, &bufsize, "%{+}%s (begin() + %zu, begin() + %zu, " 
                      "%{?}string (%{#*s})%{;}%{?}*this%{;})",
                      fname, tcase.off, tcase.off + tcase.size, 
                      !self, int (tcase.arg_len), tcase.arg, self);
        break;

    case replace_ptr_size:
        rw_asnprintf (&buf, &bufsize, "%{+}%s (begin() + %zu, begin() + %zu, " 
                      "%{?}%{#*s}%{;}%{?}this->c_str ()%{;}, %zu)", 
                      fname, tcase.off, tcase.off + tcase.size, !self, 
                      int (tcase.arg_len), tcase.arg, self, tcase.size2);
        break;

    case replace_size_val:
        rw_asnprintf (&buf, &bufsize, 
                      "%{+}%s (begin() + %zu, begin() + %zu, %zu, %#c)",
                      fname, tcase.off, tcase.off + tcase.size, 
                      tcase.size2, tcase.val);
        break;

    case replace_range:
        rw_asnprintf (&buf, &bufsize, "%{+}%s (begin() + %zu, begin() + %zu, "
                      "%{?}%{#*s}%{;}%{?}this->%{;}begin() + %zu, "
                      "%{?}%{#*s}%{;}%{?}this->%{;}begin() + %zu)", 
                      fname, tcase.off, tcase.off + tcase.size, !self, 
                      int (tcase.arg_len), tcase.arg, self, tcase.off2, !self, 
                      int (tcase.arg_len), tcase.arg, self, 
                      tcase.off2 + tcase.size2);
        break;

    default:
        RW_ASSERT (!"test logic error: unknown overload");
    }

    return buf;
}
