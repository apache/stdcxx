#!/bin/sh
#
# $Id$
#
##############################################################################
#
# Licensed to the Apache Software  Foundation (ASF) under one or more
# contributor  license agreements.  See  the NOTICE  file distributed
# with  this  work  for  additional information  regarding  copyright
# ownership.   The ASF  licenses this  file to  you under  the Apache
# License, Version  2.0 (the  "License"); you may  not use  this file
# except in  compliance with the License.   You may obtain  a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the  License is distributed on an  "AS IS" BASIS,
# WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
# implied.   See  the License  for  the  specific language  governing
# permissions and limitations under the License.
#
# Copyright 1999-2008 Rogue Wave Software, Inc.
#
##############################################################################
#
# NAME
#     run_locale_utils.sh
#
# SYNOPSIS
#     run_locale_utils.sh
#
# DESCRIPTION
#     Exercise the locale utilities:
#     1. Locale utilities sanity: location, output, etc;
#     2. Functionality:
#         - (re)generation of databases;
#    
# OPTIONS:
#    - "-d" debug
#    - "-n" no cleanup
#    - "-s" tests location, output
#    - "-f" tests functionality; is followed by:
#        - "-i <nlsdir>"
#        - "-l <locale name>"
#    
##############################################################################


OSNAME=`uname`

if [ "$OSNAME" = "OSF1" -a "$BIN_SH" != "xpg4" ]; then
    # make getopts work on Tru64 by setting the BIN_SH variable
    # and re-executing self with the same command line arguments
    BIN_SH=xpg4
    export BIN_SH
    exec $0 $*
elif [ "$OSNAME" = "AIX" -a -z "$BASH_VERSION" ]; then
    # use an alternate shell to work around a bug (?) in the AIX system
    # shell that prevents it from trapping SIGHUP (and invoking the signal
    # handler)
    if [ -x /bin/bash ]; then
        exec /bin/bash $0 $*
    fi
fi


locale="./locale"
localedef="./localedef"

output ()
{
    if test -z "$outfile"; then
        echo "$*" 2>/dev/null
    else
        echo "$*" >> $outfile
    fi;
}


debug_output ()
{
    if test ! -z "$dbgout"; then
        echo "$*" >> $dbgout 2>/dev/null
    fi;
}

##############################################################################
#	Function definitions - checking sanity
##############################################################################

check_locale_help ()
{
    debug_output "Checking \"locale --help\" output..."

    loc_h_out=`${locale} --help 2>/dev/null`
    loc_h_xout="NAME SYNOPSIS DESCRIPTION"
    loc_h_regexp="locale - get locale-specific information"
    for it in $loc_h_xout; do
        assertions=`expr $assertions + 1`
        if test -z "`echo $loc_h_out | grep $it`"; then
            debug_output " incorrect."
            debug_output "ERROR: \"locale --help\" gives wrong output ($it)."
            debug_output ""
            failedassertions=`expr $failedassertions + 1`
        fi;
    done

    debug_output "...ok"
}


check_locale_all()
{
    debug_output "Checking \"locale -a\" output..."

    loc_a_out=`${locale} -a 2>/dev/null`
    loc_a_regexp="[a-z]\{2\}_[A-Z]\{2\}"
    for it in $loc_a_out; do					
        assertions=`expr $assertions + 1`
	test "$it" = "C" && continue;
	if test -z "`echo $it | grep -e \"$loc_a_regexp\"`"; then
            debug_output "incorrect."
            debug_output "Warning: Locale name \"$it\" not matching pattern."
            failedassertions=`expr $failedassertions + 1`
	fi;							
    done

    debug_output "...ok"
}


check_locale_m()
{
    debug_output "Checking \"locale -m\" output..."

    any_failed=0

    loc_m_out=`${locale} -m 2>/dev/null`
    any_failed=$?

    for it in $loc_m_out; do
        assertions=`expr $assertions + 1`
	if test -z "`echo $it | grep .cm`" ; then
            any_failed=1
            debug_output " incorrect."
            debug_output "ERROR: \"locale -m\" failed."
            debug_output
            failedassertions=`expr $failedassertions + 1`
        fi;
    done;						

    if [ $any_failed -eq 0 ]; then
        debug_output "...ok"
    fi
}


