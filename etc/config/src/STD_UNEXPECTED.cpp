// checking for std::unexpected()

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

// will fail if namespaces aren't supported
namespace std {

// looking for a definition in compiler support library
void unexpected ();

}

// will cause an error if namespace std isn't being honored
int unexpected ();

int main (int argc, char *argv[])
{
    (void)&argv;

    if (argc > 1)
        std::unexpected ();

    return 0;
}
