// checking if exceptions are thread safe

#include "thread.h"


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
#if defined (_WIN32)
    // disable displaying the critical-error-handler
    // and general-protection-fault message boxes
    // windows.h included in thread.h
    SetErrorMode (SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif   // _WIN32

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
