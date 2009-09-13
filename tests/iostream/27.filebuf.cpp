/***************************************************************************
 *
 * 27.filebuf.cpp - test exercising class template basic_filebuf
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
 * Copyright 1994-2008 Rogue Wave Software, Inc.
 *
 **************************************************************************/

#include <cerrno>    // for errno
#include <csignal>   // for signal()
#include <cstdio>    // for FILE, fopen(), remove()
#include <cstring>   // for memcmp()
#include <cwchar>    // for mbstate_t

#include <fstream>

#ifndef _WIN32

#  ifdef __SUNPRO_CC
     // working around a SunOS/SunPro bug (PR #26255)
#    undef _TIME_T
#  endif

#  include <cstdlib>      // for exit()
#  include <fcntl.h>      // for open(), O_XXX constants
#  include <unistd.h>     // for fork()
#  include <sys/stat.h>   // for mkfifo()
#  include <sys/types.h>  // for pid_t
#  include <sys/wait.h>   // for wait()
#else
#  include <fcntl.h>      // for O_XXX constants
#  include <io.h>         // for open()
#endif   // _WIN32

#ifndef SIGPIPE
#  define SIGPIPE   13   /* HP-UX, Linux, and SunOS value */
#endif   // SIGPIPE

#include <rw_driver.h>
#include <rw_file.h>
#include <rw_valcmp.h>


#define REMOVE_FILE(tmpfname) \
    rw_warn (0 == std::remove (tmpfname), __FILE__, __LINE__, \
             "std::remove(\"%s\") failed: %m", tmpfname)

/**************************************************************************/


template <class charT>
static void
test_ctors (const char* tname)
{
    typedef std::basic_filebuf<charT, std::char_traits<charT> > Filebuf;

    int fdcount [2];
    int next_fd [2];

    //////////////////////////////////////////////////////////////////
    rw_info (0, __FILE__, __LINE__,
             "std::basic_filebuf<%s>::basic_filebuf()", tname);

    next_fd [0] = rw_nextfd (fdcount + 0);

    {
        Filebuf fb;

        // verify the postcondition in 27.8.1.2, p2
        rw_assert (!fb.is_open (), __FILE__, __LINE__,
                   "basic_filebuf<%s>::basic_filebuf().is_open() == "
                   "false, got true", tname);

        // verify that no file descriptor has been allocated
        next_fd [1] = rw_nextfd (fdcount + 1);

        rw_assert (next_fd [0] == next_fd [1] && fdcount [0] == fdcount [1],
                   __FILE__, __LINE__,
                   "%d file descriptor leak(s) detected after construction",
                   fdcount [1] - fdcount [0]);
    }

    // verify that no file descriptor has been closed
    next_fd [1] = rw_nextfd (fdcount + 1);

    //////////////////////////////////////////////////////////////////
    rw_info (0, __FILE__, __LINE__,
             "std::basic_filebuf<%s>::~basic_filebuf()", tname);

    rw_assert (next_fd [0] == next_fd [1] && fdcount [0] == fdcount [1],
               __FILE__, __LINE__,
               "%d file descriptor leak(s) detected after destruction",
               fdcount [1] - fdcount [0]);


    const char* const tmpfname = rw_tmpnam (0);
    if (!tmpfname) {
        return;
    }

    {
        Filebuf fb;

        next_fd [0] = rw_nextfd (fdcount + 0);

        Filebuf *tmp = fb.open (tmpfname, std::ios::out);

        // verify that open() succeeds and allocates a file descriptor
        rw_assert (0 != tmp, __FILE__, __LINE__,
                   "basic_filebuf<%s>::open(\"%s\", std::ios_base::out) "
                   "failed", tname, tmpfname);

        next_fd [1] = rw_nextfd (fdcount + 1);

        rw_assert (next_fd [0] != next_fd [1]
                   && fdcount [0] + 1 == fdcount [1], __FILE__, __LINE__,
                   "%d file descriptor mismatch detected after open()",
                   fdcount [1] - fdcount [0]);
    }

    // verify that dtor closes the file descriptor
    next_fd [1] = rw_nextfd (fdcount + 1);

    rw_assert (next_fd [0] == next_fd [1] && fdcount [0] == fdcount [1],
               __FILE__, __LINE__,
               "%d file descriptor leak(s) detected after destruction",
               fdcount [1] - fdcount [0]);


#ifndef _RWSTD_NO_EXT_FILEBUF

    //////////////////////////////////////////////////////////////////
    rw_info (0, __FILE__, __LINE__,
             "std::basic_filebuf<%s>::basic_filebuf(FILE*) "
             "[extension]", tname);

#  ifdef stdin

    {
        std::FILE* const fp = std::fopen (tmpfname, "w");

        next_fd [0] = rw_nextfd (fdcount + 0);

        // object takes over the ownership of the file pointer
        Filebuf fb (fp);

        rw_assert (fb.is_open (), __FILE__, __LINE__,
                   "basic_filebuf<%s>::basic_filebuf (FILE*).is_open() "
                   "== true, got false", tname);

        next_fd [1] = rw_nextfd (fdcount + 1);

        rw_assert (next_fd [0] == next_fd [1] && fdcount [0] == fdcount [1],
                   __FILE__, __LINE__,
                   "%d file descriptor mismatch detected after open()",
                   fdcount [1] - fdcount [0]);
    }

    // verify that dtor closes the file descriptor
    next_fd [1] = rw_nextfd (fdcount + 1);

    rw_assert (fdcount [0] == fdcount [1] + 1, __FILE__, __LINE__,
               "%d file descriptor leak(s) detected after destruction",
               fdcount [1] - fdcount [0]);

#  else   // if !defined (stdin)

    rw_assert (false, __FILE__, __LINE__,
               "macro stdin unexpectedly not #defined, "
               "basic_filebuf<%s>::basic_filebuf (FILE*) "
               "extension not tested", tname);

#  endif   // stdin

    //////////////////////////////////////////////////////////////////
    rw_info (0, __FILE__, __LINE__,
             "std::basic_filebuf<%s>::basic_filebuf(int) "
             "[extension]", tname);

    Filebuf *pfb = 0;

    {
        std::FILE* const fp = std::fopen (tmpfname, "w");

        // object takes over the ownership of the file pointer
        pfb = new Filebuf (fp);

        next_fd [0] = rw_nextfd (fdcount + 0);

        // object takes over the ownership of the file descriptor
        Filebuf fb (pfb->fd ());

        next_fd [1] = rw_nextfd (fdcount + 1);

        rw_assert (fb.is_open (), __FILE__, __LINE__,
                   "basic_filebuf<%s>::basic_filebuf (int).is_open() "
                   "== true, got false", tname);

        rw_assert (next_fd [0] == next_fd [1] && fdcount [0] == fdcount [1],
                   __FILE__, __LINE__,
                   "%d file descriptor mismatch detected after open()",
                   fdcount [1] - fdcount [0]);
    }

    // verify that dtor closes the file descriptor
    next_fd [1] = rw_nextfd (fdcount + 1);

    rw_assert (fdcount [0] == fdcount [1] + 1, __FILE__, __LINE__,
               "%d file descriptor leak(s) detected after destruction",
               fdcount [1] - fdcount [0]);

    delete pfb;

#endif   // _RWSTD_NO_EXT_FILEBUF

    REMOVE_FILE (tmpfname);
}

/***************************************************************************/

extern "C" {

// instead of calling signal(SIGPIPE, SIG_IGN)
void ignore_signal (int)
{
    std::signal (SIGPIPE, ignore_signal);
}

}   // extern "C"


