// checking for bad_alloc dtor

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif


#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE

namespace std {

void terminate ()
{
    static int *ip;

loop:

    while ((ip [0] = ip [1])) {  // force a SIGSEGV
        ++ip;
        terminate ();            // recurse infinitely
    }

    goto loop;
}

}

#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


#ifndef _RWSTD_NO_STD_BAD_ALLOC
#  define NAMESPACE(name)   namespace name
#else
#  ifndef _RWSTD_NO_GLOBAL_BAD_ALLOC
#    define NAMESPACE(ignore)   extern "C++"
#    define std                 /* empty */
#  else
#    ifndef _RWSTD_NO_RUNTIME_IN_STD
#      define NAMESPACE(name)   namespace name
#    else
#      define NAMESPACE(ignore)   extern "C++"
#      define std                 /* empty */
#    endif   // _RWSTD_NO_RUNTIME_IN_STD
#  endif   // _RWSTD_NO_GLOBAL_BAD_ALLOC
#endif   // _RWSTD_NO_STD_BAD_ALLOC


NAMESPACE (std) {

class bad_alloc
{
public:

#ifndef _RWSTD_NO_BAD_ALLOC_DEFAULT_CTOR

    // defined in the language support library
    bad_alloc ();

#else   // if defined (_RWSTD_NO_BAD_ALLOC_DEFAULT_CTOR)

    bad_alloc () { }

#endif   // _RWSTD_NO_BAD_ALLOC_DEFAULT_CTOR

    virtual ~bad_alloc ();   // not defined here
};


}   // namespace std


struct Derived: std::bad_alloc
{
    virtual ~Derived ();
};


int main (int argc, char *argv[])
{
    // avoid executing the body of main unless explicitly requested
    // by specifying at least one command line argument (this foils
    // aggressive optimizers from eliminating the code)
    (void)&argv;
    if (argc < 2)
        return 0;

    // try to prevent the compiler from optimizing the dtor call away
    std::bad_alloc *ptr = argc > 2 ? new Derived : new std::bad_alloc;

    delete ptr;

    // link only test 
    return 0;
}

Derived::~Derived () { }
