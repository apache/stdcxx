/***************************************************************************
 *
 * 23.vector.capacity.cpp - test exercising [lib.vector.capacity]
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

#include <stdexcept>   // for length_error
#include <vector>      // for vector

#include <alg_test.h>
#include <driver.h>

/**************************************************************************/

static unsigned rw_opt_nloops = 1024;
static int      rw_opt_no_exceptions;

/**************************************************************************/

typedef std::vector<X, std::allocator<X> > Vector;

// exercise vector<>::capacity() and vector<>::reserve()
// focus on the complexity of the function
void test_capacity (Vector::size_type nelems)
{
    // create a vector with nelems elements
    Vector v (nelems);

    const Vector::size_type     cap   = v.capacity ();
    const Vector::size_type     size  = v.size ();
          Vector::const_pointer begin = size ? &v [0] : 0;

    X::reset_totals ();

    // call reserve capacity that is less than or equal to the current value
    v.reserve (cap / 2);

    // verify that the call had no effect
    rw_assert (v.capacity () == cap, 0, __LINE__,
               "vector<X>(%zu).reserve(%zu); capacity() == %zu, got %zu",
               nelems, cap / 2, cap, v.capacity ());

    if (size) {
        // verify that no reallocation took place
        rw_assert (begin == &v [0], 0, __LINE__,
                   "vector<X>(%zu).reserve(%zu) unexpectedly reallocated",
                   nelems, cap / 2);
    }

    // call reserve the same capacity as the current value
    v.reserve (cap);

    // verify that the call had no effect
    rw_assert (v.capacity () == cap, 0, __LINE__,
               "vector<X>(%zu).reserve(%zu); capacity() == %zu, got %zu",
               nelems, cap / 2, cap, v.capacity ());

    if (size) {
        // verify that no reallocation took place
        rw_assert (begin == &v [0], 0, __LINE__,
                   "vector<X>(%zu).reserve(%zu) unexpectedly reallocated",
                   nelems, cap / 2);
    }

    // call reserve with a larger capacity then is available
    v.reserve (cap + 1);

    // 23.2.4.2, p2: After reserve (), capacity () is greater or equal
    //               to the reserve value if reallocation happens
    rw_assert (v.capacity () >= cap + 1, 0, __LINE__,
               "vector<X>(%zu).reserve(%zu); capacity() > %zu, got %zu",
               nelems, cap + 1, cap, v.capacity ());

    // 23.2.3.2, p3: reserve shall not change the size of the sequence
    rw_assert (v.size () == size, 0, __LINE__,
               "vector<X>(%zu).reserve(); size() == %zu, got %zu",
               nelems, size, v.size ());

    // 23.2.3.2, p3: takes at most linear time in the size of the sequence
    rw_assert (X::n_total_copy_ctor_ == v.size (), 0, __LINE__,
               "vector<X>(%zu).reserve(%zu) complexity: "
               "copy ctor called %zu times when size() = %zu",
               nelems, cap + 1, X::n_total_copy_ctor_, v.size ());

    if (size) {
        begin = &v [0];
            
        // verify 23.2.4.2, p5: no reallocation takes place until
        // the size of the container would exceed its capacity
        for (Vector::size_type i = 0; i != v.capacity () - size; ++i) {
            v.push_back (X ());

            rw_assert (begin == &v [0], 0, __LINE__,
                       "vector<X>(%zu).reserve(%zu); insertion of element "
                       "%zu unexpectedly reallocated; size() = %zu",
                       nelems, cap + 1, v.size ());
        }
    }

    if (rw_opt_no_exceptions)
        return;

#ifndef _RWSTD_NO_EXCEPTIONS

    // exercise reserve exception
    const char *caught = 0;

    Vector::size_type too_much = v.max_size () + 1;

    if (!too_much)
        too_much = v.max_size ();

    try {
        v.reserve (too_much);
    }
    catch (std::length_error) {
        caught = "";
    }
    catch (...) {
        caught = "unknown exception";
    }

    rw_assert (0 != caught, 0, __LINE__,
               "vector<X>(%zu).reserve(%zu) expected exception not thrown",
               nelems, too_much);

    if (caught)
        rw_assert ('\0' == *caught, 0, __LINE__,
                   "vector<X>(%zu).reserve(%zu) expected length_error, got %s",
                   nelems, too_much, caught);

#endif // _RWSTD_NO_EXCEPTIONS

}

/**************************************************************************/

static int
run_test (int /* argc */, char** /* argv */)
{
    rw_info (0, 0, 0, "std::vector<X>::capacity() const");
    rw_info (0, 0, 0, "std::vector<X>::reserve(size_type)");

    const Vector::size_type max_elems = Vector::size_type (rw_opt_nloops);

    rw_note (0 == rw_opt_no_exceptions, 0, 0, "exception test disabled");

    for (Vector::size_type i = 0; i < max_elems; ++i) {
        test_capacity (i);
    }

    return 0;
}

/**************************************************************************/

int main (int argc, char** argv)
{
    return rw_test (argc, argv, __FILE__,
                    "lib.vector.capacity",
                    0 /* no comment */,
                    run_test,
                    "|-no-exceptions# "
                    "|-nloops#0",
                    &rw_opt_no_exceptions,
                    &rw_opt_nloops);
}