template <class charT>
static void
test_open (const char* tname)
{
    typedef std::basic_filebuf<charT, std::char_traits<charT> > Filebuf;

    const char* const tmpfname = rw_tmpnam (0);

    if (!tmpfname) {
        return;
    }

    rw_info (0, __FILE__, __LINE__,
             "std::basic_filebuf<%s>::is_open ()", tname);

    rw_info (0, __FILE__, __LINE__,
             "std::basic_filebuf<%s>::open (const char*, "
             "ios_base::openmode)", tname);

    /*****************************************************************

    Table 92

    +-----+-----+-----+-----+-----+-----+
    | bin | in  | out |trunc| app |stdio|
    +=====+=====+=====+=====+=====+=====+
    |     |     |  +  |     |     | "w" |
    +-----+-----+-----+-----+-----+-----+
    |     |     |  +  |     |  +  | "a" |
    +-----+-----+-----+-----+-----+-----+
    |     |     |  +  |  +  |     | "w" |
    +-----+-----+-----+-----+-----+-----+
    |     |  +  |     |     |     | "r" |
    +-----+-----+-----+-----+-----+-----+
    |     |  +  |  +  |     |     |"r+" |
    +-----+-----+-----+-----+-----+-----+
    |     |  +  |  +  |  +  |     |"w+" |
    +=====+=====+=====+=====+=====+=====+
    |  +  |     |  +  |     |     |"wb" |
    +-----+-----+-----+-----+-----+-----+
    |  +  |     |  +  |     |  +  |"ab" |
    +-----+-----+-----+-----+-----+-----+
    |  +  |     |  +  |  +  |     |"wb" |
    +-----+-----+-----+-----+-----+-----+
    |  +  |  +  |     |     |     |"rb" |
    +-----+-----+-----+-----+-----+-----+
    |  +  |  +  |  +  |     |     |"r+b"|
    +-----+-----+-----+-----+-----+-----+
    |  +  |  +  |  +  |  +  |     |"w+b"|
    +-----+-----+-----+-----+-----+-----+

    r          open text file for reading
    w          truncate to zero length or create text file for writing
    a          append; open or create text file for writing at EOF
    rb         open binary file for reading
    wb         truncate to zero length or create binary file for writing
    ab         append; open or create binary file for writing at EOF
    r+         open text file for update (reading and writing)
    w+         truncate to zero length or create text file for update
    a+         append; open or create text file for update at EOF
    r+b or rb+ open binary file for update (reading and writing)
    w+b or wb+ truncate to zero length or create binary file for update
    a+b or ab+ append; open or create binary file for update at EOF

    ******************************************************************/

#define BEGIN_MODE(openmode, ext)                                \
    mode = (openmode),                                           \
    rw_info (0, __FILE__, __LINE__, "%{Io} %s", mode, ext)

#define ASSERT_OPEN(expr, mode, txt)                             \
    rw_assert (expr, __FILE__, __LINE__,                         \
               "basic_filebuf<%s>::open (\"%s\", %{Io}) %s ",    \
               tname, tmpfname, mode, txt)                       \

    std::ios_base::openmode mode;

    const std::ios::openmode iomodes[] = {
        std::ios::openmode ()

#ifndef _RWSTD_NO_EXT_STDIO

        , std::ios::stdio

#endif   // _RWSTD_NO_EXT_STDIO

    };

    const std::size_t niomodes = sizeof iomodes / sizeof *iomodes;

    const char* buf  = 0;
    std::size_t size = 0;

    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::out)

    for (std::size_t minx = 0; minx != niomodes; ++minx) {

        BEGIN_MODE (std::ios::out | iomodes [minx], "");

        {
            Filebuf ().open (tmpfname, mode);
        }

        buf = _RWSTD_STATIC_CAST (char*, rw_fread (tmpfname, &size));

        ASSERT_OPEN (0 != buf, mode, "failed to create a new file");

        if (buf)
            ASSERT_OPEN (!size, mode,
                         "unexpectedly created a non-empty file");

        rw_fwrite (tmpfname, "foobar");

        {
            Filebuf ().open (tmpfname, mode);
        }

        buf = _RWSTD_STATIC_CAST (char*, rw_fread (tmpfname, &size));

        ASSERT_OPEN (0 != buf, mode, "failed to open an existing file");

        if (buf)
            ASSERT_OPEN (!size, mode,
                         "unexpectedly created a non-empty file");
    }

    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::out | ios::app)

    for (std::size_t minx = 0; minx != niomodes; ++minx) {

        BEGIN_MODE (std::ios::out | std::ios::app | iomodes [minx], "");

        REMOVE_FILE (tmpfname);

        {
            Filebuf ().open (tmpfname, mode);
        }

        buf = _RWSTD_STATIC_CAST (char*, rw_fread (tmpfname, &size));

        ASSERT_OPEN (0 != buf, mode, "failed to create a new file");

        if (buf)
            ASSERT_OPEN (!size, mode, "unexpectedly created a non-empty file");

        rw_fwrite (tmpfname, "foobar");

        {
            Filebuf ().open (tmpfname, mode);
        }

        buf = _RWSTD_STATIC_CAST (char*, rw_fread (tmpfname, &size));

        ASSERT_OPEN (0 != buf, mode, "failed to open an existing file");

        if (buf)
            ASSERT_OPEN (6 == size, mode,
                         "unexpectedly truncated a non-empty file");
    }

    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::out | ios::trunc)

    for (std::size_t minx = 0; minx != niomodes; ++minx) {

        BEGIN_MODE (std::ios::out | std::ios::trunc | iomodes [minx], "");

        rw_fwrite (tmpfname, "foobar");

        {
            // open a file for writing and truncate it to 0 size
            Filebuf ().open (tmpfname, mode);
        }

        // read the contents of the file and verify that they are empty
        buf = _RWSTD_STATIC_CAST (char*, rw_fread (tmpfname, &size));

        // file must exist
        ASSERT_OPEN (0 != buf, mode, "failed to open an existing file");

        // the size of the file must be 0
        if (buf)
            ASSERT_OPEN (!size, mode, "failed to truncate a non-empty file");
    }


    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::nocreate)

    for (std::size_t minx = 0; minx != niomodes; ++minx) {

        // ios::nocreate is meaningless by itself
        BEGIN_MODE (std::ios::nocreate | iomodes [minx], "[extension]");

        Filebuf fb;

        fb.open (tmpfname, mode);

        // verify that the call failed
        ASSERT_OPEN (!fb.is_open (), mode,
                     "unexpectedly succeeded to open a non-existent file");
    }

    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::out | ios::nocreate)

    BEGIN_MODE (std::ios::out | std::ios::nocreate, "[extension]");

    // ios::nocreate will prevent the call to open from creating
    // the file if it doesn't exist

    REMOVE_FILE (tmpfname);

    {
        Filebuf fb;

        // try to open a file that doesn't exist for output but prevent
        // the call from creating it by setting the nocreate bit
        fb.open (tmpfname, mode);

        // verify that the call failed
        ASSERT_OPEN (!fb.is_open (), mode,
                     "unexpectedly succeeded to open a non-existent file");
    }

    buf = _RWSTD_STATIC_CAST (char*, rw_fread (tmpfname, &size));

    ASSERT_OPEN (0 == buf, mode, "unexpectedly created a file");

    // the same as above but with an existing file
    rw_fwrite (tmpfname, "foobar");

    {
        Filebuf fb;

        // try to open an existing file
        fb.open (tmpfname, mode);

        // verify that the call succeeded
        ASSERT_OPEN (fb.is_open (), mode,
                     "unexpectedly succeeded to open an existing file");
    }

    buf = _RWSTD_STATIC_CAST (char*, rw_fread (tmpfname, &size));

    ASSERT_OPEN (0 != buf, mode, "failed to open an existing file");

#ifndef _RWSTD_NO_EXT_STDIO

    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::stdio| ios::out | ios::nocreate)

    BEGIN_MODE (std::ios::stdio | std::ios::out | std::ios::nocreate,
                "[extension]");

    REMOVE_FILE (tmpfname);

    // ios::nocreate can't be implemented with ios::stdio
    // and the call must fail
    {
        Filebuf fb;

        fb.open (tmpfname, mode);

        // verify that the call failed
        ASSERT_OPEN (!fb.is_open (), mode,
                     "unexpectedly succeeded to open a non-existent file");
    }

    // verify that the file wasn't created
    buf = _RWSTD_STATIC_CAST (char*, rw_fread (tmpfname, &size));

    ASSERT_OPEN (0 == buf, mode, "unexpectedly created a file");

