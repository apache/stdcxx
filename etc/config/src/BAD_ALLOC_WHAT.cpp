// checking for bad_alloc::what()

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif


#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE

namespace std {

void terminate ()
{
    static int *ip;

    *ip++ = 0;      // force a SIGSEGV
    terminate ();   // recurse infinitely
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

    bad_alloc ();

    virtual ~bad_alloc ();

    virtual const char* what () const;
};

#ifdef _RWSTD_NO_BAD_ALLOC_DEFAULT_CTOR

bad_alloc::bad_alloc () { }

#endif   // _RWSTD_NO_BAD_ALLOC_DEFAULT_CTOR

#ifdef _RWSTD_NO_BAD_ALLOC_DTOR

bad_alloc::~bad_alloc () { }

#endif   // _RWSTD_NO_BAD_ALLOC_DEFAULT_CTOR

}   // namespace std


int main (int argc, char *argv[])
{
    // avoid executing the body of main unless explicitly requested
    // by specifying at least one command line argument (this foils
    // aggressive optimizers from eliminating the code)
    (void)&argv;
    if (argc < 2)
        return 0;

    std::bad_alloc e;

    // link only test
    return !e.what ();
}
