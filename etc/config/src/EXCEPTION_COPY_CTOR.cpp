// checking for exception copy ctor

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

    exception (const exception&);
};

}   // namespace std


int copy;

struct Derived: std::exception
{
    typedef std::exception Base;

    Derived (): Base () { }

    Derived (const Derived &e): Base (e) { ++copy; }
};

int main (int argc, char *argv[])
{
    (void)&argv;

    // prevent the code from actually being called but do it so
    // that the optimizer can't actually figure it out and eliminate
    // the function
    if (argc > 1) {
        Derived d0;
        Derived d1 (d0);
        std::exception e (d0);

#ifndef _RWSTD_NO_EXCEPTIONS

        try {
            // prevent aggressive optimizers such as Intel C++
            // from optimizing away the call to the copy ctor
            if (argc > 4)
                throw d1;
            if (argc > 3)
                throw d0;
            if (argc > 2)
                throw e;
        }
        catch (Derived d) {
            return !(2 == copy);
        }
        catch (std::exception e) {
            return !(1 == copy);
        }
#endif

    }
    else
	copy = 1;

    // link only test
    return !(1 == copy);
}