#endif   // _RWSTD_NO_EXT_STDIO

    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::out | ios::nocreate | ios::trunc)

    // ios::trunc has no effect on ios::nocreate
    BEGIN_MODE (std::ios::out | std::ios::nocreate | std::ios::trunc,
                "[extension]");

    // remove file in case it was unexpectedly created above
    std::remove (tmpfname);

    {
        Filebuf fb;

        // try to open a file that doesn't exist for output but prevent
        // the call from creating it by setting the nocreate bit
        fb.open (tmpfname, mode);

        // verify that the call failed
        ASSERT_OPEN (!fb.is_open (), mode,
                     "unexpectedly succeeded to open a non-existent file");
    }

    buf = _RWSTD_STATIC_CAST (char*, rw_fread (tmpfname, &size));

    ASSERT_OPEN (0 == buf, mode, "unexpectedly created a file");

    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::noreplace)

    // remove file in case it was unexpectedly created above
    std::remove (tmpfname);

    for (std::size_t minx = 0; minx != niomodes; ++minx) {

        // ios::noreplace is meaningless by itself
        BEGIN_MODE (std::ios::noreplace | iomodes [minx], "[extension]");

        Filebuf fb;

        fb.open (tmpfname, mode);

        // verify that the call failed
        ASSERT_OPEN (!fb.is_open (), mode,
                     "unexpectedly succeeded to open a non-existent file");
    }

    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::out | ios::noreplace)

    // ios::noreplace will cause the call to open to succeed only
    // if the file doesn't exist, otherwise the call will fail
    BEGIN_MODE (std::ios::out | std::ios::noreplace, "[extension]");

    rw_fwrite (tmpfname, "foobar");

    {
        Filebuf fb;

        // try to open an existing file for output but prevent
        // the call from replacing it by setting the noreplace bit
        fb.open (tmpfname, mode);

        // verify that the call failed
        ASSERT_OPEN (!fb.is_open (), mode,
                     "unexpectedly succeeded to open an existing file");
    }

    buf = _RWSTD_STATIC_CAST (char*, rw_fread (tmpfname, &size));

    ASSERT_OPEN (0 != buf, mode, "unexpectedly removed an existing file");

    if (buf)
        ASSERT_OPEN (0 != size, mode, "unexpectdly truncated a non-empty file");

    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::out | ios::trunc | ios::noreplace)

    BEGIN_MODE (std::ios::out | std::ios::trunc | std::ios::noreplace,
                "[extension]");

    rw_fwrite (tmpfname, "foobar");

    {
        Filebuf fb;

        // try to open an existing file for output
        fb.open (tmpfname, mode);

        // verify that the call failed
        ASSERT_OPEN (!fb.is_open (), mode,
                     "failed to open an existing file");
    }

    buf = _RWSTD_STATIC_CAST (char*, rw_fread (tmpfname, &size));

    ASSERT_OPEN (0 != buf, mode, "unexpectedly removed an existing file");

    if (buf)
        ASSERT_OPEN (0 != size, mode, "failed to truncate a non-empty file");


    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::in)

    charT readbuf [256];
    const std::size_t readbuf_size = sizeof readbuf / sizeof *readbuf;

    for (std::size_t minx = 0; minx != niomodes; ++minx) {

        BEGIN_MODE (std::ios::in | iomodes [minx], "");

        rw_fwrite (tmpfname, "foobar");

        Filebuf fb;

        const charT foobar[] = { 'f', 'o', 'o', 'b', 'a', 'r' };

        if (fb.open (tmpfname, mode)) {

            rw_assert (fb.is_open (), __FILE__, __LINE__,
                       "basic_filebuf<%s>::is_open()", tname);

            std::memset (readbuf, 0, sizeof readbuf);
            fb.sgetn (readbuf, readbuf_size);

            ASSERT_OPEN (!std::memcmp (readbuf, foobar, sizeof foobar),
                         mode, "failed to open an existing file for reading");
        }
        else {
            ASSERT_OPEN (0, mode, "failed to open an existing file");
        }
    }

    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::in | ios::out)

    for (std::size_t minx = 0; minx != niomodes; ++minx) {

        BEGIN_MODE (std::ios::in | std::ios::out | iomodes [minx], "");

        rw_fwrite (tmpfname, "foobar");

        Filebuf fb;

        if (fb.open (tmpfname, mode)) {

            rw_assert (fb.is_open (), __FILE__, __LINE__,
                       "basic_filebuf<%s>::is_open()", tname);

            const charT FOO[] = { 'F', 'O', 'O' };
            const charT bar[] = { 'b', 'a', 'r' };

            if (3 != fb.sputn (FOO, sizeof FOO / sizeof *FOO))
                ASSERT_OPEN (false, mode,
                             "failed to open an existing file for writing "
                             "(sputn() failed)");

            // an input operation must be separated from a
            // preceding output operation by a seek() or flush()
            fb.pubsync ();

            std::memset (readbuf, 0, sizeof readbuf);
            const std::streamsize got = fb.sgetn (readbuf, readbuf_size);

            if (3 == got) {
                rw_assert (!std::memcmp (readbuf, bar, sizeof bar),
                           __FILE__, __LINE__,
                           "sgetn() retrieved the wrong data)");
            }
            else
                rw_assert (false, __FILE__, __LINE__,
                           "sgetn() == 3, got %td: %{*.*Ac}, "
                           "expected %{*.*Ac}",
                           got, int (sizeof (charT)), got, readbuf,
                           int (sizeof (charT)),
                           int (sizeof bar / sizeof *bar), bar);
        }
        else {
            ASSERT_OPEN (0, mode, "failed to open an existing file");
        }
    }


    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::in | ios::out | ios::trunc)

    for (std::size_t minx = 0; minx != niomodes; ++minx) {

        BEGIN_MODE (  std::ios::in | std::ios::out | std::ios::trunc
                    | iomodes [minx], "");

        rw_fwrite (tmpfname, "foobar");

        Filebuf fb;

        if (fb.open (tmpfname, mode)) {

            rw_assert (fb.is_open (), __FILE__, __LINE__,
                       "basic_filebuf<%s>::is_open()", tname);

            const charT FOO[] = { 'F', 'O', 'O' };

            std::memset (readbuf, 0, sizeof readbuf);
            if (0 != fb.sgetn (readbuf, readbuf_size))
                ASSERT_OPEN (false, mode,
                             "failed to truncate a non-empty file");

            if (3 != fb.sputn (FOO, sizeof FOO / sizeof *FOO))
                ASSERT_OPEN (false, mode,
                             "failed to open an existing file for writing "
                             "(sputn() failed)");

            // an input operation must be separated from a
            // preceding output operation by a seek() or flush()
            fb.pubseekoff (0, std::ios::beg);

            std::memset (readbuf, 0, sizeof readbuf);
            if (3 != fb.sgetn (readbuf, readbuf_size))
                ASSERT_OPEN (false, mode,
                             "failed to open an existing file for reading "
                             "(sgetn() failed after a sputn())");

            ASSERT_OPEN (!std::memcmp (readbuf, FOO, sizeof FOO), mode,
                         "failed to open an existing file for reading "
                         "and writing (sgetn() retrieved the wrong data)");
        }
        else {
            ASSERT_OPEN (0, mode, "failed to open an existing file");
        }
    }


    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::binary | ios::in)

    for (std::size_t minx = 0; minx != niomodes; ++minx) {

        BEGIN_MODE (std::ios::binary | std::ios::in | iomodes [minx], "");

        // create a file containing various combinations of new-line
        // and line-feed and read the file in using the ios::binary
        // flag, expecting the original contents of the file on
        // output (i.e., no CR/LF conversion on Win32)
        const char bindata[] = "\nf\ro\n\ro\r\n\b\n\ra\r\rr";

        // rw_fwrite() returns the number of bytes successfully written
        size = rw_fwrite (tmpfname, bindata, sizeof bindata - 1, "wb");

        Filebuf fb;

        if (fb.open (tmpfname, mode)) {

            rw_assert (fb.is_open (), __FILE__, __LINE__,
                       "basic_filebuf<%s>::is_open()", tname);

            std::memset (readbuf, 0, sizeof readbuf);
            const std::streamsize N = fb.sgetn (readbuf, readbuf_size);

            if (N != std::streamsize (size))
                rw_assert (false, __FILE__, __LINE__,
                           "basic_filebuf<%s>::sgetn (%p, %d) == %d, "
                           "got %d", tname, readbuf,
                           readbuf_size, size, N);

            for (std::streamsize i = 0; i != N; ++i) {

                if (char (readbuf [i]) != bindata [i]) {

                    rw_assert (false, __FILE__, __LINE__,
                               "basic_filebuf<%s>::sgetn() data "
                               "mismatch: got \"%s\", expected \"%s\"",
                               tname, readbuf, bindata);
                    break;
                }
            }
        }
        else {
            ASSERT_OPEN (0, mode, "failed to open an existing file");
        }
    }

    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::binary | ios::out)

    for (std::size_t minx = 0; minx != niomodes; ++minx) {

        BEGIN_MODE (std::ios::binary | std::ios::out | iomodes [minx], "");

        Filebuf fb;

        if (fb.open (tmpfname, mode)) {

            rw_assert (fb.is_open (), __FILE__, __LINE__,
                       "basic_filebuf<%s>::is_open()", tname);

            const charT XYZ[] = {
                '\n', 'X', '\r', 'Y', '\n', '\r', 'Z', '\r', '\n', '\0'
            };

            const std::streamsize N = sizeof XYZ / sizeof *XYZ - 1;

            if (N != fb.sputn (XYZ, N))
                ASSERT_OPEN (false, mode,
                             "failed to open an existing file for writing "
                             "(sputn() failed)");

            fb.close ();

            size = 0;
            buf  = _RWSTD_STATIC_CAST (char*, rw_fread (tmpfname, &size, "rb"));

            rw_assert (std::size_t (N) == size, __FILE__, __LINE__,
                       "basic_filebuf<%s>::sputn() wrote %d bytes, "
                       "%d expected",
                       tname, size, N * sizeof (charT));

            for (std::streamsize i = 0; i != N; ++i) {

                if (buf [i] != char (XYZ [i])) {

                    rw_assert (false, __FILE__, __LINE__,
                               "basic_filebuf<%s>::sputn() data "
                               "mismatch: wrote \"%s\", expected \"%s\"",
                               tname, buf, XYZ);
                    break;
                }
            }
        }
        else {
            ASSERT_OPEN (0, mode, "failed to open an existing file");
        }
    }

    REMOVE_FILE (tmpfname);

    //////////////////////////////////////////////////////////////////
    // exercise open ((const char*)0, ios::in)

    for (std::size_t minx = 0; minx != niomodes; ++minx) {

        BEGIN_MODE (std::ios::in | iomodes [minx],
                    ", file name = 0 [extension]");

        // verify that open() succeeds when the first argument
        // is the null pointer (the call creates a temporary
        // file and opens it for reading -- such a file may not
        // be very useful but since it's harmless there's no
        // reason it shouldn't be possible)

        Filebuf fb;
        fb.open ((const char*)0, mode);

        rw_assert (fb.is_open (), __FILE__, __LINE__,
                   "basic_filebuf<%s>::is_open()", tname);

        // FIXME: verify that the call to close removes the file
        fb.close ();
    }

    //////////////////////////////////////////////////////////////////
    // exercise open ((const char*)0, ios::out)

    for (std::size_t minx = 0; minx != niomodes; ++minx) {

        BEGIN_MODE (std::ios::out | iomodes [minx],
                    ", file name = 0 [extension]");

        // verify that open() succeeds when the first argument
        // is the null pointer (the call creates a temporary
        // file and opens it for writing)

        Filebuf fb;
        fb.open ((const char*)0, mode);

        rw_assert (fb.is_open (), __FILE__, __LINE__,
                   "basic_filebuf<%s>::is_open()", tname);

        // FIXME: verify that the call to close removes the file
        fb.close ();
    }

    //////////////////////////////////////////////////////////////////
    // exercise open ((const char*)0, ios::in | ios::out)

    for (std::size_t minx = 0; minx != niomodes; ++minx) {

        BEGIN_MODE (std::ios::in | std::ios::out | iomodes [minx],
                    ", file name = 0 [extension]");

        Filebuf fb;
        fb.open ((const char*)0, mode);

        rw_assert (fb.is_open (), __FILE__, __LINE__,
                   "basic_filebuf<%s>::is_open()", tname);

        // FIXME: verify that the call to close removes the file
        fb.close ();
    }


#ifndef _WIN32

    //////////////////////////////////////////////////////////////////
    // exercise open (..., ios::ate)

    BEGIN_MODE (std::ios::ate | std::ios::out, "");

    std::signal (SIGPIPE, ignore_signal);

    if (mkfifo (tmpfname, S_IRWXU))
        rw_assert (false, __FILE__, __LINE__,
                   "mkfifo (\"%s\", S_IRWXU) failed", tmpfname);
    else {

        const pid_t childpid = fork ();

        if (childpid > 0) {   // parent process

            Filebuf fb;

            const Filebuf* const fbp = fb.open (tmpfname, mode);

            // verify that open(ate) fails for an existing but unseekable file
            if (fbp) {
                rw_assert (false, __FILE__, __LINE__,
                           "basic_filebuf<%s>::open(\"%s\", ios::ate | "
                           "ios::out) == 0 after a failed seek to end",
                           tname, tmpfname);
            }

            // verify that is_open() returns false after a failed open
            if (fb.is_open ()) {
                rw_assert (false, __FILE__, __LINE__,
                           "basic_filebuf<%s>::is_open() == false"
                           " after a failed call to open", tname);
            }

            // reap our child's exit status
            wait (0);
        }
        else if (0 == childpid) {   // child process

            Filebuf fb;

            if (fb.open (tmpfname, std::ios::in))
                fb.sgetc ();
            else
                rw_assert (false, __FILE__, __LINE__,
                           "basic_filebuf<%s>::open(\"%s\", "
                           "ios::in) failed in child process",
                           tname, tmpfname);

            std::exit (0);
        }
        else
            rw_assert (false, __FILE__, __LINE__, "fork() failed");

        REMOVE_FILE (tmpfname);
    }

    //////////////////////////////////////////////////////////////////
    // exercise close ()

    rw_info (0, __FILE__, __LINE__,
             "std::basic_filebuf<%s>::close ()", tname);

    std::signal (SIGPIPE, ignore_signal);

    if (mkfifo (tmpfname, S_IRWXU))
        rw_assert (false, __FILE__, __LINE__,
                   "mkfifo (\"%s\", S_IRWXU) failed", tmpfname);
    else {

        // verify that close() returns 0 on failure by opening and
        // writing into a pipe in the parent process that the child
        // prematurely closes

        const pid_t childpid = fork ();

        if (childpid > 0) {   // parent process

            Filebuf fb;

            if (fb.open (tmpfname, std::ios::out)) {

                // write into the file buffer
                fb.sputc (charT ());

                // wait for child to open and close the pipe
                wait (0);

                // close() will try to flush the contents of the buffer
                // into the pipe which should fail, since the other end
                // of the pipe has already been closed
                if (fb.close ())
                    rw_assert (false, __FILE__, __LINE__,
                               "basic_filebuf<%s>::close() unexpectedly "
                               "suceeded", tname);
            }
            else
                rw_assert (false, __FILE__, __LINE__,
                           "basic_filebuf<%s>::open (\"%s\", ios::out) "
                           " != 0; got 0", tname, tmpfname);
        }
        else if (0 == childpid) {   // child process

            Filebuf fb;

            if (!fb.open (tmpfname, std::ios::in))
                rw_assert (false, __FILE__, __LINE__,
                           "basic_filebuf<%s>::open(\"%s\", "
                           "ios::in) failed in child process",
                           tname, tmpfname);

            std::exit (0);
        }
        else
            rw_assert (false, __FILE__, __LINE__, "fork() failed");

        REMOVE_FILE (tmpfname);
    }

