/***************************************************************************
 *
 * localedef.cpp
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
 * Copyright 2001-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include "aliases.h"
#include "def.h"             // for Def
#include "diagnostic.h"      // for issue_diag()
#include "loc_exception.h"   // for loc_exception
#include "path.h"

#include <fstream>        // for ifstream
#include <iostream>       // for cerr
#include <string>         // for string
#include <vector>         // for vector

#include <clocale>        // for setlocale(), LC_XXX
#include <cstdlib>        // for atoi()
#include <cstring>        // for strcmp()


// exit status
#define EXIT_OK       0
#define EXIT_ERRORS   4


// charmaps holds pointers to the character maps being used to generate
// the locales.
static std::vector<Charmap*> charmaps;


static void
print_help_msg ()
{
    static const char msg[] = 
        "NAME\n"
        "\tlocaledef - generate a locale environment\n"
        "\n"
        "SYNOPSIS\n"
        "\tlocaledef [-c][-w[###]][-f charmap_file][-i locale_defintion]\n"
        "\t\tlocale_name\n"
        "\n"
        "\tlocaledef -g [-m creation_list]\n"
        "\t\t[-d output_dir][-r charmap_dir][-s src_dir][--ucs]\n"
        "\n"
        "DESCRIPTION\n"
        "\tThe localedef utility sets up the language environment for the\n"
        "\tnamed locale. localedef reads a locale definition file from\n"
        "\tstandard input or from locale_definition file and creates a\n"
        "\tlocale database with the name specified on the command line.\n"
        "\n"
        "OPTIONS\n"
        "\tThe localedef utility recognizes the following options:\n"
        "\n"
        "\t-c\tCreate output irrespective of warnings.\n"
        "\n"
        "\t-f charmap_file\n"
        "\t\tIf the locale definition uses symbolic names use charmap_file.\n"
        "\n"
        "\t-i locale_definition\n"
        "\t\tUse locale_definition as input, instead of standard input\n"
        "\t\t(default).\n"
        "\n"
        "\t-g\tGenerate all the locales listed in the creation list.\n"
        "\n"
        "\t--ucs\tUse UCS for the internal (wchar_t) encoding."
        "\n"
        "\n\t-m creation_list\n"
        "\t\tSpecify a file that lists what combinations of charmaps and\n"
        "\t\tsource files should be used to create a locale.\n"
        "\n"
        "\t-r charmap_dir\n"
        "\t\tSpecify the directory where the character maps are located.\n"
        "\n"
        "\t-s src_dir\n"
        "\t\tSpecify the directory where the source files are located.\n"
        "\n"
        "\t-d output_dir\n"
        "\t\tSpecify the directory where the generated locale database\n"
        "\t\tfiles should be placed.\n"
        "\n"
        "\t-w\tDisable all warning messages.\n"
        "\n"
        "\t-w###\n"
        "\t\tDisable warning number ###.\n"
        "\n"
        "\t--notes\tEnable notes (informational messages).\n"
        "\t-?\n"
        "\t--help\tPrint out this message.\n";

    std::cout << msg;
}


static void
create_locale (std::string std_src,
               std::string std_cmap, 
               std::string outdir,
               std::string std_locale, 
               bool force_output, bool use_ucs,
               bool no_position, bool link_aliases)
{
    // extract the names of the locale and of the codeset
    std::string lname (std_src);
    std::string cname (std_cmap);

    if (lname.rfind(_RWSTD_PATH_SEP) != std::string::npos)
        lname = lname.substr(lname.rfind(_RWSTD_PATH_SEP) + 1, lname.size());

    if (cname.rfind(_RWSTD_PATH_SEP) != std::string::npos)
        cname = cname.substr(cname.rfind(_RWSTD_PATH_SEP) + 1, cname.size());

    if (lname.find('.') != std::string::npos)
        lname = lname.substr(0, lname.find('.'));
    if (cname.find('.') != std::string::npos)
        cname = cname.substr(0, cname.find('.'));
    
    // the vector of corresponding C locales
    StringVector C_locales;
#ifndef _WIN32
    get_same_encoding_C_locale (lname, cname, C_locales);
#endif  // _WIN32

    // C library locale using same encoding
    std::string enc_C_locale;

#ifdef _RWSTD_NO_ISO_10646_WCHAR_T
    // the encoding C locale
    enc_C_locale = get_C_encoding_locale (cname);

    // platforms with locale dependant wchar_t encodings need the current 
    // C locale to be set.  If there is no C locale with the same name
    // or that uses the same encoding as the locale we are creating 
    // issue warning
    if (enc_C_locale.empty ()) {
        issue_diag (W_COMPAT, false, 0, "no compatible locale found\n");
    } else
        std::setlocale (LC_ALL, enc_C_locale.c_str ());
        
#endif   // _RWSTD_NO_ISO_10646_WCHAR_T

    // if no charmap is present assume ISO-8859-1
    if (std_cmap.empty ())
        std_cmap = "ISO-8859-1";
        
    // retrieve UTF-8 encoding aliases
    std::string utf8_cname("UTF-8");
    StringVector utf8_aliases;
    get_cname_aliases (utf8_cname, utf8_aliases);

    // is it a UTF-8 encoded locale?
    bool is_utf8 = false;
    StringVector::iterator pos = utf8_aliases.begin();
    for (; pos != utf8_aliases.end (); pos++)
        if (ci_compare (cname, *pos) == 0) {
            is_utf8 = true;
            break;
        }

    // retrieve the charmap/codeset object 
    Charmap* charmap_p = 0;        
    std::vector <Charmap*>::iterator charmaps_it = charmaps.begin();
    for (; charmaps_it != charmaps.end(); charmaps_it++){
        if ((*charmaps_it)->get_full_charmap_name() == std_cmap) {
            charmap_p = *charmaps_it;
            break;
        }
    }

    // if none found, create one and parse the corresponding file
    if (0 == charmap_p) {

        issue_diag (I_STAGE, false, 0,
                    "processing character set description file %s\n",
                    std_cmap.c_str ());

        charmap_p = new Charmap (enc_C_locale.c_str (),
                                 std_cmap.c_str (), 
                                 is_utf8, true, true, use_ucs);
        charmaps.push_back (charmap_p);
    }

    // parse the source definition files
    bool def_error = false;

    issue_diag (I_STAGE, false, 0,
                "processing locale definition file %s\n", std_src.c_str ());

    Def def (std_src.c_str (), (outdir + std_locale).c_str (),
             *charmap_p, no_position);

    try {
        // try to parse the input files
        def.process_input ();
    }
    catch (...) {
        def_error = true;
    }

    // create the locale directory
    std::string locale_dir (outdir + std_locale);

    makedir (locale_dir.c_str ());

    if (def_error) {
        // write out the codecvt database and exit if parsing failed
        def.write_codecvt (locale_dir);
        throw loc_exception ("abort.");
    }

    // no output when it hasn't been forced and warnings were present
    if (!force_output && def.warnings_occurred_) {
        std::cerr << "Warnings occurred - No output produced\n";
        return;
    }

    // and write out the locale categories data
    issue_diag (I_STAGE, false, 0, "generating LC_CTYPE database\n");
    def.write_ctype (locale_dir);

    issue_diag (I_STAGE, false, 0, "generating codeset database\n");
    def.write_codecvt (locale_dir);

    issue_diag (I_STAGE, false, 0, "generating LC_MONETARY database\n");
    def.write_monetary (locale_dir);

    issue_diag (I_STAGE, false, 0, "generating LC_NUMERIC database\n");
    def.write_numeric (locale_dir);

    issue_diag (I_STAGE, false, 0, "generating LC_TIME database\n");
    def.write_time (locale_dir);

    issue_diag (I_STAGE, false, 0, "generating LC_COLLATE database\n");
    def.write_collate (locale_dir);

#ifndef _WIN32

    issue_diag (I_STAGE, false, 0, "generating LC_MESSAGES database\n");
    def.write_messages (locale_dir);

#endif  // _WIN32

    // no C library locales equivalents 
    if (C_locales.empty ())
        return;

#if !defined (_WIN32) && !defined (__CYGWIN__)

    if (link_aliases == false)
        return;

    // some corresponding C lib locale names where found for this name
    StringVector::iterator it = C_locales.begin ();
    for (; it != C_locales.end (); it++) {
        // check if the name actually exists
        if (*it == std_locale)
            continue;

        // set a symlink with the name of the C lib locale
        // pointing to our locale database
        create_symlink (outdir, std_locale, *it);
    }
#endif  // !_WIN32 && !__CYGWIN__
}


static void
generate_locales (const char* map_name, const char* /*alias_name*/,
                  const char* charmap_dir, const char* src_dir,
                  const char* output_dir, bool use_ucs, 
                  bool no_position, bool link_aliases)
{
    std::ifstream f (map_name);

    if (!f) {
        issue_diag (E_OPENRD, true, 0, 
                    "the generation list '%s' "
                    "could not be opened\n", map_name);
        return;
    }

    while (f) {
        std::string s1;
        std::string s2;

        f >> s1 >> s2;
        if (f) {
            std::string lname = s1.substr (0, s1.find('.'));
            std::string cname = s2.substr (0, s2.find('.'));

            // our name for a locale database is <locale>.<codeset>
            std::string std_locale(lname + "." + cname);

            // create the locale database
            std::cout << "creating locale " << std_locale << '\n';
            try { 
                create_locale ((std::string(src_dir) + s1),
                               (std::string(charmap_dir) + s2), 
                               std::string (output_dir), std_locale,
                               true, use_ucs, no_position, link_aliases);

            } 
            catch (const std::ios::failure& error) {
                std::cerr << "I/O exception " << error.what() << '\n';
            } 
            catch (loc_exception& e) {
                std::cerr << "Unable to create locale " << std_locale
                          << " : " << e.what () << '\n';
            } 
            catch (const std::exception& error) {
                std::cerr <<"ERROR: " << error.what() << '\n';
            }
            catch (...) {
                std::cerr << "Unable to create locale " << std_locale 
                          << '\n';
            }
        }
    } 
}