check_locale_k()
{
    debug_output "Checking \"locale -k LC_ALL\" output..."

    loc_k_out=`${locale} -k LC_ALL 2>/dev/null`
    loc_k_xout="upper lower space print                                     \
                cntrl alpha digit punct                                     \
                graph xdigit toupper tolower                                \
                abday day abmon  mon                                        \
                am_pm d_t_fmt d_fmt t_fmt t_fmt_ampm                        \
                int_curr_symbol currency_symbol mon_decimal_point           \
                mon_thousands_sep mon_grouping positive_sign negative_sign  \
                int_frac_digits frac_digits p_cs_precedes p_sep_by_space    \
                n_cs_precedes n_sep_by_space p_sign_posn n_sign_posn        \
                decimal_point thousands_sep grouping"

    any_failed=0
    for it in $loc_k_xout; do
        assertions=`expr $assertions + 1`;
        if test -z "`echo $loc_k_out | grep $it`"; then
            # output text only for the first failure
            [ $any_failed -eq 0 ] && debug_output " incorrect."
            debug_output "ERROR: \"locale -k\" gives wrong output ($it)."
            debug_output
            failedassertions=`expr $failedassertions + 1`
            any_failed=1
        fi;
    done

    if [ $any_failed -eq 0 ]; then
        debug_output "...ok"
    fi
}

check_localedef_help()
{
    debug_output "Checking \"localedef --help\" output..."

    locdef_h_out=`${localedef} --help 2>/dev/null`
    locdef_h_xout="NAME SYNOPSIS DESCRIPTION"
    locdef_h_regexp="localedef - generate a locale environment"
    for it in $locdef_h_xout; do
        assertions=`expr $assertions + 1`;
        if test -z "`echo $locdef_h_out | grep $it`"; then
            debug_output " incorrect."
            debug_output "ERROR: \"localedef --help\" gives wrong output ($it)."
            debug_output
            failedassertions=`expr $failedassertions + 1`
        fi;
    done

    debug_output "...ok"
}

##############################################################################
#	Function definitions - checking functionality
##############################################################################


#
# Generates one specified locale
#
generate_locale()
{
    err="Cannot generate locale database - "

    # 1 - character map file used in generating the locale database
    # 2 - source/locale definition file
    # 3 - locale database name

    if [ "$1" = "" ]; then
        output "$err character maps file not specified."
        exit 1;
    fi

    if [ "$2" = "" ]; then
        output "$err source input file not specified."
        exit 1;
    fi

    if [ "$3" = "" ]; then
        output "$err output locale name not specified."
        exit 1;
    fi

    assertions=`expr $assertions + 1`

    # Generating the database
    debug_output "${localedef} -w -c -f $1 -i $2 $3 >${dbgout:-/dev/null} 2>&1"
    ${localedef} -w -c -f $1 -i $2 $3 >${dbgout:-/dev/null} 2>&1

    retcode=$?
    if [ $retcode -ne 0 ]; then
        # exit with the same status as the tool
        exit $retcode
    fi
}


#
#
#
dump_charmap()
{
    err="Cannot create characater set description file - "

    # $1: LC_ALL value
    # $2: output file name

    assertions=`expr $assertions + 1`

    # dumping charmap
    output "LC_ALL=$1 ${locale} --charmap -l >$2 2>${dbgout:-/dev/null}"
    LC_ALL=$1 ${locale} --charmap -l > $2 2>${dbgout:-/dev/null}

    retcode=$?
    if [ $retcode -ne 0 ]; then 
        # exit with the same status as the tool
        exit $retcode
    fi
}

#
# Dumps one locale database
#
dump_locale ()
{
    err="Cannot dump locale database - "

    # $1: LC_ALL value
    # $2: current locale dump file

    if [ "$2" = "" ]; then
        output "$err - no output file specified."
        exit 1
    fi

    assertions=`expr $assertions + 1`

    # dumping locale database
    output "LC_ALL=$1 ${locale} -ck -h -l LC_ALL >$2 2>${dbgout:-/dev/null}"
    LC_ALL=$1 ${locale} -ck -h -l LC_ALL > $2 2>${dbgout:-/dev/null}
    
    retcode=$?
    if [ $retcode -ne 0 ]; then 
        # exit with the same status as the tool
        exit $retcode
    fi
}

