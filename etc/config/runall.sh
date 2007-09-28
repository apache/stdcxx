#!/bin/sh

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
# Copyright 1999-2007 Rogue Wave Software, Inc.
#
##############################################################################
#
# NAME
#     runall.sh, run - run programs and analyze output
#
# SYNOPSIS
#     runall.sh [option(s)...] [file(s)...]
#
# DESCRIPTION
#    Takes a list  of files as arguments. The files  are assumed to be
#    executable; if a  file isn't executable, it is  assumed to have a
#    .o suffix, in  which case it is considered as  an object file and
#    its existence is  taken as success. If the  file isn't executable
#    and doesn't have a .o suffix, it is considered a failure.
#
#    Runs  all executables  named on  the command  line giving  each a
#    fixed amount  of time to complete. If  the program fails  to exit
#    within  the  alotted   amount  of  time,  it  is   killed  via  a
#    signal. Collects  and reports statistics on  each executable with
#    the -p option tries to post  a result file into a revison control
#    system (Perforce).
#
# OPTIONS
#    -a <age>
#            Do not  run programs,  just collect output  unless output
#            file is older than `age' minutes.
#
#    -c or --color
#            Produce colorized output (default). Requires xterm.
#
#    -d <text>
#            Use   `text'  as   a   description   when   submitting  a
#            report. If `text' is  an existing file, use  the contents
#            of the file as the description.  Meaningful only with -p.
#
#    -l <fname> or --log <fname>
#            Append the file  named `fname'  to the end  of the result
#            file before posting. Meaningful only with -p.
#
#    --label <name>
#            Label files with the  label `name', or generate own label
#            name if  "--label -".  The generated label  will have the
#            following   format:   stdlib-<compiler>-<os>-<timestamp>.
#            Meaningful only with -p.
#
#    -m or --mono or --nocolor
#            Produce monochrome output (no escape sequences).  Default
#            for all terminals other than xterm.
#
#    -o <fname>
#            Redirect  process output  to the  named file.  The option
#            affects only the  next executable to be run,  it is reset
#            for all others.
#
#    -p <postdir>
#            Run  tests and examples  and post  results in  a revision
#            control system (Perforce by default).
#
#    --ph    Run plumhall LVS tests
#
#    --rate <percent>
#            Minimum pass rate (in percent) to post.  If the pass rate
#            is  lower,  send email  to  $P4USER  notifying that  post
#            failed. Meaningful only with -p.
#
#    -r <fname>
#           Compare  results against `fname' and report differences as
#           regressions.
#
#    -s <signo>
#           Set the  signal to  be sent to  the hung  process. SIGKILL
#           (signo 9) is default.
#
#    -t <sec>
#           Set process  timeout to  `sec' seconds. Negative  value of
#           `sec' allows an infinite timeout.
#
#    -T <ts0,ts1,ts2>
#           Specifies the timestamps  corresponding to the local times
#           at which the following events started:  configuration, lib
#           build,  and  tests  (and examples) build,  in  that order.
#           Meaningful only with -p.
#
#    -X <text>
#           Specifies additional  command line options to be passed to
#           the executable being run.
#
#    -x <dir>
#           Run  subsequent filenames as examples  and  diff   output  
#           against  files  in  the respective subdirectories of `dir' 
#           (manual/ or tutorial/), if they exist. 
#
##############################################################################


##############################################################################
# GLOBAL VARIABLES
##############################################################################

TIMEOUT=10               # seconds by default
SIGNO=9                  # signal to be sent by default (SIGKILL by default)
REGRESS="."              # regression file (cwd by default, ignored)
LINES=24                 # default number of continuous lines
MANDIR="-"               # directory containing manual (none by default)
MIN_PASS_RATE=70         # minimum acceptable pass rate
POSTFILE="."             # posted results file name
POSTDESC="test results"  # description used when posting
LABEL="-"                # generate a label when posting
OUTFILE="."              # default output file name
AGEMAX=0                 # if non-zero will not run tests, just looks at
                         # test results not older than AGEMAX minutes
XOPTS="-D ."             # additional options to be passed to executable
BUILDLOG=/dev/null       # log to append to results when posting
TIMESTAMPS="-"           # times when lib and tests builds started

