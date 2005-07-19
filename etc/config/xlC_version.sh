#!/bin/sh
#
# $Id: //stdlib/dev/etc/stdlib/config/xlC_version.sh#1 $
#
# prints VisualAge version

if [ $# -gt 0 ]; then
    CXX=$1
else
    CXX=xlC
fi

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

rm -f $tmpsrc $tmpobj 2>/dev/null

if [ "$version" = "" ]; then
    version=`$CXX -qversion \
             | awk '{ if ($4 == "Version") print $5; else print $7; }'`
fi

echo $version | sed "s/[^0-9]*\([0-9].*\)/\1/"
