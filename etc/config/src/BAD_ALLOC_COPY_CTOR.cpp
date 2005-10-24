// checking for bad_alloc copy ctor

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

}   // namespace std

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

    bad_alloc (const bad_alloc&);
};

#ifdef _RWSTD_NO_BAD_ALLOC_DEFAULT_CTOR

bad_alloc::bad_alloc () { }

#endif   // _RWSTD_NO_BAD_ALLOC_DEFAULT_CTOR

#ifdef _RWSTD_NO_BAD_ALLOC_DTOR

virtual bad_alloc::~bad_alloc () { }

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

#ifndef _RWSTD_NO_EXCEPTIONS

    try {
        try {
            if (argc > 1) {
                std::bad_alloc e;

                throw e;
            }
        }
        catch (std::bad_alloc e) {

            std::bad_alloc copy (e);

            if (argc > 4)
                throw copy;

            if (argc > 3)
                throw e;

            if (argc > 2)
                throw;
        }
    }
    catch (std::bad_alloc e) {

        (void)&e;
        return 1;
    }
    catch (...) {
        return 2;
    }

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    std::bad_alloc a;
    std::bad_alloc b (a);
    (void)&b;

#endif   // _RWSTD_NO_EXCEPTIONS

    // link only test
    return 0;
}
