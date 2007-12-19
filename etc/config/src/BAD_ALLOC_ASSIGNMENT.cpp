// checking for bad_alloc assignment operator

/***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
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
 * Copyright 1999-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

// NOTE: this file is #included from the following tests,
//       each of which #defines bad_alloc to the name of
//       the corresponding exception class, and exercises
//       the respective class members
//
//   BAD_ALLOC_COPY_CTOR.cpp
//   BAD_ALLOC_DEFAULT_CTOR.cpp
//   BAD_ALLOC_DTOR.cpp

//   BAD_CAST_ASSIGNMENT.cpp
//   BAD_CAST_COPY_CTOR.cpp
//   BAD_CAST_DEFAULT_CTOR.cpp
//   BAD_CAST_DTOR.cpp

//   BAD_EXCEPTION_ASSIGNMENT.cpp
//   BAD_EXCEPTION_COPY_CTOR.cpp
//   BAD_EXCEPTION_DEFAULT_CTOR.cpp
//   BAD_EXCEPTION_DTOR.cpp

//   BAD_TYPEID_ASSIGNMENT.cpp
//   BAD_TYPEID_COPY_CTOR.cpp
//   BAD_TYPEID_DEFAULT_CTOR.cpp
//   BAD_TYPEID_DTOR.cpp

//   EXCEPTION_ASSIGNMENT.cpp
//   EXCEPTION_COPY_CTOR.cpp
//   EXCEPTION_DEFAULT_CTOR.cpp
//   EXCEPTION_DTOR.cpp

#include "config.h"


#if 2 == __GNUG__
#  ifndef _RWSTD_NO_HONOR_STD
#    ifdef _RWSTD_NO_STD_TERMINATE
#      include "terminate.h"
#    endif   // _RWSTD_NO_STD_TERMINATE
#  endif   // _RWSTD_NO_HONOR_STD
#endif   // gcc 2.x


#ifndef _RWSTD_NO_RUNTIME_IN_STD
#  define NAMESPACE(name)     namespace name
#else
#  define NAMESPACE(ignore)   extern "C++"
#  define std                 /* empty */
#endif   // _RWSTD_NO_RUNTIME_IN_STD


int ndefault_ctors;
int ncopy_ctors;
int ndtors;
int nassignments;


NAMESPACE (std) {

class bad_alloc
{
public:
    const void *what_;

#if defined (TEST_DEFAULT_CTOR)

    bad_alloc ();

#else   // if !defined (TEST_DEFAULT_CTOR)

    // provide a default ctor but with a different signature
    // than the one above to prevent multiply defined symbol
    // linker errors in case that one is defined in the
    // runtime library
    bad_alloc (int = 0) { }

#  if defined (TEST_DTOR)

    virtual ~bad_alloc ();

#  elif defined (TEST_COPY_CTOR)

    bad_alloc (const bad_alloc&);

#  elif defined (TEST_WHAT)

    virtual const char* what () const;

#  else   // test assignment

#    if !defined (TEST_ASSIGNMENT)
#      define TEST_ASSIGNMENT
#    endif

    bad_alloc& operator= (const bad_alloc&);

    bad_alloc& assign (const bad_alloc &rhs) {
        ++nassignments;
        return *this = rhs;
    }

#  endif   // TEST_...
#endif   // !TEST_DEFAULT_CTOR

};


}   // namespace std


// use virtual inheritance here to prevent aggressive optimizers
// (like VisualAge C++) from optimizing the base class dtor away
struct Base0: virtual std::bad_alloc
{
    int index0;

    typedef std::bad_alloc Base;

    Base0 ();

    Base0 (const Base0&);

    virtual ~Base0 ();

    Base0& operator= (const Base0&);

    virtual const char* what () const;
};


struct Base1: virtual std::bad_alloc
{
    int index1;

    typedef std::bad_alloc Base;

    Base1 ();

    Base1 (const Base1&);

    virtual ~Base1 ();

    Base1& operator= (const Base1&);

    virtual const char* what () const;
};


int which_what;

struct Derived: Base0, Base1
{
    typedef std::bad_alloc Base;

    Derived ();

    Derived (const Derived&);

    virtual ~Derived ();

    Derived& operator= (const Derived&);

    virtual const char* what () const;
};


#if defined (TEST_COPY_CTOR)   ///////////////////////////////////////

int test_member (int argc, void *ptr)
{
    // the code below must not be executed

#  ifndef _RWSTD_NO_EXCEPTIONS

    try {
        try {
            if (1 < argc) {
                // jump through hoops to try to foil
                // even the most aggressive optimizers
                Derived        *pd  = (Derived*)ptr;
                Base0          *pb0 = (Derived*)ptr + argc;
                Base1          *pb1 = (Derived*)ptr + argc + 1;
                std::bad_alloc *pba = (Derived*)ptr + argc + 2;

                if (argc < 6)
                    throw *pd;
                if (argc < 7)
                    throw *pb0;
                if (argc < 8)
                    throw *pb1;
                if (argc < 9)
                    throw *pba;
            }
        }
        catch (Derived e) {
            e.index0 = e.index1 = __LINE__;
            throw e;
        }
        catch (Base0 b0) {
            b0.index0 = __LINE__;
            throw b0;
        }
        catch (Base1 b1) {
            b1.index1 = __LINE__;
            throw b1;
        }
        catch (std::bad_alloc e) {

            std::bad_alloc copy (e);

            if (argc < 5)
                throw copy;

            if (argc < 4)
                throw e;

            if (argc < 3)
                throw;
        }
    }
    catch (std::bad_alloc e) {
        return 0 != e.what_;
    }
    catch (...) {
        return 2;
    }

    return 3;

#  else   // if defined (_RWSTD_NO_EXCEPTIONS)

    std::bad_alloc *pe = (std::bad_alloc*)ptr;
    std::bad_alloc *pf = new std::bad_alloc (*pe);

    return !(1 == ndefault_ctors && 1 == ncopy_ctors && pe->what_ == pf->what_);

#  endif   // _RWSTD_NO_EXCEPTIONS

}

