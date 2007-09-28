// checking for madvise() in <sys/mman.h>

#include <sys/mman.h>    // for madvise(), MADV_WILLNEED
#include <sys/types.h>   // for caddr_t


int main (int argc, char *argv[])
{
    const caddr_t addr = (caddr_t)argv;

    const int result = madvise (addr, argc, MADV_WILLNEED);

    if (1 < argc) {
        // invoke with any command line arguments to test
        return result;
    }

    // always succeed when invoked without arguments
    return 0;
}
