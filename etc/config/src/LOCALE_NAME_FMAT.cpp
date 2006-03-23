// determining combined locale name format

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


#if !defined (_WIN32) && !defined (_WIN64) || defined (__CYGWIN__)
char cat_seps[] = " \n\t/\\:;#%";
#else
char cat_seps[] = "\n\t/\\:;#%";
#endif


int main ()
{
#if !defined (_RWSTD_USE_CONFIG)

    printf ("/**/\n#undef _RWSTD_LOCALE_NAME_FMAT\n");

#endif   // _RWSTD_USE_CONFIG

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

    char namebuf [1024];
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
            char def_locale [256];
            def_locale [0] = '\0';

            const char *tmpname = setlocale (LC_ALL, "");
            if (tmpname)
                strcpy (def_locale, tmpname);

            char buf [256];
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

#ifdef LC_MESSAGES
    { -1, LC_MESSAGES, "LC_MESSAGES=C", "messages" },
#endif

#ifdef LC_NAME
    { -1, LC_NAME, "LC_NAME=C", "name" },
#endif

#ifdef LC_PAPER
    { -1, LC_PAPER, "LC_PAPER=C", "paper" },
#endif

#ifdef LC_IDENTIFICATION
    { -1, LC_IDENTIFICATION, "LC_IDENTIFICATION=C", "ident" },
#endif

#ifdef LC_ADDRESS
    { -1, LC_ADDRESS, "LC_ADDRESS=C", "address" },
#endif

#ifdef LC_TELEPHONE
    { -1, LC_TELEPHONE, "LC_TELEPHONE=C", "telephone" },
#endif

#ifdef LC_MEASUREMENT
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

#ifdef _AIX

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

/*********************************************************************/

const char* const test_locale_names[] = {
    "ar", "ar_EG.UTF-8", "bg_BG", "bg_BG.ISO8859-5", "ca", "ca_ES",


    "C.iso88591",
    "C.iso885915",
    "C.utf8",
    "DE_AT",
    "DE_AT.UTF-8",
    "DE_AT.UTF-8@euro",
    "DE_AT@euro",
    "DE_CH",
    "DE_CH.UTF-8",
    "DE_DE",
    "DE_DE.UTF-8",
    "DE_DE.UTF-8@euro",
    "DE_DE@euro",
    "DE_LU",
    "DE_LU.UTF-8",
    "DE_LU.UTF-8@euro",
    "DE_LU@euro",
    "De_CH",
    "De_CH.IBM-850",
    "De_DE",
    "De_DE.IBM-850",
    "FR_BE",
    "FR_BE.UTF-8",
    "FR_BE.UTF-8@euro",
    "FR_BE@euro",
    "FR_CA",
    "FR_CA.UTF-8",
    "FR_CH",
    "FR_CH.UTF-8",
    "FR_FR",
    "FR_FR.UTF-8",
    "FR_FR.UTF-8@euro",
    "FR_FR@euro",
    "FR_LU",
    "FR_LU.UTF-8",
    "FR_LU.UTF-8@euro",
    "FR_LU@euro",
    "Fr_BE",
    "Fr_BE.IBM-850",
    "Fr_CA",
    "Fr_CA.IBM-850",
    "Fr_CH",
    "Fr_CH.IBM-850",
    "Fr_FR",
    "Fr_FR.IBM-850",
    "JA_JP",
    "JA_JP.UTF-8",
    "Ja_JP",
    "Ja_JP.IBM-932",
    "Ja_JP.IBM-943",
    "af_ZA",
    "ar",
    "ar_AE",
    "ar_AE.utf8",
    "ar_BH",
    "ar_BH.utf8",
    "ar_DZ",
    "ar_DZ.arabic8",
    "ar_DZ.utf8",
    "ar_EG",
    "ar_EG.UTF-8",
    "ar_EG.utf8",
    "ar_IN",
    "ar_IQ",
    "ar_IQ.utf8",
    "ar_JO",
    "ar_JO.utf8",
    "ar_KW",
    "ar_KW.utf8",
    "ar_LB",
    "ar_LB.utf8",
    "ar_LY",
    "ar_LY.utf8",
    "ar_MA",
    "ar_MA.utf8",
    "ar_OM",
    "ar_OM.utf8",
    "ar_QA",
    "ar_QA.utf8",
    "ar_SA",
    "ar_SA.arabic8",
    "ar_SA.iso88596",
    "ar_SA.utf8",
    "ar_SD",
    "ar_SD.utf8",
    "ar_SY",
    "ar_SY.utf8",
    "ar_TN",
    "ar_TN.utf8",
    "ar_YE",
    "ar_YE.utf8",
    "be_BY",
    "be_BY.utf8",
    "bg_BG",
    "bg_BG.ISO8859-5",
    "bg_BG.iso88595",
    "bg_BG.utf8",
    "bokmal",
    "bokmål",
    "br",
    "br_FR",
    "bs_BA",
    "ca",
    "ca_ES",
    "ca_ES.ISO8859-1",
    "ca_ES.ISO8859-15",
    "ca_ES.ISO8859-15@euro",
    "ca_ES.utf8",
    "ca_ES.utf8@euro",
    "ca_ES@euro",
    "catalan",
    "chinese",
    "common",
    "croatian",
    "cs",
    "cs.po",
    "cs_CZ",
    "cs_CZ.ISO8859-2",
    "cs_CZ.iso88592",
    "cs_CZ.utf8",
    "cy_GB",
    "cz",
    "czech",
    "da",
    "da.ISO8859-15",
    "da.po",
    "da_DK",
    "da_DK.ISO8859-1",
    "da_DK.ISO8859-15",
    "da_DK.ISO8859-15@euro",
    "da_DK.UTF-8",
    "da_DK.iso88591",
    "da_DK.iso885915@euro",
    "da_DK.roman8",
    "da_DK.utf8",
    "danish",
    "dansk",
    "de",
    "de.ISO8859-15",
    "de.UTF-8",
    "de.po",
    "de_AT",
    "de_AT.8859-15",
    "de_AT.8859-15@euro",
    "de_AT.ISO8859-1",
    "de_AT.ISO8859-15",
    "de_AT.ISO8859-15@euro",
    "de_AT.utf8",
    "de_AT.utf8@euro",
    "de_AT@euro",
    "de_BE",
    "de_BE.utf8",
    "de_BE.utf8@euro",
    "de_BE@euro",
    "de_CH",
    "de_CH.8859-15",
    "de_CH.ISO8859-1",
    "de_CH.ISO8859-15",
    "de_CH.utf8",
    "de_DE",
    "de_DE.8859-15",
    "de_DE.8859-15@euro",
    "de_DE.IBM-1252",
    "de_DE.IBM-1252@euro",
    "de_DE.ISO8859-1",
    "de_DE.ISO8859-15",
    "de_DE.ISO8859-15@euro",
    "de_DE.UTF-8",
    "de_DE.UTF-8@euro",
    "de_DE.iso88591",
    "de_DE.iso885915@euro",
    "de_DE.roman8",
    "de_DE.utf8",
    "de_DE.utf8@euro",
    "de_DE@euro",
    "de_LU",
    "de_LU.8859-15",
    "de_LU.8859-15@euro",
    "de_LU.utf8",
    "de_LU.utf8@euro",
    "de_LU@euro",
    "deutsch",
    "dk",
    "dutch",
    "eesti",
    "el",
    "el.sun_eu_greek",
    "el_GR",
    "el_GR.ISO8859-7",
    "el_GR.ISO8859-7@euro",
    "el_GR.greek8",
    "el_GR.iso88597",
    "el_GR.utf8",
    "en",
    "en_AU",
    "en_AU.ISO8859-1",
    "en_AU.ISO8859-15",
    "en_AU.utf8",
    "en_BW",
    "en_BW.utf8",
    "en_CA",
    "en_CA.ISO8859-1",
    "en_CA.ISO8859-15",
    "en_CA.utf8",
    "en_DK",
    "en_DK.utf8",
    "en_EU.UTF-8@euro",
    "en_GB",
    "en_GB.ISO8859-1",
    "en_GB.ISO8859-15",
    "en_GB.ISO8859-15@euro",
    "en_GB.UTF-8",
    "en_GB.iso88591",
    "en_GB.iso885915@euro",
    "en_GB.roman8",
    "en_GB.utf8",
    "en_HK",
    "en_HK.utf8",
    "en_IE",
    "en_IE.ISO8859-1",
    "en_IE.ISO8859-15",
    "en_IE.ISO8859-15@euro",
    "en_IE.utf8",
    "en_IE.utf8@euro",
    "en_IE@euro",
    "en_IN",
    "en_NZ",
    "en_NZ.ISO8859-1",
    "en_NZ.utf8",
    "en_PH",
    "en_PH.utf8",
    "en_RN",
    "en_SG",
    "en_SG.utf8",
    "en_UK",
    "en_US",
    "en_US.8859-15",
    "en_US.ISO8859-1",
    "en_US.ISO8859-15",
    "en_US.ISO8859-15@euro",
    "en_US.UTF-8",
    "en_US.UTF-8@euro",
    "en_US.cp850",
    "en_US.iso88591",
    "en_US.roman8",
    "en_US.utf8",
    "en_ZA",
    "en_ZA.utf8",
    "en_ZW",
    "en_ZW.utf8",
    "eo",
    "es",
    "es.ISO8859-15",
    "es.UTF-8",
    "es.po",
    "es_AR",
    "es_AR.ISO8859-1",
    "es_AR.ISO8859-15",
    "es_AR.utf8",
    "es_BO",
    "es_BO.ISO8859-1",
    "es_BO.utf8",
    "es_CL",
    "es_CL.ISO8859-1",
    "es_CL.utf8",
    "es_CO",
    "es_CO.ISO8859-1",
    "es_CO.utf8",
    "es_CR",
    "es_CR.ISO8859-1",
    "es_CR.utf8",
    "es_DO",
    "es_DO.utf8",
    "es_EC",
    "es_EC.ISO8859-1",
    "es_EC.utf8",
    "es_ES",
    "es_ES.ISO8859-1",
    "es_ES.ISO8859-15",
    "es_ES.ISO8859-15@euro",
    "es_ES.UTF-8",
    "es_ES.UTF-8@euro",
    "es_ES.iso88591",
    "es_ES.iso885915@euro",
    "es_ES.roman8",
    "es_ES.utf8",
    "es_ES.utf8@euro",
    "es_ES@euro",
    "es_GT",
    "es_GT.ISO8859-1",
    "es_GT.utf8",
    "es_HN",
    "es_HN.utf8",
    "es_MX",
    "es_MX.ISO8859-1",
    "es_MX.ISO8859-15",
    "es_MX.utf8",
    "es_NI",
    "es_NI.ISO8859-1",
    "es_NI.utf8",
    "es_PA",
    "es_PA.ISO8859-1",
    "es_PA.utf8",
    "es_PE",
    "es_PE.ISO8859-1",
    "es_PE.utf8",
    "es_PR",
    "es_PR.utf8",
    "es_PY",
    "es_PY.ISO8859-1",
    "es_PY.utf8",
    "es_SV",
    "es_SV.ISO8859-1",
    "es_SV.utf8",
    "es_US",
    "es_US.utf8",
    "es_UY",
    "es_UY.ISO8859-1",
    "es_UY.utf8",
    "es_VE",
    "es_VE.ISO8859-1",
    "es_VE.utf8",
    "estonian",
    "et",
    "et_EE",
    "et_EE.ISO8859-15",
    "et_EE.utf8",
    "eu_ES",
    "eu_ES.utf8",
    "eu_ES.utf8@euro",
    "eu_ES@euro",
    "fa_IR",
    "fi",
    "fi.ISO8859-15",
    "fi.po",
    "fi_FI",
    "fi_FI.ISO8859-1",
    "fi_FI.ISO8859-15",
    "fi_FI.ISO8859-15@euro",
    "fi_FI.UTF-8",
    "fi_FI.iso88591",
    "fi_FI.iso885915@euro",
    "fi_FI.roman8",
    "fi_FI.utf8",
    "fi_FI.utf8@euro",
    "fi_FI@euro",
    "finnish",
    "fo_FO",
    "fo_FO.utf8",
    "fr",
    "fr.ISO8859-15",
    "fr.UTF-8",
    "fr.po",
    "fr_BE",
    "fr_BE.8859-15",
    "fr_BE.8859-15@euro",
    "fr_BE.IBM-1252",
    "fr_BE.IBM-1252@euro",
    "fr_BE.ISO8859-1",
    "fr_BE.ISO8859-15",
    "fr_BE.ISO8859-15@euro",
    "fr_BE.UTF-8",
    "fr_BE.UTF-8@euro",
    "fr_BE.utf8",
    "fr_BE.utf8@euro",
    "fr_BE@euro",
    "fr_CA",
    "fr_CA.8859-15",
    "fr_CA.ISO8859-1",
    "fr_CA.ISO8859-15",
    "fr_CA.iso88591",
    "fr_CA.iso885915",
    "fr_CA.roman8",
    "fr_CA.utf8",
    "fr_CH",
    "fr_CH.8859-15",
    "fr_CH.ISO8859-1",
    "fr_CH.ISO8859-15",
    "fr_CH.utf8",
    "fr_FR",
    "fr_FR.8859-15",
    "fr_FR.8859-15@euro",
    "fr_FR.IBM-1252",
    "fr_FR.IBM-1252@euro",
    "fr_FR.ISO8859-1",
    "fr_FR.ISO8859-15",
    "fr_FR.ISO8859-15@euro",
    "fr_FR.UTF-8",
    "fr_FR.UTF-8@euro",
    "fr_FR.iso88591",
    "fr_FR.iso885915@euro",
    "fr_FR.roman8",
    "fr_FR.utf8",
    "fr_FR.utf8@euro",
    "fr_FR@euro",
    "fr_LU",
    "fr_LU.8859-15",
    "fr_LU.8859-15@euro",
    "fr_LU.utf8",
    "fr_LU.utf8@euro",
    "fr_LU@euro",
    "french",
    "ga",
    "ga_IE",
    "ga_IE.utf8",
    "ga_IE.utf8@euro",
    "ga_IE@euro",
    "galego",
    "galician",
    "german",
    "gl",
    "gl_ES",
    "gl_ES.utf8",
    "gl_ES.utf8@euro",
    "gl_ES@euro",
    "greek",
    "gv_GB",
    "gv_GB.utf8",
    "he",
    "he_IL",
    "he_IL.UTF-8",
    "he_IL.utf8",
    "hebrew",
    "hi_IN",
    "hi_IN.UTF-8",
    "hr",
    "hr_HR",
    "hr_HR.ISO8859-2",
    "hr_HR.iso88592",
    "hr_HR.utf8",
    "hrvatski",
    "hu",
    "hu_HU",
    "hu_HU.ISO8859-2",
    "hu_HU.iso88592",
    "hu_HU.utf8",
    "hungarian",
    "icelandic",
    "id_ID",
    "id_ID.utf8",
    "in",
    "in_ID",
    "is",
    "is.po",
    "is_IS",
    "is_IS.ISO8859-1",
    "is_IS.ISO8859-15",
    "is_IS.iso88591",
    "is_IS.iso885915@euro",
    "is_IS.roman8",
    "is_IS.utf8",
    "iso_8859_1",
    "iso_8859_13",
    "iso_8859_15",
    "iso_8859_2",
    "iso_8859_5",
    "iso_8859_7",
    "iso_8859_9",
    "it",
    "it.ISO8859-15",
    "it.UTF-8",
    "it_CH",
    "it_CH.ISO8859-15",
    "it_CH.utf8",
    "it_IT",
    "it_IT.ISO8859-1",
    "it_IT.ISO8859-15",
    "it_IT.ISO8859-15@euro",
    "it_IT.UTF-8",
    "it_IT.UTF-8@euro",
    "it_IT.iso88591",
    "it_IT.iso885915@euro",
    "it_IT.roman8",
    "it_IT.utf8",
    "it_IT.utf8@euro",
    "it_IT@euro",
    "italian",
    "iw_IL",
    "iw_IL.hebrew8",
    "iw_IL.iso88598",
    "iw_IL.utf8",
    "ja",
    "ja_JP",
    "ja_JP.EUC",
    "ja_JP.IBM-eucJP",
    "ja_JP.PCK",
    "ja_JP.SJIS",
    "ja_JP.SJIS@ucs4",
    "ja_JP.UTF-8",
    "ja_JP.deckanji",
    "ja_JP.deckanji@ucs4",
    "ja_JP.eucJP",
    "ja_JP.eucjp",
    "ja_JP.kana8",
    "ja_JP.sdeckanji",
    "ja_JP.ujis",
    "ja_JP.utf8",
    "japanese",
    "japanese.euc",
    "japanese.sjis",
    "ka_GE",
    "kl_GL",
    "kl_GL.utf8",
    "ko",
    "ko.UTF-8",
    "ko_KR",
    "ko_KR.EUC",
    "ko_KR.EUC@dict",
    "ko_KR.UTF-8",
    "ko_KR.UTF-8@dict",
    "ko_KR.eucKR",
    "ko_KR.euckr",
    "ko_KR.utf8",
    "korean",
    "korean.euc",
    "kw_GB",
    "kw_GB.utf8",
    "lithuanian",
    "lt",
    "lt_LT",
    "lt_LT.ISO8859-13",
    "lt_LT.utf8",
    "lv",
    "lv_LV",
    "lv_LV.ISO8859-13",
    "lv_LV.utf8",
    "mi_NZ",
    "mk",
    "mk_MK",
    "mk_MK.ISO8859-5",
    "mk_MK.utf8",
    "mr_IN",
    "ms_MY",
    "ms_MY.utf8",
    "mt_MT",
    "mt_MT.utf8",
    "nb_NO",
    "nb_NO.ISO-8859-1",
    "nl",
    "nl.ISO8859-15",
    "nl_BE",
    "nl_BE.ISO8859-1",
    "nl_BE.ISO8859-15",
    "nl_BE.ISO8859-15@euro",
    "nl_BE.UTF-8",
    "nl_BE.utf8",
    "nl_BE.utf8@euro",
    "nl_BE@euro",
    "nl_NL",
    "nl_NL.ISO8859-1",
    "nl_NL.ISO8859-15",
    "nl_NL.ISO8859-15@euro",
    "nl_NL.UTF-8",
    "nl_NL.iso88591",
    "nl_NL.iso885915@euro",
    "nl_NL.roman8",
    "nl_NL.utf8",
    "nl_NL.utf8@euro",
    "nl_NL@euro",
    "nn_NO",
    "nn_NO.utf8",
    "no",
    "no.po",
    "no@nynorsk",
    "no_NO",
    "no_NO.ISO8859-1",
    "no_NO.ISO8859-15",
    "no_NO.ISO8859-1@bokmal",
    "no_NO.ISO8859-1@nynorsk",
    "no_NO.UTF-8",
    "no_NO.iso88591",
    "no_NO.iso885915@euro",
    "no_NO.roman8",
    "no_NO.utf8",
    "no_NY",
    "norwegian",
    "nr",
    "nynorsk",
    "oc_FR",
    "pl",
    "pl.UTF-8",
    "pl.po",
    "pl_PL",
    "pl_PL.ISO8859-2",
    "pl_PL.UTF-8",
    "pl_PL.iso88592",
    "pl_PL.utf8",
    "polish",
    "portuguese",
    "pt",
    "pt.ISO8859-15",
    "pt_BR",
    "pt_BR.ISO8859-1",
    "pt_BR.ISO8859-15",
    "pt_BR.UTF-8",
    "pt_BR.po",
    "pt_BR.utf8",
    "pt_PT",
    "pt_PT.ISO8859-1",
    "pt_PT.ISO8859-15",
    "pt_PT.ISO8859-15@euro",
    "pt_PT.UTF-8",
    "pt_PT.iso88591",
    "pt_PT.iso885915@euro",
    "pt_PT.roman8",
    "pt_PT.utf8",
    "pt_PT.utf8@euro",
    "pt_PT@euro",
    "ro",
    "ro_RO",
    "ro_RO.ISO8859-2",
    "ro_RO.iso88592",
    "ro_RO.utf8",
    "romanian",
    "ru",
    "ru.UTF-8",
    "ru.koi8-r",
    "ru_RU",
    "ru_RU.ANSI1251",
    "ru_RU.ISO8859-5",
    "ru_RU.KOI8-R",
    "ru_RU.UTF-8",
    "ru_RU.iso88595",
    "ru_RU.koi8r",
    "ru_RU.utf8",
    "ru_SU",
    "ru_UA",
    "ru_UA.utf8",
    "russian",
    "se",
    "sh_BA",
    "sh_BA.ISO8859-2@bosnia",
    "sk",
    "sk_SK",
    "sk_SK.ISO8859-2",
    "sk_SK.iso88592",
    "sk_SK.utf8",
    "sl",
    "sl_SI",
    "sl_SI.ISO8859-2",
    "sl_SI.iso88592",
    "sl_SI.utf8",
    "slovak",
    "slovene",
    "slovenian",
    "spanish",
    "sq_AL",
    "sq_AL.ISO8859-2",
    "sq_AL.utf8",
    "sr",
    "sr_SP",
    "sr_YU",
    "sr_YU.ISO8859-5",
    "sr_YU.utf8",
    "sr_YU.utf8@cyrillic",
    "sr_YU@cyrillic",
    "sv",
    "sv.ISO8859-15",
    "sv.UTF-8",
    "sv_FI",
    "sv_FI.utf8",
    "sv_FI.utf8@euro",
    "sv_FI@euro",
    "sv_SE",
    "sv_SE.ISO8859-1",
    "sv_SE.ISO8859-15",
    "sv_SE.ISO8859-15@euro",
    "sv_SE.UTF-8",
    "sv_SE.UTF-8@euro",
    "sv_SE.iso88591",
    "sv_SE.iso885915@euro",
    "sv_SE.roman8",
    "sv_SE.utf8",
    "swedish",
    "ta_IN",
    "te_IN",
    "tg_TJ",
    "th",
    "th_TH",
    "th_TH.ISO8859-11",
    "th_TH.TIS620",
    "th_TH.UTF-8",
    "th_TH.tis620",
    "th_TH.utf8",
    "thai",
    "tl_PH",
    "tr",
    "tr_TR",
    "tr_TR.ISO8859-9",
    "tr_TR.UTF-8",
    "tr_TR.iso88599",
    "tr_TR.turkish8",
    "tr_TR.utf8",
    "turkish",
    "ua",
    "uk",
    "uk_UA",
    "uk_UA.utf8",
    "univ.utf8",
    "universal.UTF-8",
    "universal.utf8@ucs4",
    "ur_PK",
    "uz_UZ",
    "vi_VN",
    "wa",
    "yi_US",
    "zh",
    "zh.GBK",
    "zh.UTF-8",
    "zh_CN",
    "zh_CN.EUC",
    "zh_CN.EUC@pinyin",
    "zh_CN.EUC@radical",
    "zh_CN.EUC@stroke",
    "zh_CN.GB18030",
    "zh_CN.GB18030@pinyin",
    "zh_CN.GB18030@radical",
    "zh_CN.GB18030@stroke",
    "zh_CN.GB2312",
    "zh_CN.GBK",
    "zh_CN.GBK@pinyin",
    "zh_CN.GBK@radical",
    "zh_CN.GBK@stroke",
    "zh_CN.UTF-8",
    "zh_CN.UTF-8@pinyin",
    "zh_CN.UTF-8@radical",
    "zh_CN.UTF-8@stroke",
    "zh_CN.gb18030",
    "zh_CN.gbk",
    "zh_CN.hp15CN",
    "zh_CN.utf8",
    "zh_HK",
    "zh_HK.BIG5HK",
    "zh_HK.BIG5HK@radical",
    "zh_HK.BIG5HK@stroke",
    "zh_HK.UTF-8",
    "zh_HK.UTF-8@radical",
    "zh_HK.UTF-8@stroke",
    "zh_HK.big5",
    "zh_HK.utf8",
    "zh_TW",
    "zh_TW.BIG5",
    "zh_TW.BIG5@pinyin",
    "zh_TW.BIG5@radical",
    "zh_TW.BIG5@stroke",
    "zh_TW.BIG5@zhuyin",
    "zh_TW.Big5",
    "zh_TW.EUC",
    "zh_TW.EUC@pinyin",
    "zh_TW.EUC@radical",
    "zh_TW.EUC@stroke",
    "zh_TW.EUC@zhuyin",
    "zh_TW.UTF-8",
    "zh_TW.UTF-8@pinyin",
    "zh_TW.UTF-8@radical",
    "zh_TW.UTF-8@stroke",
    "zh_TW.UTF-8@zhuyin",
    "zh_TW.big5",
    "zh_TW.ccdc",
    "zh_TW.eucTW",
    "zh_TW.euctw",
    "zh_TW.utf8",

    // Windows names
    "ENU", "ENG", "ENA", "ENC", "ENZ", "ENI", "ENS", "ENJ", "ENB", "ENL",
    "ENT", "BGR", "CSY", "DAN", "DEU", "DES", "DEA", "DEL", "DEC", "ELL",
    "ESP", "ESM", "ESN", "ESG", "ESC", "ESA", "ESD", "ESV", "ESO", "ESR",
    "ESS", "ESF", "ESL", "ESY", "ESZ", "ESB", "ESE", "ESH", "ESI", "ESU",
    "FIN", "FRA", "FRB", "FRC", "FRS", "FRL", "HUN", "ISL", "ITA", "ITS",
    "NLD", "NLB", "NOR", "NON", "PLK", "PTB", "PTG", "ROM", "RUS", "HRV",
    "SRL", "SRB", "SKY", "SVE", "SVF", "TRK", "SLV", "AFK", "SQI", "EUQ",
    "BEL", "CAT", "ETI", "FOS", "IND", "LVI", "LTH", "UKR"
};

const unsigned nlocales = sizeof test_locale_names / sizeof *test_locale_names;
