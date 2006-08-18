#!/bin/sh
#
# $Id$
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
#     Checks locale utilities:
#     1. Locale utilities sanity: location, output, etc;
#     2. Functionality:
#         - (re)generation  of databases;
#    
# OPTIONS:
#    - "-d" debug;
#    - "-s" tests location, otput;
#    - "-f" tests functionality; is followed by:
#        - "-i <nlsdir>"
#        - "-l <locale name>"
#    
##############################################################################

locale="./locale"
localedef="./localedef"

##############################################################################
#	Function definitions - checking sanity
##############################################################################

check_locale_help ()
{
    printf "Checking \"locale --help\" output..."  >$dbgout
    loc_h_out=`${locale} --help`
    loc_h_xout="NAME SYNOPSIS DESCRIPTION"
    loc_h_regexp="locale - get locale-specific information"
    for it in $loc_h_xout; do
        assertions=`expr $assertions + 1`
        if test -z "`echo $loc_h_out | grep $it`"; then
            echo " incorrect."  >$dbgout
            echo "ERROR: \"locale --help\" gives wrong output ($it)." >$dbgout 
            echo >$dbgout
            failedassertions=`expr $failedassertions + 1`
        fi;
    done
    echo " correct." >$dbgout
}

check_locale_all()
{
    printf "Checking \"locale -a\" output..." >$dbgout
    loc_a_out=`${locale} -a`
    loc_a_regexp="[a-z]\{2\}_[A-Z]\{2\}"
    for it in $loc_a_out; do					
        assertions=`expr $assertions + 1`
	test "$it" = "C" && continue;
	if test -z "`echo $it | grep -e \"$loc_a_regexp\"`"; then
            echo "incorrect." >$dbgout
            echo "    Warning: Locale name \"$it\" not matching pattern." \
                >$dbgout
            failedassertions=`expr $failedassertions + 1`
	fi;							
    done
    echo "check completed." >$dbgout
}

check_locale_m()
{
    printf "Checking \"locale -m\" output..." >$dbgout
    loc_m_out=`${locale} -m`
    for it in $loc_m_out; do
        assertions=`expr $assertions + 1`
	if test -z "`echo $it | grep .cm`" ; then
            echo " incorrect." >$dbgout
            echo "ERROR: \"locale -m\" failed."  >$dbgout
            echo >$dbgout
            failedassertions=`expr $failedassertions + 1`
        fi;
    done;						
    echo " correct." >$dbgout
}

check_locale_k()
{
    printf "Checking \"locale -k LC_ALL\" output..." >$dbgout
    loc_k_out=`${locale} -k LC_ALL`
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
            [ $any_failed -eq 0 ] && echo " incorrect." >$dbgout
            echo "ERROR: \"locale -k\" gives wrong output ($it)." >$dbgout
            echo >$dbgout
            failedassertions=`expr $failedassertions + 1`
            any_failed=1
        fi;
    done

    [ $any_failed -eq 0 ] && echo " correct." >$dbgout
}

check_localedef_help()
{
    printf "Checking \"localedef --help\" output..." >$dbgout
    locdef_h_out=`${localedef} --help`
    locdef_h_xout="NAME SYNOPSIS DESCRIPTION"
    locdef_h_regexp="localedef - generate a locale environment"
    for it in $locdef_h_xout; do
        assertions=`expr $assertions + 1`;
        if test -z "`echo $locdef_h_out | grep $it`"; then
            echo " incorrect." >$dbgout
            echo "ERROR: \"localedef --help\" gives wrong output ($it)." >$dbgout
            echo >$dbgout
            failedassertions=`expr $failedassertions + 1`
        fi;
    done
    echo " correct." >$dbgout
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

    if [ "$1" = "" ] ; then
        echo $err character maps file not specified. >>$out
        exit 1;
    fi

    if [ "$2" = "" ] ; then
        echo $err source input file not specified. >>$out
        exit 1;
    fi

    if [ "$3" = "" ] ; then
        echo $err output locale name not specified. >>$out
        exit 1;
    fi

    assertions=`expr $assertions + 1`

    # Generating the database
    echo "${localedef} -w -c -f $1 -i $2 $3 2>"$dbgout >$dbgout
    ${localedef} -w -c -f $1 -i $2 $3 2>$dbgout

    retcode=$?
    if [ $retcode -ne 0 ] ; then 
        echo "Error - localedef returned code: $retcode" >>$out
        failedassertions=`expr $failedassertions + 1`
    fi
}

#
# Dumps one locale database
#
dump_locale ()
{
    err="Cannot dump locale database - "

    # 1 - current locale dump file

    if [ "$1" = "" ] ; then
        echo $err - no output file specified. >> $out
        exit 1
    fi

    assertions=`expr $assertions + 1`

    # Dumping locale database
    echo "${locale} -ck -h LC_ALL > $1" > $dbgout
    ${locale} -ck -h LC_ALL > $1
    
    retcode=$?
    if [ $retcode -ne 0 ] ; then 
        echo "Error - locale returned code: $retcode" >>$out
        failedassertions=`expr $failedassertions + 1`
    fi
}

