// checking for std::terminate()

#include "config.h"

// will fail if namespaces aren't supported
namespace std {

// looking for a definition in compiler support library
void terminate ();

}

// will cause an error if namespace std isn't being honored
int terminate ();

int main (int argc, char *argv[])
{
    (void)&argv;

    if (argc > 1)
        std::terminate ();

    return 0;
}
