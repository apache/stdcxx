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


char* StringMembers::
format (charT cid, Traits tid, Allocator aid, MemberFunction which,
        const TestCase &tcase)
{
    char*  buf     = 0;
    size_t bufsize = 0;

    const bool self = 0 == tcase.arg;

    if (   DefaultTraits == tid
        && (Char == cid || WChar == cid))
        rw_asnprintf (&buf, &bufsize,
                      "std::%{?}w%{;}string (%{?}%{#*s}%{;}).",
                      WChar == cid,
                      tcase.str != 0, int (tcase.str_len), tcase.str);
    else
        rw_asnprintf (&buf, &bufsize,
                      "std::basic_string<%s, %s<%1$s>, %s<%1$s>>"
                      "(%{?}%{#*s}%{;}).",
                      char_names [cid],
                      traits_names [tid],
                      allocator_names [aid],
                      tcase.str != 0, int (tcase.str_len), tcase.str);

    const char *fname = "assign";

    switch (which) {
    case append_ptr:
        fname = "append";
    case assign_ptr:
        rw_asnprintf (&buf, &bufsize,
                      "%{+}%s (%{?}%{#*s}%{;}%{?}this->c_str ()%{;})",
                      fname, !self, int (tcase.arg_len), tcase.arg, self);
        break;

    case append_str:
        fname = "append";
    case assign_str:
        rw_asnprintf (&buf, &bufsize,
                      "%{+}%s (%{?}string (%{#*s})%{;}%{?}*this%{;})",
                      fname, !self, int (tcase.arg_len), tcase.arg, self);
        break;

    case append_ptr_size:
        fname = "append";
    case assign_ptr_size:
        rw_asnprintf (&buf, &bufsize, "%{+}%s ("
                      "%{?}%{#*s}%{;}%{?}this->c_str ()%{;}, %zu)",
                      fname, !self, int (tcase.arg_len), tcase.arg,
                      self, tcase.size);
        break;

    case append_str_off_size:
        fname = "append";
    case assign_str_off_size:
        rw_asnprintf (&buf, &bufsize, "%{+}%s ("
                      "%{?}string (%{#*s})%{;}%{?}*this%{;}, %zu, %zu)",
                      fname, !self, int (tcase.arg_len), tcase.arg, self,
                      tcase.off, tcase.size);
        break;

    case append_size_val:
        fname = "append";
    case assign_size_val:
        rw_asnprintf (&buf, &bufsize,
                      "%{+} %s (%zu, %#c)", tcase.size, tcase.val);
        break;

    case append_range:
        fname = "append";
    case assign_range:
        rw_asnprintf (&buf, &bufsize, "%{+}%s ("
                      "%{?}%{#*s}%{;}%{?}*this%{;}.begin + %zu, "
                      "%{?}%{#*s}%{;}%{?}*this%{;}.begin + %zu)",
                      fname, !self, int (tcase.arg_len), tcase.arg,
                      self, tcase.off, !self, int (tcase.arg_len), tcase.arg,
                      self, tcase.off + tcase.size);
        break;

    default:
        RW_ASSERT (!"test logic error: unknown overload");
    }

    return buf;
}
