#!/usr/bin/awk -f
#
# $Id$
#
########################################################################
#
#  Licensed to the Apache Software  Foundation (ASF) under one or more
#  contributor  license agreements.  See  the NOTICE  file distributed
#  with  this  work  for  additional information  regarding  copyright
#  ownership.   The ASF  licenses this  file to  you under  the Apache
#  License, Version  2.0 (the  "License"); you may  not use  this file
#  except in  compliance with the License.   You may obtain  a copy of
#  the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the  License is distributed on an  "AS IS" BASIS,
#  WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
#  implied.   See  the License  for  the  specific language  governing
#  permissions and limitations under the License.
#
########################################################################
#
# SYNOPSIS
#     myname [bodyonly=0|1 logdir=<logdir> version=<version>] logs...
#
# VARIABLES:
#   bodyonly     when non-zero, suppresses the <html> tags
#   logdir
#   version      stdcxx version (branch) to generate results for.
#
########################################################################


BEGIN {
    # array of component counts for each section and component
    # compcnts [section, compname]

    # array of lists of component names for each section with
    # each list maintaining the same order as in the logs
    # sectcomponents [section]

    # map of [FILENAME,components] to component status
    # compstatuses [FILENAME, compcnts]

    sectnames [1] = "locale"
    sectnames [2] = "test"
    sectnames [3] = "example"

    # names of build stages
    buildstages [1] = "config"
    buildstages [2] = "lib"
    buildstages [3] = "examples"
    buildstages [4] = "bin"
    buildstages [5] = "tests"
    buildstages [6] = "runall"
    buildstages [7] = "total"

    # displayed and cout as failures
    states ["ASSERT"] =  1
    states ["BUILD"]  =  1
    states ["COMP"]   =  1
    states ["DIFF"]   =  1
    states ["EXEC"]   =  1
    states ["FORMAT"] =  1
    states ["EXIT"]   =  1
    states ["LINK"]   =  1
    states ["SIGNAL"] =  1

    # displayed but don't cout as failures
    states ["OUTPUT"] = -1
    states ["WARN"]   = -1

    # expected failures are displayed but not counted as failures
    states ["XASSERT"] = -1
    states ["XBUILD"]  = -1
    states ["XCOMP"]   = -1
    states ["XDIFF"]   = -1
    states ["XEXEC"]   = -1
    states ["XFORMAT"] = -1
    states ["XEXIT"]   = -1
    states ["XLINK"]   = -1
    states ["XPASS"]   = -1
    states ["XSIGNAL"] = -1

    # not displayed and doesn't cout as a failure
    states ["MISS"] = 0
    states ["OK"]   = 0

    # add all states/classes that count as failures to failclasses
    for (e in states) {
        if (1 == states [e])
            failclasses [e]
    }

    # month numbers used to format dates
    monthnames ["Jan"] = 1
    monthnames ["Feb"] = 2
    monthnames ["Mar"] = 3
    monthnames ["Apr"] = 4
    monthnames ["May"] = 5
    monthnames ["Jun"] = 6
    monthnames ["Jul"] = 7
    monthnames ["Aug"] = 8
    monthnames ["Sep"] = 9
    monthnames ["Oct"] = 10
    monthnames ["Nov"] = 11
    monthnames ["Dec"] = 12

    # mapping from build type to the more descriptive build mode
    buildmodes ["8a"]  = "shared archive, optimized"
    buildmodes ["8A"]  = "shared archive, optimized, wide"
    buildmodes ["8d"]  = "shared, optimized"
    buildmodes ["8D"]  = "shared, optimized, wide"
    buildmodes ["8s"]  = "archive, optimized"
    buildmodes ["8S"]  = "archive, optimized, wide"
    buildmodes ["11a"] = "shared archive, debug"
    buildmodes ["11A"] = "shared archive, debug, wide"
    buildmodes ["11d"] = "shared, debug"
    buildmodes ["11D"] = "shared, debug, wide"
    buildmodes ["11s"] = "archive, debug"
    buildmodes ["11S"] = "archive, debug, wide"
    buildmodes ["12a"] = "shared archive, optimized, reentrant"
    buildmodes ["12A"] = "shared archive, optimized, reentrant, wide"
    buildmodes ["12d"] = "shared, optimized, reentrant"
    buildmodes ["12D"] = "shared, optimized, reentrant, wide"
    buildmodes ["12s"] = "archive, optimized, reentrant"
    buildmodes ["12S"] = "archive, optimized, reentrant, wide"
    buildmodes ["15a"] = "shared archive, debug, reentrant"
    buildmodes ["15A"] = "shared archive, debug, reentrant, wide"
    buildmodes ["15d"] = "shared, debug, reentrant"
    buildmodes ["15D"] = "shared, debug, reentrant, wide"
    buildmodes ["15s"] = "archive, debug, reentrant"
    buildmodes ["15S"] = "archive, debug, reentrant, wide"

    # regular expression to match a name (e.g., compiler or OS)
    re_name    = "[A-Za-z][A-Za-z_0-9]*"

    # regular expression to match a version string (includes 0)
    re_version = "[0-9]+(\\.[0-9]+)*"

    # regular expression matching the buildtype string
    re_buildtype = "(8|11|12|15)[aAdDsS](-(solaris|win32))?"

    # regular expresssion matching a log file name
    re_logname = re_name "-" re_version    \
        "-" re_name "-" re_name "-" re_version \
        "-" re_buildtype "-" "[1-9][0-9]*-log"

    # get today's date
    cmd = "LC_ALL=C date"
    cmd | getline todays_date
    close(cmd)

    # set the conversion format to two decimal places
    CONVFMT = "%.3g"

    # field separator character (not an awk variable)
    FSEP = "|"

}   # BEGIN

########################################################################

# detect the type of file
1 == FNR {

    if (svnpath == "") {

        # initialize svnpath using version (assume trunk by default)
        svnpath = "http://svn.apache.org/viewvc/stdcxx"

        if (version == "" || version == "trunk")
            svnpath = svnpath "/trunk"
        else
            svnpath = svnpath "/branches/" version
    }

    section = 0

    if (0 == match(FILENAME, re_logname)) {
        # treat files whose names don't match the log file name pattern
        # as lists of expected failures
        expect_file = 1
    }
    else {
        expect_file = 0

        logsections [FILENAME] = 0

        logcompcnts [1, FILENAME] = 0
        logcompcnts [2, FILENAME] = 0
        logcompcnts [3, FILENAME] = 0

        ++logcount

        # append the current log file name to the list
        logfnames [logcount] = FILENAME
    }
}