struct ProgramOptions
{
    const char* program_name;
    const char* charmap_name;
    const char* source_name;
    const char* map_file;
    const char* alias_file;
    const char* locale_name;

    std::string charmap_dir;
    std::string src_dir;
    std::string output_dir;
    
    bool gen;
    bool force_output;
    bool use_ucs;
    bool no_position;
    bool link_aliases;
};


static bool
process_command_line (ProgramOptions *opts, int argc, char* argv[])
{
    opts->program_name = argv [0];
    opts->charmap_name = "";
    opts->source_name  = "";
    opts->map_file     = 0;
    opts->alias_file   = 0;
    opts->locale_name  = 0;

    opts->gen          = false;
    opts->force_output = false;
    opts->use_ucs      = false;
    opts->no_position  = false;
    opts->link_aliases = false;

    int i;   // index of command line argument being processed

    for (i = 1; i < argc && '-' == argv [i][0]; ++i) {

        switch (argv [i][1]) {

        case 'f':   // set character set description file name
            if (argv [i + 1])
                opts->charmap_name = argv [++i];
            break;

        case 'i':   // set locale definition file name
            if (argv [i + 1])
                opts->source_name = argv [++i];
            break;

        case 'c':   // create output even if warnings are issued
            opts->force_output = true;
            break;

        case 'g':   // generate more than one locale database
            opts->gen = true;
            break;

        case 'm':
            if (argv [i + 1])
                opts->map_file = argv [++i];
            break;

        case 'a':
            if (argv [i + 1])
                opts->alias_file = argv [++i];
            break;

        case 'r':
            if (argv [i + 1])
                opts->charmap_dir = argv [++i];
            break;

        case 's':
            if (argv [i + 1])
                opts->src_dir = argv [++i];
            break;

        case 'd':
            if (argv [i + 1])
                opts->output_dir = argv [++i];
            break;

        case 'w':   // disable one or all warnings
            if (argv [i][2])
                issue_diag (std::atoi (argv [i] + 2), false, 0, 0);
            else
                issue_diag (W_DISABLE, false, 0, 0);
            break;

        case '?':
            print_help_msg ();
            return false;

        case '-':
            if (0 == std::strcmp (argv [i] + 2, "help")) {
                print_help_msg ();
                return false;
            }

            if (0 == std::strcmp (argv [i] + 2, "ucs")) {
                // --ucs: use UCS as the internal wchar_t encoding
                opts->use_ucs = true;
                break;
            }
            else if (0 == std::strcmp (argv [i] + 2, "no_position")) {
                opts->no_position = true;
                break;
            }
            else if (0 == std::strcmp (argv [i] + 2, "aliases")) {
                opts->link_aliases = true;
                break;
            }
            else if (0 == std::strcmp (argv [i] + 2, "notes")) {
                // --notes: enable informational messages (notes)
                issue_diag (I_ENABLE, false, 0, 0);
                break;
            }
            // fall through

        default:
            issue_diag (E_CMDARG, true, 0, "invalid option %s\n", argv [i]);
        }
    }

    if (opts->gen) {

        bool errors = false;

        // make sure that all the required options are specified
        if (0 == opts->map_file) {
            issue_diag (E_NOARG, true, 0, 
                        "option %s requires a string argument\n", "-m");
        }

        if (opts->charmap_dir.empty ()) {
            issue_diag (E_NOARG, true, 0, 
                        "option %s requires a string argument\n", "-r");
        }

        if (opts->src_dir.empty ()) {
            issue_diag (E_NOARG, true, 0, 
                        "option %s requires a string argument\n", "-s");
        }

        if (opts->output_dir.empty ()) {
            issue_diag (E_NOARG, true, 0, 
                        "option %s requires a string argument\n", "-d");
        }

        // append a slash to the directories if the user didn't
        if (opts->output_dir [opts->output_dir.size () - 1] != _RWSTD_PATH_SEP)
            opts->output_dir += _RWSTD_PATH_SEP;

        if (opts->src_dir [opts->src_dir.size () - 1] != _RWSTD_PATH_SEP)
            opts->src_dir += _RWSTD_PATH_SEP;

        if (opts->charmap_dir [opts->charmap_dir.size () - 1] != _RWSTD_PATH_SEP)
            opts->charmap_dir += _RWSTD_PATH_SEP;
    }

    if (0 == argv [i] && !opts->gen) {
        issue_diag (E_NOARG, true, 0, "missing command line argument\n");
    }

    opts->locale_name = argv [i];

    if (argv [i + 1]) {
        issue_diag (E_XARG, true, 0,
                    "extra command line arguments after %s\n", argv [i]);
    }

    return true;
}


