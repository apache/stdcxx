// checking for std::uncaught_exception()

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


// will fail if namespaces aren't supported
namespace std {

// looking for a definition in compiler support library
bool uncaught_exception ();

}

// will cause an error if namespace std isn't being honored
bool (*uncaught_exception)();

bool foo () { return false; }

bool uncaught;

struct S {
    ~S () {
        uncaught = (*uncaught_exception)();
    }
};


int main (int argc, char *argv[])
{
    (void)&argv;

    int result = 0;

    if (argc > 1)
        uncaught_exception = &std::uncaught_exception;
    else
        uncaught_exception = foo;

    if ((*uncaught_exception)())
        result = result << 1 + 1;

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

    if (uncaught)
        result = result << 1 + 1;

    return result;
}
