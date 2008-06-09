/***************************************************************************
 *
 * 27.ios.members.static.cpp - test exercising [lib.ios.members.static]
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
 * Copyright 1994-2008 Rogue Wave Software.
 *
 **************************************************************************/

// exercise that the default setting of ios_base::sync_with_stdio()
// produces the expected output, i.e., that the ouptut of the standard
// iostream objects, std::cout, std::cerr, etc., is properly interleaved
// with the output produced by interleaving calls to stdio printf()
// also exercises the ability of buffered standard iostream objects
// (i.e., cout, wcout, clog, and wclog) to flush output on program exit

#include <cerrno>     // for errno
#include <cstdio>     // for fprintf, L_tmpnam
#include <cstdlib>    // for STD{ERR,OUT}_FILENO
#include <cstring>    // for strerror
#include <iostream>

// rwtest headers
#include <rw_driver.h>
#include <rw_file.h>

#ifndef _WIN32

#  include <fcntl.h>
#  include <unistd.h>
#  include <sys/types.h>
#  include <sys/wait.h>

#  define DEV_TTY   "/dev/tty"

#else   // ifdef _WIN32

#  include <fcntl.h>
#  include <io.h>

#  ifndef STDIN_FILENO
#    define STDIN_FILENO  0
#    define STDOUT_FILENO 1
#    define STDERR_FILENO 2
#  endif   // STDIN_FILENO

#  define DEV_TTY   "CON:"

#endif   // _WIN32


// use buffers larger than L_tmpnam with rw_tmpnam()
char stderr_fname [256];   // name of file to which stderr is redirected
char stdout_fname [256];   // name of file to which stderr is redirected
char stdio_fname  [256];   // same as above but for both stderr and stdout

/**************************************************************************/

template <class charT>
/*static*/ void
test_file (const char* fname, const char* expect)
{
    std::FILE* fp = std::fopen (fname, "r");

    if (!fp) {
        rw_assert (false, __FILE__, __LINE__,
                   "fopen (\"%s\", \"r\") failed: %s",
                   fname, std::strerror (errno));

        return;
    }

    enum { bufsiz = 256 };
    char buf [bufsiz] = { 0 };
    std::fgets (buf, bufsiz, fp);

    rw_assert (!std::strcmp (expect, buf), __FILE__, __LINE__,
               "expected output: \"%s\", got: \"%s\"", expect, buf);

    std::fclose (fp);
}

/**************************************************************************/

static void
do_test ()
{
    static const char stdout_expect[] = {
        "[STDOUT_FILENO]"
        "[cout,1][stdout,1][cout,2][stdout,2]"

#ifndef _RWSTD_NO_WCHAR_T
        "[wcout,1][stdout,3][wcout,2][stdout,4]"
#endif   // _RWSTD_NO_WCHAR_T
    };

    static const char stderr_expect[] = {
        "[STDERR_FILENO]"
        "[cerr,1][stderr,1][cerr,2][stderr,2]"
        "[clog,1][stderr,3][clog,2][stderr,4]"

#ifndef _RWSTD_NO_WCHAR_T
        "[wcerr,1][stderr,5][wcerr,2][stderr,6]"
        "[wclog,1][stderr,7][wclog,2][stderr,8]"
#endif   // _RWSTD_NO_WCHAR_T
    };

    static const char stdio_expect[] = {
        "[STDOUT_FILENO][STDERR_FILENO]"
        "[cout,1][stdout,1][cout,2]"   // no [stdout,2] here
        "[cerr,1][stderr,1][cerr,2][stderr,2]"
        "[clog,1][stderr,3][clog,2][stderr,4]"
        "[stdout,2]"   // but [stdout,2] flushed here

#ifndef _RWSTD_NO_WCHAR_T
        "[wcout,1][stdout,3][wcout,2]"   // no [stdout,2] here
        "[wcerr,1][stderr,5][wcerr,2][stderr,6]"
        "[wclog,1][stderr,7][wclog,2][stderr,8]"
        "[stdout,4]"   // but [stdout,2] flushed here
#endif   // _RWSTD_NO_WCHAR_T
    };

    rw_info (0, __FILE__, __LINE__,
             "ios_base::sync_with_stdio (true)");

    // call not necessary, stdio synchronized by default
    // std::ios_base::sync_with_stdio (true);

    rw_info (0, __FILE__, __LINE__,
             "interleaved std::cout and stdout output");
    test_file<char> (stdout_fname, stdout_expect);

    rw_info (0, __FILE__, __LINE__,
             "interleaved std::cerr and stderr output");
    test_file<char> (stderr_fname, stderr_expect);

    rw_info (0, __FILE__, __LINE__,
             "interleaved std::cout/cerr and stdout/stderr output");
    test_file<char> (stdio_fname,  stdio_expect);
}

