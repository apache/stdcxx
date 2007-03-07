// checking for std::set_unexpected()

#include "config.h"


#ifdef _RWSTD_NO_EXCEPTION_SPECIFICATION

#  define throw()

#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION

// will fail if namespaces aren't supported
namespace std {

typedef void (*unexpected_handler)();

// looking for a definition in compiler support library
unexpected_handler set_unexpected (unexpected_handler) throw ();

}

// will cause an ambiguity if namespace std isn't being honored
int set_unexpected ();

void handler () { }

int main (int argc, char *argv[])
{
    if (argc > 1) {
        std::unexpected_handler save = std::set_unexpected (handler);

        return save == handler;
    }

    return 0;
}