# process file containing specification of expected failures
1 == expect_file && $1 ~ "^[^#][^#]" {

    if (1 == index($0, " ")) {
        # component specification continued from previous line
        cont = 1
    }
    else {
        compname = $1
        cont     = 0
    }

    pattern = $(2 - cont)
    status  = $(3 - cont)
    comment = $(4 - cont)

    # convert shell globbing pattern to a regular expression by
    # escaping periods and replacing astersisks with ".*" and
    # question marks with "."
    gsub("\\.", "\\.", pattern)
    gsub("\\*", ".*", pattern)
    gsub("\\?", ".", pattern)

    # if patter contains an open brace expand it using the shell
    if (0 < index(pattern, "{")) {
        cmd = "echo " pattern
        cmd | getline pattern
        close(cmd)
    }

    # split the (potentially expanded) pattern into an array
    n = split(pattern, patlist, " ")

    spec = ""
    sep  = ""

    # concatenate the array of expanded patters, the status,
    # and the comment, to form a single specification
    for (i = 1; i <= n; ++i) {

        spec = spec sep patlist [i] " " status " " comment

        sep = ":"
    }

    # insert the specification to the expected_statuses array
    if (compname in nexpected_statuses)
        spec = expected_statuses [compname] ":" spec

    expected_statuses [compname] = spec

    # skip the actions below
    next
}


########################################################################
# logfile only processing below

# extract operating system name and its version, and hardware
# architecture (if possible)
/^ *#+ *uname / {

    getline

    uname_output = $0

    osname   = $1
    osver    = ""
    osdesc   = uname_output
    arch     = ""
    archdesc = ""

    if (osname == "AIX") {
        # AIX <host> 2 5 00CBEEBE4C00
        osver = $4 "." $3
        arch  = "PowerPC"
    }
    else if (osname ~ "^CYGWIN") {
        # CYGWIN_NT-5.1 <host> 1.5.24(0.156/4/2) 2007-01-31 10:57 i686 Cygwin
        osname = "Cygwin"
        osver  = substr($3, 1, index($3, "(") - 1)

        pos = match(uname_output, " i[2-6]86 ")
        if (0 < pos) {
            arch = substr(uname_output, pos + 1, 4)
        }
        else if (uname_output ~ " x86 ") {
            arch = "x86"
        }
    }
    else if (osname == "FreeBSD") {
        # 
        osver = $3
    }
    else if (osname == "HP-UX") {
        # HP-UX <host> B.11.31 U ia64 3417177861 unlimited-user license
        # HP-UX <host> B.11.23 U 9000/800 3952255646 unlimited-user license
        # HP-UX <host> B.11.11 U 9000/800 1936254444 unlimited-user license
        osver = $3
        arch  = $5
    }
    else if (osname == "IRIX64") {
        # IRIX64 <host> 6.5 04101930 IP27
        osver = $3
        arch  = "MIPS"
    }
    else if (osname == "Linux")
        # Linux <host> <ver> #1 SMP <builddate> x86_64 x86_64 x86_64 GNU/Linux
        osver = ""
    else if (osname == "SunOS") {
        # SunOS <host> <ver> Generic_118855-33 i86pc i386 i86pc
        # SunOS <host> <ver> Generic_118833-33 sun4u sparc SUNW,Sun-Fire-V215
        # SunOS <host> <ver> Generic_117350-43 sun4u sparc SUNW,Sun-Fire-V240
        osver = $3
        if (0 < index($0, "sparc"))
            arch = "SPARC"
        else
            arch = "x86"
    }
    else if (osname == "OSF1") {
        # OSF1 <host.domain> V5.1 1885 alpha
        osver = $3
    }

    logos [FILENAME] = osname " " osver FSEP osdesc FSEP arch FSEP archdesc
}


# extract Linux distro name and version
/^ *#+ *cat  *\/etc\/.*-release/ {

    getline

    if (1 == match($1, "^LSB_VERSION"))
        getline

    osname   = ""
    osver    = ""
    osdesc   = $0
    arch     = ""
    archdesc = ""

    # Red Hat Enterprise Linux Server release 5 (Tikanga)
    # Red Hat Enterprise Linux AS release 4 (Nahant Update 4)
    # Red Hat Enterprise Linux AS release 4 (Nahant Update 2)
    # Red Hat Enterprise Linux AS release 3 (Taroon Update 8)

    # SUSE Linux Enterprise Server 10 (x86_64)
    # SUSE LINUX Enterprise Server 9 (x86_64)

    # remove the Red Hat code name including the release
    # and keep the distribution code name and update info:
    #   RHEL 5:      Tikanga (Update 1 through 2)
    #   RHEL 4:      Nahant (Update 1 through 6)
    #   RHEL 3:      Taroon (Update 1 through 9)
    #   RHEL 2.1 AS: Pensacola
    #   RHEL 2.1 ES: Panama

    if ("RedHat" == $1 $2) {

        if ("EnterpriseLinux" == $3 $4) {
            if ("AS" == $5)
                osname = "RHAS"
            else if ("ES" == $5)
                osname = "RHES"
            else if ("WS" == $5)
                osname = "RHWS"
            else
                osname = "RHEL"
        }
        else
            osname = "RHL"

        match($0, "release " re_version)
        if (0 < RSTART)
            osver = substr($0, RSTART + 8, RLENGTH - 8)

        match($0, "Update [1-9][0-9]*")
        if (0 < RSTART)
            osver = osver "." substr($0, RSTART + 7, RLENGTH - 7)
        else
            osver = osver ".0"
    }
    else if ($1 == "SUSE") {
        osname = "SLES"
        osver  = $5
    }

    logos [FILENAME] = osname " " osver FSEP osdesc FSEP arch FSEP archdesc
}


# extract x86 processor name and version
/^ *#+ *cat  *\/proc\/cpuinfo/ {

    arch     = ""
    archdesc = ""

    # look for CPU manufacturer and other goodies in the contents
    # of /proc/cpuinfo
    do {
        getline

        if ($1 $2 $3 == "modelname:") {

            $1 = ""   # model
            $2 = ""   # name
            $3 = ""   # :

            archdesc = $0
        }

        if ($1 $2 == "arch:") {

            $1 = ""   # arch
            $2 = ""   # :

            arch = $0
        }

        if ($1 $2 == "family:") {

            $1 = ""   # family
            $2 = ""   # :

            if (arch == "")
                arch = "IA64"

            archdesc = $0
        }

        if (arch != "" && archdesc != "")
            break

    } while (0 < NF)

    # bail if no architecture could be determined
    if (arch == "" && archdesc == "")
        next

    # strip leading whitespace (result of assigning $N = "")
    archdesc = substr(archdesc, match(archdesc, "[A-Za-z]"))

    n = split(logos [FILENAME], platfields, FSEP)
    osname   = platfields [1]
    osdesc   = platfields [2]
    # arch     = platfields [3]
    # archdesc = platfields [4]

    if (uname_output ~ " x86_64 ") {
        if (archdesc ~ "Intel\\(R\\)")
            arch = "EM64T"
        else if (archdesc ~ "AMD ")
            arch = "AMD64"
        else
            arch = "x86_64"
    }
    else if (uname_output ~ " ia64 ") {
        if (arch == "")
            arch = "IA64"
    }
    else if (arch == "") {
        pos = match(uname_output, " i[3456]86 ")
        if (0 < pos)
            arch = substr(uname_output, pos + 1, 4)
        else if (uname_output ~ " x86 ")
            arch = "x86"
    }

    logos [FILENAME] = osname FSEP osdesc FSEP arch FSEP archdesc
}


