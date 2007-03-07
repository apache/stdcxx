// checking for std::set_terminate()

#include "config.h"

#ifdef _RWSTD_NO_EXCEPTION_SPECIFICATION

#  define throw()

#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION

// will fail if namespaces aren't supported
namespace std {

typedef void (*terminate_handler)();

// looking for a definition in compiler support library
terminate_handler set_terminate (terminate_handler) throw ();

}

// will cause an ambiguity if namespace std isn't being honored
int set_terminate ();

void handler () { }

int main (int argc, char *argv[])
{
    (void)&argv;

    if (argc > 1) {
        std::terminate_handler save = std::set_terminate (handler);

        return save == handler;
    }

    return 0;
}
