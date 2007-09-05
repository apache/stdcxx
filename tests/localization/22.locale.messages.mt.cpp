/************************************************************************
 *
 * 22.locale.messages.mt.cpp
 *
 * test exercising the thread safety of the messages facet
 *
 * $Id$
 *
 ***************************************************************************
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
 **************************************************************************/

#include <locale>

#include <driver.h>      // for rw_test()
#include <environ.h>     // for rw_putenv()
#include <file.h>        // for rw_nextfd()
#include <rw_locale.h>   // for rw_locales()
#include <rw_process.h>  // for rw_system()
#include <rw_thread.h>
#include <valcmp.h>    // for rw_strncmp ()

#ifndef _MSC_VER
#  include <nl_types.h>  // for cat operations
#endif

#include <cstring>   // for strlen()
#include <cstdlib>   // for getcwd(), getenv()
#include <cstdio>    // for FILE, fopen(), fprintf()
#include <clocale>   // for LC_ALL

#include <cwchar>    // for mbsinit()


#ifndef _RWSTD_NO_NEW_HEADER
#  include <stdio.h>         // for fileno()
#  if defined (_MSC_VER)
#    include <io.h>          // for _open()
#    include <direct.h>
#  else
#    include <sys/types.h>
#    include <sys/stat.h>
#    include <unistd.h>      // for getcwd()
#  endif
#  include <fcntl.h>         // for mode flags for _open
#endif   // _RWSTD_NO_NEW_HEADER

#undef open
#undef close

#if defined (_MSC_VER)
#  define open(f,m) _open  (f, _##m)
#  define close(f)  _close (f)
#else
#  define open(f,m) open  (f, m)
#  define close(f)  close (f)
#endif // defined (_MSC_VER)


// maximum number of threads allowed by the command line interface
#define MAX_THREADS      32
#define MAX_LOOPS    100000

// default number of threads (will be adjusted to the number
// of processors/cores later)
int rw_opt_nthreads = 1;

// the number of times each thread should iterate (unless specified
// otherwise on the command line)
int rw_opt_nloops = 100000;

// locale for threads to share
static const
std::locale locale;

// message catalog for threads to share
static
std::messages_base::catalog catalog;

static
std::messages_base::catalog wcatalog;

/**************************************************************************/

#ifndef _WIN32
#  define CAT_NAME "./rwstdmessages.cat"
#  define MSG_NAME "rwstdmessages.msg"
#else
#  define CAT_NAME "rwstdmessages.dll"
#  define MSG_NAME "rwstdmessages.rc"
#endif

#define NLS_CAT_NAME "rwstdmessages"

#define MAX_SETS 5
#define MAX_MESSAGES  5


int msg_id (int set, int id)
{
#ifdef _WIN32

    return (set - 1) * 5 + id;

#else

    _RWSTD_UNUSED (set);
    return id;

#endif
}

/**************************************************************************/

static const char* const
messages [MAX_SETS][MAX_MESSAGES] = {
    { "First set, first message",
      "First set, second message",
      "First set, third message",
      "First set, fourth message",
      "First set, fifth message"
    },
    { "Second set, first message",
      "Second set, second message",
      "Second set, third message",
      "Second set, fourth message",
      "Second set, fifth message"
    },
    { "Third set, first message",
      "Third set, second message",
      "Third set, third message",
      "Third set, fourth message",
      "Third set, fifth message"
    },
    { "Fourth set, first message",
      "Fourth set, second message",
      "Fourth set, third message",
      "Fourth set, fourth message",
      "Fourth set, fifth message"
    },
    { "Fifth set, first message",
      "Fifth set, second message",
      "Fifth set, third message",
      "Fifth set, fourth message",
      "Fifth set, fifth message"
    }
};


void generate_catalog (const char *msg_name,
                       const char* const text [MAX_SETS][MAX_MESSAGES])
{
    std::FILE* const f = std::fopen (msg_name, "w");

    if (!f)
        return;

#ifndef _WIN32

    for (int i = 0; i < MAX_SETS; ++i) {
        std::fprintf (f, "$set %d This is Set %d\n", i+1, i+1);
        for (int j = 0; j < MAX_MESSAGES; ++j) {
            std::fprintf (f, "%d %s\n", j + 1, text [i][j]);
        }
    }

#else   // if defined (_WIN32)

    std::fprintf (f, "STRINGTABLE\nBEGIN\n");
    for (int i = 0; i < MAX_SETS; ++i) {
        for (int j = 0; j < MAX_MESSAGES; ++j) {
            const int msgid = msg_id (i + 1, j + 1);
            std::fprintf (f, "%d \"%s\"\n", msgid, text[i][j]);
        }
    }

    std::fprintf (f, "END\n");

#endif   // _WIN32

    std::fclose (f);

    char *cat_name = new char [std::strlen (msg_name) + 1];
    const char *dot = std::strrchr (msg_name, '.');
    std::strncpy (cat_name, msg_name, dot - msg_name);
    *(cat_name + (dot - msg_name)) = '\0';

#ifndef _WIN32

    rw_system ("gencat %s.cat %s", cat_name, msg_name);

#else   // if defined (_WIN32)

    char cpp_name [128];

    std::sprintf (cpp_name, "%s.cpp", cat_name);

    std::FILE* const cpp_file = std::fopen (cpp_name, "w");
    std::fprintf (cpp_file, "void foo () { }");
    std::fclose (cpp_file);

    rw_system (   "rc -r %s.rc "
               "&& cl -nologo -c %s"
               "&& link -nologo /DLL /OUT:%s.dll %s.obj %s.res",
               cat_name,
               cpp_name,
               cat_name, cat_name, cat_name);

    rw_system (SHELL_RM_F "%s %s.rc %s.res %s.obj",
               cpp_name, cat_name, cat_name, cat_name);

#endif   // _WIN32

    delete[] cat_name;

    std::remove (msg_name);
}

