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
#     myname [n=#] [name=$name] [dispname=$dispname]
#            [svnpath=$svnpath] [verbose=0|1] [comptype=$comp]
#            [totalsfile=$totalsfile] [expectfile=$expectfile]
#            logs...
#
# VARIABLES:
#   n            component number
#   name         component name
#   dispname     component display name
#   verbose      verbose mode
#   comptype     component type (example, locale, or test)
#   totalsfile   name of totals file
#   expectfile   name of file with expected results
#
########################################################################


BEGIN {
    # the text of all columns, output after all logs have been processed
    columns = ""

    # total number of times the current component has been found to fail
    # across all logs (a row for the component is normally output only
    # when compfails is non-zero, except when in verbose mode)
    compfails = 0

    # maximum and minimum number of assertions for the current component
    max_asserts = 0
    min_asserts = -1

    # one-based file number (the first file on the command line
    # is number one, etc.)
    fileno = 0

    # the (one-based) number of the file in which the current
    # component was last found (detects a component that is
    # missing from one or more files)
    found_in_file = -1

    # array of the total numbers of components found in each log
    # logcounts

    # array of the total numbers of failed components in each log
    # logfails

    # array of the total numbers of components in each log that
    # failed one or more runtime assertions
    # assertcounts

    # array of the total numbers of components in each log that
    # failed to build (compile or link)
    # buildfails

    # array of the total numbers of components in each log that
    # produced unexpected output (DIFF)
    # diffcounts

    # array of the total numbers of components in each log that
    # exited with a non-zero status
    # exitcounts

    # array of the total numbers of components in each log that
    # exited with a signal
    # signalcounts

    # array of the total numbers of components in each log that
    # exited with SIGHUP (likely timed out)
    # sighupcounts

    # array of the total numbers of components missing from each log
    # missingcounts
}


# action to keep track of the current file number
1 == FNR {
    if (0 < fileno && found_in_file < fileno) {
        ++missingcounts [fileno]

        columns = columns "\n        <td class=\"missing\">N/A</td>"
        found_in_file = -1
    }

    ++fileno
}


# match the first record to the name optionally followed
# by one of the three suffixes
   $1 ~ "^" name "$"       \
|| $1 ~ "^" name ".bat$"   \
|| $1 ~ "^" name ".exe$"   \
|| $1 ~ "^" name ".sh$" {

    # EXPECTED RECORD FORMAT(s)
    #
    #  $1     $2       $3     $4      $5     $6       $7    $8    $9
    # <name> <status> <warn> <asrts> <fail> <percnt> <usr> <sys> <real>
    #
    #  $1     $2                                      $3    $4    $5
    # <name> <status>                                <usr> <sys> <real>
    #
    # The <warn>, <asrts>, <fail>, and <percnt> fields might be missing.

    # increment the total of components found in this log
    ++logcounts [fileno]

    # the second field is always the exit status of the component
    status = $2

    # the tile (tooltip) to display over the value
    title = ""

    # the value of the cell
    value = ""

    found_in_file = fileno

    if (status == 0) {

        # successful exit status

        if (comptype == "example" || $5 == 0) {

            # component is an example or, when not, had zero failures

            # get the number of warnings (only valid when the number
            # fields is greater than 5)
            warnings = 5 < NF ? $3 : 0

            if (warnings == 0) {
                class = "OK"
                value = "0"
            }
            else {
                ++compfails;
                ++logfails [fileno]

                class = "RUNWARN"
                title = warnings " runtime warnings"
                value = "(" warnings ")"
            }
        }
        else {

            # component is not an example or, when it is, had failures

            ++compfails
            ++logfails [fileno]
            ++assertcounts [fileno]

            class = "ASSERT"
            title = $5 " failed assertions out of " $4
            value = "(" $5 ")"
        }

        if (comptype == "test") {
            asserts [n] = $5

            if (max_asserts < $4) {
                max_asserts = $4
            }

            if ($4 < min_asserts || min_asserts < 0) {
                min_asserts = $4
            }
        }
    }
    else if (0 < status && status < 256) {
        ++compfails
        ++logfails [fileno]
        ++exitcounts [fileno]

        class = "EXIT"
        title = "exit status of " status
        value = status
    }
    else if (status == "FORMAT" || status == "NOUT" || status == "OUTPUT") {
        ++compfails

        class = status
        value = status == "FORMAT" ? "FMAT" : status
    }
    else if (status == "DIFF") {
        ++compfails
        ++logfails [fileno]
        ++diffcounts [fileno]

        class = status
        value = status
    }
    else if (status == "COMP" || status == "LINK") {
        ++compfails
        ++logfails [fileno]
        ++buildfails [fileno]

        class = status
        value = status
    }
    else {
        ++compfails
        ++logfails [fileno]
        if (status == "HUP" || status == "SIGHUP")
            ++sighupcounts [fileno]
        else
            ++signalcounts [fileno]

        class = "SIGNAL"
        value = status
    }

    columns = columns "\n          <td class=\"" class "\""
    if (title != "")
        columns = columns " title=\"" title "\""
    columns = columns ">" value "</td>"
}


function print_totals (class, title, text, totals, nfields)
{
    sum = 0

    row = ""

    for (i = 1; i <= nfields; ++i) {
        sum += totals [i]

        row = row "            <td class=\"total\">"
        if (totals [i] != 0)
            row = row totals [i]

        row = row "</td>"
    }

    if (sum != 0) {
   
        print "          <tr>"
        print "            <td class=\"header\"></td>"
        print "            <td class=\"" class "\" title=\"" \
            title "\">" text "</td>"
        print extra_cell
        print "            " row
    }
}


