// checking for class std::exception

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

}   // namespace std

#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


// will fail if namespaces aren't supported
namespace std {

class exception { };

}

// force a failure if namespace std isn't honored
class exception { };


struct B { virtual ~B () { } };
struct D1: B { };
struct D2: B { };

int main ()
{
    D1 d1;
    B &b = d1;

    try {
        // throws [std::] bad_cast
        D2 &d2 = dynamic_cast<D2&>(b);
    }
    catch (exception&) {
        // fail if namespace std isn't honored
        return 1;
    }
    catch (std::exception&) {
        return 0;
    }
    catch (...) {
        // fail if cast throws anything other than an std::exception-derivative
        return 1;
    }

    // fail if cast doesn't throw
    return 2;
}
