// proclimits.h must be included in .cpp file
// only after #if[n]def _RWSTD_NO_SETRLIMIT line

#if !defined (_RWSTD_NO_SETRLIMIT)

#  include <sys/resource.h>   // for setrlimit()

void limit_memory (unsigned long limit)
{
    struct rlimit rl = { limit, limit };
    setrlimit (RLIMIT_DATA, &rl);
}

#elif defined (_WIN32)

#  ifndef _WIN32_WINNT
#    define _WIN32_WINNT 0x0500
#  endif

#  if _WIN32_WINNT >= 0x0500

// Job Objects available only in Windows 2000 and later

#    include <windows.h>

void limit_memory (unsigned long limit)
{
    SYSTEM_INFO info;
    GetSystemInfo (&info);

    HANDLE hJob = CreateJobObject (NULL, NULL);
    if (hJob) {
        if (AssignProcessToJobObject (hJob, GetCurrentProcess ())) {
            JOBOBJECT_EXTENDED_LIMIT_INFORMATION job_info = { 0 };
            job_info.BasicLimitInformation.LimitFlags =
                JOB_OBJECT_LIMIT_PROCESS_MEMORY;
            job_info.ProcessMemoryLimit =
                limit > info.dwPageSize ? limit : info.dwPageSize;
            SetInformationJobObject (hJob,
                                     JobObjectExtendedLimitInformation,
                                     &job_info, sizeof (job_info));
        }
        CloseHandle (hJob);
    }
}

#  else   // _WIN32_WINNT < 0x0500

void limit_memory (unsigned long) { /* no-op */ }

#  endif  // _WIN32_WINNT

#else   // _RWSTD_NO_SETRLIMIT && !_WIN32

void limit_memory (unsigned long) { /* no-op */ }

#endif  // _RWSTD_NO_SETRLIMIT
