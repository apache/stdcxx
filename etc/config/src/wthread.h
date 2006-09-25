// definitions of Win32 POSIX compatibility layer functions

#include <windows.h>

struct pthread_attr_t;

struct pthread_t
{
    long  id;
    void* handle;
};

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void*), void *arg)
{
    int result = 0;

    pthread_t tmpid;

    if (0 == thread)
        thread = &tmpid;

    DWORD nid;

    const HANDLE hthread = CreateThread (0, 0,
        (LPTHREAD_START_ROUTINE)start_routine,
        arg, 0, &nid);

    if (INVALID_HANDLE_VALUE == hthread) {
        thread->id     = -1;
        thread->handle = 0;
        result         = -1;
    }
    else {
        thread->id     = nid;
        thread->handle = hthread;
    }

    return result;
}

int pthread_join(pthread_t thread, void **value_ptr)
{
    int result = 0;

    const DWORD retcode = WaitForSingleObject (thread.handle, INFINITE);

    if (WAIT_OBJECT_0 == retcode) {
        if (value_ptr) {
            DWORD exit_code;

            if (GetExitCodeThread (thread.handle, &exit_code))
                *value_ptr = (void*)exit_code;
            else
                result = -1;
        }
    }
    else
        result = -1;

    return result;
}
