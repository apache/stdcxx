// checking if errno is thread safe

#include <errno.h>
#include "thread.h"


void *errno_addr = 0;


extern "C" void* thread_routine (void*)
{
    errno_addr = &errno;
    return 0;
}


int main ()
{
    pthread_t thread;

    int err;

    // create a thread that gets the address of errno
    if ((err = pthread_create (&thread, 0, thread_routine, 0))) {
        return 3;
    }

    if ((err = pthread_join (thread, 0))) {
        return 2;
    }

    // compare the address of the (presumably) thread-local
    // errno variable against that of the global errno and
    // return 0 if they are different, 1 otherwise
    return !(&errno != errno_addr);
}
