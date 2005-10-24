// checking for bad_typeid::what()

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


#ifndef _RWSTD_NO_STD_BAD_TYPEID
#  define NAMESPACE(name)   namespace name
#else
#  ifndef _RWSTD_NO_GLOBAL_BAD_TYPEID
#    define NAMESPACE(ignore)   extern "C++"
#    define std                 /* empty */
#  else
#    ifndef _RWSTD_NO_RUNTIME_IN_STD
#      define NAMESPACE(name)   namespace name
#    else
#      define NAMESPACE(ignore)   extern "C++"
#      define std                 /* empty */
#    endif   // _RWSTD_NO_RUNTIME_IN_STD
#  endif   // _RWSTD_NO_GLOBAL_BAD_TYPEID
#endif   // _RWSTD_NO_STD_BAD_TYPEID


NAMESPACE (std) {

class bad_typeid
{
public:

#ifndef _RWSTD_NO_BAD_TYPEID_DEFAULT_CTOR

    bad_typeid ();

#else

    bad_typeid () { }

#endif   // _RWSTD_NO_BAD_TYPEID_DEFAULT_CTOR

#ifndef _RWSTD_NO_BAD_TYPEID_DTOR

    virtual ~bad_typeid ();

#else

    virtual ~bad_typeid () { }

#endif   // _RWSTD_NO_BAD_TYPEID_DEFAULT_CTOR

    virtual const char* what () const;
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

    std::bad_typeid e;

    // link only test
    return !e.what ();
}
