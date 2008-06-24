// determining combined locale name format

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

// If strict_ansi_errors is present, the definition of __PURE_CNAME 
// dictates inclusion of  locale_cname_impl & locale_ansi_impl which 
// do not define LC_MESSAGES locale category
#if defined (__PURE_CNAME)
#  undef __PURE_CNAME
#endif // __PURE_CNAME

#include <locale.h>
#include <stdio.h>
#include <string.h>

// prevent IBM xlC 5.0 errors caused by using /usr/include/string.h
// which #defines these (and other) names to undeclared symbols
#undef strcpy
#undef strcmp
#undef strlen
#undef strcat


#include "locale_names.h"   // for test_locale_names


// the longest combined locale name handled by the test (GNU glibc
// can generate some awfully long names since in addition to the
// name of the locale name for each category it includes the name
// of the category itself, e.g., "LC_CTYPE=en;LC_NUMERIC=es;...")
#define MAX_LOCALE_NAME_LEN   1024


#if 0   // disabled

// enable for debugging to emulate a system with no locales installed

extern "C" char* setlocale (int, const char *name)
{
    static char cname[] = "C";

    if (0 == name)
        return cname;

    if ('C' != name [0] || '\0' != name [1])
        return 0;

    return cname;
}

#endif   // 0/1


// define own version of the libc functions to prevent problems
// caused by them being not declared (sometimes due to the fact
// that they must be overloaded in C++ which may not be done in
// headers that are #included in the test)
 
char* rw_strchr (const char *s, char c)
{
    for (; *s && *s != c; ++s);

    return *s == c ? (char*)s : 0;
}


char* rw_strpbrk (const char *s, const char *seps)
{
    for (; *seps; ++seps) {
        char *where = rw_strchr (s, *seps);
        if (where)
            return where;
    }

    return 0;
}


char* rw_strstr (const char *str, const char *s)
{
    for (; *str; ++str) {

        str = rw_strchr (str, *s);

        if (!str)
            return 0;

        const char *s1 = str;
        const char *s2 = s;

        for (; *s1 && *s2 && *s1 == *s2; ++s1, ++s2);

        if (!*s2)
            return (char*)str;
    }

    return 0;
}


extern "C" int putenv (char*);


extern const char* const test_locale_names[];
extern const unsigned    nlocales;

int print_lc_constants ();
int print_categories (const char*, int, int&, int&, int, char&, char&);
int print_locale_name_format (int, int, int, int, int, char, char);


#if !defined (_WIN32) || defined (__CYGWIN__)
char cat_seps[] = " \n\t/\\:;#%";
#else
char cat_seps[] = "\n\t/\\:;#%";
#endif