case "`uname`" in
    CYGWIN*)
      DIFF_STRIP_CR="--strip-trailing-cr";; # strip trailing CR on CygWin
esac


##############################################################################
# FUNCTION DEFINITIONS
##############################################################################

setcolors ()
{
    COLORS=$1

    if [ $COLORS -ne 0 ] ; then
        ESC0="[30;30;0m"          # escape sequence turning off any formatting

        CLR_BLACK="[30;30;1m"
        CLR_RED="[30;31;1m"
        CLR_GREEN="[30;32;1m"
        CLR_BLUE="[30;34;1m"
        CLR_YELLOW="[30;33;1m"
        CLR_MAGENTA="[30;35;1m"
        CLR_CYAN="[30;36;1m"
        CLR_GRAY="[30;37;1m"

        CLR_ERROR="[41;33;1m"     # serious error (core dump, etc)
        CLR_SUCCESS="[42;37;1m"   # important improvement (e.g., regress < 0)
        CLR_WARN="[41;33;7;1m"    # potential problem (decrease in assertins)
    else
        unset ESC0
        unset CLR_BLACK
        unset CLR_RED
        unset CLR_GREEN
        unset CLR_BLUE
        unset CLR_YELLOW
        unset CLR_CYAN
        unset CLR_GRAY
        unset CLR_ERROR
        unset CLR_SUCCESS
        unset CLR_WARN
    fi
}

# post results in Perforce
post_results ()
{
    p4 opened 2>&1 | grep "File(s) not opened on this client."
    if [ $? -ne 0 ] ; then
        echo "Warning: files opened on $P4CLIENT."
        # exit 1
    fi

    # create a label containing all files in our client view
    if [ "$LABEL" ] ; then
        [ "$LABEL" = "-" ] && LABEL="stdlib-$POSTFILE-`date +%m.%d.%y-%H:%M`"
        ( echo Label: $LABEL ;
          echo View: ;
          p4 client -o | sed -n "s:\(//.*\) //.*:\1:p" ) | p4 label -i
    fi

    # see if we can check the file out
    P4PATH=`p4 edit $POSTDIR/$POSTFILE 2>&1`
    echo $P4PATH | grep " - file(s) not on client." > /dev/null 2>&1

    # add the file if it is not in p4
    [ $? -eq 0 ] && P4PATH=`p4 add $POSTDIR/$POSTFILE`

    [ ! -x "$POSTDIR" ] && echo "$POSTDIR doesn't exist, can't post"

    # copy postfile (and a log, if any) where it can be checked in from
    cat $POSTFILE $BUILDLOG > $POSTDIR/$POSTFILE

    # remove junk p4 appends to the end of file names
    P4PATH=`echo $P4PATH | sed "s/#[1-9][0-9]* - .*$//"`

    echo "executing: p4 submit -i $P4PATH"

    # submit into Perforce
    cat << EOF | p4 submit -i $P4PATH
Change: new
Description:
    `cat /tmp/summary-$$`
Files: $P4PATH
EOF

    # clean up
    rm /tmp/summary-$$
}

# send mail with postfile (containing summary info) to p4 user
send_mail ()
{
    # check for mailx to use -s subject
    if type mailx > /dev/null 2>&1 ; then
        mailcmd=mailx -s "could not post $POSTFILE"
    elif [ `uname` = Linux ] ; then
        # mail on Linux understands -s
        mailcmd=mail -s "could not post $POSTFILE"
    else
        mailcmd=mail
    fi
    email=`p4 user -o | sed -n "s/^Email:.\(.*\)/\1/p"`
    cat $POSTFILE | $mailcmd $email
}