/**************************************************************************/

template <class T>
void test_open_close (const std::locale& loc,
                      const std::messages<T>& msgs,
                      const std::string& name)
{
    std::messages_base::catalog cat =
        (msgs.open) (name, loc);

    RW_ASSERT (! (cat < 0));

    (msgs.close) (cat);
}

template <class T>
void test_get (const std::messages<T>& msgs,
               const std::messages_base::catalog cat,
               int set, int msgid,
               const std::basic_string<T>& dflt)
{
    // the msg_id() thing seems like a bug to me. if anything, the user
    // should never need to write or call msg_id().

    const typename std::messages<T>::string_type res =
        msgs.get (cat, set, msg_id (set, msgid), dflt);

    RW_ASSERT (!rw_strncmp (messages [set-1][msgid-1], res.c_str ()));
}

/**************************************************************************/

extern "C" {

bool test_char;    // exercise messages<char>
bool test_wchar;   // exercise messages<wchar_t>

static void*
thread_func (void*)
{
    const std::string name (CAT_NAME);

    const std::messages<char>& nmsgs =
        std::use_facet<std::messages<char> >(locale);

    const std::string ndflt ("\1\2\3\4");

#ifndef _RWSTD_NO_WCHAR_T
    const std::messages<wchar_t>& wmsgs =
        std::use_facet<std::messages<wchar_t> >(locale);

    const std::wstring wdflt (L"\1\2\3\4");
#endif // _RWSTD_NO_WCHAR_T

    for (int i = 0; i != rw_opt_nloops; ++i) {

        int set   = 1 + i % MAX_SETS;
        int msgid = 1 + i % MAX_MESSAGES;

        if (test_char) {
            if (i & 1) {
                test_get<char>(nmsgs, catalog, set, msgid, ndflt);
            }
            else {
                test_open_close<char>(locale, nmsgs, name);
            }
        }

        if (test_wchar) {

#ifndef _RWSTD_NO_WCHAR_T

            if (i & 1) {
                test_get<wchar_t>(wmsgs, wcatalog, set, msgid, wdflt);
            }
            else {
                test_open_close<wchar_t>(locale, wmsgs, name);
            }

#endif // _RWSTD_NO_WCHAR_T

        }
    }

    return 0;
}

}   // extern "C"

/**************************************************************************/

static int
run_test (int, char**)
{
    // generate a message catalog
    generate_catalog (MSG_NAME, messages);
    const std::string name (CAT_NAME);

    const std::messages<char>& nmsgs =
        std::use_facet<std::messages<char> >(locale);

    catalog = (nmsgs.open) (name, locale);

#ifndef _RWSTD_NO_WCHAR_T

    const std::messages<wchar_t>& wmsgs =
        std::use_facet<std::messages<wchar_t> >(locale);

    wcatalog = (wmsgs.open) (name, locale);

#endif

    rw_info (0, 0, 0,
             "testing std::messages<charT> with %d thread%{?}s%{;}, "
             "%zu iteration%{?}s%{;} each",
             rw_opt_nthreads, 1 != rw_opt_nthreads,
             rw_opt_nloops, 1 != rw_opt_nloops);

    ///////////////////////////////////////////////////////////////////////

    test_char   = true;
    test_wchar  = false;

    rw_info (0, 0, 0, "exercising std::messages<char>");

    int result = 
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0,
                        thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

    ///////////////////////////////////////////////////////////////////////

#ifndef _RWSTD_NO_WCHAR_T

    test_char   = false;
    test_wchar  = true;

    rw_info (0, 0, 0, "exercising std::messages<wchar_t>");

    result = 
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0,
                        thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

    ///////////////////////////////////////////////////////////////////////

    test_char   = true;
    test_wchar  = true;

    rw_info (0, 0, 0, "exercising std::messages<char> and "
                      "std::messages<wchar_t>");

    result = 
        rw_thread_pool (0, std::size_t (rw_opt_nthreads), 0,
                        thread_func, 0);

    rw_error (result == 0, 0, __LINE__,
              "rw_thread_pool(0, %d, 0, %{#f}, 0) failed",
              rw_opt_nthreads, thread_func);

#endif // _RWSTD_NO_WCHAR_T

    ///////////////////////////////////////////////////////////////////////

    (nmsgs.close) (catalog);

#ifndef _RWSTD_NO_WCHAR_T

    (wmsgs.close) (wcatalog);
    
#endif // _RWSTD_NO_WCHAR_T

    std::remove (CAT_NAME);

    return result;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
#ifdef _RWSTD_REENTRANT

    // set nthreads to the greater of the number of processors
    // and 2 (for uniprocessor systems) by default
    rw_opt_nthreads = rw_get_cpus ();
    if (rw_opt_nthreads < 2)
        rw_opt_nthreads = 2;

#endif   // _RWSTD_REENTRANT

    return rw_test (argc, argv, __FILE__,
                    "lib.locale.messages",
                    "thread safety", run_test,
                    "|-nloops#0 "        // must be non-negative
                    "|-nthreads#0-* ",    // must be in [0, MAX_THREADS]
                    &rw_opt_nloops,
                    int (MAX_THREADS),
                    &rw_opt_nthreads);
}
