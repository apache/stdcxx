/***************************************************************************
 *
 * 20.forward.cpp - tests exercising move/forward helpers
 *
 * $Id: 20.forward.cpp 673865 2008-07-03 23:26:24Z vitek $
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 * Copyright 2008 Rogue Wave Software.
 * 
 **************************************************************************/

#include <rw_driver.h>
#include <rw/_defs.h>

// compile out all test code if extensions disabled
#ifndef _RWSTD_NO_EXT_CXX_0X

#include <rw/_forward.h>
#include <rw/_meta_rel.h>
#include <rw/_static_assert.h>



struct Foo
{
   Foo (int& ) { }
};


static void
test_identity ()
{
    rw_info (0, __FILE__, __LINE__, "std::identity<T> class template");

    typedef std::identity<Foo> FooIdent;
    typedef std::identity<Foo>::type FooIdentType;

    _RWSTD_STATIC_ASSERT ((_RW::__rw_is_same<Foo, FooIdentType>::value),
                          "is_same<Foo, FooIdentType>::value is false, "
                          "expected true");

    int i = 1;
    FooIdent foo_ident;
    Foo foo = foo_ident (i);

}

/**************************************************************************/

#ifndef _RWSTD_NO_RVALUE_REFERENCES

// using example from standard as a test case

template <class Type>
class shared_ptr
{
    Type* ptr_;

public:
    shared_ptr (Type* ptr): ptr_ (ptr) { }
    ~shared_ptr () { delete ptr_; }
};

template <class Type, class AType>
shared_ptr<Type> factory (AType&& at)
{
    return shared_ptr<Type> (new Type (std::forward<AType> (at)));
}

/**************************************************************************/

static void
test_forward ()
{
    rw_info (0, __FILE__, __LINE__, "std::forward() function");

}

/**************************************************************************/

static void
test_move ()
{
    rw_info (0, __FILE__, __LINE__, "std::move() function");

}

#endif // _RWSTD_NO_RVALUE_REFERENCES

/**************************************************************************/

static int
run_test (int /*unused*/, char* /*unused*/ [])
{
    test_identity ();

#if !defined _RWSTD_NO_RVALUE_REFERENCES

    test_forward ();
    test_move ();

#else // no rvalue references

    rw_warn (0, 0, __LINE__,
             "No compiler support for rvalue references; tests disabled.");

#endif   // !defined _RWSTD_NO_RVALUE_REFERENCES

    return 0;
}

#else // !_RWSTD_NO_EXT_CXX_0X

static int run_test (int, char*[])
{
    rw_warn (0, 0, __LINE__,
             "test disabled because _RWSTD_NO_EXT_CXX_0X is defined");
    return 0;
}

#endif // !_RWSTD_NO_EXT_CXX_0X

/*extern*/ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "[forward]", "20.2.2  forward/move helpers",
                    run_test, "", 0);
}

