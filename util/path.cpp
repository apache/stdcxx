/***************************************************************************
 *
 * path.cpp
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef _WIN32
#  ifdef __linux__
     // for symlink()
#    ifndef _XOPEN_SOURCE
#      define _XOPEN_SOURCE
#    endif
#    ifndef _XOPEN_SOURCE_EXTENDED
#      define _XOPEN_SOURCE_EXTENDED
#    endif
#  endif   // __linux__

#  include <unistd.h>      // for getcwd()
#  include <sys/stat.h>    // for struct stat, stat()
#else
#  include <direct.h>      // for struct _stat
#  include <io.h>
#  include <windows.h>     // for _getcwd()
#  include <sys/types.h>
#  include <sys/stat.h>    // for struct stat, stat()
#endif  // _WIN32


#include "path.h"

#include "diagnostic.h"   // for issue_diag()

#include <cassert>        // for assert()
#include <cerrno>         // for ERANGE
#include <cstdlib>        // for getenv()
#include <cstring>        // for strerror()
#include <string>         // for string


static char* get_cwd (char* s, std::size_t l)
{
#if !defined (_MSC_VER)
    return getcwd (s, l);
#else
    return _getcwd (s, l);
#endif
}


#if !defined (_WIN32) && !defined (__CYGWIN__)

void
create_symlink (const std::string &dir, 
                const std::string &xname, 
                const std::string &sname)
{
    int   ret  = 0;
    char* pret = 0;
    char  buf [1024];

    if (dir.size ()) {
        pret = get_cwd (buf, 1024);
        if (pret == 0) {
            issue_diag (213, true, 0, 
                        "could not retrieve current working directory\n") ;
        }
    
        ret = chdir (dir.c_str ());
        if (ret == -1)
            issue_diag (214, false, 0, "could not change directory to %s\n", 
                        dir.c_str ()) ;
    }

    ret = symlink(xname.c_str(), sname.c_str());

#ifdef EEXIST

    if (ret == -1 && errno != EEXIST) {
        issue_diag (215, false, 0,
                    "could not create symbolic link to %s "
                    "locale database (%s)\n", 
                    xname.c_str (), std::strerror (errno)) ;
    }

#endif   // EEXIST

    // go back to previous wd
    if (dir.size ()) {
        chdir (pret);
    }
}

#endif  // !_WIN32 && !__CYGWIN__


inline int
filemode (const char *path)
{
    assert (0 != path);

    struct stat buf;
    return stat (path, &buf) ? 0 : buf.st_mode;
}


std::string
get_pathname (const std::string &fname,
              const std::string &other /* = std::string () */)
{
    // use absolute path as given by fname
#if !defined (_WIN32)
    if (fname.size () && _RWSTD_PATH_SEP == fname [0]) 
#else
    if (fname.size () && 
        fname [1] && fname [1] == ':' && 
        fname [2] && fname [2] == _RWSTD_PATH_SEP) 
#endif
        return fname;

    std::string pathname;
    std::string dir_other (
        other.substr (0,other.rfind (_RWSTD_PATH_SEP) + 1));

    // OR use the path given through "other", be it relative or absolute
#if !defined (_WIN32)
    if (other.size () && _RWSTD_PATH_SEP == other [0]) {
#else
    if (other.size () && 
        other [1] && other [1] == ':' && 
        other [2] && other [2] == _RWSTD_PATH_SEP) {
#endif
        // other is an absolute path; compose dirname(other) + fname
        (pathname += dir_other) += fname;

    } else {
        pathname.resize (1024);

        // other is a relative path; compose cwd() + dirname(other) + fname
        while (!get_cwd (&pathname [0], pathname.size ())) {
            if (ERANGE != errno) {
                issue_diag (213, true, 0, 
                            "could not retrieve current working directory\n");
                return fname;
            }
            // buffer too small, reallocate
            pathname.resize (pathname.size () * 2);
        }
        
        pathname.resize (std::strlen (pathname.data ()));

        if (pathname.size() && 
            pathname [pathname.size () - 1] != _RWSTD_PATH_SEP)
            pathname += _RWSTD_PATH_SEP;

        (pathname += dir_other) += fname;
    }

#if !defined (_MSC_VER)
    if (S_ISREG (filemode (pathname.c_str ())))
#else
    if (S_IFREG & filemode (pathname.c_str ()))
#endif // !defined (_MSC_VER)
        return pathname;


    // use the value of RWSTD_SRC_ROOT
    const char* const src_root = std::getenv ("RWSTD_SRC_ROOT");
    if (src_root && *src_root) {
        // if the environment variable is not empty, append `fname'
        // to it and see if the file exists and names a regular file
        // if so, return its pathname
        pathname = src_root;
        (((pathname += _RWSTD_PATH_SEP) += "src") += _RWSTD_PATH_SEP) += fname;

#if !defined (_MSC_VER)
                if (S_ISREG (filemode (pathname.c_str ())))
#else
                if (S_IFREG & filemode (pathname.c_str ()))
#endif // !defined (_MSC_VER)
            return pathname;
    }

    // allocate a sufficiently large buffer for the cwd
    pathname.resize (1024);

    // use current working directory
    while (!get_cwd (&pathname [0], pathname.size ())) {

        if (ERANGE != errno) {
            issue_diag (213, true, 0, 
                        "could not retrieve current working directory\n");
            return fname;
        }

        // buffer too small, reallocate
        pathname.resize (pathname.size () * 2);
    }

    // chop off everything past the terminating NUL
    pathname.resize (std::strlen (pathname.data ()));
    pathname += fname;

#if !defined (_MSC_VER)
    if (S_ISREG (filemode (pathname.c_str ())))
#else
    if (S_IFREG & filemode (pathname.c_str ()))
#endif // !defined (_MSC_VER)
        return pathname;

    return fname;
}


int makedir (const char *name)
{
#ifdef _WIN32

   if (   0 == CreateDirectory (name, NULL)
        && GetLastError () != ERROR_ALREADY_EXISTS) {

        LPVOID lpMsgBuf;

        FormatMessage (  FORMAT_MESSAGE_ALLOCATE_BUFFER
                       | FORMAT_MESSAGE_FROM_SYSTEM
                       | FORMAT_MESSAGE_IGNORE_INSERTS,
                       0,
                       GetLastError (),
                       MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPTSTR)&lpMsgBuf,
                       0,
                       0);

        issue_diag (E_CALL, true, 0, (char*)lpMsgBuf);

        return -1;
    }
    
#else   // ifndef _WIN32

   if (-1 == mkdir (name, 0755)) {

#  ifdef EEXIST

       if (errno != EEXIST) {
           issue_diag (E_CALL, true, 0, "failed to create directory %s: %s\n",
                       name, std::strerror (errno));

           return -1;
       }

#  endif   // EEXIST 

   }

#endif   // _WIN32

    return 0;
}
