// checking exceptions

#include "config.h"

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


int main ()
{
    try {
        throw 1;
    }
    catch (int i) {
        return 0;
    }
    catch (...) {
        return 1;
    }
    return 2;
}
