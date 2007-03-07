// checking for std::set_new_handler()

#include "config.h"

#ifdef _RWSTD_NO_EXCEPTION_SPECIFICATION

#  define throw()

#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION

// will fail if namespaces aren't supported
namespace std {

typedef void (*new_handler)();

// looking for a definition in compiler support library
new_handler set_new_handler (new_handler) throw ();

}

// will cause an ambiguity if namespace std isn't being honored
int set_new_handler ();

void handler () { }

int main (int argc, char *argv[])
{
    (void)&argv;

    if (argc > 1) {
        std::new_handler save = std::set_new_handler (handler);

        return save == handler;
    }

    return 0;
}
