// checking for reinterpret_cast

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG


int main ()
{
    const int *i = reinterpret_cast<const int*>("");

    return !(0 != i);
}
