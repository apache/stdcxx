#!/bin/awk
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
#     myname [comptype=<string>] [bodyonly=0|1] logs...
#
# VARIABLES:
#   bodyonly     when non-zero, suppresses the <html> tags
#   comptype     component type (example, locale, or test)
#   expect       set of expected results
#
########################################################################


BEGIN {
    # array of component counts for each component
    # compcnts [<name>]

    # one-based array of files processed
    # files [1..]

    # map of [FILENAME,components] to component status
    # compstatuses [FILENAME, compcnts]

    sectnames [1] = "locale"
    sectnames [2] = "test"
    sectnames [3] = "example"

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

    svnpath="http://svn.apache.org/viewvc/incubator/stdcxx/trunk"

    # regular expression to match a name (e.g., compiler or OS)
    re_name    = "[A-Za-z][A-Za-z_0-9]*"

    # regular expression to match a version string
    re_version = "[1-9][0-9]*(\\.[0-9][0-9]*)*"

    # regular expression matching the buildtype string
    re_buildtype = "(8|11|12|15)[aAdDsS](-(solaris|win32))?"

    # regular expresssion matching a log file name
    re_logname = re_name "-" re_version    \
        "-" re_name "-" re_name "-" re_version \
        "-" re_buildtype "-" "[1-9][0-9]*-log"
}


# action to detect the type of file
1 == FNR {
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


# action to process file containing specification of expected failures
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
    gsub("?", ".", pattern)

    # if patter contains an open brace expand it using the shell
    if (0 < index(pattern, "{")) {
        cmd = "echo " pattern
        cmd | getline pattern
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

# action to extract the build date and time
/^##* *date *: *$/ {

    getline

    logdates [FILENAME] = $0
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


# locale, example, or test section
1 == start {

    compname = $1

    # trim suffix from component name
    sub("(\\.bat|\\.exe|\\.sh)$", "", compname)

    compstatus = $2
    expect     = ""
    comment    = ""

    # increment the number of times the current component occurs
    # in the current section across all logs
    ++compcnts [section, compname]

    # increment the number of components in the current section
    # and log
    ++logcompcnts [section, FILENAME]

    if (sectmaxcompcnts [section] < logcompcnts [section, FILENAME])
        sectmaxcompcnts [section] = logcompcnts [section, FILENAME]

    if (compname in expected_statuses)
        expspec = expected_statuses [compname]
    else
        expspec = ""    

    n = split(expspec, slist, ":")

    for (i = 1; i <= n; ++i) {

        split(slist [i], fields, " ")

        # try to match the log file name against the specified pattern
        if (match(FILENAME, fields [1])) {
            # extract the expected component status and the comment
            expect  = fields [2] 
            comment = fields [3]

            break
        }
    }

    # class, value, and optional detail for the cell
    class  = ""
    value  = ""
    detail = ""

    # compstatus:
    # <class>:[<value>]:[<detail>]

    if (0 == compstatus) {
        # successful exit status

        class = "OK"

        if (3 != section) {
            runwarns = $3
            asserts  = $4
            failed   = $5

            if (0 < failed) {
                class  = "ASSERT"
                value  = failed
                detail = asserts
            }
            else if (0 < runwarns) {
                class = "RUNWARN"
                value = runwarns
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
                # signal matches the expected signal
                ++xfailcounts [section, FILENAME]

               class = "X" class
            }
            else {
                # signal doesn't match expected status
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
    split(fulldate, fields)
    month = fields [2]
    mday  = fields [3]

    date = monthnames [month] "/" mday

    return date
}


function get_time(fulldate)
{
    split(fulldate, fields)

    return fields [4]
}


# extracts the build type from the log file name
function get_buildtype(fname)
{
    # look for the beginning of the buildtype component followed
    # by the (Subversion) revision number
    pos = match(fname, "-[1-9][0-9]*[aAdDsS]-[1-9][0-9]*-log")

    # is not found, try looking for the buildtype followed by
    # the name of the threads library and only then followed
    #  by the (Subversion) revision number
    if (0 == pos)
        pos = match(fname, "-[1-9][0-9]*[aAdDsS]-[a-z][a-z]*-[1-9][0-9]*-log*")

    buildtype = substr(fname, pos + 1)

    pos = index(buildtype, "-")
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

    thead =       "      <thead>\n"
    thead = thead "        <tr class=\"header\">\n"
    thead = thead "          <td rowspan=3 title=\"" \
        sectname " number\">###</td>\n"
    thead = thead "          <td rowspan=3 title=\""    \
        sectname " name\">"
    thead = thead sectname " name</td>\n"

    if (2 == section) {
        thead = thead "          <td rowspan=3 "
        thead = thead "title=\"maximum total assertions\">"
        thead = thead "max<br>asserts</td>\n"
    }

    row0 = ""
    row1 = "        <tr class=\"header\">\n"
    row2 = "        <tr class=\"header\">\n"

    colnos = ""

    # iterate over the array of section counts for each log file
    # and compose the column headers for each log
    for (i = 1; i <= logcount; ++i) {

        logname = logfnames [i]

        loggzfname = substr(logname, 1, index(logname, ".txt") - 1) ".gz.txt"

        colnos = colnos "          <td title=\"" logname "\">"
        colnos = colnos "<a href=\"" loggzfname "\">" i "</a></td>"

        buildtype = get_buildtype(logname)
        buildmode = buildmodes [buildtype]

        row1 = row1 "          <td title=\"" buildmode "\">" buildtype "</td>\n"

        fulldate = logdates [logname]
        date     = get_date(fulldate)
        time     = get_time(fulldate)

        row2 = row2 "          <td title=\"" fulldate "\">" date "</td>\n"
    }

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

    # one-based component index (of all found)
    compinx = 0

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

    rowinx = 1

    for (ci in compcnts) {

        # only process the specified section
        if (section != substr(ci, 1, 1))
            continue

        # extract the name of the component from the key
        compname = substr(ci, 3)

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


END {

    if (0 == bodyonly) {
        print "<html>"
        print "  <head>"
        print "    <link rel=\"stylesheet\" href=\"resultstyle.css\">"
        print "  </head>"
        print "  <body>"
    }

    for (section = 1; section <= 3; ++section)
        print_section(section)

    if (0 == bodyonly) {
        print "  </body>"
        print "</html>"
    }
}