#endif   // _WIN32

}

/***************************************************************************/

template <class charT>
static void
test_sync (const char* tname)
{
    typedef std::basic_filebuf<charT, std::char_traits<charT> > Filebuf;

    rw_info (0, __FILE__, __LINE__,
             "std::basic_filebuf<%s>::sync ()", tname);

    const char* const tmpfname = rw_tmpnam (0);
    if (!tmpfname) {
        return;
    }

    {
        Filebuf fb;

        static const charT foo[] = { 'f', 'o', 'o' };
        static const charT BAR[] = { 'B', 'A', 'R' };

        // create a non-empty file
        rw_fwrite (tmpfname, "foobar");

        if (fb.open (tmpfname, std::ios::in)) {

            rw_assert (fb.is_open (), __FILE__, __LINE__,
                       "basic_filebuf<%s>::is_open()", tname);

            charT readbuf [256] = { 0 };

            // read the first part of the file; filebuf will actually
            // end up reading and caching the entire contents of the
            // file
            std::streamsize got = fb.sgetn (readbuf, 3);

            // verify the contents of buffer match that of the file
            if (3 != got)
                rw_assert (false, __FILE__, __LINE__,
                           "sgetn() == 3, got %d", got);

            rw_assert (!std::memcmp (readbuf, foo, sizeof foo),
                       __FILE__, __LINE__,
                       "sgetn() retrieved \"%s\", expected \"%s\")",
                       readbuf, foo);

            // overwrite the contents of the file
            rw_fwrite (tmpfname, "FOOBAR");

            // call sync, excpecting filebuf to synchromnize with
            // the new contents of the file while maintaining
            // the correct position
            const int res = fb.pubsync ();

            rw_assert (0 == res, __FILE__, __LINE__,
                       "pubsync() == 0, got %d", res);

            std::memset (readbuf, 0, sizeof readbuf);

            // continue to read the file where the first sgetn()
            // call left off
            got = fb.sgetn (readbuf, 3);

            // verify that the new contents of the file have been read
            if (3 != got)
                rw_assert (false, __FILE__, __LINE__,
                           "sgetn() == 3, got %d", got);

            rw_assert (!std::memcmp (readbuf, BAR, sizeof BAR),
                       __FILE__, __LINE__,
                       "sgetn() retrieved \"%s\", expected \"%s\")",
                       readbuf, BAR);
        }
        else {
            rw_assert (false, __FILE__, __LINE__,
                       "failed to open an existing file");
        }
    }

    {
        Filebuf fb;

        static const charT foo[] = { 'f', 'o', 'o' };

        // create a non-empty file
        rw_fwrite (tmpfname, "foobar");

        if (fb.open (tmpfname, std::ios::in)) {

            rw_assert (fb.is_open (), __FILE__, __LINE__,
                       "basic_filebuf<%s>::is_open()", tname);

            charT readbuf [256] = { 0 };

            // read the first part of the file; filebuf will actually
            // end up reading and caching the entire contents of the
            // file
            std::streamsize got = fb.sgetn (readbuf, 3);

            // verify the contents of buffer match that of the file
            if (3 != got)
                rw_assert (false, __FILE__, __LINE__,
                           "sgetn() == 3, got %d", got);

            rw_assert (!std::memcmp (readbuf, foo, sizeof foo),
                       __FILE__, __LINE__,
                       "sgetn() retrieved \"%s\", expected \"%s\")",
                       readbuf, foo);

            // overwrite the contents of the file with less data
            rw_fwrite (tmpfname, "BA");

            // call sync, excpecting filebuf to synchromnize with
            // the new contents of the file
            // since there is less data in the file than the current
            // offset, verify that the offset is adjusted to be the
            // same as the end of the actual file
            const int res = fb.pubsync ();

            rw_assert (0 == res, __FILE__, __LINE__,
                       "pubsync() == 0, got %d", res);

            std::memset (readbuf, 0, sizeof readbuf);

            // try to continue to read the file where the first sgetn()
            // call left off, expecting a failure
            got = fb.sgetn (readbuf, 3);

            // verify that nothing has been read
            if (0 != got)
                rw_assert (false, __FILE__, __LINE__,
                           "sgetn() == 0, got %d", got);

            // verify that the end of the file is correctly reported
            const typename Filebuf::off_type off =
                fb.pubseekoff (0, std::ios::end);

            rw_assert (off == 2, __FILE__, __LINE__,
                       "pubseekoff(0, ios::end) == 2, got %d", off);
        }
        else {
            rw_assert (false, __FILE__, __LINE__,
                       "failed to open an existing file");
        }
    }
}

/***************************************************************************/