# extract processor architecture info on Windows
/^PROCESSOR_IDENTIFIER=/ {

    n = split(logos [FILENAME], platfields, FSEP)
    osname   = platfields [1]
    osdesc   = platfields [2]
    arch     = platfields [3]
    archdesc = substr($0, index($0, "=") + 1)

    if ("" == arch)
        arch = substr(archdesc, 1, index(archdesc, " ") - 1)

    logos [FILENAME] = osname FSEP osdesc FSEP arch FSEP archdesc
}


# extract the (POSIX) build date and time (date output)
/^ *#+ *date *: *$/ {

    getline

    logdates [FILENAME] = $0
}


# extract the Windows build date (date /T output)
/>date  *\/T *$/ {

    getline

    logdates [FILENAME] = $0
}


# extract compiler name and version
/^configuring stdcxx / {

    cxxdesc = $5

    pos = index(cxxdesc, "-")
    if (0 < pos) {
        cxxname = substr(cxxdesc, 1, pos - 1)
        cxxver  = substr(cxxdesc, pos + 1)
    }
    else
        cxxname = cxxdesc

    if (cxxname == "aCC")
        cxxname = "HP aCC"
    else if (cxxname == "CC" && uname_output ~ "^IRIX64")
        cxxname = "SGI MIPSpro"
    else if (cxxname == "cxx" && uname_output ~ "^OSF1")
        cxxname = "HP C++"
    else if (cxxname == "eccp")
        cxxname = "EDG eccp"
    else if (cxxname == "icc")
        cxxname = "Intel C++"
    else if (cxxname ~ "^xlC")
        cxxname = "IBM XLC++"

    logos [FILENAME] = logos [FILENAME] FSEP cxxname " " cxxver FSEP cxxdesc
}


# extract compiler name and version (Visual Studio build)
/^Configuring for / {

    cxxdesc = $3

    pos = index(cxxdesc, "-")
    if (0 < pos) {
        cxxname = substr(cxxdesc, 1, pos - 1)
        cxxver  = substr(cxxdesc, pos + 1)
    }
    else
        cxxname = cxxdesc

    if (cxxname == "msvc")
        cxxname = "MSVC"
    else if (cxxname = "icc")
        cxxname = "Intel C++"

    logos [FILENAME] = logos [FILENAME] FSEP cxxname " " cxxver FSEP cxxdesc
}


# extract compiler name and version (Visual Studio build)
/^Selected compiler: / {
    $1 = ""
    $2 = ""
    cxxdesc = $0

    if (cxxdesc ~ "Intel.*C++") {
        cxxname = "Intel C++"
        cxxver  = $5
    }
    else {
        cxxname = $1 " " $2
        cxxver  = $3
    }

    logos [FILENAME] = logos [FILENAME] FSEP cxxname " " cxxver FSEP cxxdesc
}


# see if the library failed to configure or build (UNIX)
/^g?make: \*\*\* \[(config|lib)\] Error/ {
    buildstatus [FILENAME] = "LIB"
}


# see if the library failed to configure or build (Windows)
/.stdcxx - [1-9][0-9]* error(s), [1-9][0-9]* warning(s)/ {
    buildstatus [FILENAME] = "LIB"
}


# extract the real, user and system times for the children of the shell
# that executed the commands from the log
# the format of the output is:
#   <real-time>
#   <times-output>
# with <real-time> looking like:
#   [1-9][0-9]*m[1-9][0-9]s
# and with <times-output> being the output of the POSIX standard times
# built-in utility, i.e., the first line giving the system and user times
# for the shell and the second line giving the system and user times for
# its children in the format:
#   "%dm%fs %dm%fs"
# we don't care about the shell times, just the times for its children,
# so we skip that line
/^ *#+ real, user, system time \(/ {

    stage = substr($6, 2, index($6, ")") - 2);

    getline      # real time
    times = $0
    getline      # ignore shell times
    getline      # times for all children

    pos   = index($0, " ")
    times = times FSEP substr($0, 1, pos - 1)
    times = times FSEP substr($0, pos + 1)

    # times is: <real> FSEP <usr> FSEP <sys>
    logstagetimes [FILENAME, stage] = times
}


# extract the size of the library
/^ *#+ ls -l/ {

    while (0 < NF) {
        size = $5
        getline
    }

    libsizes [FILENAME] = size

}


# extract the full build size or the size after the clean target
/^ *#+ *du -sk / {

    getline

    if (FILENAME in buildsizes)
        cleansizes [FILENAME] = $1
    else
        buildsizes [FILENAME] = $1
}


# count the number of (make) errors
/^g?make: \*\*\* \[.*\] Error /{

    ++logerrors [FILENAME]
}


# count the number of (Visual Studio) errors
/ - [1-9][0-9]* error\(s\), [1-9][0-9]* warning\(s\)/{

    if (cxxname ~ "^MSVC")
        ++logerrors [FILENAME]
}


# count the number of warnings
/[^A-Z_0-9](WARNING|[Ww]arning)[^A-Z_0-9]/ {

    # if the library hasn't been seen yet count this as a library warning
    if (!(FILENAME in libsizes))
        ++libwarnings [FILENAME]
    else
        ++logwarnings [FILENAME]
}


# action at the beginning of component table
/^NAME  *STATUS WARN/ {
    start = 1

    ++section

    ++logsections [FILENAME]

    logcompcnts [section, FILENAME] = 0

    # skip this record
    next
}


# end of component table
/^PROGRAM SUMMARY:$/ {

    start = 0
}

0 == start {
    # skip this record
    next
}


