/************************************************************************
 *
 * localedef.cpp - definitions of locale helpers
 *
 * $Id:$
 *
 ************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 *
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <localedef.h>

#include <environ.h>    // for rw_putenv()
#include <file.h>       // for SHELL_RM_RF, rw_tmpnam
#include <system.h>     // for rw_system()


#if defined __linux__
   // on Linux define _XOPEN_SOURCE to get CODESET defined in <langinfo.h>
#  define _XOPEN_SOURCE   500   /* Single Unix conformance */
   // bring __int32_t into scope (otherwise <wctype.h> fails to compile)
#  include <sys/types.h>
#endif   // __linux__

#include <fcntl.h>
#include <sys/stat.h>   // for stat

#if !defined  (_WIN32) && !defined (_WIN64)
#  include <unistd.h>
#  include <sys/wait.h>   // for WIFEXITED(), WIFSIGNALED(), WTERMSIG()
#else
#  include <io.h>
#  include <crtdbg.h> // for _malloc_dbg()
#endif

#include <ios>        // for ios::*
#include <limits>     // for numeric_limits
#include <locale>     // for money_base::pattern

#include <assert.h>   // for assert
#include <errno.h>    // for EBADF
#include <float.h>    // for {FLT,DBL,LDBL}_DIG
#include <limits.h>   // for CHAR_BIT, PATH_MAX
#include <locale.h>   // for LC_XXX macros, setlocale
#include <stdarg.h>   // for va_copy, va_list, ...
#include <stdio.h>    // for fgets, remove, sprintf, ...
#include <stdlib.h>   // for getenv, free, malloc, realloc
#include <string.h>   // for strcat, strcpy, strlen, ...
#include <ctype.h>
#include <wchar.h>    // for wcslen, ...

#ifndef PATH_MAX
#  define PATH_MAX   1024
#endif

#ifndef _MSC_VER
#  include <clocale>
#  ifndef LC_MESSAGES
#    define LC_MESSAGES _RWSTD_LC_MESSAGES
#  endif   // LC_MESSAGES
#  include <langinfo.h>
#  define EXE_SUFFIX    ""
#else   // if MSVC
#  define EXE_SUFFIX    ".exe"
#endif  // _MSC_VER


#define TOPDIR   "TOPDIR"   /* the TOPDIR environment variable */
#define BINDIR   "BINDIR"   /* the BINDIR environment variable */


#if _RWSTD_PATH_SEP == '/'
#  define SLASH    "/"
#  define IS_ABSOLUTE_PATHNAME(path)   (_RWSTD_PATH_SEP == *(path))
#else
#  define SLASH    "\\"
#  define IS_ABSOLUTE_PATHNAME(path)                    \
       (   (   'A' <= *(path) && 'Z' >= *(path)         \
            || 'a' <= *(path) && 'z' >= *(path))        \
        && ':' == (path)[1]                             \
        && _RWSTD_PATH_SEP == (path)[2])
#endif

// relative paths to the etc/nls directory and its subdirectories
#if defined (_RWSTD_USE_CONFIG)
#  define RELPATH        "etc" SLASH "nls"
#  define TESTS_ETC_PATH "tests" SLASH "etc"
#else
#  define RELPATH        "etc" SLASH "stdlib" SLASH "nls"
#  define TESTS_ETC_PATH "tests" SLASH "stdlib" SLASH "etc"
#endif  // _RWSTD_USE_CONFIG
            
/**************************************************************************/

_TEST_EXPORT int
rw_locale (const char *args, const char *fname)
{
    // use BINDIR to determine the location of the locale command
    const char* bindir = getenv ("BINDIR");
    if (!bindir)
        bindir = ".." SLASH "bin";

    int ret;

    if (fname)
        ret = rw_system ("%s%slocale%s %s",
                         bindir, SLASH, EXE_SUFFIX, args);
    else
        ret = rw_system ("%s%slocale%s %s >%s",
                         bindir, SLASH, EXE_SUFFIX, args, fname);

    return ret;
}

/**************************************************************************/

