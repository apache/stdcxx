/************************************************************************
 *
 * 0.locale.cpp - test exercising the test driver locale helpers
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
 * Copyright 2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include <cstdio>        // for remove()
#include <cstdlib>       // for getenv()

#include <rw_environ.h>  // for rw_putenv()
#include <rw_file.h>     // for rw_fwrite()
#include <rw_driver.h>   // for rw_test(), etc.
#include <rw_locale.h>   // for rw_localedef()
#include <rw_printf.h>   // for rw_fprintf()

/**************************************************************************/

static int
test_localedef ()
{
    // the root of the locale directory (RWSTD_LOCALE_ROOT)
    static const char* const locale_root = rw_set_locale_root ();

    static int inx;

    const int id = inx++;

    // create a temporary locale definition file
    char srcfname [1024];
    if (rw_snprintf (srcfname, sizeof srcfname, "%s%c%s.%d",
                     locale_root, _RWSTD_PATH_SEP, "locale", id) < 0)
        return -1;

    // create a temporary character map file
    char cmfname [1024];
    if (rw_snprintf (cmfname, sizeof cmfname, "%s%c%s.%d.src",
                     locale_root, _RWSTD_PATH_SEP, "charmap", id) < 0)
        return -1;

    rw_fwrite (srcfname, "LC_CTYPE\nEND LC_CTYPE\n");
    rw_fwrite (cmfname, "CHARMAP\nEND CHARMAP\n");

    const char opts[]    = "-w";
    char locname[40];
    rw_snprintf (locname, sizeof locname, "%s.%d", "locale", id);

    // try to create the locale
    const char* const ret =
        rw_localedef (opts, srcfname, cmfname, locname);

    const char* const topdir = std::getenv ("TOPDIR");

    rw_assert (0 != ret, 0, __LINE__,
               "rw_localedef(%s, %#s, %#s, %#s) failed with TOPDIR=%#s",
               opts, srcfname, cmfname, locname, topdir);

    // remove temporary files
    std::remove (cmfname);
    std::remove (srcfname);

    return 0;
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    // exercise with the default setting of TOPDIR
    rw_error (0 == test_localedef (), 0, __LINE__, "");

    // exercise with TOPDIR set but empty
    rw_putenv ("TOPDIR=");
    rw_error (0 == test_localedef (), 0, __LINE__, "");

    // exercise with TOPDIR unset
    rw_putenv ("TOPDIR");
    rw_error (0 == test_localedef (), 0, __LINE__, "");

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    0 /* no section */,
                    0 /* no comment */,
                    run_test,
                    "",
                    0);
}
