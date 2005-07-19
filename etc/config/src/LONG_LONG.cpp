// checking for long long

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG


const char* foo (long long) { return "long long"; }
const char* foo (unsigned long long) { return "unsigned long long"; }

const char* bar (int i)
{
    long long ll = 0;
    unsigned long long ull = 0;

    return i < 0 ? foo (ll) : foo (ull);
}
