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
#            [svnpath=$svnpath] [verbose=0|1] [comp=$comp]
#            [totalsfile=$totalsfile] [expectfile=$expectfile]
#            logs...
#
# VARIABLES:
#   n            component number
#   name         component name
#   dispname     component display name
#   verbose      verbose mode
#   comp         component kind (example, locale, or test)
#   totalsfile   name of totals file
#   expectfile   name of file with expected results
#
########################################################################


BEGIN {
    # the text of all columns, output after all logs have been processed
    columns = ""

    # total number of times the current component has been found to fail
    # in all logs
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
}


# action to keep track of the current file number
1 == FNR {
    if (0 < fileno && found_in_file < fileno) {
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
    ++logcounts [fileno - 1]

    # the second field is always the exit status of the component
    status = $2

    found_in_file = fileno

    if (status == 0) {

        # successful exit status

        if (comp == "example" || $5 == 0) {

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
                ++logfails [fileno - 1]
                class = "RUNWARN"
                value = "<div title=\"" warnings " warnings\">(" warnings \
                        ")</div>"
            }
        }
        else {

            # component is not an example or, when it is, had failures

            ++compfails
            ++logfails [fileno - 1]

            class = "ASSERT"
            value = "<div title=\"" $5 " failed assertions out of " $4 \
                    "\">(" $5 ")</div>"
        }

        if (comp == "test") {
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
        ++logfails [fileno - 1]

        class = "EXIT"
        value = "<div title=\"exit status of " status "\">" status \
                "</div>"
    }
    else if (status == "FORMAT" || status == "NOUT" || status == "OUTPUT") {
        ++compfails

        class = status
        value = status == "FORMAT" ? "FMAT" : status
    }
    else if (status == "DIFF" || status == "COMP" || status == "LINK") {
        ++compfails
        ++logfails [fileno - 1]

        class = status
        value = status
    }
    else {
        ++compfails
        ++logfails [fileno - 1]

        class = "SIGNAL"
        value = status
    }

    columns = columns "\n          <td class=\"" class "\">" value "</td>"
}


function build_summary ()
{
    if (verbose || compfails) {
        print "        <tr>"
        print "          <td class=\"rowno\">" n "</td>"
        print "          <td class=\"name\"><a name=\"" name "\"></a>"
        print "            " dispname
        print "          </td>"

        if (comp == "test") {
            print "          <td class=\"number\">" max_asserts "</td>"
        }

        print columns;
        print "      </tr>"
    }

    if (compfails) {
        # increment counts in the totals file

        getline < totalsfile
        close(totalsfile)

        # set the Output Record Separator to space
        ORS=" "

        for (i = 0; i < fileno; ++i) {
            print logcounts [i] + $(i + 1) > totalsfile
        }

        for (i = 0; i < fileno; ++i) {
            print logfails [i] + $(i + fileno + 1) > totalsfile
        }

        # append a newline to the file
        ORS="\n"
        print "" >> totalsfile
        close(totalsfile)
    }

    if (name == "@") {

        # the special (bogus) name indicates a request to format
        # totals from the totals file

        # read the totals from the totalsfile
        getline < totalsfile

        nfields=NF

        # output the totals of failed components first
        print "        <tr>"
        print "          <td class=\"header\"></td>"
        print "          <td class=\"header\">"
        print "            <div title=\"number of failed " comp "s\">failed</div>"
        print "          </td>"

        if (comp == "test") {
            print "          <td class=\"header\"></td>"
        }

        for (i = nfields / 2; i < nfields; ++i) {
            print "          <td class=\"total\">" $(i + 1) "</td>"
        }

        print "        </tr>"

        # output the totals of all components next
        print "        <tr>"
        print "          <td class=\"header\"></td>"
        print "          <td class=\"header\">"
        print "            <div title=\"total number of " comp "s\">total</div>"
        print "          </td>"

        if (comp == "test") {
            print "          <td class=\"header\"></td>"
        }

        for (i = 0; i < nfields / 2; ++i) {
            print "          <td class=\"total\">" $(i + 1) "</td>"
        }

        print "        </tr>"

        system("rm -f " totalsfile)
    }
}

function component_summary () {
    print columns;
}

END {
    if (comp != "") {
        if (found_in_file < fileno) {

            # the component wasn't found in the last file processed
            columns = columns "\n          <td class=\"missing\">N/A</td>"
        }

        build_summary()
    }
    else {
        component_summary()
    }
}
