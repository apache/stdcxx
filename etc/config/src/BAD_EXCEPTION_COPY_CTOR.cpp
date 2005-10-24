// checking for bad_exception copy ctor

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif


#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE

namespace std {

void terminate ()
{
    static int *ip;

    if ((ip [0] = ip [1])) {   // force a SIGSEGV
        ++ip;
        terminate ();          // recurse infinitely
    }
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

#ifndef _RWSTD_NO_BAD_EXCEPTION_DEFAULT_CTOR

    bad_exception ();

#else

    bad_exception () { }

#endif   // _RWSTD_NO_BAD_EXCEPTION_DEFAULT_CTOR

#ifndef _RWSTD_NO_BAD_EXCEPTION_DTOR

    virtual ~bad_exception ();

#else

    virtual ~bad_exception () { }

#endif   // _RWSTD_NO_BAD_EXCEPTION_DEFAULT_CTOR

    bad_exception (const bad_exception&);
};

}   // namespace std


int main (int argc, char *argv[])
{
    // avoid executing the body of main unless explicitly requested
    // by specifying at least one command line argument (this foils
    // aggressive optimizers from eliminating the code)
    (void)&argv;
    if (argc < 2)
        return 0;

    std::bad_exception a;
    std::bad_exception b (a);

    (void)&a;
    (void)&b;

    // link only test
    return 0;
}
