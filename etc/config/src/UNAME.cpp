// determining OS name and version

/***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 * Copyright 1999-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <stdio.h>

#ifndef _WIN32
#  include <sys/utsname.h>
#else
#  include <windows.h>
#endif


static int print_os_version ();


int main ()
{
    return print_os_version ();
}

////////////////////////////////////////////////////////////////////////

static unsigned length (const char *s)
{
    unsigned len = 0;

    while (*s++)
        ++len;

    return len;
}


static int compare (const char *a, const char *b)
{
    for (; *a && *a == *b; ++a, ++b);
    return *a - *b;
}


static char* capitalize (char *str)
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


#ifndef _WIN32


static void print_linux_release ()
{
    static const char* const files[] = {
        // Debian
        "/etc/debian_release",   // see also /etc/debian_version?
        // Fedora
        "/etc/fedora-release",
        // Gentoo
        "/etc/gentoo-release",
        // Novell SUSE
        "/etc/SuSE-release",
        // Red Hat
        "/etc/redhat-release",   // see also /etc/redhat_version?
        // Slackware
        "/etc/slackware-release",   // see also /etc/slackware-version?
        // Mandrake
        "/etc/mandrake-release",
        // Mandriva
        "/etc/mandriva-release",
        // MEPIS Linux
        "/etc/mepis-release",
        // Sun JDS
        "/etc/sun-release",
        // Solaris/SPARC
        "/etc/release",
        // Turbo Linux
        "/etc/turbolinux-release",
        // UnitedLinux
        "/etc/UnitedLinux-release",
        // Ubuntu
        "/etc/lsb-release",
        // Yellow Dog
        "/etc/yellowdog-release",

        ////////////////////////////////////////////////////
        // Last entry must be 0
        0
    };

    FILE *fp = 0;

    for (unsigned i = 0; files [i]; ++i) {
        fp = fopen (files [i], "r");
        if (fp)
            break;
    }


    char buffer [1024] = "(unknown release)";

    if (fp) {
        char *s = fgets (buffer, int (sizeof buffer), fp);
        if (s)
            buffer [length (s) - 1] = '\0';

    }

    printf ("#define _RWSTD_LINUX_RELEASE \"%s\"\n", buffer);
}


static int print_os_version ()
{
    struct utsname uts;

    if (0 > uname (&uts))
        return 1;

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

    if (0 == compare ("LINUX", uts.sysname)) {

        // determine Linux distribution
        print_linux_release ();
    }

    return 0;
}


#else   // if defined (_WIN32)


static int print_os_version ()
{
    OSVERSIONINFO osinfo;
    osinfo.dwOSVersionInfoSize = sizeof osinfo;

    const BOOL success = GetVersionEx (&osinfo);

    printf ("#define _RWSTD_OS_WINDOWS\n");
    printf ("#define _RWSTD_OS_SYSNAME \"WINDOWS\"\n");

    if (!success)
        return 1;

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

    return 0;
}

#endif   // _WIN32