# component (locale, example, or test) section
1 == start {

    # store the name of the component
    compname = $1

    # trim suffix from component name
    sub("(\\.bat|\\.exe|\\.sh)$", "", compname)

    # store the component status
    compstatus = $2

    # the default expected status is empty (no record for it exists)
    expect  = ""
    comment = ""

    # append the component name to the list of component names
    # for the current section
    if (0 == ((section, compname) in compcnts))
        sectcomponents [section] = sectcomponents [section] " " compname

    # increment the number of times the current component occurs
    # in the current section across all logs
    ++compcnts [section, compname]

    # increment the number of components in the current section
    # and log
    ++logcompcnts [section, FILENAME]

    # if appropriate, increase the maximum number of components
    # across all logs
    if (sectmaxcompcnts [section] < logcompcnts [section, FILENAME])
        sectmaxcompcnts [section] = logcompcnts [section, FILENAME]

    # look up the component's name in the array of components each
    # with a record of the platform(s) and the expecred status of
    # the component on the platform(s), something like
    #    "aix-.*-ppc-.*-.*-12{d,D} SEGV STDCXX-123"
    #   ":linux.*-.*-gcc           ABRT STDCXX-345"
    #     ^                        ^    ^
    #     |                        |    |
    #     |                        |    +-- required comment (issue)
    #     |                        +-- expected component status
    #     +-- platform matching ERE pattern

    if (compname in expected_statuses)
        expspec = expected_statuses [compname]
    else
        expspec = ""    

    # split the list of platforms/expected statuses into an array
    # with one element for each status and a pattern matching one
    # or more platforms
    n = split(expspec, slist, ":")

    for (i = 1; i <= n; ++i) {

        # get the platform matching pattern, expected status, and
        # the required comment
        split(slist [i], fields, " ")

        # try to match the log file name against the specified pattern
        if (match(FILENAME, fields [1])) {
            # extract the expected component status and the comment
            expect  = fields [2] 
            comment = fields [3]

            break
        }
    }

    # class (determins the color of the cell), value (the text displayed
    # in the cell), and the optional detail for the cell
    class  = ""
    value  = ""
    detail = ""

    # compstatus:
    # <class>:[<value>]:[<detail>]

    if (0 == compstatus) {
        # successful exit status: component built but may have failed
        # assertions or runtime warnings

        class = "OK"

        if (3 != section) {
            # locales and tests but not examples
            runwarns = $3   # number of runtime warnings
            asserts  = $4   # number of assertions tried
            failed   = $5   # number of failed assertions

            if (0 < failed) {
                # non-zero number of assertions
                class  = "ASSERT"
                value  = failed
                detail = asserts
            }
            else if (0 < runwarns) {
                # non-zero runtime warnings
                class = "RUNWARN"
                value = "(" runwarns ")"
            }
        }

        if (expect != "") {
            if ("OK" == class && expect != class) {
                # unexpected success
                class  = "XPASS"
                value  = "XPASS<br>(X" expect ")"
                detail = "unexpected success"
            }
            else if ("OK" != class) {
                # expected status doesn't match
                ++xmatchcounts [section, FILENAME]

                value = value "<br>" expect
            }
        }
    }
    else if (0 < compstatus && compstatus < 256) {
        # non-zero exit status
        class = "EXIT"
        value = compstatus

        if (expect != "") {
            if (expect == value) {
                # actual exit status matches the expected status
                ++xfailcounts [section, FILENAME]

                class = "X" class
            }
            else {
                ++xmatchcounts [section, FILENAME]

                value  = value "<br>(X" expect ")"
                detail = "expected failure doesn't match actual status"
            }
        }
    }
    else if (   "COMP" == compstatus       \
             || "DIFF" == compstatus       \
             || "FORMAT" == compstatus     \
             || "LINK" ==  compstatus      \
             || "NOUT" == compstatus       \
             || "OUTPUT" == compstatus) {
        # DIFF     unexpected (example) output
        # FORMAT   unexpected (test) output format
        # NOUT     no (test) output
        # OUTPUT   no (example) master output file
        class = compstatus

        if (expect != "") {
            if (expect == class) {
                # actual status matches expected status
                ++xfailcounts [section, FILENAME]

                class = "X" class
            }
            else {
                # actual status doesn't match expected status
                ++xmatchcounts [section, FILENAME]

                value  = class "<br>(X" expect ")"
                detail = "expected failure doesn't match actual status"
            }
        }
    }
    else {
        # signal

        class = "SIGNAL"
        value = compstatus

        if (expect != "") {
            if (expect == value) {
                # received signal matches the expected signal
                ++xfailcounts [section, FILENAME]

                # prepend "X" to the class/value to indicate that
                # the signal was expected
                class = "X" class
                value = "X" value
            }
            else {
                # received signal doesn't match expected status
                ++xmatchcounts [section, FILENAME]

                value  = value "<br>(X" expect ")"
                detail = "expected failure doesn't match actual status"
            }
        }
    }

    compstatus = class ":" value ":" detail

    compstatuses [section, FILENAME, compname] = compstatus
}

# locale section
1 == section && 1 == start {
}

# test section
2 == section && 1 == start {
}

# example section
3 == section && 1 == start {
}

########################################################################
# functions

function get_date(fulldate)
{
    n = split(fulldate, fields)

    if (2 == n) {
        # assume Windows 'date /T' format (i.e., "AbWeekDay MM/DD/YYYY")

        date = fields [2]

        split(date, fields, "/")

        date = (0 + fields [1]) "/" (0 + fields [2])
    }
    else if (2 < n) {
        # assume POSIX standard 'date' format in the "C" locale
        # i.e., "AbWeekDay AbMon  D HH:MM:SS $TZ YYYY"

        month = fields [2]
        mday  = fields [3]

        date = monthnames [month] "/" mday
    }
    else
        date = "N/A"

    return date
}


function get_time(fulldate)
{
    split(fulldate, fields)

    return fields [4]
}


# extracts operating system name and version from the log file name
function get_osname(fname)
{
    # strip directory prefix from filename
    pos = match(fname, ".*/")
    if (0 < pos)
        fname = substr(fname, RLENGTH + 1)

    # strip anything after the first dash
    pos = match(fname, "-")
    if (0 == pos)
        return ""

    osname = substr(fname, 1, pos - 1)
    # TO DO: extract version here
    return osname
}


# extracts the build type from the log file name
function get_buildtype(fname)
{
    # look for the beginning of the buildtype component followed
    # by the (Subversion) revision number
    pos = match(fname, "-(8|11|12|15)[aAdDsS]-[1-9][0-9]*-log")

    # is not found, try looking for the buildtype followed by
    # the name of the threads library and only then followed
    #  by the (Subversion) revision number
    if (0 == pos)
        pos = match(fname,
                    "-(8|11|12|15)[aAdDsS]-[a-z][a-z_0-9]*-[1-9][0-9]*-log*")

    buildtype = substr(fname, pos + 1)
    pos       = index(buildtype, "-")
    buildtype = substr(buildtype, 1, pos - 1)

    return buildtype
}


function get_dispname(section, compname)
{
    dir = ""

    if (1 == section) {
        # compose a URL pointing to the character set description
        # file and the locale defintion file in the repository
        pos = index(compname, ".")
        if (0 < pos) {
            locale  = substr(compname, 1, pos - 1)
            codeset = substr(compname, pos + 1);

            url      = svnpath "/etc/nls/src/" locale "?view=markup"
            dispname = "<a href=\"" url "\">" locale "</a>"

            url      = svnpath "/etc/nls/charmaps/" codeset "?view=markup"
            dispname = dispname ".<a href=\"" url "\">" codeset "</a>"
        }
        else {
            dispname = compname
        }
    }
    else if (2 == section) {
        dir = "tests"

        if (match(compname, "\\.stdcxx-"))
            dir = dir "/regress"
        else if (match(compname, "^0\\."))
            dir = dir "/self"
        else if (match(compname, "^17\\."))
            dir = dir "/intro"
        else if (match(compname, "^18\\."))
            dir = dir "/support"
        else if (match(compname, "^19\\."))
            dir = dir "/diagnostics"
        else if (match(compname, "^20\\."))
            dir = dir "/utilities"
        else if (match(compname, "^21\\."))
            dir = dir "/strings"
        else if (match(compname, "^22\\."))
            dir = dir "/localization"
        else if (match(compname, "^23\\."))
            dir = dir "/containers"
        else if (match(compname, "^24\\."))
            dir = dir "/iterators"
        else if (match(compname, "^25\\."))
            dir = dir "/algorithm"
        else if (match(compname, "^26\\."))
            dir = dir "/numerics"
        else if (match(compname, "^27\\."))
            dir = dir "/iostream"
        else if (match(compname, "^d_"))
            ; # dir = dir "/depr"
        else
            dir = ""
    
        if (dir != "") {
            dispname = "<a href=\"" svnpath "/" dir "/" compname
            dispname = dispname ".cpp?view=markup\">"
            dispname = dispname compname "</a>"
        }
        else {
            dispname = compname
        }
    }
    else {
        dispname = compname
    }

    return dispname
}


