// checking for ::uncaught_exception()

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG


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

}   // namespace std

#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


// looking for a definition in compiler support library
bool uncaught_exception ();


bool uncaught;

struct S {
    ~S () {
        uncaught = uncaught_exception ();
    }
};


int main (int argc, char *argv[])
{
    (void)&argv;

#ifndef _RWSTD_NO_EXCEPTIONS

    try {
        S s;

        // do not actually invoke uncaught_exception()
        // unless at least one command-line argument
        // is specified (done to prevent the compiler
        // from optimizing the function call away)
        if (argc > 1)
            throw 0;
    }
    catch (...) {
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    return uncaught;
}