# format a line on output
#   $1: STATUS, $2: ASSRTS, $3 HIGH, $4 DELTA, $5 FAILED, $6 LOW,
#   $7: REGRESS, $8: PERCNT
format ()
{
    if [ $4 -ne 0 ] ; then
        ESC2="$CLR_BLACK"
        ESC4="$ESC2"
    fi

    # colorize regression field (numeric $1 okay, non-numeric considered error)
    if [ $7 -gt 0 ] || [ `expr $1 + 0 >/dev/null 2>&1; echo $?` -eq 2 ] ; then
        ESC7="$CLR_ERROR"
    elif [ $7 -lt 0 ] ; then
        ESC7="$CLR_SUCCESS"
    fi

    printf "$ESC1" >&2 ; printf " %6.6s" $1 ; printf "$ESC0" >&2

    # skip negative value (indicates that values don't make sense)
    if [ $2 -ge 0 ] ; then
        printf "$ESC2" >&2 ; printf " %6d" $2 ; printf "$ESC0" >&2
        printf "$ESC3" >&2 ; printf " %6d" $3 ; printf "$ESC0" >&2
        printf "$ESC4" >&2 ; printf " %6d" $4 ; printf "$ESC0" >&2
        printf "$ESC5" >&2 ; printf " %6d" $5 ; printf "$ESC0" >&2
        printf "$ESC6" >&2 ; printf " %6d" $6 ; printf "$ESC0" >&2
        printf "$ESC7" >&2 ; printf " %6d" $7 ; printf "$ESC0" >&2
        printf "$ESC8" >&2 ; printf " %5d%%" $8 ; printf "$ESC0" >&2
        printf "$ESC0" >&2
    fi

    echo
}

# get signal name from signal number (first argument)
signame ()
{
    n=`expr \( \( $1 - 1 \) % 4 + 1 \) \* 2`
    n=`kill -l | grep ".$1)" | head -1 | awk "{ print $"$n" }" | sed "s/SIG//"`

    [ "$n" = "" ] && n="SIG$1"
    echo $n
}

