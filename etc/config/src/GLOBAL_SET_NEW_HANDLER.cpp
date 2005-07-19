// checking for ::set_new_handler()

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#ifdef _RWSTD_NO_EXCEPTION_SPECIFICATION

#  define throw()

#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION

typedef void (*new_handler)();

// looking for a definition in compiler support library
new_handler set_new_handler (new_handler) throw ();


void handler () { }

int main (int argc, char *argv[])
{
    (void)&argv;

    if (argc > 1) {
        new_handler save = set_new_handler (handler);

        return save == handler;
    }

    return 0;
}
