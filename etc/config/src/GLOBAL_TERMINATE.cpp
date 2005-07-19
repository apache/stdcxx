// checking for ::terminate()

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

// looking for a definition in compiler support library
void terminate ();

int main (int argc, char *argv[])
{
    (void)&argv;

    if (argc > 1)
        terminate ();

    return 0;
}