# compmute the elapsed time between adjacent arguments, each of which is
# expected to be in date +%T format, result is sent to stdout in +%T format
elapsed_time ()
{
    first=$1
    t0=`echo $1 | awk 'BEGIN { FS=":" } { print $1 * 3600 + $2 * 60 + $3 }'`
    shift
    for t in $* ; do
        t1=`echo $t | awk 'BEGIN { FS=":" } { print $1 * 3600 + $2 * 60 + $3 }'`
        [ $t1 -lt $t0 ] && t1=`expr $t1 + 24 \* 60 \* 60`
        hrs=`expr \( $t1 - $t0 \) / 3600`
        if [ $hrs -gt 0 ] ; then
            printf "%d:%02d:%02d " $hrs                     \
                   `expr \( \( $t1 - $t0 \) % 3600 \) / 60` \
                   `expr \( $t1 - $t0 \) % 60`
        else
            printf "%d:%02d " `expr \( \( $t1 - $t0 \) % 3600 \) / 60` \
                              `expr \( $t1 - $t0 \) % 60`
        fi
        t0=$t1
    done

    # compute the difference between the first and the last args
    [ $# -gt 2 ] && elapsed_time $first $t
}


fmat_diff ()
{
    # search file for the first occurrence of a number followed by
    # the string in $2; keep number and exit immediately to save time
    d=`sed -n "/$2/{s/[^0-9]*\([1-9][0-9]*\).*$2.*/\1/p;q
                   }" $REGRESS`

    if [ "$d" != "" ] ; then
        d=`expr $1 - $d`
        # output only if difference is non-zero
        [ $d -ne 0 ] && printf "(%+d) " $d
    fi
    unset d
}

##############################################################################
# MAIN LOOP
##############################################################################

# ensure consistent behavior of POSIX utilities such as grep and sed
LANG=POSIX

# show help and exit when invoked with no arguments
if [ $# -eq 0 ] ; then
    # print out top of this file (between the ###-lines)
    # sed expression split up to satisfy non-GNU sed
    sed -n "/^###/!D; {: print
                         n; /^###/q; s/^#//p; t print
                      }" $0
    exit 0
fi

# reset the terminal settings on exit 
trap 'printf "$ESC0" >&2 && exit' 0 2 3 15

SIGNAME=`signame $SIGNO`

HEADER="NAME               STATUS ASSRTS   HIGH  DELTA FAILED    LOW RGRESS PERCNT"

# write out our command line - uncomment for debugging
# echo $0 $*

tst_count=0   # number of tests tried
tst_pass=0    # number of successfully run tests (perhaps with assertions)
tst_ncomp=0   # number of tests that failed to compile
tst_nlink=0   # number of tests that failed to link
tst_fail=0    # number of tests run to completion but with failures
tst_core=0    # number of tests that dumped core
tst_kill=0    # number of tests that were forcefully killed
tst_rgrs=0    # number of tests with regressions

asrt_count=0  # total number of assertions
asrt_dlta=0   # increase in assertions from last run
asrt_pass=0   # total number of passed assertions
asrt_fail=0   # total number of failed assertions
asrt_rgrs=0   # increase in failed assertions from last run
asrt_hi=0     # the total highest number of failed assertions ever
asrt_lo=0     # the total lowest number of failed assertions ever

t0=`date +%T` # timestamp

# get $LINES and $COLUMNS set if possible
type resize > /dev/null 2>&1 && eval `resize` 2>&1 > /dev/null  
[ -z "$LINES" ] && LINES=40
[ -z "$COLUMNS" ] && COLUMNS=80

if [ "$TERM" = "xterm" ] ;then
    setcolors 1   # turn on colorized output
else
    setcolors 0   # disable colorization
fi

for i do
    # process command line options

    # avoid the unportable [ ! "$foo" ]
    if [ "" = "$AGEMAX" ] ; then
        AGEMAX=$i
        continue
    elif [ "$i" = "-a" ] ; then
        unset AGEMAX
        continue
    elif [ "" = "$POSTDESC" ] ; then
        POSTDESC="$i"
        continue
    elif [ "$i" = "-d" ] ; then
        unset POSTDESC
        continue
    elif [ "$i" = "-m" ] || [ "$i" = "--mono" ] || [ "$i" = "--nocolor" ]; then
        setcolors 0
        continue
    elif [ "$i" = "-c" ] || [ "$i" = "--color" ] ; then
        setcolors 1
        continue
    elif [ "" = "$BUILDLOG" ] ; then
        BUILDLOG=$i
        continue
    elif [ "$i" = "-l" ] || [ "$i" = "--log" ]; then
        unset BUILDLOG
        continue
    elif [ "" = "$LABEL" ] ; then
        LABEL=$i
        continue
    elif [ "$i" = "--label" ] ; then
        unset LABEL
        continue
    elif [ "" = "$REGRESS" ] ; then
        REGRESS=$i
        continue
    elif [ "$i" = "-r" ] ; then
        unset REGRESS
        continue
    elif [ "" = "$MIN_PASS_RATE" ] ; then
        MIN_PASS_RATE=$i
        continue
    elif [ "$i" = "--rate" ] ; then
        unset MIN_PASS_RATE
        continue
    elif [ "" = "$TIMEOUT" ] ; then
        TIMEOUT=$i
        continue
    elif [ "$i" = "-t" ] ; then
        unset TIMEOUT
        continue;
    elif [ "" = "$TIMESTAMPS" ] ; then
        TIMESTAMPS=$i
        continue
    elif [ "$i" = "-T" ] ; then
        unset TIMESTAMPS
        continue;
    elif [ "" = "$SIGNO" ] ; then
        SIGNO=$i
        SIGNAME=`signame $SIGNO`
        continue
    elif [ "$i" = "-s" ] ; then
        unset SIGNO
        continue
    elif [ "" = "$MANDIR" ] ; then
        MANDIR=$i/manual
        TUTDIR=$i/tutorial
        continue
    elif [ "" = "$XOPTS" ] ; then
        XOPTS="$i"
        continue
    elif [ "$i" = "-X" ] ; then
        unset XOPTS
        continue
    elif [ "$i" = "-x" ] ; then
        unset MANDIR
        continue
    elif [ "$i" = "--ph" ] ; then
        PLUMHALL=1              
        MANDIR='-'              # indicates we're running tests
        continue
    elif [ "" = "$POSTFILE" ] ; then
        POSTDIR=`dirname $i`
        POSTFILE=`basename $i`
        continue
    elif [ "$i" = "-p" ] ; then
        unset POSTFILE
        continue
    elif [ "" = "$OUTFILE" ] ; then
        OUTFILE=$i
        continue
    elif [ "$i" = "-o" ] ; then
        unset OUTFILE
        continue
    fi;

    n_asrt=0      # number of assertions in this test
    d_asrt=0      # increase in assertions from last run
    p_asrt=0      # passed assertions in this test
    f_asrt=0      # failed assertions in this test
    r_asrt=0      # number of regressions
    h_asrt=0      # the highest number of assertions ever tested
    l_asrt=0      # the least number of assertions ever tested
    pcnt_asrt=0   # percentage of passed assertions in this test
    no_asserts=0  # non-zero if output doesn't contain assertion tags
    exit_status=0 # successful exit status by default (will be overridden)

    ESC1="$ESC0" ; ESC2="$ESC0" ; ESC3="$ESC0" ; ESC4="$ESC0"
    ESC5="$ESC0" ; ESC6="$ESC0" ; ESC7="$ESC0" ; ESC8="$ESC0"

    if [ `expr $tst_count % $LINES` -eq 0 ] ; then
        # redirect test number to stderr in order not to mess up diff
        echo ;           printf "$CLR_BLACK" >&2 ; printf "%s" "--- " >&2
        echo "$HEADER" ; printf "$ESC0" >&2
    fi

    tst_count=`expr $tst_count + 1`
    printf "%3d " $tst_count 1>&2

    # strip trailing `.sh' from filename if present
    file=`basename $i | sed "s/\.sh//"`
    printf "%-18.18s" $file

    # if an executable file exists _or_ if AGEMAX is not 0
    if [ -x "$i" ] || [ $AGEMAX -ne 0 ] ; then
        # get the name of input file if any (/dev/null by default)
        if [ -f $MANDIR/in/$file.in ] ; then
            input=$MANDIR/in/$file.in
        elif [ -f $TUTDIR/in/$file.in ] ; then
            input=$TUTDIR/in/$file.in
        else
            input=/dev/null
        fi

        if [ "$OUTFILE" = "." ] ; then
            output="$i.out"
        else
            output=$OUTFILE
        fi

        if [ $AGEMAX -eq 0 ] ; then

            # save cursor position in case the shell (bash 2 on SunOS)
            # reports when a process gets a signal despite redirection
            [ "$COLORS" != "0" ] && printf "7" >&2

            # set alarm in process (in case we can't kill it if it hangs)
            alrm=`expr $TIMEOUT + 3`

            # run test in the background, redirect I/O to a file
            # stderr redirected to /dev/null to prevent compilation
            # warnings in "script" mode from causing false errors
            $i $XOPTS -O $output -L $alrm -M "`uname -a`" \
               > $output < $input 2>/dev/null &

            # get the background process' pid
            testpid=$!

            # start a subshell to kill hung process
            if [ $TIMEOUT -ge 0 ] ; then
                (t=$TIMEOUT
                 while [ $t -ne 0 ]; do
                     # exit right away if process doesn't exist
                     ps -p $testpid > /dev/null 2>&1 || exit 0 

                     # show countdown if colorized
                     [ "$COLORS" != "0" ] \
                         && printf "$CLR_WARN %6d$ESC0[7D" $t >&2
                     t=`expr $t - 1`
                     sleep 1
                 done
                 kill -s $SIGNO $testpid > /dev/null 2>&1
                )&
            fi

            # we'll test for this later
            kill_status=`expr 128 + $SIGNO`

            # wait for test to exit (on its own or forcefully)
            wait $testpid
            exit_status=$?

            # restore cursor position
            [ "$COLORS" != "0" ] && printf "8" >&2
        else
            # set kill status to be different than exit_status
            kill_status=1
            found=`find ./$output -mmin -$AGEMAX`
            if [ "" = "$found" ] ; then
                exit_status=-1
            else
                # indicate that output file was stale (older than AGEMAX)
                exit_status=0
            fi
        fi
	
	if [ $exit_status -lt 126 ] && [ "$PLUMHALL" ]; then
	    exit_status=0   #plum hall exit status is number of failed tests
	fi

        if [ $exit_status -eq 0 ]; then
            if [ "$MANDIR" = "-" ] ; then   # running tests

                # make sure output file exists
                if [ -f "$output" ] ; then
                    tst_pass=`expr $tst_pass + 1`

                    # grep for the number of total and failed assertions
		    if [ "$PLUMHALL" ]; then
			f_asrt=`grep "errors detected" $output \
                            | sed "s/\*\*\*\*\* \([0-9]*\) .*/\1/"`
			n_success=`grep "successful test case" $output \
                            | sed "s/\*\*\*\*\* \([0-9]*\) .*/\1/"`
			if [ "$f_asrt" ] && [ "$n_success" ]; then
			    n_asrt=`expr $n_success + $f_asrt`
			fi
		    else
			n_asrt=`sed "s/[^ -~]/ /g" $output \
                            | grep "## Assertions" \
                            | tail -1 | sed "s/[^0-9]//g"`
			f_asrt=`sed "s/[^ -~]/ /g" $output \
                            | grep "## FailedAssertions" \
                            | tail -1 | sed "s/[^0-9]//g"`
		    fi

                    if [ "" = "$n_asrt" ] ; then
                        n_asrt=0
                        no_assert=1
                        # no regression line found, something's fishy
                        exit_status="FORMAT" ; ESC1="$CLR_WARN"
                    fi

                    [ "" = "$f_asrt" ] && f_asrt=0

                    # increment the number of tests not passing at 100%
                    [ $f_asrt -gt 0 ] && tst_fail=`expr $tst_fail + 1`
                
                    # compute percentage of passed assertions
                    p_asrt=`expr $n_asrt - $f_asrt`
                    asrt_count=`expr $asrt_count + $n_asrt`
                    asrt_pass=`expr $asrt_pass + $p_asrt`
                    asrt_fail=`expr $asrt_fail + $f_asrt`

                    [ $f_asrt -gt 0 ] && ESC5="$CLR_RED"

                    # prevent division by zero
                    [ $n_asrt -ne 0 ] \
                        && pcnt_asrt=`expr 100 \* $p_asrt / $n_asrt`
                else   # output file not found
                    exit_status="OUTPUT" ; ESC1="$CLR_ERROR"
                fi
            else   # running examples (expect output)
                no_asserts=1

                # get the name of file against which to diff the output
                if [ -f $MANDIR/out/$file.out ] ; then
                    diff_file=$MANDIR/out/$file.out
                elif [ -f $TUTDIR/out/$file.out ] ; then
                    diff_file=$TUTDIR/out/$file.out
                else
                    unset diff_file
                fi

                if [ "$diff_file" != "" ] ; then
                    diff $DIFF_STRIP_CR $diff_file $output >/dev/null 2>&1
                    if [ $? -eq 0 ] ; then
                        tst_pass=`expr $tst_pass + 1`
                    else
                        exit_status="DIFF" ; ESC1="$CLR_ERROR"
                    fi
                else
                    exit_status="OUTPUT" ; ESC1="$CLR_WARN"
                    tst_pass=`expr $tst_pass + 1`
                fi
                ESC2="$ESC0" ; ESC3="$ESC2" ; ESC4="$ESC2" ; ESC5="$ESC2"
                ESC6="$ESC2" ; ESC7="$ESC2" ; ESC8="$ESC2"
            fi
        elif [ $exit_status -eq 126 ] ; then
            exit_status="EXIST"    # can't find executable
            ESC1="$CLR_WARN"
        elif [ $exit_status -eq 127 ] ; then
            exit_status="EXEC"     # can't execute (check LD_LIBRARY_PATH)
            ESC1="$CLR_WARN"
        elif [ $exit_status -eq $kill_status ] ; then
            exit_status=`signame $SIGNO`   # forcefully killed
            ESC1="$CLR_ERROR"
            tst_kill=`expr $tst_kill + 1`
        elif [ $exit_status -ge 128 ] ; then
            n=`expr $exit_status - 128`
            exit_status=`signame $n`
            ESC1="$CLR_ERROR"
            tst_core=`expr $tst_core + 1`
        elif [ $exit_status -eq -1 ] ; then
            exit_status="STALE"
            ESC1="$CLR_WARN"
        else
            ESC1="$CLR_WARN"
        fi
    elif [ -f $i.o ] ; then
        exit_status="LINK"   # .o exists but no executable, failed to link
        ESC1="$CLR_ERROR"
        tst_nlink=`expr $tst_nlink + 1`
    else
        # file isn't executable, see if it's an object file...
        suffix=`echo $i | sed "s/.*\(\..*\)/\1/"`
        if [ "$suffix" = ".o" ] && [ -f "$i" ] ; then
            no_asserts=1
            tst_pass=`expr $tst_pass + 1`
        else
            exit_status="COMP"   # no .o, failed to compile
            ESC1="$CLR_ERROR"
            tst_ncomp=`expr $tst_ncomp + 1`
        fi
    fi

    if [ -f "$REGRESS" ] ; then
        # find line for same executable in a previous report file
        line=`grep "^$file[ .] " $REGRESS | head -n 1`

        # compute difference in tried assertions
        d_asrt=`echo $line | awk '{ print $3 }'`
        d_asrt=`expr $n_asrt - 0$d_asrt`

        # only consider if current exit status is 0 (success)
        if [ "$exit_status" = "0" ] ; then

            # regressions make sense only if previous exit status was 0
            if  [ "`echo $line | awk '{ print $2 }'`" = "0" ] ; then
                r_asrt=`echo $line | awk '{ print $6 }'`
                r_asrt=`expr $f_asrt - 0$r_asrt`
            fi

            # see if the lowest number of failed assertions improved
            l_asrt=`echo $line | awk '{ print $7 }'`
            [ "" = "$l_asrt" ] && l_asrt=0
            [ $l_asrt -gt $f_asrt ] && l_asrt=$f_asrt && ESC6="$CLR_SUCCESS"
        else
            # carry over previous low assertions on failure
            l_asrt=`echo $line | awk '{ print $7 }'`
            [ "" = "$l_asrt" ] && l_asrt=0

            # count the test as regressing if the last exit status was 0
            old_status=`echo $line | awk '{ print $2 }'`
            [ "$old_status" = "0" ] && tst_rgrs=`expr $tst_rgrs + 1`
        fi

        # adjust highest number of assertions achieved
        h_asrt=`echo $line | awk '{ print $4 }'`
        [ "" = "$h_asrt" ] && h_asrt=0
        [ $h_asrt -lt $n_asrt ] && h_asrt=$n_asrt && ESC3="$CLR_BLACK"

        # increment cumulative totals
        asrt_dlta=`expr $asrt_dlta + $d_asrt`
        asrt_rgrs=`expr $asrt_rgrs + $r_asrt`

        [ $r_asrt -gt 0 ] && tst_rgrs=`expr $tst_rgrs + 1`
    else
        l_asrt=$f_asrt
        h_asrt=$n_asrt
    fi

    # get rid of leading zeros (to prevent octal conversion by printf)
    l_asrt=`expr $l_asrt + 0`

    asrt_hi=`expr $h_asrt + $asrt_hi`
    asrt_lo=`expr $l_asrt + $asrt_lo`

    # set number of assertions negative if assertion tags not found
    [ $no_asserts -ne 0 ] && n_asrt=-1

    # careful with leading zeros (interpreted as octal with %d)
    format $exit_status $n_asrt $h_asrt $d_asrt \
           $f_asrt $l_asrt $r_asrt $pcnt_asrt

    # reset name of output file
    OUTFILE="."
done

# compute pass rate as percentage of tests with perfect score
# out of the total number of tests tried
pass_rate=0
[ $tst_count -gt 0 ] && pass_rate=`expr \( 100 \* \( $tst_pass - $tst_fail \) \) / $tst_count`

if [ $tst_count -gt 1 ] ; then

    [ $asrt_count -ne 0 ] && pcnt_asrt=`expr 100 \* $asrt_pass / $asrt_count`

    # show cumulative results
    printf "\n--- " 1>&2
    printf "%-18.18s" ""

    ESC1="" ; ESC2="" ; ESC3="" ; ESC4=""
    ESC5="" ; ESC6="" ; ESC7="" ; ESC8=""

    # careful with leading zeros (interpreted as octal with %d)
    format $tst_pass $asrt_count $asrt_hi $asrt_dlta \
           $asrt_fail $asrt_lo $asrt_rgrs $pcnt_asrt

    echo ; printf "${CLR_BLACK}    " >&2
    echo "                     FAIL   COMP   LINK ASSERT   CORE   KILL RGRESS";
    printf "$ESC0      " >&2
    printf "                 %6d %6d %6d %6d %6d %6d %6d\n" \
           `expr $tst_count - $tst_pass` \
           $tst_ncomp $tst_nlink $tst_fail $tst_core $tst_kill $tst_rgrs
    echo

    if [ -f "$REGRESS" ] ; then

        # compute and format summary regressions wrt the previous report
        # each var will be empty when there's no change, else "(+/-N) "
        d_count=`fmat_diff $tst_count "programs"`
        d_ncomp=`fmat_diff $tst_ncomp "failed to compile"`
        d_nlink=`fmat_diff $tst_nlink "failed to link"`
        d_fail=`fmat_diff $tst_fail "failed at least one assertion"`
        d_core=`fmat_diff $tst_core "exited with a signal"`
        d_kill=`fmat_diff $tst_kill "killed with signal"`
        d_rate=`fmat_diff $pass_rate "pass rate"`

        # special treatment (no variable $tst_nzero exists)
        d_nzero=`expr $tst_count - $tst_pass`
        d_nzero=`fmat_diff $d_nzero "exited with non-zero status"`

        if [ -f "$POSTFILE" ] ; then
            # modify REGRESS to be the p4 path name and rev # of what
            # regressions are being reported against (used in description)
            tmp=`p4 have $REGRESS | sed "s/ - .*//" 2>/dev/null`
            [ $? -eq 0 ] && REGRESS="$tmp"
        fi
        regress_line="$asrt_rgrs regressions (baseline was $REGRESS)"
    else
        regress_line="no regressions (this is baseline)"
    fi

    # present summary only for more files than fits on screen or when posting
    if [ $tst_count -ge $LINES -o -f "$POSTFILE" ] ; then

        # compute and format timestamps
        if [ "$TIMESTAMPS" != "-" ] ; then
            # expect: "h0:m0:s0,h1:m1:s1,h2:m2:s2"
            TIMESTAMPS="$TIMESTAMPS $t0 `date +%T`"
            # replace commas with spaces
            TIMESTAMPS=`echo $TIMESTAMPS | sed "s/,/ /g"`
            TIMESTAMPS=`elapsed_time $TIMESTAMPS | awk '{ print \
                "cfg: " $1 " lib: " $2 " tst: " $3 " run: " $4 " all: " $5 }'`
        fi

        # create a file containing summary info about this run
        cat << EOF | tee /tmp/summary-$$

        Generated by $LOGNAME@`uname -n` running `uname -sr`

        `if [ -f "$POSTDESC" ]; then cat "$POSTDESC" ; else echo $POSTDESC; fi`

        `[ "$TIMESTAMPS" != "-" ] && echo "Times: $TIMESTAMPS"`

        Summary: out of $tst_count ${d_count}programs
            `expr $tst_count - $tst_pass` ${d_nzero}exited with non-zero status
            $tst_ncomp ${d_ncomp}failed to compile
            $tst_nlink ${d_nlink}failed to link
            $tst_fail ${d_fail}failed at least one assertion
            $tst_core ${d_core}exited with a signal
            $tst_kill ${d_kill}killed with signal $SIGNO
            $regress_line
            $pass_rate% ${d_rate}pass rate

EOF
    fi

else
    echo
fi


# post results in revision control system
if [ -f "$POSTFILE" ] ; then
    # make sure minimum pass rate is achieved
    if [ $pass_rate -lt $MIN_PASS_RATE ] ; then
        # send email to p4 user if not passing at a desired rate
        send_mail
        echo "pass rate < $MIN_PASS_RATE%, can't post"
    elif [ "$P4CLIENT" != "" ] ; then
        # post results in Perforce
        post_results
    else
        send_mail
        echo "P4CLIENT not defined, can't post"
    fi

elif [ "$POSTFILE" != "." ] ; then
    echo "$POSTFILE not found, can't post"
    exit 1
fi