# prints a row of columns for the given section and component
function print_component(section, compname)
{
    # increment for each component, regardless of whether it gets
    # included in the output or not
    ++compinx

    row = ""
        
    # the number of failures and warning states of the current
    # component across all logs to decide whether to include
    # the component in the output or not
    nfailures = 0
    nwarnings = 0

    # maximum number of assertions exercised by the current
    # test across all logs
    maxasserts = 0

    # the number of columns to span the same class/value
    repeat = 1

    last_class = ""
    last_value = ""
    last_title = ""

    # iterate over all log files
    for (i = 1; i <= logcount; ++i) {

        # only process the specified section
        # if (section != substr(fi, 1, 1))
        #     continue

        # extract the name of the log file from the key
        # fname = substr(fi, 3)
        fname = logfnames [i]

        # check to see if the component was found in the current
        # log file being careful not to add a new entry to the
        # array
        if ((section, fname, compname) in compstatuses) {

            # component found

            status = compstatuses [section, fname, compname]

            n = split(status, triple, ":")

            class = triple [1]
            value = "" == triple [2] ? class : triple [2]
            title = triple [3]

            if ("ASSERT" == class) {

                if (0 < value) {
                    title = "failed " value " out of "  \
                        triple [3] " assertions"
                }
                else
                    title = ""

                if (maxasserts < triple [3])
                    maxasserts = triple [3]
            }
        }
        else {
            # component missing from log file
            class = "MISS"
            title = "not tested"
            value = "N/A"
        }

        if (class in failclasses) {
            ++nfailures
            ++logcompfails [fname]
        }
        else if (class != "OK" && class != "MISS") {
            ++nwarnings
            ++logcompwarns [fname]
        }

        if ("" == last_class) {
            last_class = class
            last_title = title
            last_value = value
        }
        else if (   last_class == class \
                 && last_value == value \
                 && last_title == title) {
            ++repeat
        }
        else {
            row = row "          <td class=\"" last_class "\""

            if (1 < repeat) {
                row     = row " colspan=" repeat
                coltext = "columns " i - repeat " to " i - 1
            }
            else {
                coltext = "column " i - 1
            }

            if (last_title == "")
                last_title = coltext
            else
                last_title = coltext ":" last_title

            row = row " title=\"" last_title "\""
            row = row ">" last_value "</td>\n"

            repeat     = 1
            last_class = class
            last_title = title
            last_value = value
        }

        ++statuscnts [fname, class]
    }

    row = row "          <td class=\"" last_class "\""

    if (1 < repeat) {
        row     = row " colspan=" repeat
        coltext = "columns " i - repeat " to " i - 1
    }
    else {
        coltext = "column " i - 1
    }

    if (last_title == "")
        last_title = coltext
    else
        last_title = coltext ": " last_title

    row = row " title=\"" last_title "\""
    row = row ">" last_value "</td>"

    repeat = 0

    if (0 < nfailures || nwarnings) {

        # increment only for components included in the output
        ++rowinx

        dispname = get_dispname(section, compname)

        print "        <tr>"
        print "          <td class=\"header\">" compinx "</td>"
        print "          <td class=\"name\">" dispname "</td>"

        if (2 == section)
            print "          <td class=\"total\">" maxasserts "</td>"

        print row
        print "        </tr>"

        return 1
    }

    return 0
}


