#!/bin/sh
#
# $Id$
#
# prints VisualAge version

if [ $# -gt 0 ]; then
    CXX=$1
else
    CXX=xlC
fi


xlcout=/tmp/$CXX_version.$$.out

$CXX -qversion >$xlcout 2>&1

if [ $? -eq 0 ]; then
    # VisualAge 7.0, 2005/8/31 or later
    version=`grep "Version:" $xlcout \
             | awk '{ if ($1 == "Version:") print $2; \
                      else if ($4 == "Version") print $5; \
                      else print $7; }'`
    rm $xlcout

    # collapse consecutive leading zeros
    echo $version | awk 'BEGIN { FS="." }{ print (0 + $1) "." (0 + $2) "." (0 + $3) "." (0 + $4) }'
    exit
fi

# check to see if this is the base 7.0
grep "V7" $xlcout > /dev/null 2>&1
v7_or_6=$?

if [ $v7_or_6 -ne 0 ]; then
    # if it's not 7.0 see if it's 6.0
    grep "version 6.0" $xlcout > /dev/null 2>&1
    v7_or_6=$?
fi

if [ $v7_or_6 -eq 0 ]; then

    # VisualAge 6.0 or 7.0 (unpatched)
    tmpsrc=/tmp/$CXX_version.$$.cpp
    tmpobj=/tmp/$CXX_version.$$.o

    echo "#pragma comment(compiler)" > $tmpsrc
    echo "int main() { }" >> $tmpsrc

    $CXX -c $tmpsrc -o $tmpobj
    status=$?

    if [ $status -eq 0 ]; then
        version=`strings $tmpobj \
                 | sed -n 's/.*Version \([^ ]*\) *.*/\1/p' \
                 | tail -1`
    fi

    rm -f $tmpsrc $tmpobj $xlcout 2>/dev/null

    if [ "$version" = "" ]; then
        version=`$CXX -qversion \
                 | awk '{ if ($4 == "Version") print $5; else print $7; }'`
    fi

    echo $version | sed "s/[^0-9]*\([0-9].*\)/\1/"
    exit
fi

# VisualAge 5.0 (and prior?)
echo >> $xlcout
sed "s/.*version \(.*\)/\1/" $xlcout
rm $xlcout