template <class charT>
static void
test_attach (const char* tname)
{
#ifndef _RWSTD_NO_EXT_FILEBUF

    //////////////////////////////////////////////////////////////////
    // exercise attach(int) and fd()

    rw_info (0, __FILE__, __LINE__,
             "std::basic_filebuf<%s>::attach (int) [extension]", tname);

    rw_info (0, __FILE__, __LINE__,
             "std::basic_filebuf<%s>::fd () [extension]", tname);

    typedef std::basic_filebuf<charT, std::char_traits<charT> > Filebuf;

    // Extension:
    // basic_filebuf* attach(int fd)
    //   Connects *this to an open file descriptor, fd. Unless detach() is
    //   called, the file descriptor will be closed during the first call
    //   to close() or when the object is destroyed. Returns this on success,
    //   0 on failure (e.g., when this->is_open() evaluates to true).

    int lastfd = -1;

    {
        Filebuf fb;

        // get a new valid file descriptor
        const int fd = open (DEV_NULL, O_RDWR);

        if (fd < 0) {
            rw_assert (false, __FILE__, __LINE__,
                       "open (\"%s\", O_RDWR) < 0; "
                       "aborting test", DEV_NULL);
            return;
        }

        lastfd = fd;

        // attach the filebuf object to the file descriptor
        Filebuf *pfb = fb.attach (fd);

        // verify that attach() succeeded
        if (&fb != pfb)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::attach(int = %d) == %p, got 0",
                       tname, fd, &fb);

        // verify that fd() returns the attached file descriptor
        if (fd != fb.fd ())
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::fd() == %d, got %d",
                       tname, fd, fb.fd ());

        // verify that the filebuf object is open
        if (!fb.is_open ())
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::is_open() == true, "
                       "got false after a successful call to attach(%d)",
                       tname, fd);

        // Filebuf dtor should close the file descriptor (checked below)
    }

    {
        Filebuf fb;

        // create a new file descriptor
        const int fd = open (DEV_NULL, O_RDWR);

        // verify that the dtor in the block above closed the filebuf's fd
        if (fd != lastfd)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::attach(int = %d) caused "
                       "a file descriptor leak after object destruction",
                       tname, lastfd);

        lastfd = fd;

        // attach this object to the new file descriptor
        Filebuf *pfb = fb.attach (fd);

        // verify that attach() succeeded
        if (&fb != pfb)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::attach(int = %d) == %p, got 0",
                       tname, fd, &fb);

        // verify that fd() returns the attached file descriptor
        if (fd != fb.fd ())
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::fd() == %d, got %d",
                       tname, fd, fb.fd ());

        // verify that the filebuf object is open
        if (!fb.is_open ())
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::is_open() == true, "
                       "got false after a successful call to attach(%d)",
                       tname, fd);

        // call close
        pfb = fb.close ();

        // verify that the call to close() succeeded
        if (&fb != pfb)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::close() == %p, got 0",
                       tname, &fb);

        // verify that fd() returns an invalid file descriptor after
        // a successful call to close()
        if (0 <= fb.fd ())
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::fd() < 0, got %d after "
                       "a call to close()",
                       tname, fb.fd ());

        // verify that is_open() returns false after the call to close()
        if (fb.is_open ())
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::is_open() == false, "
                       "got true after a successful call to close()",
                       tname);
    }

    {
        Filebuf fb;

        // get a new valid file descriptor
        const int fd = open (DEV_NULL, O_RDWR);

        // verify that the dtor in the block above closed the filebuf's fd
        if (fd != lastfd)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::attach(int = %d) caused "
                       "a file descriptor leak after object destruction",
                       tname, lastfd);

        lastfd = fd;

        // attach the filebuf object to the file descriptor
        Filebuf *pfb = fb.attach (fd);

        // verify that attach() succeeded
        if (&fb != pfb)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::attach(int = %d) == %p, got 0",
                       tname, fd, &fb);

        pfb = fb.attach (fd);

        // verify that a subsequent call to attach() (with the same
        // or different file descriptor) failed
        if (&fb == pfb)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::attach(int = %d) == 0, got %p",
                       tname, fd, &fb);

        const int new_fd = open (DEV_NULL, O_RDWR);

        pfb = fb.attach (fd);

        if (&fb == pfb)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::attach(int = %d) == 0, got %p",
                       tname, fd, &fb);

        close (new_fd);

        // Filebuf dtor should close the file descriptor (checked below)
    }

    //////////////////////////////////////////////////////////////////
    // exercise detach()

    rw_info (0, __FILE__, __LINE__,
             "std::basic_filebuf<%s>::detach () [extension]", tname);

    // Extension:
    // int detach()
    //   Flushes any waiting output to the file associated with the file
    //   descriptor, and disconnects the file descriptor from *this so
    //   that subsequent calls to close() will not close the file
    //   descriptor. Returns the detached file descriptor on success,
    //   -1 on failure.

    {
        Filebuf fb;

        // get a new valid file descriptor
        const int fd = open (DEV_NULL, O_RDWR);

        // verify that the dtor in the block above closed the filebuf's fd
        if (fd != lastfd)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::attach(int = %d) caused "
                       "a file descriptor leak after object destruction",
                       tname, lastfd);

        lastfd = fd;

        // attach the filebuf object to the file descriptor
        fb.attach (fd);

        const int old_fd = fb.detach ();

        // verify that detach() returned the original file descriptor
        if (fd != old_fd)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::detach() == %d, got %d",
                       tname, fd, old_fd);

        // verify that is_open() returns false after the call to detach()
        if (fb.is_open ())
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::is_open() == false, "
                       "got true after a call to detach()", tname);

        // verify that fd() returns an invalid file descriptor after
        // a successful call to detach()
        if (0 <= fb.fd ())
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::fd() < 0, got %d after "
                       "a call to detach()()", tname, fb.fd ());

        close (old_fd);

        errno = 0;
    }

    // verify that the dtor in the block above didn't try to close
    // the already explicitly closed file descriptor
    if (errno)
        rw_assert (false, __FILE__, __LINE__,
                   "basic_filebuf<%s>::~basic_filebuf() attempted to "
                   "close a detached file descriptor", tname);

    {
        Filebuf fb;

        if (fb.fd () >= 0)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::fd() < 0, got %d for an "
                       "object that's not open", tname, fb.fd ());

        const int old_fd = fb.detach ();

        // verify detach() returns an invalid file descriptor when called
        // on a filebuf object that's not open
        if (0 <= old_fd)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::detach() < 0, got %d",
                       tname, old_fd);
    }

    const char* const tmpfname = rw_tmpnam (0);

    if (!tmpfname) {
        return;
    }

    {
        Filebuf fb;

        // open a writeable file
        const int fd = open (tmpfname, O_CREAT | O_WRONLY, 0666);

        if (fd < 0) {
            rw_warn (false, __FILE__, __LINE__,
                     "open (\"%s\", O_CREAT | O_WRONLY, 0666) "
                     "failed: %m", tmpfname);
        }

        // attach filebuf to the file descriptor
        fb.attach (fd);

        const charT data[] = { '0', '1', '2', '\0' };
        const std::size_t nelems = sizeof data / sizeof *data - 1;

        // write into the filebuf object
        const std::size_t wrote = fb.sputn (data, nelems);

        if (wrote != nelems)
            rw_assert (false, __FILE__, __LINE__,
                       "sputn(\"%s\", %d) == %d, got %d",
                       data, nelems, nelems, wrote);

        // and detach
        const int old_fd = fb.detach ();
        if (fd != old_fd)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::detach() == %d, got %d",
                       fd, old_fd);

        // verify by reading the contents of the named file that
        // the call to detach() wrote the contents of the filebuf
        // object's data buffer into the file before detaching
        std::size_t nbytes = 0;
        const char* const buf =
            _RWSTD_STATIC_CAST (char*, rw_fread (tmpfname, &nbytes));

        if (   !buf || nbytes != nelems
            || rw_strncmp (buf, data, nelems))
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::detach() failed to flush "
                       "data to the attached file descriptor; "
                       "read %lu bytes: %s, expected \"012\"",
                       tname, nbytes, buf);

        REMOVE_FILE (tmpfname);
    }

    {
        Filebuf fb;

        // create a new file and open it for writing
        const int fd = open (tmpfname, O_CREAT | O_WRONLY, 0666);

        // attach filebuf to the file descriptor (will set mode to ios::out
        // based on the O_WRONLY open mode of the file descriptor)
        fb.attach (fd);

        // close and reopen the same file descriptor as read-only
        close (fd);
        if (fd != open (tmpfname, O_RDONLY)) {
            rw_assert (false, __FILE__, __LINE__,
                       "open (\"%s\", O_RDONLY) != %d; aborting test",
                       tmpfname, fd);
            return;
        }

        static const charT data[] = { '0', '1', '2', '\0' };
        static const std::size_t nelems = sizeof data / sizeof *data - 1;

        // write into the filebuf object's buffer
        if (nelems != std::size_t (fb.sputn (data, nelems)))
            rw_assert (false, __FILE__, __LINE__, "sputn() failed");

        // and try to detach (must try to flush data into file)
        const int bad_fd = fb.detach ();

        // verify that detach() failed (due to the failure
        // to write to a read-only file descriptor)
        if (bad_fd >= 0)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::detach() < 0, got %d after "
                       "a failure to write to read-only file descriptor",
                       tname, bad_fd);

        // verify that the call to detach() failed to flush the contents
        // of the filebuf object's data buffer into a read-only file by
        // testing that the size of the file is unchanged (i.e., 0)
        std::size_t nbytes = 0;

        const char* buf  =
            _RWSTD_STATIC_CAST (char*, rw_fread (tmpfname, 0));

        if (buf && nbytes)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::detach() unexpectedly wrote "
                       "%u bytes to a read-only file: \"%s\"",
                       tname, nbytes, buf);

        // close and reopen the same file descriptor as write-only
        close (fd);
        if (fd != open (tmpfname, O_WRONLY)) {
            rw_assert (false, __FILE__, __LINE__,
                       "open (\"%s\", O_WRONLY) != %d; aborting test",
                       tmpfname, fd);
            return;
        }

        // and detach (must successfully flush data into file)
        const int good_fd = fb.detach ();

        // verify that the call to detach() was successful
        if (good_fd < 0)
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::detach() == %d, got %d",
                       tname, fd, good_fd);

        // verify that the filebuf object is no longer open
        if (fb.is_open ())
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::is_open() unexpectedly "
                       "true after a successful call to detach()",
                       tname);

        // verify that this call to detach() wrote out the contents of the
        // filebuf object's data buffer into the writeable file by checking
        // the size and contents of the file
        buf = (char*)rw_fread (tmpfname, &nbytes);

        if (   !buf || nbytes != nelems
            || rw_strncmp (buf, data, nelems))
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::detach() failed to flush "
                       "data to the attached file descriptor; "
                       "read %lu bytes: %s, expected %s",
                       tname, nbytes, buf, data);

        // close the file descriptor before removing the file
        close (good_fd);

        REMOVE_FILE (tmpfname);
    }

#else   // if defined (_RWSTD_NO_EXT_FILEBUF)

    _RWSTD_UNUSED (tname);

#endif   // _RWSTD_NO_EXT_FILEBUF

}

/***************************************************************************/

// CodeCvt<charT> performs a state-dependent conversion
// the first byte of the state_type object encodes the state,
// any remaining bytes must be 0
// internal characters are externally represented as sequences
// of 1 to 22 chars, escape sequences are 11 to 13 chars long
// and all start with the "<ESC-" prefix
//
// for example, the string "Hello, World!\n" will be externally
// represented by the following sequence of narrow characters:
//
// "<ESC-UPPER>H<ESC-LOWER>ello<ESC-PUNCT><comma><SP><ESC-UPPER>"
// "W<ESC-LOWER>orld<ESC-PUNCT><exclamation-mark><ESC-CNTRL>\n"

template <class charT>
struct CodeCvt: std::codecvt<charT, char, std::mbstate_t>
{
    typedef std::codecvt<charT, char, std::mbstate_t> Base;

    enum { cntrl, punct, digit, upper, lower, hexcode };

public:

    typedef typename Base::intern_type intern_type;
    typedef typename Base::extern_type extern_type;
    typedef typename Base::state_type  state_type;

    explicit CodeCvt (std::size_t ref = 0)
        : Base (ref) { }

protected:

    virtual std::codecvt_base::result
    do_out (state_type&,
            const intern_type*, const intern_type*, const intern_type*&,
            extern_type*, extern_type*, extern_type*&) const;

    virtual std::codecvt_base::result
    do_in (state_type&,
           const extern_type*, const extern_type*, const extern_type*&,
           intern_type*, intern_type*, intern_type*&) const;

    virtual std::codecvt_base::result
    do_unshift (state_type&, extern_type*, extern_type*, extern_type*&) const;

    virtual int do_encoding () const _THROWS (()) {
        return -1;   // state-dependent encoding
    }

    virtual bool do_always_noconv () const _THROWS (()) {
        return false;   // conversion always necessary
    }

    // returns the maximum `N' of extern chars in the range [from, from_end)
    // such that N represents max or fewer internal chars
    virtual int
    do_length (state_type&, const extern_type*,
               const extern_type*, std::size_t) const;

    // returns the max value do_length (s, from, from_end, 1) can return
    // for any valid range [from, from_end) - see LWG issue 74
    virtual int do_max_length () const _THROWS (()) {
        return -1;
    }
};


extern const char* const escapes[] = {
    "<ESC-CNTRL>", "<ESC-PUNCT>", "<ESC-DIGIT>",
    "<ESC-UPPER>", "<ESC-LOWER>", "<ESC-HEXCODE>"
};


extern const char* const charnames[] = {
    // control characters (<ESC-CNTRL>)
    "<NUL>", "<SOH>", "<STX>", "<ETX>", "<EOT>", "<ENQ>", "<ACK>", "<BEL>",
    "<BS>", "<HT>", "\n", "<VT>", "<FF>", "<CR>" /* '\r' */, "<SO>", "<SI>",
    "<DLE>", "<DC1>", "<DC2>", "<DC3>", "<DC4>", "<NAK>", "<SYN>", "<ETB>",
    "<CAN>", "<EM>", "<SUB>", "<ESC>", "<FS>", "<GS>", "<RS>", "<US>",
    // punctuators (<ESC-PUNCT>)
    "<SP>",
    "<exclamation-mark>",
    "<quotation-mark>",
    "<number-sign>",
    "<dollar-sign>",
    "<percent-sign>",
    "<ampersand>",
    "<apostrophe>",
    "<left-parenthesis>",
    "<right-parenthesis>",
    "<asterisk>",
    "<plus-sign>",
    "<comma>",
    "<hyphen>",
    "<period>",
    "<slash>",
    // digits (<ESC-DIGIT>)
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    // punctuators (<ESC-PUNCT>)
    "<colon>",
    "<semicolon>",
    "<less-than-sign>",
    "<equals-sign>",
    "<greater-than-sign>",
    "<question-mark>",
    "<commercial-at>",
    // uppercase letters (<ESC_UPPER>)
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
    // punctuators (<ESC-PUNCT>)
    "<left-square-bracket>",
    "<backslash>",
    "<right-square-bracket>",
    "<circumflex>",
    "<underscore>",
    "<grave-accent>",
    // lowercase letters (<ESC_LOWER>)
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
    "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
    // punctuators (<ESC-PUNCT>)
    "<left-curly-bracket>",
    "<vertical-line>",
    "<right-curly-bracket>",
    "<tilde>",
    // control characters (<ESC-CNTRL>)
    "<DEL>"
    // characters above '\x7f' are represented in hex (<ESC-HEXCODE>)
};