# prints a table for each of the three sections (examples, locales,
# and tests)
function print_section(section)
{
    # the name of this section ("example", "locale", or "test")
    sectname = sectnames [section]

    if (0 == sectmaxcompcnts [section]) {
        print
        print "    <a name=\"" sectname "s\"></a>"
        print "    <h2>No " sectname "s found in " logcount " logs</h2>"

        return
    }

    # create the table header roughly in the following format:
    #
    # | ### | component |  1 |  2 |  3 |  4 |  5  |  6  | ... |  16 |
    # |     |    name   | 8d | 8D | 8s | 8S | 11d | 11D | ... | 15S |
    # |     |           |     date     |      date      | ... | date|

    thead =       "      <thead>\n"
    thead = thead "        <tr class=\"header\">\n"
    thead = thead "          <td rowspan=3 title=\"" \
        sectname " number\">###</td>\n"
    thead = thead "          <td rowspan=3 title=\""    \
        sectname " name\">"
    thead = thead sectname " name</td>\n"

    if (2 == section) {
        # for tests only, insert a column at offset 2 with the maximum
        # number of assertions found in the test on the given row across
        # all build logs
        thead = thead "          <td rowspan=3 "
        thead = thead "title=\"maximum total assertions\">"
        thead = thead "max<br>asserts</td>\n"
    }

    row0 = ""
    row1 = "        <tr class=\"header\">\n"
    row2 = "        <tr class=\"header\">\n"

    colnos = ""

    lastdate = ""   # date (M/d) of the last log
    lastfull = ""   # full date and time of the last log
    datespan = 0    # number of same consecutive dates

    # iterate over the array of section counts for each log file
    # and compose the column headers for each log
    for (i = 1; i <= logcount; ++i) {

        logname = logfnames [i]

        # strip directory prefix from file name
        loghref = get_gzlogfname(logname)

        colnos = colnos "          <td title=\"" loghref "\">"
        colnos = colnos "<a href=\"" loghref "\">" i "</a></td>"

        buildtype = get_buildtype(logname)
        buildmode = buildmodes [buildtype]

        row1 = row1 "          <td title=\"" buildmode "\">" buildtype "</td>\n"

        if (logname in logdates) { 
            # parse the date and time from the date extracted
            # from the log
            fulldate = logdates [logname]
            date     = get_date(fulldate)
            time     = get_time(fulldate)
        }
        else {
            # date not available
            fulldate = "unknown date"
            date     = "N/A"
        }

        if (0 == datespan) {
            # first iteration
            lastdate = date
            lastfull = fulldate
            datespan = 1
        }
        else if (date == lastdate) {
            # increment the span of the last seen date
            ++datespan
        }
        else {
            row2 = row2 "          <td"

            # append the full date as a tooltip only for a date
            # for a single log, otherwise the timestamps are most
            # likely different for each log
            if (1 < datespan)
                row2 = row2 " colspan=" datespan ">" lastdate "</td>\n"
            else
                row2 = row2 " title=\"" lastfull "\">" lastdate "</td>\n"

            lastdate = date
            lastfull = fulldate
            datespan = 1
        }
    }

    # append the date of last set of logs
    row2 = row2 "          <td"

    # as above, append the full date as a tooltip only for a date
    # for a single log, otherwise the timestamps are most likely
    # different for each log
    if (1 < datespan)
        row2 = row2 " colspan=" datespan ">" lastdate "</td>\n"
    else
        row2 = row2 " title=\"" lastfull "\">" lastdate "</td>\n"

    row0 = row0 colnos "\n        </tr>\n"
    row1 = row1 "        </tr>\n"
    row2 = row2 "        </tr>\n"

    thead = thead row0 row1 row2 "      </thead>"

    print "    <a name=\"" sectname "s\"></a>"
    print "    <h2>Results of " sectmaxcompcnts [section] " "        \
        sectname "s from " logcount " logs</h2>"
    print "    <table>"

    print thead

    print "      <tbody>"

    # reset the arrays
    split("", logcompwarns)
    split("", logcompfails)
    split("", statuscnts)

    # iterate over elements of the compcnts array using the section
    # number and the name of the comoponent as the key (index)

    if (2 == section)
        colspan = " colspan=2"
    else
        colspan = ""

    # one-based component index (of all found)
    # not every component makes it into the table
    compinx = 0

    # one-based row index (of all rows printed)
    rowinx = 1

    # split the list of components in the current section into an array
    # (the list of component names is in the same order as in the log)
    compcount = split(sectcomponents [section], components, " ")

    # iterate over the array
    for (ci = 1; ci <= compcount; ++ci) {

        compname = components [ci]

        # determine whether the current component should be included
        # in output or not and if so, print it out
        printed = print_component(section, compname)

        if (printed && (rowinx % 20) == 0) {

            print "        <tr class=\"header\">"
            print "          <td></td>"
            print "          <td " colspan ">column number and log</td>"
            print colnos
            print "        </tr>"
        }
    }

    print "      </tbody>"

    # print totals in the table footer
    print "      <tfooter>"

    ##################################################################
    ### print column headers again

    print "        <tr class=\"header\">"
    print "          <td></td>"
    print "          <td " colspan ">column number and log</td>"
    print colnos
    print "        </tr>"

    ##################################################################
    ### print totals for each interesting status (class)
    print "        <tr class=\"header\">"
    print "          <td></td>"
    print "          <td" colspan ">status</td>"
    print "          <td colspan=" logcount ">totals for status</td>"
    print "        </tr>"

    for (si in states) {

        row =     "        <tr>\n"
        row = row "          <td class=\"header\"></td>\n"
        row = row "          <td class=\"" si "\"" colspan ">" si "</td>\n"

        nfails = 0

        # the number of times the same value consecutively repeats 
        repeat     = 1
        last_value = "?"

        for (i = 1; i <= logcount; ++i) {

            fname = logfnames [i]

            if ((fname, si) in statuscnts) {

                ++nfails

                value = statuscnts [fname, si]
            }
            else
                value = ""

            if ("?" == last_value)
                last_value = value
            else if (value == last_value)
                ++repeat
            else {
                row = row "          <td class=\"total\""

                if (1 < repeat) {
                    row = row " colspan=" repeat
                    row = row " title=\"columns " i - repeat " to " i - 1 "\""
                }
                else {
                    row = row " title=\"column " i - 1 "\""
                }

                row = row ">" last_value "</td>\n"

                last_value = value
                repeat     = 1
            }
        }

        if (0 < nfails) {
            row = row "          <td class=\"total\""

            if (1 < repeat) {
                row = row " colspan=" repeat
                row = row " title=\"columns " i - repeat " to " i - 1 "\""
            }
            else {
                row = row " title=\"column " i - 1 "\""
            }

            row = row ">" last_value "</td>\n"

            print row "        </tr>"
        }
    }

    ##################################################################
    # print number of expected failures
    row    = ""
    nfails = 0

    # the number of times the same value consecutively repeats 
    repeat     = 1
    last_value = "?"

    for (i = 1; i <= logcount; ++i) {

        fname = logfnames [i]

        if ((section, fname) in xfailcounts) {

            ++nfails

            value = xfailcounts [section, fname]
        }
        else
            value = ""

        if ("?" == last_value)
            last_value = value
        else if (value == last_value)
            ++repeat
        else {
            row = row "          <td class=\"total\""

            if (1 < repeat) {
                row = row " colspan=" repeat
                row = row " title=\"columns " i - repeat " to " i - 1 "\""
            }
            else {
                row = row " title=\"column " i - 1 "\""
            }

            row = row ">" last_value "</td>\n"

            last_value = value
            repeat     = 1
        }
    }

    if (0 < nfails) {
        row = row "          <td class=\"total\""

        if (1 < repeat) {
            row = row " colspan=" repeat
            row = row " title=\"columns " i - repeat " to " i - 1 "\""
        }
        else {
            row = row " title=\"column " i - 1 "\""
        }

        row = row ">" last_value "</td>\n"

        print "        <tr>"
        print "          <td class=\"header\"></td>"
        print "          <td class=\"header\" title=\"number "          \
            "of expected failures\"" colspan ">expected failures</td>"
        print row "        </tr>"
    }

    ##################################################################
    # print expected/unexpected mismatches
    row    = ""
    nfails = 0

    # the number of times the same value consecutively repeats 
    repeat     = 1
    last_value = "?"

    for (i = 1; i <= logcount; ++i) {

        fname = logfnames [i]

        if ((section, fname) in xmatchcounts) {

            ++nfails

            value = xmatchcounts [section, fname]
        }
        else
            value = ""

        if ("?" == last_value)
            last_value = value
        else if (value == last_value)
            ++repeat
        else {
            row = row "          <td class=\"total\""

            if (1 < repeat) {
                row = row " colspan=" repeat
                row = row " title=\"columns " i - repeat " to " i - 1 "\""
            }
            else {
                row = row " title=\"column " i - 1 "\""
            }

            row = row ">" last_value "</td>\n"

            last_value = value
            repeat     = 1
        }
    }

    if (0 < nfails) {
        row = row "          <td class=\"total\""

        if (1 < repeat) {
            row = row " colspan=" repeat
            row = row " title=\"columns " i - repeat " to " i - 1 "\""
        }
        else {
            row = row " title=\"column " i - 1 "\""
        }

        row = row ">" last_value "</td>\n"

        print "        <tr>"
        print "          <td class=\"header\"></td>"
        print "          <td class=\"header\" title=\"number "          \
            "of mismatches between expected and actual statuses\"" colspan \
            ">expected/actual mismatches</td>"
        print row "        </tr>"
    }

    ##################################################################
    # print total number of failures
    row    = ""
    nfails = 0

    # the number of times the same value consecutively repeats 
    repeat     = 1
    last_value = "?"

    for (i = 1; i <= logcount; ++i) {

        fname = logfnames [i]

        if (fname in logcompfails) {

            ++nfails

            value = logcompfails [fname]
        }
        else
            value = ""

        if ("?" == last_value)
            last_value = value
        else if (value == last_value)
            ++repeat
        else {
            row = row "          <td class=\"total\""

            if (1 < repeat) {
                row = row " colspan=" repeat
                row = row " title=\"columns " i - repeat " to " i - 1 "\""
            }
            else {
                row = row " title=\"column " i - 1 "\""
            }

            row = row ">" last_value "</td>\n"

            last_value = value
            repeat     = 1
        }
    }

    if (0 < nfails) {
        row = row "          <td class=\"total\""

        if (1 < repeat) {
            row = row " colspan=" repeat
            row = row " title=\"columns " i - repeat " to " i - 1 "\""
        }
        else {
            row = row " title=\"column " i - 1 "\""
        }

        row = row ">" last_value "</td>\n"

        print "        <tr>"
        print "          <td class=\"header\"></td>"
        print "          <td class=\"header\" title=\"total number "    \
            "of failed " sectname "s\"" colspan ">failures</td>"
        print row "        </tr>"
    }

    ##################################################################
    # print total number of components exercised
    row    = ""
    nfails = 0

    # the number of times the same value consecutively repeats 
    repeat     = 1
    last_value = "?"

    for (i = 1; i <= logcount; ++i) {

        fname = logfnames [i]

        if ((section, fname) in logcompcnts) {

            ++nfails

            value = logcompcnts [section, fname]
        }
        else
            value = ""

        if ("?" == last_value)
            last_value = value
        else if (value == last_value)
            ++repeat
        else {
            row = row "          <td class=\"total\""

            if (1 < repeat) {
                row = row " colspan=" repeat
                row = row " title=\"columns " i - repeat " to " i - 1 "\""
            }
            else {
                row = row " title=\"column " i - 1 "\""
            }

            row = row ">" last_value "</td>\n"

            last_value = value
            repeat     = 1
        }
    }

    if (0 < nfails) {
        row = row "          <td class=\"total\""

        if (1 < repeat) {
            row = row " colspan=" repeat
            row = row " title=\"columns " i - repeat " to " i - 1 "\""
        }
        else {
            row = row " title=\"column " i - 1 "\""
        }

        row = row ">" last_value "</td>\n"

        print "        <tr>"
        print "          <td class=\"header\"></td>"
        print "          <td  class=\"header\" title=\"total number of " \
            sectname "s exercised\"" colspan ">total</td>"

        print row "        </tr>"
    }

    ##################################################################
    print "      </tfooter>"
    print "    </table>"
}


