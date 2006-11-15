// checking library exceptions

#include "config.h"

#if 0   // guard invalid preprocessor symbol below

   // establish a dependency on lib_exceptions.lib.*
#  ifndef _RWSTD_NO_lib_exceptions.lib

     // link this compiled translation unit with the lib below
     // LDOPTS = lib_exceptions.lib$(LIBSUFFIX)

#  endif   // _RWSTD_NO_lib_exceptions.lib
#endif   // 0


#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE

namespace std {

void terminate ()
{
    for (; ;) {
        char *p = 0;
        *p = 0;        // force a SIGSEGV
    }
}

}   // std


#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


// defined in a library, throws an exception
int foo (int);


int main ()
{
    try {
        // expected to throw an exception
        return foo (1);
    }
    catch (...) {
        // success: exception caught
        return 0;
    }

    // failure: no exception thrown
    return 1;
}