template <class charT>
std::codecvt_base::result
CodeCvt<charT>::do_out (      state_type  &state,
                        const intern_type *from,
                        const intern_type *from_end,
                        const intern_type *&from_next,
                              extern_type *to,
                              extern_type *to_end,
                              extern_type *&to_next) const
{
    std::codecvt_base::result res = std::codecvt_base::ok;

    char hexbuf [5] = "";

    int int_state = hexcode;

    for (from_next = from, to_next = to; from_next != from_end; ++from_next) {

        typedef std::char_traits<intern_type> Traits;

        const std::size_t uch = std::size_t (Traits::to_int_type (*from_next));

        if (uch < ' ') {
            int_state = cntrl;
        }
        else if (uch >= ' ' && uch <= '/') {
            int_state = punct;
        }
        else if (uch >= '0' && uch <= '9') {
            int_state = digit;
        }
        else if (uch >= ':' && uch <= '@') {
            int_state = punct;
        }
        else if (uch >= 'A' && uch <= 'Z') {
            int_state = upper;
        }
        else if (uch >= '[' && uch <= '`') {
            int_state = punct;
        }
        else if (uch >= 'a' && uch <= 'z') {
            int_state = lower;
        }
        else if (uch >= '{' && uch <= '~') {
            int_state = punct;
        }
        else if (uch == '\x7f') {
            int_state = cntrl;
        }
        else if (uch <= 0xff) {
            int_state = hexcode;

            if (to_end - to_next < 4) {
                res = std::codecvt_base::partial;
                break;
            }

            // convert `uch' to a hexadecimal escape sequence
            static const char hexdigits[] = "0123456789abcdef";

            hexbuf [0] = '\\';
            hexbuf [1] = 'x';
            hexbuf [2] = hexdigits [uch >> 8];
            hexbuf [3] = hexdigits [uch & 0x0fU];
            hexbuf [4] = '\0';
        }
        else {
            res = std::codecvt_base::error;
            break;
        }

        if (int (*(char*)&state) != int_state) {

            const std::size_t len = std::strlen (escapes [int_state]);

            if (len > std::size_t (to_end - to_next)) {
                res = std::codecvt_base::partial;
                break;
            }

            std::memcpy (to_next, escapes [int_state], len);
            to_next += len;

            *(char*)&state = char (int_state);
        }

        const char* const outs = *hexbuf ? hexbuf : charnames [uch];

        const std::size_t len = std::strlen (outs);

        if (len > std::size_t (to_end - to_next)) {
            res = std::codecvt_base::partial;
            break;
        }

        std::memcpy (to_next, outs, len);
        to_next += len;
    }

    return res;
}


// looks for a character whose name starts at `*pbeg' in the table
// of character names, `chartbl' containing `tblsize' elements
// if a unique (even partial) match is found, returns the index of
// the character in the table; if the unique match is complete,
// also advances `*pbeg' by the length of the match
std::size_t find_char (const char **pbeg, const char *end,
                       const char* const chartbl[], std::size_t tblsize)
{
    const std::size_t nchars = sizeof charnames / sizeof *charnames;

    std::size_t hits [nchars] = { 0 };

    const char* cur = *pbeg;

    for (; cur != end; ++cur) {

        const std::size_t inx = cur - *pbeg;

        std::size_t nmatches = 0;

        for (std::size_t i = 0; i != tblsize; ++i) {
            if (hits [i] == inx && chartbl [i] && *cur == chartbl [i][inx]) {
                ++hits [i];
                ++nmatches;
            }
        }

        if (!nmatches)
            break;
    }

    std::size_t nmatches = 0;
    std::size_t imaxhits = 0;
    std::size_t inx      = std::size_t (-1);

    for (std::size_t i = 0; i != tblsize; ++i) {

        if (!chartbl [i])
            continue;

        if (hits [i] > hits [imaxhits])
            imaxhits = i;

        if (hits [i] == std::strlen (chartbl [i])) {

            if (std::size_t (-1) == inx)
                inx = 0;

            if (hits [i] == hits [inx]) {
                ++nmatches;
                inx = i;
            }
            else if (hits [i] > hits [inx]) {
                nmatches = 1;
                inx = i;
            }
        }
        else if (std::size_t (-1) != inx && hits [i] > hits [inx]) {
            nmatches = 0;
            inx = i;
        }
    }

    if (inx != std::size_t (-1) && 1 == nmatches)
        *pbeg = cur;

    if (hits [imaxhits] == std::size_t (end - *pbeg))
        inx = imaxhits;

    return inx;
}


template <class charT>
std::codecvt_base::result
CodeCvt<charT>::do_in (      state_type  &state,
                       const extern_type *from,
                       const extern_type *from_end,
                       const extern_type *&from_next,
                             intern_type *to,
                             intern_type *to_end,
                             intern_type *&to_next) const
{
    std::codecvt_base::result res = std::codecvt_base::ok;

    for (to_next = to, from_next = from; to_next != to_end; ++to_next) {

    top_of_loop:

        if (from_next == from_end)
            break;

        const std::size_t navail = from_end - from_next;

        // check the beginning of the sequence to see if
        // it may possibly start with an escape sequence

        if (navail <= 5 && !std::memcmp (from_next, "<ESC-", navail))
            return std::codecvt_base::partial;

        if (navail > 5 && !std::memcmp (from_next, "<ESC-", 5)) {

            // found the beginning of what might be an escape sequence

            if (navail >= 11) {

                const std::size_t nescapes = sizeof escapes / sizeof *escapes;

                for (std::size_t i = 0; i != nescapes; ++i) {

                    const std::size_t len = std::strlen (escapes [i]);

                    if (!std::memcmp (escapes [i], from_next, len)) {

                        // found an escape sequence

                        // set the state variable
                        *(char*)&state = char (i);

                        // advance past the escape sequence
                        from_next += len;

                        // continue iterating over the rest of the sequence
                        goto top_of_loop;
                    }
                }
            }
            else {
                // not enough external elements to convert
                // to an internal character
                return std::codecvt_base::partial;
            }
        }

        std::size_t chinx = std::size_t (-1);   // character index
        std::size_t choff = 0;                  // character offset

        const char* const from_next_save = from_next;

        switch (*(char*)&state) {

        case cntrl:
            chinx = find_char (&from_next, from_end, charnames, ' ' + 1U);
            if (   std::size_t (-1) == chinx
                && 5 <= from_end - from_next
                && !std::memcmp (from_next, "<DEL>", 5)) {
                chinx = '\x7f';
                from_next += 5;
            }
            break;

        case punct: {

            static const char* const pun[] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                "<SP>",
                "<exclamation-mark>",
                "<quotation-mark>",
                "<number-sign>",
                "<dollar-sign>",
                "<percent-sign>",
                "<ampersand>",
                "<apostrophe>",
                "<left-parenthesis>",
                "<right-parenthesis>",
                "<asterisk>",
                "<plus-sign>",
                "<comma>",
                "<hyphen>",
                "<period>",
                "<slash>",
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                "<colon>",
                "<semicolon>",
                "<less-than-sign>",
                "<equals-sign>",
                "<greater-than-sign>",
                "<question-mark>",
                "<commercial-at>",
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                "<left-square-bracket>",
                "<backslash>",
                "<right-square-bracket>",
                "<circumflex>",
                "<underscore>",
                "<grave-accent>",
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                "<left-curly-bracket>",
                "<vertical-line>",
                "<right-curly-bracket>",
                "<tilde>",
                0
            };

            chinx = find_char (&from_next, from_end,
                               pun, sizeof pun / sizeof *pun);

            break;
        }

        case digit:
            choff = '0';

            if (*from_next >= '0' && *from_next <= '9')
                chinx = *from_next++ - '0';

            // commented out code replaced by the above for efficiency
            // chinx = find_char (&from_next, from_end, charnames + choff, 10);
            break;

        case lower:
            choff = 'a';

            // assume ASCII
            if (*from_next >= 'a' && *from_next <= 'z')
                chinx = *from_next++ - 'a';

            // commented out code replaced by the above for efficiency
            // chinx = find_char (&from_next, from_end, charnames + choff, 26);
            break;

        case upper:
            choff = 'A';

            // assume ASCII
            if (*from_next >= 'A' && *from_next <= 'Z')
                chinx = *from_next++ - 'A';

            // commented out code replaced by the above for efficiency
            // chinx = find_char (&from_next, from_end, charnames + choff, 26);
            break;

        case hexcode: {
            if (from_end - from_next < 4)
                return std::codecvt_base::partial;

            if (   from_next [0] != '\\' || from_next [1] != 'x'
                || !(   (from_next [2] >= '0' && from_next [2] <= '9')
                     || (from_next [2] >= 'A' && from_next [2] <= 'F')
                     || (from_next [2] >= 'a' && from_next [2] <= 'f'))
                || !(   (from_next [3] >= '0' && from_next [3] <= '9')
                     || (from_next [3] >= 'A' && from_next [3] <= 'F')
                     || (from_next [3] >= 'a' && from_next [3] <= 'f')))
                return std::codecvt_base::error;

            // convert a hex literal to a number
            chinx = 0;
            for (unsigned i = 2; i != 4; ++i) {
                chinx <<= 4;
                if (from_next [i] >= '0' && from_next [i] <= '9')
                    chinx += from_next [i] - '0';
                else if (from_next [i] >= 'A' && from_next [i] <= 'F')
                    chinx += 10 + (from_next [i] - 'A');
                else
                    chinx += 10 + (from_next [i] - 'a');
            }

            break;
        }

        default:
            chinx = std::size_t (-1);
            break;
        }

        if (std::size_t (-1) == chinx) {
            res = std::codecvt_base::error;
            break;
        }

        // the combination of a valid `chinx' and an unmodified
        // `from_next' pointer indicates a partial match
        if (from_next == from_next_save) {
            res = std::codecvt_base::partial;
            break;
        }

        *to_next = charT (chinx + choff);
    }

    return res;
}


template <class charT>
std::codecvt_base::result
CodeCvt<charT>::
do_unshift (state_type  &state,
            extern_type *to,
            extern_type *to_end,
            extern_type *&to_next) const
{
    to_next = to;

    if (!*(char*)&state)
        return std::codecvt_base::noconv;

    const std::size_t len = std::strlen (escapes [0]);

    if (len > std::size_t (to_end - to_next))
        return std::codecvt_base::partial;

    std::memcpy (to_next, escapes [0], len);
    to_next += len;

    std::memset (&state, 0, sizeof state);

    return std::codecvt_base::ok;
}