function format_size(size)
{
    if (1000000000 <= size)
        return size / 1073741824.0 " GB"

    if (1000000 <= size)
        return size / 1048576.0 " MB "

    if (1000 <= size)
        return size / 1024 " kB"

    if (0 < size)
        return size " B"

    return size
}


function get_gzlogfname(fname)
{
    # strip directory prefix from file name
    pos = match(fname, ".*/")
    if (0 < pos)
        fref = substr(fname, RLENGTH + 1)
    else
        fref = fname

    # replace the temporary PID suffix with ".gz"
    pos = match(fref, "\\.[1-9][0-9]*$")
    if (0 < pos)
        fref = substr(fref, 1, pos - 1)

    # replace the trailing .txt suffix with .gz.txt
    sub("\\.txt$", ".gz.txt", fref)

    return "logs/" fref
}


function print_logtable()
{
    thead =                                                             \
        "      <thead>\n"                                               \
        "        <tr>\n"                                                \
        "          <th rowspan=3 title=\"column number and log\">"      \
        "log</th>\n"                                                    \
        "          <th rowspan=3>operating<br>system</th>\n"            \
        "          <th rowspan=3 title=\"hardware architecture\">"      \
        "arch</th>\n"                                                   \
        "          <th rowspan=3>compiler</th>\n"                       \
        "          <th rowspan=3>build<br>type</th>\n"                  \
        "          <th rowspan=3>start date and time</th>\n"            \
        "          <th rowspan=3 title=\"build age at the time "        \
        "of this report\">"                                             \
        "age</th>\n"                                                    \
        "          <th rowspan=3>revision</th>\n"                       \
        "          <th colspan=5>sizes</th>\n"                          \
        "          <th colspan=3>diagnostics</th>\n"                    \
        "          <!-- <th colspan=6>components</th> -->\n"            \
        "        </tr>\n"                                               \
        "        <tr>\n"                                                \
        "          <th rowspan=2 title=\"size of the library binary\">" \
        "library</th>\n"                                                \
        "          <th colspan=2>log</th>\n"                            \
        "          <th colspan=2>build</th>\n"                          \
        "          <th rowspan=2 title=\"number of error messages\">"   \
        "errors</th>\n"                                                 \
        "          <th colspan=2 title=\"number of warning messages\">" \
        "warnings</th>\n"                                               \
        "          <!-- <th colspan=2>examples</th> -->\n"              \
        "          <!-- <th colspan=2>locales</th> -->\n"               \
        "          <!-- <th colspan=2>tests</th> -->\n"                 \
        "        </tr>\n"                                               \
        "        <tr>\n"                                                \
        "          <th title=\"size of gzipped log\">gzip</th>\n"       \
        "          <th title=\"size of expanded log\">text</th>\n"      \
        "          <th title=\"size of full build\">full</th>\n"        \
        "          <th title=\"size of clean build\">clean</th>\n"      \
        "          <th title=\"library\">lib</th>\n"                    \
        "          <th title=\"other components\">other</th>\n"         \
        "          <!-- <th title=\"number of examples exercised\">"    \
        "total</th> -->\n"                                              \
        "          <!-- <th title=\"number of examples failed\">"       \
        "failed</th> -->\n"                                             \
        "          <!-- <th title=\"number of locales exercised\">"     \
        "total</th> -->\n"                                              \
        "          <!-- <th title=\"number of locales failed\">"        \
        "failed</th> -->\n"                                             \
        "          <!-- <th title=\"number of tests exercised\">"       \
        "total</th> -->\n"                                              \
        "          <!-- <th title=\"number of tests failed\">"          \
        "failed</th> -->\n"                                             \
        "        </tr>\n"                                               \
        "      </thead>"
    
    print "    <h2>Logs and Columns</h2>"
    print "    <table>"
    print thead
    print "    <tbody>"

    for (i = 1; i <= logcount; ++i) {

        fname = logfnames [i]

        n = split(logos [fname], platfields, FSEP)
        osname   = platfields [1]
        osdesc   = platfields [2]
        arch     = platfields [3]
        archdesc = platfields [4]
        cxxname  = platfields [5]
        cxxdesc  = platfields [6]

        if (fname in buildstatus)
            print "        <tr class=\"" buildstatus [fname] "\">"
        else
            print "        <tr>"

        # strip directory prefix from file name
        loghref = get_gzlogfname(fname)
        print "          <td><a href=\"" loghref "\">" i "</a></td>"

        ################################################################
        # extract and format the operating system name and version
        oscell = "<td"

        if ("" == osdesc) {
            # extract osname from log filename
            osname = get_osname(fname)
        }
        else {
            oscell = oscell " title=\"" osdesc "\""
        }

        oscell = oscell ">" osname"</td>"

        ################################################################
        # extract and format the hardware architecture
        archcell = "<td"

        if ("" != archdesc)
            archcell = archcell " title=\"" archdesc "\""

        archcell = archcell ">" arch "</td>"

        ################################################################
        # extract and format the compiler and version
        cxxcell = "<td"

        if ("" != cxxdesc)
            cxxcell = cxxcell " title=\"" cxxdesc "\""

        cxxcell = cxxcell ">" cxxname "</td>"

        print "          " oscell
        print "          " archcell
        print "          " cxxcell

        ################################################################
        # extract build type from log file name
        buildtype = get_buildtype(fname)
        buildmode = buildmodes [buildtype]

        print "          <td>" buildtype "</td>"
        print "          <td>" logdates [fname] "</td>"

        # compute and format the age of the build
        duration = "duration -f \"" logdates [fname] \
                               "\" \"" todays_date "\""
        duration | getline fullage
        close(duration)

        pos      = index(fullage, ", ")
        buildage = substr(fullage, 1, pos - 1)
        print "          <td title=\"" fullage "\">" buildage "</td>"

        pos = match(fname, "-[1-9][0-9]*-log")
        buildrev = substr(fname, pos + 1, RLENGTH - 5)

        ################################################################
        # format a ViewVC URL to the revision number/log
        revurl = "http://svn.apache.org/viewvc?view=rev&rev=" buildrev
        print "          <td><a href=\"" revurl "\">" buildrev "</a></td>"

        ################################################################
        # library size
        fullsize = fname in libsizes ? libsizes [fname] : ""
        size     = format_size(libsizes [fname])
        print "          <td title=\"" fullsize " bytes\">" size "</td>"

        print "          <td>" gzlogsize "</td>"

        ################################################################
        # format the size of the expanded log file
        cmd = "du -k " fname
        cmd | getline
        close(cmd)

        fullsize = $1
        size     = format_size(fullsize * 1024)

        print "          <td title=\"" fullsize " kb\">" size "</td>"

        ################################################################
        # compute the full build size
        fullsize = fname in buildsizes ? buildsizes [fname] : ""
        size     = format_size(fullsize * 1024)
        print "          <td title=\"" fullsize " kB\">" size "</td>"

        ################################################################
        # format the build size after the clean target has been run
        fullsize = fname in cleansizes ? cleansizes [fname] : ""
        size     = format_size(fullsize * 1024)
        print "          <td title=\"" fullsize " kB\">" size "</td>"

        print "          <td>" logerrors [fname] "</td>"
        print "          <td>" libwarnings [fname] "</td>"
        print "          <td>" logwarnings [fname] "</td>"
        print "        </tr>"
    }

    print "      </tbody>"
    print "      <tfooter>"
    print "      </tfooter>"
    print "    </table>"
}


