// checking for class ::bad_cast

#include "config.h"


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


class bad_cast { };


struct B { virtual ~B () { } };
struct D1: B { };
struct D2: B { };

int main ()
{
    D1 d1;
    B &b = d1;

    try {
        // throws [std::]bad_cast
        D2 &d2 = dynamic_cast<D2&>(b);
        (void)&d2;
    }
    catch (bad_cast&) {
        return 0;
    }
    catch (...) {
        // fail if cast throws anything other than a ::bad_cast
        return 1;
    }

    // fail if cast doesn't throw
    return 2;
}