int main ()
{
    // compute and print the values of the LC_XXX constants
    // and their relationship to the std::locale::category
    // constants
    if (print_lc_constants ())
        return 0;

    // try to determine the format of combined locale names
    int  setlocale_environ    = 0;
    int  loc_name_use_cat     = 0;
    int  loc_name_prepend_sep = 0;
    int  loc_name_condense    = 0;
    char loc_name_cat_sep     = '\0';
    char loc_name_cat_eq      = '\0';

    char namebuf [MAX_LOCALE_NAME_LEN];
    namebuf [0] = '\0';

    // determine whether setlocale(LC_ALL, name) returns
    // a string containing the equivalent of `name' for
    // each LC_XXX category or whether it collapses all
    // equivalent names into just a single locale name
    const char *locname = setlocale (LC_ALL, "C");

    char *sep = locname ? rw_strpbrk (locname, cat_seps) : 0;
    if (sep) {
        if ('C' == sep [-1] && 1 == sep - locname)
            loc_name_cat_sep = *sep;
        else {
            loc_name_condense = 1;
            *rw_strchr (cat_seps, *sep) = '\n';
        }
    }
    else {
        loc_name_condense = 1;
    }

    // find the first valid locale name other than "C" or
    // the default locale returned by locale (LC_ALL, "")
    if ((locname = setlocale (LC_ALL, "")))
        strcpy (namebuf, locname);

    unsigned i;

    for (i = 0; i != nlocales; ++i) {

        locname = test_locale_names [i];

#if defined (_MSC_VER) && _MSC_VER <= 1200

        // work around an MSVC libc bug (PR #27990)
        if (rw_strpbrk (locname, ".-"))
            continue;

#endif   // MSVC <= 6.0

        locname = setlocale (LC_ALL, locname);
        if (locname && strcmp (namebuf, locname))
            break;
    }

    if (locname) {
        locname = strcpy (namebuf, locname);

        sep = rw_strpbrk (locname, cat_seps);
        if (sep)
            *sep = '\0';

        // determine if the environment has any effect on setlocale()
        {
            char def_locale [MAX_LOCALE_NAME_LEN];
            def_locale [0] = '\0';

            const char *tmpname = setlocale (LC_ALL, "");
            if (tmpname)
                strcpy (def_locale, tmpname);

            char buf [MAX_LOCALE_NAME_LEN];
            strcpy (buf, "LC_COLLATE=");
            strcat (buf, locname);
            putenv (buf);

            tmpname = setlocale (LC_ALL, "");
            setlocale_environ = tmpname ? strcmp (def_locale, tmpname) : 1;
        }
    }

    print_categories (locname,
                      setlocale_environ,
                      loc_name_use_cat,
                      loc_name_prepend_sep,
                      loc_name_condense,
                      loc_name_cat_sep,
                      loc_name_cat_eq);

    return print_locale_name_format (0 == locname,
                                     setlocale_environ,
                                     loc_name_use_cat,
                                     loc_name_prepend_sep,
                                     loc_name_condense,
                                     loc_name_cat_sep,
                                     loc_name_cat_eq);
}

/*********************************************************************/

static struct LC_vars
{
    int         ord;
    int         cat;
    char        name [64];
    const char *lower;
} lc_vars [] = {
    { -1, LC_COLLATE,  "LC_COLLATE=C",  "collate" },
    { -1, LC_CTYPE,    "LC_CTYPE=C",    "ctype" },
    { -1, LC_MONETARY, "LC_MONETARY=C", "monetary" },
    { -1, LC_NUMERIC,  "LC_NUMERIC=C",  "numeric" },
    { -1, LC_TIME,     "LC_TIME=C",     "time" },

#if defined (LC_MESSAGES)
    { -1, LC_MESSAGES, "LC_MESSAGES=C", "messages" },
#endif

#if defined (LC_NAME)
    { -1, LC_NAME, "LC_NAME=C", "name" },
#endif

#if defined (LC_PAPER)
    { -1, LC_PAPER, "LC_PAPER=C", "paper" },
#endif

#if defined (LC_IDENTIFICATION)
    { -1, LC_IDENTIFICATION, "LC_IDENTIFICATION=C", "ident" },
#endif

#if defined (LC_ADDRESS)
    { -1, LC_ADDRESS, "LC_ADDRESS=C", "address" },
#endif

#if defined (LC_TELEPHONE)
    { -1, LC_TELEPHONE, "LC_TELEPHONE=C", "telephone" },
#endif

#if defined (LC_MEASUREMENT)
    { -1, LC_MEASUREMENT, "LC_MEASUREMENT=C", "measurement" },
#endif

    { -1, LC_ALL, "LC_ALL", 0 },
    { -1, 0, "", 0 }
};