#
# Test one locale
#
test_locale()
{
    err="Cannot test locale - "

    # $1 - nls subdirectory of the source directory tree
    # $2 - the test (sandbox) directory
    # $3 - name of the locale database
    
    if [ "$1" = "" ]; then 
        output "$err - nls directory not specified."
        exit 1
    fi

    if [ "$2" = "" ]; then 
        output "$err - temporary directory not specified."
        exit 1
    fi

    if [ "$3" = "" ]; then 
        output "$err - locale database name not specified."
        exit 1
    fi

    nlsdir=$1
    tmpdir=$2
    fname=$3

    # get the name of the locale and the character map
    source=`echo $fname | sed "s:\([^.]*\)\.\([^@]*\)\(.*\):\1\3:;y:@:.:"`;
    charmap=`echo $fname | sed "s:\([^.]*\)\.\([^@]*\)\(.*\):\2:"`;

    src_path=$nlsdir/src/$source
    cm_path=$nlsdir/charmaps/$charmap

    stage_1=$tmpdir/stage.1
    stage_2=$tmpdir/stage.2
    stage_3=$tmpdir/stage.3

    # point locale at the original source directory
    debug_output "RWSTD_SRC_ROOT="$nlsdir
    RWSTD_SRC_ROOT=$nlsdir
    debug_output "export RWSTD_SRC_ROOT"
    export RWSTD_SRC_ROOT

    # create a directory for stage 1 charmap source files
    debug_output "mkdir -p $stage_1/charmaps"
    mkdir -p $stage_1/charmaps

    assertions=`expr $assertions + 1`

    ## generate stage 1 locale database from the orignal sources
    generate_locale $cm_path $src_path $stage_1/$fname
    if [ $? -ne 0 ]; then
        failedassertions=`expr $failedassertions + 1`
        return;
    fi

    # set necessary environment variables
    # debug_output "LC_ALL=$fname"
    # LC_ALL=$fname
    # debug_output "export LC_ALL"
    # export LC_ALL
    # debug_output "LANG=$fname"
    # LANG=$fname
    # debug_output "export LANG"
    # export LANG

    assertions=`expr $assertions + 1`

    # dump the charmap and the locale data from the database
    # to a pair of charmap and locale source files
    dump_charmap $stage_1/$fname $stage_1/charmaps/$charmap
    dump_locale $stage_1/$fname $stage_1/$source.src

    if [ -z "$no_clean" ]; then
        # remove stage 1 locale to free up disk space but keep
        # the text dumps for later processing
        debug_output "rm $stage_1/$fname"
        rm $stage_1/$fname
    fi

    # create a directory for stage 2 charmap source files
    debug_output "mkdir -p $stage_2/charmaps"
    mkdir -p $stage_2/charmaps

    assertions=`expr $assertions + 1`

    # generate stage 2 locale database from the charmap and locale
    # source files produced by locale from the stage 1 database
    generate_locale $stage_1/charmaps/$charmap \
                    $stage_1/$source.src \
                    $stage_2/$fname

    if [ $? -ne 0 ]; then
        failedassertions=`expr $failedassertions + 1`
        return;
    fi

    # point locale at the stage 1 directory
    debug_output "RWSTD_SRC_ROOT=$stage_1"
    RWSTD_SRC_ROOT=$stage_1
    debug_output "export RWSTD_SRC_ROOT"
    export RWSTD_SRC_ROOT

    assertions=`expr $assertions + 1`

    # dump the charmap and the locale data from the database
    # to a pair of charmap and locale source files
    dump_charmap $stage_2/$fname $stage_2/charmaps/$charmap
    dump_locale $stage_2/$fname $stage_2/$source.src

    if [ -z "$no_clean" ]; then
        # remove stage 2 locale to free up disk space but keep
        # the text dumps for later processing
        debug_output "rm $stage_2/$fname"
        rm $stage_2/$fname
    fi

    assertions=`expr $assertions + 1`

    # create a directory for stage 2 charmap source files
    debug_output "mkdir -p $stage_3/charmaps"
    mkdir -p $stage_3/charmaps

    # generate stage 3 locale database from the charmap and locale
    # source files produced by locale from the stage 2 database
    generate_locale $stage_2/charmaps/$charmap \
                    $stage_2/$source.src \
                    $stage_3/$fname

    if [ $? -ne 0 ]; then
        failedassertions=`expr $failedassertions + 1`
        return;
    fi

    # point locale at the stage 2 directory
    debug_output "RWSTD_SRC_ROOT=$stage_2"
    RWSTD_SRC_ROOT=$stage_2
    debug_output "export RWSTD_SRC_ROOT"
    export RWSTD_SRC_ROOT

    assertions=`expr $assertions + 1`

    # dump the charmap and the locale data from the database
    # to a pair of charmap and locale source files
    dump_charmap $stage_3/$fname $stage_3/charmaps/$charmap
    dump_locale $stage_3/$fname $stage_3/$source.src

    if [ -z "$no_clean" ]; then
        # remove stage 3 locale to free up disk space but keep
        # the text dumps for later processing
        debug_output "rm $stage_3/$fname"
        rm $stage_3/$fname
     fi

    assertions=`expr $assertions + 1`

    # verify that stage 1 and stage 2 charmaps are the same
    debug_output "diff $stage_1/charmaps/$charmap " \
                      "$stage_2/charmaps/$charmap >/dev/null"
    diff $stage_1/charmaps/$charmap $stage_2/charmaps/$charmap >/dev/null
    if [ $? -ne 0 ]; then
        debug_output "## AssertionFailed: $stage_1/charmaps/$charmap " \
                     "and $stage_2/charmaps/$charmap differ."
        failedassertions=`expr $failedassertions + 1`
    fi

    # verify that stage 2 and stage 3 charmaps are the same
    debug_output "diff $stage_2/charmaps/$charmap " \
                      "$stage_3/charmaps/$charmap >/dev/null"
    diff $stage_2/charmaps/$charmap $stage_3/charmaps/$charmap >/dev/null
    if [ $? -ne 0 ]; then
        debug_output "## AssertionFailed: $stage_2/charmaps/$charmap " \
                     "and $stage_3/charmaps/$charmap differ."
        failedassertions=`expr $failedassertions + 1`
    fi

    # verify that stage 2 and stage 3 locale sources are the same
    debug_output "diff $stage_2/$source.src " \
                      "$stage_3/$source.src >/dev/null"
    diff $stage_2/$source.src $stage_3/$source.src >/dev/null
    if [ $? -ne 0 ]; then
        debug_output "## AssertionFailed: $stage_2/$source.src " \
                     "and $stage_3/$source.src differ."
        failedassertions=`expr $failedassertions + 1`
    fi

    if [ -z "$no_clean" ]; then
        # clean up
        debug_output "rm -rf $stage_1 $stage_2 $stage_3"
        rm -rf $stage_1 $stage_2 $stage_3
    fi
}

