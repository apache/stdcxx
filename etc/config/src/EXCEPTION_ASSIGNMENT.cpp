// checking for exception assignment operator

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


NAMESPACE (std) {

class exception
{
public:

#ifndef _RWSTD_NO_EXCEPTION_DEFAULT_CTOR

    exception ();

#else

    exception () { }

#endif   // _RWSTD_NO_EXCEPTION_DEFAULT_CTOR

#ifndef _RWSTD_NO_EXCEPTION_DTOR

    virtual ~exception ();

#else

    virtual ~exception () { }

#endif   // _RWSTD_NO_EXCEPTION_DEFAULT_CTOR

    exception& operator= (const exception&);
};


}   // namespace std


void foo ()
{
    std::exception e;
    std::exception f;

    e = f;
}


int main (int argc, char *argv[])
{
    // avoid executing the body of main unless explicitly requested
    // by specifying at least one command line argument (this foils
    // aggressive optimizers from eliminating the code)
    (void)&argv;
    if (argc < 2)
        return 0;

    foo ();

    // link only test
    return 0;
}