template <class charT>
int
CodeCvt<charT>::
do_length (state_type&, const extern_type*,
           const extern_type*, std::size_t) const
{
    return 0;
}

/***************************************************************************/

static const char*
get_codecvt_result (std::codecvt_base::result res)
{
    switch (res) {
    case std::codecvt_base::error: return "std::codecvt_base::error";
    case std::codecvt_base::noconv: return "std::codecvt_base::noconv";
    case std::codecvt_base::ok: return "std::codecvt_base::ok";
    case std::codecvt_base::partial: return "std::codecvt_base::partial";
    }

    return "unknown";
}


template <class charT>
static void
test_codecvt (const char* tname)
{
    rw_info (0, __FILE__, __LINE__,
             "std::basic_filebuf<%s> with a state-dependent encoding",
             tname);

    rw_info (0, __FILE__, __LINE__,
             "sgetn() with state-dependent code conversion");

    // read in the text of this source file as plain text

    typedef std::basic_filebuf<charT, std::char_traits<charT> > Filebuf;
    typedef typename Filebuf::off_type                          off_type;

    Filebuf noconv_in;

    // use ios_base::binary to avoid CR/LF conversion issues
    if (!noconv_in.open (__FILE__, std::ios::binary | std::ios::in)) {
        rw_assert (false, __FILE__, __LINE__,
                   "basic_filebuf<%s>::open (\"%s\", "
                   "ios_base::binary | ios_base::in) failed",
                   tname, __FILE__);
        return;
    }

    // seek to the end of the file to get its size in bytes
    const std::streamsize noconv_fsize =
        noconv_in.pubseekoff (0, std::ios::end);

    if (noconv_fsize <= 0) {
        rw_assert (false, __FILE__, __LINE__,
                   "basic_filebuf<%s>::pubseekoff "
                   "(0, ios_base::end) >= 0, got %i",
                   tname, noconv_fsize);
        return;
    }

    if (0 != noconv_in.pubseekoff (0, std::ios::beg)) {
        rw_assert (false, __FILE__, __LINE__,
                   "basic_filebuf<%s>::pubseekoff "
                   "(0, ios_base::beg) failed", tname);
        return;
    }

    const char* const tmpfname = rw_tmpnam (0);

    if (!tmpfname) {
        return;
    }

    // allocate a buffer large enough to hold the contents of the
    // file in internal representation without codeset conversion
    charT *noconv_intbuf = new charT [noconv_fsize];

    // sgetn() returns the number of internal characters read in
    // which must equal the number of external characters read by
    // the `noconv_in' file buffer (since it does no conversion)
    std::streamsize nread = noconv_in.sgetn (noconv_intbuf, noconv_fsize);

    if (nread != noconv_fsize) {
        rw_assert (false, __FILE__, __LINE__,
                   "basic_filebuf<%s>::sgetn (%p, %i) == %i), got %i",
                   tname, noconv_intbuf,
                   noconv_fsize, noconv_fsize, nread);
        delete[] noconv_intbuf;
        return;
    }

    const CodeCvt<charT> cvt (1);

    // write out the text of this file using the test codecvt facet
    // that performs state-dependent encoding
    Filebuf conv_out;

    conv_out.pubimbue (std::locale (std::locale::classic (), &cvt));

    if (!conv_out.open (tmpfname, std::ios::binary | std::ios::out)) {
        rw_assert (false, __FILE__, __LINE__,
                   "basic_filebuf<%s>::open (\"%s\", ios_base::out) "
                   "failed", tname, tmpfname);
        delete[] noconv_intbuf;
        return;
    }

    // sputn() returns the number of internal characters written out
    // which must equal the number of external characters read by
    // the `noconv_in' file buffer (since it does no conversion)
    const std::streamsize nwrote = conv_out.sputn (noconv_intbuf, nread);

    if (nread != nwrote) {
        rw_assert (false, __FILE__, __LINE__,
                   "basic_filebuf<%s>::sputn (\"%s\", %i) == %i), "
                   "got %i", tname,
                   noconv_intbuf, nread, nread, nwrote);
        delete[] noconv_intbuf;
        return;
    }

    // close file buffer to flush it out
    if (!conv_out.close ())
        rw_warn (false, __FILE__, __LINE__,
                 "basic_filebuf<%s>(\"%s\").close() failed",
                 tname, tmpfname);

    std::filebuf nin;   // narrow input stream buffer

    if (!nin.open (tmpfname, std::ios::binary | std::ios::in)) {
        rw_assert (false, __FILE__, __LINE__,
                   "basic_filebuf<%s>::open (\"%s\", ios_base::in) failed",
                   tname, tmpfname);
        delete[] noconv_intbuf;
        return;
    }

    // get the size of the converted file in external characters
    const std::streamsize conv_fsize = nin.pubseekoff (0, std::ios::end);

    if (conv_fsize <= noconv_fsize) {
        rw_assert (false, __FILE__, __LINE__,
                   "filebuf::pubseekoff (0, ios_base::end) > %i, got %i",
                   noconv_fsize, conv_fsize);
        delete[] noconv_intbuf;
        return;
    }

    if (0 != nin.pubseekoff (0, std::ios::beg)) {
        rw_assert (false, __FILE__, __LINE__,
                   "filebuf::pubseekoff (0, ios_base::beg) failed");
        delete[] noconv_intbuf;
        return;
    }

    // allocate a buffer large enough to hold the contents of the
    // file in external representation without codeset conversion
    char *noconv_extbuf = new char [conv_fsize];
    std::memset (noconv_extbuf, 0, conv_fsize);

    // read in the (unconverted) contents of the file
    nread = nin.sgetn (noconv_extbuf, conv_fsize);

    if (nread != conv_fsize) {
        rw_assert (false, __FILE__, __LINE__,
                   "filebuf::sgetn (%p, %i) == %i, got %i",
                   noconv_extbuf, conv_fsize, conv_fsize, nread);
        delete[] noconv_intbuf;
        delete[] noconv_extbuf;
        return;
    }

    if (!nin.close ())
        rw_warn (false, __FILE__, __LINE__,
                 "basic_filebuf<%s>(\"%s\").close() failed",
                 tname, tmpfname);

    std::mbstate_t state;
    std::memset (&state, 0, sizeof state);

    const char *from      = noconv_extbuf;
    const char *from_end  = noconv_extbuf + nread;
    const char *from_next = 0;

    // allocate a buffer large enough to hold the contents of the
    // file in internal representation with codeset conversion
    charT *conv_intbuf = new charT [noconv_fsize];
    std::memset (conv_intbuf, 0, noconv_fsize * sizeof *conv_intbuf);

    charT *to      = conv_intbuf;
    charT *to_end  = conv_intbuf + noconv_fsize;
    charT *to_next = 0;

    // convert the contents of the file using the test codecvt facet
    const std::codecvt_base::result cvtres =
        cvt.in (state, from, from_end, from_next, to, to_end, to_next);

    if (std::codecvt_base::ok != cvtres) {
        rw_assert (false, __FILE__, __LINE__,
                   "CodeCvt<%s>::in ({ %d, ... },  %p, %p, %p, %p, %p, %p) "
                   "== std::codecvt_base::ok, got %s",
                   tname, *(unsigned char*)&state,
                   from, from_end, from_next, to, to_end, to_next,
                   get_codecvt_result (cvtres));
        delete[] noconv_intbuf;
        delete[] noconv_extbuf;
        delete[] conv_intbuf;
        return;
    }

    const std::streamsize n_ext_cvt = from_next - from;
    const std::streamsize n_int_cvt = to_next - to;

    rw_assert (n_ext_cvt == conv_fsize, __FILE__, __LINE__,
               "CodeCvt<%s>::in() converted %i external characters, "
               "expected %i", tname, n_ext_cvt, conv_fsize);

    rw_assert (n_int_cvt == noconv_fsize, __FILE__, __LINE__,
               "CodeCvt<%s>::in() produced %i internal characters, "
               "expected %i", tname, n_int_cvt, noconv_fsize);

    // verify that the contents of the unconverted internal buffer
    // (i.e., the text of this file in internal representation) are
    // the same as the contents of the converted internal buffer
    // (i.e., the text of this file converted to the external
    // state-dependent representation and then converted back to
    // the internal encoding)
    rw_assert (!std::memcmp (noconv_intbuf, conv_intbuf, n_int_cvt),
               __FILE__, __LINE__, "code conversion mismatch");


    // read in the text of the temporary file encoded in
    // a state-dependent encoding and convert it to its
    // internal representation
    Filebuf conv_in;

    conv_in.pubimbue (std::locale (std::locale::classic (), &cvt));

    conv_in.open (tmpfname, std::ios::in);

    std::memset (conv_intbuf, 0, noconv_fsize * sizeof *conv_intbuf);

    nread = conv_in.sgetn (conv_intbuf, noconv_fsize);

    if (nread != noconv_fsize) {
        rw_assert (false, __FILE__, __LINE__,
                   "basic_filebuf<%s>::sgetn (%p, %i) == %i, got %i",
                   tname, conv_intbuf, noconv_fsize,
                   noconv_fsize, nread);
        delete[] noconv_intbuf;
        delete[] noconv_extbuf;
        delete[] conv_intbuf;
        return;
    }

    rw_assert (!std::memcmp (noconv_intbuf, conv_intbuf,
                             n_int_cvt * sizeof *noconv_intbuf),
               __FILE__, __LINE__, "code conversion mismatch");

    if (!noconv_in.close ())
        rw_warn (false, __FILE__, __LINE__,
                 "basic_filebuf<%s>(" __FILE__ ").close() failed",
                 tname);

    if (!conv_in.close ())
        rw_warn (false, __FILE__, __LINE__,
                 "basic_filebuf<%s>(\"%s\").close() failed",
                 tname, tmpfname);

    rw_info (0, __FILE__, __LINE__,
             "pubseekoff() and pubseekpos() with state-dependent "
             "code conversion");

    noconv_in.open (__FILE__, std::ios::binary | std::ios::in);
    conv_in.open (tmpfname, std::ios::binary | std::ios::in);

    for (std::streamsize i = 0; i < noconv_fsize; i += noconv_fsize / 13) {

        typedef typename Filebuf::pos_type pos_type;

        noconv_in.pubseekoff (0, std::ios::beg);
        conv_in.pubseekoff (0, std::ios::beg);

        std::memset (noconv_intbuf, 0, noconv_fsize * sizeof *noconv_intbuf);
        std::memset (conv_intbuf, 0, noconv_fsize * sizeof *conv_intbuf);

        nread = noconv_in.sgetn (noconv_intbuf, i);

        if (i != nread) {
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::sgetn (%p, %d) == %d, "
                       "got %d", tname, noconv_intbuf, i, i, nread);
            break;
        }

        nread = conv_in.sgetn (conv_intbuf, i);

        if (i != nread) {
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::sgetn (%p, %d) == %d, "
                       "got %d", tname, conv_intbuf, i, i, nread);
            break;
        }

        if (std::memcmp (noconv_intbuf, conv_intbuf,
                         i * sizeof *conv_intbuf)) {
            rw_assert (false, __FILE__, __LINE__,
                       "%d. code conversion mismatch", i);
            break;
        }

        const pos_type noconv_pos =
            noconv_in.pubseekoff (0, std::ios::cur);

        const pos_type conv_pos =
            conv_in.pubseekoff (0, std::ios::cur);

        std::memset (noconv_intbuf, 0, noconv_fsize * sizeof *noconv_intbuf);
        std::memset (conv_intbuf, 0, noconv_fsize * sizeof *conv_intbuf);

        nread = noconv_in.sgetn (noconv_intbuf, noconv_fsize);

        if (noconv_fsize - i != nread) {
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::sgetn (%p, %d) == %d, "
                       "got %d", tname, noconv_intbuf,
                       noconv_fsize, noconv_fsize - i, nread);
            break;
        }

        nread = conv_in.sgetn (conv_intbuf, noconv_fsize);

        if (noconv_fsize - i != nread) {
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::sgetn (%p, %d) == %d, "
                       "got %d", tname, conv_intbuf,
                       noconv_fsize, noconv_fsize - i, nread);
            break;
        }

        if (std::memcmp (noconv_intbuf, conv_intbuf,
                         nread * sizeof *noconv_intbuf)) {
            rw_assert (false, __FILE__, __LINE__,
                       "%d. data mismatch after conversion", i);
            break;
        }

        if (!noconv_in.close ())
            rw_warn (false, __FILE__, __LINE__,
                     "basic_filebuf<%s>(" __FILE__ ").close() failed",
                     tname);

        if (!conv_in.close ())
            rw_warn (false, __FILE__, __LINE__,
                     "basic_filebuf<%s>(\"%s\").close() failed",
                     tname, tmpfname);

        noconv_in.open (__FILE__, std::ios::binary | std::ios::in);
        conv_in.open (tmpfname, std::ios::binary | std::ios::in);

        const pos_type noconv_pos_new = noconv_in.pubseekpos (noconv_pos);

        if (noconv_pos_new != noconv_pos) {

            std::mbstate_t noconv_state     = noconv_pos.state ();
            std::mbstate_t noconv_state_new = noconv_pos_new.state ();

            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::pubseekpos ({ %d, %d }, "
                       "ios_base::cur) == { %d, %d }, got { %d, %d }",
                       tname,
                       off_type (noconv_pos), *(char*)&noconv_state,
                       off_type (noconv_pos), *(char*)&noconv_state,
                       off_type (noconv_pos_new),
                       *(char*)&noconv_state_new);
            break;
        }

        const pos_type conv_pos_new = conv_in.pubseekpos (conv_pos);

        if (conv_pos_new != conv_pos) {
            std::mbstate_t conv_state     = noconv_pos.state ();
            std::mbstate_t conv_state_new = noconv_pos_new.state ();

            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::pubseekpos ({ %d, %d }, "
                       "ios_base::cur) == { %d, %d }, got { %d, %d }",
                       tname,
                       off_type (conv_pos), *(char*)&conv_state,
                       off_type (conv_pos), *(char*)&conv_state,
                       off_type (conv_pos_new),
                       *(char*)&conv_state_new);
            break;
        }

        std::memset (noconv_intbuf, 0, noconv_fsize * sizeof *noconv_intbuf);
        nread = noconv_in.sgetn (noconv_intbuf, noconv_fsize);

        if (noconv_fsize - i != nread) {
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::sgetn (%p, %d) == %d, "
                       "got %d (no code conversion)",
                       tname, noconv_intbuf,
                       noconv_fsize, noconv_fsize - i, nread);
            break;
        }

        if (std::memcmp (noconv_intbuf, conv_intbuf,
                         nread * sizeof *noconv_intbuf)) {
            rw_assert (false, __FILE__, __LINE__,
                      "data mismatch (no code conversion)");
            break;
        }

        std::memset (conv_intbuf, 0, noconv_fsize * sizeof *conv_intbuf);
        nread = conv_in.sgetn (conv_intbuf, noconv_fsize);

        if (noconv_fsize - i != nread) {
            rw_assert (false, __FILE__, __LINE__,
                       "basic_filebuf<%s>::sgetn (%p, %d) == %d, "
                       "got %d (with code conversion)",
                       tname, conv_intbuf,
                       noconv_fsize, noconv_fsize - i, nread);
            break;
        }

        if (std::memcmp (noconv_intbuf, conv_intbuf,
                         nread * sizeof *noconv_intbuf)) {
            rw_assert (false, __FILE__, __LINE__,
                       "code conversion mismatch");
            break;
        }
    }

    delete[] noconv_intbuf;
    delete[] noconv_extbuf;
    delete[] conv_intbuf;

    if (!noconv_in.close ())
        rw_warn (false, __FILE__, __LINE__,
                 "basic_filebuf<%s>(" __FILE__ ").close() failed",
                 tname);

    if (!conv_in.close ())
        rw_warn (false, __FILE__, __LINE__,
                 "basic_filebuf<%s>(\"%s\").close() failed",
                 tname, tmpfname);

    REMOVE_FILE (tmpfname);
}