// the known order of categories in combined locale names
const int lc_cat_order[] = {

#if defined (_AIX)
    LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME, LC_MESSAGES,
    -1, -1, -1, -1, -1, -1
#elif defined (__FreeBSD__) || defined (__NetBSD__)
    LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME, LC_MESSAGES,
    -1, -1, -1, -1, -1, -1
#elif defined (__hpux)
    LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME, LC_MESSAGES,
    -1, -1, -1, -1, -1, -1
#elif defined (__GLIBC__)
    LC_CTYPE, LC_NUMERIC, LC_TIME, LC_COLLATE, LC_MONETARY, LC_MESSAGES,

#  if defined (LC_PAPER)
    LC_PAPER, LC_NAME, LC_ADDRESS, LC_TELEPHONE, LC_MEASUREMENT,
    LC_IDENTIFICATION
#  else
    -1, -1, -1, -1, -1, -1
#  endif
#elif defined (__osf__)
    LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME, LC_MESSAGES,
    -1, -1, -1, -1, -1, -1
#elif defined (__sgi)
    LC_CTYPE, LC_NUMERIC, LC_TIME, LC_COLLATE, LC_MONETARY, LC_MESSAGES,
    -1, -1, -1, -1, -1, -1
#elif    (defined (__sun__) || defined (__sun) || defined (sun)) \
      && defined (__svr4__)
    LC_CTYPE, LC_NUMERIC, LC_TIME, LC_COLLATE, LC_MONETARY, LC_MESSAGES,
    -1, -1, -1, -1, -1, -1
#elif defined (_WIN32)
    LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME, -1,
    -1, -1, -1, -1, -1, -1,
#elif defined (__CYGWIN__)
    // this is just a wild guess since localization support
    // on CygWin seems to be very limited
    LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC, LC_TIME, -1,
    -1, -1, -1, -1, -1, -1,
#else
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1
#endif

};

/*********************************************************************/

int print_lc_constants ()
{
    // determine the values of LC_XXX constants

    unsigned lc_max_inx = 0;
    unsigned lc_min_inx = 0;

    char *eq;

    for (unsigned i = 0; *lc_vars [i].name; ++i) {
        eq = rw_strchr (lc_vars [i].name, '=');
        if (eq)
            *eq = '\0';

        printf ("#define _RWSTD_%-20s %2d\n",
                lc_vars [i].name, lc_vars [i].cat);

        if (lc_vars [i].cat > lc_vars [lc_max_inx].cat)
            lc_max_inx = i;

        if (lc_vars [i].cat < lc_vars [lc_min_inx].cat)
            lc_min_inx = i;

        if (eq)
            *eq = '=';
    }

    eq = rw_strchr (lc_vars [lc_max_inx].name, '=');
    if (eq)
        *eq = '\0';

    printf ("#define %-27s _RWSTD_%s\n",
            "_RWSTD_LC_MAX", lc_vars [lc_max_inx].name);

    if (eq)
        *eq = '=';

    eq = rw_strchr (lc_vars [lc_min_inx].name, '=');
    if (eq)
        *eq = '\0';

    printf ("#define %-27s _RWSTD_%s\n",
            "_RWSTD_LC_MIN", lc_vars [lc_min_inx].name);

    if (eq)
        *eq = '=';

    return 0;
}

/*********************************************************************/

