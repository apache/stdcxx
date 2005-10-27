// checking for bad_alloc dtor

// NOTE: this file is #included from the following tests,
//       each of which #defines bad_alloc to the name of
//       the corresponding exception class:
//   BAD_CAST_DTOR.cpp
//   BAD_EXCEPTION_DTOR.cpp
//   BAD_TYPEID_DTOR.cpp
//   EXCEPTION_DTOR.cpp

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif


#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE

namespace std {

void terminate ()
{
    static int *ip;

    if ((ip [0] = ip [1])) {  // force a SIGSEGV
        ++ip;
        terminate ();         // recurse infinitely
    }
}

}

#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


#ifndef _RWSTD_NO_STD_EXCEPTION
#  define NAMESPACE(name)   namespace name
#else
#  ifndef _RWSTD_NO_GLOBAL_EXCEPTION
#    define NAMESPACE(ignore)   extern "C++"
#    define std                 /* empty */
#  else
#    ifndef _RWSTD_NO_RUNTIME_IN_STD
#      define NAMESPACE(name)   namespace name
#    else
#      define NAMESPACE(ignore)   extern "C++"
#      define std                 /* empty */
#    endif   // _RWSTD_NO_RUNTIME_IN_STD
#  endif   // _RWSTD_NO_GLOBAL_EXCEPTION
#endif   // _RWSTD_NO_STD_EXCEPTION


#if !defined (bad_alloc)
#  ifdef _RWSTD_NO_BAD_ALLOC_DEFAULT_CTOR
#    define NO_DEFAULT_CTOR
#  endif   // _RWSTD_NO_BAD_ALLOC_DEFAULT_CTOR
#endif


NAMESPACE (std) {

class bad_alloc
{
public:

#  if !defined (NO_DEFAULT_CTOR)

    // defined in the language support library
    bad_alloc ();

#  else   // if defined (NO_DEFAULT_CTOR)

    bad_alloc () { }

#  endif   // NO_DEFAULT_CTOR

    virtual ~bad_alloc ();   // not defined here
};


}   // namespace std


// use virtual inheritance here to prevent aggressive optimizers
// (like VisualAge C++) from optimizing the base class dtor away
struct B0: virtual std::bad_alloc { };
struct B1: virtual std::bad_alloc { };

struct Derived: B0, B1
{
    virtual ~Derived ();
};


int dtor;

// other tests that #include this file #define main to a function
// with a different name (such as test_bad_cast_dtor) and call it
// from their main()
int main (int argc, char *argv[])
{
    // avoid executing the body of main unless explicitly requested
    // by specifying at least one command line argument (this foils
    // aggressive optimizers from eliminating the code)
    (void)&argv;
    if (argc < 2)
        return 0;

    // try to prevent the compiler from optimizing the dtor call away
    std::bad_alloc *ptr;

    if (1 < argc)
        ptr = new Derived;
    else
        ptr = new std::bad_alloc;

    delete ptr;

    return !(1 < argc ? 1 == dtor : 0 == dtor);
}


Derived::~Derived () { ++dtor; }
