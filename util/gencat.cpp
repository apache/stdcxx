/***************************************************************************
 *
 * gencat.cpp - Utility for generating message catalogs on Windows
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

#include <cstdlib>   // for system(), getenv()
#include <cstdio>    // for printf()
#include <cstring>   // for strcmp(), strrchr()
#include <cstddef>   // for size_t
#include <cassert>   // for assert()

#include <string>

static const char
usage_text[] = {
    "Usage: %s OUTPUT-FILE INPUT-FILE\n"
    "Generate message catalog.\n"
    "\n"
    "  -?, --help                 Give this help list\n"
};

#ifdef _WIN32
#  define SLASH '\\'

// replace file extension in str by new extension ext
static void change_ext (std::string& str, const char* ext)
{
    const std::string::size_type npos = std::string::npos;
    std::string::size_type dot_pos = str.find_last_of ('.');
    std::string::size_type quote_pos =
        npos == dot_pos ? str.find_last_of ('\"')
        : str.find_first_of ('\"', dot_pos);
    if (npos == quote_pos)
        quote_pos = str.size ();
    if (npos == dot_pos)
        dot_pos = quote_pos;
    str.replace (dot_pos, quote_pos - dot_pos, ext);
}

#else    // !_WIN32
#  define SLASH '/'
#endif   // _WIN32

int main (int argc, char *argv[])
{
    const char* exe_name = "gencat";

    if (argv [0]) {
        if (const char* slash = std::strrchr (argv [0], SLASH))
            exe_name = slash + 1;
        else
            exe_name = argv [0];
    }

    assert (exe_name);

    if (1 == argc) {
        std::printf (usage_text, exe_name);
        return 0;
    }

    --argc;

    while (0 != *++argv && 0 < argc-- && '-' == **argv) {

        switch (*++*argv) {

        case '?':
            std::printf (usage_text, exe_name);
            return 0;

        case '-':
            if (0 == std::strcmp (*argv, "-help")) {
                std::printf (usage_text, exe_name);
                return 0;
            }

            // fall through...
        default:
            std::printf ("%s: invalid option -%s\n",
                         exe_name, *argv);
            return 1;
        }
    }

    if (1 > argc) {
        std::printf ("%s: missing arguments\n Try '%s --help'\n",
                     exe_name, exe_name);
        return 1;
    }

    std::string cmd;

#ifdef _WIN32

#  ifndef __MINGW32__

    const char* const env_vars [] = {
        "VS90COMNTOOLS", "VS80COMNTOOLS",
        "VS71COMNTOOLS", "VSCOMNTOOLS"
    };

    for (size_t i = 0; i < sizeof (env_vars) / sizeof (*env_vars); ++i) {
        if (const char* vcvarspath = std::getenv (env_vars [i])) {
            cmd = vcvarspath;
            cmd += "vsvars32.bat";
            break;
        }
    }

    if (std::string::npos != cmd.find (' ')) {
        cmd.insert (cmd.begin (), 1, '\"');
        cmd.push_back ('\"');
    }

    if (!cmd.empty ())
        cmd += " && ";

#  endif   // !__MINGW32__

    const char* const dll_name = argv [0];
    const char* const rc_name = argv [1];

    std::string res_name (rc_name);
    change_ext (res_name, ".res");

#  ifdef __MINGW32__

    cmd += "windres -O coff -i ";
    cmd += rc_name;
    cmd += " -o ";
    cmd += res_name;
    cmd += " && gcc -shared -mno-cygwin -o ";
    cmd += dll_name;
    cmd += ' ';
    cmd += res_name;

#  else

#    ifndef _WIN64
#      define PLATFORM "X86"
#    else   // _WIN64
#      define PLATFORM "X64"
#    endif  // _WIN64

    cmd += "rc ";
    cmd += rc_name;
    cmd += " && link /NOLOGO /DLL /NOENTRY /MACHINE:" PLATFORM " /OUT:";
    cmd += dll_name;
    cmd += ' ';
    cmd += res_name;
    
#  endif   // __MINGW32__

    const int ret = std::system (cmd.c_str ());

    std::remove (res_name.c_str ());

#else    // !_WIN32

    const char* const cat_name = argv [0];
    const char* const msg_name = argv [1];

    cmd = "/usr/bin/gencat ";
    cmd += cat_name;
    cmd += ' ';
    cmd += msg_name;

    const int ret = std::system (cmd.c_str ());

#endif   // _WIN32

    return ret;
}
