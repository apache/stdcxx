/************************************************************************
 *
 * locale.cpp - definitions of locale helpers
 *
 * $Id$
 *
 ************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
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
 * Copyright 2001-2006 Rogue Wave Software.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC


#include <rw_locale.h>

#include <environ.h>      // for rw_putenv()
#include <file.h>         // for SHELL_RM_RF, rw_tmpnam
#include <rw_process.h>   // for rw_system()
#include <driver.h>       // for rw_error()

#if defined (_RWSTD_OS_LINUX) && !defined (_XOPEN_SOURCE)
   // on Linux define _XOPEN_SOURCE to get CODESET defined in <langinfo.h>
#  define _XOPEN_SOURCE   500   /* Single Unix conformance */
   // bring __int32_t into scope (otherwise <wctype.h> fails to compile)
#  include <sys/types.h>
#endif   // Linux

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
#include <limits.h>   // for PATH_MAX
#include <locale.h>   // for LC_XXX macros, setlocale
#include <stdarg.h>   // for va_copy, va_list, ...
#include <stdio.h>    // for fgets, remove, sprintf, ...
#include <stdlib.h>   // for getenv, free, malloc, realloc
#include <string.h>   // for strcat, strcpy, strlen, ...
#include <ctype.h>
#include <wchar.h>    // for wcslen, ...

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


#if !defined (PATH_MAX) || PATH_MAX < 128 || 4096 < PATH_MAX
   // deal  with undefined, bogus, or excessive values
#  undef  PATH_MAX
#  define PATH_MAX   1024
#endif


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
#define RELPATH        "etc" SLASH "nls"
#define TESTS_ETC_PATH "tests" SLASH "etc"
            
/**************************************************************************/

_TEST_EXPORT int
rw_locale (const char *args, const char *fname)
{
    // use BINDIR to determine the location of the locale command
    const char* bindir = getenv (BINDIR);
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
        rw_error (0, __FILE__, __LINE__,
                  "the environment variable %s is %s",
                  TOPDIR, topdir ? "empty" : "undefined");
        return 0;
    }

    // use BINDIR to determine the location of the localedef command
    const char* bindir = getenv (BINDIR);
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

static char rw_locale_root [PATH_MAX];

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

    strcat (envvar, locale_root);

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

    rw_error (0 == atexit (atexit_rm_locale_root), __FILE__, __LINE__,
              "atexit(atexit_rm_locale_root) failed: %m");

    return locale_root;
}

/**************************************************************************/