/**************************************************************************/

static int
redirect_to_file (const char* fname, int fd)
{
    static int fd_tty = open (DEV_TTY, O_WRONLY);

    if (fd_tty < 0) {
        std::fprintf (stderr,
                      "open (\"" DEV_TTY "\", O_WRONLY) failed: %s\n",
                      std::strerror (errno));

        fd_tty = STDERR_FILENO;
    }

    char buf [1024];
    int n;

    // create a new file and redirect `fd' to it
    const int fd_tmp =
        open (fname, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0666);

    if (fd_tmp < 0) {
        n = std::sprintf (buf,
                          "open (\"%s\", O_WRONLY | O_CREAT | O_TRUNC, "
                          "0666) failed: %s\n",
                          fname, std::strerror (errno));
        write (fd_tty, buf, n);
        return -1;   // bail out after a critical error
    }

    const int fd2 = dup2 (fd_tmp, fd);

    if (0 > fd2) {
        n = std::sprintf (buf,
                          "dup2 (%d, %d) failed: %s\n",
                          fd2, fd, std::strerror (errno));
        write (fd_tty, buf, n);
        return -2;   // bail out after a critical error
    }

    if (fd2 != fd) {
        n = std::sprintf (buf,
                          "dup2 (%d, %d) != %d; got %d\n",
                          fd_tmp, fd, fd, fd2);
        // should never happen
        write (fd_tty, buf, n);
    }

    if (close (fd_tmp)) {
        n = std::sprintf (buf,
                          "close (%d) failed: %s\n",
                          fd_tmp, std::strerror (errno));
        write (fd_tty, buf, n);
        // proceed after a benign error
    }

    return fd2;
}


static int
exec_stdout_setup ()
{
    // create a new file and redirect stdout to it
    const int fd_stdout = redirect_to_file (stdout_fname, STDOUT_FILENO);

    if (fd_stdout != STDOUT_FILENO)
        return fd_stdout;

    // expected output (w/o newlines):
    //     "[STDOUT_FILENO]"
    //     "[cout,1][stdout,1]"
    //     "[cout,2][stdout,2]"
    //     "[wcout,1][stdout,3]"
    //     "[wcout,2][stdout,4]"

    write (STDOUT_FILENO, "[STDOUT_FILENO]", 15);
    std::cout << "[cout,1]";
    std::printf ("[stdout,1]");
    std::cout << "[cout,2]";      // flushes the printf() above
    std::printf ("[stdout,2]");

#ifndef _RWSTD_WCHAR_T

    std::wcout << "[wcout,1]";    // flushes the printf() above
    std::printf ("[stdout,3]");
    std::wcout << "[wcout,2]";    // flushes the printf() above
    std::printf ("[stdout,4]");   // flushed during termination

#endif   // _RWSTD_WCHAR_T

    return 0;
}


static int
exec_stderr_setup ()
{
    // create a new file and redirect stderr to it
    const int fd_stderr = redirect_to_file (stderr_fname, STDERR_FILENO);

    if (fd_stderr != STDERR_FILENO)
        return fd_stderr;

    // expected output (w/o newlines):
    //     "[STDERR_FILENO]"
    //     "[cerr,1][stderr,1][cerr,2][stderr,2]"
    //     "[clog,1][stderr,3][clog,2][stderr,4]"
    //     "[wcerr,1][stderr,5][wcerr,2][stderr,6]"
    //     "[wclog,1][stderr,7][wclog,2][stderr,8]"

    write (STDERR_FILENO, "[STDERR_FILENO]", 15);
    std::cerr << "[cerr,1]";
    std::fprintf (stderr, "[stderr,1]");
    std::cerr << "[cerr,2]";
    std::fprintf (stderr, "[stderr,2]");
    std::clog << "[clog,1]";
    std::fprintf (stderr, "[stderr,3]");
    std::clog << "[clog,2]";
    std::fprintf (stderr, "[stderr,4]");

#ifndef _RWSTD_WCHAR_T

    std::wcerr << "[wcerr,1]";
    std::fprintf (stderr, "[stderr,5]");
    std::wcerr << "[wcerr,2]";
    std::fprintf (stderr, "[stderr,6]");
    std::wclog << "[wclog,1]";
    std::fprintf (stderr, "[stderr,7]");
    std::wclog << "[wclog,2]";
    std::fprintf (stderr, "[stderr,8]");

#endif   // _RWSTD_WCHAR_T

    return 0;
}


