// checking for bad_exception default ctor

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

}   // namespace std

#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


#ifndef _RWSTD_NO_STD_BAD_EXCEPTION
#  define NAMESPACE(name)   namespace name
#else
#  ifndef _RWSTD_NO_GLOBAL_BAD_EXCEPTION
#    define NAMESPACE(ignore)   extern "C++"
#    define std                 /* empty */
#  else
#    ifndef _RWSTD_NO_RUNTIME_IN_STD
#      define NAMESPACE(name)   namespace name
#    else
#      define NAMESPACE(ignore)   extern "C++"
#      define std                 /* empty */
#    endif   // _RWSTD_NO_RUNTIME_IN_STD
#  endif   // _RWSTD_NO_GLOBAL_BAD_EXCEPTION
#endif   // _RWSTD_NO_STD_BAD_EXCEPTION


NAMESPACE (std) {

class bad_exception
{
public:
    bad_exception ();
};

}   // namespace std


void* foo ()
{
    // construct but do not destroy or copy
    // to avoid calling the dtor or copy ctor
    return new std::bad_exception ();
}


int main (int argc, char *argv[])
{
    // avoid executing the body of main unless explicitly requested
    // by specifying at least one command line argument (this foils
    // aggressive optimizers from eliminating the code)
    (void)&argv;
    if (argc < 2)
        return 0;

    // link only test
    return argv [0] == foo ();
}