#
# cleanup/signal handler
#
cleanup ()
{
    if [ -z "$no_clean" ]; then
        # clean up
        debug_output "rm -rf $tmpdir"
        rm -rf $tmpdir
    fi

    exit
}

##############################################################################
#  Main code
##############################################################################


## assertions
assertions=0
failedassertions=0

## main functions
debug=no
chk_sanity=no
chk_func=no

## directories
bindir=""
nlsdir=""
locdir=""

## Get the options from the command line
while getopts ":nsfdb:i:l:O:L:M:C:D:" opt_name; do
    case $opt_name in 
        d) dbgout="/dev/tty";;
        s) chk_sanity=yes;;
        f) chk_func=yes;;
        b) bindir=$OPTARG;;
        i) nlsdir=$OPTARG;;
        l) locale_db=$OPTARG;;
        n) no_clean=yes;;
        O) outfile=$OPTARG;;
        L);; ## ignored
        M);; ## ignored
        C);; ## ignored
        D);; ## ignored
        *) echo "Unimplemented option : -$opt_name" >&2;
           exit 1;;
     esac;
done

## set temporary (working) directory
if [ -z "$TMPDIR" ]; then
    TMPDIR="/tmp"
    export TMPDIR
fi

tmpdir=$TMPDIR/${locale_db:-unnamed-locale}.$$

## Actual test
if [ "$chk_sanity" = "yes" ]; then

    ## checking locale sanity
    check_locale_help
    check_locale_all
    check_locale_m
    check_locale_k
    check_localedef_help

elif [ "$chk_func" = "yes" ]; then

    ## create the temp dir
    mkdir $tmpdir
    if [ $? -ne 0 ]; then
        echo "$0: Unable to create $tmpdir, aborting" >&2
        exit 1
    fi

    # clean up temporary files on signal or exit
    trap cleanup HUP INT QUIT TERM EXIT

    # test only one locale
    test_locale $nlsdir $tmpdir $locale_db;
else 
    ## Invocation is wrong
    echo "$0: Usage : " >&2
    echo "script -s -b <bindir> ## checking sanity" >&2
    echo "script -f -i <nlsdir> -l <locale name> ## check functionality" >&2
    exit 2
fi

pcnt=`expr 100 \* \( $assertions - $failedassertions \) / $assertions`
output "# +-----------------------+----------+----------+----------+"
output "# | DIAGNOSTIC            |  ACTIVE  |   TOTAL  | INACTIVE |"
output "# +-----------------------+----------+----------+----------+"

if test -z "$outfile"; then
    printf "# | (S7) ASSERTION        | %8d | %8d | %7d%% |\n" \
            $failedassertions $assertions $pcnt
else
    printf "# | (S7) ASSERTION        | %8d | %8d | %7d%% |\n" \
            $failedassertions $assertions $pcnt >> $outfile
fi

output "# +-----------------------+----------+----------+----------+"
output
output "## Warnings = 0"
output "## Assertions = "$assertions
output "## FailedAssertions = "$failedassertions
output