static int
exec_stdio_setup ()
{
    // create a new file and redirect both stdout and stderr to it
    const int fd_stdout = redirect_to_file (stdio_fname, STDOUT_FILENO);
    const int fd_stderr = redirect_to_file (stdio_fname, STDERR_FILENO);

    if (fd_stdout != STDOUT_FILENO)
        return fd_stdout;

    if (fd_stderr != STDERR_FILENO)
        return fd_stderr;

    // expected output (w/o newlines):
    //     "[STDOUT_FILENO][STDERR_FILENO]"
    //     "[cout,1][stdout,1][cout,2]"
    //     "[cerr,1][stderr,1][cerr,2][stderr,2]"
    //     "[clog,1][stderr,3][clog,2][stderr,4]"
    //     "[stdout,2]"
    //     "[wcout,1][stdout,3][wcout,2]"
    //     "[wcerr,1][stderr,5][wcerr,2][stderr,6]"
    //     "[wclog,1][stderr,7][wclog,2][stderr,8]"
    //     "[stdout,4]"

    write (STDOUT_FILENO, "[STDOUT_FILENO]", 15);
    write (STDERR_FILENO, "[STDERR_FILENO]", 15);

    std::cout << "[cout,1]";
    std::printf ("[stdout,1]");
    std::cout << "[cout,2]";
    std::printf ("[stdout,2]");    // flushed after STDERR output below

    std::cerr << "[cerr,1]";
    std::fprintf (stderr, "[stderr,1]");
    std::cerr << "[cerr,2]";
    std::fprintf (stderr, "[stderr,2]");
    std::clog << "[clog,1]";
    std::fprintf (stderr, "[stderr,3]");
    std::clog << "[clog,2]";
    std::fprintf (stderr, "[stderr,4]");

#ifndef _RWSTD_NO_WCHAR_T

    std::wcout << "[wcout,1]";
    std::printf ("[stdout,3]");
    std::wcout << "[wcout,2]";
    std::printf ("[stdout,4]");    // flushed after STDERR output below

    std::wcerr << "[wcerr,1]";
    std::fprintf (stderr, "[stderr,5]");
    std::wcerr << "[wcerr,2]";
    std::fprintf (stderr, "[stderr,6]");
    std::wclog << "[wclog,1]";
    std::fprintf (stderr, "[stderr,7]");
    std::wclog << "[wclog,2]";
    std::fprintf (stderr, "[stderr,8]");

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}

/**************************************************************************/

struct cleanup
{
    int dummy;

    ~cleanup () {
        if (*stdout_fname)
            std::remove (stdout_fname);
        if (*stderr_fname)
            std::remove (stderr_fname);
        if (*stdio_fname)
            std::remove (stdio_fname);
    }
};


static int
run_test (int /* unused */, char* /* unused */ [])
{
    const cleanup remove_tmp_files = { 0 };

    // prevent unused warnings
    (void)&remove_tmp_files;

    if (!rw_tmpnam (stdout_fname)) {
        std::fprintf (stderr,
                      "rw_tmpnam (%p) failed: %s\n",
                      stdout_fname, std::strerror (errno));
        return 1;
    }

    if (!rw_tmpnam (stderr_fname)) {
        std::fprintf (stderr,
                      "rw_tmpnam (%p) failed: %s\n",
                      stderr_fname, std::strerror (errno));
        return 1;
    }

    if (!rw_tmpnam (stdio_fname)) {
        std::fprintf (stderr,
                      "rw_tmpnam (%p) failed: %s\n",
                      stdio_fname, std::strerror (errno));
        return 1;
    }

#ifndef _WIN32

    // create three child process and have each redirect
    // its stdout, stderr, and both, respectively, to
    // a file
    // after all the children exit, the parent will check
    // the contents of the file to determine whether all
    // streams have been properly flushed and synchronized

    for (int i = 0; i != 3; ++i) {
        const pid_t child_pid = fork ();

        if (child_pid < 0) {   // fork error

            std::fprintf (stderr, "fork() failed: %s\n",
                          std::strerror (errno));

            return 1;
        }
        else if (child_pid > 0) {   // parent
            wait (0);
        }
        else {   // child

            int ret = 1;

            switch (i) {
            case 0: ret = exec_stderr_setup (); break;
            case 1: ret = exec_stdout_setup (); break;
            case 2: ret = exec_stdio_setup (); break;
            }

            // prevent child process from cleaning up files
            *stdout_fname = *stderr_fname = *stdio_fname = '\0';
            return ret;
        }
    }

#endif   // _WIN32

    do_test ();

    return 0;
}


/* extern */ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.ios.members.static",
                    "27.4.2.4 ios_base static members",
                    run_test, "", 0);
}

