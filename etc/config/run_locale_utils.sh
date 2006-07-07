#! /bin/sh
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

##############################################################################
#	Function definitions - checking sanity
##############################################################################
check_locale_location()
{
    # 1 = location of util directory
    $echo_n "Checking location of locale utilities..." >$dbgout
    if test "`which locale`" != "$1/locale" ; then
        echo " incorrect."  >$dbgout
        echo "ERROR: Locale utility is not present." >$dbgout
        echo  >$dbgout
        exit 1
    fi
    if test "`which localedef`" != "$1/localedef"; then 	
        echo "ERROR: Localedef utility is not present." >$dbgout
        echo >$dbgout
        exit 1 
    fi
    echo " correct."  >$dbgout
}

check_locale_help ()
{
    $echo_n "Checking \"locale --help\" output..."  >$dbgout
    loc_h_out=`locale --help`
    loc_h_xout="NAME SYNOPSIS DESCRIPTION"
    loc_h_regexp="locale - get locale-specific information"
    for it in $loc_h_xout; do
        let assertions+=1
        if test -z "`echo $loc_h_out | grep $it`"; then
            echo " incorrect."  >$dbgout
            echo "ERROR: \"locale --help\" gives wrong output ($it)." >$dbgout 
            echo >$dbgout
            let failedassertions+=1
        fi;
    done
    echo " correct." >$dbgout
}

check_locale_all()
{
    $echo_n "Checking \"locale -a\" output..." >$dbgout
    loc_a_out=`locale -a`
    loc_a_regexp="[a-z]\{2\}_[A-Z]\{2\}"
    for it in $loc_a_out; do					
        let assertions+=1
	test "$it" = "C" && continue;
	if test -z "`echo $it | grep -e \"$loc_a_regexp\"`"; then
            echo "incorrect." >$dbgout
            echo "    Warning: Locale name \"$it\" not matching pattern." \
                >$dbgout
            let failedassertions+=1
	fi;							
    done
    echo "check completed." >$dbgout
}

check_locale_m()
{
    $echo_n "Checking \"locale -m\" output..." >$dbgout
    loc_m_out=`locale -m`
    for it in $loc_m_out; do
        let assertions+=1
	if test -z "`echo $it | grep .cm`" ; then
            echo " incorrect." >$dbgout
            echo "ERROR: \"locale -m\" failed."  >$dbgout
            echo >$dbgout
            let failedassertions+=1
        fi;
    done;						
    echo " correct." >$dbgout
}

check_locale_k()
{
    $echo_n "Checking \"locale -k LC_ALL\" output..." >$dbgout
    loc_k_out=`locale -k LC_ALL`
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
    for it in $loc_k_xout; do
        let assertions+=1;
        if test -z "`echo $loc_k_out | grep $it`"; then
            echo " incorrect." >$dbgout
            echo "ERROR: \"locale -k\" gives wrong output ($it)." >$dbgout
            echo >$dbgout
            let failedassertions+=1
        fi;
    done
    echo " (seems) correct." >$dbgout
}