function build_summary ()
{
    if (verbose || compfails) {
        print "        <tr>"
        print "          <td class=\"rowno\">" n "</td>"
        print "          <td class=\"name\"><a name=\"" name "\"></a>"
        print "            " dispname
        print "          </td>"

        if (comptype == "test") {
            print "          <td class=\"number\">" max_asserts "</td>"
        }

        print columns;
        print "      </tr>"
    }

    if (0 > getline line < totalsfile) {
        # the first time through there is no totalsfile
        nfields = fileno
    }
    else {
        nfields = split(line, logcounts_prev)

        getline line < totalsfile
        split(line, logfails_prev)

        getline line < totalsfile
        split(line, assertcounts_prev)

        getline line < totalsfile
        split(line, buildfails_prev)

        getline line < totalsfile
        split(line, diffcounts_prev)

        getline line < totalsfile
        split(line, exitcounts_prev)

        getline line < totalsfile
        split(line, signalcounts_prev)

        getline line < totalsfile
        split(line, sighupcounts_prev)

        getline line < totalsfile
        split(line, missingcounts_prev)

        close(totalsfile)
    }

    if (name == "@") {

        # the special (bogus) name indicates a request to format
        # totals from the totals file

        if (comptype == "test")
            extra_cell = "            <td class=\"header\">&mdash;</td>"
        else
            extra_cell = ""

        print "        <tfoot>"
        print "          <tr>"
        print "            <td class=\"header\"></td>"
        print "            <td class=\"header\">totals for status</td>"
        print extra_cell

        for (i = 1; i <= nfields; ++i)
            print "<td class=\"header\">&mdash;</td>"

        print "          </tr>"

        # output the totals of components that failed to build
        print_totals("COMP", "number of " comptype "s that failed to build",
                     "BUILD", buildfails_prev, nfields)

        # output the totals of examples that produced unexpected output
        print_totals("DIFF",
                     "number of " comptype "s with unexpected output",
                     "DIFF", diffcounts_prev, nfields);

        # output the totals of components that had assertions
        print_totals("ASSERT",
                     "number of " comptype "s with failed assertions",
                     "ASSERT", assertcounts_prev, nfields);

        # output the totals of components that exited with non-zero status
        print_totals("EXIT",
                     "number of " comptype "s with non-zero exit status",
                     "EXIT", exitcounts_prev, nfields);

        # output the totals of components that exited with a signal
        print_totals("SIGNAL",
                     "number of signalled " comptype "s",
                     "SIGNAL", signalcounts_prev, nfields);

        # output the totals of components that exited with a signal
        print_totals("SIGNAL",
                     "number of " comptype "s that (likely) timed out",
                     "SIGHUP", sighupcounts_prev, nfields);

        # output the number if missing components
        print_totals("MISSING",
                     "number of missing " comptype "s",
                     "N/A", missingcounts_prev, nfields);

        # output the totals of failed components
        print_totals("header",
                     "number of " comptype "s with any failures",
                     "all failures", logfails_prev, nfields);

        # output the totals of all components
        print_totals("header",
                     "total number of " comptype "s exercised",
                     "total exercised", logcounts_prev, nfields);

        print "        </tfoot>"

        close(totalsfile)
        system("rm -f " totalsfile)
    }
    else {
        # increment counts in the totals file for each component

        # set the Output Record Separator to space
        ORS=" "

        for (i = 1; i <= nfields; ++i)
            print logcounts [i] + logcounts_prev [i] > totalsfile

        print "\n" >> totalsfile

        for (i = 1; i <= nfields; ++i)
            print logfails [i] + logfails_prev [i] >> totalsfile

        print "\n" >> totalsfile

        for (i = 1; i <= nfields; ++i)
            print assertcounts [i] + assertcounts_prev [i] >> totalsfile

        print "\n" >> totalsfile

        for (i = 1; i <= nfields; ++i)
            print buildfails [i] + buildfails_prev [i] >> totalsfile

        print "\n" >> totalsfile

        for (i = 1; i <= nfields; ++i)
            print diffcounts [i] + diffcounts_prev [i] >> totalsfile

        print "\n" >> totalsfile

        for (i = 1; i <= nfields; ++i)
            print exitcounts [i] + exitcounts_prev [i] >> totalsfile

        print "\n" >> totalsfile

        for (i = 1; i <= nfields; ++i)
            print signalcounts [i] + signalcounts_prev [i] >> totalsfile

        print "\n" >> totalsfile

        for (i = 1; i <= nfields; ++i)
            print sighupcounts [i] + sighupcounts_prev [i] >> totalsfile

        print "\n" >> totalsfile

        for (i = 1; i <= nfields; ++i)
            print missingcounts [i] + missingcounts_prev [i] >> totalsfile

        print "\n" >> totalsfile

        close(totalsfile)

        # append a newline to the file
        # ORS="\n"
        # print "" >> totalsfile
        # close(totalsfile)
    }
}

function component_summary () {
    print columns;
}

END {
    if (comptype != "") {
        if (found_in_file < fileno) {

            ++missingcounts [fileno]

            # the component wasn't found in the last file processed
            columns = columns "\n          <td class=\"missing\">N/A</td>"
        }

        build_summary()
    }
    else {
        component_summary()
    }
}