_TEST_EXPORT char*
rw_locales (int loc_cat, const char* grep_exp)
{
    static char* slocname = 0;

    static size_t size       = 0;         // the number of elements in the array
    static size_t total_size = 5120;      // the size of the array
    static int    last_cat   = loc_cat;   // last category

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
    char  namebuf [PATH_MAX];

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

            const size_t linelen = strlen (linebuf);

            linebuf [linelen ? linelen - 1 : 0] = '\0';

#ifdef _RWSTD_OS_SUNOS

            const char iso_8859_pfx[] = "iso_8859_";

            // avoid locales named common and iso_8859_* on SunOS
            // since they are known to cause setlocale() to fail
            if (   !strcmp ("common", linebuf)
                || sizeof iso_8859_pfx <= linelen 
                && !memcmp (iso_8859_pfx, linebuf, sizeof iso_8859_pfx - 1))
                continue;

#endif   // _RWSTD_OS_SUNOS

            // if our buffer is full then dynamically allocate a new one
            if (total_size < (size += (strlen (linebuf) + 1))) {
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

/**************************************************************************/

// finds a multibyte character that is `bytes' long if `bytes' is less
// than or equal to MB_CUR_MAX, or the longest multibyte sequence in
// the current locale
static const char*
_get_mb_char (char *buf, size_t bytes)
{
    _RWSTD_ASSERT (0 != buf);

    *buf = '\0';

    if (0 == bytes)
        return buf;

    const bool exact = bytes <= size_t (MB_CUR_MAX);

    if (!exact)
        bytes = MB_CUR_MAX;

    wchar_t wc;

    // search the first 64K characters sequentially
    for (wc = wchar_t (1); wc != wchar_t (0xffff); ++wc) {

        if (   int (bytes) == wctomb (buf, wc)
            && int (bytes) == mblen (buf, bytes)) {
            // NUL-terminate the multibyte character of the requested length
            buf [bytes] = '\0';
            break;
        }

        *buf = '\0';
    }

#if 2 < _RWSTD_WCHAR_SIZE

    // if a multibyte character of the requested size is not found
    // in the low 64K range, try to find one using a random search
    if (wchar_t (0xffff) == wc) {

        // iterate only so many times to prevent an infinite loop
        // in case when MB_CUR_MAX is greater than the longest
        // multibyte character
        for (int i = 0; i != 0x100000; ++i) {

            wc = wchar_t (rand ());

            if (RAND_MAX < 0x10000) {
                wc <<= 16;
                wc |=  wchar_t (rand ());
            }

            if (   int (bytes) == wctomb (buf, wc)
                && int (bytes) == mblen (buf, bytes)) {
                // NUL-terminate the multibyte character
                buf [bytes] = '\0';
                break;
            }

            *buf = '\0';
        }
    }

#endif   // 2 < _RWSTD_WCHAR_SIZE

    // return 0 on failure to find a sequence exactly `bytes' long
    return !exact || bytes == strlen (buf) ? buf : 0;
}


_TEST_EXPORT size_t
rw_get_mb_chars (rw_mbchar_array_t mb_chars)
{
    _RWSTD_ASSERT (0 != mb_chars);

    const char* mbc = _get_mb_char (mb_chars [0], size_t (-1));

    if (0 == rw_note (0 != mbc, __FILE__, __LINE__,
                      "failed to find any multibyte characters "
                      "in locale \"%s\" with MB_CUR_MAX = %u",
                      setlocale (LC_CTYPE, 0), MB_CUR_MAX))
        return 0;

    size_t mb_cur_max = strlen (mbc);

    if (_RWSTD_MB_LEN_MAX < mb_cur_max)
        mb_cur_max = _RWSTD_MB_LEN_MAX;

    // fill each element of `mb_chars' with a multibyte character
    // of the corresponding length
    for (size_t i = mb_cur_max; i; --i) {

        // try to generate a multibyte character `i' bytes long
        mbc = _get_mb_char (mb_chars [i - 1], i);

        if (0 == mbc) {
            // zh_CN.gb18030 and zh_TW.euctw on Linux are examples
            // of multibyte locales where MB_CUR_MAX == 4 but,
            // apparently, no 3-byte characters
            if (0 == rw_note (mb_cur_max <= i, __FILE__, __LINE__,
                              "failed to find %u-byte characters "
                              "in locale \"%s\" with MB_CUR_MAX = %u",
                              i, setlocale (LC_CTYPE, 0), MB_CUR_MAX)) {
                mb_cur_max = 0;
                break;
            }
            --mb_cur_max;
        }
    }

    return mb_cur_max;
}


_TEST_EXPORT const char*
rw_find_mb_locale (size_t            *mb_cur_max,
                   rw_mbchar_array_t  mb_chars)
{
    _RWSTD_ASSERT (0 != mb_cur_max);
    _RWSTD_ASSERT (0 != mb_chars);

    if (2 > _RWSTD_MB_LEN_MAX) {
        rw_warn (0, __FILE__, __LINE__, "MB_LEN_MAX = %d, giving up",
                 _RWSTD_MB_LEN_MAX);
        return 0;
    }

    static const char *mb_locale_name;

    char saved_locale_name [1024];
    strcpy (saved_locale_name, setlocale (LC_CTYPE, 0));

    _RWSTD_ASSERT (strlen (saved_locale_name) < sizeof saved_locale_name);

    *mb_cur_max = 0;

    // iterate over all installed locales
    for (const char *name = rw_locales (_RWSTD_LC_CTYPE, 0); name && *name;
         name += strlen (name) + 1) {

        if (setlocale (LC_CTYPE, name)) {

            // try to generate a set of multibyte characters
            // with lengths from 1 and MB_CUR_MAX (or less)
            const size_t cur_max = rw_get_mb_chars (mb_chars);

            if (*mb_cur_max < cur_max) {
                *mb_cur_max    = cur_max;
                mb_locale_name = name;

                // break when we've found a multibyte locale
                // with the longest possible encoding
                if (_RWSTD_MB_LEN_MAX == *mb_cur_max)
                    break;
            }
        }
    }

    if (*mb_cur_max < 2) {
        rw_warn (0, __FILE__, __LINE__,
                 "failed to find a full set of multibyte "
                 "characters in locale \"%s\" with MB_CUR_MAX = %u "
                 "(computed)", mb_locale_name, *mb_cur_max);
        mb_locale_name = 0;
    }
    else {
        // (re)generate the multibyte characters for the saved locale
        // as they may have been overwritten in subsequent iterations
        // of the loop above (while searching for a locale with greater
        // value of MB_CUR_MAX)
        setlocale (LC_CTYPE, mb_locale_name);
        rw_get_mb_chars (mb_chars);
    }

    setlocale (LC_CTYPE, saved_locale_name);

    return mb_locale_name;
}


/**************************************************************************/

_TEST_EXPORT const char*
rw_create_locale (const char *charmap, const char *locale)
{
    // only one locale is enough (avoid invoking localedef more than once)
    static const char* locname;
    const char* locale_root;

    if (locname)
        return locname;

    // set up RWSTD_LOCALE_ROOT and other environment variables
    locale_root = rw_set_locale_root ();

    if (0 == locale_root)
        return 0;

    // create a temporary locale definition file that exercises as
    // many different parts of the collate standard as possible
    char srcfname [PATH_MAX];
    sprintf (srcfname, "%s%slocale.src", locale_root, SLASH);

    FILE *fout = fopen (srcfname, "w");

    if (!fout) {
        rw_error (0, __FILE__, __LINE__,
                  "fopen(#%s, \"w\") failed: %m", srcfname);
        return 0;
    }

    fprintf (fout, "%s", locale);

    fclose (fout);

    // create a temporary character map file
    char cmfname [PATH_MAX];
    sprintf (cmfname, "%s%scharmap.src", locale_root, SLASH);

    fout = fopen (cmfname, "w");

    if (!fout) {
        rw_error (0, __FILE__, __LINE__,
                  "fopen(%#s, \"w\") failed: %m", cmfname);
        return 0;
    }

       fprintf (fout, "%s", charmap);

    fclose (fout);

       locname = "test-locale";

    // process the locale definition file and character map
    if (0 == rw_localedef ("-w", srcfname, cmfname, locname))
        locname = 0;

    return locname;
}


/**************************************************************************/

static const char*
_rw_locale_names;

_TEST_EXPORT const char* const&
rw_opt_locales = _rw_locale_names;


_TEST_EXPORT int
rw_opt_setlocales (int argc, char* argv[])
{
    RW_ASSERT (0 < argc && argv [0]);

    // the option requires an equals sign followed by an optional argument
    char *args = strchr (argv [0], '=');

    RW_ASSERT (0 != args);

    // small static buffer should be sufficient in most cases
    static char buffer [256];

    const size_t len = strlen (++args);

    // dynamically allocate a bigger buffer when the small buffer
    // isn't big enough (let the dynamically allocated buffer leak)
    char* const locale_names =
        sizeof buffer < len + 2 ? (char*)malloc (len + 2) : buffer;

    if (0 == locale_names)
        return 1;

    locale_names [len]     = '\0';
    locale_names [len + 1] = '\0';

    memcpy (locale_names, args, len);

    for (char *next = locale_names; ; ) {
        next = strpbrk (next, ", ");
        if (next)
            *next++ = '\0';
        else
            break;
    }

    _rw_locale_names = locale_names;

    // return 0 on success
    return 0;
}
