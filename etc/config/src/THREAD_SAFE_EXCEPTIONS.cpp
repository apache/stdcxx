// checking if exceptions are thread safe

#ifndef _WIN32

#include <pthread.h>

#if defined (__sun) && defined (__SVR4)
// use the Solaris threads API in case this is Solaris 2.6
// where the POSIX pthread_setconcurrency() API is missing
#  include <thread.h>
#  define set_concurrency(n)   thr_setconcurrency (n)
#else
#  define set_concurrency(n)   pthread_setconcurrency (n)
#endif   // __sun && __SVR4

#else

#include "wthread.h"
#define set_concurrency(n) 

#endif


extern "C" void* thread_proc (void *arg)
{
    long sum = 0;
    long n   = (char*)arg - (char*)0;

    for (long i = 0; i != 0x20000; ++i) {
        try {
            if (i % 2 == n)
                throw i;
            else
                throw arg;
        }
        catch (long) {
            ++sum;
        }
        catch (...) {
            sum += 2;
        }
    }

    return (void*)sum;
}


int main ()
{
    pthread_t tid [2];

    const unsigned N = sizeof tid / sizeof *tid;

    set_concurrency (N);

    unsigned i;

    for (i = 0; i != N; ++i)
        if (pthread_create (tid + i, 0, thread_proc, (void*)i))
            return 1;

    for (i = 0; i != N; ++i)
        pthread_join (tid [i], 0);

    return 0;
}