# reformat time string argument in the original format of "NmN.NNNs"
# (i.e., as produced by the times shell utility), as "M:SS" rounding
# fractional seconds as necessary
function format_time(timestr)
{
    pos  = index(timestr, "m")
    mins = substr(timestr, 1, pos - 1)
    secs = substr(timestr, pos + 1)
    secs = substr(secs, 1, length(secs) - 1)

    timestr = mins ":"
    if (int(secs) < 10)
        timestr = timestr "0"

    timestr = timestr int(secs)

    return timestr
}


function print_timingstable()
{
    thead =                                                             \
        "<table>\n"                                                     \
        "  <thead>\n"                                                   \
        "    <tr>\n"                                                    \
        "      <th rowspan=3 title=\"column number and log\">"          \
        "log</th>\n"                                                    \
        "      <th rowspan=3>build<br>type</th>\n"                      \
        "      <th colspan=21>build and run times (M:SS)</th>\n"        \
        "    </tr>\n"                                                   \
        "    <tr>\n"                                                    \
        "      <th colspan=3 title=\"library configuration times\">"    \
        "config</th>\n"                                                 \
        "      <th colspan=3 title=\"library build times\">"            \
        "library</th>\n"                                                \
        "      <th colspan=3 title=\"build times for all examples\">"   \
        "examples</th>\n"                                               \
        "      <th colspan=3 title=\"build times for all utilities\">"  \
        "utilities</th>\n"                                              \
        "      <th colspan=3 title=\"build times for all tests\">"      \
        "tests</th>\n"                                                  \
        "      <th colspan=3 title=\"run times for all components\">"   \
        "run times</th>\n"                                              \
        "      <th colspan=3 title=\"total build and run times\">"      \
        "<b>total</b></th>\n"                                           \
        "    </tr>\n"                                                   \
        "    <tr>\n"                                                    \
        "      <th>real</th>\n"                                         \
        "      <th>user</th>\n"                                         \
        "      <th>sys</th>\n"                                          \
        "      <th>real</th>\n"                                         \
        "      <th>user</th>\n"                                         \
        "      <th>sys</th>\n"                                          \
        "      <th>real</th>\n"                                         \
        "      <th>user</th>\n"                                         \
        "      <th>sys</th>\n"                                          \
        "      <th>real</th>\n"                                         \
        "      <th>user</th>\n"                                         \
        "      <th>sys</th>\n"                                          \
        "      <th>real</th>\n"                                         \
        "      <th>user</th>\n"                                         \
        "      <th>sys</th>\n"                                          \
        "      <th>real</th>\n"                                         \
        "      <th>user</th>\n"                                         \
        "      <th>sys</th>\n"                                          \
        "      <th>real</th>\n"                                         \
        "      <th>user</th>\n"                                         \
        "      <th>sys</th>\n"                                          \
        "    </tr>\n"                                                   \
        "  </thead>\n"

    print "    <h2>Timings</h2>"
    print "    <table>"
    print thead
    print "    <tbody>"

    for (i = 1; i <= logcount; ++i) {

        fname = logfnames [i]

        if (fname in buildstatus)
            print "        <tr class=\"" buildstatus [fname] "\">"
        else
            print "        <tr>"

        # strip directory prefix from file name
        loghref = get_gzlogfname(fname)
        print "          <td><a href=\"" loghref "\">" i "</a></td>"

        buildtype = get_buildtype(fname)
        print "          <td>" buildtype "</td>"

        timecells = ""

        for (j = 1; j in buildstages; ++j) {

            stage = buildstages [j];

            if ((fname, stage) in logstagetimes) {

                # format real, user, and system times for the stage
                timestr = logstagetimes [fname, stage]
                split(timestr, atimes, FSEP)

                realtim = format_time(atimes [1])
                usrtim  = format_time(atimes [2])
                systim  = format_time(atimes [3])

                # highlight total times in bold
                if ("total" == stage) {
                    pfx = "<b>"
                    sfx = "</b>"
                }
                else {
                    pfx = ""
                    sfx = ""
                }
                    
                timecells = timecells "<td>" pfx realtim sfx "</td>"
                timecells = timecells "<td>" pfx usrtim  sfx "</td>"
                timecells = timecells "<td>" pfx systim  sfx "</td>"
            }
            else {
                timecells = timecells "<td></td><td></td><td></td>"
            }
        }

        print timecells "</tr>"
    }

    print "      </tbody>"
    print "      <tfooter>"
    print "      </tfooter>"
    print "    </table>"
}


END {

    if (0 == bodyonly) {
        print "<html>"
        print "  <head>"
        print "    <link rel=\"stylesheet\" href=\"resultstyle.css\">"
        print "  </head>"
        print "  <body>"
    }

    print_logtable()

    print_timingstable()

    for (section = 1; section <= 3; ++section)
        print_section(section)

    if (0 == bodyonly) {
        print "  </body>"
        print "</html>"
    }
}
