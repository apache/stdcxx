// checking for exception dtor

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


NAMESPACE (std) {

class exception
{
public:

#ifndef _RWSTD_NO_EXCEPTION_DEFAULT_CTOR

    // defined in the language support library
    exception ();

#else   // if defined (_RWSTD_NO_EXCEPTION_DEFAULT_CTOR)

    exception () { }

#endif   // _RWSTD_NO_EXCEPTION_DEFAULT_CTOR

    virtual ~exception ();   // not defined here
};


}   // namespace std

struct Derived: std::exception
{
    virtual ~Derived ();
};


int dtor;


int main (int argc, char *argv[])
{
    // avoid executing the body of main unless explicitly requested
    // by specifying at least one command line argument (this foils
    // aggressive optimizers from eliminating the code)
    (void)&argv;
    if (argc < 2)
        return 0;

    // try to prevent the compiler from optimizing the dtor call away
    std::exception *ptr;

    if (1 < argc)
        ptr = new Derived;
    else
        ptr = new std::exception;

    delete ptr;

    return !(1 < argc ? 1 == dtor : 0 == dtor);
}

Derived::~Derived () { ++dtor; }
