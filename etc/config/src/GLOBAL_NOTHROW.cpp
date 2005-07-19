// checking for ::nothrow

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG


struct nothrow_t { };

extern const nothrow_t nothrow;

const nothrow_t *ptr = &nothrow;


int main (int argc, char *argv[])
{
    (void)&argv;

    // foil clever optimizers
    if (argc > 2)
        ptr = 0;

    // will fail at link-time if ::nothrow is not defined
    // in the compiler support library
    return !(0 != ptr);
}
