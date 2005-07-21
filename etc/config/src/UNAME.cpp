// determining OS name and version

#include <stdio.h>

#if !defined (_WIN32) && !defined (_WIN64)
#  include <sys/utsname.h>
#else
#  include <windows.h>
#endif


int compare (const char *a, const char *b)
{
    for (; *a && *a == *b; ++a, ++b);
    return *a - *b;
}


char* capitalize (char *str)
{
    static const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static const char lower[] = "abcdefghijklmnopqrstuvwxyz0123456789";

    for (char *s = str; *s; ++s) {
        for (unsigned i = 0; ; ++i) {
            
            if (*s == lower [i]) {
                *s = upper [i];
                break;
            }

            if (*s == upper [i])
                break;

            if ('\0' == lower [i]) {
                *s = '_';
                break;
            }
        }
    }

    return str;
}


#define ISDIGIT(x)   ((x) >= '0' && (x) <= '9')

int main ()
{
#if !defined (_RWSTD_USE_CONFIG)

    printf ("\n#undef _RWSTD_NO_UNAME /**/\n");

#endif   // _RWSTD_USE_CONFIG

#if !defined (_WIN32) && !defined (_WIN64)

    struct utsname uts;

    if (0 > uname (&uts))
        return 0;

    printf ("#define _RWSTD_OS_%s\n", capitalize (uts.sysname));

    printf ("#define _RWSTD_OS_SYSNAME \"%s\"\n", uts.sysname);
    printf ("#define _RWSTD_OS_RELEASE \"%s\"\n", uts.release);
    printf ("#define _RWSTD_OS_VERSION \"%s\"\n", uts.version);

    const char *s;
    unsigned long num;

#if !defined (SYS_NMLN)
#  define SYS_NMLN   128
#endif   // SYS_NMLN

    char str [SYS_NMLN * 2];

    for (num = 0; (str [num] = uts.release [num]); ++num);
    str [num] = '.';
    for (++num, s = uts.version; (str [num] = *s); ++num, ++s);

    for (s = str; *s && !ISDIGIT (*s); ++s);
    for (num = 0; ISDIGIT (*s); ++s)
        num = num * 10 + *s - '0';

    const char *major = "MAJOR";
    const char *minor = "MINOR";

    if (!compare ("AIX", uts.sysname)) {
        major = "MINOR";
        minor = "MAJOR";
    }

    printf ("#define _RWSTD_OS_%s %lu\n", major, num);

    for ( ; *s && !ISDIGIT (*s); ++s);
    for (num = 0; ISDIGIT (*s); ++s)
        num = num * 10 + *s - '0';

    printf ("#define _RWSTD_OS_%s %lu\n", minor, num);
        
    for (; *s && !ISDIGIT (*s); ++s);
    for (num = 0; ISDIGIT (*s); ++s)
        num = num * 10 + *s - '0';

    printf ("#define _RWSTD_OS_MICRO %lu\n", num);

#else   // if defined (_WIN{32,64})

    OSVERSIONINFO osinfo;
    osinfo.dwOSVersionInfoSize = sizeof osinfo;

    const BOOL success = GetVersionEx (&osinfo);

    printf ("#define _RWSTD_OS_WINDOWS\n");
    printf ("#define _RWSTD_OS_SYSNAME \"WINDOWS\"\n");

    if (!success)
        return 0;

    printf ("#define _RWSTD_OS_MAJOR %lu\n", osinfo.dwMajorVersion);
    printf ("#define _RWSTD_OS_MINOR %lu\n", osinfo.dwMinorVersion);
    printf ("#define _RWSTD_OS_MICRO %lu /* build number */\n",
            osinfo.dwBuildNumber);

    const char *flavor = 0;

    if (4 == osinfo.dwMajorVersion) {
        switch (osinfo.dwMinorVersion) {
        case 0:
            if (VER_PLATFORM_WIN32_NT == osinfo.dwPlatformId)
                flavor = "NT";
            else
                flavor = "95";
            break;
                
        case 10: flavor = "98"; break;
        case 90: flavor = "ME"; break;
        }
    }
    else if (5 == osinfo.dwMajorVersion) {
        switch (osinfo.dwMinorVersion) {
        case 0: flavor = "2000"; break;
        case 1: flavor = "XP"; break;
        case 2: flavor = "2003"; break;
        }
    }

    if (flavor) {
        printf ("#define _RWSTD_OS_RELEASE \"Windows %s\"\n", flavor);
        printf ("#define _RWSTD_OS_WINDOWS_%s\n", flavor);
    }
    else {
        printf ("#define _RWSTD_OS_RELEASE \"\"\n");
    }

    printf ("#define _RWSTD_OS_VERSION \"%s\"\n", osinfo.szCSDVersion);

#endif   // _WIN{32,64}

    return 0;
}
