// -*- C++ -*-
/***************************************************************************
 *
 * 20.meta.trans.cv.cpp - test exercising meta.trans.cv
 *
 * $Id$
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
 * Copyright 1999-2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#include <rw_driver.h>
#include <rw/_defs.h>

// compile out all test code if extensions disabled
#ifndef _RWSTD_NO_EXT_CXX_0X

#include <type_traits>

/**************************************************************************/

template <class T, class U>
struct test_is_same
{
    enum { value = 0 };
};

template <class T>
struct test_is_same<T,T>
{
    enum { value = 1 };
};

/**************************************************************************/

void test_trait (int line, bool same,
                 const char* trait,
                 const char* typeT,
                 const char* typeU)
{
    rw_assert (same, 0, line,
               "%s<%s>::type is %{?}exactly%{:}not%{;} %s",
               trait, typeT, same, typeU);
}

#define TEST(Trait,TypeT,TypeU)                                      \
    test_trait(__LINE__,                                             \
               std::is_same<Trait<TypeT>::type, TypeU>::value,       \
               #Trait, #TypeT, #TypeU)

/**************************************************************************/

#define C  const
#define V  volatile
#define CV const volatile

/**************************************************************************/

static void test_remove_const ()
{
    TEST (std::remove_const,    int,   int);
    TEST (std::remove_const, C  int,   int);
    TEST (std::remove_const,  V int, V int);
    TEST (std::remove_const, CV int, V int);
    
    TEST (std::remove_const,    int*,    int*);
    TEST (std::remove_const, C  int*, C  int*);
    TEST (std::remove_const,  V int*,  V int*);
    TEST (std::remove_const, CV int*, CV int*);
    
    TEST (std::remove_const,    int* C ,    int*  );
    TEST (std::remove_const,    int*  V,    int* V);
    TEST (std::remove_const,    int* CV,    int* V);

    TEST (std::remove_const, C  int* C , C  int*  );
    TEST (std::remove_const, C  int*  V, C  int* V);
    TEST (std::remove_const, C  int* CV, C  int* V);

    TEST (std::remove_const,  V int* C ,  V int*  );
    TEST (std::remove_const,  V int*  V,  V int* V);
    TEST (std::remove_const,  V int* CV,  V int* V);

    TEST (std::remove_const, CV int* C , CV int*  );
    TEST (std::remove_const, CV int*  V, CV int* V);
    TEST (std::remove_const, CV int* CV, CV int* V);
}

static void test_remove_volatile ()
{
    TEST (std::remove_volatile,    int,    int);
    TEST (std::remove_volatile, C  int, C  int);
    TEST (std::remove_volatile,  V int,    int);
    TEST (std::remove_volatile, CV int, C  int);
    
    TEST (std::remove_volatile,    int*,    int*);
    TEST (std::remove_volatile, C  int*, C  int*);
    TEST (std::remove_volatile,  V int*,  V int*);
    TEST (std::remove_volatile, CV int*, CV int*);
    
    TEST (std::remove_volatile,    int* C ,    int* C);
    TEST (std::remove_volatile,    int*  V,    int*  );
    TEST (std::remove_volatile,    int* CV,    int* C);

    TEST (std::remove_volatile, C  int* C , C  int* C);
    TEST (std::remove_volatile, C  int*  V, C  int*  );
    TEST (std::remove_volatile, C  int* CV, C  int* C);

    TEST (std::remove_volatile,  V int* C ,  V int* C);
    TEST (std::remove_volatile,  V int*  V,  V int*  );
    TEST (std::remove_volatile,  V int* CV,  V int* C);

    TEST (std::remove_volatile, CV int* C , CV int* C);
    TEST (std::remove_volatile, CV int*  V, CV int*  );
    TEST (std::remove_volatile, CV int* CV, CV int* C);
}

static void test_remove_cv ()
{
    TEST (std::remove_cv,    int, int);
    TEST (std::remove_cv, C  int, int);
    TEST (std::remove_cv,  V int, int);
    TEST (std::remove_cv, CV int, int);
    
    TEST (std::remove_cv,    int*,    int*);
    TEST (std::remove_cv, C  int*, C  int*);
    TEST (std::remove_cv,  V int*,  V int*);
    TEST (std::remove_cv, CV int*, CV int*);
    
    TEST (std::remove_cv,    int* C , int*);
    TEST (std::remove_cv,    int*  V, int*);
    TEST (std::remove_cv,    int* CV, int*);

    TEST (std::remove_cv, C  int* C , C  int*);
    TEST (std::remove_cv, C  int*  V, C  int*);
    TEST (std::remove_cv, C  int* CV, C  int*);

    TEST (std::remove_cv,  V int* C ,  V int*);
    TEST (std::remove_cv,  V int*  V,  V int*);
    TEST (std::remove_cv,  V int* CV,  V int*);

    TEST (std::remove_cv, CV int* C , CV int*);
    TEST (std::remove_cv, CV int*  V, CV int*);
    TEST (std::remove_cv, CV int* CV, CV int*);
}

/**************************************************************************/

static void test_add_const ()
{
    TEST (std::add_const,    int, C  int);
    TEST (std::add_const, C  int, C  int);
    TEST (std::add_const,  V int, CV int);
    TEST (std::add_const, CV int, CV int);
    
    TEST (std::add_const,    int*,    int* C);
    TEST (std::add_const, C  int*, C  int* C);
    TEST (std::add_const,  V int*,  V int* C);
    TEST (std::add_const, CV int*, CV int* C);
    
    TEST (std::add_const,    int* C ,    int* C);
    TEST (std::add_const,    int*  V,    int* CV);
    TEST (std::add_const,    int* CV,    int* CV);

    TEST (std::add_const, C  int* C , C  int* C );
    TEST (std::add_const, C  int*  V, C  int* CV);
    TEST (std::add_const, C  int* CV, C  int* CV);

    TEST (std::add_const,  V int* C ,  V int* C );
    TEST (std::add_const,  V int*  V,  V int* CV);
    TEST (std::add_const,  V int* CV,  V int* CV);

    TEST (std::add_const, CV int* C , CV int* C );
    TEST (std::add_const, CV int*  V, CV int* CV);
    TEST (std::add_const, CV int* CV, CV int* CV);

    TEST (std::add_const, void (*)(), void (*C)());

    // if T is a reference, then type shall be the same as T
    TEST (std::add_const,    int&,    int&);
    TEST (std::add_const, C  int&, C  int&);
    TEST (std::add_const,  V int&,  V int&);
    TEST (std::add_const, CV int&, CV int&);

    TEST (std::add_const, void (&)(), void (&)());

    // if T is a function, then type shall be the same as T
    TEST (std::add_const, void    (), void    ());
}

static void test_add_volatile ()
{
    TEST (std::add_volatile,    int,  V int);
    TEST (std::add_volatile, C  int, CV int);
    TEST (std::add_volatile,  V int,  V int);
    TEST (std::add_volatile, CV int, CV int);
    
    TEST (std::add_volatile,    int*,    int*  V);
    TEST (std::add_volatile, C  int*, C  int*  V);
    TEST (std::add_volatile,  V int*,  V int*  V);
    TEST (std::add_volatile, CV int*, CV int*  V);
    
    TEST (std::add_volatile,    int* C ,    int* CV);
    TEST (std::add_volatile,    int*  V,    int*  V);
    TEST (std::add_volatile,    int* CV,    int* CV);

    TEST (std::add_volatile, C  int* C , C  int* CV);
    TEST (std::add_volatile, C  int*  V, C  int*  V);
    TEST (std::add_volatile, C  int* CV, C  int* CV);

    TEST (std::add_volatile,  V int* C ,  V int* CV);
    TEST (std::add_volatile,  V int*  V,  V int*  V);
    TEST (std::add_volatile,  V int* CV,  V int* CV);

    TEST (std::add_volatile, CV int* C , CV int* CV);
    TEST (std::add_volatile, CV int*  V, CV int*  V);
    TEST (std::add_volatile, CV int* CV, CV int* CV);

    TEST (std::add_volatile, void (*)(), void (*V)());

    // if T is a reference, then type shall be the same as T
    TEST (std::add_volatile,    int&,    int&);
    TEST (std::add_volatile, C  int&, C  int&);
    TEST (std::add_volatile,  V int&,  V int&);
    TEST (std::add_volatile, CV int&, CV int&);

    TEST (std::add_volatile, void (&)(), void (&)());

    // if T is a function, then type shall be the same as T
    TEST (std::add_volatile, void    (), void    ());
}

static void test_add_cv ()
{
    TEST (std::add_cv,    int, CV int);
    TEST (std::add_cv, C  int, CV int);
    TEST (std::add_cv,  V int, CV int);
    TEST (std::add_cv, CV int, CV int);
    
    TEST (std::add_cv,    int*,    int* CV);
    TEST (std::add_cv, C  int*, C  int* CV);
    TEST (std::add_cv,  V int*,  V int* CV);
    TEST (std::add_cv, CV int*, CV int* CV);
    
    TEST (std::add_cv,    int* C ,    int* CV);
    TEST (std::add_cv,    int*  V,    int* CV);
    TEST (std::add_cv,    int* CV,    int* CV);

    TEST (std::add_cv, C  int* C , C  int* CV);
    TEST (std::add_cv, C  int*  V, C  int* CV);
    TEST (std::add_cv, C  int* CV, C  int* CV);

    TEST (std::add_cv,  V int* C ,  V int* CV);
    TEST (std::add_cv,  V int*  V,  V int* CV);
    TEST (std::add_cv,  V int* CV,  V int* CV);

    TEST (std::add_cv, CV int* C , CV int* CV);
    TEST (std::add_cv, CV int*  V, CV int* CV);
    TEST (std::add_cv, CV int* CV, CV int* CV);

    TEST (std::add_cv, void (*)(), void (*CV)());

    // if T is a reference, then type shall be the same as T
    TEST (std::add_cv,    int&,    int&);
    TEST (std::add_cv, C  int&, C  int&);
    TEST (std::add_cv,  V int&,  V int&);
    TEST (std::add_cv, CV int&, CV int&);

    TEST (std::add_cv, void (&)(), void (&)());

    // if T is a function, then type shall be the same as T
    TEST (std::add_cv, void    (), void    ());
}

/**************************************************************************/

static int run_test (int, char*[])
{
    test_remove_const();
    test_remove_volatile();
    test_remove_cv();

    test_add_const();
    test_add_volatile();
    test_add_cv();

    return 0;
}

/**************************************************************************/

#else // _RWSTD_NO_EXT_CXX_0X

/**************************************************************************/

static int run_test (int, char*[])
{
    rw_warn (0, 0, __LINE__,
             "test disabled because _RWSTD_NO_EXT_CXX_0X is defined");
    return 0;
}

#endif // !_RWSTD_NO_EXT_CXX_0X

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "meta.trans.cv",
                    0 /* no comment */,
                    run_test,
                    0);
}