int localedef_main (int argc, char *argv[])
{
    ProgramOptions opts;

    if (!process_command_line (&opts, argc, argv))
        return EXIT_OK;

    if (opts.gen) {
        // create the locale databases as specified in generation list
        generate_locales (opts.map_file, opts.alias_file,
                          opts.charmap_dir.c_str (),
                          opts.src_dir.c_str (),
                          opts.output_dir.c_str (),
                          opts.use_ucs,
                          opts.no_position,
                          opts.link_aliases);
    }
    else {
        assert (0 != opts.locale_name);

        // C++ locale name requested
        std::string std_locale (opts.locale_name);

        // retrieve the output directory if any
        std::string std_outdir ("");

        if (std_locale.rfind (_RWSTD_PATH_SEP) != std::string::npos) {
            std_outdir = 
                std_locale.substr (
                    0, std_locale.rfind (_RWSTD_PATH_SEP) + 1);

            std_locale = 
                std_locale.substr (std_locale.rfind (_RWSTD_PATH_SEP) + 1,
                                   std_locale.size ());
        }

        // create the locale database
        create_locale (opts.source_name, opts.charmap_name,
                       std_outdir, std_locale,
                       opts.force_output, opts.use_ucs,
                       opts.no_position, opts.link_aliases);
    }

    return EXIT_OK;
}


// defined in locale.cpp
int locale_main (int, char*[]);


int main (int argc, char *argv[])
{
    int exit_status = EXIT_OK;

    try {

        if (1 < argc && 0 == std::strcmp (argv [1], "--locale-mode")) {

            // invoked with the special option ""--locale-mode"
            // to act like the locale utility

            // remove argv [1] from command line
            for (int i = 2; argv [i]; ++i)
                argv [i - 1] = argv [i];

            // NULL-terminate argv
            argv [argc - 1] = 0;

            exit_status = locale_main (argc - 1, argv);
        }
        else {
            exit_status = localedef_main (argc, argv);
        }
    }
    catch (const std::ios::failure& error) {
        std::cerr << "Error: I/O exception: " << error.what () << '\n';
        exit_status = EXIT_ERRORS;
    } 
    catch (loc_exception&) {
        exit_status = EXIT_ERRORS;
    } 
    catch (const std::exception& error) {
        std::cerr <<"Error: " << error.what () << '\n';
        exit_status = EXIT_ERRORS;
    }
    catch (...) {
        std::cerr << "Error: unknown exception\n";
        exit_status = EXIT_ERRORS;
    }

    return exit_status;
}