/***************************************************************************/


template <class charT>
static void
test_codecvt_with_seek ()
{
    rw_info (0, __FILE__, __LINE__,
             "interleaved input, output, and seeks in ios_base::app "
             "mode with state-dependent code conversion");

    typedef std::basic_filebuf<charT, std::char_traits<charT> > FileBuf;

    FileBuf inbuf;

    // open this source file
    inbuf.open (__FILE__, std::ios::in);

    const std::streamsize N = 0xfffff;
    charT *buf = new charT [N];

    // read the text of the file into a local buffer
    const std::streamsize nchars = inbuf.sgetn (buf, N);

    if (nchars <= 0) {
        rw_assert (false, __FILE__, __LINE__,
                   "sgetn(..., %d) > 0, got %d", N, nchars);
        delete[] buf;
        return;
    }

    inbuf.close ();

    FileBuf iobuf;

    // imbue a locale containing a CodeCvt<charT> facet into the filebuf
    iobuf.pubimbue (std::locale (std::locale::classic (), new CodeCvt<charT>));

    const char* const tmpfname = rw_tmpnam (0);

    if (!tmpfname) {
        delete[] buf;
        return;
    }

    // remove file just in case it exists (it shouldn't)
    std::remove (tmpfname);

    // open the filebuf on a temporary file
    if (!iobuf.open (tmpfname, std::ios::in | std::ios::out | std::ios::app)) {
        rw_assert (false, __FILE__, __LINE__,
                   "open (\"%s\", ios::in | ios::out | ios::app) failed",
                   tmpfname);
        delete[] buf;
        return;
    }

    charT *pb = buf;

    // alterante writes, seeks, and reads to/from the filebuf
    for (std::streamsize n = nchars, i = nchars; n; i = n / 2) {

        const std::streamsize nput = iobuf.sputn (pb, n - i);

        iobuf.pubseekoff (0, std::ios::beg);

        charT *localbuf = new charT [N];
        const std::streamsize nget = iobuf.sgetn (localbuf, n - i);

        // verify the consistency of the read data
        if (nget != n - i || std::memcmp (buf, localbuf, nget * sizeof *buf)) {

            rw_assert (false, __FILE__, __LINE__,
                       "sgetn(..., %d) == %d, got %d (or inconsistent data)",
                       n - i, n - i, nget);
            delete[] localbuf;
            break;
        }

        delete[] localbuf;

        iobuf.pubseekoff (0, std::ios::beg);

        pb += nput;
        n  -= nput;

    }

    iobuf.close ();

    // imbue the filebuf object with a locale with the conversion facet
    inbuf.pubimbue (std::locale (std::locale::classic (),
                                 new CodeCvt<charT>));

    // and open the temporary file one more time
    if (!inbuf.open (tmpfname, std::ios::in)) {
        rw_assert (false, __FILE__, __LINE__, "");
        delete[] buf;

        REMOVE_FILE (tmpfname);
        return;
    }

    charT *cvtbuf = new charT [N];

    // read the whole file into a temporary buffer
    std::streamsize ncvt = inbuf.sgetn (cvtbuf, N);

    // verify that the contents are consistent with the original data
    if (nchars != ncvt || std::memcmp (buf, cvtbuf, nchars * sizeof *buf)) {
        rw_assert (false, __FILE__, __LINE__,
                   "sgetn(..., %d) == %d, got %d (or inconsistent data)",
                   N, nchars, ncvt);

    }

    delete[] buf;
    delete[] cvtbuf;

    // close file before removing it
    inbuf.close ();

    REMOVE_FILE (tmpfname);
}

/***************************************************************************/


template <class charT>
static void
do_test (const char* tname)
{
    rw_info (0, __FILE__, __LINE__, "std::basic_filebuf<%s>", tname);

    // exercise basic_filebuf constructors
    test_ctors<charT> (tname);

    // exercise basic_filebuf::open()
    test_open<charT> (tname);

    // exercise basic_filebuf::sync()
    test_sync<charT> (tname);

    // exercise the basic_filebuf::attach() and detach() extensions
    test_attach<charT> (tname);

    // exercise basic_filebuf functionality with a non-trivial
    // codecvt facet that implements stateful conversion
    test_codecvt<charT> (tname);
    test_codecvt_with_seek<charT> ();
}


static int
run_test (int /*argc*/, char* /*argv*/ [])
{
    do_test<char> ("char");

#ifndef _RWSTD_NO_WCHAR_T

    do_test<wchar_t> ("wchar_t");

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}


/*extern*/ int
main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "[lib.filebuf]",
                    "",   // no comment
                    run_test, "", 0);
}

