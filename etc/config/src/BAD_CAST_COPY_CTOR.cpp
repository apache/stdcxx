// checking for bad_cast copy ctor

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


#ifndef _RWSTD_NO_STD_BAD_CAST
#  define NAMESPACE(name)   namespace name
#else
#  ifndef _RWSTD_NO_GLOBAL_BAD_CAST
#    define NAMESPACE(ignore)   extern "C++"
#    define std                 /* empty */
#  else
#    ifndef _RWSTD_NO_RUNTIME_IN_STD
#      define NAMESPACE(name)   namespace name
#    else
#      define NAMESPACE(ignore)   extern "C++"
#      define std                 /* empty */
#    endif   // _RWSTD_NO_RUNTIME_IN_STD
#  endif   // _RWSTD_NO_GLOBAL_BAD_CAST
#endif   // _RWSTD_NO_STD_BAD_CAST


NAMESPACE (std) {

class bad_cast
{
public:

#ifndef _RWSTD_NO_BAD_CAST_DEFAULT_CTOR

    bad_cast ();

#else

    bad_cast () { }

#endif   // _RWSTD_NO_BAD_CAST_DEFAULT_CTOR

#ifndef _RWSTD_NO_BAD_CAST_DTOR

    virtual ~bad_cast ();

#else

    virtual ~bad_cast () { }

#endif   // _RWSTD_NO_BAD_CAST_DEFAULT_CTOR

    bad_cast (const bad_cast&);
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

    std::bad_cast a;
    std::bad_cast b (a);

    (void)&a;
    (void)&b;

    // link only test
    return 0;
}