int print_categories (const char *locname,
                      int         setlocale_environ,
                      int        &loc_name_use_cat,
                      int        &loc_name_prepend_sep,
                      int         loc_name_condense,
                      char       &loc_name_cat_sep,
                      char       &loc_name_cat_eq)
{
    // set or overwrite LC_ALL to prevent it from
    // overriding the settings below, and also reset
    // the global C locale to "C"
    char lc_all[] = "LC_ALL=";
    putenv (lc_all);
    setlocale (LC_ALL, "C");

    unsigned i;

    // set up the default environment (i.e., LC_COLLATE=C; LC_CTYPE=C; etc.)
    for (i = 0; lc_vars [i].cat != LC_ALL; ++i)
        putenv (lc_vars [i].name);

    for (i = 0; locname && lc_vars [i].cat != LC_ALL; ++i) {
        if (i) {
            if (setlocale_environ) {
                // replace previous LC_XXX environment variable
                strcpy (rw_strchr (lc_vars [i - 1].name, '=') + 1, "C");
                putenv (lc_vars [i - 1].name);
            }
            else {
                setlocale (lc_vars [i - 1].cat, "C");
            }
        }

        strcpy (rw_strchr (lc_vars [i].name, '=') + 1, locname);

        // add/modify variable to/in the environemnt (may or may not
        // be necessary depending on whether putenv() adds the actual
        // string or a copy of it
        putenv (lc_vars [i].name);

        // set the combined locale
        char *combined;
        
        if (setlocale_environ) {
            // create combined name from the environment
            combined = setlocale (LC_ALL, "");
        }
        else {
            // create combined name programmatically
            // by setting just one category
            setlocale (lc_vars [i].cat, locname);
            combined = setlocale (LC_ALL, 0);
        }

        // construct LC_XXX name
        char *eq = rw_strchr (lc_vars [i].name, '=');
        *eq = '\0';

        // look for LC_XXX string in locale name
        const char *where =
            combined ? rw_strstr (combined, lc_vars [i].name) : (char*)0;

        if (where) {
            // found the name of the category constant
            // in the name of the combined locale
            loc_name_use_cat = 1;

            // look for a separator between LC_XXX
            // and the locale name (typically '=')
            if (!loc_name_cat_eq)
                loc_name_cat_eq = where [strlen (lc_vars [i].name)];

            int j = -1;

            char* sep   = rw_strpbrk (combined, cat_seps);
            char* first = rw_strstr (combined, locname);

            for (const char *s = combined; *s && s != first; ++j)
                s = rw_strchr (s, loc_name_cat_eq) + 1;

            lc_vars [i].ord = j;

            // look for a separator between LC_XXX=name pairs
            // (typically ';')
            if (where != combined && !loc_name_cat_sep) {
                loc_name_cat_sep = where [-1];
            }
        }
        else {
            // look for a separator between locale categories
            char* sep = combined ? rw_strpbrk (combined, cat_seps) : (char*)0; 
            if (sep == combined)
                loc_name_prepend_sep = 1;

            if (sep)
                loc_name_cat_sep = *sep;

            int j = -loc_name_prepend_sep;

            if (sep) {
                char *first = rw_strstr (combined, locname);
                for (const char *s = combined; *s && s != first; ++j) {
                    s = rw_strchr (s, *sep);
                    if (s)
                        s += 1;
                    else
                        break;
                }

                lc_vars [i].ord = j;
            }
            else {
                // combined locale name is the same as the name
                // of one of the combining locales (e.g., Windows)
                lc_vars [i].ord = i;
            }
        }

        // replace the original '=' in the LC_XXX=name environment variable
        *eq = '=';

        if (combined && *combined == loc_name_cat_sep)
            loc_name_prepend_sep = 1;
    }
    
    for (i = 0; lc_vars [i].cat != LC_ALL; ++i) {

        char* eq = rw_strchr (lc_vars [i].name, '=');
        if (eq)
            *eq = '\0';

        const char *comment = 0;
        int inx = -1;

        for (int j = 0; *lc_vars [j].name; ++j) {
            if (lc_vars [i].ord == j) {
                comment = 0;
                inx     = j;
                break;
            }

            if (lc_vars [i].cat == lc_cat_order [j]) {
                comment = "   // assumed";
                inx     = j;
            }
        }

        if (-1 < inx)
            printf ("#define _RWSTD_CAT_%d(pfx) "
                    "{ %d, \"%s\", pfx::_C_%s }%s\n",
                    inx, lc_vars [i].cat, lc_vars [i].name,
                    lc_vars [i].lower, comment ? comment : "");
    }

    return 0;
}

/*********************************************************************/

