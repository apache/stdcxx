// checking for bad_typeid copy ctor

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

    bad_typeid (const bad_typeid&);
};

}   // namespace std


int main (int argc, char *argv[])
{
    // prevent the code from actually being executed but do it so
    // that the optimizer can't actually figure it out and eliminate
    // the function
    if (argc > 256) {
        std::bad_typeid a;
        std::bad_typeid b (a);

        (void)&a;
        (void)&b;
    }

    // link only test
    return 0;
}