#elif defined (TEST_DEFAULT_CTOR)   //////////////////////////////////

int test_member (int, void*)
{
    // the code below must not be executed

    std::bad_alloc *pe1 = new std::bad_alloc;
    std::bad_alloc *pe2 = new Derived;

    return pe1 < pe2;
}

#elif defined (TEST_DTOR)   //////////////////////////////////////////

int test_member (int argc, void *ptr)
{
    // the code below must not be executed

    std::bad_alloc *pe;

    if (1 < argc)
        pe = (Derived*)ptr;
    else
        pe = (std::bad_alloc*)ptr;

    delete pe;

    return !(1 < argc ? 1 == ndtors : 0 == ndtors);
}

#elif defined (TEST_WHAT)   //////////////////////////////////////////

int test_member (int argc, void *ptr)
{
    // the code below must not be executed

    std::bad_alloc *pe = (std::bad_alloc*)ptr;
    Derived        *pd = (Derived*)ptr + 1;

    which_what = argc;

    return pe [0].what () == pd [argc].what ();
}

#else   // test assignment   /////////////////////////////////////////

int test_member (int argc, void *ptr)
{
    // the code below must not be executed

    std::bad_alloc *pe = (Derived*)ptr;

    std::bad_alloc& (std::bad_alloc::*pf)(const std::bad_alloc&) =
        argc < 3 ? &std::bad_alloc::operator= : &std::bad_alloc::assign;

    which_what = argc;

    pe [argc] = pe [argc + 1];
    (pe [argc + 1].*pf)(pe [argc + 2]);

    return pe [0].what_ == pe [1].what_ || 2 < nassignments;
}


#endif   // TEST_...   ///////////////////////////////////////////////


int main (int argc, char *argv[])
{
    (void)&argv;
    (void)&nassignments;
    (void)&ncopy_ctors;
    (void)&ndefault_ctors;

    // avoid executing the body of main unless explicitly requested
    // by specifying at least one command line argument (this foils
    // aggressive optimizers from eliminating the code)
    if (argc < 2)
        return 0;

    return test_member (argc, argv);
}


Base0::Base0 (): Base (), index0 (++ndefault_ctors) { }


Base0::Base0 (const Base0 &rhs)

#if defined (TEST_COPY_CTOR)
    : Base (rhs),
#else   // if defined (TEST_COPY_CTOR)
    : Base (),
#endif   // TEST_COPY_CTOR
      index0 (rhs.index0)
{
    (void)&rhs;   // silence unused argument warnings

    ++ncopy_ctors;
}


Base0::~Base0 ()
{
    ++ndtors;
    index0 = 0;
}


Base0& Base0::operator=(const Base0 &rhs)
{
    ++nassignments;
    index0 = rhs.index0;

#if defined (TEST_ASSIGNMENT)

    Base::operator= (rhs);

#endif   // TEST_ASSIGNMENT

    return *this;
}


const char* Base0::what () const
{
#if defined (TEST_WHAT)

    if (1 < which_what++)
        return Base::what ();

#endif   // TEST_WHAT

    return "Base0";


}


Base1::Base1 (): Base (), index1 (++ndefault_ctors) { }


Base1::Base1 (const Base1 &rhs)

#if defined (TEST_COPY_CTOR)
    : Base (rhs),
#else   // if defined (TEST_COPY_CTOR)
    : Base (),
#endif   // TEST_COPY_CTOR
      index1 (rhs.index1)
{
    (void)&rhs;   // silence unused argument warnings

    ++ncopy_ctors;
}


Base1::~Base1 ()
{
    ++ndtors;
    index1 = 0;
}


const char* Base1::what () const
{
#if defined (TEST_WHAT)

    if (2 < which_what++)
        return Base::what ();

#endif   // TEST_WHAT

    return "Base1";
}


Base1& Base1::operator=(const Base1 &rhs)
{
    ++nassignments;
    index1 = rhs.index1;

#if defined (TEST_ASSIGNMENT)

    Base::operator= (rhs);

#endif   // TEST_ASSIGNMENT

    return *this;
}


Derived::Derived ()
    : Base (), Base0 (), Base1 ()
{
    ++ndefault_ctors;
}


Derived::Derived (const Derived &rhs)

#if defined (TEST_COPY_CTOR)
    : Base (rhs), Base0 (rhs), Base1 (rhs)
#else   // if defined (TEST_COPY_CTOR)
    : Base (), Base0 (), Base1 ()
#endif   // TEST_COPY_CTOR

{
    (void)&rhs;   // silence unused argument warnings

    ++ncopy_ctors;
}


Derived::~Derived ()
{
    ++ndtors;
}


Derived& Derived::operator=(const Derived &rhs)
{
    ++nassignments;

    Base0::operator= (rhs);
    Base1::operator= (rhs);

    return *this;
}


const char* Derived::what () const
{
    if (3 < which_what++)
        return Base1::what ();

    return Base0::what ();
}