#
# Test one locale
#
test_locale()
{
    err="Cannot test locale - "

    # $1 - nlsdir
    # $2 - test directory
    # $3 - name of the locale database
    
    if [ "$1" = "" ] ; then 
        echo $err - nls directory not specified. >> $out
        exit 1
    fi

    if [ "$2" = "" ] ; then 
        echo $err - test directory not specified. >> $out
        exit 1
    fi

    if [ "$3" = "" ] ; then 
        echo $err - locale database name not specified. >> $out
        exit 1
    fi

    # get locale's name and encoding
    locale_src=`echo $3 | \
                sed -n "s:\([^\.]*\)\.\([^\.\@]*\)\(.*\):\1\3:p;s:\@$::g"`;
    locale_encoding=`echo $3 | \
                     sed -n "s:\([^\.]*\)\.\([^\.\@]*\)\(.*\):\2:p"`;

    ## generate the first locale database
    generate_locale $1/charmaps/$locale_encoding     \
                    $1/src/$locale_src               \
                    $2/$3

    # set necessary environment variables
    echo "LC_ALL="$3 >$dbgout
    LC_ALL=$3
    echo "export LC_ALL" >$dbgout
    export LC_ALL
    echo "LANG="$3 >$dbgout
    LANG=$3
    echo "export LANG" >$dbgout
    export LANG

    ## adjust the locale root
    echo "RWSTD_LOCALE_ROOT="$2 >$dbgout
    RWSTD_LOCALE_ROOT=$2
    echo "export RWSTD_LOCALE_ROOT" >$dbgout
    export RWSTD_LOCALE_ROOT

    # dump the locale database content to temporary location
    dump_locale $2/out.1
    echo > $dbgout

    # remove stage one database
    echo "rm -rf $2/$3"
    rm -rf $2/$3
    
    # generate stage two database
    generate_locale $1/charmaps/$locale_encoding                  \
                    $2/out.1                                      \
                    $2/$3

    # dump stage two database to file
    dump_locale $2/out.2

    # remove stage two database
    echo "rm -rf $2/$3"
    rm -rf $2/$3

    # generate stage three database
    generate_locale $1/charmaps/$locale_encoding            \
                    $2/out.2                                \
                    $2/$3

    # and finally dump it to file
    dump_locale $2/out.3

    assertions=`expr $assertions + 1`

    # compare
    file_diff=`diff $2/out.2 $2/out.3` >/dev/null
    if [ $? -ne 0 ] ; then
        echo "ERROR: $2/out.2 $2/out.3 differ." > $dbgout
        failedassertions=`expr $failedassertions + 1`
    fi
}

#
# Cleanup handler
#

signal_cleanup ()
{
    echo "Cleaning up " $tmpdir > $dbgout
    rm -rf $tmpdir
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

## output stream
out="/dev/stdout"
dbgout="/dev/null"

## Temporary (working) directory
[ -z "$TMP" ] && TMP="/tmp";
tmpdir=$TMP/locale.$$

## Get the options from the command line
while getopts ":sfdb:i:l:O:L:M:C:D:" opt_name; do
    case $opt_name in 
        d) dbgout="/dev/tty";;
        s) chk_sanity=yes;;
        f) chk_func=yes;;
        b) bindir=$OPTARG;;
        i) nlsdir=$OPTARG;;
        l) locale_db=$OPTARG;;
        O) out=$OPTARG;;
        L);; ## ignored
        M);; ## ignored
        C);; ## ignored
        D);; ## ignored
        *) echo "Unimplemented option : -$opt_name" >&2;
           exit 1;;
     esac;
done

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

    ## checking locale functionality
    echo "RWSTD_SRC_ROOT="$nlsdir >$dbgout
    RWSTD_SRC_ROOT=$nlsdir
    echo "export RWSTD_SRC_ROOT" >$dbgout
    export RWSTD_SRC_ROOT
    echo "RWSTD_LOCALE_ROOT="$tmpdir >$dbgout
    RWSTD_LOCALE_ROOT=$tmpdir
    echo "export RWSTD_LOCALE_ROOT" >$dbgout
    export RWSTD_LOCALE_ROOT

    # set our cleanup on exit trap
    trap signal_cleanup EXIT

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
echo "# +-----------------------+--------+--------+--------+" >> $out
echo "# | DIAGNOSTIC            | ACTIVE |  TOTAL |INACTIVE|" >> $out
echo "# +-----------------------+--------+--------+--------+" >> $out
printf "# | (S7) ASSERTION        | %6d | %6d | %5d%% |\n"  $failedassertions $assertions $pcnt >> $out
echo "# +-----------------------+--------+--------+--------+" >> $out
echo >> $out
echo "## Assertions = "$assertions >> $out
echo "## FailedAssertions = "$failedassertions >> $out
echo >> $out