_TEST_EXPORT const char*
rw_localedef (const char *args,
              const char* src, const char *charmap, const char *locname)
{
    assert (src && charmap);

    // create a fully qualified pathname of the locale database
    // when (locname == 0), the pathname is computed by appending
    // the name of the character map file `charmap' to the name
    // of the locale definition file `src'
    // otherwise, when `locname' is not a pathname, the pathname
    // of the locale database is formed by appending `locname'
    // to the name of the locale root directory
    static char locale_path [PATH_MAX];

    const char* locale_root = getenv (LOCALE_ROOT_ENVAR);
    if (!locale_root)
        locale_root = ".";

    assert (  strlen (locale_root)
            + strlen (src)
            + strlen (charmap)
            + 2 < sizeof locale_path);

    strcpy (locale_path, locale_root);

    if (locname) {
        if (strchr (locname, _RWSTD_PATH_SEP))
            strcpy (locale_path, locname);
        else {
            strcat (locale_path, SLASH);
            strcat (locale_path, locname);
        }
    }
    else {
        // compute the locale pathname from `src', `charmap',
        // and `locale_root'
        strcpy (locale_path, locale_root);
        strcat (locale_path, SLASH);

        const char *slash = strrchr (src, _RWSTD_PATH_SEP);
        slash = slash ? slash + 1 : src;

        strcat (locale_path, src);
        strcat (locale_path, ".");

        slash = strrchr (charmap, _RWSTD_PATH_SEP);
        slash = slash ? slash + 1 : charmap;

        strcat (locale_path, slash);
    }

    // check to see if the locale database already exists and
    // if so, return immediately the locale filename to the caller
#if !defined (_MSC_VER)
    struct stat sb;
    if (!stat (locale_path, &sb)) {
#else
    struct _stat sb;
    if (!_stat (locale_path, &sb)) {
#endif
        return strrchr (locale_path, _RWSTD_PATH_SEP) + 1;
    }

    // otherwise, try to create the locale database

    // use TOPDIR to determine the root of the source tree
    const char* const topdir = getenv (TOPDIR);
    if (!topdir || !*topdir) {
        fprintf (stderr, "%s:%d: the environment variable %s is %s\n",
                 __FILE__, __LINE__, TOPDIR, topdir ? "empty" : "undefined");
        return 0;
    }

    // use BINDIR to determine the location of the localedef command
    const char* bindir = getenv ("BINDIR");
    if (!bindir)
        bindir = ".." SLASH "bin";

    // if `src' is relative pathname (or a filename) construct the fully
    // qualified absolute pathname to the locale definition file from it
    char src_path [PATH_MAX];

    if (!IS_ABSOLUTE_PATHNAME (src)) {
        strcpy (src_path, topdir);
        strcat (src_path, SLASH RELPATH SLASH "src" SLASH);
        strcat (src_path, src);

        // if the file doesn't exist, see if there is a file
        // with that name in the locale root directory (e.g.,
        // a temporary file)
        FILE* const file_exists = fopen (src_path, "r");
        if (file_exists)
            fclose (file_exists);
        else {
            strcpy (src_path, locale_root);
            strcat (src_path, SLASH);
            strcat (src_path, src);
        }

        src = src_path;
    }

    char charmap_path [PATH_MAX];
    if (!IS_ABSOLUTE_PATHNAME (charmap)) {
        strcpy (charmap_path, topdir);
        strcat (charmap_path, SLASH RELPATH SLASH "charmaps" SLASH);
        strcat (charmap_path, charmap);

        // if the file doesn't exist, see if there is a file
        // with that name in the locale root directory (e.g.,
        // a temporary file)
        FILE* const file_exists = fopen (charmap_path, "r");
        if (file_exists)
            fclose (file_exists);
        else {
            strcpy (charmap_path, locale_root);
            strcat (charmap_path, SLASH);
            strcat (charmap_path, charmap);
        }

        charmap = charmap_path;
    }

    if (!args)
        args = "";

    const int ret = rw_system ("%s%slocaledef%s %s -c -f %s -i %s %s",
                               bindir, SLASH, EXE_SUFFIX, args,
                               charmap, src, locale_path);

    // return the unqualified locale file name on success or 0 on failure
    return ret ? (char*)0 : strrchr (locale_path, _RWSTD_PATH_SEP) + 1;
}

/**************************************************************************/

extern "C" {

static char rw_locale_root [256];

static void atexit_rm_locale_root ()
{
    // remove temporary locale databases created by the test
    rw_system (SHELL_RM_RF "%s", rw_locale_root);
}

}

_TEST_EXPORT const char*
rw_set_locale_root ()
{
    // set any additional environment variables defined in
    // the RW_PUTENV environment variable (if it exists)
    rw_putenv (0);

    // create a temporary directory for files created by the test
    const char* const locale_root = rw_tmpnam (rw_locale_root);
    if (!locale_root)
        return 0;

    char envvar [sizeof LOCALE_ROOT_ENVAR + sizeof rw_locale_root] =
        LOCALE_ROOT_ENVAR "=";

    std::strcat (envvar, locale_root);

    // remove temporary file if mkstemp() rw_tmpnam() called mkstemp()
    if (rw_system (SHELL_RM_RF " %s", locale_root)) {

#if defined (_WIN32) || defined (_WIN64)
        // ignore errors on WIN32 where the stupid DEL command
        // fails even with /Q /S when the files don't exist
#else
        // assume a sane implementation of SHELL_RM_RF
        return 0;
#endif   // _WIN{32,64}
    }

    if (rw_system ("mkdir %s", locale_root))
        return 0;

    // set the "RWSTD_LOCALE_ROOT" environment variable
    // where std::locale looks for locale database files
    rw_putenv (envvar);

    if (atexit (atexit_rm_locale_root))
        perror ("atexit(atexit_rm_locale_root) failed");

    return locale_root;
}

/**************************************************************************/

_TEST_EXPORT char*
rw_locales (int loc_cat, const char* grep_exp)
{
    static char* slocname = 0;

    static int size       = 0;         // the number of elements in the array
    static int total_size = 5120;      // the size of the array
    static int last_cat   = loc_cat;   // last category

    // allocate first time through
    if (!slocname) {

#ifndef _MSC_VER
        slocname = _RWSTD_STATIC_CAST (char*, malloc (5120));
#else
        // prevent this leaked allocation from causing failures
        // in tests that keep track of storage allocated in
        //  _NORMAL_BLOCKS
        slocname = _RWSTD_STATIC_CAST (char*,
                       _malloc_dbg (5120, _CLIENT_BLOCK, 0, 0));
#endif
        *slocname = '\0';
    }

    // return immediately if buffer is already initialized
    if (*slocname && loc_cat == last_cat)
        return slocname;

    // remmeber the category we were last called with
    last_cat = loc_cat;

    char* locname = slocname;

    char* save_localename = 0;
    char  namebuf [256];

    if (loc_cat != _UNUSED_CAT) {
        // copy the locale name, the original may be overwitten by libc
        save_localename = strcpy (namebuf, setlocale (loc_cat, 0));
    }

    const char* const fname = rw_tmpnam (0);

    if (!fname) {
        return 0;   // error
    }

    // make sure that grep_exp is <= 80 
    if (grep_exp && 80 < strlen (grep_exp)) {
        abort ();
    }

    // execute a shell command and redirect its output into the file
    const int exit_status =
          grep_exp && *grep_exp
        ? rw_system ("locale -a | grep \"%s\" > %s", grep_exp, fname)
        : rw_system ("locale -a > %s", fname);

    if (exit_status) {
        return 0;   // error
    }

    // open file containing the list of installed locales
    FILE *file = fopen (fname, "r");

    if (file) {

        char linebuf [256];

        // even simple locale names can be very long (e.g., on HP-UX,
        // where a locale name always consists of the names of all
        // categories, such as "C C C C C C")
        char last_name [256];
        *last_name = '\0';

        // if successful, construct a char array with the locales
        while (fgets (linebuf, sizeof linebuf, file)) {

            linebuf [strlen (linebuf) - 1] = '\0';

#ifdef _RWSTD_OS_SUNOS

            // avoid the bad locale named iso_8859_1 on SunOS
            if (!strcmp ("iso_8859_1", linebuf))
                continue;

#endif   // _RWSTD_OS_SUNOS

            // if our buffer is full then dynamically allocate a new one
            if ((size += (strlen (linebuf) + 1)) > total_size) {
                total_size += 5120;

                char* tmp =
                    _RWSTD_STATIC_CAST (char*, malloc (total_size));

                memcpy (tmp, slocname, total_size - 5120);
                free (slocname);

                slocname = tmp;
                locname  = slocname + size - strlen (linebuf) - 1;
            }

#ifdef _WIN64

            // prevent a hang (OS/libc bug?)
            strcpy (locname, linebuf);
            locname += strlen (linebuf) + 1;

#else   // if !defined (_WIN64)
            if (loc_cat != _UNUSED_CAT) {

                // set the C locale to verify that the name is valid
                const char *name = setlocale (loc_cat, linebuf);

                // if it is and if the actual locale name different
                // from the last one, append it to the list
                if (name && strcmp (last_name, name)) {
                    strcpy (locname, linebuf);
                    locname += strlen (linebuf) + 1;

                    // save the last locale name
                    assert (strlen (name) < sizeof last_name);
                    strcpy (last_name, name);
                }
            }
            else {
                strcpy (locname, linebuf);
                locname += strlen (linebuf) + 1;
            }

#endif   // _WIN64

        }
        *locname = '\0';
    }

    if (loc_cat != _UNUSED_CAT)
        setlocale (loc_cat, save_localename);

    // close before removing
    fclose (file);

    remove (fname);

    return slocname;
}