int print_locale_name_format (int  guess,
                              int  setlocale_environ,
                              int  loc_name_use_cat,
                              int  loc_name_prepend_sep,
                              int  loc_name_condense,
                              char loc_name_cat_sep,
                              char loc_name_cat_eq)
{
    const char* os_name = 0;

    if (guess) {

#if defined (_AIX)

        setlocale_environ    = 1;
        loc_name_use_cat     = 0;
        loc_name_prepend_sep = 0;
        loc_name_condense    = 0;
        loc_name_cat_sep     = ' ';
        loc_name_cat_eq      = '\0';
        os_name              = "AIX";

#elif defined (__FreeBSD__)

        setlocale_environ    = 1;
        loc_name_use_cat     = 0;
        loc_name_prepend_sep = 0;
        loc_name_condense    = 1;
        loc_name_cat_sep     = '/';
        loc_name_cat_eq      = '\0';
        os_name              = "FreeBSD";

#elif defined (__hpux)

        setlocale_environ    = 1;
        loc_name_use_cat     = 0;
        loc_name_prepend_sep = 0;
        loc_name_condense    = 0;
        loc_name_cat_sep     = ' ';
        loc_name_cat_eq      = '\0';
        os_name              = "HP-UX";

#elif defined (__GLIBC__)

        setlocale_environ    = 1;
        loc_name_use_cat     = 1;
        loc_name_prepend_sep = 0;
        loc_name_condense    = 0;
        loc_name_cat_sep     = ';';
        loc_name_cat_eq      = '=';
        os_name              = "GNU libc";

#elif defined (__NetBSD__)

        setlocale_environ    = 0;
        loc_name_use_cat     = 0;
        loc_name_prepend_sep = 0;
        loc_name_condense    = 1;
        loc_name_cat_sep     = '\0';
        loc_name_cat_eq      = '\0';
        os_name              = "NetBSD";

#elif defined (__osf__)

        setlocale_environ    = 1;
        loc_name_use_cat     = 0;
        loc_name_prepend_sep = 0;
        loc_name_condense    = 0;
        loc_name_cat_sep     = ' ';
        loc_name_cat_eq      = '\0';
        os_name              = "Tru64 UNIX";

#elif defined (__sgi)

        setlocale_environ    = 1;
        loc_name_use_cat     = 0;
        loc_name_prepend_sep = 1;
        loc_name_condense    = 1;
        loc_name_cat_sep     = '/';
        loc_name_cat_eq      = '\0';
        os_name              = "SGI IRIX";

#elif    (defined (__sun__) || defined (__sun) || defined (sun)) \
      && defined (__svr4__)

        setlocale_environ    = 1;
        loc_name_use_cat     = 0;
        loc_name_prepend_sep = 1;
        loc_name_condense    = 1;
        loc_name_cat_sep     = '/';
        loc_name_cat_eq      = '\0';
        os_name              = "SunOS";

#elif defined (_WIN32)

        setlocale_environ    = 0;
        loc_name_use_cat     = 1;
        loc_name_prepend_sep = 0;
        loc_name_condense    = 1;
        loc_name_cat_sep     = ';';
        loc_name_cat_eq      = '=';
        os_name              = "Windows";

#elif defined (__CYGWIN__)

        // guessing this might be the same as Windows
        setlocale_environ    = 0;
        loc_name_use_cat     = 1;
        loc_name_prepend_sep = 0;
        loc_name_condense    = 1;
        loc_name_cat_sep     = ';';
        loc_name_cat_eq      = '=';
        // change the OS name to CygWin as soon as CygWin
        // has implemented locale support
        os_name              = "Windows";

#else

        printf ("// no locales found, unknown system\n");
        return -1;   // unknown OS, fail

#endif

    }

    if (os_name)
        printf ("// no locales found, using %s format\n", os_name);

    if (setlocale_environ)
        printf ("// #define _RWSTD_NO_SETLOCALE_ENVIRONMENT\n");
    else
        printf ("#define _RWSTD_NO_SETLOCALE_ENVIRONMENT\n");

    if (loc_name_use_cat)
        printf ("// #define _RWSTD_NO_CAT_NAMES\n");
    else
        printf ("#define _RWSTD_NO_CAT_NAMES\n");

    if (loc_name_cat_sep)
        printf ("#define _RWSTD_CAT_SEP \"%c\"\n", loc_name_cat_sep);
    else
        printf ("#define _RWSTD_NO_CAT_SEP\n");

    if (loc_name_cat_eq)
        printf ("#define _RWSTD_CAT_EQ \"%c\"\n", loc_name_cat_eq);
    else
        printf ("#define _RWSTD_NO_CAT_EQ\n");

    if (loc_name_prepend_sep)
        printf ("// #define _RWSTD_NO_INITIAL_CAT_SEP\n");
    else
        printf ("#define _RWSTD_NO_INITIAL_CAT_SEP\n");

    if (loc_name_condense)
        printf ("// #define _RWSTD_NO_CONDENSED_NAME\n");
    else
        printf ("#define _RWSTD_NO_CONDENSED_NAME\n");

    return 0;
}