check_localedef_help()
{
    $echo_n "Checking \"localedef --help\" output..." >$dbgout
    locdef_h_out=`localedef --help`
    locdef_h_xout="NAME SYNOPSIS DESCRIPTION"
    locdef_h_regexp="localedef - generate a locale environment"
    for it in $locdef_h_xout; do
        let assertions+=1;
        if test -z "`echo $locdef_h_out | grep $it`"; then
            echo " incorrect." >$dbgout
            echo "ERROR: \"localedef --help\" gives wrong output ($it)." >$dbgout
            echo >$dbgout
            let failedassertions+=1
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

    let assertions+=1

    # Generating the database
    echo Generating locale database $3... >$dbgout
    echo "localedef -c -f $1 -i $2 $3 &>/dev/null" >$dbgout
    localedef -c -f $1 -i $2 $3 &>/dev/null

    retcode=$?
    if [ $retcode -ne 0 ] ; then 
        echo "Error - localedef returned code: $retcode" >>$out
        let failedassertions+=1
    fi

    echo Done generating locale database. >$dbgout
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

    let assertions+=1

    # Dumping locale database
    echo Dump current locale to $1... > $dbgout
    echo "locale -ck -h LC_ALL > $1" > $dbgout
    locale -ck -h LC_ALL > $1
    
    retcode=$?
    if [ $retcode -ne 0 ] ; then 
        echo "Error - locale returned code: $retcode" >>$out
        let failedassertions+=1
    fi

    echo Dumping done. > $dbgout
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
    LC_ALL=$3
    LANG=$3

    ## adjust the locale root
    RWSTD_LOCALE_ROOT=$2
    export RWSTD_LOCALE_ROOT

    # dump the locale database content to temporary location
    echo Dump stage one database to file... > $dbgout
    dump_locale $2/out.1
    echo > $dbgout

    # remove stage one database
    echo Remove stage one database... > $dbgout
    rm -rf $2/$3
    echo > $dbgout
    
    # generate stage two database
    echo Generate stage two database... > $dbgout
    generate_locale $1/charmaps/$locale_encoding                  \
                    $2/out.1                                      \
                    $2/$3
    echo > $dbgout

    # dump stage two database to file
    echo Dump stage two database to file... > $dbgout
    dump_locale $2/out.2
    echo > $dbgout

    # remove stage two database
    echo Remove stage two database... > $dbgout
    rm -rf $2/$3
    echo > $dbgout
    
    # generate stage three database
    echo Generate stage three database... > $dbgout
    generate_locale $1/charmaps/$locale_encoding            \
                    $2/out.2                                \
                    $2/$3
    echo > $dbgout

    # and finally dump it to file
    echo Dump stage three database to file... > $dbgout
    dump_locale $2/out.3
    echo > $dbgout

    # compare
    file_diff=`diff $2/out.2 $2/out.3`
    if [ "$file_diff" = "" ] ; then
        echo "Database $3 - encoding $locale_encoding - OK." > $dbgout
    else
        echo "Database $3 - encoding $locale_encoding - FAIL." > $dbgout
    fi

    # and remove database
    [ -d $2/$3 ] && rm -rf $2/$3 

    # remove dump files
    [ -f $2/out.1 ] && rm $2/out.1
    [ -f $2/out.2 ] && rm $2/out.2
    [ -f $2/out.3 ] && rm $2/out.3

    # and finally remove the tmp directory
    [ -d $2 ] && rm -rf $2
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
locale=""

## output stream
out="/dev/null"
dbgout="/dev/null"

echo_n="echo -n"

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
        *) echo "Unimplemented option : -$opt_name" >> $out;
           exit 1;;
     esac;
done

## Actual test
if [ "$chk_sanity" = "yes" ]; then

    ## checking locale sanity
    check_locale_location $bindir
    check_locale_help
    check_locale_all
    check_locale_m
    check_locale_k
    check_localedef_help

elif [ "$chk_func" = "yes" ]; then

    ## set the temp dir
    [ -z "$TMP" ] && TMP="/tmp";
    [ -d $TMP/locale ] || mkdir $TMP/locale;

    ## checking locale functionality
    RWSTD_SRC_ROOT=$nlsdir
    export RWSTD_SRC_ROOT
    RWSTD_LOCALE_ROOT=$TMP/locale
    export RWSTD_LOCALE_ROOT

    # test only one locale
    test_locale $nlsdir $TMP/locale $locale_db;
else 
    ## Invocation is wrong
    echo "Wrong invocation." >> $out
    echo "Usage : " >> $out
    echo "script -s -b <bindir> ## checking sanity" >> $out
    echo "script -f -i <nlsdir> -l <locale name> ## check functionality" >> $out
    exit 2
fi


echo >> $out
echo "## Assertions = "$assertions >> $out
echo "## FailedAssertions = "$failedassertions >> $out
echo >> $out
