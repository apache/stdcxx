// checking for bad_typeid dtor

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

    // defined in the language support library
    bad_typeid ();

#else   // if defined (_RWSTD_NO_BAD_TYPEID_DEFAULT_CTOR)

    bad_typeid () { }

#endif   // _RWSTD_NO_BAD_TYPEID_DEFAULT_CTOR

    virtual ~bad_typeid ();   // not defined here
};


}   // namespace std


struct Derived: std::bad_typeid
{
    virtual ~Derived ();
};


int main (int argc, char *argv[])
{
    (void)&argv;

    // try to prevent the compiler from optimizing the dtor call away
    std::bad_typeid *ptr = argc > 1 ? new Derived : new std::bad_typeid;

    delete ptr;

    // link only test 
    return 0;
}

Derived::~Derived () { }
