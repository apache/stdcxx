// checking for ::set_terminate()

#include "config.h"


#ifdef _RWSTD_NO_EXCEPTION_SPECIFICATION

#  define throw()

#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION

typedef void (*terminate_handler)();

// looking for a definition in compiler support library
terminate_handler set_terminate (terminate_handler) throw ();


void handler () { }

int main (int argc, char *argv[])
{
    (void)&argv;

    if (argc > 1) {
        terminate_handler save = set_terminate (handler);

        return save == handler;
    }

    return 0;
}
